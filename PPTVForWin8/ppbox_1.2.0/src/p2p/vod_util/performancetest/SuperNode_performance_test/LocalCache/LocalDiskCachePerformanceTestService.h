//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_LOCAL_DISK_CACHE_PERFORMANCE_TEST_H
#define SUPER_NODE_LOCAL_DISK_CACHE_PERFORMANCE_TEST_H

#include "../PerformanceTestService.h"
#include "SuperNodeServiceStruct.h"

namespace super_node
{
    class AsioServiceRunner;
    class LocalDiskCache;
}

namespace super_node_test
{
    using namespace super_node;

    class MockRemoteCache;
    class LocalDiskCacheConsumer;

    class LocalDiskCachePerformanceTestService
        : public IPerformanceTestService
    {
    public:
        LocalDiskCachePerformanceTestService();

        void Start();
        void Stop();

    private:
        boost::shared_ptr<AsioServiceRunner> main_service_runner_;
        boost::shared_ptr<AsioServiceRunner> secondary_service_runner_;
        
        boost::shared_ptr<LocalDiskCache> local_disk_cache_;
        boost::shared_ptr<MockRemoteCache> remote_cache_;
        boost::shared_ptr<LocalDiskCacheConsumer> local_disk_cache_consumer_;
    };
}

#endif  // SUPER_NODE_LOCAL_DISK_CACHE_PERFORMANCE_TEST_H
