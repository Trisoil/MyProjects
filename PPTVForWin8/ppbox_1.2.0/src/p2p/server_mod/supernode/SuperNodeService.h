//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_SUPERNODE_SERVICE_H
#define SUPER_NODE_SUPERNODE_SERVICE_H

#include "SuperNodeServiceStruct.h"

namespace super_node
{
    class AsioServiceRunner;
    class LocalDiskCache;
    class RemoteCache;
    class UdpServerListenHandler;
    class UdpServerManager;
    class ContentServerResourceManager;
    class IServerListAsker;
    class MemoryCache;
    class SessionManager;
    class ConfigManager;

    class SuperNodeService
        : public boost::enable_shared_from_this<SuperNodeService>,
        public count_object_allocate<SuperNodeService>
    {
    public:
        SuperNodeService();
        bool Start();
        void Stop();
        void ConfigUpdateListener();
        void LoadConfig();
        bool StartModules();

    private:
        void UpdateConfig();

    private:
        boost::shared_ptr<AsioServiceRunner> main_service_runner_;
        boost::shared_ptr<AsioServiceRunner> secondary_service_runner_;

        boost::shared_ptr<boost::asio::io_service> main_io_service_;
        boost::shared_ptr<boost::asio::io_service> secondary_io_service_;

        boost::shared_ptr<IServerListAsker> server_list_asker_;
        boost::shared_ptr<ContentServerResourceManager> content_server_resource_manager_;
        boost::shared_ptr<RemoteCache> remote_cache_;
        boost::shared_ptr<LocalDiskCache> local_disk_cache_;
        boost::shared_ptr<MemoryCache> memory_cache_;
        boost::shared_ptr<SessionManager> session_manager_;

        UdpServerConfig udp_server_config_;
        RemoteCacheConfig remote_cache_config_;
        SessionManagerConfig session_manager_config_;
        boost::shared_ptr<boost::asio::io_service> io_service_;
        boost::shared_ptr<UdpServerListenHandler> udp_server_listener_;
        boost::shared_ptr<UdpServerManager> udp_server_manager_;
        boost::shared_ptr<ConfigManager> config_manager_;

        bool config_exists_;
    };
}

#endif  // SUPER_NODE_SUPERNODE_SERVICE_H
