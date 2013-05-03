// Main.cpp

#include "ppbox/alive/Common.h"
#include "ppbox/alive/Version.h"
#include "ppbox/alive/AliveProxy.h"

using namespace ppbox::alive;

//#include <ppbox/common/ConfigMgr.h>
#include <ppbox/common/Debuger.h>
#include <ppbox/common/PortManager.h>

#include <framework/process/Process.h>
#include <framework/process/SignalHandler.h>

#include <boost/bind.hpp>

int alive_main(int argc, char * argv[])
{
    util::daemon::Daemon my_daemon("ppbox_alive.conf");
    char const * default_argv[] = {
        "++framework::logger::Stream.0.file=$LOG/ppbox_alive.log", 
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
        util::daemon::use_module<ppbox::common::CommonModule>(my_daemon, "PpboxAlive");
    common.set_version(ppbox::alive::version());

    util::daemon::use_module<ppbox::common::PortManager>(my_daemon);
    util::daemon::use_module<ppbox::alive::AliveProxy>(my_daemon);
    //util::daemon::use_module<ppbox::common::ConfigMgr>(my_daemon);
    util::daemon::use_module<ppbox::common::Debuger>(my_daemon);

    my_daemon.start(framework::process::notify_wait);

    return 0;
}

#ifndef _LIB
int main(int argc, char * argv[])
{
	return alive_main(argc,argv);
}
#endif

