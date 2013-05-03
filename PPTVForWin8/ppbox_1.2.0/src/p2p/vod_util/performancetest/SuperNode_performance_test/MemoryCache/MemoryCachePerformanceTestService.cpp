//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Common.h"
#include "MemoryCachePerformanceTestService.h"
#include "framework/configure/Config.h"
#include "AsioServiceRunner.h"
#include "MemoryCache.h"
#include "ServiceHealthMonitor.h"
#include "MockLocalCache.h"
#include "MemoryCacheConsumer.h"

namespace super_node_test
{
    using namespace super_node;

    MemoryCachePerformanceTestService::MemoryCachePerformanceTestService()
    {
    }

    void MemoryCachePerformanceTestService::Start()
    {
        assert(!main_service_runner_);
        assert(!secondary_service_runner_);

        main_service_runner_.reset(new AsioServiceRunner("main IO service"));
        boost::shared_ptr<boost::asio::io_service> main_io_service = main_service_runner_->Start();

        secondary_service_runner_.reset(new AsioServiceRunner("secondary IO service"));
        boost::shared_ptr<boost::asio::io_service> secondary_io_service = secondary_service_runner_->Start();

        ServiceHealthMonitor::Instance()->Start(main_io_service);
        
        local_disk_cache_.reset(new MockLocalCache(main_io_service, secondary_io_service));
        local_disk_cache_->Start();

        memory_cache_.reset(new MemoryCache(local_disk_cache_, main_io_service));
        memory_cache_->Start();

        memory_cache_consumer_.reset(new MemoryCacheConsumer(memory_cache_, main_io_service));
        memory_cache_consumer_->Start();
    }

    void MemoryCachePerformanceTestService::Stop()
    {
        ServiceHealthMonitor::Instance()->Stop();

        if (memory_cache_consumer_)
        {
            memory_cache_consumer_->Stop();
            memory_cache_consumer_.reset();
        }

        if (local_disk_cache_)
        {
            local_disk_cache_->Stop();
            local_disk_cache_.reset();
        }

        if (memory_cache_)
        {
            memory_cache_->Stop();
            memory_cache_.reset();
        }

        main_service_runner_->Stop();
        main_service_runner_.reset();
        secondary_service_runner_->Stop();
        secondary_service_runner_.reset();
    }
}