// DacInfoPlayClose.h

#ifndef _PPBOX_DAC_DAC_INFO_PLAY_CLOSE_H_
#define _PPBOX_DAC_DAC_INFO_PLAY_CLOSE_H_

#include "ppbox/dac/DacInfo.h"

#include <ppbox/cdn/PptvMedia.h>

#include <ppbox/demux/base/DemuxStatistic.h>
#include <ppbox/data/base/DataStatistic.h>

namespace ppbox
{
    namespace dac
    {

        struct DacPlayCloseInfo    // 结束播放需要提交的数据
            : DacCoreCommInfo
        {
            // 平均下载速度：（单位KB/s）
            // 如果有内核，则内核统计，如果没有内核，则SDK统计。
            boost::uint32_t avg_dl_speed;

            // 内核统计的CDN视频服务器的请求响应时间：（单位毫秒）
            // 如果没有P2P内核，则此值为0
            boost::uint32_t va_cdn_res_time;

            // 峰值带宽（最大下载速度）：（单位KB/s）
            // 如果有内核，则内核统计，如果没有内核，则SDK统计。
            boost::uint32_t max_dl_speed;

            // 观看时间：（单位毫秒）
            // 从开始调用 Open开始 到 Close 的时间
            boost::uint64_t total_play_time;

            // 有效观看时间：（单位毫秒）
            boost::uint64_t play_time;

            // 启动缓冲时间：（单位毫秒）
            // 从调用Open函数 到 第一次ReadSample读取到数据的 时间
            boost::uint64_t open_buf_time;

            // 拖动次数： 调用Seek的次数
            size_t seek_count;

            // 拖动缓冲时间：（单位毫秒）
            // 从调用Seek函数 到 第一次ReadSample读取到数据的 时间 为一次拖动缓冲时间
            // 拖动缓冲时间 为 每次拖动缓冲时间 之和
            boost::uint64_t seek_block_time;

            // 中途缓冲次数
            size_t block_count;

            // 中途缓冲时间：（单位毫秒）
            // 中途缓冲时间为 每次中途缓冲时间 之和
            boost::uint64_t block_time;

            // 视频码流率：（单位Kbps）
            boost::uint32_t bit_rate;

            // 内容名称：提交播放串，服务器端好统计 某一部影片卡的次数
            std::string name;

            // 总发送字节数
            boost::uint64_t total_bytes;

            // 用于记录传入的额外参数值
            std::string param;

            DacPlayCloseInfo(
                ppbox::cdn::PptvMedia const & media, 
                ppbox::demux::DemuxStatistic const & demux_stat, 
                ppbox::data::DataStatistic const & buf_stat)
                : DacCoreCommInfo(InterfaceType::play_close_info_type)
            {
                using namespace ppbox::demux;

                time_t now = time(NULL);
                if (now > buf_stat.start_time)
                    avg_dl_speed = (boost::uint32_t)((buf_stat.total_bytes / 1024) / (now - buf_stat.start_time));
                else
                    avg_dl_speed = 0;
                max_dl_speed = buf_stat.speeds[1].peak_speed / 1024;
                total_bytes = buf_stat.total_bytes;

                //TODO:暂时不提供
                va_cdn_res_time = 0;

                boost::uint64_t opened_time = 0;        // open完成时间
                boost::uint64_t first_read_time = 0;    // 第一次ReadSample读取到数据的时间
                boost::uint64_t paused_time = 0;        // 总共有效的播放时间

                boost::uint64_t open_block_time = 0;    // 启动缓冲时间

                boost::uint64_t send_time = 0;
                boost::uint64_t used_time = 0;

                std::vector<StatusInfo> const & statusinfo = demux_stat.status_info();

                for (size_t i = 0; i < statusinfo.size(); ++i) {
                    total_play_time += statusinfo[i].elapse;

                    if (statusinfo[i].status_type == DemuxStatistic::opening) {
                        opened_time = statusinfo[i].elapse;
                    } else if (statusinfo[i].status_type == DemuxStatistic::opened) {
                        open_block_time = statusinfo[i].elapse;
                    } else if (statusinfo[i].status_type == DemuxStatistic::playing) {
                        play_time += (statusinfo[i+1].play_position - statusinfo[i].play_position);
                        send_time += (statusinfo[i+1].play_position - statusinfo[i].play_position);
                        used_time += statusinfo[i].elapse;
                    } else if (statusinfo[i].status_type == (BlockType::seek | DemuxStatistic::playing)) {
                        seek_count++;
                        // 因为该状态持续时间很短，所以忽略不计
                        //playing_time += (statusinfo[i+1].play_position - statusinfo[i].play_position);
                        // 重置 send_time used_time
                        //send_time = (statusinfo[i+1].play_position - statusinfo[i].play_position);
                        //used_time = statusinfo[i].elapse;
                    } else if (statusinfo[i].status_type == DemuxStatistic::paused) {
                        paused_time += statusinfo[i].elapse;
                    } else if (statusinfo[i].status_type == (BlockType::init | DemuxStatistic::buffering)) {
                        first_read_time = statusinfo[i].elapse;
                    } else if (statusinfo[i].status_type == (BlockType::play | DemuxStatistic::buffering)) {
                        used_time += statusinfo[i].elapse;
                        if (send_time < used_time) {
                            block_count++;
                            send_time = 0;
                            used_time = 0;
                        }
                    } else if (statusinfo[i].status_type == (BlockType::seek | DemuxStatistic::buffering)) {
                        seek_block_time += statusinfo[i].elapse;
                        seek_count++;
                        // seek引起的卡也算卡
                        block_count++;
                        send_time = 0;
                        used_time = 0;
                    }
                }

                open_buf_time = opened_time + open_block_time + first_read_time;
                boost::uint64_t t = play_time + paused_time + open_buf_time + seek_block_time;
                if (total_play_time <= t) {
                    block_time = 0;
                } else {
                    block_time = total_play_time - t;
                }

                bit_rate = media.video().bitrate / 128;
                name = media.video().name;
            }

            virtual void do_serialize(
                DacArchive & ar)
            {
                DacCoreCommInfo::do_serialize(ar);
                ar & SERIALIZATION_NVP_NAME("K", avg_dl_speed);
                ar & SERIALIZATION_NVP_NAME("L", va_cdn_res_time);
                ar & SERIALIZATION_NVP_NAME("M", max_dl_speed);
                ar & SERIALIZATION_NVP_NAME("N", total_play_time);
                ar & SERIALIZATION_NVP_NAME("O", play_time);
                ar & SERIALIZATION_NVP_NAME("P", open_buf_time);
                ar & SERIALIZATION_NVP_NAME("Q", seek_count);
                ar & SERIALIZATION_NVP_NAME("R", seek_block_time);
                ar & SERIALIZATION_NVP_NAME("S", block_count);
                ar & SERIALIZATION_NVP_NAME("T", block_time);
                ar & SERIALIZATION_NVP_NAME("U", bit_rate);
                ar & SERIALIZATION_NVP_NAME("V", name);
                ar & SERIALIZATION_NVP_NAME("W", total_bytes);
                ar & SERIALIZATION_NVP_NAME("EHS", param);
            }

        };

    } // namespace dac
} // namespace ppbox

#endif // _PPBOX_DAC_DAC_INFO_PLAY_CLOSE_H_
