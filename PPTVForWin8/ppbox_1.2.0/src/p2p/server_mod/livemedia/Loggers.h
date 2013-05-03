//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef LIVE_MEDIA_LOGGERS_H
#define LIVE_MEDIA_LOGGERS_H

#include <log4cplus/logger.h>

namespace 
{
    using log4cplus::Logger;

    class Loggers
    {
    public:
        static Logger& Operations()
        {
            static Logger operations_logger = Logger::getInstance("operations");
            return operations_logger;
        }

        static Logger& Service()
        {
            static Logger service_logger = Logger::getInstance("livemedia");
            return service_logger;
        }

        static Logger& SessionManager()
        {
            static Logger service_logger = Logger::getInstance("session_manager");
            return service_logger;
        }

        static Logger& Channel()
        {
            static Logger channel_logger = Logger::getInstance("channel");
            return channel_logger;
        }

        static Logger& HttpStreamFetcher()
        {
            static Logger http_stream_fetcher = Logger::getInstance("http_stream_fetcher");
            return http_stream_fetcher;
        }

        static Logger& UdpServer()
        {
            static Logger udp_server_logger = Logger::getInstance("udp_server");
            return udp_server_logger;
        }

        static Logger& RemoteCache()
        {
            static Logger udp_server_logger = Logger::getInstance("RemoteCache");
            return udp_server_logger;
        }
    };
}

#endif //LIVE_MEDIA_LOGGERS_H