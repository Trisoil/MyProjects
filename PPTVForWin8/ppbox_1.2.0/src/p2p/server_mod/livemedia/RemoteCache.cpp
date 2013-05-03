//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "pre.h"
#include "RemoteCache.h"
#include "RemoteCache/BlockDataDownloader.h"
#include "channel_manager.h"

namespace live_media
{
    RemoteCache::RemoteCache(
        boost::shared_ptr<boost::asio::io_service> io_service, 
        boost::shared_ptr<ChannelManager> channel_manager,
        const DownloadConfiguration& download_configuration,
        boost::shared_ptr<IBlockDataDownloader> block_downloader)
        :io_service_(io_service), channel_manager_(channel_manager), download_configuration_(download_configuration)
    {
        if (block_downloader)
        {
            block_downloader_ = block_downloader;
        }
        else
        {
            block_downloader_.reset(new BlockDataDownloader(io_service_));
        }

        tasks_in_progress_ = 0;
        is_working_ = false;
    }

    void RemoteCache::Start()
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::RemoteCache(), "RemoteCache::Start()");

        block_downloader_->Start();
        is_working_ = true;
        LOG4CPLUS_INFO(Loggers::RemoteCache(), "RemoteCache started.");
    }

    void RemoteCache::Stop()
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::RemoteCache(), "RemoteCache::Stop()");

        is_working_ = false;
        block_downloader_->Stop();

        LOG4CPLUS_INFO(Loggers::RemoteCache(), "RemoteCache Stopped.");
    }

    void RemoteCache::UpdateConfiguration(const DownloadConfiguration& download_configuration)
    {
        io_service_->post(bind(&RemoteCache::OnConfigurationUpdated, shared_from_this(), download_configuration));
    }

    void RemoteCache::OnConfigurationUpdated(const DownloadConfiguration& download_configuration)
    {
        download_configuration_ = download_configuration;
    }

    void RemoteCache::AsyncReadBlock(const channel_id& channel_identifier, size_t block_index, size_t priority, ReadBlockCallback callback)
    {
        ReadBlockCallback wrapped_callback = bind(
            &RemoteCache::OnReadBlockCallback,
            shared_from_this(),
            callback,
            _2,
            _3, 
            _1, 
            _4);

        ChannelPointer target_channel = channel_manager_->GetChannel(channel_identifier);
        if (!target_channel)
        {
            callback(ErrorCodes::ResourceNotFound, channel_identifier, block_index, boost::shared_ptr<BlockData>());
            return;
        }

        ChannelConfiguration channel_config(target_channel->channel_config_);

        io_service_->post(
            bind(
                &RemoteCache::CreateDownloadTask, 
                shared_from_this(), 
                channel_identifier, 
                block_index,
                priority,
                channel_config,
                wrapped_callback));
    }

    void RemoteCache::OnReadBlockCallback(ReadBlockCallback callback, const channel_id& channel_identifier, size_t block_index, ErrorCode error_code, boost::shared_ptr<BlockData> block_data)
    {
        callback(error_code, channel_identifier, block_index, block_data);
    }

    void RemoteCache::CreateDownloadTask(const channel_id & channel_identifier, size_t block_index, size_t priority, const ChannelConfiguration& channel_config, ReadBlockCallback callback)
    {
        if (!is_working_)
        {
            callback(ErrorCodes::ServiceStopped, channel_identifier, block_index, boost::shared_ptr<BlockData>());
            return;
        }

        boost::shared_ptr<DownloadTask> task_to_process;
        task_to_process.reset(
            new DownloadTask(
                channel_identifier, 
                block_index, 
                channel_config, 
                block_downloader_, 
                shared_from_this(), 
                download_configuration_.download_timeout_in_seconds_,
                boost::bind(&RemoteCache::OnReadBlockCallback, shared_from_this(), callback, channel_identifier, block_index, _1, _2), 
                priority));

        pending_tasks_.Push(task_to_process);
        ProcessDownloadTaskIfApproprite();
    }

    void RemoteCache::OnDownloadTaskCompleted(DownloadTask & task, bool succeed)
    {
        assert(tasks_in_progress_ >= 0);

        if (tasks_in_progress_ > 0)
        {
            --tasks_in_progress_; 
        }

        ProcessDownloadTaskIfApproprite();
    }

    void RemoteCache::ProcessDownloadTaskIfApproprite()
    {
        if (!is_working_)
        {
            return ;
        }
        
        while (tasks_in_progress_ < download_configuration_.max_concurrent_download_tasks_)
        {
            if (pending_tasks_.Empty())
            {
                break;
            }
            boost::shared_ptr<DownloadTask> task_to_process = pending_tasks_.PopFront();
            task_to_process->Execute();
            ++tasks_in_progress_;
        }
        
        while (pending_tasks_.Size() > download_configuration_.download_task_queue_size_)
        {
            boost::shared_ptr<DownloadTask> task_to_cancel = pending_tasks_.PopBack();
            task_to_cancel->Cancel();
        }
    }
}