// MutexClient.h

#ifndef _MUTEX_CLIENT_MUTEX_CLIENT_H_
#define _MUTEX_CLIENT_MUTEX_CLIENT_H_

#include "mutex/client/ProtocolHandle.h"
#include "mutex/client/MutexServerInfo.h"

#include <framework/timer/TimeTraits.h>

#include <boost/function.hpp>

namespace mutex
{
    namespace client
    {

        struct ProtocolType
        {
            enum Enum
            {
                UDP = 1, 
                HTTP
            };
        };

        class MutexClient
        {
        public:
            typedef boost::function<
                void (boost::system::error_code const &)
            > state_call_back_type;

        public:
            MutexClient(
                boost::asio::io_service & io_svc);

            ~MutexClient();

        public:
            boost::system::error_code start(void);

            boost::system::error_code stop(void);

            boost::system::error_code cancel(void);

            boost::system::error_code timer_start(
                boost::system::error_code &ec);

            void set_mutex_info(
                std::string const & client_id,
                std::string const & session_id,
                boost::uint64_t server_time,
                std::vector< ::mutex::client::MutexServerInfo> const & mutex_servers,
                std::string const & mutex_encrypt,
                state_call_back_type const & state_func);

            ProtocolType::Enum protocol_type() const
            {
                return protocol_type_;
            }

        private:
            void on_timer(
                boost::system::error_code const & ec);

            boost::system::error_code send_keep_alive(
                boost::system::error_code & ec);

            boost::system::error_code select_mutex_server(
                boost::system::error_code & ec);

            void receive_callback(
                mutex::protocol::Head const & head, 
                std::streambuf &);

            boost::system::error_code change_mutex_server(
                boost::system::error_code & ec);

            void leave(void);

        private:
            boost::asio::io_service & io_svc_;

            clock_timer * keep_alive_timer_;
            ProtocolHandle::pointer protocol_handle_;
            state_call_back_type state_call_back_;

            std::string session_id_;
            boost::uint64_t server_time_;
            std::string mutex_encrypt_;
            std::string client_id_;
            // ×´Ì¬±äÁ¿
            std::vector< ::mutex::client::MutexServerInfo> mutex_servers_;

            bool is_first_receive_;
            boost::uint32_t try_left_;
            bool is_retry_first_index_;
            boost::uint32_t not_alive_time_;
            ProtocolType::Enum protocol_type_;
            boost::uint32_t interval_;
            boost::uint16_t first_mutex_server_index_;
            boost::uint16_t curr_mutex_server_index_;
            boost::system::error_code last_error_;

        public:
            static boost::uint8_t const CLIENTLENGTH = 16;
#ifdef _TEST_
            static boost::uint32_t const MININTERVAL = 3; // second
#else
            static boost::uint32_t const MININTERVAL = 20; // second
#endif
            static boost::uint32_t const MAXINTERVAL = 300; // second
            static boost::uint32_t const HTTPPORT    = 80;
        };

    } // namespace client
} // namespace mutex

#endif // _MUTEX_CLIENT_MUTEX_CLIENT_H_
