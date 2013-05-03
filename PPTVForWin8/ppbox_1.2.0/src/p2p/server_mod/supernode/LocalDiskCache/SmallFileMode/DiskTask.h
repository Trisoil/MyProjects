//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_DISK_TASK_H
#define SUPER_NODE_DISK_TASK_H

#include "../../LocalDiskCache.h"

namespace super_node
{
    class SmallFileLocalDisk;
    class BlockData;
    class IOStatistics;

    class DiskTasksDefaultPriority
    {
    public:
        static const size_t DiskRead = 20;
        static const size_t DiskWrite = 30;
        static const size_t DiskWalk = DiskWrite;
        static const size_t DiskDeletion = 100;
    };

    class DiskTask
        : public count_object_allocate<DiskTask>
    {
    protected:
        DiskTask(boost::shared_ptr<SmallFileLocalDisk> local_disk, int priority)
            : local_disk_(local_disk), priority_(priority)
        {
        }

    public:
        virtual ~DiskTask(){}
        int TaskPriority() const { return priority_; }
        ErrorCode Execute();
        virtual void Cancel() = 0;
    protected:
        virtual ErrorCode Execute(boost::shared_ptr<SmallFileLocalDisk> disk) = 0;
    private:
        int priority_;
        boost::shared_ptr<SmallFileLocalDisk> local_disk_;
    };

    class DiskReadTask
        :public DiskTask
        , public count_object_allocate<DiskReadTask>
    {
    public:
        DiskReadTask(boost::shared_ptr<SmallFileLocalDisk> local_disk, const ResourceIdentifier& resource_identifier, int block_id, ReadResourceBlockCallback callback, boost::shared_ptr<IOStatistics> disk_read_statistics, int priority = DiskTasksDefaultPriority::DiskRead)
            : DiskTask(local_disk, priority), resource_identifier_(resource_identifier), block_id_(block_id), callback_(callback), disk_read_statistics_(disk_read_statistics)
        {
        }

        virtual void Cancel();

    protected:
        virtual ErrorCode Execute(boost::shared_ptr<SmallFileLocalDisk> disk);
    private:
        ResourceIdentifier resource_identifier_;
        int block_id_;
        ReadResourceBlockCallback callback_;
        boost::shared_ptr<IOStatistics> disk_read_statistics_;
    };

    class DiskWriteTask
        :public DiskTask
        , public count_object_allocate<DiskWriteTask>
    {
    public:
        DiskWriteTask(boost::shared_ptr<SmallFileLocalDisk> local_disk, const ResourceIdentifier& resource_identifier, const std::map<size_t, boost::shared_ptr<BlockData> > blocks, WriteResourceTaskCallback callback, boost::shared_ptr<IOStatistics> disk_write_statistics, int priority = DiskTasksDefaultPriority::DiskWrite)
            : DiskTask(local_disk, priority), resource_identifier_(resource_identifier), blocks_(blocks), callback_(callback), disk_write_statistics_(disk_write_statistics)
        {
        }

        virtual void Cancel();
    protected:
        virtual ErrorCode Execute(boost::shared_ptr<SmallFileLocalDisk> disk);

    private:
        ErrorCode ExecuteDiskWrite(boost::shared_ptr<SmallFileLocalDisk> disk);

    private:
        ResourceIdentifier resource_identifier_;
        std::map<size_t, boost::shared_ptr<BlockData> > blocks_;
        WriteResourceTaskCallback callback_;
        boost::shared_ptr<IOStatistics> disk_write_statistics_; 
    };

    class DiskWalkTask
        :public DiskTask
        , public count_object_allocate<DiskWalkTask>
    {
    public:
        DiskWalkTask(boost::shared_ptr<SmallFileLocalDisk> local_disk, LoadResourcesTaskCallback callback, bool delete_resource_on_error, int priority = DiskTasksDefaultPriority::DiskWalk)
            : DiskTask(local_disk, priority), delete_resource_on_error_(delete_resource_on_error), callback_(callback)
        {
        }

        virtual void Cancel();

    protected:
        virtual ErrorCode Execute(boost::shared_ptr<SmallFileLocalDisk> disk);

    private:
        void EnumerateResources(boost::shared_ptr<SmallFileLocalDisk> disk, std::vector<boost::shared_ptr<DiskResource> >& resources);

        void DoEnumerateResources(boost::shared_ptr<SmallFileLocalDisk> disk, 
                std::vector<boost::shared_ptr<DiskResource> >& actual_resources);

        boost::shared_ptr<DiskResource>  LoadResource(
            boost::shared_ptr<SmallFileLocalDisk> disk, 
            const ResourceIdentifier& resource_identifier, 
            const boost::filesystem::path& resource_directory_path);

        bool RemoveFromDisk(const boost::filesystem::path & path);

    private:
        LoadResourcesTaskCallback callback_;
        bool delete_resource_on_error_;
    };

    class DiskDeletionTask
        :public DiskTask
        , public count_object_allocate<DiskDeletionTask>
    {
    public:
        DiskDeletionTask(boost::shared_ptr<SmallFileLocalDisk> local_disk, const std::vector<ResourceIdentifier>& resources, DeleteResourcesTaskCallback callback, int priority = DiskTasksDefaultPriority::DiskDeletion)
            : DiskTask(local_disk, priority), resources_(resources), callback_(callback)
        {
        }

        virtual void Cancel();

    protected:
        virtual ErrorCode Execute(boost::shared_ptr<SmallFileLocalDisk> disk);

    private:
        std::vector<ResourceIdentifier> resources_;
        DeleteResourcesTaskCallback callback_;
    };

    //这类disk task的唯一目的就是挤掉比它优先级低的disk task
    //一个例子是，在执行时间较长的DiskDeletionTask运行的时候，我们希望能cancel掉所有在该磁盘上的disk read/write task
    class DiskNoOperationTask
        : public DiskTask
        , public count_object_allocate<DiskNoOperationTask>
    {
    public:
        DiskNoOperationTask(boost::shared_ptr<SmallFileLocalDisk> local_disk, int priority)
            : DiskTask(local_disk, priority)
        {
        }

        virtual void Cancel();

    protected:
        virtual ErrorCode Execute(boost::shared_ptr<SmallFileLocalDisk> disk);
    };
}

#endif //SUPER_NODE_DISK_TASK_H
