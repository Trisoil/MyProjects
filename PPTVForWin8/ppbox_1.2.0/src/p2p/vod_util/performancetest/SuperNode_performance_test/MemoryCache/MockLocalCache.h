//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_MOCK_LOCAL_CACHE_H
#define SUPER_NODE_MOCK_LOCAL_CACHE_H

#include "LocalDiskCache.h"
#include "LocalDiskCache/PendingBlockReadRequest.h"

namespace super_node
{
    class ResourceIdentifier;
}

namespace super_node_test
{
    using namespace super_node;

    struct BlockRequest
    {
        BlockRequest(ResourceIdentifier resource_identifier, size_t block_index, ReadResourceBlockCallback callback)
            : resource_identifier_(resource_identifier), block_index_(block_index), callback_(callback)
        {}

        ResourceIdentifier resource_identifier_;
        size_t block_index_;
        ReadResourceBlockCallback callback_;
    };

    class MockLocalCache
        : public boost::enable_shared_from_this<MockLocalCache>,
        public ILocalDiskCache
    {
    public:
        MockLocalCache(boost::shared_ptr<boost::asio::io_service> main_io_service, boost::shared_ptr<boost::asio::io_service> disk_cache_io_service);

        //called from console thread
        void Start();
        void Stop();

        void AsyncReadBlock(const ResourceIdentifier& resource_identifier, size_t block_index, int read_priority, ReadResourceBlockCallback callback);

    private:
        void TimerElapsed();
        void StartTimer();
        void ProcessRequest(boost::shared_ptr<BlockRequest> block_request);
        void AddBlockReadRequest(const ResourceIdentifier& resource_identifier, size_t block_index, ReadResourceBlockCallback callback);

    private:
        boost::shared_ptr<boost::asio::io_service> main_io_service_;
        boost::shared_ptr<boost::asio::io_service> disk_cache_io_service_;
        framework::timer::TimeCounter time_counter_;
        std::multimap<size_t, boost::shared_ptr<BlockRequest> > pending_requests_;
        boost::shared_ptr<boost::asio::deadline_timer> timer_;
    };
}

#endif  // SUPER_NODE_MOCK_LOCAL_CACHE_H
