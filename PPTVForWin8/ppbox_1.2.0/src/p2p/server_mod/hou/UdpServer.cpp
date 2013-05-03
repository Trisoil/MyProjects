//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#include "Common.h"
#include "UdpServer.h"
#include "HouServer.h"

#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>

const boost::uint32_t max_length = 2048;

namespace hou
{
    UdpServer::UdpServer(boost::asio::io_service & io_svc, IUdpServerListener::pointer listener)
        :io_svc_(io_svc), listener_(listener), socket_(io_svc)
    {
    }

    UdpServer::pointer UdpServer:: Create(boost::asio::io_service & io_svc, IUdpServerListener::pointer listener)
    {
        return pointer(new UdpServer(io_svc,listener));
    }

    bool UdpServer:: Listen(unsigned short port)
    {
        try
        {
            socket_.open(boost::asio::ip::udp::v4());
            socket_.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(),port));	
        }
        catch (...)
        {
            std::cout << "udpserver bind port : " << port << " failed " << std::endl;
            return false;
        }

        return true;
    }

    void UdpServer::Recv(boost::uint32_t num)
    {
        for (boost::uint32_t i = 0 ;i < num; i++)
        { 
            EndpointPointer sender_endpoint(new boost::asio::ip::udp::endpoint());
            BufferPointer buf(new boost::asio::streambuf());

            boost::function<void (boost::system::error_code, boost::uint32_t)> func = boost::bind(&UdpServer::UdpRecvHandler, shared_from_this(),
                buf,
                sender_endpoint,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred);
            assert(func);
            socket_.async_receive_from(buf->prepare(max_length), *sender_endpoint, func);
        }
    }

    void UdpServer::UdpRecvHandler(BufferPointer buf, EndpointPointer sender_endpoint, boost::system::error_code error,
        boost::uint32_t bytes_transferred )
    {
        if (!error)
        {
            buf->commit(bytes_transferred);
            listener_->OnUdpRecv(*sender_endpoint, *buf);
            buf->consume(bytes_transferred);
        }
        else
        {
            LOG4CPLUS_WARN(Loggers::HouService(), "udpserver receive error, error message: " << error.message());
        }

        

        buf->reset();
        boost::function<void (boost::system::error_code, boost::uint32_t)> func = boost::bind(&UdpServer::UdpRecvHandler, shared_from_this(),
            buf,
            sender_endpoint,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred);
        assert(func);
        socket_.async_receive_from(buf->prepare(max_length), *sender_endpoint, func);
    }

    void UdpServer::SendTo(boost::asio::ip::udp::endpoint const & dest_ep, boost::asio::streambuf const & buf)
    {
        boost::system::error_code ec;
        socket_.send_to(buf.data(),dest_ep, 0, ec);
        if (ec)
        {
            LOG4CPLUS_ERROR(Loggers::HouService(), "UdpServer: socket.send_to() an error code. err_message: " << ec.message() << " error_code: " << ec.value());
        }
    }
}