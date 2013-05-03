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

        struct DacAuthInfo      // 认证完成时需要提交的数据
            : DacOnlyOnce<DacBaseInfo, DacAuthInfo>
        {
            // auth的DNS解析时间：（单位毫秒）正数表示成功的解析时间, -1表示解析失败
            boost::uint32_t auth_dns_time;

            // auth的连接时间：（单位毫秒）表示auth在开始连接到连上的时间
            // 正数表示成功的连接时间 -1表示连接失败
            boost::uint32_t auth_con_time;

            // auth的HTTP返回值： 
            // （整数表示HTTP的返回值，例如200,206,404,403,502等，另外-1表示格式错误）
            boost::system::error_code auth_http_ret;

            // auth的响应时间：（单位毫秒）表示auth在连接成功都获得所有数据的时间
            // 正数表示成功的连接时间
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

        struct DacMutexInfo     // MutexClient确定服务器协议时需要提交的数据
            : DacOnlyOnce<DacBaseInfo, DacMutexInfo>
        {
            // 最终确定的协议：1-UDP协议,2-HTTP协议
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

        struct DacTerminateInfo // SDK主动终止运行时需要提交的数据
            : DacBaseInfo
        {
            // 异常终止的原因：1-认证失败,2-被踢掉,3-连不上Mutex服务器,4-长时间Mutex服务器无回应,5-认证服务器无响应
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
