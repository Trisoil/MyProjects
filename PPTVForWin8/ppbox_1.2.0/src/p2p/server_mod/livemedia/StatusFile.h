//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _STATUS_FILE_H_
#define _STATUS_FILE_H_

#include "packet_statistics.h"


namespace live_media
{
    class AsioServiceRunner;

    struct StatusBase
    {
        boost::uint32_t upload_speed_;
        boost::uint32_t user_count_;
        boost::uint32_t upload_user_count_;
        boost::uint32_t block_cache_num_;
        boost::uint32_t block_data_num_;
        
        StatusBase(boost::uint32_t upload_speed, boost::uint32_t user_count, boost::uint32_t upload_user_count, boost::uint32_t block_cache_num, boost::uint32_t block_data_num)
        {
            upload_speed_ = upload_speed;
            user_count_ = user_count;
            upload_user_count_ = upload_user_count;
            block_cache_num_ = block_cache_num;
            block_data_num_ = block_data_num;
        }

        StatusBase()
        {
            upload_speed_ = 0;
            user_count_ = 0;
            upload_user_count_ = 0;
            block_cache_num_ = 0;
            block_data_num_ = 0;
        }
    };

    struct GlobalStatus
        : public StatusBase
    {
        boost::uint32_t listen_port_;
        std::string configuration_id_;

        GlobalStatus(const std::string& configuration_id, boost::uint32_t listen_port, boost::uint32_t upload_speed, boost::uint32_t user_count, boost::uint32_t upload_user_count, boost::uint32_t block_cache_num, boost::uint32_t block_data_num)
            : StatusBase(upload_speed, user_count, upload_user_count, block_cache_num, block_data_num),
            configuration_id_(configuration_id)
        {
            listen_port_ = listen_port;
        }

        GlobalStatus()
        {
            listen_port_ = 0;
        }
    };

    struct ChannelStatus
        : public StatusBase
    {
        std::string channel_id_;
        boost::uint32_t begin_time_;
        boost::uint32_t end_time_;

        
        ChannelStatus(const std::string& channel_id, boost::uint32_t begin_time, boost::uint32_t end_time, 
            boost::uint32_t upload_speed, boost::uint32_t user_count, boost::uint32_t upload_user_count, boost::uint32_t block_cache_num, boost::uint32_t block_data_num)
            : StatusBase(upload_speed, user_count, upload_user_count, block_cache_num, block_data_num),
            channel_id_(channel_id)
        {
            begin_time_ = begin_time;
            end_time_ = end_time;
        }

        ChannelStatus()
        {   
            begin_time_ = 0;
            end_time_ = 0;
        }
    };


    typedef std::map<std::string, boost::uint32_t> GroupStatus;
    class StatusFile
        :public boost::enable_shared_from_this<StatusFile>
    {
    public:
        StatusFile(){}

        void Start();

        void AddGroupStatus(std::string group_name, const std::map<std::string, boost::uint32_t> & status);

        void UpdateChannelStatus(const GlobalStatus & global_status, const std::map<std::string, ChannelStatus> & channels_status);

        const std::map<std::string, ChannelStatus> & GetChannelsMap()
        {
            return channel_map_;
        }

        const std::map<std::string, GroupStatus> & GetGroupsMap()
        {
            return groups_map_;
        }

    private:
        void DoReportStatus();

    private:
        boost::shared_ptr<AsioServiceRunner> service_runner_;
        boost::shared_ptr<boost::asio::deadline_timer> timer_;
        boost::shared_ptr<boost::asio::io_service> io_service_;
        std::map<std::string, ChannelStatus> channel_map_;
        std::map<std::string, GroupStatus> groups_map_;
        GlobalStatus global_channel_status_;
        boost::mutex mutex_;
    };
}

#endif