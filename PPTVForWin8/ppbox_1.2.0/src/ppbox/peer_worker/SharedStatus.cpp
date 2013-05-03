// SharedStatus.cpp

#include "ppbox/peer_worker/Common.h"
#include "ppbox/peer_worker/SharedStatus.h"
#include "ppbox/peer_worker/DataStruct.h"
#include "ppbox/peer_worker/Name.h"

#include <framework/process/Process.h>
#include <framework/process/Error.h>
#include <framework/string/Format.h>
#include <framework/string/Base16.h>
#include <framework/system/BytesOrder.h>
#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
using namespace framework::process;
using namespace framework::string;
using namespace framework::system;

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
using namespace boost::interprocess;
using namespace boost::system;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.peer_worker.SharedStatus", framework::logger::Info)

namespace ppbox
{
    namespace peer_worker
    {

        struct SharedMemory
        {
            SharedMemory()
                : object(NULL)
                , region(NULL)
            {
            }

            ~SharedMemory()
            {
                close();
            }

            bool open(
                std::string const & name)
            {
                close();
                try {
                    object = new shared_memory_object(open_only, name.c_str(), read_only);
                    region = new mapped_region(*object, read_only);
                } catch (...) {}
                return (region != NULL);
            }

            void * address() const
            {
                return (region != NULL) ? region->get_address() : NULL;
            }

            size_t size() const
            {
                return (region != NULL) ? region->get_size() : 0;
            }

            void detach()
            {
                region = NULL;
                object = NULL;
            }

            void close()
            {
                if (region) {
                    delete region;
                    region = NULL;
                }
                if (object) {
                    delete object;
                    object = NULL;
                }
            }

            shared_memory_object * object;
            mapped_region * region;
        };

        struct SharedStatus::Cache
        {
            Cache()
                : pid(0)
            {
            }

            int pid;
            SharedMemory root_shm;
            std::map<std::string, SharedMemory> shms;
        };

        SharedStatus::SharedStatus()
            : cache_(new Cache)
        {
        }

        SharedStatus::~SharedStatus()
        {
            delete cache_;
        }

        error_code SharedStatus::get_memory_blocks(
            std::map<std::string, Block> & blocks)
        {
            if (cache_->pid) {
                ProcessStat stat;
                if (get_process_stat(cache_->pid, stat) 
                    || stat.state == ProcessStat::zombie) {
                        LOG_DEBUG("[get_memory_blocks] old pid(" << cache_->pid << ") dead");
                        cache_->pid = 0;
                }
            }

            if (cache_->pid == 0) {
                std::vector<ProcessInfo> threads;
                error_code ec = enum_process(name_string(), threads);
                if (ec || threads.empty()) {
                    LOG_DEBUG("[get_memory_blocks] enum_process not found");
                    ProcessInfo temp;
                    temp.pid = framework::this_process::id();
                    threads.push_back(temp);
                    ec.clear();    
                    //return ec;
                }
                if (threads.empty()) {
                    return error::not_open;
                }

                for (size_t i = 0; i < threads.size(); ++i) {
                    ProcessInfo const & th = threads[i];
                    std::string root_name = "PPVIDEO_" + format(th.pid);
                    try {
                        cache_->root_shm.open(root_name);
                    } catch (...) {}
                    if (cache_->root_shm.address() == NULL)
                        continue;
                    cache_->pid = th.pid;
                    LOG_DEBUG("[get_memory_blocks] find pid " << cache_->pid);
                    break;
                }
            }
            if (cache_->pid == 0) {
                LOG_DEBUG("[get_memory_blocks] pid not find");
                return error::not_open;
            }

            Block block = {cache_->root_shm.address(), sizeof(STASTISTIC_INFO)};
            blocks.insert(std::make_pair("PPVIDEO_" + format(cache_->pid), block));
            LOG_TRACE("memory PPVIDEO_" << cache_->pid);

            STASTISTIC_INFO const * si = (STASTISTIC_INFO const *)cache_->root_shm.address();

            for(int i = 0; i < si->DownloadDriverCount; i++) {
                if (si->DownloadDriverIDs[i] == 0) {
                    continue;
                }
                std::string driver_name = "DOWNLOADDRIVER_" + format(cache_->pid) + "_" + 
                    format(BytesOrder::host_to_little_endian(si->DownloadDriverIDs[i]));
                std::map<std::string, SharedMemory>::const_iterator iter = 
                    cache_->shms.find(driver_name);
                if (iter == cache_->shms.end()) {
                    std::pair<std::string const, SharedMemory> pair(driver_name, SharedMemory());
                    pair.second.open(driver_name);
                    if (pair.second.address() != NULL) {
                        iter = cache_->shms.insert(pair).first;
                        pair.second.detach();
                    }
                }
                if (iter != cache_->shms.end()) {
                    DOWNLOADDRIVER_STATISTIC_INFO * dsi = (DOWNLOADDRIVER_STATISTIC_INFO *)iter->second.address();
                    Block block = {dsi, 
                        sizeof(DOWNLOADDRIVER_STATISTIC_INFO) 
                        + sizeof(HTTP_DOWNLOADER_INFO) * dsi->HttpDownloaderCount};
                    blocks.insert(std::make_pair(driver_name, block));
                    LOG_TRACE("memory " << driver_name);
                }
            }

            for(int i = 0; i < si->LiveDownloadDriverCount; i++) {
                if (si->LiveDownloadDriverIDs[i] == 0) {
                    continue;
                }
                std::string driver_name = "LIVEDOWNLOADDRIVER_" + format(cache_->pid) + "_" + 
                    format(BytesOrder::host_to_little_endian(si->LiveDownloadDriverIDs[i]));
                std::map<std::string, SharedMemory>::const_iterator iter = 
                    cache_->shms.find(driver_name);
                if (iter == cache_->shms.end()) {
                    std::pair<std::string const, SharedMemory> pair(driver_name, SharedMemory());
                    pair.second.open(driver_name);
                    if (pair.second.address() != NULL) {
                        iter = cache_->shms.insert(pair).first;
                        pair.second.detach();
                    }
                }
                if (iter != cache_->shms.end()) {
                    LIVE_DOWNLOADDRIVER_STATISTIC_INFO * dsi = (LIVE_DOWNLOADDRIVER_STATISTIC_INFO *)iter->second.address();
                    Block block = {dsi, 
                        sizeof(LIVE_DOWNLOADDRIVER_STATISTIC_INFO) 
                        };
                    blocks.insert(std::make_pair(driver_name, block));
                    LOG_TRACE("memory " << driver_name);
                }
            }

            for(int i = 0; i < UINT8_MAX_VALUE; i++) {
                if (si->P2PDownloaderRIDs[i].Data1 == 0) {
                    continue;
                }
                RID rid = si->P2PDownloaderRIDs[i];
                rid.Data1 = BytesOrder::detail::rotate((boost::uint32_t)rid.Data1);
                rid.Data2 = BytesOrder::detail::rotate((boost::uint16_t)rid.Data2);
                rid.Data3 = BytesOrder::detail::rotate((boost::uint16_t)rid.Data3);
                std::string downloader_name = "P2PDOWNLOADER_" + format(cache_->pid) + "_" + 
                    Base16::encode(std::string((char const *)&rid, sizeof(rid)));
                std::map<std::string, SharedMemory>::const_iterator iter = 
                    cache_->shms.find(downloader_name);
                if (iter == cache_->shms.end()) {
                    std::pair<std::string const, SharedMemory> pair(downloader_name, SharedMemory());
                    pair.second.open(downloader_name);
                    if (pair.second.address() != NULL) {
                        iter = cache_->shms.insert(pair).first;
                        pair.second.detach();
                    }
                }
                if (iter != cache_->shms.end()) {
                    P2PDOWNLOADER_STATISTIC_INFO * psi = (P2PDOWNLOADER_STATISTIC_INFO *)iter->second.address();
                    Block block = {psi, 
                        sizeof(P2PDOWNLOADER_STATISTIC_INFO)
                        + sizeof(P2P_CONNECTION_INFO) * BytesOrder::little_endian_to_host_short(psi->PeerCount)};
                    blocks.insert(std::make_pair(downloader_name, block));
                    LOG_TRACE("memory " << downloader_name);
                }
            }

            {
                //UpLoader
                std::string upload_name = "UPLOAD_" + format(cache_->pid);
                std::map<std::string, SharedMemory>::const_iterator iter = 
                    cache_->shms.find(upload_name);
                if (iter == cache_->shms.end()) 
                {
                    std::pair<std::string const, SharedMemory> pair(upload_name, SharedMemory());
                    pair.second.open(upload_name);
                    if (pair.second.address() != NULL) 
                    {
                        iter = cache_->shms.insert(pair).first;
                        pair.second.detach();
                    }
                }

                if (iter != cache_->shms.end()) {
                    UPLOAD_INFO * psi = (UPLOAD_INFO *)iter->second.address();
                    Block block = {psi, 
                        sizeof(UPLOAD_INFO)
                        };
                    blocks.insert(std::make_pair(upload_name, block));
                    LOG_TRACE("memory " << upload_name);
                }
            }

            return error_code();
        }

    } // namespace peer_worker
} // namespace ppbox

