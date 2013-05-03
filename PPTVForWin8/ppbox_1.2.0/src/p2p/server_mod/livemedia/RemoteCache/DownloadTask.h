//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _SN_DOWNLOAd_TASK_H_
#define _SN_DOWNLOAd_TASK_H_

#include "framework/timer/TimeCounter.h"
#include "DownloadResultHandler.h"
#include "Configuration.h"

namespace live_media
{
    class IBlockDataDownloader;
    class ErrorCode;
    class BlockData;
    class IDownloadProgressListener;

    typedef boost::function<
        void (ErrorCode, boost::shared_ptr<BlockData> block)
        > ReadFromRemoteCacheCallBack;

    class DownloadTask
        :public IDownloadResultHandler,
        public boost::enable_shared_from_this<DownloadTask>
    {
    public:
        DownloadTask(const channel_id& channel_identifier, size_t block_index, const ChannelConfiguration& channel_config, boost::shared_ptr<IBlockDataDownloader> block_downloader,
             boost::shared_ptr<IDownloadProgressListener> progress_listener, size_t timeout_in_seconds, ReadFromRemoteCacheCallBack call_back, int priority = 0)
             :channel_identifier_(channel_identifier), block_index_(block_index), block_downloader_(block_downloader), progress_listener_(progress_listener),
             channel_config_(channel_config),
             call_back_to_local_disk_cache_(call_back)
        {
            priority_ = priority;
            timeout_in_seconds_ = timeout_in_seconds;
            wait_time_ = 0;
            download_time_ = 0;
        }

        void Execute();
        void Cancel();
        void HandleDownloadResult(const boost::system::error_code& err, boost::shared_ptr<BlockData> block);
        int TaskPriority()const;
        

        const boost::uint32_t GetWaitTime()const;
        const boost::uint32_t GetDownloadTime()const;
       
    private:
        channel_id channel_identifier_;
        size_t block_index_;
        boost::shared_ptr<IBlockDataDownloader> block_downloader_;
        boost::shared_ptr<IDownloadProgressListener> progress_listener_;
        ReadFromRemoteCacheCallBack call_back_to_local_disk_cache_;
        int priority_;
        boost::uint32_t wait_time_;
        size_t timeout_in_seconds_;
        boost::uint32_t download_time_;
        framework::timer::TimeCounter timer_;
        ChannelConfiguration channel_config_;
    };
}

#endif