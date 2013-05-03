#ifndef _HOU_LOGGERS_H_
#define _HOU_LOGGERS_H_

#include <log4cplus/logger.h>

namespace hou
{
    using log4cplus::Logger;

    class Loggers
    {
    public:
        static Logger& HouService()
        {
            static Logger hou_service_logger = Logger::getInstance("houservice");
            return hou_service_logger;
        }

        static Logger& Operations()
        {
            static Logger operations_logger = Logger::getInstance("operations");
            return operations_logger;
        }

        static Logger& HttpService()
        {
            static Logger operations_logger = Logger::getInstance("httpservice");
            return operations_logger;
        }
    };
}

#endif //SUPER_NODE_LOGGERS_H