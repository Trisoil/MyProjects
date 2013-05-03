// HttpHandle.h

#ifndef _MUTEX_CLIENT_HTTP_HANDLE_H_
#define _MUTEX_CLIENT_HTTP_HANDLE_H_

#include "mutex/client/ProtocolHandle.h"

#include <boost/asio/ip/udp.hpp>
using namespace boost::asio::ip;

using namespace boost::system;

namespace util
{
    namespace protocol
    {
        class HttpClient;
        class HttpRequest;
    }
}

namespace mutex
{
    namespace client
    {

        class HttpHandle 
            : public ProtocolHandle
        {
        public:
            HttpHandle(
                boost::asio::io_service & io_svc);

            ~HttpHandle(void);

            boost::system::error_code connect(
                NetName const & addr, 
                boost::system::error_code & ec);

        private:
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
                PacketBuffer<char> & packet);

            void handle_receive(
                boost::system::error_code const & ec);

        private:
            boost::asio::io_service & io_svc_;
            util::protocol::HttpClient * http_;
            util::protocol::HttpRequest * request_;
            bool is_busy_;
            bool is_pending_;
            bool is_received_;
        };

    } // namespace client
} // namespace mutex

#endif // _MUTEX_CLIENT_HTTP_HANDLE_H_
