// Auth.h

#ifndef _PPBOX_ALIVE_AUTH_H_
#define _PPBOX_ALIVE_AUTH_H_

#include <ppbox/certify/KeepaliveCommand.h>

#include <mutex/client/MutexServerInfo.h>

#include <util/protocol/http/HttpClient.h>

#include <framework/timer/TimeTraits.h>

#include <boost/function.hpp>

namespace ppbox
{
    namespace alive
    {

        struct AuthRequest
        {
            AuthRequest()
            {
            }

            std::string gid;
            std::string pid;
            std::string auth;
            std::string clientid;
            std::string hid;
            std::string mid;
            std::string aid;
            std::string version;
        };

        struct AuthResponse
        {
            AuthResponse()
                : isSuccess(false)
                , mutex_enable(true)
                , reason(0)
            {
            }

            void init()
            {
                isSuccess = false;
                key = NULL;
                session_id = "";
                vod_playlink_key = "";
                live_playlink_key = "";
                server_time = 0;
                mutex_enable = true;
                mutex_servers.clear();
                mutex_encrypt = "";
                reason = 0;
            }

            bool isSuccess;
            char const * key;
            std::string session_id;
            std::string vod_playlink_key;
            std::string live_playlink_key;
            boost::uint64_t server_time;
            bool mutex_enable;
            std::vector< ::mutex::client::MutexServerInfo> mutex_servers;
            std::string mutex_encrypt;
            boost::uint32_t reason;
        };

        class Auth
        {
        public:
            Auth(
                boost::asio::io_service & io_svc);

            ~Auth();

        public:
            typedef boost::function<void (
                AuthResponse const &, 
                boost::system::error_code const &)> certify_response_type;

            void async_certify(
                AuthRequest const & request, 
                certify_response_type const & resp);

            void cancel(
                boost::system::error_code & ec);

            util::protocol::HttpClient::Statistics const & get_stat() const
            {
                return first_stat_;
            }

            boost::system::error_code last_error() const
            {
                return error_last_;
            }

        private:
            std::string get_auth_string(
                AuthRequest const & request) const;

            void on_certify_timer(
                boost::system::error_code const & ec);

            void handle_certify(
                boost::system::error_code const & ec);

            boost::system::error_code parse_response(
                AuthResponse & authrespone, 
                boost::system::error_code & ec);

            boost::system::error_code decrypt_md5(
                char const * key,
                boost::system::error_code & ec);

        private:
            boost::uint32_t random_;
            util::protocol::HttpClient http_;
            boost::system::error_code error_last_;
            certify_response_type certify_response_;

            boost::uint32_t auth_retry_times_;
            util::protocol::HttpClient::Statistics first_stat_;

            clock_timer * timer_;
        };

    } // namespace alive
} // namespace ppbox

#endif // _PPBOX_ALIVE_AUTH_H_

