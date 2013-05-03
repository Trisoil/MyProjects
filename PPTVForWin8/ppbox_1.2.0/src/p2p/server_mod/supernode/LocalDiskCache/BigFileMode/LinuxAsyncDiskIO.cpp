#include <libaio.h>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <set>
#include <map>
#include <fcntl.h>
#include <exception>
#include "AsyncDiskIO.h"


namespace super_node
{

class LinuxAsyncDiskIO : public AsyncDiskIO
{
private:
    void Start();
    void Stop() { DoStop(); }
    void Sync();

    bool AsyncReadImpl(const char* filename, offset_t offset, void* buf, size_t bytes_to_read, Handler* pHandler);
    bool AsyncWriteImpl(const char* filename, offset_t offset, const void* buf, size_t bytes_to_write, Handler* pHandler);
    bool AsyncIOImpl(bool for_read, const char* filename, offset_t offset, void* buf, size_t bytes_to_transfer, Handler* pHandler);

    virtual ~LinuxAsyncDiskIO() { DoStop(); }

    class FileDescriptor
    {
    public:
        FileDescriptor(int fd = -1) : fd_(fd) {}
        ~FileDescriptor()
        {
            if (fd_ >= 0) close(fd_);
        }

        operator int() { return fd_; }

    private:
        int fd_;
    };

    boost::shared_ptr<FileDescriptor> GetFileDescriptor(const std::string& path, bool for_read);
    void ThreadProc();
    void DoStop();
    void HandleIOEvent(io_event& evt);

private:
    const static int max_events_in_io_context = 1 << 16;
    io_context_t ctx_id_;
    boost::shared_ptr<boost::thread> thread_;
    bool stop_flag_;

    // this is the data associated to iocb through iocb->data
    struct IocbData
    {
        boost::shared_ptr<Handler>  handler_;
        boost::shared_ptr<FileDescriptor> file_descriptor_;
        size_t bytes_transfered_;

        IocbData(boost::shared_ptr<Handler> handler, boost::shared_ptr<FileDescriptor> file_descriptor)
            : handler_(handler)
            , file_descriptor_(file_descriptor)
            , bytes_transfered_(0)
        { }

    };
    boost::mutex iocb_mutex_;
    std::set<iocb*> iocb_submitted_;

    // file descriptor cache
    boost::mutex  descriptor_cache_mutex_;
    const static size_t max_descriptor_cache_size = 20000;
    // path -> (file descriptor, order of last visit)
    std::map<std::string,
            std::pair<boost::shared_ptr<FileDescriptor>, std::list<std::string>::iterator>
        > cached_descriptors_;
    std::list<std::string> hit_queue_;
};

boost::shared_ptr<LinuxAsyncDiskIO::FileDescriptor> LinuxAsyncDiskIO::GetFileDescriptor(const std::string& path, bool for_read)
{
    boost::mutex::scoped_lock lock(descriptor_cache_mutex_);

    std::map<std::string,
            std::pair<boost::shared_ptr<FileDescriptor>, std::list<std::string>::iterator>
        >::iterator iter = cached_descriptors_.find(path);

    if (iter != cached_descriptors_.end())
    {
        // move this descriptor to the first position
        hit_queue_.erase(iter->second.second);
        hit_queue_.push_front(path);
        iter->second.second =  hit_queue_.begin();

        return iter->second.first;
    }

    if (cached_descriptors_.size() == max_descriptor_cache_size && !cached_descriptors_.empty())
    {
        // drop the last
        cached_descriptors_.erase(hit_queue_.back());
        hit_queue_.pop_back();
    }

    int fd = open(path.c_str(), O_RDWR | O_LARGEFILE | O_DIRECT | (for_read ? 0 : O_CREAT), S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);

    if (fd < 0)
        return boost::shared_ptr<FileDescriptor>();

    boost::shared_ptr<FileDescriptor> descriptor(new FileDescriptor(fd));

    hit_queue_.push_front(path);
    cached_descriptors_[path] = std::make_pair(descriptor, hit_queue_.begin());

    return descriptor;
}

void LinuxAsyncDiskIO::Start()
{
    if (!thread_)
    {
        memset(&ctx_id_, 0, sizeof(ctx_id_));
        int err = io_setup(max_events_in_io_context, &ctx_id_);
        if (err)
        {
            assert(false);
            throw std::runtime_error("Unexpected system error: io_setup(...)");
        }

        stop_flag_ = false;
        thread_.reset( new boost::thread(boost::bind(&LinuxAsyncDiskIO::ThreadProc, /*shared_from_this()*/ this ) ) );
    }
}

void LinuxAsyncDiskIO::DoStop()
{
    if (thread_)
    {
        stop_flag_ = true;
        thread_->join();
        thread_.reset();
        stop_flag_ = false;

        // io_destroy(ctx_id_) must be called in the background thread
    }
}

void LinuxAsyncDiskIO::HandleIOEvent(io_event& evt)
{
    struct iocb* pIocb = evt.obj;
    struct IocbData* pData = (IocbData*)(pIocb->data);
    ErrorCode err = E_FAIL;

    /*
     * evt.res: same as the return value of synchronous IO
     * evt.res2: status (0 means OK)
     */

    if ((int)evt.res < 0 || evt.res > pIocb->u.c.nbytes) // error occured
        goto completed;

    pData->bytes_transfered_ += evt.res;

    if (evt.res2 != 0) // error occured
        goto completed;

    if (evt.res == pIocb->u.c.nbytes || evt.res == 0)
    {  // 0 means reach the end of file
        err = E_SUCCESS;
        goto completed;
    }

    // re-submit for the left data
    pIocb->u.c.offset += evt.res;
    pIocb->u.c.buf = (char*)(pIocb->u.c.buf) + evt.res;
    pIocb->u.c.nbytes -= evt.res;

    if (io_submit(ctx_id_, 1, &pIocb) == 1)
        return;
    // else failed, so follow through the following code
    
completed:
    {
        boost::mutex::scoped_lock lock(iocb_mutex_);
        iocb_submitted_.erase(pIocb);
    }

    try
    {
        (*(pData->handler_))(err, pData->bytes_transfered_);
    }
    catch (...)
    {
        // ignore
    }

    delete pData;
    free(pIocb);
}

void LinuxAsyncDiskIO::ThreadProc()
{
    while (!stop_flag_)
    {
        const static size_t event_buf_size = 128;
        io_event events[event_buf_size];

        timespec timeout;
        timeout.tv_sec = 0;
        timeout.tv_nsec = 500 * 1000 * 1000; // 500 ms

        int cnt = io_getevents(ctx_id_, 1, event_buf_size, events, &timeout);

        for (int i = 0; i < cnt; ++i)
        {
            HandleIOEvent(events[i]);
        }
    }

    // must do this before the deletions of [struct iocbs], otherwise segment-fault may result,
    // for kernel may use it in the same time
    io_destroy(ctx_id_);
    memset(&ctx_id_, 0, sizeof(ctx_id_));

    // cancel the remaining submitted iocbs
    while (true)
    {
        iocb* pIocb = 0;
        {
            boost::mutex::scoped_lock lock(iocb_mutex_);

            if (!iocb_submitted_.empty())
            {
                pIocb = *(iocb_submitted_.begin());
                iocb_submitted_.erase(iocb_submitted_.begin());
            }
        }

        if (pIocb)
        {
            IocbData* pData = (IocbData*)pIocb->data;
            try
            {
                (*(pData->handler_))(E_CANCEL, (size_t)0U);
            }
            catch (...)
            {
                // ignore
            }
            delete pData;
            free(pIocb);
        }
        else
        {
            break;
        }
    }
}

bool LinuxAsyncDiskIO::AsyncIOImpl(bool for_read, const char* filename, offset_t offset, void* buf, size_t bytes_to_transfer, Handler* __pHandler)
{
    bool ret = false;

    boost::shared_ptr<Handler> handler(__pHandler);

    if (thread_)
    {
        boost::shared_ptr<FileDescriptor>  file_descriptor = GetFileDescriptor(filename, for_read);

        if (file_descriptor)
        {
            struct iocb* pIocb = (struct iocb*)malloc(sizeof(struct iocb));

            if (pIocb)
            {
                if (for_read)
                    io_prep_pread(pIocb, *file_descriptor, buf, bytes_to_transfer, offset);
                else
                    io_prep_pwrite(pIocb, *file_descriptor, buf, bytes_to_transfer, offset);

                pIocb->data = new IocbData(handler, file_descriptor);

                {
                    boost::mutex::scoped_lock lock(iocb_mutex_);
                    iocb_submitted_.insert(pIocb);
                }

                ret = io_submit(ctx_id_, 1, &pIocb) == 1;

                if (!ret)
                {
                    boost::mutex::scoped_lock lock(iocb_mutex_);
                    iocb_submitted_.erase(pIocb);
                    delete (IocbData*)(pIocb->data);
                    free(pIocb);
                }
            }
        }
    }

    return ret;
}

bool LinuxAsyncDiskIO::AsyncReadImpl(const char* filename, offset_t offset, void* buf, size_t bytes_to_read, Handler* pHandler)
{
    return AsyncIOImpl(true, filename, offset, buf, bytes_to_read, pHandler);
}

bool LinuxAsyncDiskIO::AsyncWriteImpl(const char* filename, offset_t offset, const void* buf, size_t bytes_to_write, Handler* pHandler)
{
    return AsyncIOImpl(false, filename, offset, const_cast<void*>(buf), bytes_to_write, pHandler);
}

void LinuxAsyncDiskIO::Sync()
{
    while (true)
    {
        {
            boost::mutex::scoped_lock lock(iocb_mutex_);
            if (iocb_submitted_.empty())
                return;
        }

        timespec tm;
        tm.tv_sec = 0;
        tm.tv_nsec = 20 * 1000 * 1000; // 20 ms;
        nanosleep(&tm, 0);
    }
}


AsyncDiskIO* CreateAsyncDiskIO()
{
    return new LinuxAsyncDiskIO();
}

}
