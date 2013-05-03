// DiskManager.h

#ifndef _DISKMANAGER_H_
#define _DISKMANAGER_H_

#include <Container/Ordered.h>
#include <Container/List.h>

#include <boost/thread.hpp>
#include <boost/multi_index/member.hpp>

class DiskProxy;
struct CopyTask;
struct SyncResourceStruct;

struct Resource
    : Util::OrderedHook<Resource>::type
    , Util::ObjectMemoryPoolObjectNoThrow<Resource, Util::NullLock>
{
    enum StatusEnum
    {
        working, 
        paused
    };

    ResId rid;
    size_t size;
    size_t idisk;
    Statistics req;
    StatusEnum status;
};

class DiskManager
{
public:
    static Util::ErrorCode init(
        Util::Config & conf);

    static Util::ErrorCode start(
        boost::asio::io_service & io_service);

    static Util::ErrorCode stop();

public:
    typedef Util::Ordered<
        Resource, 
        boost::multi_index::member<
            Resource, 
            ResId const, 
            &Resource::rid
        >, 
        std::less<ResId>, 
        Util::ordered_non_unique_tag
    > ResourceMap;

    static ResourceMap const & GetResource()
    {
        return res_map_;
    }

    static Util::ErrorCode AddResource(
        ResId const & rid, 
        size_t disk, 
        size_t & size);

    static void DeleteResource(
        ResId const & mid, 
        Util::Action::response_type const & r);

    static void SyncResource(
        Util::Action::response_type const & r);

    static void CopyResource(
        ResId const & mid, 
        std::vector<size_t> const &dests, 
        Util::Action::response_type const & r);

    static Util::ErrorCode ReadData(
        ResId const & rid, 
        size_t offset, 
        size_t size, 
        size_t priority, 
        disk_task_response_type const & r);

    static size_t CacheNeed()
    {
        return disks_.size() + 1;
    }

    static void Tick();

private:
    static size_t GetResourceOfDisk(
        size_t idisk, 
        Resource::StatusEnum stat, 
        size_t load, 
        std::vector<Resource *> & ress);

    static void HandleDeleteResource(
        Util::ErrorCode ec, 
        std::string const & msg, 
        ResId const & rid, 
        Util::Action::response_type const & r);

    static void HandleSyncResource(
        SyncResourceStruct & sr, 
        size_t idisk, 
        std::map<ResId, size_t> & ress, 
        Util::ErrorCode err, 
        std::string const & msg);

    static void HandleCopyTask(
        Util::ErrorCode err, 
        CopyTask * task);

    static void HandleReadWriteBlock(
        CopyTask * task, 
        Util::ErrorCode err, 
        WeakPtr const & data);

private:
    friend class DiskProxy;
    static std::vector<std::string> disks;
    static size_t max_queue_size;
    static size_t interval_statistics;
    static size_t num_drop_alarm;
    static size_t time_busy_refuse;
    static size_t copy_percent;
    static size_t pause_percent;
    static size_t resume_percent;
    static bool enable_priority;
    static size_t delete_type;
    static int log_level;

private:
    static boost::asio::io_service * io_service_;
    static boost::asio::io_service * io_service_adjust_;
    static boost::thread * thread_;
    static std::vector<DiskProxy *> disks_;
    static ResourceMap res_map_;
    static Util::List<CopyTask> * copy_tasks_;
    static size_t time_left_statistics_;
};

#endif // #ifndef _DISKMANAGER_H_
