// UdpServer.h

#ifndef _MUTEX_SERVER_UDP_SERVER_H_
#define _MUTEX_SERVER_UDP_SERVER_H_

#include "mutex/server/Common.h"
#include "mutex/server/MutexSession.h"

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

namespace mutexserver
{
    // UDP�հ� �� ������
    struct UDPRecvBuffer
    {
        // ÿ��UDP�հ�������ֽ���,��ΪUDPû�о�ȷ�հ���С
        static const size_t UDP_RECV_BUFFER_SIZE = 4*1024;
        
        // ����UDP���������ݵĻ�����
        boost::shared_array<char> data_;
        
        // �Է���endpoint
        boost::shared_ptr<boost::asio::ip::udp::endpoint> remote_endpoint_;
        
        UDPRecvBuffer() 
            : data_(new char[UDP_RECV_BUFFER_SIZE])
            , remote_endpoint_(new boost::asio::ip::udp::endpoint)
        {};
    };

    class UdpServer
    {
    public:
        UdpServer(
            boost::asio::io_service& io_service) 
            : socket_(io_service)
            , is_open_(false)
        {}

    public:
        bool Listen(
            boost::uint16_t port);

        void StartRecv();

        void UdpSendTo(
            boost::asio::const_buffers_1 buffer,
            const boost::asio::ip::udp::endpoint& endpoint);

        void Close();

        boost::uint16_t GetUdpPort() const
        {
            if (!is_open_)
                return 0;
            return port_;
        }

    protected:
        void HandleUdpRecvFrom(
            const boost::system::error_code& error, 
            std::size_t bytes_transferred, 
            UDPRecvBuffer& recv_buffer);

        void HandleUdpSendTo(
            const boost::system::error_code& error, 
            std::size_t bytes_transferred);

    private:
        boost::asio::ip::udp::socket socket_;
        bool is_open_;
        boost::uint16_t port_;
        UDPRecvBuffer recv_buffer_;
    };

}

#endif // _MUTEX_SERVER_UDP_SERVER_H_
