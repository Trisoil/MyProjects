#ifndef SUPER_NODE_SMALL_FILE_LOCAL_DISK_H
#define SUPER_NODE_SMALL_FILE_LOCAL_DISK_H

#include "../LocalDisk.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include "../../PriorityTasksQueue.h"
// #include "../LocalDiskCache.h"
#include "DiskTask.h"
#include "../../ServiceHealthMonitor/ServiceComponent.h"


namespace super_node
{

class SmallFileLocalDisk
    : public ILocalDisk,
    public IServiceComponentHealthPredicate,
    public boost::enable_shared_from_this<SmallFileLocalDisk>,
    public count_object_allocate<SmallFileLocalDisk>
{
public:
    SmallFileLocalDisk(const string& file_path);

    void Start();
    void Stop();

    void UpdateConfig();

    bool AsyncRead(const ResourceIdentifier& resource_identifier, int block_index, int read_priority, ReadResourceBlockCallback callback);
    bool AsyncWrite(const ResourceIdentifier& resource_identifier, const std::map<size_t, boost::shared_ptr<BlockData> >& blocks, WriteResourceTaskCallback callback);
    bool AsyncLoadResources(LoadResourcesTaskCallback callback, bool delete_resource_on_error);
    bool AsyncDeleteResources(const std::vector<ResourceIdentifier>& resources, DeleteResourcesTaskCallback callback);

    boost::filesystem::path GetResourceBlockFilePath(const ResourceIdentifier& resource_identifier, size_t block_index) const;
    boost::filesystem::path GetResourceFolderPath(const ResourceIdentifier& resource_identifier) const;
    boost::filesystem::path GetDiskPath() const { return disk_path_; }

    //these functions do NOT actually add/remove/update resources, 
    //they are exposed for disk tasks to notify disk statistics to update disk space used as appropriate
    void UpdateResources(const std::vector<boost::shared_ptr<DiskResource> >& resources);
    void UpdateResource(const ResourceIdentifier& resource_identifier);
    void RemoveResource(const ResourceIdentifier& resource_identifier);

    size_t GetFreeSpacePercentage() const;
    boost::uint64_t GetDiskSpaceInBytes() const;
    
    bool IsComponentHealthy() const;

protected:
    //for unittestability
    bool EnqueueTask(boost::shared_ptr<DiskTask> task);

private:
    void DiskThread();
    void ReportStatus(boost::uint32_t ticks_since_last_report);
    static string GetBlockFileName(size_t block_index);
    string DiskComponentName() const;
    void DropPendingTasks();

private:
    static const int QueueSize;

    PriorityTasksQueue<DiskTask> pending_tasks_;
    boost::filesystem::path disk_path_;

    bool stopped_;
    boost::mutex mutex_;
    boost::condition condition_;

    boost::shared_ptr<boost::thread> disk_thread_;

    boost::shared_ptr<DiskStatistics> disk_statistics_;

    boost::shared_ptr<ComponentStatusItem> disk_read_speed_;
    boost::shared_ptr<ComponentStatusItem> disk_read_count_;
    boost::shared_ptr<ComponentStatusItem> disk_read_failure_rate_;
    boost::shared_ptr<ComponentStatusItem> disk_read_consecutive_failures_;
    boost::shared_ptr<ComponentStatusItem> disk_write_speed_;
    boost::shared_ptr<ComponentStatusItem> disk_write_count_;
    boost::shared_ptr<ComponentStatusItem> disk_write_failure_rate_;
    boost::shared_ptr<ComponentStatusItem> disk_write_consecutive_failures_;
    boost::shared_ptr<ComponentStatusItem> max_queue_length_;
    boost::shared_ptr<ComponentStatusItem> average_queue_length_;
    boost::shared_ptr<ComponentStatusItem> queue_drop_rate_;

    boost::uint32_t disk_task_failed_times_;
    boost::uint32_t disk_task_failed_times_since_last_success_;
};

}

#endif
