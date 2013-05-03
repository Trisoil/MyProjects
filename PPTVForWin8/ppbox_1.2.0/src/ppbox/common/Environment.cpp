// Environment.cpp

#include "ppbox/common/Common.h"
#include "ppbox/common/Environment.h"

#include <framework/network/Interface.h>
using namespace framework::network;
using namespace framework::system;

namespace ppbox
{
    namespace common
    {

        Environment::Environment()
        {
            version = Version(0, 5, 2);

            std::string mac;
            std::vector<Interface> infs;
            if (!enum_interface(infs)) {
                for (size_t i = 0; i < infs.size(); ++i) {
                    if (std::string(infs[i].name) == "lo"
                        || infs[i].hard_addr_string(':', false) == "00:00:00:00:00:00")
                        continue;
                    if (::memcmp(infs[i].hwaddr, "\0\0\0\0\0\0", 6) == 0)
                        continue;
                    if (mac.length() != 0)
                        mac += "|";
                    mac += infs[i].hard_addr_string(':', false);
                    if (mac.size() >= 71) // 最多4个
                        break;
                }
            }
            if (mac.empty())
                mac = "aa:bb:cc:dd:ee:gg";
            mid = mac;
        }

        std::string Environment::get_bid() const
        {
            return "####" + mid + "##";
        }

    } // namespace common
} // namespace ppbox
