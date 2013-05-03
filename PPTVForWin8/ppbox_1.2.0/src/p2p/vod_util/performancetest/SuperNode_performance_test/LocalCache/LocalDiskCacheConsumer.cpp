//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Common.h"
#include "LocalDiskCacheConsumer.h"
#include "LocalDiskCache.h"

namespace super_node_test
{
    using namespace super_node;

    LocalDiskCacheConsumer::LocalDiskCacheConsumer(boost::shared_ptr<LocalDiskCache> local_disk_cache, boost::shared_ptr<boost::asio::io_service> io_service)
    {
        io_service_ = io_service;
        local_disk_cache_ = local_disk_cache;
        timer_.reset(new boost::asio::deadline_timer(*io_service_));
    }

    void LocalDiskCacheConsumer::Start()
    {
        time_counter_.reset();
        ResetCounters();
        StartTimer();
    }

    void LocalDiskCacheConsumer::Stop()
    {
        timer_->cancel();
    }

    void LocalDiskCacheConsumer::TimerElapsed()
    {
        RequestData();
        StartTimer();

        const size_t StatisticsIntervalInSeconds = 10;
        if (time_counter_.elapse() >= StatisticsIntervalInSeconds*1000)
        {
            std::cout<<"Requests sent per second:"<<requests_sent_/StatisticsIntervalInSeconds<<std::endl;
            std::cout<<"Responses received per second:"<<responses_received_/StatisticsIntervalInSeconds<<std::endl;
            std::cout<<"Successful Disk Read per second:"<<success_count_/StatisticsIntervalInSeconds<<std::endl;
            std::cout<<"Rejected Request per second:"<<rejected_count_/StatisticsIntervalInSeconds<<std::endl;
            std::cout<<"Resource Not Found:"<<resource_not_found_count_<<std::endl;
            std::cout<<"Disk IO failures:"<<disk_io_failures_count_<<std::endl;

            ResetCounters();
            time_counter_.reset();
        }
    }

    void LocalDiskCacheConsumer::StartTimer()
    {
        timer_->expires_from_now(boost::posix_time::milliseconds(50));
        timer_->async_wait(bind(&LocalDiskCacheConsumer::TimerElapsed, shared_from_this()));
    }

    void LocalDiskCacheConsumer::RequestData()
    {
        const size_t BatchSize = 30;
        for(size_t i = 0; i < BatchSize; ++i)
        {
            std::ostringstream resource_name_builder;
            resource_name_builder<<"performance-test-movie["<<rand()%10000<<"]";
            ResourceIdentifier resource_identifier(resource_name_builder.str());
            size_t block_index = rand()%10;

            local_disk_cache_->AsyncReadBlock(
                resource_identifier, 
                block_index, 
                20, 
                bind(&LocalDiskCacheConsumer::HandleReadBlock, shared_from_this(), _1, _2, resource_identifier));

            ++requests_sent_;
        }
    }

    void LocalDiskCacheConsumer::ResetCounters()
    {
        requests_sent_ = 0;
        responses_received_ = 0;
        success_count_ = 0;
        resource_not_found_count_ = 0;
        rejected_count_ = 0;
        disk_io_failures_count_ = 0;
    }

    void LocalDiskCacheConsumer::HandleReadBlock(ErrorCode error_code, boost::shared_ptr<BlockData> block_data, const ResourceIdentifier & resource_identifier)
    {
        ++responses_received_;
        if (error_code == ErrorCodes::Success)
        {
            ++success_count_;
        }
        else if (error_code == ErrorCodes::ResourceNotFound)
        {
            ++resource_not_found_count_;
        }
        else if (error_code == ErrorCodes::ServiceBusy)
        {
            ++rejected_count_;
        }
        else
        {
            ++disk_io_failures_count_;
        }
    }
}