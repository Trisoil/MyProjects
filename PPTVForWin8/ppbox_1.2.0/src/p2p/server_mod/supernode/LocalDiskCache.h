//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_LOCAL_DISK_CACHE_H
#define SUPER_NODE_LOCAL_DISK_CACHE_H

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include "LocalDiskCache/DiskResourcesManagement.h"
#include "LocalDiskCache/PendingBlockReadRequestsManagement.h"
#include "LocalDiskCache/RecentResourceWriteAttempts.h"

namespace super_node
{
    class BlockData;
    class DiskResource;
    class ILocalDisk;
    class IRemoteCache;
    class LocalDiskCacheStatistics;

    typedef boost::function<
        void (const ErrorCode&, std::vector<boost::shared_ptr<DiskResource> >&)
    > LoadResourcesTaskCallback;

    typedef boost::function<
        void (const ErrorCode&, const std::vector<ResourceIdentifier>&)
    > DeleteResourcesTaskCallback;

    typedef boost::function<
        void (ErrorCode, const ResourceIdentifier&)
    > WriteResourceTaskCallback;

    class ILocalDiskCache
    {
    public:
        virtual void AsyncReadBlock(const ResourceIdentifier& resource_identifier, size_t block_index, int read_priority, ReadResourceBlockCallback callback) = 0;
        virtual ~ILocalDiskCache(){}
    };

    class LocalDiskCache
        : public ILocalDiskCache,
        public boost::enable_shared_from_this<LocalDiskCache>,
        public count_object_allocate<LocalDiskCache>
    {
    public:
        class LocalDiskCacheSettings
        {
        public:
            LocalDiskCacheSettings()
            {
                Initialize();
            }

            LocalDiskCacheSettings(const std::vector<string>& local_disks_path)
                : local_disks_path_(local_disks_path)
            {
                Initialize();
            }

            void GetDisksPath(std::vector<string>& local_disks_path) const 
            { 
                local_disks_path = local_disks_path_; 
            }

            void LoadFromConfig();

            size_t GetMinimumFreeDiskSpacePercentage() const { return minimum_free_disk_space_percentage_; }
            size_t GetDesiredFreeDiskSpacePercentage() const { return desired_free_disk_space_percentage_; }
            size_t GetMinimumResourcesToDelete() const { return minimum_resources_to_delete_; }
            size_t GetMaximumBlocksToDownloadInBatch() const { return max_blocks_to_download_in_batch_; }
            size_t GetPopularResourceRequestsCount() const { return popular_resource_requests_count_; }

            size_t GetMaximumRemoteResourcesToCache() const { return max_remote_resources_to_cache_; }

            bool DeleteResourceOnError() const { return delete_resource_on_error_; }

        private:
            void Initialize()
            {
                minimum_free_disk_space_percentage_ = 20;
                desired_free_disk_space_percentage_ = 25;
                minimum_resources_to_delete_ = 100;
                max_blocks_to_download_in_batch_ = 2;
                delete_resource_on_error_ = false;
                popular_resource_requests_count_ = ResourceWriteAttemptRecord::DefaultPopularResourceRequestsCount;
                max_remote_resources_to_cache_ = 512;
            }

            std::vector<string> local_disks_path_;
            size_t minimum_free_disk_space_percentage_;
            size_t desired_free_disk_space_percentage_;
            size_t minimum_resources_to_delete_;
            size_t max_blocks_to_download_in_batch_;
            size_t popular_resource_requests_count_;
            bool delete_resource_on_error_;
            size_t max_remote_resources_to_cache_;
        };

    protected:
        class RemoteResourceCache
        {
        public:
            RemoteResourceCache()
                : max_blocks_to_cache_(512)
            { }

            void Insert(const ResourceIdentifier& rid, size_t block_id, boost::shared_ptr<BlockData> block);
            void Insert(const ResourceIdentifier& rid, const std::map<size_t, boost::shared_ptr<BlockData> >& blocks);
            void Erase(const ResourceIdentifier& rid, size_t block_id);

            boost::shared_ptr<BlockData> GetBlock(const ResourceIdentifier& rid, size_t block_id);
            void GetBlocks(const ResourceIdentifier& rid, std::map<size_t, boost::shared_ptr<BlockData> >& blocks);

            void SetCacheSize(size_t max_blocks_to_cache)
            {
                max_blocks_to_cache_ = max_blocks_to_cache;
            }

        private:
            typedef std::pair<ResourceIdentifier, size_t>  BlockIdentifier;
            typedef std::map<BlockIdentifier, std::pair<boost::shared_ptr<BlockData>, std::list<BlockIdentifier>::iterator> >::iterator block_iterator;

            size_t max_blocks_to_cache_;
            std::map<BlockIdentifier, std::pair<boost::shared_ptr<BlockData>, std::list<BlockIdentifier>::iterator> > block_map_;
            std::list<BlockIdentifier> come_order_queue_;
        };

    public:
        LocalDiskCache(boost::shared_ptr<IRemoteCache> remote_cache, boost::shared_ptr<boost::asio::io_service> io_service);
        
        //called from console thread
        void Start();
        void Stop();

        void AsyncReadBlock(const ResourceIdentifier& resource_identifier, size_t block_index, int read_priority, ReadResourceBlockCallback callback);

        //for unit-testability
        void Start(const LocalDiskCacheSettings& settings);

        void UpdateConfig();

     protected:
        //for unit-testability
        virtual boost::shared_ptr<ILocalDisk> CreateLocalDisk(const string& disk_path);

    private:
        void HandleAsyncDiskReadCompleted(const ResourceIdentifier& resource_identifier, int block_index, int read_priority, ReadResourceBlockCallback callback, ErrorCode error_code, boost::shared_ptr<BlockData> block_data);

        //handler called from local disk thread
        void HandleLocalDiskLoadCompleted(size_t disk_index, const ErrorCode& error_code, std::vector<boost::shared_ptr<DiskResource> >& resources);

        void HandleAsyncWriteResource(ErrorCode error_code, const ResourceIdentifier& resource_identifier, size_t disk_index, const std::map<size_t, boost::shared_ptr<BlockData> >& blocks);
        void HandleAsyncRemoteCacheReadCompleted(const ResourceIdentifier& resource_identifier, ErrorCode error_code, const std::map<size_t, boost::shared_ptr<BlockData> >& blocks, size_t start_block_index, size_t num_of_blocks, bool attempt_disk_write_upon_success);
        void HandleAsyncDeleteResources(ErrorCode error_code, const std::vector<ResourceIdentifier>& resources);
        
        void DeleteResourcesIfApplicable(size_t disk_index);
        
        void AsyncWriteResource(const ResourceIdentifier& resource_identifier, const std::map<size_t, boost::shared_ptr<BlockData> >& blocks);

        bool TryReadBlockLocally(const ResourceIdentifier& resource_identifier, size_t block_index, int read_priority, ReadResourceBlockCallback callback);

        void OnTimerElapsed();

        void StartLocalDisks();
        void ResetLocalDisks(const std::vector<std::string>& paths);
        void ClearLocalDisksAndRestart();

        void CancelPendingReadRequests();

        size_t GetConsecutiveMissingBlocks(const ResourceIdentifier& resource_identifier, size_t start_block_index, size_t max_num_of_blocks) const;

        void UpdateStableState();

        bool BypassDisk() { return local_disks_.empty(); }

        // attention: the wrapped callback will alter the remote_resources_cache_, and should be careful if it's invoked in a loop of remote_resources_cache_
        void AsyncReadCallbackWrapper(ReadResourceBlockCallback callback, const ResourceIdentifier& resource_identifier, size_t block_index, ErrorCode, boost::shared_ptr<BlockData>);

    private:
        bool running_;
        LocalDiskCacheSettings settings_;

        boost::shared_ptr<IRemoteCache> remote_cache_;
        //after a resource is read from a remote cache, and before it's written to disk, it's cached here
        RemoteResourceCache remote_resource_cache_;

        bool local_disk_is_resetting_;
        std::vector<boost::shared_ptr<ILocalDisk> > local_disks_;
        PendingBlockReadRequestsManagement pending_block_read_requests_;
        RecentResourceWriteAttempts recent_resource_write_attempts_;
        DiskResourcesManagement disk_resources_;

        boost::shared_ptr<LocalDiskCacheStatistics> statistics_;

        boost::shared_ptr<boost::asio::io_service> io_service_;

        size_t pending_disk_deletion_tasks_;
        size_t num_of_disks_load_completed_;
        boost::mutex mutex_;
        boost::condition condition_;

        boost::asio::deadline_timer timer_;
        size_t elapsed_minutes_count_;

        std::vector<std::string> disk_paths_;
        std::vector<std::string> new_disk_paths_;

        bool disks_in_stable_state_;
    };
}

#endif //SUPER_NODE_LOCAL_DISK_CACHE_H
