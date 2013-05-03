//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef LIVE_MEDIA_REMOTE_CACHE_H
#define LIVE_MEDIA_REMOTE_CACHE_H

#include "RemoteCache/DownloadProgressListener.h"
#include "RemoteCache/DownloadTask.h"
#include "PriorityTasksQueue.h"
#include "BlockCache.h"
#include "Configuration.h"

namespace live_media
{
    class BlockDataDownloader;
    class ChannelManager;

    class IRemoteCache
    {
    public:
        virtual void AsyncReadBlock(const channel_id& channel_identifier, size_t block_index, size_t priority, ReadBlockCallback callback) = 0;
        virtual void Start(){}
        virtual void Stop(){}
        virtual void UpdateConfiguration(const DownloadConfiguration& download_configuration){}
        virtual ~IRemoteCache(){}
    };

    class RemoteCache
        : public IRemoteCache,
        public boost::enable_shared_from_this<RemoteCache>,
        public IDownloadProgressListener
    {
    public:
        RemoteCache(
            boost::shared_ptr<boost::asio::io_service> io_service, 
            boost::shared_ptr<ChannelManager> channel_manager,
            const DownloadConfiguration& download_configuration,
            boost::shared_ptr<IBlockDataDownloader> block_downloader=boost::shared_ptr<IBlockDataDownloader>());

        void Start();
        void Stop();
        void UpdateConfiguration(const DownloadConfiguration& download_configuration);
        void AsyncReadBlock(const channel_id& channel_identifier, size_t block_index, size_t priority, ReadBlockCallback callback);

    private:
        void CreateDownloadTask(const channel_id & resource_identifier, size_t block_index, size_t priority, const ChannelConfiguration& channel_config, ReadBlockCallback callback);
        void OnDownloadTaskCompleted(DownloadTask & task, bool succeed);
        void ProcessDownloadTaskIfApproprite();
        void OnReadBlockCallback(ReadBlockCallback callback, const channel_id& channel_identifier, size_t block_index, ErrorCode error_code, boost::shared_ptr<BlockData> block_data);
        void OnConfigurationUpdated(const DownloadConfiguration& download_configuration);

    private:
        boost::shared_ptr<boost::asio::io_service> io_service_;
        PriorityTasksQueue<DownloadTask> pending_tasks_;
        boost::shared_ptr<IBlockDataDownloader> block_downloader_;
        DownloadConfiguration download_configuration_;
        size_t tasks_in_progress_;
        bool is_working_;
        boost::shared_ptr<ChannelManager> channel_manager_;
    };
}

#endif //LIVE_MEDIA_REMOTE_CACHE_H