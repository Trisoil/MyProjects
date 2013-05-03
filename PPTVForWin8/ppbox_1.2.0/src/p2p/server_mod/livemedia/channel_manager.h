//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef CHANNLE_MANAGER_H
#define CHANNLE_MANAGER_H

#include "channel.h"
#include "hash_map.h"

namespace live_media
{
    class ChannelServer;
    class StatusFile;

    class IChannelManager
    {
    public:
        virtual const std::vector<IChannelPointer> GetChannels() const = 0;
        virtual ~IChannelManager(){}
    };

    class ChannelManager
        : public IChannelManager
    {
        struct channel_id_hash
        {
            enum
            {
                bucket_size = 4,	// 0 < bucket_size
                min_buckets = 64   // minimum number of buckets, power of 2, >0 
            };

            size_t operator()(const channel_id& id) const 
            {
                return id.hash_value();
            }

            bool operator()(const channel_id& left, const channel_id& right) const 
            {
                return left < right;
            }
        };

        typedef hash_map<channel_id, ChannelPointer, channel_id_hash> ChannelsCollection;

    public:
        ChannelManager(ChannelServer * server);
        ~ChannelManager();

        bool Initalize();
        void Uninitalize();

        /**
        *	指定一个 channel_id,增加一个新的 channel, 如果已经存在,或者 c_id 格式不对,返回 0, 表示失败
        */
        ChannelPointer AddChannel(const ChannelConfiguration & c);

        /**
        *	指定一个 channel_id,删除指定的 channel, 如果不存在,或者 c_id 格式不对,返回 false, 表示失败
        */
        bool RemoveChannel(const channel_id & c_id);

        /**
        *	指定一个 channel_id,返回指定的 channel, 如果不存在,或者 c_id 格式不对,返回 0, 表示失败
        */
        ChannelPointer GetChannel(const channel_id & c_id);

        const std::vector<IChannelPointer> GetChannels() const;

        ChannelServer * server_;

    private:
        ChannelsCollection channels_;

        bool StartTimer();
        void OnTimer();

        framework::timer::PeriodicTimer::pointer channels_on_timer_;
        framework::timer::AsioTimerManager * asio_timer_manager_;

        time_t last_check_channel_priority_time_;

        void UpdateTrackerRidList();

        void GenerateStatusFile();

        void RemoveObsoleteChannels();

    private:
        // 总的对速度的限制值
        size_t total_upload_speed_limit_;
        size_t total_upload_connection_limit_;
        size_t total_keep_connection_limit_;
        size_t total_uploading_peer_count_;

        time_t last_report_state_time_;
        boost::thread * state_report_thread_;
        bool exiting_report_thread_;
        boost::shared_ptr<StatusFile> status_file_;

        typedef ChannelsCollection::iterator  channel_iterator;

    public:
        size_t current_uploading_count_;
    };
}
#endif //CHANNLE_MANAGER_H
