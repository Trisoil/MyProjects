//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_MEMORY_CACHE_CONSUMER_H
#define SUPER_NODE_MEMORY_CACHE_CONSUMER_H

namespace super_node
{
    class MemoryCache;
}

namespace super_node_test
{
    using namespace super_node;

    class MemoryCacheConsumer
        : public boost::enable_shared_from_this<MemoryCacheConsumer>
    {
    public:
        MemoryCacheConsumer(boost::shared_ptr<MemoryCache> memory_cache, boost::shared_ptr<boost::asio::io_service> io_service);

        void Start();
        void Stop();

    private:
        void TimerElapsed();
        void StartTimer();
        void HandleReadBlock(const ErrorCode & error_code, const boost::shared_ptr<BlockData> & block,
            const string & resource_name, boost::uint16_t block_index, const std::set<boost::uint16_t> & subpiece_indexs);
        void RequestData();
        void ResetCounters();

    private:
        boost::shared_ptr<boost::asio::io_service> io_service_;
        boost::shared_ptr<MemoryCache> memory_cache_;
        
        boost::shared_ptr<boost::asio::deadline_timer> timer_;
        framework::timer::TimeCounter time_counter_;

        size_t requests_sent_;
        size_t responses_received_;
    };
}

#endif  // SUPER_NODE_MEMORY_CACHE_CONSUMER_H
