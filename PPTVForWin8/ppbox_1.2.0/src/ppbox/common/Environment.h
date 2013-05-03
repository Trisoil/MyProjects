// Environment.h

#ifndef _PPBOX_COMMON_ENVIRONMENT_H_
#define _PPBOX_COMMON_ENVIRONMENT_H_

#include <framework/system/Version.h>

namespace ppbox
{
    namespace common
    {

        struct Environment
        {
            Environment();

            std::string get_bid() const;

            framework::system::Version version;

            std::string hid;
            std::string aid;
            std::string mid;

        };

    } // namespace common
} // namespace ppbox

#endif // _PPBOX_COMMON_ENVIRONMENT_H_
