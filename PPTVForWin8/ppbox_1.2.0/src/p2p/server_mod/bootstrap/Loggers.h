//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef BOOTSTRAP_LOGGERS_H
#define BOOTSTRAP_LOGGERS_H

#include <log4cplus/logger.h>

namespace bootstrap
{
    using log4cplus::Logger;

    class Loggers
    {
    public:
        static Logger& Operations()
        {
            static Logger operations_logger = log4cplus::Logger::getInstance("operations");
            return operations_logger;
        }

        static Logger& BootStrap()
        {
            static Logger bootstrap_logger = log4cplus::Logger::getInstance("bootstrap");
            return bootstrap_logger;
        }

        static Logger& MisSync()
        {
            static Logger mis_sync_logger = log4cplus::Logger::getInstance("mis_sync");
            return mis_sync_logger;
        }

        static Logger& IpLib()
        {
            static Logger iplib_logger = log4cplus::Logger::getInstance("iplib");
            return iplib_logger;
        }

        static Logger& ServerPacker()
        {
            static Logger server_packer_logger = log4cplus::Logger::getInstance("server_packer");
            return server_packer_logger;
        }
    };
}

#endif  // BOOTSTRAP_LOGGERS_H
