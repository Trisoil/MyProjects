// StatusProxy.h

#ifndef _PPBOX_PEER_WORKER_STATUS_PROXY_H_
#define _PPBOX_PEER_WORKER_STATUS_PROXY_H_

#include <framework/network/NetName.h>

namespace ppbox
{
    namespace peer_worker
    {

        class StatusProxy
            : public util::daemon::ModuleBase<StatusProxy>
        {
        public:
            StatusProxy(
                util::daemon::Daemon & daemon);

            ~StatusProxy();

        private:
            virtual boost::system::error_code startup();

            virtual void shutdown();

        private:
            class ProxyManager;
            class StatusSession;
            framework::network::NetName addr_;
            ProxyManager * mgr_;
        };

    } // namespace peer_worker
} // namespace ppbox

#endif // _PPBOX_peer_worker_STATUS_PROXY_H_
