#include "Common.h"
#include "SmallFileLocalDisk.h"
#include "DiskTask.h"
#include "Statistics/DiskStatistics.h"
#include "Statistics/QueueStatistics.h"
#include "Statistics/IOStatistics.h"
#include <iomanip>
#include <framework/string/Md5.h>
#include "ServiceHealthMonitor.h"


namespace super_node
{
const int SmallFileLocalDisk::QueueSize = 20;

SmallFileLocalDisk::SmallFileLocalDisk(const string& disk_path)
    : disk_path_(disk_path), stopped_(true), disk_task_failed_times_(0), disk_task_failed_times_since_last_success_(0)
{
    disk_statistics_.reset(new DiskStatistics(disk_path));
}

void SmallFileLocalDisk::Start()
{
    stopped_ = false;

    boost::shared_ptr<ServiceComponent> service_component = ServiceHealthMonitor::Instance()->RegisterComponent(DiskComponentName());
    service_component->SetHealthPredicate(shared_from_this());

    disk_read_speed_ = service_component->RegisterStatusItem("Disk Read - Average Speed", "MB/s");
    disk_read_count_ = service_component->RegisterStatusItem("Disk Read - Completed", "reads/s");
    disk_read_failure_rate_ = service_component->RegisterStatusItem("Disk Read - Failure Rate", "%");
    disk_read_consecutive_failures_ = service_component->RegisterStatusItem("Consecutive Disk Read Failures", "");
    disk_write_speed_ = service_component->RegisterStatusItem("Average Disk Write Speed", "MB/s");
    disk_write_count_ = service_component->RegisterStatusItem("Disk Writes", "writes/s");
    disk_write_failure_rate_ = service_component->RegisterStatusItem("Disk Write Failure Rate", "%");
    disk_write_consecutive_failures_ = service_component->RegisterStatusItem("Consecutive Disk Write Failures", "");
    average_queue_length_ = service_component->RegisterStatusItem("Average Queue Length", "tasks");
    max_queue_length_ = service_component->RegisterStatusItem("Maximum Queue Length", "tasks");
    queue_drop_rate_ = service_component->RegisterStatusItem("Queue Drop Rate", "%");

    disk_thread_.reset(
        new boost::thread(
            bind(
                &SmallFileLocalDisk::DiskThread, 
                shared_from_this())));
}

void SmallFileLocalDisk::Stop()
{
    stopped_ = true;
    ServiceHealthMonitor::Instance()->UnregisterComponent(DiskComponentName());
    condition_.notify_one();
    disk_thread_->join();
    disk_thread_.reset();
    
    DropPendingTasks();
}

void SmallFileLocalDisk::UpdateConfig()
{
}

bool SmallFileLocalDisk::IsComponentHealthy() const
{
    if (stopped_)
    {
        return true;
    }

    //TODO, ericzheng, consider making these settings configurable
    const size_t MaxConsecutiveDiskWriteFailuresAllowed = 10;
    const size_t MaxConsecutiveDiskReadFailuresAllowed = 20;

    if (disk_read_consecutive_failures_ && 
        disk_read_consecutive_failures_->GetStatus() > MaxConsecutiveDiskReadFailuresAllowed)
    {
        OperationsLogging::Log(OperationsLogEvents::WarningTooManyConsecutiveDiskReadFailures, this->disk_path_.string(), Warning);
        return false;
    }
    
    if (disk_write_consecutive_failures_ &&
        disk_write_consecutive_failures_->GetStatus() > MaxConsecutiveDiskWriteFailuresAllowed)
    {
        OperationsLogging::Log(OperationsLogEvents::WarningTooManyConsecutiveDiskWriteFailures, this->disk_path_.string(), Warning);
        return false;
    }

    return true;
}

void SmallFileLocalDisk::DropPendingTasks()
{
    boost::mutex::scoped_lock lock(mutex_);
    while(!pending_tasks_.Empty())
    {
        pending_tasks_.PopFront();
    }
}

void SmallFileLocalDisk::DiskThread()
{
    LOG4CPLUS_INFO(Loggers::LocalDisk(), "disk thread for "<<this->disk_path_<<" started.");
    
#ifdef BOOST_WINDOWS_API
    std::locale loc = std::locale::global(std::locale(""));
#endif
    framework::timer::TimeCounter time_counter;

    while(!stopped_) 
    {
        boost::shared_ptr<DiskTask> task;

        {
            boost::mutex::scoped_lock lock(mutex_);
            
            while(pending_tasks_.Empty() && !stopped_)
            {
                condition_.wait(lock);
            }

            if (stopped_)
            {
                break;
            }

            disk_statistics_->GetQueueStatistics()->Add(pending_tasks_.Size());

            task = pending_tasks_.PopFront();
        }

        assert(task);

        ErrorCode error_code = ErrorCodes::UnknownError;
        try
        {
            error_code = task->Execute();
        }
        catch(const boost::filesystem::filesystem_error& error)
        {
            LOG4CPLUS_ERROR(Loggers::LocalDisk(), "An error occurred while processing disk task: "<<error.what());
            task->Cancel();
            error_code = ErrorCodes::DiskReadWriteFailure;
        }

        if (time_counter.elapse() > 5000)
        {
            ReportStatus(time_counter.elapse());
            time_counter.reset();
        }

        if (error_code == ErrorCodes::Success)
        {
            disk_task_failed_times_since_last_success_ = 0;
        }
        else if (error_code != ErrorCodes::ResourceNotFound)
        {
            ++disk_task_failed_times_;
            ++disk_task_failed_times_since_last_success_;
        }
    }

    while(!pending_tasks_.Empty())
    {
        boost::shared_ptr<DiskTask> task = pending_tasks_.PopFront();
        task->Cancel();
    }

#ifdef BOOST_WINDOWS_API
    std::locale::global(std::locale(loc));
#endif
    LOG4CPLUS_INFO(Loggers::LocalDisk(), "disk thread for "<<this->disk_path_<<" stopped.");
}

string SmallFileLocalDisk::DiskComponentName() const
{
    return ServiceComponents::LocalDiskPrefix + disk_path_.string();
}

void SmallFileLocalDisk::ReportStatus(boost::uint32_t ticks_since_last_report)
{
    boost::shared_ptr<IOStatistics> disk_read = disk_statistics_->GetDiskReadStatistics();
    disk_read_speed_->SetStatus(disk_read->AverageSpeed());
    disk_read_count_->SetStatus(disk_read->SuccessCount()*1000/ticks_since_last_report);
    disk_read_failure_rate_->SetStatus(disk_read->FailureRate());
    disk_read_consecutive_failures_->SetStatus(disk_read->ConsecutiveFailureCount());

    boost::shared_ptr<IOStatistics> disk_write = disk_statistics_->GetDiskWriteStatistics();
    disk_write_speed_->SetStatus(disk_write->AverageSpeed());
    disk_write_count_->SetStatus(disk_write->SuccessCount()*1000/ticks_since_last_report);
    disk_write_failure_rate_->SetStatus(disk_write->FailureRate());
    disk_write_consecutive_failures_->SetStatus(disk_write->ConsecutiveFailureCount());

    boost::shared_ptr<QueueStatistics> queue_statistics = disk_statistics_->GetQueueStatistics();
    average_queue_length_->SetStatus(queue_statistics->AverageQueueSize());
    max_queue_length_->SetStatus(queue_statistics->MaxQueueLength());
    queue_drop_rate_->SetStatus(queue_statistics->DropRate());

    disk_read->Reset();
    disk_write->Reset();
    queue_statistics->Reset();
}

bool SmallFileLocalDisk::AsyncRead(const ResourceIdentifier& resource_identifier, int block_index, int read_priority, ReadResourceBlockCallback callback)
{
    return EnqueueTask(
        boost::shared_ptr<DiskTask>(
            new DiskReadTask(
                shared_from_this(), 
                resource_identifier, 
                block_index, 
                callback, 
                disk_statistics_->GetDiskReadStatistics(), 
                read_priority)));
}

bool SmallFileLocalDisk::AsyncWrite(const ResourceIdentifier& resource_identifier, const std::map<size_t, boost::shared_ptr<BlockData> >& blocks, WriteResourceTaskCallback callback)
{
    return EnqueueTask(
        boost::shared_ptr<DiskTask>(
            new DiskWriteTask(
                shared_from_this(), 
                resource_identifier, 
                blocks, 
                callback, 
                disk_statistics_->GetDiskWriteStatistics())));
}

bool SmallFileLocalDisk::AsyncLoadResources(LoadResourcesTaskCallback callback, bool delete_resource_on_error)
{
    return EnqueueTask(
        boost::shared_ptr<DiskTask>(
            new DiskWalkTask(
                shared_from_this(), 
                callback,
                delete_resource_on_error)));
}

bool SmallFileLocalDisk::AsyncDeleteResources(const std::vector<ResourceIdentifier>& resources, DeleteResourcesTaskCallback callback)
{
    bool enqued_deletion_task = EnqueueTask(
        boost::shared_ptr<DiskTask>(
            new DiskDeletionTask(
                shared_from_this(),
                resources,
                callback)));

    if (enqued_deletion_task)
    {
        //enqueue QueueSize empty but pretty high-priority tasks to cancel out all except the disk deletion task that has highest priority.
        for(size_t i = 0; i < SmallFileLocalDisk::QueueSize; ++i)
        {
            EnqueueTask(
                boost::shared_ptr<DiskTask>(
                    new DiskNoOperationTask(
                        shared_from_this(),
                        DiskTasksDefaultPriority::DiskDeletion - 1)));
        }
    }

    return enqued_deletion_task;
}

bool SmallFileLocalDisk::EnqueueTask(boost::shared_ptr<DiskTask> task)
{
    boost::mutex::scoped_lock lock(mutex_);
    boost::shared_ptr<QueueStatistics> queue_statistics = disk_statistics_->GetQueueStatistics();
    
    pending_tasks_.Push(task);
    queue_statistics->Enqueue();
    bool new_task_dropped = false;

    while (pending_tasks_.Size() > SmallFileLocalDisk::QueueSize)
    {
        boost::shared_ptr<DiskTask> task_to_cancel = pending_tasks_.PopBack();
        task_to_cancel->Cancel();
        queue_statistics->Drop();

        if (task_to_cancel == task)
        {
            new_task_dropped = true;
        }
    }

    condition_.notify_one();
    return !new_task_dropped;
}

boost::filesystem::path SmallFileLocalDisk::GetResourceFolderPath(const ResourceIdentifier& resource_identifier) const
{
    framework::string::Md5 message_digest;
    string resource_name = resource_identifier.GetResourceName();
    message_digest.update(reinterpret_cast<const boost::uint8_t*>(resource_name.c_str()), resource_name.size());
    message_digest.final();
    string message_digest_value = message_digest.to_string();
    assert(message_digest_value.size() >= 4);

    boost::filesystem::path path = disk_path_ / message_digest_value.substr(0, 2) / message_digest_value.substr(2, 2) / resource_name;
    return path;
}

boost::filesystem::path SmallFileLocalDisk::GetResourceBlockFilePath(const ResourceIdentifier& resource_identifier, size_t block_index) const
{
    boost::filesystem::path resource_path = GetResourceFolderPath(resource_identifier);
    
    return resource_path / GetBlockFileName(block_index);
}

string SmallFileLocalDisk::GetBlockFileName(size_t block_index)
{
    assert(block_index < 1000);
    std::ostringstream stream;
    stream<<std::setfill('0')<<std::setw(3)<<block_index<<".block";
    return stream.str();
}

void SmallFileLocalDisk::UpdateResources(const std::vector<boost::shared_ptr<DiskResource> >& resources)
{
    disk_statistics_->UpdateDiskAvailableSpace();
}

void SmallFileLocalDisk::UpdateResource(const ResourceIdentifier& resource_identifier)
{
    disk_statistics_->UpdateDiskAvailableSpace();
}

void SmallFileLocalDisk::RemoveResource(const ResourceIdentifier& resource_identifier)
{
    disk_statistics_->UpdateDiskAvailableSpace();
}

size_t SmallFileLocalDisk::GetFreeSpacePercentage() const
{
    return disk_statistics_->GetFreeSpacePercentage();
}

boost::uint64_t SmallFileLocalDisk::GetDiskSpaceInBytes() const
{
    return disk_statistics_->GetDiskSpaceInBytes();
}
}
