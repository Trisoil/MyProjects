#include "Common.h"

boost::asio::ip::udp::endpoint IpPortToUdpEndpoint(u_long ip, u_short port)
{
    boost::asio::ip::address_v4 addr = boost::asio::ip::address_v4(ip);
    boost::asio::ip::udp::endpoint endpoint(addr, port);
    return endpoint;
}

std::ostream& operator << (std::ostream& os, const SOCKET_ADDR& socket_addr)
{
    return os << IpPortToUdpEndpoint(socket_addr.IP, socket_addr.Port);
}

std::string GetDateString()
{
#ifdef WIN32
    char date_str[1024];

    // current time
    SYSTEMTIME time;
    ::GetLocalTime(&time);

    // date str
    int outputCount = _snprintf(date_str, 1024, "%04d.%02d.%02d_%02d.%02d.%02d",
        time.wYear,time.wMonth,time.wDay,
        time.wHour,time.wMinute,time.wSecond
        );

    return std::string(date_str);
#else
    return "now_";
#endif
}

