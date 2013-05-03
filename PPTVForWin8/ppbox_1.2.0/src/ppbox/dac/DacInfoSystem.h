// DacInfoSystem.h

#ifndef _PPBOX_DAC_DAC_INFO_SYSTEM_H_
#define _PPBOX_DAC_DAC_INFO_SYSTEM_H_

#include "ppbox/dac/DacType.h"

#include <framework/process/Process.h>

#include <boost/shared_ptr.hpp>

namespace ppbox
{
    namespace dac
    {

        struct DacSysInfo       // 第一次播放开始的第5分钟时需要提交的数据
            : DacCoreCommInfo
        {
            // 系统CPU占用百分比（单位 %）
            // 提交格式:[CPU占有百分比]|[CPU占有百分比]|…
            boost::uint32_t cpu_rate;

            // 提交CPU的型号,例如：arm  或者 mips
            std::string cpu_type;

            // 进程CPU占用百分比（单位 %） 和 内存占用（单位 MB）
            // 提交格式：[进程名1]:[CPU占用]:[内存占用]|[进程名2]:[CPU占用百分比]:[内存占用]|…
            std::string process_info;

            DacSysInfo(
                std::string const & live_name, 
                std::string const & vod_name)
                : DacCoreCommInfo(InterfaceType::run_info_type)
                , live_worker_proc_(new framework::process::Process)
                , peer_worker_proc_(new framework::process::Process)
            {
                framework::process::get_system_stat(system_stat_);

                boost::system::error_code ec;
                if (!live_worker_proc_->open(vod_name, ec)) {
                    live_worker_proc_->stat(live_worker_stat_);
                }
                if (!peer_worker_proc_->open(vod_name, ec)) {
                    peer_worker_proc_->stat(peer_worker_stat_);
                }
            }

            void calc()
            {
                std::string process_info;

                framework::process::SystemStat system_stat;
                framework::process::get_system_stat(system_stat);

				if (system_stat.cpu.total == system_stat_.cpu. total) {
					cpu_rate = 0;
				} else {
					cpu_rate = ((system_stat.cpu.user + system_stat.cpu.system) - (system_stat_.cpu.user + system_stat_.cpu.system));
					cpu_rate = cpu_rate * 100 / (system_stat.cpu.total - system_stat_.cpu. total);
				}

                if (live_worker_proc_->is_open()) {
                    framework::process::ProcessStat live_worker_stat;
                    live_worker_proc_->stat(live_worker_stat);
                    framework::process::ProcessStatM live_worker_statm;
                    live_worker_proc_->statm(live_worker_statm);
                    process_info += "live_worker:";
                    calc_process_info(process_info, live_worker_stat_, live_worker_stat, system_stat_, system_stat, live_worker_statm);
                }
                if (peer_worker_proc_->is_open()) {
                    framework::process::ProcessStat peer_worker_stat;
                    peer_worker_proc_->stat(peer_worker_stat);
                    process_info += "peer_worker:";
                    framework::process::ProcessStatM peer_worker_statm;
                    live_worker_proc_->statm(peer_worker_statm);
                    calc_process_info(process_info, live_worker_stat_, peer_worker_stat, system_stat_, system_stat, peer_worker_statm);
                }

                // cpu type
            }

            void calc_process_info(
                std::string & info,
                framework::process::ProcessStat const & stat1, 
                framework::process::ProcessStat const & stat2, 
                framework::process::SystemStat const & system_stat1, 
                framework::process::SystemStat const & system_stat2, 
                framework::process::ProcessStatM const & statm)
            {
                boost::uint32_t cpu_percent = (system_stat2.cpu.total == system_stat1.cpu.total) ? 0
                    : ((stat2.stime + stat2.utime) - (stat1.stime + stat1.utime)) * 100 / (system_stat2.cpu.total - system_stat1.cpu.total);
                info += framework::string::format(cpu_percent);
                info += ":";
                info += framework::string::format(statm.resident * 4 / 1024);
                info += "|";
            }

            virtual void do_serialize(DacArchive & ar)
            {
                DacCoreCommInfo::do_serialize(ar);
                ar & SERIALIZATION_NVP_NAME("K", cpu_rate);
                ar & SERIALIZATION_NVP_NAME("L", cpu_type);
                ar & SERIALIZATION_NVP_NAME("M", process_info);
            }

        private:
            boost::shared_ptr<framework::process::Process> live_worker_proc_;
            boost::shared_ptr<framework::process::Process> peer_worker_proc_;
            framework::process::SystemStat system_stat_;
            framework::process::ProcessStat live_worker_stat_;
            framework::process::ProcessStat peer_worker_stat_;
        };

    } // namespace dac
} // namespace ppbox

#endif // _PPBOX_DAC_DAC_INFO_SYSTEM_H_
