//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_LOGGERS_H
#define SUPER_NODE_LOGGERS_H

#include <log4cplus/logger.h>

namespace super_node
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
            static Logger service_logger = Logger::getInstance("supernode_service");
            return service_logger;
        }

        static Logger& LocalDiskCache()
        {
            static Logger local_disk_cache_logger = Logger::getInstance("local_disk_cache");
            return local_disk_cache_logger;
        }

        static Logger& LocalDisk()
        {
            static Logger local_disk_logger = Logger::getInstance("local_disk");
            return local_disk_logger;
        }

        static Logger& MemoryCache()
        {
            static Logger memory_cache_logger = Logger::getInstance("memory_cache");
            return memory_cache_logger;
        }

        static Logger& UdpServer()
        {
            static Logger udp_server_logger = Logger::getInstance("udp_server");
            return udp_server_logger;
        }

        static Logger& RemoteCache()
        {
            static Logger remote_cache_logger = Logger::getInstance("remote_cache");
            return remote_cache_logger;
        }

        static Logger& ConfigManager()
        {
            static Logger config_manager_logger = Logger::getInstance("config_manager");
            return config_manager_logger;
        }
    };
}

#endif //SUPER_NODE_LOGGERS_H