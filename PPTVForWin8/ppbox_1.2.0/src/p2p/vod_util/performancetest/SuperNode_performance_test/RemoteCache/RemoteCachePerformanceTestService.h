//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_REMOTE_CACHE_PERFORMANCE_TEST_H
#define SUPER_NODE_REMOTE_CACHE_PERFORMANCE_TEST_H

#include "../PerformanceTestService.h"
#include "SuperNodeServiceStruct.h"

namespace super_node
{
    class AsioServiceRunner;
    class RemoteCache;
    class IServerListAsker;
    class ContentServerResourceManager;
}

namespace super_node_test
{
    using namespace super_node;

    class RemoteCacheConsumer;

    class RemoteCachePerformanceTestService
        : public IPerformanceTestService
    {
    public:
        RemoteCachePerformanceTestService();

        void Start();
        void Stop();

    private:
        boost::shared_ptr<AsioServiceRunner> main_service_runner_;
        boost::shared_ptr<AsioServiceRunner> secondary_service_runner_;
        
        boost::shared_ptr<RemoteCache> remote_cache_;
        boost::shared_ptr<RemoteCacheConsumer> remote_cache_consumer_;

        boost::shared_ptr<IServerListAsker> server_list_asker_;
        boost::shared_ptr<ContentServerResourceManager> content_server_resource_manager_;

        RemoteCacheConfig remote_cache_config_;
    };
}

#endif  // SUPER_NODE_REMOTE_CACHE_PERFORMANCE_TEST_H
