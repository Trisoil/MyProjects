//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Common.h"
#include "MockLocalCache.h"
#include "framework/configure/Config.h"

namespace super_node_test
{
    using namespace super_node;

    MockLocalCache::MockLocalCache(boost::shared_ptr<boost::asio::io_service> main_io_service, boost::shared_ptr<boost::asio::io_service> disk_cache_io_service)
    {
        main_io_service_ = main_io_service;
        disk_cache_io_service_ = disk_cache_io_service;
        timer_.reset(new boost::asio::deadline_timer(*disk_cache_io_service_));
    }

    void MockLocalCache::Start()
    {
        time_counter_.reset();
        StartTimer();
    }

    void MockLocalCache::Stop()
    {
    }

    void MockLocalCache::AsyncReadBlock(const ResourceIdentifier& resource_identifier, size_t block_index, int read_priority, ReadResourceBlockCallback callback)
    {
        disk_cache_io_service_->post(bind(&MockLocalCache::AddBlockReadRequest, shared_from_this(), resource_identifier, block_index, callback));
    }

    void MockLocalCache::AddBlockReadRequest(const ResourceIdentifier& resource_identifier, size_t block_index, ReadResourceBlockCallback callback)
    {
        pending_requests_.insert(
            std::make_pair(
                time_counter_.elapse() + 30, 
                boost::shared_ptr<BlockRequest>(
                    new BlockRequest(
                        resource_identifier, 
                        block_index, 
                        callback))));
    }

    void MockLocalCache::StartTimer()
    {
        timer_->expires_from_now(boost::posix_time::milliseconds(20));
        timer_->async_wait(bind(&MockLocalCache::TimerElapsed, shared_from_this()));
    }

    void MockLocalCache::TimerElapsed()
    {
        size_t now = time_counter_.elapse();
        for(std::multimap<size_t, boost::shared_ptr<BlockRequest> >::iterator iter = pending_requests_.begin();
            iter != pending_requests_.end();)
        {
            if (iter->first <= now)
            {
                ProcessRequest(iter->second);
                pending_requests_.erase(iter++);
            }
            else
            {
                break;
            }
        }

        StartTimer();
    }

    void MockLocalCache::ProcessRequest(boost::shared_ptr<BlockRequest> block_request)
    {
        //两个目的
        //1. 避免构造BlockData对MemoryCache性能测试的开销
        //2. 这样实际的内存使用量很小（即便MemoryCache中的BlockCache数目很高），方便在内存有限的情况下测试memory cache的性能。
        static std::vector<char> data(BlockData::MaxBlockSize);
        static boost::shared_ptr<BlockData> block_data(new BlockData(data.begin(), data.end()));
        main_io_service_->post(bind(block_request->callback_, ErrorCodes::Success, block_data));
    }
}