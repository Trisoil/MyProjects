// UdpHandle.h

#ifndef _MUTEX_CLIENT_UDP_HANDLE_H_
#define _MUTEX_CLIENT_UDP_HANDLE_H_

#include "mutex/client/ProtocolHandle.h"
#include "mutex/client/PacketBuffer.h"

#include <boost/asio/ip/udp.hpp>

namespace mutex
{
    namespace client
    {

        class UdpHandle
            : public ProtocolHandle
        {
        public:
            UdpHandle(
                boost::asio::io_service & io_svc);

            virtual ~UdpHandle(void);

        private:
            virtual boost::system::error_code connect(
                framework::network::NetName const & addr,
                boost::system::error_code & ec);

            virtual void do_send_packet(
                std::streambuf & packet);

            virtual std::streambuf & create_packet();

            virtual void delete_packet(
                std::streambuf & packet);

            virtual void do_stop();

        private:
            void handle_send(
                boost::system::error_code const & ec, 
                size_t len,
                std::streambuf & packet);

            void handle_receive(
                boost::system::error_code const & ec, 
                size_t len);

        private:
            boost::asio::ip::udp::socket socket_;
            PacketBuffer<char> recv_buf_;
        };

    } // namespace client
} // namespace mutex

#endif // _MUTEX_CLIENT_UDP_HANDLE_H_
