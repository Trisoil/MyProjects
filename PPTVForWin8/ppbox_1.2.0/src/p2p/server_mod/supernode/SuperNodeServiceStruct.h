//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#ifndef _SUPER_NODE_SERVICE_STRUCT_
#define _SUPER_NODE_SERVICE_STRUCT_

#include "framework/configure/Config.h"
#include "ConfigManager/ConfigManager.h"

namespace super_node
{
    struct UdpServerConfig
    {
        boost::uint16_t port_;
        boost::uint32_t recv_num_;
        boost::uint16_t minimal_protocol_version_;
        boost::uint16_t response_senders_count_;

        static const boost::uint16_t DefaultPort = 10000;
        static const boost::uint32_t DefaultRecvNum = 100;
        static const boost::uint16_t DefaultMinimalProtocolVersion = 0;
        static const boost::uint16_t DefaultResponseSendersCount = 2;

        UdpServerConfig()
        {
            port_ = DefaultPort;
            recv_num_ = DefaultRecvNum;
            minimal_protocol_version_ = DefaultMinimalProtocolVersion;
            response_senders_count_ = DefaultResponseSendersCount;
        }

        void SetConfig(boost::uint16_t port, boost::uint32_t recv_num, boost::uint16_t minimal_protocol_version, boost::uint16_t response_senders_count)
        {
            port_ = port;
            recv_num_ = recv_num;
            minimal_protocol_version_ = minimal_protocol_version;
            response_senders_count_ = response_senders_count;
        }

        void LoadConfig()
        {
            framework::configure::Config conf(ConfigManager::AllConfigFileName);
            conf.register_module("UdpServer")
                << CONFIG_PARAM_NAME_RDONLY("port", port_)
                << CONFIG_PARAM_NAME_RDONLY("minimal_protocol_version", minimal_protocol_version_)
                << CONFIG_PARAM_NAME_RDONLY("recv_num", recv_num_)
                << CONFIG_PARAM_NAME_RDONLY("response_senders", response_senders_count_);
        }
    };

    struct SessionManagerConfig
    {
        boost::uint16_t max_session_num_;
        boost::uint16_t sesson_keep_time_;
        boost::uint16_t max_transaction_id_num_;

        static const boost::uint16_t DefaultMaxSessionNum = 2000;
        static const boost::uint16_t DefaultSessionKeepTime = 5;
        static const boost::uint16_t DefaultMaxTransactionIdNum = 10;
        
        SessionManagerConfig()
        {
            max_session_num_ = DefaultMaxSessionNum;
            sesson_keep_time_ = DefaultSessionKeepTime;
            max_transaction_id_num_ = DefaultMaxTransactionIdNum;
        }

        void SetSessionManagerCongig(const boost::uint16_t max_session_num, const boost::uint16_t session_keep_time, const boost::uint16_t max_transaction_id_num)
        {
            max_session_num_ = max_session_num;
            sesson_keep_time_ = session_keep_time;
            max_transaction_id_num_ = max_transaction_id_num;
        }

        void LoadConfig()
        {
            framework::configure::Config conf(ConfigManager::AllConfigFileName);
            conf.register_module("SessionManager")
                << CONFIG_PARAM_NAME_RDONLY("max_session_num", max_session_num_)
                << CONFIG_PARAM_NAME_RDONLY("session_keep_time", sesson_keep_time_)
                << CONFIG_PARAM_NAME_RDONLY("max_transaction_id_num", max_transaction_id_num_);
        }
    };

    struct RemoteCacheConfig
    {
        std::string  place_identifier_;
        size_t max_queue_size_;
        size_t max_concurrent_download_tasks_;
        size_t ask_for_server_list_interval_;
        size_t ask_for_resource_list_interval_;
        size_t resource_list_download_timeout_in_seconds_;
        size_t resource_download_timeout_in_seconds_;
        
        static const std::string DefaultPlaceIdentifier;
        static const size_t DefaultMaxQueueSize = 10;
        static const size_t DefaultMaxConcurrentDownloadTasks = 10;
        static const size_t DefaultAskForServerListInterval = 4;
        static const size_t DefaultAskForResourceListInterval = 60;
        static const size_t DefaultResourceListDownloadTimeoutInSeconds = 60;
        static const size_t DefaultResourceDownloadTimeoutInSeconds = 8;

        RemoteCacheConfig()
        {
            place_identifier_ = DefaultPlaceIdentifier;
            max_queue_size_ = DefaultMaxQueueSize;
            max_concurrent_download_tasks_ = DefaultMaxConcurrentDownloadTasks;
            ask_for_server_list_interval_ = DefaultAskForServerListInterval;
            ask_for_resource_list_interval_ = DefaultAskForResourceListInterval;
            resource_download_timeout_in_seconds_ = DefaultResourceDownloadTimeoutInSeconds;
            resource_list_download_timeout_in_seconds_ = DefaultResourceListDownloadTimeoutInSeconds;
        }

        void SetConfig(const std::string & place_identifier, size_t max_concurrent_download_tasks, 
            size_t max_queue_size, size_t ask_for_server_list_interval, size_t ask_for_resource_list_interval, size_t resource_download_timeout_in_seconds, size_t resource_list_download_timeout_in_seconds)
        {
            place_identifier_ = place_identifier;
            max_concurrent_download_tasks_ = max_concurrent_download_tasks;
            max_queue_size_ = max_queue_size;
            ask_for_server_list_interval_ = ask_for_server_list_interval;
            ask_for_resource_list_interval_ = ask_for_resource_list_interval;
            resource_list_download_timeout_in_seconds_ = resource_list_download_timeout_in_seconds;
            resource_download_timeout_in_seconds_ = resource_download_timeout_in_seconds;
        }

        void LoadConfig()
        {
            framework::configure::Config conf(ConfigManager::AllConfigFileName);
            conf.register_module("RemoteCache")
                << CONFIG_PARAM_NAME_RDONLY("place_number", place_identifier_)
                << CONFIG_PARAM_NAME_RDONLY("max_queue_size", max_queue_size_)
                << CONFIG_PARAM_NAME_RDONLY("max_concurrent_download_tasks", max_concurrent_download_tasks_)
                << CONFIG_PARAM_NAME_RDONLY("ask_for_server_list_interval_in_minutes", ask_for_server_list_interval_)
                << CONFIG_PARAM_NAME_RDONLY("ask_for_resource_list_interval_in_minutes", ask_for_resource_list_interval_)
                << CONFIG_PARAM_NAME_RDONLY("resource_list_download_timeout_in_seconds", resource_list_download_timeout_in_seconds_)
                << CONFIG_PARAM_NAME_RDONLY("resource_download_timeout_in_seconds", resource_download_timeout_in_seconds_);
        }
    };
}

#endif