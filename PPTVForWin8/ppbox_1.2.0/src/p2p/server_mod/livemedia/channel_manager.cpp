//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "pre.h"
#include "channel_manager.h"
#include "channel_server.h"
#include "AsioServiceRunner.h"
#include "SessionManagement/SessionManager.h"
#include <fstream>
#include "StatusFile.h"
#include "channel.h"
#include "ChannelCache.h"

namespace live_media
{
    ChannelManager::ChannelManager(ChannelServer * server) : server_(server), asio_timer_manager_(0)
        , channels_on_timer_(0), total_upload_speed_limit_(0), total_upload_connection_limit_(0), total_keep_connection_limit_(0)
        , total_uploading_peer_count_(0), current_uploading_count_(0), last_report_state_time_(0), state_report_thread_(0)
        , exiting_report_thread_(false)
    {
        this->last_check_channel_priority_time_ = time(0);
    }

    ChannelManager::~ChannelManager()
    {
    }

    const std::vector<IChannelPointer> ChannelManager::GetChannels() const
    {
        std::vector<IChannelPointer> result(channels_.size());
        size_t i = 0;
        for(ChannelsCollection::const_iterator iter = channels_.begin();
            iter != channels_.end();
            ++iter)
        {
            result[i++] = iter->second;
        }

        return result;
    }

    bool ChannelManager::Initalize()
    {
        this->total_upload_speed_limit_ = 0;
        this->total_upload_connection_limit_ = 0;
        this->total_keep_connection_limit_ = 0;

        // 载入所有的 channel
        std::vector<ChannelConfiguration> & channels = this->server_->GetServerConfig().channels;
        for (size_t i = 0 ; i < channels.size() ; ++i) {
            ChannelConfiguration & c = channels[i];
            this->AddChannel(c);
        }
        
        
        // 初始化定时器
        if (!this->StartTimer()) {
            return false;
        }
        
        this->last_check_channel_priority_time_ = time(0);
        
        // 启动提交状态线程
        this->status_file_.reset(new StatusFile());
        status_file_->Start();

        return true;
    }

    bool ChannelManager::StartTimer()
    {
        if(!this->asio_timer_manager_) 
        {
            this->asio_timer_manager_ = 
                new framework::timer::AsioTimerManager(*this->server_->io_service(), boost::posix_time::seconds(1));

            if(!this->asio_timer_manager_)
                return false;

            this->asio_timer_manager_->start();
        }

        // 
        if (!this->channels_on_timer_) 
        {
            this->channels_on_timer_ = 
                new framework::timer::PeriodicTimer(*this->asio_timer_manager_, 1000, boost::bind(&ChannelManager::OnTimer, this));
            if (!this->channels_on_timer_) 
            {
                return false;
            }
            this->channels_on_timer_->start();
        }

        return true;
    }

    void ChannelManager::Uninitalize()
    {
        this->exiting_report_thread_ = true;

        for (ChannelsCollection::iterator i = this->channels_.begin() ; i != this->channels_.end() ; ++i) 
        {
            i->second->Uninitalize();
        }

        if (this->state_report_thread_) 
        {
            state_report_thread_->join();
            delete state_report_thread_;
        }

        this->channels_.clear();
    }

    ChannelPointer ChannelManager::AddChannel(const ChannelConfiguration & c)
    {
        ChannelPointer cha(new Channel(c.guid, server_, server_->GetIoService()));

        if (!cha->Initalize()) 
        {
            LOG4CPLUS_ERROR(Loggers::Channel(), "Failed to add channel "<<c.guid);
            cha.reset();
            return cha;
        }

        cha->SetChannelConfig(c);

        this->total_upload_speed_limit_ += c.max_upload_speed;
        this->total_upload_connection_limit_ += c.max_upload_in_a_while;
        this->total_keep_connection_limit_ += c.max_keep_connections;

        this->channels_[c.guid] = cha;
        LOG4CPLUS_INFO(Loggers::Channel(), "channel added, guid: " << c.guid.to_string());

        return cha;
    }

    bool ChannelManager::RemoveChannel(const channel_id & c_id)
    {
        ChannelsCollection::iterator iter = channels_.find(c_id);

        if (iter == this->channels_.end()) 
        {
            return false;
        }

        ChannelPointer cha = iter->second;
        if (cha)
        {
            cha->Uninitalize();
        }

        LOG4CPLUS_INFO(Loggers::Channel(), "channel removed, guid: " << c_id.to_string());

        this->channels_.erase(iter);
        return true;
    }

    void ChannelManager::RemoveObsoleteChannels()
    {
        time_t now = ::time(0);

        for (channel_iterator it = channels_.begin(); it != channels_.end(); )
        {
            if (it->second->GetLastVisitTime() + server_->GetServerConfig().channel_obsolete_time_in_seconds < now)
            {
                it->second->Uninitalize();
                channels_.erase(it++);
            }
            else
            {
                ++it;
            }
        }
    }

    ChannelPointer ChannelManager::GetChannel(const channel_id & c_id)
    {
        ChannelsCollection::iterator iter = channels_.find(c_id);
        if (iter != channels_.end())
        {
            return iter->second;
        }

        return ChannelPointer();
    }

    void ChannelManager::OnTimer()
    {

        this->server_->OnTimer();

        for(ChannelsCollection::iterator iter = channels_.begin(); iter != channels_.end(); ++iter)
        {
            iter->second->OnTimer();
        }

        GenerateStatusFile();
        //
        this->server_->ReloadServerConfig();

        this->UpdateTrackerRidList();

        if (server_->GetServerConfig().open_channel_on_demand)
            RemoveObsoleteChannels();
    }

    void ChannelManager::UpdateTrackerRidList()
    {
        // 更新 channel_list

        // 检查是否需要更新
        std::set<RID> rids;
        std::vector<IChannelPointer> chns = this->GetChannels();
        for(size_t i = 0; i < chns.size(); i++) {
            // 
            rids.insert(chns[i]->GetChannelId());
        }

        this->server_->tracker_manager_->UpdateTrackerRidList(rids);
    }


    void ChannelManager::GenerateStatusFile()
    {
        static size_t counter(0);
        boost::uint32_t total_block_cache_num = 0;
        boost::uint32_t total_block_data_num = 0;
        boost::uint32_t total_upload_user_count = 0;
        std::map<std::string, ChannelStatus> channels_status;

        if (!channels_.empty())
        {
            for (ChannelsCollection::iterator iter = channels_.begin();
                iter != channels_.end(); iter++)
            {
                ChannelPointer channel = iter->second;
                BlocksCacheMap  blocks = channel->GetCache()->GetBlocks();
                total_block_cache_num += channel->GetCache()->GetBlockCacheCount();
                total_block_data_num += channel->GetCache()->GetBlockDataCount();
                boost::uint32_t begin_time = blocks.empty() ? 0 : blocks.begin()->first;
                boost::uint32_t end_time = blocks.empty() ? 0 : blocks.rbegin()->first;
                boost::uint32_t user_count = this->server_->GetSessionManager()->GetChannelSessionsCount(channel->GetChannelId());
                boost::uint32_t upload_user_count = this->server_->GetSessionManager()->GetChannelUploadSessionCount(channel->GetChannelId());
                total_upload_user_count += upload_user_count;
                boost::uint32_t upload_speed = channel->speed_statistics_->GetCurentCycleSpeedSend();
                ChannelStatus status(channel->GetChannelId().to_string(), begin_time, end_time, upload_speed, user_count, upload_user_count,
                    channel->GetCache()->GetBlockCacheCount(), channel->GetCache()->GetBlockDataCount());
                channels_status[channel->GetChannelId().to_string()] = status;
            }
        }

        GlobalStatus global_status(
            this->server_->GetServerConfig().config_id, 
            this->server_->GetServerConfig().p2p_listen_port,
            this->server_->speed_statistics_->GetCurentCycleSpeedSend(), 
            this->server_->GetSessionManager()->GetCurrentSessionNum(),
            total_upload_user_count,
            total_block_cache_num, 
            total_block_data_num);
        status_file_->UpdateChannelStatus(global_status, channels_status);


        (this->server_->receive_packet_statistics_)->LogStatus();
        (this->server_->send_packet_statistics_)->LogStatus();
        this->server_->announce_request_relative_to_max_hits_statistics_->LogStatus();
        this->server_->subpiece_request_relative_to_max_hits_statistics_->LogStatus();

        if (++counter%10 == 0)
        {
            this->server_->announce_request_relative_to_max_hits_statistics_->ResetCounter();
            this->server_->subpiece_request_relative_to_max_hits_statistics_->ResetCounter();
        }

        status_file_->AddGroupStatus("z_packet_receive",server_->receive_packet_statistics_->GetMap());
        status_file_->AddGroupStatus("z_packet_send", server_->send_packet_statistics_->GetMap());
        status_file_->AddGroupStatus(server_->announce_request_relative_to_max_hits_statistics_->GetName(), server_->announce_request_relative_to_max_hits_statistics_->GetMap());
        status_file_->AddGroupStatus(server_->subpiece_request_relative_to_max_hits_statistics_->GetName(), server_->subpiece_request_relative_to_max_hits_statistics_->GetMap());
    }
}
