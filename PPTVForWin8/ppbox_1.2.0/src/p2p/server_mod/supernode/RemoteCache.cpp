//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "RemoteCache.h"
#include "RemoteCache/ContentServer.h"
#include "RemoteCache/ServerListAsker.h"
#include "RemoteCache/ContentServerResourceManager.h"
#include "Statistics/RemoteCacheStatistics.h"

namespace super_node
{
    RemoteCache::RemoteCache(boost::shared_ptr<boost::asio::io_service> io_service, const RemoteCacheConfig & config, boost::shared_ptr<IContentServerResourceManager> content_server_resource_manager)
        :io_service_(io_service), config_(config), content_server_resource_manager_(content_server_resource_manager)
    {
        tasks_in_progress_ = 0;
        is_working_ = false;
        statistics_.reset(new RemoteCacheStatistics());
    }

    void RemoteCache::Start()
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::RemoteCache(), "RemoteCache::Start()");
        content_server_resource_manager_->Start();

        statistics_->Start(io_service_);

        is_working_ = true;
        LOG4CPLUS_INFO(Loggers::RemoteCache(), "RemoteCache started.");
    }

    void RemoteCache::Stop()
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::RemoteCache(), "RemoteCache::Stop()");
        is_working_ = false;
        content_server_resource_manager_->Stop();

        statistics_->Stop();

        LOG4CPLUS_INFO(Loggers::RemoteCache(), "RemoteCache Stopped.");
    }

    void RemoteCache::AsyncReadResource(const ResourceIdentifier& resource_identifier, size_t start_block_index, size_t num_of_blocks, ReadResourceCallback callback)
    {
        ReadResourceCallback wrapped_callback = bind(
            &RemoteCache::OnReadResourceCallback,
            shared_from_this(),
            callback,
            _1,
            _2);

        io_service_->post(
            bind(
                &RemoteCache::CreateDownloadTask, 
                shared_from_this(), 
                resource_identifier, 
                start_block_index,
                num_of_blocks,
                wrapped_callback));
    }

    void RemoteCache::OnReadResourceCallback(ReadResourceCallback callback, ErrorCode error_code, const std::map<size_t, boost::shared_ptr<BlockData> >& blocks)
    {
        if (error_code == ErrorCodes::ServiceBusy || 
            error_code == ErrorCodes::ServiceStopped)
        {
            statistics_->OnRequestDropped();
        }
        else if (error_code == ErrorCodes::ResourceNotFound)
        {
            statistics_->OnResourceNotFound();
        }
        else if (error_code == ErrorCodes::DownloadFailure)
        {
            statistics_->OnDownloadFailed();
        }
        else if (error_code == ErrorCodes::Success)
        {
            statistics_->OnDownloadSucceeded(blocks);
        }
        else
        {
            assert(false);
        }

        callback(error_code, blocks);
    }

    void RemoteCache::CreateDownloadTask(const ResourceIdentifier & resource_identifier, size_t start_block_index, size_t num_of_blocks, ReadResourceCallback callback)
    {
        if (!is_working_)
        {
            callback(ErrorCodes::ServiceStopped, std::map<size_t, boost::shared_ptr<BlockData> > ());
            return;
        }

        std::string resource_name;
        size_t segment_index;
        if (TryParseResourceNameAndSegmentIndex(resource_identifier.GetResourceName(), resource_name, segment_index))
        {
            boost::shared_ptr<IContentServer> content_server = content_server_resource_manager_->FindContentServer(resource_name);
            if (content_server)
            {
                boost::shared_ptr<DownloadTask> task_to_process;
                task_to_process.reset(new DownloadTask(resource_name, segment_index, start_block_index, num_of_blocks, content_server, shared_from_this(), callback));
                pending_tasks_.Push(task_to_process);
                statistics_->OnRequestEnqueued();
                ProcessDownloadTaskIfApproprite();
            }
            else
            {
                callback(ErrorCodes::ResourceNotFound, std::map<size_t, boost::shared_ptr<BlockData> > ());
                LOG4CPLUS_INFO(Loggers::RemoteCache(), "resource "<<resource_name<<" is not found on any CDN");
            }
        }
        else
        {
            LOG4CPLUS_WARN(Loggers::RemoteCache(), "Try parse resource name and segment index fail, " << resource_identifier.GetResourceName() << "may be illegal");
            callback(ErrorCodes::ResourceNotFound, std::map<size_t, boost::shared_ptr<BlockData> > ());
        }
    }

    void RemoteCache::OnDownloadTaskCompleted(DownloadTask & task, bool succeed)
    {
        assert(tasks_in_progress_ >= 0);

        if (tasks_in_progress_ > 0)
        {
            --tasks_in_progress_; 
        }
        
        if (succeed)
        {
            statistics_->OnDownloadCompleted(task.GetDownloadTime());
        }

        ProcessDownloadTaskIfApproprite();
    }

    void RemoteCache::ProcessDownloadTaskIfApproprite()
    {
        if (!is_working_)
        {
            return ;
        }
        
        statistics_->SetCurrentProcessingTasksCount(tasks_in_progress_);
        statistics_->SetCurrentDownloadQueueLength(pending_tasks_.Size());

        while (tasks_in_progress_ < config_.max_concurrent_download_tasks_)
        {
            if (pending_tasks_.Empty())
            {
                break;
            }
            boost::shared_ptr<DownloadTask> task_to_process = pending_tasks_.PopFront();
            statistics_->OnDownloadStarted(task_to_process->GetWaitTime());
            task_to_process->Execute();
            ++tasks_in_progress_;
        }
        
        while (pending_tasks_.Size() > config_.max_queue_size_)
        {
            boost::shared_ptr<DownloadTask> task_to_cancel = pending_tasks_.PopBack();
            task_to_cancel->Cancel();
        }
 
    }

    bool RemoteCache::TryParseResourceNameAndSegmentIndex(const std::string & resource_name_and_segment_index, std::string & resource_name, size_t & segment_index)
    {
        resource_name = resource_name_and_segment_index;
        segment_index = 0;
        if (resource_name_and_segment_index.size() <= 3)
        {
            return false;
        }

        string::size_type left_pos = resource_name_and_segment_index.find_last_of('[');
        string::size_type right_pos = resource_name_and_segment_index.find_last_of(']');
        if (left_pos == string::npos ||
            right_pos == string::npos ||
            left_pos + 2 > right_pos)
        {
            return false;
        }

        std::stringstream stream(resource_name_and_segment_index.substr(left_pos+1, right_pos - left_pos - 1));
        size_t num(0);
        if (stream >> num)
        {
            resource_name = resource_name_and_segment_index.substr(0, left_pos)+resource_name_and_segment_index.substr(right_pos+1);
            segment_index = num;
            return true;
        }

        return false;
    }

    void RemoteCache::UpdateConfig()
    {
        config_.LoadConfig();
        content_server_resource_manager_->UpdateConfig(config_);
    }
}
