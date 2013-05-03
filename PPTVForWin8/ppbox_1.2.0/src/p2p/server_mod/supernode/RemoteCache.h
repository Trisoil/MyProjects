//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_REMOTE_CACHE_H
#define SUPER_NODE_REMOTE_CACHE_H

#include "SuperNodeServiceStruct.h"
#include "RemoteCache/DownloadProgressListener.h"
#include "RemoteCache/ResourceChangeListener.h"
#include "RemoteCache/DownloadTask.h"
#include "PriorityTasksQueue.h"

namespace super_node
{
    typedef boost::function<
        void (ErrorCode, const std::map<size_t, boost::shared_ptr<BlockData> >&)
    > ReadResourceCallback;

    class ContentServer;
    class ServerListAsker;
    class IContentServerResourceManager;
    class IServerListAsker;
    class RemoteCacheStatistics;

    class IRemoteCache
    {
    public:
        virtual void AsyncReadResource(const ResourceIdentifier& resource_identifier, size_t start_block_index, size_t num_of_blocks, ReadResourceCallback callback) = 0;
        virtual ~IRemoteCache(){}
    };

    class RemoteCache
        : public IRemoteCache,
        public boost::enable_shared_from_this<RemoteCache>,
        public IDownloadProgressListener,
        public count_object_allocate<RemoteCache>
    {
    public:
        RemoteCache(boost::shared_ptr<boost::asio::io_service> io_service, const RemoteCacheConfig & config, boost::shared_ptr<IContentServerResourceManager> content_server_resource_manager);

        void Start();
        void Stop();
        void AsyncReadResource(const ResourceIdentifier& resource_identifier, size_t start_block_index, size_t num_of_blocks, ReadResourceCallback callback);
        void UpdateConfig();

    private:
        void CreateDownloadTask(const ResourceIdentifier & resource_identifier, size_t start_block_index, size_t num_of_blocks, ReadResourceCallback callback);
        void OnDownloadTaskCompleted(DownloadTask & task, bool succeed);
        void ProcessDownloadTaskIfApproprite();
        static bool TryParseResourceNameAndSegmentIndex(const std::string & resource_name_and_segment_index, std::string & resource_name, size_t & segment_index);
        void OnReadResourceCallback(ReadResourceCallback callback, ErrorCode error_code, const std::map<size_t, boost::shared_ptr<BlockData> >& blocks);

    private:
        RemoteCacheConfig config_;
        boost::shared_ptr<boost::asio::io_service> io_service_;
        PriorityTasksQueue<DownloadTask> pending_tasks_;
        boost::shared_ptr<IContentServerResourceManager> content_server_resource_manager_;

        size_t tasks_in_progress_;
        bool is_working_;

        boost::shared_ptr<RemoteCacheStatistics> statistics_;
    };
}

#endif //SUPER_NODE_REMOTE_CACHE_H