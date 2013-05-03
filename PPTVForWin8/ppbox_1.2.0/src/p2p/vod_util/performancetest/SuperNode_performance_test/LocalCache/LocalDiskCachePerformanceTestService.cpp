//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Common.h"
#include "LocalDiskCachePerformanceTestService.h"
#include "framework/configure/Config.h"
#include "AsioServiceRunner.h"
#include "ServiceHealthMonitor.h"
#include "LocalDiskCache.h"
#include "MockRemoteCache.h"
#include "LocalDiskCacheConsumer.h"

namespace super_node_test
{
    using namespace super_node;

    LocalDiskCachePerformanceTestService::LocalDiskCachePerformanceTestService()
    {
    }

    void LocalDiskCachePerformanceTestService::Start()
    {
        assert(!main_service_runner_);
        assert(!secondary_service_runner_);

        main_service_runner_.reset(new AsioServiceRunner("main IO service"));
        boost::shared_ptr<boost::asio::io_service> main_io_service = main_service_runner_->Start();

        secondary_service_runner_.reset(new AsioServiceRunner("secondary IO service"));
        boost::shared_ptr<boost::asio::io_service> secondary_io_service = secondary_service_runner_->Start();

        ServiceHealthMonitor::Instance()->Start(main_io_service);

        remote_cache_.reset(new MockRemoteCache(secondary_io_service));
        remote_cache_->Start();

        local_disk_cache_.reset(new LocalDiskCache(remote_cache_, main_io_service));
        local_disk_cache_->Start();

        local_disk_cache_consumer_.reset(new LocalDiskCacheConsumer(local_disk_cache_, main_io_service));
        local_disk_cache_consumer_->Start();
    }

    void LocalDiskCachePerformanceTestService::Stop()
    {
        ServiceHealthMonitor::Instance()->Stop();

        if (local_disk_cache_consumer_)
        {
            local_disk_cache_consumer_->Stop();
            local_disk_cache_consumer_.reset();
        }

        if (local_disk_cache_)
        {
            local_disk_cache_->Stop();
            local_disk_cache_.reset();
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