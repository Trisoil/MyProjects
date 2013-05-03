// ServerPacket.h

#ifndef _PROTOCOL_SERVER_PACKET_H_
#define _PROTOCOL_SERVER_PACKET_H_

#include "protocol/Packet.h"

namespace protocol
{

    struct ServerPacket
        : Packet
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            Packet::serialize(ar);
            ar & IsRequest;
            if (IsRequest) {
                ar & peer_version_;
            } else {
                ar & error_code_;
            }
        }

        ServerPacket()
        {
            IsRequest = 1;
            peer_version_ = PEER_VERSION;
        }

        boost::uint8_t IsRequest;        
        union {
            boost::uint32_t peer_version_;
            boost::uint8_t error_code_;
        };
    };

    template <boost::uint8_t action>
    struct ServerPacketT
        : PacketT<action>
        , ServerPacket
    {
    };

}

#endif // _PROTOCOL_SERVER_PACKET_H_
