//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _SN_DOWNLOAd_TASK_H_
#define _SN_DOWNLOAd_TASK_H_

#include "framework/timer/TimeCounter.h"
#include "DownloadResultHandler.h"

namespace super_node
{
    class IContentServer;
    class ErrorCode;
    class BlockData;
    class IDownloadProgressListener;

    typedef boost::function<
        void (ErrorCode, const std::map<size_t, boost::shared_ptr<BlockData> > & blockes)
        > ReadFromRemoteCacheCallBack;

    class DownloadTask
        :public IDownloadResultHandler,
        public boost::enable_shared_from_this<DownloadTask>
        , public count_object_allocate<DownloadTask>
    {
    public:
        DownloadTask(std::string resource_name, size_t segment_index, size_t start_block_index, size_t num_of_blocks_to_download, boost::shared_ptr<IContentServer> content_server,
             boost::shared_ptr<IDownloadProgressListener> progress_listener, ReadFromRemoteCacheCallBack call_back, int priority = 0)
             :resource_name_(resource_name), segment_index_(segment_index), my_server_(content_server), progress_listener_(progress_listener),
             start_block_index_(start_block_index), num_of_blocks_(num_of_blocks_to_download),
             call_back_to_local_disk_cache_(call_back)
        {
            wait_time_ = 0;
            download_time_ = 0;
        }

        void Execute();
        void Cancel();
        void HandleDownloadResult(const boost::system::error_code& err, const std::map<size_t, boost::shared_ptr<BlockData> > & blocks);
        int TaskPriority()const;
        

        const boost::uint32_t & GetWaitTime()const;
        const boost::uint32_t & GetDownloadTime()const;
       

    private:
        std::string resource_name_;
        size_t segment_index_;
        size_t start_block_index_;
        size_t num_of_blocks_;
        boost::shared_ptr<IContentServer> my_server_;
        boost::shared_ptr<IDownloadProgressListener> progress_listener_;
        ReadFromRemoteCacheCallBack call_back_to_local_disk_cache_;
        int priority_;
        boost::uint32_t wait_time_;
        boost::uint32_t download_time_;
        framework::timer::TimeCounter timer_;
    };
}

#endif