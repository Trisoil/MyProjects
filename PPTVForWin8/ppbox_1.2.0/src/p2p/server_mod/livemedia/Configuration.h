//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef LIVEMEDIA_CONFIGURATION_H
#define LIVEMEDIA_CONFIGURATION_H

namespace live_media
{
    struct DownloadConfiguration
    {
        static const boost::uint32_t DefaultMaxConcurrentDownloadTasks = 50;
        static const boost::uint32_t DefaultDownloadTaskQueueSize = 50;
        static const boost::uint32_t DefaultDownloadTimeOutInSeconds = 5;

        DownloadConfiguration(
            boost::uint32_t max_concurrent_download_tasks = DefaultMaxConcurrentDownloadTasks,
            boost::uint32_t download_task_queue_size = DefaultDownloadTaskQueueSize,
            boost::uint32_t download_timeout_in_seconds = DefaultDownloadTimeOutInSeconds)
        {
            max_concurrent_download_tasks_ = max_concurrent_download_tasks;
            download_task_queue_size_ = download_task_queue_size;
            download_timeout_in_seconds_ = download_timeout_in_seconds;
        }

        boost::uint32_t max_concurrent_download_tasks_;
        boost::uint32_t download_task_queue_size_;
        boost::uint32_t download_timeout_in_seconds_;
    };

    struct CacheExpirationConfiguration
    {
        static const boost::uint32_t DefaultBlockDataObsoleteLimitInSeconds = 60*3;
        static const boost::uint32_t DefaultBlockCacheObsoleteLimitInSeconds = 60*10;
        static const boost::uint32_t DefaultLastVisitLimitInSeconds = 5;

        CacheExpirationConfiguration(
            boost::uint32_t block_data_obsolete_limit_in_second = DefaultBlockDataObsoleteLimitInSeconds,
            boost::uint32_t block_cache_obsolete_limit_in_second = DefaultBlockDataObsoleteLimitInSeconds,
            boost::uint32_t last_visit_limit_in_second = DefaultLastVisitLimitInSeconds)
        {
            block_cache_obsolete_limit_in_second_ = block_cache_obsolete_limit_in_second;
            block_data_obsolete_limit_in_second_ = block_data_obsolete_limit_in_second;
            last_visit_limit_in_second_ = last_visit_limit_in_second;
        }

        boost::uint32_t block_data_obsolete_limit_in_second_;
        boost::uint32_t block_cache_obsolete_limit_in_second_;
        boost::uint32_t last_visit_limit_in_second_;
    };

    struct SessionManagementConfiguration
    {
        size_t max_session_num_;
        boost::uint16_t sesson_keep_time_;
        boost::uint16_t max_transaction_id_num_;
        boost::uint16_t upload_session_time_limit_in_second_;

        static const size_t DefaultMaxSessionNum = 0;
        static const boost::uint32_t DefaultSessionKeepTime = 10;
        static const boost::uint32_t DefaultMaxTransactionIdNum = 5;
        static const boost::uint32_t DefaultUploadSessionTimeLimitInSecond = 1;

        SessionManagementConfiguration(
            size_t max_session_num = DefaultMaxSessionNum, 
            boost::uint32_t session_keep_time = DefaultSessionKeepTime, 
            boost::uint32_t max_transaction_id_num = DefaultMaxTransactionIdNum,
            boost::uint32_t upload_session_time_limit_in_second = DefaultUploadSessionTimeLimitInSecond)
        {
            max_session_num_ = max_session_num;
            sesson_keep_time_ = session_keep_time;
            max_transaction_id_num_ = max_transaction_id_num;
            upload_session_time_limit_in_second_ = upload_session_time_limit_in_second;
        }
    };

    struct ChannelConfiguration 
    {
        static const boost::uint32_t DefaultMaxUploadSpeed = 1024000000;
        static const boost::uint32_t DefaultMaxSessionCount = 5000;
        static const boost::uint32_t DefaultChannelStepSize = 5;
        
        ChannelConfiguration()
        {
            max_upload_speed = DefaultMaxUploadSpeed;
            max_keep_connections = DefaultMaxSessionCount;
            max_upload_in_a_while = DefaultMaxSessionCount;
            channel_step_time = DefaultChannelStepSize;
        }

        channel_id guid;
        boost::uint32_t max_upload_speed;
        boost::uint32_t max_upload_in_a_while;
        boost::uint32_t max_keep_connections;
        std::string channel_fetch_base_url;

        boost::uint32_t channel_step_time;
    };

    struct server_config
    {
        static const boost::uint32_t DefaultListenPort = 8888;
        static const size_t DefaultGlobalMaxUploadSpeed = 0;
        static const size_t DefaultResponseSendersCount = 2;
        static const size_t DefaultReceiverQueueSize = 1;

        boost::uint32_t p2p_listen_port;
        peer_id my_id;

        // 这个ID是由服务器下发的一个ID值，用来较验服务器的配置是否生效
        std::string config_id;

        size_t global_max_upload_speed;
        size_t response_senders_count;
        size_t receiver_queue_size;
        std::string fetch_domain;

        bool open_channel_on_demand;
        size_t channel_obsolete_time_in_seconds;
        size_t max_upload_speed_per_channel;
        size_t max_keep_connections_per_channel;

        CacheExpirationConfiguration cache_expiration;
        SessionManagementConfiguration session_management;
        DownloadConfiguration download;

        std::vector<ChannelConfiguration> channels;

        server_config()
        {
            p2p_listen_port = DefaultListenPort;
            fetch_domain = "doghole";
            global_max_upload_speed = DefaultGlobalMaxUploadSpeed;
            config_id = "unknown";
            response_senders_count = DefaultResponseSendersCount;
            receiver_queue_size = DefaultReceiverQueueSize;
            open_channel_on_demand = true;
            channel_obsolete_time_in_seconds = 30 * 60;
            max_upload_speed_per_channel = DefaultGlobalMaxUploadSpeed;
            max_keep_connections_per_channel = 100000;
        }
    };
}

#endif //LIVEMEDIA_CONFIGURATION_H
