//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "pre.h"
#include "channel_server.h"

int main (int argc, const char * argv[]) 
{
    log4cplus::PropertyConfigurator::doConfigure("livemedia-logging.conf");
    boost::shared_ptr<live_media::ChannelServer> server(new live_media::ChannelServer());
    server->Run();

    return 0;
}
