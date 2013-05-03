// DacInfoWorker.h

#ifndef _PPBOX_DAC_DAC_INFO_WORKER_H_
#define _PPBOX_DAC_DAC_INFO_WORKER_H_

#include "ppbox/dac/DacInfo.h"

#include <framework/timer/TimeCounter.h>
#include <framework/string/Slice.h>

namespace ppbox
{
    namespace dac
    {

        struct DacRestartInfo       // 内核进程重启时需要提交的数据
            : DacCoreCommInfo
        {
            enum CoreTypeEnum
            {
                free, 
                vod, 
                live, 
            } type;

            // 内核进程从上次启动到现在运行的时间：(单位毫秒)
            boost::uint32_t run_time;

            DacRestartInfo(
                CoreTypeEnum type)
                : DacCoreCommInfo(InterfaceType::run_info_type)
                , type(type)
                , run_time(counter(type).elapse())
            {
                counter(type).reset();
            }

            virtual void do_serialize(
                DacArchive & ar)
            {
                DacCoreCommInfo::do_serialize(ar);
                ar & SERIALIZATION_NVP_NAME("K", run_time);
                ar & SERIALIZATION_NVP_NAME("L", type);
            }

            static framework::timer::TimeCounter & counter(
                CoreTypeEnum type)
            {
                static std::map<CoreTypeEnum, framework::timer::TimeCounter> counters;
                return counters[type];
            }
        };

        struct PeerInfo
        {
            PeerInfo()
                : peer_count_(0)
                , down_count_(0)
            {
            }

            void reset()
            {
                peer_count_ = 0;
                down_count_ = 0;
            }

            float calc(
                std::string const & msg)
            {
                // g_peerDac_  计算带宽节约比
                boost::uint32_t peer_count = 0;
                boost::uint32_t down_count = 0;

                boost::uint32_t temp = 0;
                framework::string::map_find(msg, "_J", temp, "&");
                peer_count = temp;
                down_count = temp;
                temp = 0;
                framework::string::map_find(msg, "_K", temp, "&");
                down_count += temp;
                temp = 0;
                framework::string::map_find(msg, "_Q1", temp, "&");
                down_count += temp;
                down_count_ += down_count;
                peer_count_ += peer_count;
                if (down_count) {
                    float perent = (float)peer_count/(float)down_count;
                    perent = (float)perent*100;
                    return perent;
                } else {
                    return 0.0;
                }
            }

            float percent() const
            {
                if (down_count_ == 0)
                    return 0.0;
                else
                    return (float)peer_count_ / (float)down_count_;
            }

            boost::uint32_t peer_count_; 
            boost::uint32_t down_count_;
        };

        struct DacPeerStatInfo       // 内核进程重启时需要提交的数据
            : DacCoreCommInfo
        {
            std::string msg;

            DacPeerStatInfo(
                std::string const & msg)
                : DacCoreCommInfo(InterfaceType::peer_info_type)
                , msg(msg)
            {
                percent_ = peer_info().calc(msg);
            }

            float percent() const
            {
                return percent_;
            }

            virtual void do_serialize(
                DacArchive & ar)
            {
                DacCoreCommInfo::do_serialize(ar);
                ar & msg;
            }

            static PeerInfo & peer_info()
            {
                static PeerInfo g_peerDac_;
                return g_peerDac_;
            }

        private:
            float percent_;
        };

    } // namespace dac
} // namespace ppbox

#endif // _PPBOX_DAC_DAC_INFO_AUTH_H_
