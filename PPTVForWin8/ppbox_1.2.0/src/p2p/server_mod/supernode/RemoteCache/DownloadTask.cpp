//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "DownloadTask.h"
#include "ContentServer.h"
#include "DownloadProgressListener.h"

namespace super_node
{
    void DownloadTask::Execute()
    {
        wait_time_ = timer_.elapse();
        my_server_->AsyncDownload(resource_name_, segment_index_, start_block_index_, num_of_blocks_, shared_from_this());
    }

    const boost::uint32_t & DownloadTask::GetWaitTime() const
    {
        return wait_time_;
    }
    
    const boost::uint32_t & DownloadTask::GetDownloadTime()const 
    {
        return download_time_;
    }

    int DownloadTask::TaskPriority()const
    {
        return priority_;
    }

    void DownloadTask::Cancel()
    {
        call_back_to_local_disk_cache_(ErrorCodes::ServiceBusy, std::map<size_t, boost::shared_ptr<BlockData> >());
    }

    void DownloadTask::HandleDownloadResult(const boost::system::error_code& err, const std::map<size_t, boost::shared_ptr<BlockData> > & blocks)
    {
        assert(timer_.elapse() >= wait_time_);
        download_time_ = timer_.elapse() - wait_time_;
        if (err)
        {
            progress_listener_->OnDownloadTaskCompleted(*this, false);
            call_back_to_local_disk_cache_(ErrorCodes::DownloadFailure, std::map<size_t, boost::shared_ptr<BlockData> >());
        }
        else
        {
            progress_listener_->OnDownloadTaskCompleted(*this, true);
            call_back_to_local_disk_cache_(ErrorCodes::Success, blocks);
        }
    }
}
