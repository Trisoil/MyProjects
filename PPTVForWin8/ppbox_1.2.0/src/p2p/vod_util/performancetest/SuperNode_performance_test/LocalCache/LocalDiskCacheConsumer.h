//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_LOCAL_DISK_CACHE_CONSUMER_H
#define SUPER_NODE_LOCAL_DISK_CACHE_CONSUMER_H

namespace super_node
{
    class LocalDiskCache;
}

namespace super_node_test
{
    using namespace super_node;

    class LocalDiskCacheConsumer
        : public boost::enable_shared_from_this<LocalDiskCacheConsumer>
    {
    public:
        LocalDiskCacheConsumer(boost::shared_ptr<LocalDiskCache> local_disk_cache, boost::shared_ptr<boost::asio::io_service> io_service);

        void Start();
        void Stop();

    private:
        void TimerElapsed();
        void StartTimer();
        void RequestData();
        void ResetCounters();
        void HandleReadBlock(ErrorCode error_code, boost::shared_ptr<BlockData> block_data, const ResourceIdentifier & resource_identifier);

    private:
        boost::shared_ptr<boost::asio::io_service> io_service_;
        boost::shared_ptr<LocalDiskCache> local_disk_cache_;
        
        boost::shared_ptr<boost::asio::deadline_timer> timer_;
        framework::timer::TimeCounter time_counter_;

        size_t requests_sent_;
        size_t responses_received_;
        size_t success_count_;
        size_t resource_not_found_count_;
        size_t rejected_count_;
        size_t disk_io_failures_count_;
    };
}

#endif  // SUPER_NODE_LOCAL_DISK_CACHE_CONSUMER_H
