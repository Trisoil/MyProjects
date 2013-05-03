//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Common.h"
#include "MockRemoteCache.h"

namespace super_node_test
{
    using namespace super_node;

    MockRemoteCache::MockRemoteCache(boost::shared_ptr<boost::asio::io_service> io_service)
    {
        io_service_ = io_service;
        timer_.reset(new boost::asio::deadline_timer(*io_service_));
    }

    void MockRemoteCache::Start()
    {
        StartTimer();
    }

    void MockRemoteCache::Stop()
    {
        
    }

    void MockRemoteCache::DoAsyncReadResource(const ResourceIdentifier& resource_identifier, ReadResourceCallback callback)
    {
        pending_requests_.push_back(callback);
    }

    void MockRemoteCache::AsyncReadResource(const ResourceIdentifier& resource_identifier, ReadResourceCallback callback)
    {
        io_service_->post(bind(&MockRemoteCache::DoAsyncReadResource, shared_from_this(), resource_identifier, callback));
    }

    void MockRemoteCache::StartTimer()
    {
        timer_->expires_from_now(boost::posix_time::milliseconds(150));
        timer_->async_wait(bind(&MockRemoteCache::TimerElapsed, shared_from_this()));
    }

    void MockRemoteCache::TimerElapsed()
    {
        static std::vector<boost::shared_ptr<BlockData> > blocks;
        if (blocks.empty())
        {
            for(size_t i = 0; i < 10; ++i)
            {
                std::vector<char> data(BlockData::MaxBlockSize);
                if (i == 9)
                {
                    data.resize(BlockData::MaxBlockSize - 39);
                }
                blocks.push_back(boost::shared_ptr<BlockData>(new BlockData(data.begin(), data.end())));
            }
        }

        if (pending_requests_.size() > 0)
        {
            ReadResourceCallback callback = pending_requests_.front();
            pending_requests_.pop_front();

            if (rand()%100 <=2)
            {
                callback(ErrorCodes::DownloadFailure, std::vector<boost::shared_ptr<BlockData> >());
            }
            else
            {
                callback(ErrorCodes::Success, blocks);
            }
        }

        StartTimer();
    }
}