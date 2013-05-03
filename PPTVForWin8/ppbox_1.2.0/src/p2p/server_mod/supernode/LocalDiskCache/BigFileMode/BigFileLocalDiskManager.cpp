#include "Common.h"
#include "../../AsioServiceRunner.h"
#include "BigFileLocalDiskManager.h"
#include "BigFileLocalDisk.h"
#include "AsyncDiskIO.h"
#include "../../Loggers.h"
#include <framework/configure/Config.h>
#include "../../ConfigManager/ConfigManager.h"


namespace super_node
{

BigFileLocalDiskManager::BigFileLocalDiskManager()
    : task_queue_size_bound_(64)
    , task_submit_low_(0)
    , task_submit_high_(64)
{
}

BigFileLocalDiskManager::~BigFileLocalDiskManager()
{
    Stop();
}

bool BigFileLocalDiskManager::Start(const std::string& meta_db_file, size_t thread_pool_size, size_t disk_io_queue_size)
{
    LOG4CPLUS_INFO(Loggers::LocalDisk(), "BigFileLocalDiskManager Start( " << meta_db_file << ", " << thread_pool_size << ")");

    assert(thread_pool_size);

    Stop();

    res_meta_db_.reset( new ResourceMetaInfoDB() );

    if (!res_meta_db_->Start(meta_db_file))
    {
        LOG4CPLUS_ERROR(Loggers::LocalDisk(), "Fail to Start BigFileLocalDiskManager, can't open DB file");

        res_meta_db_.reset();
        return false;
    }

    io_runner_.reset(new AsioServiceRunner("BigFileLocalDiskManager ThreadPool"));

    io_service_ = io_runner_->Start(thread_pool_size);
    assert(io_service_);

    async_io_.reset(CreateAsyncDiskIO());
    async_io_->Start();

    Config(disk_io_queue_size, 0, disk_io_queue_size);

    LOG4CPLUS_INFO(Loggers::LocalDisk(), "BigFileLocalDiskManager Started");

    return true;
}

void BigFileLocalDiskManager::Stop()
{
    if (io_runner_)
    {
        io_runner_->Stop();
        io_service_.reset();
        io_runner_.reset();

        async_io_->Sync();
        async_io_->Stop();
        async_io_.reset();

        res_meta_db_->Stop();
        res_meta_db_.reset();

        task_queue_map_.clear();
    }
}

boost::shared_ptr<boost::asio::deadline_timer> BigFileLocalDiskManager::CreateTimer()
{
    return boost::shared_ptr<boost::asio::deadline_timer>(new boost::asio::deadline_timer(*io_service_));
}

void BigFileLocalDiskManager::AsyncRead(const std::string& filename, boost::uint64_t offset, void* buf, size_t bytes_to_read, int priority, IOCallback callback)
{
    assert(io_service_);

    boost::shared_ptr<DiskTask> task(new DiskTask);

    task->mode = DiskTask::E_READ;
    task->io_pos = offset;
    task->priority = priority;

    AIOImplCallback  aio_impl_callback = boost::bind(&BigFileLocalDiskManager::EndTask, this, task, callback, _1, _2);
    task->work = boost::bind(&BigFileLocalDiskManager::DoAsyncRead, this, filename, offset, buf, bytes_to_read, aio_impl_callback);

    task->cancel = boost::bind(callback, ErrorCodes::ServiceBusy, 0);

    EnqueueTask(filename, task);
}

void BigFileLocalDiskManager::AsyncWrite(const std::string& filename, boost::uint64_t offset, const void* buf, size_t bytes_to_write, int priority, IOCallback callback)
{
    assert(io_service_);

    boost::shared_ptr<DiskTask>  task(new DiskTask);

    task->mode = DiskTask::E_WRITE;
    task->io_pos = offset;
    task->priority = priority;

    AIOImplCallback  aio_impl_callback = boost::bind(&BigFileLocalDiskManager::EndTask, this, task, callback, _1, _2);
    task->work = boost::bind(&BigFileLocalDiskManager::DoAsyncWrite, this, filename, offset, buf, bytes_to_write, aio_impl_callback);

    task->cancel = boost::bind(callback, ErrorCodes::ServiceBusy, 0);

    EnqueueTask(filename, task);
}

void BigFileLocalDiskManager::DoAsyncRead(const std::string& filename, boost::uint64_t offset, void* buf, size_t bytes_to_read, AIOImplCallback callback)
{
    assert(async_io_);

    bool ret = async_io_->AsyncRead(filename.c_str(), offset, buf, bytes_to_read, io_service_->wrap(callback));

    if (!ret) // async_io_ will not invoke callback, so post it
        io_service_->post( boost::bind(callback, AsyncDiskIO::E_FAIL, 0) );
}

void BigFileLocalDiskManager::DoAsyncWrite(const std::string& filename, boost::uint64_t offset, const void* buf, size_t bytes_to_write, AIOImplCallback callback)
{
    assert(async_io_);

    bool ret = async_io_->AsyncWrite(filename.c_str(), offset, buf, bytes_to_write, io_service_->wrap(callback));

    if (!ret)
        io_service_->post( boost::bind(callback, AsyncDiskIO::E_FAIL, 0) );
}

void BigFileLocalDiskManager::EndTask(boost::shared_ptr<DiskTask> task, IOCallback callback, AsyncDiskIO::ErrorCode err, size_t bytes_transfered)
{
    std::vector<boost::shared_ptr<DiskTask> >  tasks_to_submit;

    {
        boost::mutex::scoped_lock  guard(mutex_);

        assert(task->pQueue && task->pQueue->submitted_task_cnt > 0);

        task->pQueue->submitted_task_cnt--;

        GetTasksToSubmit(tasks_to_submit, *(task->pQueue));
    }

    for (std::vector<boost::shared_ptr<DiskTask> >::iterator it = tasks_to_submit.begin(); it != tasks_to_submit.end(); ++it)
    {
        (*it)->work();
        (*it)->Clear();
    }

    if (err == AsyncDiskIO::E_SUCCESS)
        callback(ErrorCodes::Success, bytes_transfered);
    else
    {
        callback(task->mode == DiskTask::E_READ ?  ErrorCodes::DiskReadFailure : ErrorCodes::DiskWriteFailure, bytes_transfered);
    }
}

bool BigFileLocalDiskManager::EnqueueTask(const std::string& filename, boost::shared_ptr<DiskTask> task)
{
    assert(task);

    std::vector<boost::shared_ptr<DiskTask> > tasks_to_cancel; // cancellation should be done without mutex_ held
    std::vector<boost::shared_ptr<DiskTask> > tasks_to_submit;

    {
        boost::mutex::scoped_lock guard(mutex_);

        boost::shared_ptr<TaskQueue>  task_queue = GetTaskQueue(filename);

        if (task_queue)
        {
            task->pQueue = &*task_queue;
            task_queue->pending_task_queue.Push(task);

            while (!task_queue->pending_task_queue.Empty() && task_queue->pending_task_queue.Size() + task_queue->submitted_task_cnt > task_queue_size_bound_)
            {
                tasks_to_cancel.push_back( task_queue->pending_task_queue.PopBack() );

                if (tasks_to_cancel.back() == task)
                    task.reset();
            }

            GetTasksToSubmit(tasks_to_submit, *task_queue);
        }
        else
        {
            tasks_to_cancel.push_back(task);
            task.reset();
        }
    }

    for (std::vector<boost::shared_ptr<DiskTask> >::iterator it = tasks_to_submit.begin(); it != tasks_to_submit.end(); ++it)
    {
        (*it)->work();
        (*it)->Clear();
    }

    while (!tasks_to_cancel.empty())
    {
        tasks_to_cancel.back()->cancel();  // from high priority to low priority
        tasks_to_cancel.back()->Clear();
        tasks_to_cancel.pop_back();
    }

    return !!task;
}

void BigFileLocalDiskManager::GetTasksToSubmit(std::vector<boost::shared_ptr<DiskTask> >& tasks, TaskQueue& task_queue)
{
    if (task_queue.submitted_task_cnt <= task_submit_low_)
    {
        tasks.reserve(task_submit_high_ - task_submit_low_);

        while (!task_queue.pending_task_queue.Empty() && task_queue.submitted_task_cnt < task_submit_high_)
        {
            tasks.push_back(task_queue.pending_task_queue.PopFront());
            ++task_queue.submitted_task_cnt;
        }

//        std::sort(tasks.begin(), tasks.end(), DiskTaskCmpor());
    }
}

boost::shared_ptr<BigFileLocalDiskManager::TaskQueue> BigFileLocalDiskManager::GetTaskQueue(const std::string& filename)
{
    std::map<std::string, boost::shared_ptr<TaskQueue> >::iterator it = task_queue_map_.find(filename);

    if (it == task_queue_map_.end())
    {
        task_queue_map_.insert( std::make_pair(filename, boost::shared_ptr<TaskQueue>(new TaskQueue) ) );
        it = task_queue_map_.find(filename);
    }

    return it->second;
}


BigFileLocalDiskManager& BigFileLocalDiskManager::Instance()
{
    static BigFileLocalDiskManager manager;

    return manager;
}

void BigFileLocalDiskManager::Config(size_t task_queue_size, size_t task_submit_low, size_t task_submit_high)
{
    boost::mutex::scoped_lock  guard(mutex_);

    task_queue_size_bound_ = task_queue_size;
    task_submit_low_ = task_submit_low;
    task_submit_high_ = task_submit_high;
}

bool BigFileLocalDiskManager::Start()
{
    std::string meta_db_file;
    size_t thread_pool_size = 1;
    size_t disk_io_task_queue_size = 64;

    framework::configure::Config conf(ConfigManager::AllConfigFileName);

    conf.register_module("LocalDiskCache")
        << CONFIG_PARAM_NAME_RDONLY("resource_meta_info_db_file", meta_db_file)
        << CONFIG_PARAM_NAME_RDONLY("thread_pool_size", thread_pool_size)
        << CONFIG_PARAM_NAME_RDONLY("disk_io_task_queue_size", disk_io_task_queue_size);

    if (meta_db_file.empty())
        meta_db_file = "resource-metainfo.db";

    if (thread_pool_size == 0 || thread_pool_size > 20)
        thread_pool_size = 3;

    if (disk_io_task_queue_size == 0 || disk_io_task_queue_size >= (1 << 16))
        disk_io_task_queue_size = 64;

    return Start(meta_db_file, thread_pool_size, disk_io_task_queue_size);
}

void BigFileLocalDiskManager::LoadConfig()
{
    size_t disk_io_task_queue_size = 64;

    framework::configure::Config conf(ConfigManager::AllConfigFileName);

    conf.register_module("LocalDiskCache")
        << CONFIG_PARAM_NAME_RDONLY("disk_io_task_queue_size", disk_io_task_queue_size);

    if (disk_io_task_queue_size == 0 || disk_io_task_queue_size >= (1 << 16))
        disk_io_task_queue_size = 64;

    Config(disk_io_task_queue_size, 0, disk_io_task_queue_size);
}

BigFileLocalDisk* BigFileLocalDiskManager::CreateBigFileLocalDisk(const std::string& path, boost::uint64_t file_size_bound)
{
    if (!async_io_)
    {
        LOG4CPLUS_ERROR(Loggers::LocalDisk(), "Fail to create BigFileLocalDisk: the manager has not started");
        return 0;
    }

    try
    {
        return new BigFileLocalDisk(this, path, file_size_bound);
    }
    catch (std::exception& exp)
    {
        LOG4CPLUS_ERROR(Loggers::LocalDisk(), "Fail to create BigFileLocalDisk: " << exp.what());
        return 0;
    }
}

}
