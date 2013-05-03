//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_LOCAL_DISK_CACHE_CONSUMER_H
#define SUPER_NODE_LOCAL_DISK_CACHE_CONSUMER_H

namespace super_node
{
    class RemoteCache;
}

namespace super_node_test
{
    using namespace super_node;

    class RemoteCacheConsumer
        : public boost::enable_shared_from_this<RemoteCacheConsumer>
    {
    public:
        RemoteCacheConsumer(boost::shared_ptr<RemoteCache> remote_cache, boost::shared_ptr<boost::asio::io_service> io_service);

        void Start();
        void Stop();

    private:
        void TimerElapsed();
        void StartTimer();
        void RequestData();
        void ResetCounters();
        void HandleReadResource(ErrorCode error_code, const std::vector<boost::shared_ptr<BlockData> >& blocks);
        void LoadMovies();

    private:
        boost::shared_ptr<boost::asio::io_service> io_service_;
        boost::shared_ptr<RemoteCache> remote_cache_;
        
        std::vector<ResourceIdentifier> test_resource_identifiers_;

        boost::shared_ptr<boost::asio::deadline_timer> timer_;
        framework::timer::TimeCounter time_counter_;

        size_t requests_sent_;
        size_t responses_received_;
        size_t success_count_;
        size_t download_failures_count_;
        size_t resource_not_found_count_;
        size_t rejected_count_;
    };
}

#endif  // SUPER_NODE_LOCAL_DISK_CACHE_CONSUMER_H
