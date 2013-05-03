// AliveProxy.h

#ifndef _PPBOX_ALIVE_ALIVE_PROXY_H_
#define _PPBOX_ALIVE_ALIVE_PROXY_H_

#include "ppbox/alive/Auth.h"

#include <ppbox/certify/AuthStatus.h>

#include <ppbox/common/PortManager.h>

#include <mutex/client/MutexClient.h>

#include <framework/timer/TimeTraits.h>

namespace ppbox
{
    namespace certify
    {
        struct KeepaliveRequest;
    }

    namespace alive
    {

        class AliveProxy
            : public util::daemon::ModuleBase<AliveProxy>
        {
        public:
            AliveProxy(
                util::daemon::Daemon & daemon);

            ~AliveProxy();

        private:
            virtual boost::system::error_code startup();

            virtual void shutdown();

        private:
            bool start_auth(
                ppbox::certify::KeepaliveRequest const & keepalive_request);

            void auth_callback(
                AuthResponse const & response, 
                boost::system::error_code const & ec);

            void keepalive_callback(
                boost::system::error_code const & ec);

            void send_response(
                boost::system::error_code const & ec);

            void on_timer(
                boost::system::error_code const & ec);

        private:
            class ProxyManager;
            class AliveSession;

            ppbox::common::PortManager& portMgr_;

            framework::network::NetName addr_;

            ProxyManager * mgr_;

            Auth auth_;

            ::mutex::client::MutexClient client_;

            int distance_time_;

            clock_timer * timer_;

            ppbox::certify::AuthStatus::Enum auth_status_;

            std::string vod_playlink_key_;
            std::string live_playlink_key_;
        };

    } // namespace alive
} // namespace ppbox

#endif // _PPBOX_ALIVE_ALIVE_PROXY_H_
