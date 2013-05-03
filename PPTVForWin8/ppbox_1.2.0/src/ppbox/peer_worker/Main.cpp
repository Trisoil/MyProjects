// Main.cpp

#include "ppbox/peer_worker/Common.h"
#include "ppbox/peer_worker/Version.h"
#include "ppbox/peer_worker/WorkerModule.h"
#include "ppbox/peer_worker/StatusProxy.h"
#include "ppbox/peer_worker/Interface.h"

//#include <ppbox/common/ConfigMgr.h>
#include <ppbox/common/Debuger.h>
#include <ppbox/common/PortManager.h>

#include <framework/logger/Logger.h>
#include <framework/process/Process.h>
#include <framework/process/SignalHandler.h>

#include <boost/bind.hpp>

#ifdef BOOST_POSIX_API
#include <unistd.h>
#include <signal.h>
#endif

namespace ppbox
{
    namespace peer_worker
    {

        int vod_main(int argc, char * argv[])
        {
            util::daemon::Daemon my_daemon("peer_worker.conf");
            char const * default_argv[] = {
                "++framework::logger::Stream.0.file=$LOG/peer_worker.log", 
                "++framework::logger::Stream.0.append=true", 
                "++framework::logger::Stream.0.roll=true", 
                "++framework::logger::Stream.0.level=5", 
                "++framework::logger::Stream.0.size=102400", 
            };
            my_daemon.parse_cmdline(sizeof(default_argv) / sizeof(default_argv[0]), default_argv);
            my_daemon.parse_cmdline(argc, (char const **)argv);

            framework::process::SignalHandler sig_handler(
                framework::process::Signal::sig_int, 
                boost::bind(&util::daemon::Daemon::post_stop, &my_daemon), true);

            framework::logger::load_config(my_daemon.config());

            ppbox::common::log_versions();

            ppbox::common::CommonModule & common = 
                util::daemon::use_module<ppbox::common::CommonModule>(my_daemon, "VodWorker");
            common.set_version(ppbox::peer_worker::version());

            //util::daemon::use_module<ppbox::common::ConfigMgr>(my_daemon);
            util::daemon::use_module<ppbox::common::Debuger>(my_daemon);
            util::daemon::use_module<ppbox::common::PortManager>(my_daemon);
            util::daemon::use_module<ppbox::peer_worker::WorkerModule>(my_daemon);
            util::daemon::use_module<ppbox::peer_worker::StatusProxy>(my_daemon);

            my_daemon.start(framework::process::notify_wait);

            return 0;
        }
    }
}
#ifndef _LIB
int main(int argc, char * argv[])
{
    return ppbox::peer_worker::vod_main(argc,argv);
}
#endif
