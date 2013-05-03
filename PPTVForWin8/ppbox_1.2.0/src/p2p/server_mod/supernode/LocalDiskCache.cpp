//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "LocalDiskCache.h"
#include "LocalDiskCache/LocalDisk.h"
#include "LocalDiskCache/DiskSelectionStrategy.h"
#include "Statistics/LocalDiskCacheStatistics.h"
#include "RemoteCache.h"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <framework/configure/Config.h>
#include "ConfigManager/ConfigManager.h"

#ifndef BOOST_WINDOWS_API
#include "LocalDiskCache/BigFileMode/BigFileLocalDiskManager.h"
#endif

namespace super_node
{
    LocalDiskCache::LocalDiskCache(boost::shared_ptr<IRemoteCache> remote_cache, boost::shared_ptr<boost::asio::io_service> io_service)
        : remote_cache_(remote_cache), io_service_(io_service), timer_(*io_service)
    {
        pending_disk_deletion_tasks_ = 0;
        num_of_disks_load_completed_ = 0;
        elapsed_minutes_count_ = 0;
        disks_in_stable_state_ = false;
        running_ = false;
        local_disk_is_resetting_ = false;

        statistics_.reset(new LocalDiskCacheStatistics());
    }

    boost::shared_ptr<ILocalDisk> LocalDiskCache::CreateLocalDisk(const string& disk_path)
    {
        return boost::shared_ptr<ILocalDisk>(super_node::CreateLocalDisk(disk_path));
    }

    void LocalDiskCache::Start()
    {
        LocalDiskCacheSettings settings;
        settings.LoadFromConfig();
        Start(settings);
    }

    void LocalDiskCache::Start(const LocalDiskCacheSettings& settings)
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::LocalDiskCache(), "LocalDiskCache::Start()");

        statistics_->Start();

        settings_ = settings;

        settings_.GetDisksPath(disk_paths_);

#ifndef BOOST_WINDOWS_API
        BigFileLocalDiskManager::Instance().Start();
#endif

        remote_resource_cache_.SetCacheSize(settings_.GetMaximumRemoteResourcesToCache());

        StartLocalDisks();

        timer_.expires_from_now(boost::posix_time::seconds(LocalDiskCacheStatistics::SecondsPerMinute));
        timer_.async_wait(boost::bind(&LocalDiskCache::OnTimerElapsed, shared_from_this()));

        running_ = true;
        recent_resource_write_attempts_.SetPopularResourceRequestsCount(settings_.GetPopularResourceRequestsCount());
        LOG4CPLUS_INFO(Loggers::LocalDiskCache(), "local disk cache started (disks count:"<<local_disks_.size()<<", resources loaded:"<<disk_resources_.GetResourcesCount()<<")");
    }

    void LocalDiskCache::StartLocalDisks()
    {
        local_disks_.clear();
        num_of_disks_load_completed_ = 0;
        pending_disk_deletion_tasks_ = 0;
        elapsed_minutes_count_ = 0;

        for(size_t i = 0; i < disk_paths_.size(); )
        {
            boost::shared_ptr<ILocalDisk> local_disk(CreateLocalDisk(disk_paths_[i]));

            if (local_disk)
            {
                local_disks_.push_back(local_disk);
                local_disk->Start();

                LOG4CPLUS_INFO(Loggers::Operations(), "Start Local Disk " << disk_paths_[i]);
                ++i;
            }
            else
            {
                LOG4CPLUS_INFO(Loggers::Operations(), "Fail to Start Local Disk " << disk_paths_[i]);
                disk_paths_.erase(disk_paths_.begin() + i);
            }
        }

        boost::mutex::scoped_lock lock(mutex_);

        for (size_t i = 0; i < local_disks_.size(); ++i)
        {
            local_disks_[i]->AsyncLoadResources(
                boost::bind(&LocalDiskCache::HandleLocalDiskLoadCompleted, shared_from_this(), i, _1, _2),
                settings_.DeleteResourceOnError()
            );
        }

        while(num_of_disks_load_completed_ < local_disks_.size())
        {
            condition_.wait(lock);
        }

        for (size_t i = 0; i < local_disks_.size(); )
        {
            if (disk_paths_[i].empty())
            {
                disk_paths_.erase(disk_paths_.begin() + i);
                local_disks_[i]->Stop();
                local_disks_.erase(local_disks_.begin() + i);
            }
            else
                ++i;
        }

        UpdateStableState();
    }

    void LocalDiskCache::HandleLocalDiskLoadCompleted(size_t disk_index, const ErrorCode& error_code, std::vector<boost::shared_ptr<DiskResource> >& resources)
    {
        boost::mutex::scoped_lock lock(mutex_);

        assert(disk_index < local_disks_.size());

        if (error_code == ErrorCodes::Success)
        {
            LOG4CPLUS_INFO(Loggers::Operations(), "Resources loaded from local disk - " << disk_paths_[disk_index] << ", " << resources.size());

            for(size_t i = 0; i < resources.size(); ++i)
            {
                disk_resources_.AddResource(disk_index, resources[i]);
            }
        }
        else
        {
            LOG4CPLUS_ERROR(Loggers::Operations(), "fail to load resources from disk: " << disk_paths_[disk_index]);
            LOG4CPLUS_WARN(Loggers::LocalDiskCache(), "An error occurred while disk " << local_disks_[disk_index]->GetDiskPath() << " is loaded.");

            disk_paths_[disk_index].clear();
        }

        ++num_of_disks_load_completed_;
        condition_.notify_one();

        LOG4CPLUS_INFO(Loggers::LocalDiskCache(), "disk " << local_disks_[disk_index]->GetDiskPath() << " is loaded (resources:"<<resources.size()<<")");
    }

    void LocalDiskCache::Stop()
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::LocalDiskCache(), "LocalDiskCache::Stop()");

        running_ = false;

        ResetLocalDisks(std::vector<std::string>());

        boost::system::error_code cancel_error;
        timer_.cancel(cancel_error);
        remote_cache_.reset();
        CancelPendingReadRequests();

        statistics_->Stop();

        LOG4CPLUS_INFO(Loggers::LocalDiskCache(), "local disk cache stopped");
    }

    void LocalDiskCache::CancelPendingReadRequests()
    {
        pending_block_read_requests_.CancelAll();
    }

    void LocalDiskCache::ResetLocalDisks(const std::vector<std::string>& paths)
    {
        // local_disk->Stop() may invoke pending callbacks[io_service_->wrap(...)], and post handlers to io_service_,
        // clearation should be done after these handlers are handled,
        //
        // so resetting is divide into two stage:
        // 1. invoke [local_disk->Stop()] for each disk, and post [ClearLocalDisksAndRestart(...)] to io_service_;
        // 2. no disk callbacks left unhandled at this stage, so do the clearation, and re-start the specified disks;

        new_disk_paths_ = paths;

        if (!local_disk_is_resetting_)
        {
            local_disk_is_resetting_ = true;

            LOG4CPLUS_INFO(Loggers::Operations(), "Reset local disks");

            for(size_t i = 0; i < local_disks_.size(); ++i)
            {
                boost::shared_ptr<ILocalDisk> local_disk = local_disks_[i];
                local_disk->Stop();
                LOG4CPLUS_INFO(Loggers::Operations(), "Stop local disk: " << disk_paths_[i]);
            }

            io_service_->post( boost::bind(&LocalDiskCache::ClearLocalDisksAndRestart, shared_from_this()) );
        }
    }

    void LocalDiskCache::ClearLocalDisksAndRestart()
    {
        local_disk_is_resetting_ = false;

        local_disks_.clear();
        disk_resources_.RemoveAllResource();
        recent_resource_write_attempts_.Clear();

        disk_paths_ = new_disk_paths_;

        if (!disk_paths_.empty())
            StartLocalDisks();
    }

    void LocalDiskCache::LocalDiskCacheSettings::LoadFromConfig()
    {
        string local_disk_list;
        framework::configure::Config conf(ConfigManager::AllConfigFileName);
        conf.register_module("LocalDiskCache")
            << CONFIG_PARAM_NAME_RDONLY("local_disk_list", local_disk_list)
            << CONFIG_PARAM_NAME_RDONLY("minimum_free_disk_space_percentage", minimum_free_disk_space_percentage_)
            << CONFIG_PARAM_NAME_RDONLY("desired_free_disk_space_percentage", desired_free_disk_space_percentage_)
            << CONFIG_PARAM_NAME_RDONLY("minimum_resources_to_delete", minimum_resources_to_delete_)
            << CONFIG_PARAM_NAME_RDONLY("max_blocks_to_download_in_batch", max_blocks_to_download_in_batch_)
            << CONFIG_PARAM_NAME_RDONLY("popular_resource_requests_count", popular_resource_requests_count_)
            << CONFIG_PARAM_NAME_RDONLY("delete_resource_on_error", delete_resource_on_error_)
            << CONFIG_PARAM_NAME_RDONLY("max_remote_resources_to_cache", max_remote_resources_to_cache_);

        if (max_blocks_to_download_in_batch_ == 0)
        {
            LOG4CPLUS_WARN(Loggers::LocalDiskCache(), "bad config setting: the value of max_blocks_to_download_in_batch ("<<max_blocks_to_download_in_batch_<<") is invalid and is reset to 2");
            max_blocks_to_download_in_batch_ = 2;
        }

        if (minimum_free_disk_space_percentage_ == 0 || minimum_free_disk_space_percentage_ >= 50)
        {
            LOG4CPLUS_WARN(Loggers::LocalDiskCache(), "bad config setting: the value of minimum_free_disk_space_percentage ("<<minimum_free_disk_space_percentage_<<") is invalid and is reset to 5");
            minimum_free_disk_space_percentage_ = 5;
        }

        if (desired_free_disk_space_percentage_ <= minimum_free_disk_space_percentage_ || 
            desired_free_disk_space_percentage_ >= 100)
        {
            size_t new_desired_free_disk_space_percentage = minimum_free_disk_space_percentage_ + 5;
            LOG4CPLUS_WARN(Loggers::LocalDiskCache(), "bad config setting: the value of desired_free_disk_space_percentage ("<<desired_free_disk_space_percentage_<<") is invalid and is reset to " <<new_desired_free_disk_space_percentage);
            minimum_free_disk_space_percentage_ = new_desired_free_disk_space_percentage;
        }

        if (delete_resource_on_error_)
        {
            LOG4CPLUS_WARN(Loggers::LocalDiskCache(), "delete_resource_on_error is turned on. Resource folders may be deleted if they were not as expected.");
        }
        
        local_disks_path_.clear();
        if (local_disk_list.size() > 0)
        {
            boost::algorithm::split(local_disks_path_, local_disk_list, boost::algorithm::is_any_of(","));
        }
    }

    bool LocalDiskCache::TryReadBlockLocally(const ResourceIdentifier& resource_identifier, size_t block_index, int read_priority, ReadResourceBlockCallback callback)
    {
        boost::shared_ptr<BlockData> block = remote_resource_cache_.GetBlock(resource_identifier, block_index);

        if (block)
        {
            LOG4CPLUS_TRACE(
                Loggers::LocalDiskCache(), 
                "LocalDiskCache::TryReadBlockLocally: found target block from remote_resource_cache_");

            callback(ErrorCodes::Success, block);
            return true;
        }

        if (local_disk_is_resetting_)
            return false;

        boost::shared_ptr<DiskResource> target_resource = disk_resources_.FindResource(resource_identifier);
        if (!target_resource)
        {
            LOG4CPLUS_TRACE(Loggers::LocalDiskCache(), 
                "LocalDiskCache::TryReadBlockLocally: resource " << resource_identifier.GetResourceName() << " is not found on any of the local disks.");
            return false;
        }
        
        if (!target_resource->HasBlock(block_index))
        {
            LOG4CPLUS_TRACE(Loggers::LocalDiskCache(), 
                "LocalDiskCache::TryReadBlockLocally: resource " << resource_identifier.GetResourceName() << " is found on a local disk, but the requested block is not yet available.");
            return false;
        }

        if (target_resource->GetDiskIndex() < local_disks_.size())
        {
            target_resource->Visit();
            boost::shared_ptr<ILocalDisk> containing_disk = local_disks_[target_resource->GetDiskIndex()];
            containing_disk->AsyncRead(
                resource_identifier, 
                block_index, 
                read_priority, 
                io_service_->wrap(boost::bind(&LocalDiskCache::HandleAsyncDiskReadCompleted, shared_from_this(), resource_identifier, block_index, read_priority, callback, _1, _2)));
            return true;
        }

        LOG4CPLUS_WARN(Loggers::LocalDiskCache(), 
            "LocalDiskCache::TryReadBlockLocally: disk_resources_ is in corrupted state: "
            "disk_index from disk_resources_ " << target_resource->GetDiskIndex() << " is not less than # of disks:" << local_disks_.size());

        return false;
    }

    void LocalDiskCache::HandleAsyncDiskReadCompleted(const ResourceIdentifier& resource_identifier, int block_index, int read_priority, ReadResourceBlockCallback callback, ErrorCode error_code, boost::shared_ptr<BlockData> block_data)
    {
        bool retried = false;
        if (running_ && (error_code == ErrorCodes::ServiceBusy || error_code == ErrorCodes::ResourceNotFound))
        {
            //如果磁盘下载任务因为队列满被取消，或本地其实没找着数据块
            size_t num_of_consecutive_missing_blocks = 1;
            pending_block_read_requests_.AddBlockReadRequest(resource_identifier, callback, block_index, /*in/out*/num_of_consecutive_missing_blocks);

            if (num_of_consecutive_missing_blocks > 0)
            {
                bool attempt_disk_write_upon_success = (error_code == ErrorCodes::ResourceNotFound);
                remote_cache_->AsyncReadResource(
                    resource_identifier, 
                    block_index,
                    num_of_consecutive_missing_blocks,
                    io_service_->wrap(bind(&LocalDiskCache::HandleAsyncRemoteCacheReadCompleted, shared_from_this(), resource_identifier, _1, _2, block_index, num_of_consecutive_missing_blocks, attempt_disk_write_upon_success)));
            }

            retried = true;
        }
        
        if (!retried)
        {
            callback(error_code, block_data);
        }
    }

    size_t LocalDiskCache::GetConsecutiveMissingBlocks(const ResourceIdentifier& resource_identifier, size_t start_block_index, size_t max_num_of_blocks) const
    {
        boost::shared_ptr<DiskResource> target_resource = disk_resources_.FindResource(resource_identifier);
        if (!target_resource)
        {
            return max_num_of_blocks;
        }

        for(size_t offset = 0; offset < max_num_of_blocks; ++offset)
        {
            if (target_resource->HasBlock(start_block_index + offset))
            {
                return offset;
            }
        }

        return max_num_of_blocks;
    }
    void LocalDiskCache::AsyncReadCallbackWrapper(ReadResourceBlockCallback callback,
            const ResourceIdentifier& resource_identifier, size_t block_index, ErrorCode err, boost::shared_ptr<BlockData> block)
    {
        // 从remote_resource_cache_中删除块[resource_identifier: block_index]
        remote_resource_cache_.Erase(resource_identifier, block_index);

        callback(err, block);
    }

    void LocalDiskCache::AsyncReadBlock(const ResourceIdentifier& resource_identifier, size_t block_index, int read_priority, ReadResourceBlockCallback callback)
    {
        if (!running_)
        {
            callback(ErrorCodes::ServiceStopped, boost::shared_ptr<BlockData>());
            return;
        }

        if (BypassDisk())
        {
            callback = boost::bind(&LocalDiskCache::AsyncReadCallbackWrapper, shared_from_this(), callback, resource_identifier, block_index, _1, _2);
        }

        if (TryReadBlockLocally(resource_identifier, block_index, read_priority, callback))        
        {
            statistics_->OnHit();
            return;
        }

        statistics_->OnMissed();

        size_t num_of_consecutive_missing_blocks = 1;

        if (!local_disk_is_resetting_)
        {
            size_t max_blocks_to_write_in_batch = disks_in_stable_state_ ? 1 : settings_.GetMaximumBlocksToDownloadInBatch();
            num_of_consecutive_missing_blocks = GetConsecutiveMissingBlocks(resource_identifier, block_index, max_blocks_to_write_in_batch);
            assert(num_of_consecutive_missing_blocks > 0);
        }

        pending_block_read_requests_.AddBlockReadRequest(resource_identifier, callback, block_index, /*in/out*/num_of_consecutive_missing_blocks);

        if (num_of_consecutive_missing_blocks > 0)
        {

            remote_cache_->AsyncReadResource(
                resource_identifier, 
                block_index,
                num_of_consecutive_missing_blocks,
                io_service_->wrap(bind(&LocalDiskCache::HandleAsyncRemoteCacheReadCompleted, shared_from_this(), resource_identifier, _1, _2, block_index, num_of_consecutive_missing_blocks, true)));
        }
    }

    void LocalDiskCache::UpdateStableState()
    {
        size_t num_of_disks_in_stable_state(0);
        for(size_t i = 0; i < local_disks_.size(); ++i)
        {
            if (local_disks_[i] && 
                local_disks_[i]->GetFreeSpacePercentage() <= settings_.GetDesiredFreeDiskSpacePercentage())
            {
                ++num_of_disks_in_stable_state;
            }
        }

        disks_in_stable_state_ = local_disks_.size() && (num_of_disks_in_stable_state >= 2 || 
                                  num_of_disks_in_stable_state == local_disks_.size());
    }

    void LocalDiskCache::OnTimerElapsed()
    {
        if (!running_)
        {
            return;
        }
        
        statistics_->ReportStatus(disk_resources_);

        disk_resources_.OnMinuteElapsed(++elapsed_minutes_count_);
        
        UpdateStableState();

        if (disks_in_stable_state_ && 
            (elapsed_minutes_count_ % 30 == 0))
        {
            recent_resource_write_attempts_.ExpireTrivialRecords();
        }

        timer_.expires_from_now(boost::posix_time::seconds(LocalDiskCacheStatistics::SecondsPerMinute));
        timer_.async_wait(boost::bind(&LocalDiskCache::OnTimerElapsed, shared_from_this()));
    }

    void LocalDiskCache::HandleAsyncRemoteCacheReadCompleted(const ResourceIdentifier& resource_identifier, ErrorCode error_code, const std::map<size_t, boost::shared_ptr<BlockData> >& blocks, size_t start_block_index, size_t num_of_blocks, bool attempt_disk_write_upon_success)
    {
        if (!running_)
        {
            return;
        }

        if (error_code == ErrorCodes::Success)
        {
            if (BypassDisk())
            {
                // 将新获取的资源添加到remote_resource_cache_中
                remote_resource_cache_.Insert(resource_identifier, blocks);
            }
            else if (attempt_disk_write_upon_success)
            {
                AsyncWriteResource(resource_identifier, blocks);
            }
        }

        pending_block_read_requests_.ProcessPendingReadRequests(resource_identifier, error_code, blocks, start_block_index, num_of_blocks);
    }

    void LocalDiskCache::HandleAsyncWriteResource(ErrorCode error_code, const ResourceIdentifier& resource_identifier, size_t disk_index, const std::map<size_t, boost::shared_ptr<BlockData> >& blocks)
    {
        if (error_code == ErrorCodes::Success && disk_index < local_disks_.size())
        {
            boost::shared_ptr<DiskResource> disk_resource = disk_resources_.FindResource(resource_identifier);

            if (disk_resource)
            {
                for (std::map<size_t, boost::shared_ptr<BlockData> >::const_iterator iter = blocks.begin();
                    iter != blocks.end();
                    ++iter)
                {
                    if (iter->second)
                    {
                        disk_resource->AddBlock(iter->first);
                        disk_resource->Visit();
                    }
                }
            }
            else
            {
                LOG4CPLUS_WARN(Loggers::LocalDiskCache(), "Resource " << resource_identifier.GetResourceName() << " is not found from disk_resources_ after it's successfully written to disk. It's most likely removed at an earlier point.");
            }
        }

        for (std::map<size_t, boost::shared_ptr<BlockData> >::const_iterator iter = blocks.begin();
            iter != blocks.end();
            ++iter)
        {
            remote_resource_cache_.Erase(resource_identifier, iter->first);
        }

        DeleteResourcesIfApplicable(disk_index);
    }

    void LocalDiskCache::HandleAsyncDeleteResources(ErrorCode error_code, const std::vector<ResourceIdentifier>& resources)
    {
        if (pending_disk_deletion_tasks_ > 0)
        {
            --pending_disk_deletion_tasks_;
        }

        if (error_code != ErrorCodes::ServiceBusy)
        {
            for(size_t i = 0; i < resources.size(); ++i)
            {
                disk_resources_.RemoveResource(resources[i]);
            }
        }
    }

    void LocalDiskCache::DeleteResourcesIfApplicable(size_t disk_index)
    {
        if (local_disk_is_resetting_)
            return;

        const size_t MaxResourceSizeInBytes = 20*1024*1024;

        if (pending_disk_deletion_tasks_ > 0)
        {
            return;
        }

        if (disk_index >= local_disks_.size())
        {
            assert(false);
            return;
        }

        size_t current_free_percentage = local_disks_[disk_index]->GetFreeSpacePercentage();
        if (current_free_percentage >= settings_.GetMinimumFreeDiskSpacePercentage())
        {
            return;
        }

        size_t num_of_resources_to_delete = settings_.GetMinimumResourcesToDelete();
        size_t disk_space = local_disks_[disk_index]->GetDiskSpaceInBytes();
        if (disk_space > 0)
        {
            assert(settings_.GetDesiredFreeDiskSpacePercentage() > current_free_percentage);
            num_of_resources_to_delete = (settings_.GetDesiredFreeDiskSpacePercentage() - current_free_percentage)*disk_space/(100*MaxResourceSizeInBytes);
        }

        std::vector<ResourceIdentifier> resources_to_delete;
        size_t num_of_blocks_to_delete = static_cast<size_t>(static_cast<double>(MaxResourceSizeInBytes)/BlockData::MaxBlockSize*num_of_resources_to_delete);
        disk_resources_.GetLeastSignificantResources(disk_index, num_of_blocks_to_delete, resources_to_delete);

        if(resources_to_delete.size() > 0)
        {
            ++pending_disk_deletion_tasks_;
            local_disks_[disk_index]->AsyncDeleteResources(resources_to_delete, io_service_->wrap(boost::bind(&LocalDiskCache::HandleAsyncDeleteResources, shared_from_this(), _1, _2)));
        }
    }

    void LocalDiskCache::AsyncWriteResource(const ResourceIdentifier& resource_identifier, const std::map<size_t, boost::shared_ptr<BlockData> >& blocks)
    {
        if (local_disk_is_resetting_)
            return;

        int target_disk_index = 0;
        boost::shared_ptr<DiskResource> disk_resource = disk_resources_.FindResource(resource_identifier);
        if (disk_resource)
        {
            target_disk_index = disk_resource->GetDiskIndex();
        }
        else
        {
            if (disks_in_stable_state_)
            {
                //we are not gonna write the resource into disk until it's proved to be popular
                bool is_popular_resource(false);
                recent_resource_write_attempts_.RecordWriteAttempt(resource_identifier, is_popular_resource);
                if (!is_popular_resource)
                {
                    LOG4CPLUS_INFO(Loggers::LocalDiskCache(), "Resource "<<resource_identifier.GetResourceName()<<" is not yet proved to be popular and a disk write would be skipped.");
                    return;
                }

                LOG4CPLUS_INFO(Loggers::LocalDiskCache(), "Resource "<<resource_identifier.GetResourceName()<<" is popular and would be written to disk.");
            }

            DiskSelectionStrategy disk_selection(this->local_disks_);
            target_disk_index = disk_selection.SelectDisk();
            if (target_disk_index < 0)
            {
                return;
            }

            //此时（而不是写入后）添加是为了避免在本blocks写入磁盘前，后续针对同一resource的其他block的写请求不至于选择另外一块磁盘去写。
            BlocksMap empty_blocks_map;
            boost::shared_ptr<DiskResource> resource(new DiskResource(resource_identifier, true, empty_blocks_map));
            disk_resources_.AddResource(target_disk_index, resource);
        }

        //把新下载好的数据加入remote_resource_cache_中，等到数据写入磁盘后再拿掉
        remote_resource_cache_.Insert(resource_identifier, blocks);

        assert(local_disks_.size() > static_cast<size_t>(target_disk_index));
        local_disks_[target_disk_index]->AsyncWrite(resource_identifier, blocks, io_service_->wrap(bind(&LocalDiskCache::HandleAsyncWriteResource, shared_from_this(), _1, _2, target_disk_index, blocks)));
    }

    void LocalDiskCache::UpdateConfig()
    {
        settings_.LoadFromConfig();
        recent_resource_write_attempts_.SetPopularResourceRequestsCount(settings_.GetPopularResourceRequestsCount());

        remote_resource_cache_.SetCacheSize(settings_.GetMaximumRemoteResourcesToCache());

#ifndef BOOST_WINDOWS_API
        BigFileLocalDiskManager::Instance().LoadConfig();
#endif

        std::vector<std::string> new_disk_paths;
        settings_.GetDisksPath(new_disk_paths);

        if (new_disk_paths != disk_paths_)
        {
            ResetLocalDisks(new_disk_paths);
        }
        else
        {
            for (size_t i = 0; i < local_disks_.size(); ++i)
            {
                local_disks_[i]->UpdateConfig();
            }
        }
    }

    // RemoteResourceCache

    void LocalDiskCache::RemoteResourceCache::Insert(const ResourceIdentifier& rid, size_t block_id, boost::shared_ptr<BlockData> block)
    {
        BlockIdentifier bid(rid, block_id);

        if (block_map_.find(bid) == block_map_.end() && max_blocks_to_cache_)
        {
            if (block_map_.size() >= max_blocks_to_cache_)
            {
                // 清除在队列中保留时间最长的block
                block_map_.erase(come_order_queue_.front());
                come_order_queue_.pop_front();
            }

            come_order_queue_.push_back(bid);
            block_map_[bid] = std::make_pair(block, --come_order_queue_.end());
        }
    }
    void LocalDiskCache::RemoteResourceCache::Insert(const ResourceIdentifier& rid, const std::map<size_t, boost::shared_ptr<BlockData> >& blocks)
    {
        for (std::map<size_t, boost::shared_ptr<BlockData> >::const_iterator iter = blocks.begin(); iter != blocks.end(); ++iter)
        {
            Insert(rid, iter->first, iter->second);
        }
    }

    void LocalDiskCache::RemoteResourceCache::Erase(const ResourceIdentifier& rid, size_t block_id)
    {
        block_iterator iter = block_map_.find(BlockIdentifier(rid, block_id));

        if (iter != block_map_.end())
        {
            come_order_queue_.erase(iter->second.second);
            block_map_.erase(iter);
        }
    }

    boost::shared_ptr<BlockData> LocalDiskCache::RemoteResourceCache::GetBlock(const ResourceIdentifier& rid, size_t block_id)
    {
        block_iterator iter = block_map_.find(BlockIdentifier(rid, block_id));

        if (iter != block_map_.end())
            return iter->second.first;

        return boost::shared_ptr<BlockData>();
    }

    void LocalDiskCache::RemoteResourceCache::GetBlocks(const ResourceIdentifier& rid, std::map<size_t, boost::shared_ptr<BlockData> >& blocks)
    {
        blocks.clear();
        block_iterator iter = block_map_.lower_bound(BlockIdentifier(rid, 0));

        while (iter != block_map_.end() && iter->first.first == rid)
        {
            blocks[iter->first.second] = iter->second.first;
            ++iter;
        }
    }
}
