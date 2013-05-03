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

        struct DacPlayCloseInfo    // ����������Ҫ�ύ������
            : DacCoreCommInfo
        {
            // ƽ�������ٶȣ�����λKB/s��
            // ������ںˣ����ں�ͳ�ƣ����û���ںˣ���SDKͳ�ơ�
            boost::uint32_t avg_dl_speed;

            // �ں�ͳ�Ƶ�CDN��Ƶ��������������Ӧʱ�䣺����λ���룩
            // ���û��P2P�ںˣ����ֵΪ0
            boost::uint32_t va_cdn_res_time;

            // ��ֵ������������ٶȣ�������λKB/s��
            // ������ںˣ����ں�ͳ�ƣ����û���ںˣ���SDKͳ�ơ�
            boost::uint32_t max_dl_speed;

            // �ۿ�ʱ�䣺����λ���룩
            // �ӿ�ʼ���� Open��ʼ �� Close ��ʱ��
            boost::uint64_t total_play_time;

            // ��Ч�ۿ�ʱ�䣺����λ���룩
            boost::uint64_t play_time;

            // ��������ʱ�䣺����λ���룩
            // �ӵ���Open���� �� ��һ��ReadSample��ȡ�����ݵ� ʱ��
            boost::uint64_t open_buf_time;

            // �϶������� ����Seek�Ĵ���
            size_t seek_count;

            // �϶�����ʱ�䣺����λ���룩
            // �ӵ���Seek���� �� ��һ��ReadSample��ȡ�����ݵ� ʱ�� Ϊһ���϶�����ʱ��
            // �϶�����ʱ�� Ϊ ÿ���϶�����ʱ�� ֮��
            boost::uint64_t seek_block_time;

            // ��;�������
            size_t block_count;

            // ��;����ʱ�䣺����λ���룩
            // ��;����ʱ��Ϊ ÿ����;����ʱ�� ֮��
            boost::uint64_t block_time;

            // ��Ƶ�����ʣ�����λKbps��
            boost::uint32_t bit_rate;

            // �������ƣ��ύ���Ŵ����������˺�ͳ�� ĳһ��ӰƬ���Ĵ���
            std::string name;

            // �ܷ����ֽ���
            boost::uint64_t total_bytes;

            // ���ڼ�¼����Ķ������ֵ
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

                //TODO:��ʱ���ṩ
                va_cdn_res_time = 0;

                boost::uint64_t opened_time = 0;        // open���ʱ��
                boost::uint64_t first_read_time = 0;    // ��һ��ReadSample��ȡ�����ݵ�ʱ��
                boost::uint64_t paused_time = 0;        // �ܹ���Ч�Ĳ���ʱ��

                boost::uint64_t open_block_time = 0;    // ��������ʱ��

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
                        // ��Ϊ��״̬����ʱ��̣ܶ����Ժ��Բ���
                        //playing_time += (statusinfo[i+1].play_position - statusinfo[i].play_position);
                        // ���� send_time used_time
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
                        // seek����Ŀ�Ҳ�㿨
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
