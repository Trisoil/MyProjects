// DiskManager.cpp

#include "Common.h"
#include "DiskManager.h"
#include "DiskProxy.h"
#include "DiskStatistics.h"
#include "CacheManager.h"

#include <Action/Action.h>
#include <Action/ParallelAction.h>
#include <Framework/PeriodAction.h>
#include <Generic/Operator.h>
#include <Container/SetCalc.h>
using namespace Util;

#include <boost/range.hpp>

std::vector<std::string> DiskManager::disks;
size_t DiskManager::max_queue_size = 4;
size_t DiskManager::interval_statistics = 10;
size_t DiskManager::num_drop_alarm = 8;
size_t DiskManager::time_busy_refuse = 60;
size_t DiskManager::copy_percent = 10;
size_t DiskManager::pause_percent = 10;
size_t DiskManager::resume_percent = 10;
bool DiskManager::enable_priority = false;
size_t DiskManager::delete_type = DiskProxy::move_to_recycle;
int DiskManager::log_level = 2;

boost::asio::io_service * DiskManager::io_service_ = NULL;
boost::asio::io_service * DiskManager::io_service_adjust_ = NULL;
boost::thread * DiskManager::thread_ = NULL;
std::vector<DiskProxy *> DiskManager::disks_;
DiskManager::ResourceMap DiskManager::res_map_;
Util::List<CopyTask> * DiskManager::copy_tasks_ = NULL;
size_t DiskManager::time_left_statistics_ = 0;

struct CopyTask
    : Util::ListHook<CopyTask>::type
{
    ResId rid;
    size_t size;
    size_t idisk;
    size_t ndest;
    size_t * pdests;
    size_t dests[4];
    size_t pos;
    size_t idest;
    SharePtr data;
};

namespace DiskManagerConsole
{
    void RegisterCommands();
}

ErrorCode DiskManager::init(
                    Config & conf)
{
    Config::ConfigItem params[] = {
        CONFIG_PARAM(disks, Config::allow_get), 
        CONFIG_PARAM(max_queue_size, Config::allow_get | Config::allow_set), 
        CONFIG_PARAM(interval_statistics, Config::allow_get | Config::allow_set), 
        CONFIG_PARAM(num_drop_alarm, Config::allow_get | Config::allow_set), 
        CONFIG_PARAM(time_busy_refuse, Config::allow_get | Config::allow_set), 
        CONFIG_PARAM(copy_percent, Config::allow_get | Config::allow_set), 
        CONFIG_PARAM(pause_percent, Config::allow_get | Config::allow_set), 
        CONFIG_PARAM(resume_percent, Config::allow_get | Config::allow_set), 
        CONFIG_PARAM(enable_priority, Config::allow_get | Config::allow_set), 
        CONFIG_PARAM(delete_type, Config::allow_get | Config::allow_set), 
        CONFIG_PARAM(log_level, Config::allow_get | Config::allow_set), 
    };

    conf.register_module("DiskManager", sizeof(params) / sizeof(params[0]), params);

    if (disks.empty()) {
        logger.Log(Logger::kLevelError, log_level, "[DiskManager::init] no disk!"); 
        return ERROR_ERROR;
    }

    return ERROR_SUCCEED;
}

ErrorCode DiskManager::start(
                     boost::asio::io_service & io_service)
{
    io_service_ = &io_service;

    std::vector<std::map<ResId, size_t> > ress(disks.size());
    std::vector<size_t> loaded(disks.size(), 0);

    for (size_t i = 0; i < disks.size(); ++i) {
        ErrorCode error;
        disks_.push_back(new DiskProxy(disks[i], error));
        if (error != ERROR_SUCCEED) {
            return error;
        }
        disks_[i]->GetResource(ress[i], boost::bind(&Operator::assign<size_t>, boost::ref(loaded[i]), 1));
        disks_[i]->Start();
    }

    size_t loaded_all = 0;
    boost::xtime xt;
    boost::xtime_get(&xt, boost::TIME_UTC);
    do {
        xt.sec += 1;
        boost::thread::sleep(xt);
        loaded_all = 0;
        for (size_t i = 0; i < disks_.size(); ++i) {
            loaded_all += loaded[i];
        }
    } while (loaded_all != disks.size());

    for (size_t i = 0; i < disks_.size(); ++i) {
        std::map<ResId, size_t> & ress1 = ress[i];
        disks_[i]->stat()->num_res = ress1.size();
        for (std::map<ResId, size_t>::const_iterator j = ress1.begin(); j != ress1.end(); ++j) {
            Resource * res = new Resource;
            res->rid = j->first;
            res->idisk = i;
            res->size = j->second;
            res->status = Resource::working;
            res_map_.insert(res);
        }
    }

    copy_tasks_ = new Util::List<CopyTask>;

    DiskManagerConsole::RegisterCommands();

    boost::posix_time::ptime now = 
        boost::posix_time::second_clock::universal_time();

    PeriodAction::register_action(
        "DiskReaderStatistics", 
        boost::bind(DiskManager::Tick), 
        now, 
        boost::posix_time::seconds(1));

    time_left_statistics_ = interval_statistics;

    io_service_adjust_ = new boost::asio::io_service;
    thread_ = new boost::thread(boost::bind(&boost::asio::io_service::run, io_service_adjust_));

    return ERROR_SUCCEED;
}

ErrorCode DiskManager::stop()
{
    for (size_t i = 0; i < disks.size(); ++i) {
        disks_[i]->Stop();
    }
    delete copy_tasks_;
    return ERROR_SUCCEED;
}

ErrorCode DiskManager::AddResource(
                             ResId const & rid, 
                             size_t disk, 
                             size_t & size)
{
    // 主线程执行

    ErrorCode ret = ERROR_FILE_NOT_EXIST;
    if (disk == (size_t)-1) {
        for (disk = 0; disk < disks_.size(); disk++) {
            ret = disks_[disk]->AddResourceReal(rid, size);
            if (ret != ERROR_FILE_NOT_EXIST) {
                break;
            }
        }
    } else {
        ret = disks_[disk]->AddResourceReal(rid, size);
    }
    if (ret == ERROR_SUCCEED) {
        Resource * res = new Resource;
        res->rid = rid;
        res->idisk = disk;
        res->size = size;
        res->status = Resource::working;
        res_map_.insert(res);
        disks_[disk]->stat()->num_res++;
    }
    return ret;
}

void DiskManager::DeleteResource(
                                 ResId const & rid, 
                                 Util::Action::response_type const & r)
{
    // 主线程执行

    HandleDeleteResource(ERROR_SUCCEED, "to start", rid, r);
}

void DiskManager::HandleDeleteResource(
                                       ErrorCode ec, 
                                       std::string const & msg, 
                                       ResId const & rid, 
                                       Util::Action::response_type const & r)
{
    // 主线程执行

    if (ec != ERROR_SUCCEED) {
        r(ec, msg);
        return;
    }

    ResourceMap::iterator i = res_map_.find(rid);
    if (i == res_map_.end()) {
        r(ERROR_SUCCEED, "");
        return;
    }
    disks_[i->idisk]->DeleteResource(rid, 
        io_service_->wrap(boost::bind(DiskManager::HandleDeleteResource, 
            _1, _2, rid, r)));
    Resource * res = res_map_.deref(i);
    res_map_.erase(i);
    disks_[i->idisk]->stat()->num_res--;
    if (res->status == Resource::paused)
        disks_[i->idisk]->stat()->num_res_paused--;
    delete res;
}

struct SyncResourceStruct
{
    SyncResourceStruct(
        size_t left, 
        Util::Action::response_type const & r)
        : left(left)
        , resp(r)
    {
    }
    size_t left;
    Util::Action::response_type const resp;
};

void DiskManager::SyncResource(
                               Util::Action::response_type const & r)
{
    // 主线程执行
    SyncResourceStruct * sr = new SyncResourceStruct(disks_.size(), r);
    for (size_t i = 0 ; i < disks_.size(); ++i) {
        std::map<ResId, size_t> * ress = new std::map<ResId, size_t>;
        disks_[i]->GetResource(*ress, 
            io_service_->wrap(boost::bind(DiskManager::HandleSyncResource, 
                boost::ref(*sr), i, boost::ref(*ress), _1, _2)));
    }
}

void DiskManager::HandleSyncResource(
                                     SyncResourceStruct & sr, 
                                     size_t idisk, 
                                     std::map<ResId, size_t> & ress, 
                                     ErrorCode err, 
                                     std::string const & msg)
{
    // 主线程执行
    std::map<ResId, size_t> ress_old;
    if (err == ERROR_SUCCEED) {
        for (ResourceMap::iterator i = res_map_.begin(); i != res_map_.end(); ++i) {
            if (i->idisk == idisk)
                ress_old.insert(std::make_pair(i->rid, i->size));
        }
        std::map<ResId, size_t> ress_add = ress - ress_old;
        std::map<ResId, size_t> ress_del = ress_old - ress;
        for (std::map<ResId, size_t>::const_iterator i = ress_del.begin(); i != ress_del.end(); i++) {
            std::pair<ResourceMap::iterator, ResourceMap::iterator> pair = 
                res_map_.equal_range(i->first);
            for (; pair.first != pair.second; ) {
                if (pair.first->idisk == idisk) {
                    Resource * res = res_map_.deref(pair.first);
                    res_map_.erase(pair.first);
                    disks_[idisk]->stat()->num_res--;
                    if (res->status == Resource::paused)
                        disks_[idisk]->stat()->num_res_paused--;
                    delete res;
                    break;
                }
            }
        }
        for (std::map<ResId, size_t>::const_iterator i = ress_add.begin(); i != ress_add.end(); i++) {
            std::pair<ResourceMap::iterator, ResourceMap::iterator> pair = 
                res_map_.equal_range(i->first);
            Resource * res = new Resource;
            res->rid = i->first;
            res->idisk = idisk;
            res->size = i->second;
            res->status = Resource::working;
            res_map_.insert(res);
            disks_[idisk]->stat()->num_res++;
        }
    }
    
    delete &ress;

    if (--sr.left == 0) {
        sr.resp(ERROR_SUCCEED, "");
        delete &sr;
    }
}

ErrorCode DiskManager::ReadData(
                          ResId const & rid, 
                          size_t offset, 
                          size_t size, 
                          size_t priority, 
                          disk_task_response_type const & r)
{
    // 主线程执行

    ErrorCode err = ERROR_COPY_NOT_EXIST;
    std::pair<ResourceMap::iterator, ResourceMap::iterator> pair = 
        res_map_.equal_range(rid);
    ResourceMap temp;
    for (; pair.first != pair.second; ) {
        if (pair.first->status == Resource::paused) {
            break;
        }
        // 在这里控制用不用磁盘排队策略
        if (!enable_priority)
            priority = 0;
        res_map_.deref(pair.first)->req++;
        err = disks_[pair.first->idisk]->ReadData(rid, offset, size, priority, r);
        if (err == ERROR_IO_BUSY) {
            Resource * res = res_map_.deref(pair.first);
            pair.first = res_map_.erase(pair.first);
            temp.insert(res);
        } else {
            break;
        }
    }
    for (ResourceMap::iterator i = temp.begin(); i != temp.end(); ) {
        Resource * res = temp.deref(i);
        i = temp.erase(i);
        // 据说能排在相同Rid的最后
        res_map_.insert(res, pair.second);
    }
    //if (err != ERROR_PROCESSING) {
    //    // 不可能是ERROR_SUCCEED，那样的结果应该异步返回
    //    r(err, NULL);
    //}
    return err;
}

struct DiskStat
{
    size_t idisk;
    DiskStatistics * stat;
    int free_ability; // 负值为超负荷
};

struct CompareRes
{
    bool operator () (
        Resource const * l, 
        Resource const * r)
    {
        return l->req.smooth_rate > r->req.smooth_rate;
    }

};

struct CompareRes1
{
    bool operator () (
        Resource const * l, 
        Resource const * r) const
    {
        return l->idisk < r->idisk; // 访问频率从小到大排列
    }
};

struct CompareRes2
{
    bool operator () (
        Resource const * l, 
        Resource const * r) const
    {
        return l->idisk > r->idisk; // 访问频率从大到小排列
    }
};

struct CompareStat
{
    bool operator () (
        DiskStat const & l, 
        DiskStat const & r) const
    {
        return l.free_ability > r.free_ability;
    }
};

void DiskManager::Tick()
{
    if (time_left_statistics_ == 0 || --time_left_statistics_ > 0) 
        return;
        
    time_left_statistics_ = interval_statistics;

    DiskProxy::Statistics();
    for (ResourceMap::iterator j = res_map_.begin(); j != res_map_.end(); ++j) {
        res_map_.deref(j)->req.tick(10);
    }
    static bool initting = true;
    if (initting && MemoryPool::num_used() * 100 / MemoryPool::capcity() > 95) {
        // 缓存增长结束，可以作调整了
        initting = false;
        for (ResourceMap::iterator j = res_map_.begin(); j != res_map_.end(); ++j) {
            res_map_.deref(j)->req.smooth_rate = res_map_.deref(j)->req.rate;
        }
        for (size_t i = 0; i < disks_.size(); i++) {
            DiskStatistics * stat = disks_[i]->stat();
            stat->recv.smooth_rate = stat->recv.rate;
            stat->drop.smooth_rate = stat->drop.rate;
        }
    }
    if (!copy_tasks_->empty() || initting) // 缓存还在增长中，不作调整
        return;

    size_t disk_num = disks_.size();
    std::vector<DiskStat> disk_stats(disk_num);
    for (size_t i = 0; i < disk_num; i++) {
        disk_stats[i].idisk = i;
        disk_stats[i].free_ability = 0;
        disk_stats[i].stat = disks_[i]->stat();
        if (disk_stats[i].stat->drop.smooth_rate > num_drop_alarm * interval_statistics) {
            disk_stats[i].free_ability = -(int)disk_stats[i].stat->drop.smooth_rate;
        } else if (disk_stats[i].stat->recv.smooth_rate + interval_statistics < disk_stats[i].stat->ability * interval_statistics) {
            disk_stats[i].free_ability = (int)(disk_stats[i].stat->ability * interval_statistics - disk_stats[i].stat->recv.smooth_rate);
        }
    }
    std::sort(disk_stats.begin(), disk_stats.end(), CompareStat());

    std::vector<size_t> dests; // 记录拷贝目标
    // 对每个磁盘
    for (size_t i = disk_num - 1; i != (size_t)-1; i--) {
        size_t idisk = disk_stats[i].idisk;
        if (disk_stats[i].free_ability < 0) {
            logger.Log(Logger::kLevelDebug1, log_level, "[DiskManager::Tick] disk %u busy (exceed = %u)", 
                idisk, -disk_stats[i].free_ability);
            if (disk_stats[0].free_ability > (int)interval_statistics) {
                size_t exceed = -disk_stats[i].free_ability * copy_percent / 100;
                size_t exceed_tmp = exceed;
                std::vector<Resource *> ress;
                size_t res_num = GetResourceOfDisk(idisk, Resource::working, interval_statistics, ress);
                // 按请求量从大到小排序资源
                std::sort(ress.begin(), ress.end(), CompareRes());
                // 对每个资源，作复制调整
                for (size_t k = 0; exceed > interval_statistics && k < res_num && ress[k]->req.smooth_rate > interval_statistics && disk_stats[0].free_ability > (int)interval_statistics; k++) {
                    size_t res_load = ress[k]->req.smooth_rate;
                    logger.Log(Logger::kLevelDebug1, log_level, "[DiskManager::Tick] adjust res %s (res_load = %u)", 
                        ress[k]->rid.c_str(), res_load);
                    size_t left = res_load;
                    if (left > exceed)
                        left = exceed;
                    // 记录磁盘是否已经有该资源了
                    std::vector<bool> has_res(disks_.size(), false);
                    std::pair<ResourceMap::iterator, ResourceMap::iterator> pair = 
                        res_map_.equal_range(ress[k]->rid);
                    for (; pair.first != pair.second; ++pair.first) {
                        has_res[pair.first->idisk] = true;
                    }
                    // 分配调整任务
                    for (size_t j = 0; j < disk_stats.size() && disk_stats[j].free_ability > (int)interval_statistics && left > interval_statistics; j++) {
                        if (has_res[disk_stats[j].idisk])
                            continue;
                        logger.Log(Logger::kLevelDebug1, log_level, "[DiskManager::Tick] shift load to disk %u (free_ability = %u)", 
                            disk_stats[j].idisk, disk_stats[j].free_ability);
                        if (disk_stats[j].free_ability > (int)left) {
                            disk_stats[j].free_ability -= (int)left;
                            left = 0;
                        } else {
                            left -= disk_stats[j].free_ability;
                            disk_stats[j].free_ability  = 0;
                        }
                        dests.push_back(disk_stats[j].idisk);
                    }
                    if (dests.empty())
                        continue;
                    exceed -= (res_load - left);
                    // 生成拷贝任务
                    CopyResource(ress[k]->rid, dests, Action::nop_resp);
                    dests.clear();
                    // 重排序，这个排序不会影响free_ability小于0的磁盘的原先顺序
                    std::sort(&disk_stats[0], &disk_stats[i], CompareStat());
                } // [对每个资源，作复制调整] for (k = 0; exceed > ....
                disk_stats[i].free_ability += exceed_tmp - exceed;
            } // if (disk_stats[0].free_ability > (int)interval_statistics)

            // 如果还忙，并且连续一段时间处于繁忙状态，则暂停服务一部分资源
            if ((size_t)-disk_stats[i].free_ability > num_drop_alarm * interval_statistics) {
                disk_stats[i].stat->busy++;
                if (disk_stats[i].stat->busy * interval_statistics > time_busy_refuse) {
                    // 获取影片的访问频率，并按频率从小到大排序
                    logger.Log(Logger::kLevelDebug1, log_level, "[DiskManager::Tick] disk %u enter busy (exceed = %u)", 
                        idisk, -disk_stats[i].free_ability);
                    size_t exceed = -disk_stats[i].free_ability * pause_percent / 100;
                    std::vector<Resource *> ress;
                    size_t res_num = GetResourceOfDisk(idisk, Resource::working, 0, ress);
                    //logger.Log(Logger::kLevelDebug1, log_level, "[DiskManager::Tick] (res_num = %u)", 
                    //    res_num);
                    for (size_t k = 0; k < res_num; k++) {
                        CacheManager::GetResourceRequestRate(ress[k]->rid, ress[k]->idisk);
                    }
                    //logger.Log(Logger::kLevelDebug1, log_level, "[DiskManager::Tick] starting sort");
                    std::sort(ress.begin(), ress.end(), CompareRes1());
                    // 依次暂停资源服务
                    //logger.Log(Logger::kLevelDebug1, log_level, "[DiskManager::Tick] starting pause");
                    for (size_t k = 0; exceed > interval_statistics && k < res_num; k++) {
                        logger.Log(Logger::kLevelDebug1, log_level, "[DiskManager::Tick] pause res %s (res_load = %u)", 
                            ress[k]->rid.c_str(), ress[k]->req.smooth_rate);
                        CacheManager::PauseResource(ress[k]->rid);
                        ress[k]->status = Resource::paused;
                        disks_[idisk]->stat()->num_res_paused++;
                        if (exceed > ress[k]->req.smooth_rate)
                            exceed -= ress[k]->req.smooth_rate;
                        else
                            exceed = 0;
                        Resource * res = res_map_.prev(ress[k]);
                        while (res && res->rid == ress[k]->rid) {
                            res->status = Resource::paused;
                            disks_[res->idisk]->stat()->num_res_paused++;
                            for (size_t i1 = 0; i1 < i; i1++) {
                                if (disk_stats[i1].idisk == res->idisk) {
                                    disk_stats[i1].free_ability += res->req.smooth_rate;
                                    break;
                                }
                            }
                            res = res_map_.prev(res);
                        }
                        res = res_map_.next(ress[k]);
                        while (res && res->rid == ress[k]->rid) {
                            res->status = Resource::paused;
                            disks_[res->idisk]->stat()->num_res_paused++;
                            for (size_t i1 = 0; i1 < i; i1++) {
                                if (disk_stats[i1].idisk == res->idisk) {
                                    disk_stats[i1].free_ability += res->req.smooth_rate;
                                    break;
                                }
                            }
                            res = res_map_.next(res);
                        }
                    }
                    // 恢复idisk字段
                    for (size_t k = 0; k < res_num; k++) {
                        ress[k]->idisk = idisk;
                    }
                    //logger.Log(Logger::kLevelDebug1, log_level, "[DiskManager::Tick] reset stat");
                    DiskStatistics * stat = disks_[idisk]->stat();
                    stat->recv.smooth_rate = stat->recv.rate;
                    stat->drop.smooth_rate = stat->drop.rate;
                    stat->busy = 0;
                    std::sort(&disk_stats[0], &disk_stats[i], CompareStat());
                } // if (disk_stats[i].stat->busy * interval_statistics > time_busy_refuse)
            } // if (exceed > num_drop_alarm * interval_statistics)
        } else { // if (disk_stats[i].free_ability < 0)
            // 有空闲能力，考虑恢复一部分暂停的影片
            if (disk_stats[i].stat->busy > 0) {
                disk_stats[i].stat->busy--;
                if (disk_stats[i].stat->busy * interval_statistics > time_busy_refuse) {
                    disk_stats[i].stat->busy = time_busy_refuse / interval_statistics;
                }
            }
            if (disk_stats[i].stat->busy == 0 && disk_stats[i].stat->num_res_paused > 0 && disk_stats[i].free_ability > (int)interval_statistics) {
                size_t exceed = disk_stats[i].free_ability * resume_percent;
                logger.Log(Logger::kLevelDebug1, log_level, "[DiskManager::Tick] disk %u enter free (exceed = %u)", 
                    idisk, exceed);
                // 获取该磁盘的暂停资源列表
                std::vector<Resource *> ress;
                size_t res_num = GetResourceOfDisk(idisk, Resource::paused, 0, ress);
                for (size_t k = 0; k < res_num ; ++k) {
                    CacheManager::GetResourceRequestRate(ress[k]->rid, ress[k]->idisk);
                }
                std::sort(ress.begin(), ress.end(), CompareRes2());
                // 依次恢复资源服务
                for (size_t k = 0; exceed > interval_statistics && k < res_num; k++) {
                    if (ress[k]->req.smooth_rate > exceed)
                        continue;
                    logger.Log(Logger::kLevelDebug1, log_level, "[DiskManager::Tick] resume res %s (res_load = %u)", 
                        ress[k]->rid.c_str(), ress[k]->req.smooth_rate);
                    CacheManager::ResumeResource(ress[k]->rid);
                    ress[k]->status = Resource::working;
                    disks_[idisk]->stat()->num_res_paused--;
                    disk_stats[i].stat->num_res_paused--;
                    exceed -= ress[k]->req.smooth_rate;
                    Resource * res = res_map_.prev(ress[k]);
                    while (res && res->rid == ress[k]->rid) {
                        res->status = Resource::working;
                        disks_[res->idisk]->stat()->num_res_paused--;
                        for (size_t i1 = 0; i1 < i; i1++) {
                            if (disk_stats[i1].idisk == res->idisk) {
                                disk_stats[i1].free_ability -= res->req.smooth_rate;
                                break;
                            }
                        }
                        res = res_map_.prev(res);
                    }
                    res = res_map_.next(ress[k]);
                    while (res && res->rid == ress[k]->rid) {
                        res->status = Resource::working;
                        disks_[res->idisk]->stat()->num_res_paused--;
                        for (size_t i1 = 0; i1 < i; i1++) {
                            if (disk_stats[i1].idisk == res->idisk) {
                                disk_stats[i1].free_ability -= res->req.smooth_rate;
                                break;
                            }
                        }
                        res = res_map_.next(res);
                    }
                }
                // 恢复idisk字段
                for (size_t k = 0; k < res_num; k++) {
                    ress[k]->idisk = disk_stats[i].idisk;
                }
                std::sort(&disk_stats[0], &disk_stats[i], CompareStat());
            } // if (disk_stats[i].stat->busy == 0 && disk_stats[i].stat->num_res_paused > 0)
        } // if (disk_stats[i].free_ability < 0) else
    } // 对每个磁盘
}

size_t DiskManager::GetResourceOfDisk(
                                      size_t idisk, 
                                      Resource::StatusEnum status, 
                                      size_t load, 
                                      std::vector<Resource *> & ress)
{
    size_t res_num = disks_[idisk]->stat()->num_res;
    ress.reserve(res_num);
    ress.clear();
    for (ResourceMap::const_iterator j = res_map_.begin(); ress.size() < res_num && j != res_map_.end(); ++j) {
        if (j->idisk == idisk && j->status == status && j->req.smooth_rate > load) {
            ress.push_back(res_map_.deref(j));
        }
    }
    return ress.size();
}

void DiskManager::CopyResource(
                               ResId const & rid, 
                               std::vector<size_t> const & dests, 
                               Util::Action::response_type const & r)
{
    // 主线程执行

    ResourceMap::iterator res = res_map_.find(rid);
    if (res == res_map_.end()) {
        r(ERROR_COPY_NOT_EXIST, "");
        return;
    }
    size_t n = dests.size();
    for (size_t j = 0; j < n; j++) {
        if (dests[j] >= disks_.size()) {
            r(ERROR_NO_SUCH_ITEM, "No disk " + Format::format(dests[j]));
            return;
        }
    }

    CopyTask * task = new CopyTask;
    task->rid = rid;
    task->idisk = res->idisk;
    task->size = res->size;
    task->pos = 0;
    task->idest = 0;
    task->pos = 0;
    task->idest = size_t(-2);
    if (n > 4) {
        task->pdests = new size_t[n];
    } else {
        task->pdests = task->dests;
    }
    for (size_t j = 0; j < n; j++) {
        task->pdests[j] = dests[j];
    }
    task->ndest = n;

    logger.Log(Logger::kLevelEvent, log_level, "[DiskManager::Tick] post copy res %s from %u to %s", 
        task->rid.c_str(), task->idisk, 
        Format::format(Util::make_array(task->pdests, task->ndest)).c_str());

    copy_tasks_->push_back(task);
    if (copy_tasks_->size() == 1) {
        HandleCopyTask(ERROR_SUCCEED, NULL);
    }

    r(ERROR_PROCESSING, "");
}

void DiskManager::HandleCopyTask(
                                 ErrorCode err, 
                                 CopyTask * task)
{
    // 主线程执行

    if (copy_tasks_->empty()) {
        return;
    }
    if (task != NULL) {
        if (err == ERROR_SUCCEED) {
            logger.Log(Logger::kLevelEvent, log_level, "[DiskManager::HandleReadWriteBlock] finish copy res %s from %u to %s", 
                task->rid.c_str(), task->idisk, 
                Format::format(Util::make_array(task->pdests, task->ndest)).c_str());
            ResourceMap::iterator first = res_map_.find(task->rid);
            if (first == res_map_.end()) { // 被删除了？
                err = ERROR_COPY_NOT_EXIST;
            } else {
                for (size_t i = 0; i < task->ndest; i++) {
                    size_t size = 0;
                    if (disks_[task->pdests[i]]->AddResourceReal(task->rid, size) == ERROR_SUCCEED && size == task->size) {
                        Resource * res = new Resource;
                        res->rid = task->rid;
                        res->idisk = task->pdests[i];
                        res->size = task->size;
                        res->status = first->status;
                        // 据说能排在相同Rid的最前
                        res_map_.insert(res, first);
                        disks_[res->idisk]->stat()->num_res++;
                        if (res->status == Resource::paused)
                            disks_[res->idisk]->stat()->num_res_paused++;
                    }
                }
            }
        }
        if (err != ERROR_SUCCEED) {
            for (size_t i = 0; i < task->ndest; i++) {
                disks_[task->pdests[i]]->DeleteResourceReal(task->rid, DiskProxy::do_delete);
            }
        }
        copy_tasks_->erase(task);
        if (task->ndest > 4)
            delete [] task->pdests;
        delete task;
        if (copy_tasks_->empty()) {
            return;
        }
    }
    task = copy_tasks_->first();
    logger.Log(Logger::kLevelEvent, log_level, "[DiskManager::HandleCopyTask] start copy res %s from %u to %s", 
        task->rid.c_str(), task->idisk, 
        Format::format(Util::make_array(task->pdests, task->ndest)).c_str());
    for (size_t i = 0; i < task->ndest; i++) {
        disks_[task->pdests[i]]->AddResource(task->rid, task->size);
    }
    HandleReadWriteBlock(task, ERROR_SUCCEED, WeakPtr());
}

void DiskManager::HandleReadWriteBlock(
                                       CopyTask * task, 
                                       ErrorCode err, 
                                       WeakPtr const & data)
{
    if (err != ERROR_SUCCEED) {
        if (task->idest == size_t(-1))
            logger.Log(Logger::kLevelAlarm, log_level, "[DiskManager::HandleReadWriteBlock] error read block %u of res %s from disk %u", 
                task->pos / BLOCK_SIZE, task->rid.c_str(), task->idisk);
        else
            logger.Log(Logger::kLevelAlarm, log_level, "[DiskManager::HandleReadWriteBlock] error write block %u of res %s to disk %u", 
                task->pos / BLOCK_SIZE, task->rid.c_str(), task->pdests[task->idest]);
        io_service_->post(boost::bind(DiskManager::HandleCopyTask, err, task));
        return;
    }
    if (task->idest == size_t(-1)) {
        task->data = data;
    }
    task->idest++;
    if (task->idest == task->ndest) {
        MemoryPool::free(task->data);
        task->data.release();
        task->idest = size_t(-1);
        task->pos += BLOCK_SIZE;
        if (task->pos >= task->size) {
            io_service_->post(boost::bind(DiskManager::HandleCopyTask, ERROR_SUCCEED, task));
            return;
        }
    }
    if (task->idest == size_t(-1)) {
        logger.Log(Logger::kLevelDebug1, log_level, "[DiskManager::HandleReadWriteBlock] read block %u", 
            task->pos / BLOCK_SIZE);
        disks_[task->idisk]->ReadDataNoCheck(
            task->rid, task->pos, BLOCK_SIZE, 
            boost::bind(DiskManager::HandleReadWriteBlock, task, _1, _2));
    } else {
        logger.Log(Logger::kLevelDebug1, log_level, "[DiskManager::HandleReadWriteBlock] write to dest %u", 
            task->idest);
        disks_[task->pdests[task->idest]]->WriteData(
            task->rid, task->pos, BLOCK_SIZE, task->data, 
            boost::bind(DiskManager::HandleReadWriteBlock, task, _1, _2));
    }
}

namespace DiskManagerConsole
{
    void CopyResource(
        UserIf::get_arg_type const & g, 
        Action::response_type const & r)
    {
        std::string rid, dest;
        if (g(rid)) {
            std::vector<size_t> dests;
            while (g(dest = UserIf::ignore)) {
                dests.push_back(Parse::parse<size_t>(dest));
            }
            DiskManager::CopyResource(rid, dests, r);
        }
    }

    void ShowResource(
        UserIf::get_arg_type const & g, 
        Action::response_type const & r)
    {
        std::string rid;
        if (g(rid)) {
            std::vector<size_t> disks;
            std::pair<DiskManager::ResourceMap::iterator, DiskManager::ResourceMap::iterator> pair = 
                DiskManager::GetResource().equal_range(rid);
            for (; pair.first != pair.second; ) {
                disks.push_back(pair.first->idisk);
            }
            r(ERROR_SUCCEED, Format::format(disks));
        }
    }

    void RegisterCommands()
    {
        UserIf::SubCmd cmd;
        cmd.regCmd("copy", CopyResource);
        cmd.regCmd("show", ShowResource);
        UserIf::regCmd("disk", cmd);
    }

}
