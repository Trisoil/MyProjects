//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_REMOTE_CACHE_STATISTICS_H
#define SUPER_NODE_REMOTE_CACHE_STATISTICS_H

#include "QueueStatistics.h"
#include "HitStatistics.h"

namespace super_node
{
    class ComponentStatusItem;
    class BlockData;

    class RemoteCacheStatistics:
        public boost::enable_shared_from_this<RemoteCacheStatistics>
        ,public count_object_allocate<RemoteCacheStatistics>
    {
    public:
        RemoteCacheStatistics();
        void Start(boost::shared_ptr<boost::asio::io_service> io_service);
        void Stop();

        void OnRequestEnqueued();
        void OnRequestDropped();
        void OnDownloadFailed();
        void OnDownloadSucceeded(const std::map<size_t, boost::shared_ptr<BlockData> >& blocks);
        void SetCurrentDownloadQueueLength(size_t queue_length);
        void OnResourceNotFound();
        void SetCurrentProcessingTasksCount(size_t tasks_in_progress);
        void OnDownloadStarted(size_t wait_time_in_queue_in_ms);
        void OnDownloadCompleted(size_t download_time_in_ms);
    private:
        void ResetCounters();
        void ReportStatus();
        void OnTimerElapsed();
    private:
        static const size_t RemoteCacheStatisticsUpdateIntervalInSeconds;

        boost::shared_ptr<boost::asio::deadline_timer> timer_;

        size_t bytes_downloaded_;
        QueueStatistics queue_statistics_;
        NumericStatistics tasks_in_progress_statistics_;
        NumericStatistics wait_time_before_download_statistics_;
        NumericStatistics download_time_statistics_;

        HitStatistics download_statistics_;
        HitStatistics resource_hit_statistics_;

        boost::shared_ptr<ComponentStatusItem> average_download_speed_;
        boost::shared_ptr<ComponentStatusItem> dropped_requests_rate_;
        boost::shared_ptr<ComponentStatusItem> content_server_hit_rate_;
        boost::shared_ptr<ComponentStatusItem> download_failure_rate_;
        boost::shared_ptr<ComponentStatusItem> average_download_queue_length_;
        boost::shared_ptr<ComponentStatusItem> average_wait_time_before_download_;
        boost::shared_ptr<ComponentStatusItem> average_concurrent_tasks_;
        boost::shared_ptr<ComponentStatusItem> average_download_time_;
    };
}

#endif //SUPER_NODE_REMOTE_CACHE_STATISTICS_H