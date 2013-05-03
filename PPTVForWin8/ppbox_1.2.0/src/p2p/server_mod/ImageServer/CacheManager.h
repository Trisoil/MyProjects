// CacheManager.h

#ifndef _CACHEMANAGER_H_
#define _CACHEMANAGER_H_

#include "DiskManager.h"
#include "Cache.h"

#include <Thread/NullLock.h>
typedef Util::NullLock Lock;

//#include <boost/thread/mutex.hpp>
//typedef boost::mutex Lock;

// 数据块正在从磁盘读取时，等待的拷贝任务
struct DelayRead;

class CacheManager
{
public:
    static Util::ErrorCode init(
        Util::Config & conf);

    static Util::ErrorCode start(
        boost::asio::io_service & io_service);

    static Util::ErrorCode stop();

public:
    static void dump();

public:
    static void ReadSubPiece(
        ResId const & rid, 
        size_t index_piece, // index of piece
        size_t index_sub_piece,  // index of sub piece
        unsigned char * data_buf, 
        response_type const & r);

public:
    static Util::ErrorCode AddResource(
        ResId const & rid, 
        size_t disk);

    static void DeleteResource(
        ResId const & mid, 
        Util::Action::response_type const & r);

    static void SyncResource(
        Util::Action::response_type const & r);

public:
    static Util::ErrorCode GetResourceRequestRate(
        ResId const & rid, 
        size_t & rate);

    static size_t GetPausedResource(
        std::vector<ResId> & rids);

    static Util::ErrorCode PauseResource(
        ResId const & rid);

    static Util::ErrorCode ResumeResource(
        ResId const & rid);

private:
    static Util::ErrorCode AddResource2(
        ResId const & rid, 
        size_t size);

    static void HandleBlockRead(
        Util::ErrorCode err, 
        WeakPtr const & data, 
        ResCache::pointer res, 
        BlockCache::pointer block);

    static void HandleDeleteResource(
        Util::ErrorCode err, 
        std::string const & msg, 
        ResCache::pointer res,  
        Util::Action::response_type const & r);

    static void HandleSyncResource(
        Util::ErrorCode err, 
        std::string const & msg, 
        Util::Action::response_type const & r);

    static Util::ErrorCode FindBlock(
        ResId const & rid, 
        size_t index_block, 
        ResCache::pointer& res, 
        BlockCache::pointer& block);

private:
    static void Statistics();

    static void Tick();

    static size_t DropCache(
        size_t percent);

private:

private:
    static size_t max_block_cache;
    static size_t cache_block_size;
    static size_t interval_statistics;
    static size_t interval_drop_cache;
    static size_t interval_sync_res;
    static size_t drop_percent_period;
    static size_t drop_percent_urgy;
    static size_t block_keep_time;
    static size_t dead_block_keep_time;
    static size_t priority_keep_percent;
    static int log_level;

private:
    static Lock mutex_;
    static boost::asio::io_service * io_service_;
    static Cache * cache_; // 缓存状态数据的根
    static time_t now_;
    static time_t expire_delim_;
    static size_t piece_per_cache_block_;
    static size_t time_left_statistics_;
    static size_t time_left_drop_cache_;
    static size_t time_left_sync_res_;
};

#endif // #ifndef _CACHEMANAGER_H_
