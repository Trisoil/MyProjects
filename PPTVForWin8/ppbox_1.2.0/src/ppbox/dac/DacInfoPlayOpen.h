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
            // DNS����ʱ�䣺����λ���룩������ʾ�ɹ��Ľ���ʱ��, -1��ʾ����ʧ��
            dac_time dns_time;

            // ����ʱ�䣺����λ���룩��ʾ�ӿ�ʼ���ӵ����ϵ�ʱ��
            // ������ʾ�ɹ�������ʱ�� -1��ʾ����ʧ��
            dac_time con_time;

            // HTTP����ֵ����������ʾHTTP�ķ���ֵ������200,206,404,403,502�ȣ�����-1��ʾ��ʽ����
            dac_errno error;

            // ��Ӧʱ�䣺����λ���룩��ʾdt�����ӳɹ�������������ݵ�ʱ��
            // ������ʾ�ɹ�������ʱ��
            dac_time resp_time;

            // N:dt���ԵĴ���
            boost::uint32_t try_times;

            // E:���մ�����
            dac_errno last_error;

            // N:��ʱ�䣨��λ���룩
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

        struct DacPlayOpenInfo    // ��ʼ������Ҫ�ύ������
            : DacCoreCommInfo
        {
            // dt��������Ϣ
            dac_server_info dt_info;

            // JN:dt���صķ�������ַ
            std::string dt_server_host;

            // drag��������Ϣ
            dac_server_info drag_info;

            // SDK������Ƶ�˿ڵ�ʱ�䣺����λ���룩
            // �����P2P�ںˣ�����SDK����P2P�ں˵�ʱ�� ��ֻ��¼Open���ǴΣ�����¼����Seek�ģ�
            // ���û��P2P�ںˣ�����PPBOXֱ������CDN��Ƶ��������ʱ��
            // ���� ������ʾ����ʱ�䣬-1��ʾ����ʧ��
            dac_server_info peer_info;

            // TE:Open���մ�����
            dac_errno open_last_error;

            // TN:Open��ʱ�䣨��λ���룩
            dac_time open_total_time;

            // �������ƣ��ύ���Ŵ����������˺�ͳ�� Ƶ�������� ��Ƶ������
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
                } else if (open_total_time.time > 10000) { // 10��
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
