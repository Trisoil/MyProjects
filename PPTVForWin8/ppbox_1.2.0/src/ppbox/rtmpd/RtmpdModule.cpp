// RtmpdModule.cpp

#include "ppbox/rtmpd/Common.h"
#include "ppbox/rtmpd/RtmpdModule.h"
#include "ppbox/rtmpd/RtmpDispatcher.h"
#include "ppbox/rtmpd/RtmpSession.h"

#include <ppbox/dispatch/DispatchModule.h>

#include <util/protocol/rtmp/RtmpSocket.hpp>

namespace ppbox
{
    namespace rtmpd
    {

        RtmpdModule::RtmpdModule(
            util::daemon::Daemon & daemon)
            : ppbox::common::CommonModuleBase<RtmpdModule>(daemon, "RtmpdModule")
            , framework::network::ServerManager<RtmpSession, RtmpdModule>(daemon.io_svc())
            , addr_("0.0.0.0:1935")
            , dispatch_module_(util::daemon::use_module<ppbox::dispatch::DispatchModule>(get_daemon()))
        {
            daemon.config().register_module("RtmpdModule")
                << CONFIG_PARAM_NAME_RDWR("addr",addr_ );
        }

        RtmpdModule::~RtmpdModule()
        {
        }

        boost::system::error_code RtmpdModule::startup()
        {
            boost::system::error_code ec;
            start(addr_,ec);
            return ec;
        }

        void RtmpdModule::shutdown()
        {
            stop();
        }

        RtmpDispatcher * RtmpdModule::alloc_dispatcher(
            framework::string::Url & url, 
            boost::system::error_code & ec)
        {
            dispatch_module_.normalize_url(url, ec);
            return new RtmpDispatcher(*dispatch_module_.alloc_dispatcher(true));
        }

        void RtmpdModule::free_dispatcher(
            RtmpDispatcher * dispatcher)
        {
            dispatch_module_.free_dispatcher(&dispatcher->get_dispatcher());
            delete dispatcher;
        }

    } // namespace rtmpd
} // namespace ppbox
