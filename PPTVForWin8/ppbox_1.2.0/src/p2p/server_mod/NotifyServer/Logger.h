#pragma once

#include <log4cplus/logger.h>

using log4cplus::Logger;

class Loggers
{
public:
    static Logger& NotifyHttpSvrConn()
    {
        static Logger http_server_connection_logger = log4cplus::Logger::getInstance("NotifyHttpSvrConn");
        return http_server_connection_logger;
    }

    static Logger& NotifySvrModule()
    {
        static Logger notify_server_module_logger = log4cplus::Logger::getInstance("NotifySvrModule");
        return notify_server_module_logger;
    }
};

