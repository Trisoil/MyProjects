#ifndef SUPER_NODE_BIG_FILE_LOCAL_DISK_MANAGER_H
#define SUPER_NODE_BIG_FILE_LOCAL_DISK_MANAGER_H

#include "AsyncDiskIO.h"
#include "../../PriorityTasksQueue.h"
#include "BigFileLocalDisk.h"

namespace super_node
{

class AsyncDiskIO;
class ResourceMetaInfoDB;
class AsioServiceRunner;

class BigFileLocalDiskManager
{
public:
    BigFileLocalDiskManager();
    virtual ~BigFileLocalDiskManager();

    typedef boost::function< void (ErrorCode, size_t) > IOCallback;
    /**
     * both read and write callback are invoked in the background thread
     */
    void AsyncRead(const std::string& filename, boost::uint64_t offset, void* buf, size_t bytes_to_read, int priority, IOCallback callback);
    void AsyncWrite(const std::string& filename, boost::uint64_t offset, const void* buf, size_t bytes_to_write, int priority, IOCallback callback);

    boost::shared_ptr<ResourceMetaInfoDB> GetResourceMetaInfoDB() { return res_meta_db_; }

    /**
     * post job to the background threads
     */
    template<typename Job>
    void Post(Job job) { assert(io_service_); io_service_->post(job); }

    boost::shared_ptr<boost::asio::deadline_timer> CreateTimer();

    static BigFileLocalDiskManager& Instance();

    bool Start(const std::string& meta_db_file, size_t thread_pool_size = 6, size_t disk_io_queue_size = 64);
    void Stop();

    void Config(size_t task_queue_size, size_t task_submit_low, size_t task_submit_high);

    // params needed are get from config file
    bool Start();
    void LoadConfig();

    BigFileLocalDisk* CreateBigFileLocalDisk(const std::string& path, boost::uint64_t file_size_bound = 0);

protected:
    struct TaskQueue;
    struct DiskTask
    {
        TaskQueue* pQueue;
        boost::function< void () >  work;
        boost::function< void () >  cancel;
        
        enum { E_READ, E_WRITE } mode;
        boost::uint64_t io_pos;
        int priority;

        DiskTask() : pQueue(0) {}
        int TaskPriority() { return priority; }
        // [work], [cancel] bind a shared_ptr of [this];
        // when [this] is not need, Clear() should be called, otherwise it will not destruct automatically
        void Clear() { work.clear(); cancel.clear(); }
    };

    struct DiskTaskCmpor
    {
        bool operator() (const boost::shared_ptr<DiskTask>& a, const boost::shared_ptr<DiskTask>& b) const
        {
            return a->io_pos < b->io_pos;
        }
    };

    struct TaskQueue
    {
        PriorityTasksQueue<DiskTask>  pending_task_queue;
        size_t submitted_task_cnt;

        TaskQueue() : submitted_task_cnt(0) {}
    };

    typedef boost::function<void(AsyncDiskIO::ErrorCode, size_t)>  AIOImplCallback;
    void DoAsyncRead(const std::string& filename, boost::uint64_t offset, void* buf, size_t bytes_to_read, AIOImplCallback callback);
    void DoAsyncWrite(const std::string& filename, boost::uint64_t offset, const void* buf, size_t bytes_to_write, AIOImplCallback callback);

    bool EnqueueTask(const std::string& filename, boost::shared_ptr<DiskTask> task);
    void GetTasksToSubmit(std::vector<boost::shared_ptr<DiskTask> >& tasks, TaskQueue& task_queue);
    void EndTask(boost::shared_ptr<DiskTask> task, IOCallback callback, AsyncDiskIO::ErrorCode err, size_t bytes_transfered);

    virtual boost::shared_ptr<TaskQueue> GetTaskQueue(const std::string& filename);

protected:
    boost::shared_ptr<AsyncDiskIO>  async_io_;
    boost::shared_ptr<AsioServiceRunner>  io_runner_;
    boost::shared_ptr<boost::asio::io_service>  io_service_;

    boost::shared_ptr<ResourceMetaInfoDB>  res_meta_db_;

    boost::mutex  mutex_;

    std::map<std::string, boost::shared_ptr<TaskQueue> >  task_queue_map_;
    size_t task_queue_size_bound_;
    size_t task_submit_low_;
    size_t task_submit_high_;
};


}

#endif
