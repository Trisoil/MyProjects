//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Common.h"
#include "MemoryCacheConsumer.h"
#include "MemoryCache.h"

namespace super_node_test
{
    using namespace super_node;

    MemoryCacheConsumer::MemoryCacheConsumer(boost::shared_ptr<MemoryCache> memory_cache, boost::shared_ptr<boost::asio::io_service> io_service)
    {
        io_service_ = io_service;
        memory_cache_ = memory_cache;
        timer_.reset(new boost::asio::deadline_timer(*io_service_));
    }

    void MemoryCacheConsumer::Start()
    {
        time_counter_.reset();
        ResetCounters();
        StartTimer();
    }

    void MemoryCacheConsumer::Stop()
    {
        timer_->cancel();
    }

    void MemoryCacheConsumer::TimerElapsed()
    {
        RequestData();
        StartTimer();

        if (time_counter_.elapse() >= 5000)
        {
            std::cout<<"Requests sent per second:"<<requests_sent_/5<<std::endl;
            std::cout<<"Responses received per second:"<<responses_received_/5<<std::endl;

            ResetCounters();
            time_counter_.reset();
        }
    }

    void MemoryCacheConsumer::ResetCounters()
    {
        requests_sent_ = 0;
        responses_received_ = 0;
    }

    void MemoryCacheConsumer::RequestData()
    {
        const size_t ResourceCount = 100;
        const size_t BlockRange = 1000;

        //总共可能的block数=ResourceCount*BlockRange
        //MemoryCache命中率=配置中的max_cache_block_num/block数
        for(size_t i = 0; i < ResourceCount; ++i)
        {
            std::ostringstream resource_name_builder;
            resource_name_builder << "test_movie[" <<i<<"].mp4";
            string resource_name = resource_name_builder.str();
            int block_index = rand()%BlockRange;
            std::set<boost::uint16_t> subpieces;
            const boost::uint16_t MaxSubPieceIndex = 20;
            for(boost::uint16_t subpiece_index = 0;subpiece_index < MaxSubPieceIndex; ++subpiece_index)
            {
                subpieces.insert(subpiece_index);
            }

            memory_cache_->AsyncReadBlock(resource_name, block_index, rand()%(MaxSubPieceIndex/2), bind(&MemoryCacheConsumer::HandleReadBlock, shared_from_this(), _1, _2, resource_name, block_index, subpieces));
            ++requests_sent_;
        }
    }

    void MemoryCacheConsumer::HandleReadBlock(
        const ErrorCode & error_code, 
        const boost::shared_ptr<BlockData> & block,
        const string & resource_name, 
        boost::uint16_t block_index,
        const std::set<boost::uint16_t> & subpiece_indexs)
    {
        ++responses_received_;
    }

    void MemoryCacheConsumer::StartTimer()
    {
        timer_->expires_from_now(boost::posix_time::milliseconds(5));
        timer_->async_wait(bind(&MemoryCacheConsumer::TimerElapsed, shared_from_this()));
    }
}