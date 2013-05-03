// protocol.h

#ifndef _MUTEX_PROTOCOL_PROTOCOL_H_
#define _MUTEX_PROTOCOL_PROTOCOL_H_

#include <util/archive/LittleEndianBinaryIArchive.h>
#include <util/archive/LittleEndianBinaryOArchive.h>

#include <boost/cstdint.hpp>

//#include <string>

namespace mutex
{

    namespace protocol
    {
        typedef util::archive::LittleEndianBinaryIArchive<char> IUdpArchive;
        typedef util::archive::LittleEndianBinaryOArchive<char> OUdpArchive;

        struct Head
        {
            boost::uint16_t     checksum;
            boost::uint16_t     sequece;
            boost::uint8_t      version;
            boost::uint8_t      action;
            boost::uint16_t     reverse;

            template <typename Archive>
            void serialize(Archive & ar)
            {
                ar & checksum & sequece & version & action & reverse;
            }
        };

        template<
            boost::uint8_t n
        >
        struct CommPackage
        {
            BOOST_STATIC_CONSTANT(boost::uint8_t, action = n);
        };

        struct KeepaliveRequestPackage
            : CommPackage<0x52>
        {
            template <typename Archive>
            void serialize(Archive & ar)
            {
                ar & framework::container::make_array(session_id);
                ar & time;
                ar & framework::container::make_array(enc);
                ar & util::serialization::make_sized<boost::uint32_t>(client_id);
            }

            boost::uint8_t      session_id[16];
            boost::uint64_t     time;
            boost::uint8_t      enc[16];
            std::string         client_id;
        };

        struct LeavePackage
            : CommPackage<0x53>
        {
            template <typename Archive>
            void serialize(Archive & ar)
            {
                ar & framework::container::make_array(session_id);
                ar & util::serialization::make_sized<boost::uint32_t>(client_id);
            }

            boost::uint8_t      session_id[16];
            std::string         client_id;
        };

        struct KickoutPackage
            : CommPackage<0x75>
        {
            template <typename Archive>
            void serialize(Archive & ar)
            {
                ar & framework::container::make_array(session_id);
                ar & util::serialization::make_sized<boost::uint32_t>(client_id);
            }

            boost::uint8_t      session_id[16];
            std::string         client_id;
        };

        struct KeepaliveResponePackage
            : CommPackage<0x72>
        {
            template <typename Archive>
            void serialize(Archive & ar)
            {
                ar & passstate;
                ar & interval;
                ar & reserve;
            }

            boost::uint8_t      passstate;
            boost::uint16_t     interval;
            boost::uint8_t      reserve;
        };
    }
}

#endif // _MUTEX_PROTOCOL_PROTOCOL_H_
