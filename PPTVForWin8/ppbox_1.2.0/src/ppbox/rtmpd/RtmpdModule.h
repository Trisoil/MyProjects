// RtmpdModule.h

#ifndef _PPBOX_RTMPD_RTMPD_MODULE_H_
#define _PPBOX_RTMPD_RTMPD_MODULE_H_

#include <framework/network/ServerManager.h>
#include <framework/string/Url.h>

namespace ppbox
{
    namespace dispatch
    {
        class DispatchModule;
    }

    namespace rtmpd
    {

        class RtmpSession;
        class RtmpDispatcher;

        class RtmpdModule 
            : public ppbox::common::CommonModuleBase<RtmpdModule>
            , public framework::network::ServerManager<RtmpSession, RtmpdModule>
        {
        public:
            RtmpdModule(
                util::daemon::Daemon & daemon);

            virtual ~RtmpdModule();

        public:
            virtual boost::system::error_code startup();

            virtual void shutdown();

        public:
            using ppbox::common::CommonModuleBase<RtmpdModule>::io_svc;

            RtmpDispatcher * alloc_dispatcher(
                framework::string::Url & url, 
                boost::system::error_code & ec);

            void free_dispatcher(
                RtmpDispatcher * dispatcher);

        private:
            framework::network::NetName addr_;
            ppbox::dispatch::DispatchModule & dispatch_module_;
        };

    } // namespace rtmpd
} // namespace ppbox

#endif // _PPBOX_RTMPD_RTMPD_MODULE_H_