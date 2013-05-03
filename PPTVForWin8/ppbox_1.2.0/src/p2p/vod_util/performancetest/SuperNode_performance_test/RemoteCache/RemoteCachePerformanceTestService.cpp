//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Common.h"
#include "RemoteCachePerformanceTestService.h"
#include "framework/configure/Config.h"
#include "AsioServiceRunner.h"
#include "ServiceHealthMonitor.h"
#include "RemoteCache.h"
#include "RemoteCache/ContentServerResourceManager.h"
#include "RemoteCacheConsumer.h"

namespace super_node_test
{
    using namespace super_node;

    RemoteCachePerformanceTestService::RemoteCachePerformanceTestService()
    {
    }

    void RemoteCachePerformanceTestService::Start()
    {
        assert(!main_service_runner_);
        assert(!secondary_service_runner_);

        main_service_runner_.reset(new AsioServiceRunner("main IO service"));
        boost::shared_ptr<boost::asio::io_service> main_io_service = main_service_runner_->Start();

        secondary_service_runner_.reset(new AsioServiceRunner("secondary IO service"));
        boost::shared_ptr<boost::asio::io_service> secondary_io_service = secondary_service_runner_->Start();

        ServiceHealthMonitor::Instance()->Start(main_io_service);

        remote_cache_config_.LoadConfig();
        content_server_resource_manager_.reset(new ContentServerResourceManager(secondary_io_service, remote_cache_config_));
        server_list_asker_.reset(new ServerListAsker(remote_cache_config_.ask_for_server_list_interval_, remote_cache_config_.place_number_, secondary_io_service , boost::bind(&ContentServerResourceManager::DoUpdateContentServers, content_server_resource_manager_, _1)));
        content_server_resource_manager_->SetServerListAsker(server_list_asker_);
        
        remote_cache_.reset(new RemoteCache(secondary_io_service, remote_cache_config_, content_server_resource_manager_));
        remote_cache_->Start();

        remote_cache_consumer_.reset(new RemoteCacheConsumer(remote_cache_, main_io_service));
        remote_cache_consumer_->Start();
    }

    void RemoteCachePerformanceTestService::Stop()
    {
        ServiceHealthMonitor::Instance()->Stop();

        if (remote_cache_consumer_)
        {
            remote_cache_consumer_->Stop();
            remote_cache_consumer_.reset();
        }

        if (remote_cache_)
        {
            remote_cache_->Stop();
            remote_cache_.reset();
        }

        main_service_runner_->Stop();
        main_service_runner_.reset();
        secondary_service_runner_->Stop();
        secondary_service_runner_.reset();
    }
}