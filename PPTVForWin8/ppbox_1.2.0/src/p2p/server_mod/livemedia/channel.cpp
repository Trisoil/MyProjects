//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "pre.h"
#include "channel.h"

#include "channel_manager.h"
#include "channel_server.h"
#include <framework/Framework.h>
#include <framework/string/Url.h>
#include "UploadManager.h"
#include "SessionManagement/SessionManager.h"

namespace live_media
{
    Channel::Channel(const channel_id & channel_identifier, ChannelServer* channel_server, boost::shared_ptr<boost::asio::io_service> io_service)
    : channel_id_(channel_identifier), io_service_(io_service)
    , last_visit_time_(::time(0))
    {
        session_manager_ = channel_server->GetSessionManager();
        channel_cache_.reset(
            new ChannelCache(
                channel_id_, 
                channel_server->GetRemoteCache(), 
                channel_server->io_service(), 
                channel_server->announce_request_relative_to_max_hits_statistics_, 
                channel_server->subpiece_request_relative_to_max_hits_statistics_));
        channel_cache_->Start();
    }

    Channel::~Channel()
    {
        channel_cache_.reset();
    }

    channel_id Channel::GetChannelId() const
    {
        return this->channel_config_.guid;
    }

    void Channel::OnTimer()
    {
        if (upload_manager_)
        {
            upload_manager_->OnTimer();
        }

        if (channel_cache_)
        {
            channel_cache_->OnTimer();
        }

        if (session_manager_)
        {
            LOG4CPLUS_INFO(Loggers::Channel(), "Channel "<<channel_id_<<": session count:"<<session_manager_->GetChannelSessionsCount(channel_id_)<<", session limit:"<<channel_sessions_count_constraint_.GetCurrentLimit());
        }
    }

    bool Channel::Initalize()
    {
        this->speed_statistics_.reset(new SpeedStatistics(io_service_, channel_id_.to_string() + std::string("_speed_statistics")));
       
        this->speed_statistics_->StartTimer();
        LOG4CPLUS_INFO(Loggers::Channel(), "channel: " << this->channel_id_ << "initalize succeeded.");

        this->upload_manager_.reset(new UploadManager(speed_statistics_, shared_from_this()));
        
        return true;
    }

    bool Channel::Uninitalize()
    {
        this->speed_statistics_.reset();

        return true;
    }

    void Channel::SetChannelConfig(const ChannelConfiguration & channel_config)
    {
        this->channel_config_ = channel_config;
        this->channel_sessions_count_constraint_.SetHardLimit(channel_config.max_keep_connections);
        this->upload_manager_->SetMaxUploadSpeed(channel_config.max_upload_speed);
    }

    bool Channel::AcceptsNewSession() const
    {
        size_t current_sessions_count = session_manager_->GetChannelSessionsCount(channel_id_);
        return channel_sessions_count_constraint_.AcceptsNew(current_sessions_count);
    }

    void Channel::OnIncreaseBandwidth()
    {
        LOG4CPLUS_INFO(Loggers::Channel(), "Increasing sessions limit for channel "<<channel_id_);
        size_t current_sessions_count = session_manager_->GetChannelSessionsCount(channel_id_);
        channel_sessions_count_constraint_.IncreaseLimit(current_sessions_count);
    }

    void Channel::OnDecreaseBandwidth()
    {
        LOG4CPLUS_INFO(Loggers::Channel(), "Decreasing sessions limit for channel "<<channel_id_);
        size_t current_sessions_count = session_manager_->GetChannelSessionsCount(channel_id_);
        channel_sessions_count_constraint_.DecreaseLimit(current_sessions_count);
    }
}
