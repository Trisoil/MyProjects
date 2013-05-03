// DiskProxy.cpp

#include "Common.h"
#include "DiskProxy.h"
#include "DiskManager.h"
#include "Indice.h"
#include "DiskTask.h"
#include "DiskStatistics.h"
#include "Disk.h"

#include <Indice.h>

#include <Archive/BinaryIArchive.h>
#include <Archive/BinaryOArchive.h>
#include <Serialize/Stl/map.h>
#include <Serialize/Array.h>
using namespace Util;

#include <boost/filesystem/operations.hpp> 
#include <boost/filesystem/exception.hpp> 
#include <boost/timer.hpp>

#include <fstream>

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#endif

#ifndef BOOST_ERROR_CODE_HPP

#include "Disk.h"

namespace boost
{
    namespace filesystem
    {
        typedef filesystem_error error;

#define file_string native_file_string

        bool is_regular_file(const path & ph)
        {
            return !is_directory(ph);
        }

        struct space_info
        {
            // all values are byte counts
            boost::uintmax_t capacity;
            boost::uintmax_t free;      // <= capacity
            boost::uintmax_t available; // <= free
        };

        space_info space( const path & ph )
        {
            Util::UInt64 total_space;
            Util::UInt64 free_space;
            Util::UInt64 available_space;
            ErrorCode ret = Disk::get_space(ph.file_string(), total_space, free_space, available_space);
            if (ret != ERROR_SUCCEED) {
                boost::throw_exception(filesystem_error(
					"boost::filesystem::space", ph, detail::system_error_code()));
            }
            space_info info;
            info.capacity = total_space;
            info.available = available_space;
            info.free = free_space;
            return info;
        }

    }
}

static inline std::string sys_err_msg(
                                      boost::filesystem::filesystem_error const & e)
{
    return e.what();
}

#else

namespace boost
{
    namespace filesystem
    {
        typedef boost::system::system_error error;
    }
}

#endif

DiskStatisticsHead * DiskProxy::stat_head_ = NULL;

DiskProxy::DiskProxy(
                     std::string const & path, 
                     ErrorCode & error)
                     : stopped_(false)
                     , tasks_(new List<DiskTask>)
                     , thread_(NULL)
{
    try {
        path_ = boost::filesystem::path(path, boost::filesystem::native);
        path_ = boost::filesystem::system_complete(path_);
        if (!stat_head_)
            stat_head_ = new(SHID_DISK) DiskStatisticsHead;
        stat_ = new DiskStatistics;
        stat_->path = SHARED_NEW char[path_.file_string().size() + 1];
        strcpy(stat_->path, path_.file_string().c_str());
        boost::filesystem::space_info space = boost::filesystem::space(path_);
        stat_->total_space = space.capacity / 1024 / 1024; // MB;
        stat_head_->insert(stat_);
        error = ERROR_SUCCEED;
    } catch (boost::filesystem::error const & e) {
        logger.Log(Logger::kLevelError, DiskManager::log_level, "[DiskProxy] %s", e.what());
        error = ERROR_FILE_SYSTEM;
    } catch (...) {
        logger.Log(Logger::kLevelError, DiskManager::log_level, "[DiskProxy] Unknown error!");
        error = ERROR_ERROR;
    }
}

DiskProxy::~DiskProxy()
{
    DiskTask * p;
    while ((p = tasks_->first())) {
        tasks_->pop_front();
        delete p;
    }
    delete tasks_;
    stat_head_->erase(stat_);
    delete stat_;
}

void DiskProxy::Start()
{
    thread_ = new boost::thread(boost::bind(&DiskProxy::WorkThread, this));
}

void DiskProxy::Stop()
{
    stopped_ = true;
    {
        boost::mutex::scoped_lock lock(mutex_);
        tasks_->push_front(new DiskTask());
        cond_.notify_one();
    }
    thread_->join();
    delete thread_;
}

ErrorCode DiskProxy::ReadData(
                        ResId const & rid, 
                        size_t offset, 
                        size_t size, 
                        size_t priority, 
                        disk_task_response_type const & r)
{
    boost::mutex::scoped_lock lock(mutex_);
    stat_->recv++;
    stat_head_->recv++;
    DiskTask * task = NULL;
    if (tasks_->size() >= DiskManager::max_queue_size) {
        task = tasks_->last();
        if (task->priority >= priority) {
            stat_->drop++;
            stat_head_->drop++;
            return (ERROR_IO_BUSY);
        } else {
            assert(task->type == DiskTask::read_data);
            // 剥夺该任务的执行权限
            tasks_->erase(task);
            task->r(ERROR_IO_BUSY, task->data);
            stat_->drop++;
            stat_head_->drop++;
            *task = DiskTask(rid, offset, size, priority, r);
        }
    } else {
        task = new DiskTask(rid, offset, size, priority, r);
        if (!task) {
            stat_->err++;
            stat_head_->err++;
            logger.Log(Logger::kLevelAlarm, DiskManager::log_level, "[DiskProxy::ReadData] no memory");
            return (ERROR_NO_MOMERY);
        }
    }
    // 从后向前查找，应该更优，因为队列中的priority都比较大
    Util::List<DiskTask>::reverse_iterator task1 = tasks_->rbegin();
    while (task1 != tasks_->rend() && task1->priority < task->priority)
        ++task1;
    tasks_->insert(task, task1);
    stat_->queue++;
    cond_.notify_one();
    return (ERROR_PROCESSING);
}

ErrorCode DiskProxy::ReadDataNoCheck(
                               ResId const & rid, 
                               size_t offset, 
                               size_t size, 
                               disk_task_response_type const & r)
{
    boost::mutex::scoped_lock lock(mutex_);
    stat_->recv++;
    stat_head_->recv++;
    DiskTask * task = new DiskTask(rid, offset, size, r);
    if (!task) {
        stat_->err++;
        stat_head_->err++;
        logger.Log(Logger::kLevelAlarm, DiskManager::log_level, "[DiskProxy::ReadDataNoCheck] no memory");
        return (ERROR_NO_MOMERY);
    } else {
        tasks_->push_front(task);
        stat_->queue++;
        cond_.notify_one();
    }
    return (ERROR_PROCESSING);
}

ErrorCode DiskProxy::WriteData(
                         ResId const & rid, 
                         size_t offset, 
                         size_t size, 
                         SharePtr const & data, 
                         disk_task_response_type const & r)
{
    boost::mutex::scoped_lock lock(mutex_);
    stat_->recv++;
    stat_head_->recv++;
    DiskTask * task = new DiskTask(rid, offset, size, data, r);
    if (!task) {
        stat_->err++;
        stat_head_->err++;
        logger.Log(Logger::kLevelAlarm, DiskManager::log_level, "[DiskProxy::WriteData] no memory");
        return (ERROR_NO_MOMERY);
    } else {
        tasks_->push_front(task);
        stat_->queue++;
        cond_.notify_one();
    }
    return (ERROR_PROCESSING);
}
/*
static int ListDirectory(
                         const std::string & strPath, 
                         std::map<std::string, bool> & list)
{
    if (strPath.size() < 2) {
        return 0; 
    } 

    namespace fs = boost::filesystem; 
    try {
        fs::path full_path(strPath, boost::filesystem::native); 

        if (!fs::exists(full_path) || !fs::is_directory(full_path)) { 
            return -1; 
        } 

        fs::directory_iterator end_iter; 
        for (fs::directory_iterator dir_itr(full_path); dir_itr != end_iter; ++dir_itr)
            list.insert(std::make_pair(((fs::path)(*dir_itr)).leaf(), fs::is_directory(*dir_itr)));

        return 0; 
    } catch (...) {
        return -2;
    }
} 
*/
ErrorCode DiskProxy::CheckResource(
                             ResId const & rid, 
                             size_t & size)
{
    namespace fs = boost::filesystem; 
    try {
        fs::path full_path = path_ / (std::string)rid; 

        if (!fs::exists(full_path) || !fs::is_directory(full_path)) { 
            return ERROR_FILE_NOT_EXIST;
        } 

        size_t num_block = 0;
        size_t max_block = 0;
        fs::directory_iterator end_iter; 
        for (fs::directory_iterator dir_itr(full_path); dir_itr != end_iter; ++dir_itr) {
            if (fs::is_regular_file(*dir_itr)) {
                std::string name = ((fs::path)(*dir_itr)).leaf();
                if (name.size() == 3 &&
                    name[0] >= '0' && name[0] <= '9' && 
                    name[1] >= '0' && name[1] <= '9' &&
                    name[2] >= '0' && name[2] <= '9') {
                        size_t size = file_size(*dir_itr);
                        if (size != BLOCK_SIZE) {
                            logger.Log(Logger::kLevelAlarm, DiskManager::log_level, "[DiskProxy::CheckResource] block size error (disk = %s, rid = %s, block = %s, size = %u)", 
                                path_.file_string().c_str(), rid.c_str(), name.c_str(), size);
                            return ERROR_COPY_DATA_ERROR;
                        }
                        num_block++;
                        size_t idx_block = (name[0]  - '0')* 100 + (name[1] - '0') * 10 + (name[2] - '0');
                        if (idx_block > max_block)
                            max_block = idx_block;
                }
            }
        }
        if (num_block != max_block + 1) {
            logger.Log(Logger::kLevelAlarm, DiskManager::log_level, "[DiskProxy::CheckResource] block not continues (disk = %s, rid = %s, num = %u, max = %u)", 
                path_.file_string().c_str(), rid.c_str(), num_block, max_block);
            return ERROR_COPY_DATA_ERROR;
        }
        char buf[PIECE_SIZE];
        ErrorCode ret = ReadFile(rid, 0, PIECE_SIZE, buf);
        if (ret == ERROR_SUCCEED) {
            size_t num_piece = Indice((_Indice *)buf).GetPieceCout();
            if (num_block != (num_piece + PIECE_PER_BLOCK - 1) / PIECE_PER_BLOCK) {
                logger.Log(Logger::kLevelAlarm, DiskManager::log_level, "[DiskProxy::CheckResource] resource check failed (disk = %s, rid = %s, num1 = %u, num2 = %u)", 
                path_.file_string().c_str(), rid.c_str(), num_block, (num_piece + PIECE_PER_BLOCK - 1) / PIECE_PER_BLOCK);
                ret = ERROR_COPY_DATA_ERROR;
            } else {
                size = num_block * BLOCK_SIZE;
            }
        }
        return ret;
    } catch (boost::filesystem::error const & e) {
        logger.Log(Logger::kLevelError, DiskManager::log_level, "[DiskProxy::CheckResource] filesystem error (disk = %s, rid = %s, err = %s)", 
            path_.file_string().c_str(), rid.c_str(), e.what());
        return ERROR_FILE_SYSTEM;
    } catch (...) {
        logger.Log(Logger::kLevelError, DiskManager::log_level, "[DiskProxy::CheckResource] Unknown error! (disk = %s, rid = %s)", 
            path_.file_string().c_str(), rid.c_str());
        return ERROR_ERROR;
    }
}

ErrorCode DiskProxy::AddResource(
                           ResId const & rid, 
                           size_t size)
{
    namespace fs = boost::filesystem; 

    try {
        fs::path full_path = path_ / (std::string)rid;
        full_path /= rid.c_str();
        return fs::create_directory(full_path) ? ERROR_SUCCEED : ERROR_FILE_SYSTEM;
    } catch (boost::filesystem::error const & e) {
        logger.Log(Logger::kLevelError, DiskManager::log_level, "[DiskProxy::AddResource] filesystem error (disk = %s, rid = %s, err = %s)", 
            path_.file_string().c_str(), rid.c_str(), e.what());
        return ERROR_FILE_SYSTEM;
    } catch (...) {
        logger.Log(Logger::kLevelError, DiskManager::log_level, "[DiskProxy::AddResource] Unknown error! (disk = %s, rid = %s)", 
            path_.file_string().c_str(), rid.c_str());
        return ERROR_ERROR;
    }
}

ErrorCode DiskProxy::AddResourceReal(
                                     ResId const & rid, 
                                     size_t & size)
{
    ErrorCode ret = CheckResource(rid, size);
    if (ret == ERROR_SUCCEED) {
        used_bytes_ += size + 1024 * 1024;
        stat_->used_space = used_bytes_ / 1024 / 1024; // MB
    } else if (ret == ERROR_COPY_DATA_ERROR) {
        DeleteResourceReal(rid, move_to_invalid);
    }
    return ret;
}

void DiskProxy::DeleteResource(
                               ResId const & rid, 
                               Util::Action::response_type const & r)
{
    logger.Log(Logger::kLevelDebug, DiskManager::log_level, "[DiskProxy::DeleteResource]");

    boost::mutex::scoped_lock lock(mutex_);

    size_t size = 0;
    ErrorCode ret = ERROR_SUCCEED; CheckResource(rid, size);
    if (ret == ERROR_SUCCEED) {
        DiskTask * task = new DiskTask(rid, boost::bind(r, _1, ""));
        if (!task) {
            logger.Log(Logger::kLevelAlarm, DiskManager::log_level, "[DiskProxy::DeleteResource] no memory");
            r(ERROR_NO_MOMERY, NULL);
        } else {
            task->size = size;
            tasks_->push_front(task);
            cond_.notify_one();
        }
    } else {
        r(ret, "");
    }
}

ErrorCode DiskProxy::DeleteResourceReal(
                                        ResId const & rid, 
                                        DeleteTypeEnum type)
{
    namespace fs = boost::filesystem; 

    if (type == no_delete)
        return ERROR_SUCCEED;

    try {
        if (type == do_delete) {
            fs::remove_all(path_ / rid.c_str());
        } else if (type == move_to_recycle) {
            if (!fs::exists(path_ / "deleted"))
                fs::create_directory(path_ / "deleted");
            fs::rename(path_ / rid.c_str(), path_ / "deleted" / rid.c_str());
        } else if (type == move_to_invalid) {
            if (!fs::exists(path_ / "invalid"))
                fs::create_directory(path_ / "invalid");
            fs::rename(path_ / rid.c_str(), path_ / "invalid" / rid.c_str());
        }
        return ERROR_SUCCEED;
    } catch (boost::filesystem::error const & e) {
        logger.Log(Logger::kLevelError, DiskManager::log_level, "[DiskProxy::DeleteResourceReal] filesystem error (disk = %s, rid = %s, err = %s)", 
            path_.file_string().c_str(), rid.c_str(), e.what());
        return ERROR_FILE_SYSTEM;
    } catch (...) {
        logger.Log(Logger::kLevelError, DiskManager::log_level, "[DiskProxy::DeleteResourceReal] Unknown error! (disk = %s, rid = %s)", 
            path_.file_string().c_str(), rid.c_str());
        return ERROR_ERROR;
    }
}

void DiskProxy::GetResource(
                            std::map<ResId, size_t> & ress, 
                            Util::Action::response_type const & r)
{
    // 借助线程读取资源列表，加快启动速度
    logger.Log(Logger::kLevelDebug, DiskManager::log_level, "[DiskProxy::GetResource]");

    boost::mutex::scoped_lock lock(mutex_);

    DiskTask * task = new DiskTask(ress, boost::bind(r, _1, ""));
    if (!task) {
        logger.Log(Logger::kLevelAlarm, DiskManager::log_level, "[DiskProxy::GetResource] no memory");
        r(ERROR_NO_MOMERY, NULL);
    } else {
        tasks_->push_front(task);
        cond_.notify_one();
    }
}

void DiskProxy::Statistics()
{
    size_t ability = 0;
    size_t free_space = 0;
    for (DiskStatistics::pointer stat = stat_head_->first(); stat; stat = stat_head_->next(stat)) {
        stat->recv.tick(10);
        stat->drop.tick(10);
        stat->err.tick();
        stat->send.tick();
        stat->trans.tick();
        ability += stat->ability;
        free_space += stat->total_space > stat->used_space ? stat->total_space - stat->used_space : 0;
    }
    stat_head_->recv.tick();
    stat_head_->drop.tick();
    stat_head_->err.tick();
    stat_head_->send.tick();
    stat_head_->trans.tick();
    stat_head_->ability = ability;
    stat_head_->free_space = free_space;
}

void DiskProxy::WorkThread()
{
    DiskTask * task = NULL;
    boost::timer timer;
    double sum_elapsed = 0;
    size_t num_suc_read = 0;

    while (!stopped_) {

        LogWraper lw(logger);

        {
            boost::mutex::scoped_lock lock(mutex_);

            while (tasks_->empty()) {
                cond_.wait(lock);
            }
            task = tasks_->first();
            tasks_->erase(task);

            if (task->type == DiskTask::delete_res) {
                logger.Log(Logger::kLevelDebug, DiskManager::log_level, "[DiskProxy::WorkThread] delete resource %s", task->rid.c_str());
                for (DiskTask * p = tasks_->first(); p;) {
                    if (p->rid == task->rid) {
                        p->r(ERROR_CANCELED, WeakPtr());
                        DiskTask * q = p;
                        p = tasks_->erase(p);
                        stat_->queue--;
                        delete q;
                    } else {
                        p = tasks_->next(p);
                    }
                }
                used_bytes_ -= task->size + 1024 * 1024;
                stat_->used_space = used_bytes_ / 1024 / 1024; // MB
                DeleteResourceReal(task->rid, DeleteTypeEnum(DiskManager::delete_type));
                task->r(ERROR_SUCCEED, WeakPtr());
                delete task;
                continue;
            } else if (task->type == DiskTask::get_res) {
                std::map<ResId, size_t> & ress = *(std::map<ResId, size_t> *)task->offset;
                GetResourceReal(ress);
                // 释放所有文件不存在的任务
                for (DiskTask * p = tasks_->first(); p;) {
                    if (ress.find(p->rid) == ress.end()) {
                        p->r(ERROR_MOVIE_NOT_EXIST, WeakPtr());
                        DiskTask * q = p;
                        p = tasks_->erase(p);
                        stat_->queue--;
                        delete q;
                    } else {
                        p = tasks_->next(p);
                    }
                }
                task->r(ERROR_SUCCEED, WeakPtr());
                delete task;
                continue;
            } else if (task->type == DiskTask::stop) {
                delete task;
                break;
            }

            stat_->queue--;
        }

        // run no lock

        if (!task->data) {
            task->data = MemoryPool::alloc();
            //logger.Log(Logger::kLevelDebug, DiskManager::log_level, "[DiskProxy::WorkThread] alloc block %p", 
            //    task->data);
        }
        if (!task->data) {
            logger.Log(Logger::kLevelAlarm, DiskManager::log_level, "[DiskProxy::WorkThread] alloc failed!");
            stat_->err++;
            task->r(ERROR_NO_MOMERY, task->data);
        } else {

            lw.stop();
            timer.restart();

            ErrorCode err;
            try {
                err = task->type == DiskTask::read_data ? 
                    ReadFile(task->rid, task->offset, task->size, (char *)task->data) : 
                    WriteFile(task->rid, task->offset, task->size, (char *)task->data);
            } catch (boost::filesystem::error const & e) {
                logger.Log(Logger::kLevelError, DiskManager::log_level, "[DiskProxy::WorkThread] filesystem error (err = %s)", 
                    e.what());
            } catch (...) {
                logger.Log(Logger::kLevelError, DiskManager::log_level, "[DiskProxy::WorkThread] Unknown error!");
            }

            if (err == ERROR_SUCCEED && task->type == DiskTask::read_data && 
                num_suc_read < 300000) { // 跟踪一段时间，以后不跟踪了
                    sum_elapsed += timer.elapsed();
                    if (++num_suc_read % 30 == 0) {
                        stat_->ability = (size_t)((double)num_suc_read / sum_elapsed);
                    }
            }

            lw.start();

            if (err == ERROR_SUCCEED) {
                //logger.Log(Logger::kLevelDebug, DiskManager::log_level, "[DiskProxy::WorkThread] read succeed!");
                stat_->trans += task->size;
                stat_head_->trans += task->size;
                stat_->send++;
                stat_head_->send++;
            } else {
                logger.Log(Logger::kLevelAlarm, DiskManager::log_level, 
                    "[DiskProxy::WorkThread] task failed (type = %s, disk = %s, rid = %s, block = %u, err = %s)!", 
                    task->type == DiskTask::read_data ? "read" : "write", 
                    path_.file_string().c_str(), task->rid.c_str(), task->offset / BLOCK_SIZE, 
                    ErrorMsg::system_error_message().c_str());
                stat_->err++;
                stat_head_->err++;
            }

            task->r(err, task->data);
        }

        delete task;
    }
}

void DiskProxy::GetResourceReal(
                                std::map<ResId, size_t> & ress)
{
    namespace fs = boost::filesystem; 
    try {
        std::map<ResId, size_t> list2;
        // 加载列表
        {
            std::ifstream ifs((path_ / "list").file_string().c_str(), std::ios::binary);
            Serialize::BinaryIArchive ia(ifs);
            ia >> list2;
        }
        used_bytes_ = 0;
        fs::directory_iterator end_iter;
        for (fs::directory_iterator dir_itr(path_); dir_itr != end_iter; ++dir_itr) {
            std::string name = ((fs::path)(*dir_itr)).leaf();
            if (fs::is_directory(*dir_itr) && name.size() == RES_ID_SIZE) {
                std::map<ResId, size_t>::const_iterator i = list2.find(name);
                if (i == list2.end()) {
                    size_t size;
                    if (CheckResource(name, size) != ERROR_SUCCEED) {
                        DeleteResourceReal(name, move_to_invalid);
                        continue;
                    }
                    used_bytes_ += size + 1024 * 1024;
                    ress.insert(std::make_pair(name, size));
                } else {
                    used_bytes_ += i->second;
                    ress.insert(*i);
                }
            }
        }
        used_bytes_ += ress.size() * 1024 * 1024;
        stat_->used_space = used_bytes_ / 1024 / 1024; // MB
        // 保存列表
        {
            std::ofstream ofs((path_ / "list").file_string().c_str(), std::ios::binary);
            Serialize::BinaryOArchive oa(ofs);
            oa << ress;
        }
    } catch (boost::filesystem::error const & e) {
        logger.Log(Logger::kLevelError, DiskManager::log_level, "[DiskProxy::GetResourceReal] filesystem error (err = %s)", 
            e.what());
    } catch (...) {
        logger.Log(Logger::kLevelError, DiskManager::log_level, "[DiskProxy::GetResourceReal] Unknown error!");
    }
}

static char const hex_chr[] = "0123456789ABCDEF";

ErrorCode DiskProxy::ReadFile(
                        ResId const & rid, 
                        size_t offset, 
                        size_t size, 
                        char * buf)
{
#ifdef USE_AWE
    if (buf == NULL) {
        logger.Log(Logger::kLevelAlarm, DiskManager::log_level, "[DiskProxy::ReadSubPiece] pool full (used = %u, used_virtual = %u)", 
            MemoryPool::num_used(), MemoryPool::num_used_virtual());
        return ERROR_NO_MOMERY;
    }
#endif

    boost::filesystem::path const path = path_ / (std::string)rid;
    size_t end = offset + size;
    size_t ibs = offset / BLOCK_SIZE;
    size_t ibe = end / BLOCK_SIZE;
    size_t offset_first = offset % BLOCK_SIZE;
    size_t size_first = BLOCK_SIZE - offset_first;
    size_t size_last = end % BLOCK_SIZE;
    if (size_last == 0) {
        ibe--;
        size_last = BLOCK_SIZE;
    }
    if (ibs == ibe)
        size_first = size_last - offset_first;
    char name[5] = {'/', 0};
    for (size_t ib = ibs; ib <= ibe; ib++) {
        size_t o = (ib == ibs) ? offset_first : 0;
        size_t s = (ib == ibs) ? size_first : ((ib == ibe) ? size_last : BLOCK_SIZE);
        name[1] = hex_chr[ib / 100];
        name[2] = hex_chr[(ib % 100) / 10];
        name[3] = hex_chr[ib % 10];
        //std::ifstream ifs((path + name).c_str(), std::ios::binary);
        //if (!ifs)
        //    return ERROR_FILE_NOT_EXIST;
        //ifs.seekg((std::streamsize)o, std::ios::beg);
        //ifs.read(buf, (std::streamsize)s);
        //if (ifs.gcount() != (std::streamsize)s)
        //    return ERROR_ERROR;
#ifdef _WIN32
        HANDLE hFile = CreateFile(
            (path / name).file_string().c_str(), 
            GENERIC_READ | GENERIC_WRITE, 
            0, 
            NULL, 
            OPEN_EXISTING, 
            FILE_ATTRIBUTE_NORMAL, 
            NULL);
        if (hFile == INVALID_HANDLE_VALUE)
            return ERROR_FILE_NOT_EXIST;

        HANDLE hFileMapping = CreateFileMapping(
            hFile, 
            NULL, 
            PAGE_READONLY, 
            0, 
            BLOCK_SIZE, 
            NULL);
        if (hFileMapping == NULL) {
            CloseHandle(
                hFile);

            return ERROR_ERROR;
        }

        LPVOID lpBaseAddress = MapViewOfFile(
            hFileMapping, 
            FILE_MAP_READ, 
            0, 
            0, 
            0);

        if (lpBaseAddress == NULL) {
            CloseHandle(
                hFileMapping);

            CloseHandle(
                hFile);

            return ERROR_NO_MOMERY;
        }

        memcpy(buf, (char *)lpBaseAddress + o, s);

        UnmapViewOfFile(
            lpBaseAddress);

        CloseHandle(
            hFileMapping);

        CloseHandle(
            hFile);
#else
        int fildes = open(
            (path / name).file_string().c_str(), 
            O_RDONLY);
        if (fildes == -1)
            return ERROR_FILE_NOT_EXIST;

        void * addr = mmap(
            0, 
            BLOCK_SIZE, 
            PROT_READ, 
            MAP_PRIVATE, 
            fildes, 
            0);
        if (addr == MAP_FAILED) {
            close(
                fildes);

            return ERROR_NO_MOMERY;
        }

        memcpy(buf, (char *)addr + o, s);

        munmap(
            addr, 
            BLOCK_SIZE);

        close(
            fildes);
#endif
        buf += s;
    }
    return ERROR_SUCCEED;
}

ErrorCode DiskProxy::WriteFile(
                         ResId const & rid, 
                         size_t offset, 
                         size_t size, 
                         char * buf)
{
#ifdef USE_AWE
    if (buf == NULL) {
        logger.Log(Logger::kLevelAlarm, DiskManager::log_level, "[DiskProxy::ReadSubPiece] pool full (used = %u, used_virtual = %u)", 
            MemoryPool::num_used(), MemoryPool::num_used_virtual());
        return ERROR_NO_MOMERY;
    }
#endif

    boost::filesystem::path const path = path_ / (std::string)rid;
    size_t end = offset + size;
    size_t ibs = offset / BLOCK_SIZE;
    size_t ibe = end / BLOCK_SIZE;
    size_t offset_first = offset % BLOCK_SIZE;
    size_t size_first = BLOCK_SIZE - offset_first;
    size_t size_last = end % BLOCK_SIZE;
    if (size_last == 0) {
        ibe--;
        size_last = BLOCK_SIZE;
    }
    if (ibs == ibe)
        size_first = size_last - offset_first;
    char name[5] = {'/', 0};
    for (size_t ib = ibs; ib <= ibe; ib++) {
        size_t o = (ib == ibs) ? offset_first : 0;
        size_t s = (ib == ibs) ? size_first : ((ib == ibe) ? size_last : BLOCK_SIZE);
        name[1] = hex_chr[ib / 100];
        name[2] = hex_chr[(ib % 100) / 10];
        name[3] = hex_chr[ib % 10];
        //std::ifstream ifs((path + name).c_str(), std::ios::binary);
        //if (!ifs)
        //    return ERROR_FILE_NOT_EXIST;
        //ifs.seekg((std::streamsize)o, std::ios::beg);
        //ifs.read(buf, (std::streamsize)s);
        //if (ifs.gcount() != (std::streamsize)s)
        //    return ERROR_ERROR;
#ifdef _WIN32
        HANDLE hFile = CreateFile(
            (path / name).file_string().c_str(), 
            GENERIC_READ | GENERIC_WRITE, 
            0, 
            NULL, 
            CREATE_ALWAYS, 
            FILE_ATTRIBUTE_NORMAL, 
            NULL);
        if (hFile == INVALID_HANDLE_VALUE)
            return ERROR_FILE_NOT_EXIST;

        HANDLE hFileMapping = CreateFileMapping(
            hFile, 
            NULL, 
            PAGE_READWRITE, 
            0, 
            BLOCK_SIZE, 
            NULL);
        if (hFileMapping == NULL) {
            CloseHandle(
                hFile);

            return ERROR_ERROR;
        }

        LPVOID lpBaseAddress = MapViewOfFile(
            hFileMapping, 
            FILE_MAP_WRITE, 
            0, 
            0, 
            0);

        if (lpBaseAddress == NULL) {
            CloseHandle(
                hFileMapping);

            CloseHandle(
                hFile);

            return ERROR_NO_MOMERY;
        }

        memcpy((char *)lpBaseAddress + o, buf, s);

        UnmapViewOfFile(
            lpBaseAddress);

        CloseHandle(
            hFileMapping);

        CloseHandle(
            hFile);
#else
        int fildes = open(
            (path / name).file_string().c_str(), 
            O_WRONLY | O_CREAT);
        if (fildes == -1)
            return ERROR_FILE_NOT_EXIST;

        void * addr = mmap(
            0, 
            BLOCK_SIZE, 
            PROT_WRITE, 
            MAP_PRIVATE, 
            fildes, 
            0);
        if (addr == MAP_FAILED) {
            close(
                fildes);

            return ERROR_NO_MOMERY;
        }

        memcpy((char *)addr + o, buf, s);

        munmap(
            addr, 
            BLOCK_SIZE);

        close(
            fildes);
#endif
        buf += s;
    }
    return ERROR_SUCCEED;
}

