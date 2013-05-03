//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "pre.h"
#include "DownloadTask.h"
#include "BlockDataDownloader.h"
#include "DownloadProgressListener.h"

namespace live_media
{
    void DownloadTask::Execute()
    {
        wait_time_ = timer_.elapse();
        block_downloader_->AsyncDownload(channel_identifier_, block_index_, timeout_in_seconds_, channel_config_, shared_from_this());
    }

    const boost::uint32_t DownloadTask::GetWaitTime() const
    {
        return wait_time_;
    }
    
    const boost::uint32_t DownloadTask::GetDownloadTime()const 
    {
        return download_time_;
    }

    int DownloadTask::TaskPriority()const
    {
        return priority_;
    }

    void DownloadTask::Cancel()
    {
        call_back_to_local_disk_cache_(ErrorCodes::ServiceBusy, boost::shared_ptr<BlockData>());
    }

    void DownloadTask::HandleDownloadResult(const boost::system::error_code& err, boost::shared_ptr<BlockData> block)
    {
        assert(timer_.elapse() > wait_time_);
        download_time_ = timer_.elapse() - wait_time_;

        bool succeeded = false;
        if (!err && block)
        {
            if (block->IsValid())
            {
                succeeded = true;
            }
            else
            {
                LOG4CPLUS_WARN(Loggers::RemoteCache(), "downloaded block data is invalid and will be ignored.");
            }
        }

        if (!succeeded && block)
        {
            block.reset();
        }

        progress_listener_->OnDownloadTaskCompleted(*this, succeeded);
        call_back_to_local_disk_cache_(succeeded ? ErrorCodes::Success : ErrorCodes::DownloadFailure, block);
    }
}