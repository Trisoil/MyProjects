// Version.cpp

#include "ppbox/common/Common.h"
#define VERSION_SOURCE
#include "ppbox/common/Version.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#include <boost/preprocessor/stringize.hpp>

#include <iostream>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.common.Version", framework::logger::Debug)

#undef linux

namespace ppbox
{
    namespace common
    {

        void log_versions()
        {
            char const * TITLE = BOOST_PP_STRINGIZE(PLATFORM_NAME) "-" BOOST_PP_STRINGIZE(TOOL_NAME) "-" BOOST_PP_STRINGIZE(STRATEGY_NAME);
            std::cout << TITLE << std::endl;
            LOG_ERROR( TITLE);
            for (std::map<char const *, char const *>::const_iterator iter = 
                framework::system::version_collection().begin(); iter != 
                framework::system::version_collection().end(); ++iter) {
                    std::cout << iter->first << " " << iter->second << std::endl;
                    LOG_ERROR( iter->first << " " << iter->second);
            }
        }

    } // namespace common
} // namespace ppbox
