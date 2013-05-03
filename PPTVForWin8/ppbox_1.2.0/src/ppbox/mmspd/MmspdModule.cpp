// MmspdModule.cpp

#include "ppbox/mmspd/Common.h"
#include "ppbox/mmspd/MmspdModule.h"
#include "ppbox/mmspd/MmspDispatcher.h"
#include "ppbox/mmspd/MmspSession.h"

#include <ppbox/dispatch/DispatchModule.h>

#include <framework/network/TcpSocket.hpp>

namespace ppbox
{
    namespace mmspd
    {

        MmspdModule::MmspdModule(
            util::daemon::Daemon & daemon)
            : ppbox::common::CommonModuleBase<MmspdModule>(daemon, "MmspdModule")
            , framework::network::ServerManager<MmspSession, MmspdModule>(daemon.io_svc())
            , addr_("0.0.0.0:1755")
            , dispatch_module_(util::daemon::use_module<ppbox::dispatch::DispatchModule>(get_daemon()))
        {
            daemon.config().register_module("MmspdModule")
                << CONFIG_PARAM_NAME_RDWR("addr",addr_ );
        }

        MmspdModule::~MmspdModule()
        {
        }

        boost::system::error_code MmspdModule::startup()
        {
            boost::system::error_code ec;
            start(addr_,ec);
            return ec;
        }

        void MmspdModule::shutdown()
        {
            stop();
        }

        MmspDispatcher * MmspdModule::alloc_dispatcher(
            framework::string::Url & url, 
            boost::system::error_code & ec)
        {
            dispatch_module_.normalize_url(url, ec);
            return new MmspDispatcher(*dispatch_module_.alloc_dispatcher(true));
        }

        void MmspdModule::free_dispatcher(
            MmspDispatcher * dispatcher)
        {
            dispatch_module_.free_dispatcher(&dispatcher->get_dispatcher());
            delete dispatcher;
        }

    } // namespace mmspd
} // namespace ppbox
