// UdpServer.h

#ifndef _UDPSERVER_H_
#define _UDPSERVER_H_

#include "CacheManager.h"

struct UdpDataRequestPacket2;

struct UdpServerStatistics;

class UdpServer
{
public:
    static Util::ErrorCode init(
        Util::Config & conf);

    static Util::ErrorCode start(
        boost::asio::io_service & io_service);

    static Util::ErrorCode stop();

private:
    static void HandleReceive(
        boost::asio::error_code const & err, 
        size_t size, 
        UdpDataRequestPacket2 * packet);

    static void HandleReadData(
        Util::ErrorCode err, 
        size_t size, 
        UdpDataRequestPacket2 * packet);

    static void HandleSend(
        boost::asio::error_code const & err, 
        UdpDataRequestPacket2 * packet);

    static void Statistics();

private:
    static std::string ip;
    static int port;
    static size_t max_sync_recv;
    static size_t interval_statistics;
    static int log_level;

private:
    static boost::asio::ip::udp::endpoint endpoint_;
    static boost::asio::ip::udp::socket * socket_;
    static UdpServerStatistics * stat_;
    static size_t wait_sync_recv;
};

#endif // #ifndef _UDPSERVER_H_
