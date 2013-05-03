#ifndef __ASYNC_DISK_IO_H__
#define __ASYNC_DISK_IO_H__
#include <stddef.h>

namespace super_node
{

typedef unsigned long long offset_t;

class AsyncDiskIO
{
public:
    virtual ~AsyncDiskIO() {}

    /*
     * not thread safe
     */
    virtual void Start() = 0;
    virtual void Stop() = 0;

    enum ErrorCode
    {
        E_SUCCESS,
        E_FAIL,
        E_CANCEL
    };

    /*
     * the function signature of both read and write handler is void handler(ErrorCode error, size_t bytes_transfered)
     * both of AsyncRead(...) and AsyncWrite(...) are thread safe
     * @attention: handler is called in the background thread
     */
    template<typename ReadHandler>
    bool AsyncRead(const char* filename, offset_t offset, void* buf, size_t bytes_to_read,
            ReadHandler handler)
    {
        return AsyncReadImpl(filename, offset, buf, bytes_to_read, new HandlerHolder<ReadHandler>(handler));
    }
    template<typename WriteHandler>
    bool AsyncWrite(const char* filename, offset_t offset, const void* buf, size_t bytes_to_write,
            WriteHandler handler)
    {
        return AsyncWriteImpl(filename, offset, buf, bytes_to_write, new HandlerHolder<WriteHandler>(handler));
    }

    // block until all AIOs are finished
    virtual void Sync() = 0;

protected:
    class Handler
    {
    public:
        virtual ~Handler() {}
        virtual void operator() (ErrorCode error, size_t bytes_transfered) = 0;
    };

    template<typename HandlerType>
    class HandlerHolder : public Handler
    {
        HandlerType  handler_;
        virtual void operator() (ErrorCode error, size_t bytes_transfered) { handler_(error, bytes_transfered); }
    public:
        HandlerHolder(const HandlerType& handler) : handler_(handler)
        {}
    };

    /*
     * implementation of both AsyncReadImpl and AsyncWriteImpl are responsible for pHandler's deletion
     */
    virtual bool AsyncReadImpl(const char* filename, offset_t offset, void* buf, size_t bytes_to_read, Handler* pHandler) = 0;
    virtual bool AsyncWriteImpl(const char* filename, offset_t offset, const void* buf, size_t bytes_to_write, Handler* pHandler) = 0;
};

AsyncDiskIO* CreateAsyncDiskIO();

}

#endif
