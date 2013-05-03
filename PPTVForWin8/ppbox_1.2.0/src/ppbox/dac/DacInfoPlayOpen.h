// DacInfo.h

#ifndef _PPBOX_DAC_DAC_INFO_PLAY_OPEN_H_
#define _PPBOX_DAC_DAC_INFO_PLAY_OPEN_H_

#include "ppbox/dac/DacInfo.h"

#include <ppbox/cdn/PptvMedia.h>
#include <ppbox/cdn/HttpStatistics.h>
#include <ppbox/peer/PeerSource.h>

#include <ppbox/demux/base/DemuxStatistic.h>

namespace ppbox
{
    namespace dac
    {

        struct dac_server_info
        {
            // DNS解析时间：（单位毫秒）正数表示成功的解析时间, -1表示解析失败
            dac_time dns_time;

            // 连接时间：（单位毫秒）表示从开始连接到连上的时间
            // 正数表示成功的连接时间 -1表示连接失败
            dac_time con_time;

            // HTTP返回值：（整数表示HTTP的返回值，例如200,206,404,403,502等，另外-1表示格式错误）
            dac_errno error;

            // 响应时间：（单位毫秒）表示dt在连接成功都获得所有数据的时间
            // 正数表示成功的连接时间
            dac_time resp_time;

            // N:dt尝试的次数
            boost::uint32_t try_times;

            // E:最终错误码
            dac_errno last_error;

            // N:总时间（单位毫秒）
            dac_time total_time;

            dac_server_info(
                char const c[2])
            {
                C1_[0] = c[0];
                C1_[1] = 0;
                C1_[2] = 0;
                C1_[3] = 0;
                C2_[0] = c[1];
                C2_[1] = 0;
                C2_[2] = 0;
                C2_[3] = 0;
            }

            void set_stat(
                ppbox::cdn::HttpStatistics const & stat)
            {
                error = stat.last_error;
                dns_time = stat.resolve_time;
                if (stat.connect_time != (boost::uint32_t)-1)
                    con_time = stat.connect_time - stat.resolve_time;
                if (stat.response_data_time != (boost::uint32_t)-1)
                    resp_time = stat.response_data_time - stat.connect_time;
                try_times = stat.try_times;
                last_error = stat.last_last_error;
                total_time = stat.total_elapse;
            }

            template <typename Archive>
            void serialize(Archive & ar)
            {
                if (C2_[0] != 'P')
                    ar & SERIALIZATION_NVP_NAME(C1_, dns_time);
                ++C1_[0];
                ar & SERIALIZATION_NVP_NAME(C1_, con_time);
                ++C1_[0];
                ar & SERIALIZATION_NVP_NAME(C1_, error);
                ++C1_[0];
                ar & SERIALIZATION_NVP_NAME(C1_, resp_time);

                C2_[1] = 'N';
                ar & SERIALIZATION_NVP_NAME(C2_, try_times);
                C2_[1] = 'E';
                ar & SERIALIZATION_NVP_NAME(C2_, last_error);
                C2_[1] = 'T';
                ar & SERIALIZATION_NVP_NAME(C2_, total_time);
            }

        private:
            char C1_[4];
            char C2_[4];
        };

        struct DacPlayOpenInfo    // 开始播放需要提交的数据
            : DacCoreCommInfo
        {
            // dt的连接信息
            dac_server_info dt_info;

            // JN:dt返回的服务器地址
            std::string dt_server_host;

            // drag的连接信息
            dac_server_info drag_info;

            // SDK连接视频端口的时间：（单位毫秒）
            // 如果有P2P内核，则是SDK连上P2P内核的时间 （只记录Open的那次，不记录后面Seek的）
            // 如果没有P2P内核，这是PPBOX直接连接CDN视频服务器的时间
            // 其中 正数表示连接时间，-1表示连接失败
            dac_server_info peer_info;

            // TE:Open最终错误码
            dac_errno open_last_error;

            // TN:Open总时间（单位毫秒）
            dac_time open_total_time;

            // 内容名称：提交播放串，服务器端好统计 频道日流量 视频日流量
            std::string name;

            DacPlayOpenInfo(
                ppbox::cdn::PptvMedia const & media, 
                ppbox::peer::PeerSource const & source, 
                ppbox::demux::DemuxStatistic const & demux_stat)
                : DacCoreCommInfo(InterfaceType::play_open_info_type)
                , dt_info("KJ")
                , drag_info("OD")
                , peer_info("SP")
            {
                std::vector<ppbox::cdn::HttpStatistics> const & openlogs = media.open_logs();

                name = media.video().name;

                dt_server_host = media.jump().server_host.to_string();

                size_t i = 0;
                if (openlogs.size() > i) {
                    dt_info.set_stat(openlogs[i]);
                }
                i++;
                if (openlogs.size() > i) {
                    drag_info.set_stat(openlogs[i]);
                }
                i++;

                open_last_error = demux_stat.last_error();

                open_total_time = demux_stat.elapse();

                peer_info.set_stat(source.http_stat());
            }

            virtual LogReason::Enum need_log(
                boost::system::error_code & ec)
            {
                if (open_last_error) {
                    ec = open_last_error;
                    return LogReason::open_fail;
                } else if (open_total_time.time > 10000) { // 10秒
                    ec = open_last_error;
                    return LogReason::open_too_long;
                } else {
                    return LogReason::none;
                }
            }

            virtual void do_serialize(
                DacArchive & ar)
            {
                DacCoreCommInfo::do_serialize(ar);
                ar & dt_info;
                ar & SERIALIZATION_NVP_NAME("JS", dt_server_host);
                ar & drag_info;
                ar & peer_info;
                ar & SERIALIZATION_NVP_NAME("TE", open_last_error);
                ar & SERIALIZATION_NVP_NAME("TT", open_total_time);
                ar & SERIALIZATION_NVP_NAME("V", name);
            }
        };

    } // namespace dac
} // namespace ppbox

#endif // _PPBOX_DAC_DAC_INFO_PLAY_OPEN_H_
