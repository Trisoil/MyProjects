// Main.cpp

#include "tools/transfer/Common.h"
#include "tools/transfer/Version.h"
#include "tools/transfer/TransferModule.h"

#include <ppbox/demux/DemuxerModule.h>
#include <ppbox/mux/MuxerModule.h>
#include <framework/process/Process.h>
#include <framework/process/SignalHandler.h>

#include <boost/bind.hpp>

FRAMEWORK_LOGGER_DECLARE_MODULE("PpboxTransfer");

int main(int argc, char * argv[])
{
    util::daemon::Daemon my_daemon("ppbox_transfer.conf");
    char const * default_argv[] = {
        "++Logger.stream_count=1", 
        "++Logger.ResolverService=1", 
        "++LogStream0.file=$LOG/ppbox_transfer.log", 
        "++LogStream0.append=true", 
        "++LogStream0.level=5", 
    };
    my_daemon.parse_cmdline(sizeof(default_argv) / sizeof(default_argv[0]), default_argv);
    my_daemon.parse_cmdline(argc, (char const **)argv);

    framework::process::SignalHandler sig_handler(
        framework::process::Signal::sig_int, 
        boost::bind(&util::daemon::Daemon::post_stop, &my_daemon), true);

    framework::logger::global_logger().load_config(my_daemon.config());

    util::daemon::use_module<ppbox::demux::DemuxerModule>(my_daemon);
    util::daemon::use_module<ppbox::mux::MuxerModule>(my_daemon);

    tools::transfer::TransferModule & module = 
        util::daemon::use_module<tools::transfer::TransferModule>(my_daemon);
    
    my_daemon.start(framework::this_process::notify_wait);

    return module.error().value();
}
