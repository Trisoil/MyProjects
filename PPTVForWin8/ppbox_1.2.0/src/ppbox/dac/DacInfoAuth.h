// DacInfoAuth.h

#ifndef _PPBOX_DAC_DAC_INFO_AUTH_H_
#define _PPBOX_DAC_DAC_INFO_AUTH_H_

#include "ppbox/dac/DacInfo.h"

#include <ppbox/certify/CertifyError.h>

#include <mutex/client/MutexError.h>

#include <util/protocol/http/HttpClient.h>

namespace ppbox
{
    namespace dac
    {

        struct DacAuthInfo      // ��֤���ʱ��Ҫ�ύ������
            : DacOnlyOnce<DacBaseInfo, DacAuthInfo>
        {
            // auth��DNS����ʱ�䣺����λ���룩������ʾ�ɹ��Ľ���ʱ��, -1��ʾ����ʧ��
            boost::uint32_t auth_dns_time;

            // auth������ʱ�䣺����λ���룩��ʾauth�ڿ�ʼ���ӵ����ϵ�ʱ��
            // ������ʾ�ɹ�������ʱ�� -1��ʾ����ʧ��
            boost::uint32_t auth_con_time;

            // auth��HTTP����ֵ�� 
            // ��������ʾHTTP�ķ���ֵ������200,206,404,403,502�ȣ�����-1��ʾ��ʽ����
            boost::system::error_code auth_http_ret;

            // auth����Ӧʱ�䣺����λ���룩��ʾauth�����ӳɹ�������������ݵ�ʱ��
            // ������ʾ�ɹ�������ʱ��
            boost::uint32_t auth_res_time;

            DacAuthInfo(
                util::protocol::HttpClient::Statistics const & stat)
                : DacOnlyOnce<DacBaseInfo, DacAuthInfo>(InterfaceType::auth_info_type)
            {
                auth_dns_time = stat.resolve_time;
                if (stat.connect_time != (boost::uint32_t)-1)
                    auth_con_time = stat.connect_time - stat.resolve_time;
                auth_http_ret = stat.last_error;
                if (stat.response_data_time != (boost::uint32_t)-1)
                    auth_res_time = stat.response_data_time - stat.connect_time;
            }

            virtual void do_serialize(
                DacArchive & ar)
            {
                DacBaseInfo::do_serialize(ar);
                ar & SERIALIZATION_NVP_NAME("I", auth_dns_time);
                ar & SERIALIZATION_NVP_NAME("J", auth_con_time);
                ar & SERIALIZATION_NVP_NAME("K", auth_http_ret);
                ar & SERIALIZATION_NVP_NAME("L", auth_res_time);
            }
        };

        struct DacMutexInfo     // MutexClientȷ��������Э��ʱ��Ҫ�ύ������
            : DacOnlyOnce<DacBaseInfo, DacMutexInfo>
        {
            // ����ȷ����Э�飺1-UDPЭ��,2-HTTPЭ��
            ::mutex::client::ProtocolType::Enum protocol_type;

            DacMutexInfo(
                ::mutex::client::ProtocolType::Enum protocol_type)
                : DacOnlyOnce<DacBaseInfo, DacMutexInfo>(InterfaceType::mutex_info_type)
                , protocol_type(protocol_type)
            {
            }

            virtual void do_serialize(
                DacArchive & ar)
            {
                DacBaseInfo::do_serialize(ar);
                ar & SERIALIZATION_NVP_NAME("I", protocol_type);
            }
        };

        struct DacTerminateInfo // SDK������ֹ����ʱ��Ҫ�ύ������
            : DacBaseInfo
        {
            // �쳣��ֹ��ԭ��1-��֤ʧ��,2-���ߵ�,3-������Mutex������,4-��ʱ��Mutex�������޻�Ӧ,5-��֤����������Ӧ
            TerminateType::Enum terminate_type;

            DacTerminateInfo(
                boost::system::error_code const & type)
                : DacBaseInfo(InterfaceType::terminate_info_type)
            {
                if (0) {
                } else if (type.category() == mutex::client::error::get_category()) {
                    if (type == mutex::client::error::kickout) {
                        terminate_type = TerminateType::mutex_kickout_packet;
                    } else if (type == mutex::client::error::use_all_mutex_server) {
                        terminate_type = TerminateType::mutex_not_connect;
                    } else if (type == mutex::client::error::time_out) {
                        terminate_type = TerminateType::mutex_time_out;
                    } else if (type == mutex::client::error::mev_failed) {
                        terminate_type = TerminateType::mutex_mev_failed;
                    }
                } else if (type.category() == ppbox::certify::error::get_auth_category()) {
                    if (type == ppbox::certify::error::time_out) {
                        terminate_type = TerminateType::auth_time_out;
                    } else {
                        terminate_type = TerminateType::auth_error;
                    }
                } else {
                    terminate_type = TerminateType::other_error;
                }
            }

            virtual void do_serialize(
                DacArchive & ar)
            {
                DacBaseInfo::do_serialize(ar);
                ar & SERIALIZATION_NVP_NAME("I", terminate_type);
            }
        };

    } // namespace dac
} // namespace ppbox

#endif // _PPBOX_DAC_DAC_INFO_AUTH_H_
