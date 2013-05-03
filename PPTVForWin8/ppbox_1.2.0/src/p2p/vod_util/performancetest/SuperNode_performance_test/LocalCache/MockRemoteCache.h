//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_MOCK_REMOTE_CACHE_H
#define SUPER_NODE_MOCK_REMOTE_CACHE_H

#include "RemoteCache.h"

namespace super_node_test
{
    using namespace super_node;

    class MockRemoteCache
        : public boost::enable_shared_from_this<MockRemoteCache>,
        public IRemoteCache
    {
    public:
        MockRemoteCache(boost::shared_ptr<boost::asio::io_service> io_service);

        void Start();
        void Stop();

        void AsyncReadResource(const ResourceIdentifier& resource_identifier, ReadResourceCallback callback);

    private:
        void DoAsyncReadResource(const ResourceIdentifier& resource_identifier, ReadResourceCallback callback);
        void StartTimer();
        void TimerElapsed();

    private:
        boost::shared_ptr<boost::asio::io_service> io_service_;
        boost::shared_ptr<boost::asio::deadline_timer> timer_;
        std::deque<ReadResourceCallback> pending_requests_;
    };
}

#endif  // SUPER_NODE_MOCK_REMOTE_CACHE_H
