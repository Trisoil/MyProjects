// UdpServer.h

#ifndef _NETWORK_UDP_SERVER_H_
#define _NETWORK_UDP_SERVER_H_

#include "network/UdpBuffer.h"
#include "protocol/Packet.h"

#include <util/RefenceFromThis.h>

namespace network
{

    struct IUdpServerListener
    {
        typedef IUdpServerListener * p;

        virtual void OnUdpRecv(
            protocol::Packet const & packet) = 0;

        virtual ~IUdpServerListener()
        {
        }
    };

    class UdpServer
        : boost::asio::ip::udp::socket
        , public util::RefenceFromThis<UdpServer>
    {
    public:
        UdpServer(
            boost::asio::io_service & io_svc, 
            IUdpServerListener::p handler)
            : boost::asio::ip::udp::socket(io_svc)
            , handler_(handler)
            , port_(0)
        {
        }
        
    public:
        bool Listen(
            boost::uint16_t port);

        size_t Recv(
            size_t count);

        void Close();

        boost::uint16_t GetUdpPort() const
        {
            if (!is_open())
                return 0;
            return port_;
        }

        template <typename Packet>
        void register_packet();

        void register_all_packets();

        template <typename PacketType>
        int send_packet(
            PacketType const & packet);

        template <typename T>
        int send_packet(
			boost::shared_ptr<T> const & packet)
		{
			return this->send_packet( *packet.get() );
		}

    protected:
        void UdpRecvFrom(
            UdpBuffer & recv_buffer);

        void UdpSendTo(
            UdpBuffer & send_buffer);

        void HandleUdpRecvFrom(
            const boost::system::error_code& error, 
            std::size_t bytes_transferred,
            UdpBuffer & recv_buffer);

        void HandleUdpSendTo(
            boost::system::error_code const & error, 
            std::size_t bytes_transferred,
            UdpBuffer const & send_buffer);

        template <typename Packet>
        void handle_packet(
            UdpBuffer & buffer);

    private:
        typedef void (UdpServer::*packet_handler_type)(
            UdpBuffer & buffer);

        IUdpServerListener::p handler_;
        boost::uint16_t port_;
        std::map<boost::uint8_t, packet_handler_type> packet_handlers_;
    };
}

#include <network/UdpServerHandlePacket.h>

#endif //_NETWORK_UDP_SERVER_H_
