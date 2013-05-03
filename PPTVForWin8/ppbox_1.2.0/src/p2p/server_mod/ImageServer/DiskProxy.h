// DiskProxy.h

#ifndef _DISKPROXY_H_
#define _DISKPROXY_H_

#include <MsgTrans/Types.h>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/filesystem/path.hpp>

class DiskManager;

struct DiskTask;
struct DiskStatistics;
struct DiskStatisticsHead;

class DiskProxy
{
public:
    enum DeleteTypeEnum {
        no_delete, 
        do_delete, 
        move_to_recycle, 
        move_to_invalid, 
    };

public:
    DiskProxy(
        std::string const & path, 
        Util::ErrorCode & error);

    ~DiskProxy(void);

public:
    void Start();

    void Stop();

    Util::ErrorCode ReadData(
        ResId const & rid, 
        size_t offset, 
        size_t size, 
        size_t priority, 
        disk_task_response_type const & r);

    // 不检查队列满
    Util::ErrorCode ReadDataNoCheck(
        ResId const & rid, 
        size_t offset, 
        size_t size, 
        disk_task_response_type const & r);

    Util::ErrorCode WriteData(
        ResId const & rid, 
        size_t offset, 
        size_t size, 
        SharePtr const & data, 
        disk_task_response_type const & r);

    void GetResource(
        std::map<ResId, size_t> & ress, 
        Util::Action::response_type const & r);

    Util::ErrorCode AddResource(
        ResId const & rid, 
        size_t size);

    Util::ErrorCode AddResourceReal(
        ResId const & rid, 
        size_t & size);

    void DeleteResource(
        ResId const & rid, 
        Util::Action::response_type const & r);

    Util::ErrorCode DeleteResourceReal(
        ResId const & rid, 
        DeleteTypeEnum type);

public:
    static void Statistics();

    DiskStatistics * stat()
    {
        return stat_;
    }

private:
    Util::ErrorCode CheckResource(
        ResId const & rid, 
        size_t & size);

    Util::ErrorCode ReadFile(
        ResId const & rid, 
        size_t offset, 
        size_t size, 
        char * buf);

    Util::ErrorCode WriteFile(
        ResId const & rid, 
        size_t offset, 
        size_t size, 
        char * buf);

    void GetResourceReal(
        std::map<ResId, size_t> & ress);

private:
    void WorkThread();

private:
    int stopped_;
    boost::filesystem::path path_;
    boost::mutex mutex_;
    boost::condition cond_;
    Util::List<DiskTask> * tasks_;
    boost::thread * thread_;
    DiskStatistics * stat_;
    Util::UInt64 used_bytes_;

private:
    static DiskStatisticsHead * stat_head_;
};

#endif // #ifndef _DISKPROXY_H_
