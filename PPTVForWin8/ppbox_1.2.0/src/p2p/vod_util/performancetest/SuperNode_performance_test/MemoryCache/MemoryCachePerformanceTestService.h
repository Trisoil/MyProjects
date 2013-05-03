//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_MEMORY_CACHE_PERFORMANCE_TEST_SERVICE_H
#define SUPER_NODE_MEMORY_CACHE_PERFORMANCE_TEST_SERVICE_H

#include "../PerformanceTestService.h"

namespace super_node
{
    class AsioServiceRunner;
    class MemoryCache;
}

namespace super_node_test
{
    using namespace super_node;

    class MockLocalCache;
    class MemoryCacheConsumer;

    class MemoryCachePerformanceTestService
        : public IPerformanceTestService
    {
    public:
        MemoryCachePerformanceTestService();
        void Start();
        void Stop();

    private:
        boost::shared_ptr<AsioServiceRunner> main_service_runner_;
        boost::shared_ptr<AsioServiceRunner> secondary_service_runner_;
        
        boost::shared_ptr<MockLocalCache> local_disk_cache_;
        boost::shared_ptr<MemoryCache> memory_cache_;
        boost::shared_ptr<MemoryCacheConsumer> memory_cache_consumer_;
    };
}

#endif  // SUPER_NODE_MEMORY_CACHE_PERFORMANCE_TEST_SERVICE_H
