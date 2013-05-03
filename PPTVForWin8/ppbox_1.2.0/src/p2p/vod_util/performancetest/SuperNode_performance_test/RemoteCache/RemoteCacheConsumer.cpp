//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Common.h"
#include "RemoteCacheConsumer.h"
#include "RemoteCache.h"

namespace super_node_test
{
    using namespace super_node;

    RemoteCacheConsumer::RemoteCacheConsumer(boost::shared_ptr<RemoteCache> remote_cache, boost::shared_ptr<boost::asio::io_service> io_service)
    {
        io_service_ = io_service;
        remote_cache_ = remote_cache;
        timer_.reset(new boost::asio::deadline_timer(*io_service_));
    }

    void RemoteCacheConsumer::LoadMovies()
    {
        test_resource_identifiers_.clear();

        std::ifstream movies_list_file("TestMovies.txt");
        if (movies_list_file)
        {
            size_t count(0);

            while(!movies_list_file.eof())
            {
                string movie_name;
                movies_list_file>>movie_name;
                if (movie_name.size() > 0)
                {
                    test_resource_identifiers_.push_back(ResourceIdentifier(movie_name));

                    if (++count%10 == 0)
                    {
                        std::ostringstream resource_name_builder;
                        resource_name_builder<<"resource-cache-perf-test["<<count<<"].mp4";
                        test_resource_identifiers_.push_back(ResourceIdentifier(resource_name_builder.str()));
                    }
                }
            }
        }
    }

    void RemoteCacheConsumer::Start()
    {
        LoadMovies();

        time_counter_.reset();
        ResetCounters();
        StartTimer();
    }

    void RemoteCacheConsumer::Stop()
    {
        timer_->cancel();
    }

    void RemoteCacheConsumer::TimerElapsed()
    {
        RequestData();
        StartTimer();

        const size_t StatisticsIntervalInSeconds = 10;
        if (time_counter_.elapse() >= StatisticsIntervalInSeconds*1000)
        {
            std::cout<<"In the last "<<StatisticsIntervalInSeconds<<" seconds:"<<std::endl;
            std::cout<<"Requests Sent:"<<requests_sent_<<std::endl;
            std::cout<<"Responses Received:"<<responses_received_<<std::endl;
            std::cout<<"Successful Downloads:"<<success_count_<<std::endl;
            std::cout<<"Download Failures:"<<download_failures_count_<<std::endl;
            std::cout<<"Rejected Requests:"<<rejected_count_<<std::endl;
            std::cout<<"Resource Not Found:"<<resource_not_found_count_<<std::endl;

            ResetCounters();
            time_counter_.reset();
        }
    }

    void RemoteCacheConsumer::StartTimer()
    {
        timer_->expires_from_now(boost::posix_time::seconds(1));
        timer_->async_wait(bind(&RemoteCacheConsumer::TimerElapsed, shared_from_this()));
    }

    void RemoteCacheConsumer::RequestData()
    {
        static size_t num = 0;

        if (test_resource_identifiers_.size() == 0)
        {
            return;
        }

        remote_cache_->AsyncReadResource(
            test_resource_identifiers_[(num++)%test_resource_identifiers_.size()], 
            io_service_->wrap(bind(&RemoteCacheConsumer::HandleReadResource, shared_from_this(), _1, _2)));

        ++requests_sent_;
    }

    void RemoteCacheConsumer::ResetCounters()
    {
        requests_sent_ = 0;
        responses_received_ = 0;
        success_count_ = 0;
        download_failures_count_ = 0;
        resource_not_found_count_ = 0;
        rejected_count_ = 0;
    }

    void RemoteCacheConsumer::HandleReadResource(ErrorCode error_code, const std::vector<boost::shared_ptr<BlockData> >& blocks)
    {
        ++responses_received_;
        if (error_code == ErrorCodes::Success)
        {
            ++success_count_;
        }
        else if (error_code == ErrorCodes::DownloadFailure)
        {
            ++download_failures_count_;
        }
        else if (error_code == ErrorCodes::ResourceNotFound)
        {
            ++resource_not_found_count_;
        }
        else
        {
            ++rejected_count_;
        }
    }
}