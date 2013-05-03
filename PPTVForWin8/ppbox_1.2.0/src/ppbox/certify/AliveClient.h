// AliveClient.h

#ifndef _PPBOX_CERTIFY_ALIVE_CLIENT_H_
#define _PPBOX_CERTIFY_ALIVE_CLIENT_H_

#include <framework/timer/TimeTraits.h>
#include <framework/network/NetName.h>

#include <boost/function.hpp>

namespace util
{
    namespace protocol
    {
        class HttpClient;
    }
}

namespace ppbox
{
    namespace certify
    {

        struct KeepaliveRequest;
        struct KeepaliveResponse;

        class AliveClient
        {
        public:
            AliveClient(
                boost::asio::io_service & io_svc);

            ~AliveClient();

        public:
            typedef boost::function<void (
                KeepaliveResponse const &)> certify_response_type;

            void async_keepalive(
                KeepaliveRequest const & auth_request, 
                certify_response_type const & resp);

            void cancel();

            void set_port(boost::uint16_t port);

        private:
            void on_keepalive_timer(
                boost::system::error_code const & ec);

            void handle_keepalive(
                boost::system::error_code const & ec);

            void parse_keepalive_response(
                KeepaliveResponse & response);

        private:
            util::protocol::HttpClient * http_;
            certify_response_type certify_response_;

            clock_timer * timer_;

            std::string gid_;
            std::string pid_;
            std::string auth_;

            std::string bid_;

            Time last_time_;

            framework::network::NetName addr_;
        };

    } // namespace certify
} // namespace ppbox

#endif // _PPBOX_CERTIFY_ALIVE_CLIENT_H_

