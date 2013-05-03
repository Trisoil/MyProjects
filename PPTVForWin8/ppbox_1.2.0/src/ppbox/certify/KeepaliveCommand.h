// KeepaliveCommand.h

#ifndef _PPBOX_CERTIFY_KEEPALIVE_COMMAND_H_
#define _PPBOX_CERTIFY_KEEPALIVE_COMMAND_H_

#include "ppbox/certify/AuthStatus.h"

#include <util/serialization/NVPair.h>
#include <util/serialization/ErrorCode.h>

#include <util/archive/TextIArchive.h>
#include <util/archive/TextOArchive.h>

#define TIMEOUT     30

namespace ppbox
{
    namespace certify
    {

        typedef util::archive::TextIArchive<char> IHttpArchive;
        typedef util::archive::TextOArchive<char> OHttpArchive;

        struct KeepaliveRequest
        {
            KeepaliveRequest()
                : distance_time(0)
            {
            }

            template <
                typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & gid & pid & auth & distance_time;
            }

            std::string gid;
            std::string pid;
            std::string auth;

            int distance_time;
        };

        struct KeepaliveResponse
        {
            KeepaliveResponse()
                : status(AuthStatus::stopped)
            {
            }

            template <
                typename Archive>
            void serialize(
                Archive & ar)
            {
                ar & vod_playlink_key & live_playlink_key & error_value & status;
            }

            std::string vod_playlink_key;
            std::string live_playlink_key;

            boost::system::error_code error_value;
            AuthStatus::Enum status;
        };

    } // namespace certify
} // namespace ppbox

#endif // _PPBOX_CERTIFY_KEEPALIVE_COMMAND_H_

