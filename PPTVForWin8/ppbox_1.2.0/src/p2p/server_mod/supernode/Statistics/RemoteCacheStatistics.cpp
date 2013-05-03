//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "RemoteCacheStatistics.h"
#include "ServiceHealthMonitor.h"

namespace super_node
{
    const size_t RemoteCacheStatistics::RemoteCacheStatisticsUpdateIntervalInSeconds = 60;

    RemoteCacheStatistics::RemoteCacheStatistics()
    {
        ResetCounters();
    }

    void RemoteCacheStatistics::Start(boost::shared_ptr<boost::asio::io_service> io_service)
    {
        boost::shared_ptr<ServiceComponent> service_component = ServiceHealthMonitor::Instance()->RegisterComponent(ServiceComponents::RemoteCache);

        average_download_speed_ = service_component->RegisterStatusItem("Average Download Speed", "MB/s");
        dropped_requests_rate_ = service_component->RegisterStatusItem("Rejected Requests Rate", "%");
        content_server_hit_rate_ = service_component->RegisterStatusItem("CDN Hit Rate", "%");
        download_failure_rate_ = service_component->RegisterStatusItem("Download Failure Rate", "%");
        average_download_queue_length_ = service_component->RegisterStatusItem("Average Download Queue Length", "");
        average_wait_time_before_download_ = service_component->RegisterStatusItem("Average Wait Time Before Download", "ms");
        average_download_time_ = service_component->RegisterStatusItem("Average Download Time", "ms");
        average_concurrent_tasks_ = service_component->RegisterStatusItem("Average Download Tasks in Progress", "");

        ResetCounters();

        timer_.reset(new boost::asio::deadline_timer(*io_service));
        timer_->expires_from_now(boost::posix_time::seconds(RemoteCacheStatisticsUpdateIntervalInSeconds));
        timer_->async_wait(boost::bind(&RemoteCacheStatistics::OnTimerElapsed, shared_from_this()));
    }

    void RemoteCacheStatistics::Stop()
    {
        if (timer_)
        {
            boost::system::error_code cancel_error;
            timer_->cancel(cancel_error);
            timer_.reset();
        }

        ServiceHealthMonitor::Instance()->UnregisterComponent(ServiceComponents::RemoteCache);
    }

    void RemoteCacheStatistics::ResetCounters()
    {
        bytes_downloaded_ = 0;
        queue_statistics_.Reset();
        tasks_in_progress_statistics_.Reset();
        wait_time_before_download_statistics_.Reset();
        download_time_statistics_.Reset();
        download_statistics_.Reset();
        resource_hit_statistics_.Reset();
    }

    void RemoteCacheStatistics::OnRequestEnqueued()
    {
        queue_statistics_.Enqueue();
        resource_hit_statistics_.Hit();
    }

    void RemoteCacheStatistics::OnRequestDropped()
    {
        queue_statistics_.Drop();
    }

    void RemoteCacheStatistics::OnDownloadStarted(size_t wait_time_in_queue_in_ms)
    {
        wait_time_before_download_statistics_.Add(wait_time_in_queue_in_ms);
    }

    void RemoteCacheStatistics::OnDownloadFailed()
    {
        download_statistics_.Missed();
    }

    void RemoteCacheStatistics::SetCurrentDownloadQueueLength(size_t queue_length)
    {
        queue_statistics_.Add(queue_length);
    }

    void RemoteCacheStatistics::OnResourceNotFound()
    {
        resource_hit_statistics_.Missed();
    }

    void RemoteCacheStatistics::SetCurrentProcessingTasksCount(size_t tasks_in_progress)
    {
        tasks_in_progress_statistics_.Add(tasks_in_progress);
    }

    void RemoteCacheStatistics::OnDownloadSucceeded(const std::map<size_t, boost::shared_ptr<BlockData> >& blocks)
    {
        for(std::map<size_t, boost::shared_ptr<BlockData> >::const_iterator iter = blocks.begin();
            iter != blocks.end();
            ++iter)
        {
            if (iter->second)
            {
                bytes_downloaded_ += iter->second->Size();
            }
        }

        download_statistics_.Hit();
    }

    void RemoteCacheStatistics::OnDownloadCompleted(size_t download_time_in_ms)
    {
        download_time_statistics_.Add(download_time_in_ms);
    }

    void RemoteCacheStatistics::ReportStatus()
    {
        average_download_speed_->SetStatus(bytes_downloaded_/(1024*1024*RemoteCacheStatisticsUpdateIntervalInSeconds));
        average_download_queue_length_->SetStatus(queue_statistics_.AverageQueueSize());
        average_concurrent_tasks_->SetStatus(tasks_in_progress_statistics_.Average());
        dropped_requests_rate_->SetStatus(queue_statistics_.DropRate());
        content_server_hit_rate_->SetStatus(resource_hit_statistics_.HitRate());
        download_failure_rate_->SetStatus(100 - download_statistics_.HitRate());
        average_wait_time_before_download_->SetStatus(wait_time_before_download_statistics_.Average());
        average_download_time_->SetStatus(download_time_statistics_.Average());
    }

    void RemoteCacheStatistics::OnTimerElapsed()
    {
        ReportStatus();
        ResetCounters();

        if (timer_)
        {
            timer_->expires_from_now(boost::posix_time::seconds(RemoteCacheStatisticsUpdateIntervalInSeconds));
            timer_->async_wait(boost::bind(&RemoteCacheStatistics::OnTimerElapsed, shared_from_this()));
        }
    }
}