// UdpHandle.h

#ifndef _MUTEX_CLIENT_UDP_HANDLE_H_
#define _MUTEX_CLIENT_UDP_HANDLE_H_

#include "mutex/client/ProtocolHandle.h"

#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/udp.hpp>
using namespace boost::asio::ip;

using namespace boost::system;

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

        public:
            virtual boost::system::error_code connect(
                NetName const & addr,
                boost::system::error_code & ec);

        protected:
            virtual void do_send_packet(
                std::streambuf & packet);

            virtual std::streambuf & create_packet();

            virtual void delete_packet(
                std::streambuf &packet);

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
            boost::asio::io_service & io_svc_;
            udp::socket socket_;
            PacketBuffer<char> recv_buf_;
        };

    } // namespace client
} // namespace mutex

#endif // _MUTEX_CLIENT_UDP_HANDLE_H_
