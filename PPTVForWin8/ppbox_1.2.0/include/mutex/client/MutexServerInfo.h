// MutexServerInfo.h

#ifndef _MUTEX_CLIENT_MUTEX_SERVER_INFO_H_
#define _MUTEX_CLIENT_MUTEX_SERVER_INFO_H_

namespace mutex
{
    namespace client
    {

        struct MutexServerInfo
        {
            MutexServerInfo()
                : udp_port(0)
                , http_port(0)
            {
            }

            MutexServerInfo(
                std::string ip,
                boost::uint16_t u_port,
                boost::uint16_t h_port)
                : mutex_server_ip(ip)
                , udp_port(u_port)
                , http_port(h_port)
            {
            }

            std::string mutex_server_ip;
            boost::uint16_t udp_port;
            boost::uint16_t http_port;
        };

    } // namespace client
} // namespace mutex

#endif // _MUTEX_CLIENT_MUTEX_SERVER_INFO_H_
