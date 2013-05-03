#include "Common.h"
#include "CacheManager.h"
#include "Cache.h"

#include <Framework/ErrorMsg.h>
#include <Framework/PeriodAction.h>
#include <Framework/UserIf.h>
#include <Action/Action.h>
#include <Container/SetCalc.h>

using namespace Util;

struct DelayRead
{
public:
    DelayRead(
        DelayRead * next, 
        size_t index_piece, 
        size_t index_sub_piece, 
        unsigned char * data_buf, 
        response_type const & resp)
        : next(next)
        , index_piece(index_piece)
        , index_sub_piece(index_sub_piece)
        , data_buf(data_buf)
        , resp(resp)
    {
    }

    DelayRead * next;
    size_t index_piece;
    size_t index_sub_piece;
    unsigned char * data_buf;
    response_type resp;

    //DECLARE_OBJECT_MEMORY_POOL_NOLOCK(DelayRead);
};

//IMPLEMENT_OBJECT_MEMORY_POOL_NOLIMIT(DelayRead);

size_t CacheManager::max_block_cache = 10000;
size_t CacheManager::cache_block_size = BLOCK_SIZE;
size_t CacheManager::interval_statistics = 100;
size_t CacheManager::interval_drop_cache = 100;
size_t CacheManager::interval_sync_res = 300;
size_t CacheManager::drop_percent_period = 10; // 百分之
size_t CacheManager::drop_percent_urgy = 20; // 百分之
size_t CacheManager::block_keep_time = 12; // 块的最大生命期
size_t CacheManager::dead_block_keep_time = 6; // 停尸时间，超过该值的数据块根据历史价值排序，其他数据块根据生命期排序
size_t CacheManager::priority_keep_percent = 50; // 百分之
int CacheManager::log_level = 2;

Lock CacheManager::mutex_;
boost::asio::io_service * CacheManager::io_service_ = NULL;
// 缓存状态数据的根
Cache * CacheManager::cache_ = NULL;
time_t CacheManager::now_ = 0;
time_t CacheManager::expire_delim_ = 0;
size_t CacheManager::piece_per_cache_block_ = PIECE_PER_BLOCK;
size_t CacheManager::time_left_statistics_ = 0;
size_t CacheManager::time_left_drop_cache_ = 0;
size_t CacheManager::time_left_sync_res_ = 0;

namespace CacheManagerConsole
{
    void RegisterCommands();
}

ErrorCode CacheManager::init(
                             Config & conf)
{
    Config::ConfigItem params[] = {
        CONFIG_PARAM(max_block_cache, Config::allow_get), 
        CONFIG_PARAM(cache_block_size, Config::allow_get), 
        CONFIG_PARAM(interval_statistics, Config::allow_get | Config::allow_set), 
        CONFIG_PARAM(interval_drop_cache, Config::allow_get | Config::allow_set), 
        CONFIG_PARAM(interval_sync_res, Config::allow_get | Config::allow_set), 
        CONFIG_PARAM(drop_percent_period, Config::allow_get | Config::allow_set), 
        CONFIG_PARAM(drop_percent_urgy, Config::allow_get | Config::allow_set), 
        CONFIG_PARAM(block_keep_time, Config::allow_get | Config::allow_set), 
        CONFIG_PARAM(dead_block_keep_time, Config::allow_get | Config::allow_set), 
        CONFIG_PARAM(priority_keep_percent, Config::allow_get | Config::allow_set), 
        CONFIG_PARAM(log_level, Config::allow_get | Config::allow_set), 
    };

    conf.register_module("CacheManager", sizeof(params) / sizeof(params[0]), params);

    // 最小值，下限
    size_t adjust_size = BLOCK_SIZE / 8;
    // 向上取整
    while (adjust_size < cache_block_size)
        adjust_size *= 2;
    cache_block_size = adjust_size;
    piece_per_cache_block_ = cache_block_size / PIECE_SIZE;

    return ERROR_SUCCEED;
}

ErrorCode CacheManager::start(
                              boost::asio::io_service & io_service)
{
    io_service_ = &io_service;

    cache_ = new (SHID_CACHE) Cache;
    cache_->all_stat.block_size = cache_block_size;

    DiskManager::ResourceMap const & ress = DiskManager::GetResource();

    SharedMemoryPool::set_min_block_size((sizeof(ResCache) + sizeof(BlockCache) * 512) * 32);

    size_t size = 0;
    ResId last_rid("00000000000000000000000000000000");
    for (DiskManager::ResourceMap::const_iterator i = ress.begin(); i != ress.end(); ++i) {
        if (i->rid != last_rid) {
            size += sizeof(ResCache);
            size += sizeof(BlockCache) * ((i->size - 1) / cache_block_size + 1);
            last_rid = i->rid;
        }
    }
    if (size && !SharedMemoryPool::prepare(size * 9 / 8))
        return ERROR_NO_MOMERY;

    last_rid = "00000000000000000000000000000000";
    for (DiskManager::ResourceMap::const_iterator i = ress.begin(); i != ress.end(); ++i) {
        if (i->rid != last_rid) {
            ErrorCode ret = AddResource2(i->rid, i->size);
            if (ret != ERROR_SUCCEED)
                return ret;
            last_rid = i->rid;
        }
    }

    int ret = MemoryPool::init(cache_block_size, max_block_cache);
    if (ret != 0) {
        logger.Log(Logger::kLevelError, log_level, "[CacheManager::init] MemoryPool::init failed(%d)", ret);
        return ERROR_ERROR;
    }

    // 为DiskManager
    max_block_cache -= DiskManager::CacheNeed();

    now_ = time(NULL);
    expire_delim_ = now_ - dead_block_keep_time;

    boost::posix_time::ptime now = 
        boost::posix_time::second_clock::universal_time();

    PeriodAction::register_action(
        "CacheManagerTick", 
        boost::bind(CacheManager::Tick), 
        now, 
        boost::posix_time::seconds(1));

    time_left_statistics_ = interval_statistics;
    time_left_drop_cache_ = interval_drop_cache;
    time_left_sync_res_ = interval_sync_res;

    CacheManagerConsole::RegisterCommands();

    return ERROR_SUCCEED;
}

ErrorCode CacheManager::stop()
{
    return ERROR_SUCCEED;
}

ErrorCode CacheManager::AddResource(
                                    ResId const & rid, 
                                    size_t disk)
{
    logger.Log(Logger::kLevelEvent, log_level, "[CacheManager::AddResource] (rid = %s, disk = %u)", 
        rid.c_str(), disk);

    Lock::scoped_lock lock(mutex_);

    ResCache::pointer res;
    BlockCache::pointer block;
    ErrorCode ret(ERROR_SUCCEED);
    if (FindBlock(rid, 0, res, block) == ERROR_SUCCEED)
        ret = ERROR_COPY_ALREADY_EXIST;
    size_t size;
    if (ret == ERROR_SUCCEED) {
        ret = DiskManager::AddResource(rid, disk, size);
    }
    if (ret == ERROR_SUCCEED) {
        ret = AddResource2(rid, size);
    }

    logger.Log(Logger::kLevelEvent, log_level, "[CacheManager::AddResource] finish (err %u:%s)", 
        ret.value(), ErrorMsg::error_message(ret).c_str());

    return ret;
}

void CacheManager::DeleteResource(
                                  ResId const & rid, 
                                  Util::Action::response_type const & r)
{
    logger.Log(Logger::kLevelEvent, log_level, "[CacheManager::DeleteResource] (rid = %s)", 
        rid.c_str());

    Lock::scoped_lock lock(mutex_);

    Cache::ResMap::iterator i = cache_->res_map_.find(rid);
    if (i == cache_->res_map_.end()) {
        logger.Log(Logger::kLevelAlarm, log_level, "[CacheManager::DeleteResource] not exist!");
        r(ERROR_COPY_NOT_EXIST, "");
        return;
    }
    // 从索引表里删除，后续请求就找不到该影片了
    Cache::ResMapNode::pointer node = cache_->res_map_.deref(i);
    ResCache::pointer res = node->second;
    cache_->res_map_.erase(i);
    delete node;
    cache_->res_orders.erase(res);
    res->status = ResCache::deleting;

    DiskManager::DeleteResource(rid, io_service_->wrap(boost::bind(
        CacheManager::HandleDeleteResource, _1, _2, res, r)));
}

void CacheManager::SyncResource(
                                Util::Action::response_type const & r)
{
    logger.Log(Logger::kLevelDebug, log_level, "[CacheManager::SyncResource]");

    DiskManager::SyncResource(boost::bind(
        CacheManager::HandleSyncResource, _1, _2, r));
}

ErrorCode CacheManager::PauseResource(
                                      ResId const & rid)
{
    logger.Log(Logger::kLevelEvent, log_level, "[CacheManager::PauseResource] (rid = %s)", 
        rid.c_str());

    // 这个好像不需要加锁
    Lock::scoped_lock lock(mutex_);

    Cache::ResMap::iterator i = cache_->res_map_.find(rid);
    if (i == cache_->res_map_.end()) {
        logger.Log(Logger::kLevelAlarm, log_level, "[CacheManager::PauseResource] not exist!");
        return ERROR_MOVIE_NOT_EXIST;
    }
    ResCache::pointer res = i->second;
    if (res->status == ResCache::normal) {
        res->status = ResCache::paused;
        cache_->num_paused++;
        return ERROR_SUCCEED;
    } else {
        return ERROR_SUCCEED_WITH_INFO;
    }
}

ErrorCode CacheManager::ResumeResource(
                                       ResId const & rid)
{
    logger.Log(Logger::kLevelEvent, log_level, "[CacheManager::ResumeResource] (rid = %s)", 
        rid.c_str());

    // 这个好像不需要加锁
    Lock::scoped_lock lock(mutex_);

    Cache::ResMap::iterator i = cache_->res_map_.find(rid);
    if (i == cache_->res_map_.end()) {
        logger.Log(Logger::kLevelAlarm, log_level, "[CacheManager::ResumeResource] not exist!");
        return ERROR_MOVIE_NOT_EXIST;
    }
    ResCache::pointer res = i->second;
    if (res->status == ResCache::paused) {
        res->status = ResCache::normal;
        cache_->num_paused--;
        return ERROR_SUCCEED;
    } else {
        return ERROR_SUCCEED_WITH_INFO;
    }
}

ErrorCode CacheManager::AddResource2(
                                     ResId const & rid, 
                                     size_t size)
{
    logger.Log(Logger::kLevelDebug1, log_level, "[CacheManager::AddResource2] (rid = %s, size = %u)", 
        rid.c_str(), size);

    ErrorCode ret(ERROR_SUCCEED);
    ResCache::pointer res = new ResCache(size, cache_block_size);
    Cache::ResMapNode::pointer node = new Cache::ResMapNode(rid, res);
    if (!res || !res->blocks || !node) {
        ret = ERROR_NO_MOMERY;
    } else {
        cache_->res_map_.insert(node);
        cache_->res_orders.insert(res);
        cache_->num_res++;
        cache_->num_block += res->num_block;
    }

    return ret;
}

void CacheManager::HandleDeleteResource(
                                        ErrorCode err, 
                                        std::string const & msg, 
                                        ResCache::pointer res,  
                                        Util::Action::response_type const & r)
{
    Lock::scoped_lock lock(mutex_);

    logger.Log(Logger::kLevelEvent, log_level, "[CacheManager::HandleDeleteResource] (err = %u:%s)", 
        err.value(), ErrorMsg::error_message(err).c_str());

    for (size_t i = 0; i < res->num_block; i++) {
        if (res->blocks[i].status == BlockCache::ready) {
            if (res->blocks[i].stat.expire < expire_delim_) { // 在队列2中
                cache_->ReleaseBlock(Cache::block_queue_2, &res->blocks[i]);
            } else {
                cache_->ReleaseBlock(Cache::block_queue_1, &res->blocks[i]);
            }
        }
    }
    cache_->num_res--;
    cache_->num_block -= res->num_block;
    delete res;
    r(err, msg);
}

void CacheManager::HandleSyncResource(
                                      Util::ErrorCode err, 
                                      std::string const & msg, 
                                      Util::Action::response_type const & r)
{
    if (err != ERROR_SUCCEED) {
        r(err, msg);
        return;
    }

    Lock::scoped_lock lock(mutex_);

    ResId last_rid("00000000000000000000000000000000");
    std::map<ResId, size_t> ress_old;
    std::map<ResId, size_t> ress_new;
    for (Cache::ResMap::const_iterator i = cache_->res_map_.begin(); i != cache_->res_map_.end(); ++i) {
        ress_old.insert(std::make_pair(i->first, 0));
    }
    DiskManager::ResourceMap const & ress = DiskManager::GetResource();
    for (DiskManager::ResourceMap::const_iterator i = ress.begin(); i != ress.end(); ++i) {
        if (i->rid != last_rid) {
            ress_new.insert(std::make_pair(i->rid, i->size));
        }
    }
    std::map<ResId, size_t> ress_add = ress_new - ress_old;
    std::map<ResId, size_t> ress_del = ress_old - ress_new;
    for (std::map<ResId, size_t>::const_iterator i = ress_del.begin(); i != ress_del.end(); ++i) {
        Cache::ResMap::iterator j = cache_->res_map_.find(i->first);
        if (j == cache_->res_map_.end()) {
            continue;
        }
        logger.Log(Logger::kLevelEvent, log_level, "[CacheManager::HandleSyncResource] del res (rid = %s)", 
            i->first.c_str());
        // 从索引表里删除，后续请求就找不到该影片了
        Cache::ResMapNode::pointer node = cache_->res_map_.deref(j);
        ResCache::pointer res = node->second;
        cache_->res_map_.erase(j);
        delete node;
        cache_->res_orders.erase(res);
        res->status = ResCache::deleting;
        for (size_t i = 0; i < res->num_block; i++) {
            if (res->blocks[i].status == BlockCache::ready) {
                if (res->blocks[i].stat.expire < expire_delim_) { // 在队列2中
                    cache_->ReleaseBlock(Cache::block_queue_2, &res->blocks[i]);
                } else {
                    cache_->ReleaseBlock(Cache::block_queue_1, &res->blocks[i]);
                }
            }
        }
        cache_->num_res--;
        cache_->num_block -= res->num_block;
        delete res;
    }
    for (std::map<ResId, size_t>::const_iterator i = ress_add.begin(); i != ress_add.end(); ++i) {
        ResCache::pointer res;
        BlockCache::pointer block;
        logger.Log(Logger::kLevelEvent, log_level, "[CacheManager::HandleSyncResource] add res (rid = %s, size = %u)", 
            i->first.c_str(), i->second);
        ErrorCode ret = AddResource2(i->first, i->second);
        if (ret != ERROR_SUCCEED) {
            r(ret, "");
            return;
        }
    }
    r(ERROR_SUCCEED, "");
}

static size_t const ZERO_SUB_PIECE = 0;
static size_t const RETURN_NO_DATA = 0;

void CacheManager::ReadSubPiece(
                                ResId const & mid, 
                                size_t index_piece, // index of piece
                                size_t index_sub_piece,  // index of sub piece
                                unsigned char * data_buf, 
                                response_type const & r)
{
    Lock::scoped_lock lock(mutex_);

    ResCache::pointer res = NULL;
    BlockCache::pointer blk = NULL;

    // 检查请求有效性
    ErrorCode err = FindBlock(mid, index_piece / piece_per_cache_block_, res, blk);
    if (err == ERROR_SUCCEED) {
        if (index_sub_piece >= SUB_PIECE_PER_PIECE)
            err = ERROR_INVALID_ARGV;
    }
    if (err != ERROR_SUCCEED) {
        r(err, RETURN_NO_DATA);
        return;
    }

    cache_->all_stat.visit++;
    res->stat.visit++;
    blk->stat.visit++;

    if (res->status != ResCache::normal) {
        cache_->all_stat.refused++;
        res->stat.refused++;
        blk->stat.refused++;
        r(ERROR_IO_BUSY, RETURN_NO_DATA);
        return;
    }

    size_t size = RETURN_NO_DATA;

    if (blk->status == BlockCache::free) {

        cache_->all_stat.load++;
        res->stat.load++;
        blk->stat.load++;

        err = DiskManager::ReadData(mid, index_piece / piece_per_cache_block_ * cache_block_size, cache_block_size, res->stat.load_priority, 
            io_service_->wrap(boost::bind(CacheManager::HandleBlockRead, _1, _2, res, blk)));
        if (err == ERROR_PROCESSING) {
            blk->status = BlockCache::initing;
            assert(blk->delay_read_list_ == NULL);
            // 利用data_buf直接构造DelayRead对象
            blk->delay_read_list_ = 
                new (data_buf) DelayRead(blk->delay_read_list_, index_piece % piece_per_cache_block_, index_sub_piece, data_buf, r);;
        }
    } else if (blk->status == BlockCache::initing) {

        // 利用data_buf直接构造DelayRead对象
        blk->delay_read_list_ = 
            new (data_buf) DelayRead(blk->delay_read_list_, index_piece % piece_per_cache_block_, index_sub_piece, data_buf, r);;
        err = ERROR_PROCESSING;
    } else if (blk->status == BlockCache::ready) {

        cache_->all_stat.hit++;
        res->stat.hit++;
        blk->stat.hit++;

        // 更新块的有效期截止时间
        time_t t = now_ + block_keep_time - block_keep_time * (index_piece % piece_per_cache_block_) / piece_per_cache_block_;
        if (blk->stat.expire < t) {
            if (blk->stat.expire < expire_delim_) { // 在队列2中
                //logger.Log(Logger::kLevelDebug1, log_level, "[CacheManager::ReadSubPiece] move to q1 (blk = %p, expire = -%u)", 
                //    blk, now_ - blk->stat.expire);
                cache_->all_stat.hit2++;
                res->stat.hit2++;
                cache_->all_stat.num_cache2--;
                res->stat.num_cache2--;
                res->block_orders.erase(blk);
            } else {
                cache_->block_orders1.erase(blk);
            }
            blk->stat.expire = t;
            cache_->block_orders1.insert(blk);
        }

        SharePtr ptr(blk->data);
        unsigned char * buf = (unsigned char *)ptr;
#ifdef USE_AWE
        if (buf == NULL) {
            logger.Log(Logger::kLevelDebug, log_level, "[CacheManager::ReadSubPiece] pool full (used = %u, used_virtual = %u)", 
                MemoryPool::num_used(), MemoryPool::num_used_virtual());
            err = ERROR_NO_MOMERY;
        } else {
#endif
            if (index_sub_piece == ZERO_SUB_PIECE) {
                memcpy(data_buf, buf + (index_piece % piece_per_cache_block_) * PIECE_SIZE, ZERO_SUB_PIECE_SIZE);
                size = ZERO_SUB_PIECE_SIZE;
            } else {
                memcpy(data_buf, buf + (index_piece % piece_per_cache_block_) * PIECE_SIZE + (index_sub_piece) * OTHER_SUB_PIECE_SIZE + ZERO_SUB_PIECE_SIZE - OTHER_SUB_PIECE_SIZE, OTHER_SUB_PIECE_SIZE);
                size = OTHER_SUB_PIECE_SIZE;
            }
            err = ERROR_SUCCEED;
#ifdef USE_AWE
        }
#endif
    } else {
        err = ERROR_ERROR;
    }

    // 如果没有后续处理过程，直接应答
    if (err != ERROR_PROCESSING) {
        if (err == ERROR_SUCCEED) {
            cache_->all_stat.answer++;
            res->stat.answer++;
            blk->stat.answer++;
        } else {
            cache_->all_stat.refused++;
            res->stat.refused++;
            blk->stat.refused++;
        }
        r(err, size);
    }
}

void CacheManager::HandleBlockRead(
                                   ErrorCode err, 
                                   WeakPtr const & data, 
                                   ResCache::pointer res, 
                                   BlockCache::pointer blk)
{
    DelayRead * dr = NULL;

    //logger.Log(Logger::kLevelDebug1, log_level, "[CacheManager::HandleBlockRead]");

    //{ // 以下代码加锁执行

    Lock::scoped_lock lock(mutex_);

    //logger.Log(Logger::kLevelDebug, log_level, "[CacheManager::HandleBlockRead] enter lock");

    assert(blk->status == BlockCache::initing);

    dr = blk->delay_read_list_;
    blk->delay_read_list_ = NULL;

    if (err == ERROR_SUCCEED) {
        //logger.Log(Logger::kLevelDebug, log_level, "[CacheManager::ReadSubPiece] blk = %p, data = %p", 
        //    blk, data);

        // 处理延迟读
        SharePtr ptr(data);
        unsigned char * buf = (unsigned char *)ptr;

#ifdef USE_AWE
        if (buf == NULL) {
            logger.Log(Logger::kLevelDebug, log_level, "[CacheManager::ReadSubPiece] pool full (used = %u, used_virtual = %u)", 
                MemoryPool::num_used(), MemoryPool::num_used_virtual());
        } else {
#endif
            while (dr) {
                // dr的内存就是dr->data_buf，需要将有用的变量制作拷贝
                DelayRead dr1 = *dr;
                dr->~DelayRead();

                if (dr1.index_sub_piece == ZERO_SUB_PIECE) {
                    memcpy(dr1.data_buf, buf + (dr1.index_piece % piece_per_cache_block_) * PIECE_SIZE, ZERO_SUB_PIECE_SIZE);
                    dr1.resp(err, ZERO_SUB_PIECE_SIZE);
                } else {
                    memcpy(dr1.data_buf, buf + (dr1.index_piece % piece_per_cache_block_) * PIECE_SIZE + (dr1.index_sub_piece) * OTHER_SUB_PIECE_SIZE + ZERO_SUB_PIECE_SIZE - OTHER_SUB_PIECE_SIZE, OTHER_SUB_PIECE_SIZE);
                    dr1.resp(err, OTHER_SUB_PIECE_SIZE);
                }

                dr = dr1.next;

                cache_->all_stat.answer++;
                res->stat.answer++;
                blk->stat.answer++;
            }
#ifdef USE_AWE
        }
#endif
    } else {
        while (dr) {
            DelayRead * dr1 = dr;
            dr = dr->next;
            dr1->resp(err, RETURN_NO_DATA);
            dr1->~DelayRead();

            cache_->all_stat.refused++;
            res->stat.refused++;
            blk->stat.refused++;
        }
    }

    if (err == ERROR_SUCCEED) {
        // 加载成功，扣除积分
        if (res->stat.load_priority >= cache_->all_stat.load_priority)
            res->stat.load_priority -= cache_->all_stat.load_priority;
        else
            res->stat.load_priority = 0;

        // 处理延迟读
        blk->data = data;
        blk->status = BlockCache::ready;
        blk->myres = res;

        cache_->all_stat.num_cache++;
        res->stat.num_cache++;

        cache_->all_stat.loaded++;
        res->stat.loaded++;
        blk->stat.loaded++;

        time_t t = now_ + block_keep_time;
        blk->stat.expire = t;
        cache_->block_orders1.insert(blk);

        if (cache_->all_stat.num_cache >= max_block_cache) {
            DropCache(drop_percent_urgy);
        }
    } else {
        blk->status = BlockCache::free;
    }

}

ErrorCode CacheManager::FindBlock(
                                  ResId const & rid, 
                                  size_t index_block, 
                                  ResCache::pointer& res, 
                                  BlockCache::pointer& block)
{
    Cache::ResMap::iterator i = cache_->res_map_.find(rid);
    if (i == cache_->res_map_.end())
        return (ERROR_MOVIE_NOT_EXIST);
    res = i->second;
    if (index_block >= res->num_block)
        return ERROR_BLOCK_NOT_EXIST;
    block = &res->blocks[index_block];
    return ERROR_SUCCEED;
}

void CacheManager::Tick()
{
    Lock::scoped_lock lock(mutex_);

    now_++;

    //防止修改配置block_max_keep_time配置值（增大），使得expire_delim_变小
    if (expire_delim_ < now_ - (time_t)dead_block_keep_time) {

        expire_delim_ = now_ - (time_t)dead_block_keep_time;

        //logger.Log(Logger::kLevelDebug1, log_level, "[CacheManager::Tick] move to q2 (expire_delim_ = %u)", 
        //    expire_delim_);

        size_t moved = 0;
        for (BlockCache::pointer blk = cache_->block_orders1.first(); blk && blk->stat.expire < expire_delim_; ) {
            assert(blk->status == BlockCache::ready);
            //logger.Log(Logger::kLevelDebug1, log_level, "[CacheManager::Tick] move to q2 (blk = %p, expire = -%u)", 
            //    blk, now_ - blk->stat.expire);
            BlockCache::pointer blk1 = blk;
            blk = cache_->block_orders1.erase(blk);
            blk1->myres->block_orders.insert(blk1);
            cache_->all_stat.num_cache2++;
            blk1->myres->stat.num_cache2++;
            moved++;
        }

        //logger.Log(Logger::kLevelDebug1, log_level, "[CacheManager::Tick] move to q2 (moved = %u)", 
        //    moved);
    }
    if (time_left_statistics_ && --time_left_statistics_ == 0) {
        Statistics();
        time_left_statistics_ = interval_statistics;
    }
    if (time_left_drop_cache_ && --time_left_drop_cache_ == 0) {
        DropCache(drop_percent_period);
        time_left_drop_cache_ = interval_drop_cache;
    }

    if (time_left_sync_res_ && --time_left_sync_res_ == 0) {
        SyncResource(Action::nop_resp);
        time_left_sync_res_ = interval_sync_res;
    }

}

void CacheManager::Statistics()
{
#ifdef USE_AWE
    logger.Log(Logger::kLevelDebug1, log_level, "[CacheManager::Statistics] pool statistics (used = %u, used_virtual = %u)", 
        MemoryPool::num_used(), MemoryPool::num_used_virtual());
#else
    logger.Log(Logger::kLevelDebug1, log_level, "[CacheManager::Statistics] pool statistics (used = %u)", 
        MemoryPool::num_used());
#endif

    for (Cache::ResMap::const_iterator ir = cache_->res_map_.begin(); ir != cache_->res_map_.end(); ++ir) {
        ResCache::pointer res = ir->second;
        if (res->status != ResCache::normal)
            continue;
        for (size_t j = 0; j < res->num_block; j++) {
            BlockCache::pointer blk = &res->blocks[j];
            blk->stat.visit.tick(5);
            blk->stat.hit.tick();
            blk->stat.load.tick();
            blk->stat.loaded.tick();
            blk->stat.answer.tick();
            blk->stat.refused.tick();
            //if (blk->status == BlockCache::ready && blk->stat.expire < expire_delim_) {
            //    res->block_orders.erase(blk);
            //    blk->stat.cost = blk->stat.visit.smooth_rate;
            //    res->block_orders.insert(blk);
            //}
        }
        res->stat.visit.tick(10);
        res->stat.hit.tick();
        res->stat.load.tick();
        res->stat.loaded.tick();
        res->stat.answer.tick();
        res->stat.refused.tick();
        res->stat.load_priority = 
            (res->stat.load_priority * priority_keep_percent + 
            res->stat.visit.rate * (100 - priority_keep_percent)) / 100; // 请求次数换算加载积分
        cache_->res_orders.erase(res);
        res->stat.cost = res->stat.visit.smooth_rate;
        cache_->res_orders.insert(res);
    }
    cache_->all_stat.visit.tick(90);
    cache_->all_stat.hit.tick();
    cache_->all_stat.load.tick();
    cache_->all_stat.loaded.tick();
    cache_->all_stat.answer.tick();
    cache_->all_stat.refused.tick();
    cache_->all_stat.load_priority = cache_->all_stat.loaded.rate ? cache_->all_stat.visit.rate / cache_->all_stat.loaded.rate : 0; // 平均加载消耗积分
}

size_t CacheManager::DropCache(
                               size_t percent)
{
    size_t drop_num = cache_->all_stat.num_cache * percent / 100;

    logger.Log(Logger::kLevelDebug1, log_level, "[CacheManager::DropCache] (used = %u, drop_num = %u[%u%%])", 
        cache_->all_stat.num_cache, drop_num, percent);

    size_t drop_left = drop_num;

    // 从已经超过停尸时间的块中丢弃
    for (ResCache::pointer res = cache_->res_orders.first(); res && drop_left; res = cache_->res_orders.next(res)) {
        //logger.Log(Logger::kLevelDebug1, log_level, "[CacheManager::DropCache] res (cost = %u)", 
        //    res->stat.cost);
        for (BlockCache::pointer blk = res->block_orders.first(); blk && drop_left; ) {
            //logger.Log(Logger::kLevelDebug1, log_level, "[CacheManager::DropCache] (cost = %u)", 
            //    blk->stat.cost);
            //logger.Log(Logger::kLevelDebug, log_level, "[CacheManager::DropCache] blk = %p, data = %p", 
            //    blk, blk->data);

            blk = cache_->ReleaseBlock(Cache::block_queue_2, blk);
            drop_left--;
        }
    }
    // 不够的话，从队列1中丢弃已经寿终正寝的数据块
    if (drop_left > 0) {
        logger.Log(Logger::kLevelDebug1, log_level, "[CacheManager::DropCache] (continue try 1, drop_left = %u)", 
            drop_left);
        for (BlockCache::pointer blk = cache_->block_orders1.first(); blk && blk->stat.expire < now_ && drop_left; ) {
            //logger.Log(Logger::kLevelDebug1, log_level, "[CacheManager::DropCache] (expire = -%u)", 
            //    now_ - blk->stat.expire);
            //logger.Log(Logger::kLevelDebug, log_level, "[CacheManager::DropCache] blk = %p, data = %p", 
            //    blk, blk->data);

            blk = cache_->ReleaseBlock(Cache::block_queue_1, blk);
            drop_left--;
        }
    }
    // 还不够的话，丢弃还没有死亡的数据块，但只丢弃到需求的一半
    if (drop_left > drop_num / 2) {
        logger.Log(Logger::kLevelDebug1, log_level, "[CacheManager::DropCache] (continue try 2, drop_left = %u)", 
            drop_left);
        drop_left -= drop_num / 2;
        drop_num -= drop_num / 2;
        for (BlockCache::pointer blk = cache_->block_orders1.first(); blk && drop_left; ) {
            //logger.Log(Logger::kLevelDebug1, log_level, "[CacheManager::DropCache] (expire = %u)", 
            //    blk->stat.expire - now_);
            //logger.Log(Logger::kLevelDebug, log_level, "[CacheManager::DropCache] blk = %p, data = %p", 
            //    blk, blk->data);

            blk = cache_->ReleaseBlock(Cache::block_queue_1, blk);
            drop_left--;
        }
    }

    return drop_num - drop_left;
}

ErrorCode CacheManager::GetResourceRequestRate(
    ResId const & rid, 
    size_t & rate)
{
    Cache::ResMap::iterator i = cache_->res_map_.find(rid);
    if (i == cache_->res_map_.end())
        return (ERROR_MOVIE_NOT_EXIST);
    rate = i->second->stat.visit.smooth_rate;
    return (ERROR_SUCCEED);
}

size_t CacheManager::GetPausedResource(
                                       std::vector<ResId> & rids)
{
    rids.reserve(cache_->num_paused);
    rids.clear();
    for (Cache::ResMap::const_iterator ir = cache_->res_map_.begin(); ir != cache_->res_map_.end(); ++ir) {
        if (ir->second->status == ResCache::paused)
            rids.push_back(ir->first);
    }
    return rids.size();
}

namespace CacheManagerConsole
{
    void AddResource(
        UserIf::get_arg_type const & g, 
        Action::response_type const & r)
    {
        std::string rid, disk;
        if (g(rid)) {
            if (g(disk = UserIf::ignore)) {
                r(CacheManager::AddResource(rid, Parse::parse<size_t>(disk)), "");
            } else {
                r(CacheManager::AddResource(rid, (size_t)-1), "");
            }
        }
    }

    void DeleteResource(
        UserIf::get_arg_type const & g, 
        Action::response_type const & r)
    {
        std::string rid;
        if (g(rid)) {
            CacheManager::DeleteResource(rid, r);
        }
    }

    void PauseResource(
        UserIf::get_arg_type const & g, 
        Action::response_type const & r)
    {
        std::string rid;
        if (g(rid)) {
            r(CacheManager::PauseResource(rid), "");
        }
    }

    void ResumeResource(
        UserIf::get_arg_type const & g, 
        Action::response_type const & r)
    {
        std::string rid;
        if (g(rid)) {
            r(CacheManager::ResumeResource(rid), "");
        }
    }

    void SyncResource(
        UserIf::get_arg_type const & g, 
        Action::response_type const & r)
    {
        CacheManager::SyncResource(r);
    }

    void response_delete_buf(
        ErrorCode err, 
        size_t size, 
        unsigned char * buf, 
        Action::response_type const & resp)
    {
        if (err == ERROR_SUCCEED)
            resp(err, "size:" + Format::format(size));
        else
            resp(err, ErrorMsg::error_message(err));
        delete [] buf;
    }

    void GetSubPiece(
        UserIf::get_arg_type const & g, 
        Action::response_type const & r)
    {
        std::string rid;
        std::string piece;
        std::string sub_piece;
        if (g(rid) & g(piece) & g(sub_piece)) {
            unsigned char * buf = new unsigned char[ZERO_SUB_PIECE_SIZE];
            CacheManager::ReadSubPiece(rid, 
                Parse::parse<size_t>(piece), 
                Parse::parse<size_t>(sub_piece), buf, 
                boost::bind(response_delete_buf, _1, _2, buf, r));
        }
    }

    void RegisterCommands()
    {
        UserIf::SubCmd cmd;
        cmd.regCmd("add", AddResource);
        cmd.regCmd("del", DeleteResource);
        cmd.regCmd("pause", PauseResource);
        cmd.regCmd("resume", ResumeResource);
        cmd.regCmd("sync", SyncResource);
        cmd.regCmd("get-sub-piece", GetSubPiece);
        UserIf::regCmd("cache", cmd);
    }

}
