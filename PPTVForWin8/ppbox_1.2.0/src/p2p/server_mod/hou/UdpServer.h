//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#ifndef _HOU_UDP_SERVER_H
#define _HOU_UDP_SERVER_H


#include <framework/timer/AsioTimerManager.h>


namespace hou
{
    class IUdpServerListener
    {
    public:
        typedef boost::shared_ptr<IUdpServerListener> pointer;

        virtual void OnUdpRecv(boost::asio::ip::udp::endpoint const & src_ep,
            boost::asio::streambuf & buf) = 0;
        virtual ~IUdpServerListener(){};
    };

    class UdpServer
        : public boost::noncopyable
        , public boost::enable_shared_from_this<UdpServer>
    {
        typedef boost::shared_ptr<boost::asio::ip::udp::endpoint> EndpointPointer;
        typedef boost::shared_ptr<boost::asio::streambuf> BufferPointer;

    public:
        typedef boost::shared_ptr<UdpServer> pointer;
        static pointer Create(boost::asio::io_service & io_svc, IUdpServerListener::pointer listener);

        bool Listen(unsigned short port);
        void Recv(boost::uint32_t num);
        void SendTo(boost::asio::ip::udp::endpoint const & dest_ep, boost::asio::streambuf const & buf);

    private:
        UdpServer(boost::asio::io_service & io_svc, IUdpServerListener::pointer listener);
        void UdpRecvHandler(BufferPointer buf, EndpointPointer sender_endpoint, boost::system::error_code error,
            boost::uint32_t bytes_transferred );
    private:
        boost::asio::io_service & io_svc_;
        IUdpServerListener::pointer listener_;
        boost::asio::ip::udp::socket socket_;
    };
}

#endif