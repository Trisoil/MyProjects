#include "tools/recorder/Common.h"
#include "tools/recorder/Version.h"

#include "tools/recorder/ChannelListManage.h"


//using namespace server_mod::live::live_recorder;

//#include <ppbox/common/Debuger.h>

#include <framework/process/Process.h>
#include <framework/process/SignalHandler.h>

#include <boost/bind.hpp>

FRAMEWORK_LOGGER_DECLARE_MODULE("LiveRecorder");



int main(int argc, char * argv[])
{
    util::daemon::Daemon my_daemon("live_recorder.conf");
    char const * default_argv[] = {
        "++Logger.stream_count=1", 
        "++Logger.ResolverService=1", 
        "++LogStream0.file=$LOG/live_recorder.log", 
        "++LogStream0.append=true", 
        "++LogStream0.level=5", 
    };
    my_daemon.parse_cmdline(sizeof(default_argv) / sizeof(default_argv[0]), default_argv);
    my_daemon.parse_cmdline(argc, (char const **)argv);

    framework::process::SignalHandler sig_handler(
        framework::process::Signal::sig_int, 
        boost::bind(&util::daemon::Daemon::stop, &my_daemon), true);

    framework::logger::global_logger().load_config(my_daemon.config());

    util::daemon::use_module<server_mod::live::live_recorder::ChannelListManage>(my_daemon);
    
    my_daemon.start(framework::this_process::notify_wait);

    return 0;
}
