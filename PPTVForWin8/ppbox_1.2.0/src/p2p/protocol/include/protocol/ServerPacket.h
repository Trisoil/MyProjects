//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

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
                ar & source_;
                ar & reserve_;
            } else {
                ar & error_code_;
            }
        }

        ServerPacket()
        {
            IsRequest = 1;
            peer_version_ = PEER_VERSION;
            source_ = 0;
            reserve_ = 0;
        }

        virtual boost::uint32_t length() const
        {
            boost::uint32_t length = Packet::length();
            length += sizeof(IsRequest);
            if(IsRequest)
            {
                length += sizeof(peer_version_);
                length += sizeof(source_);
                length += sizeof(reserve_);
            }
            else
            {
                length += sizeof(error_code_);
            }
            return length;
        }

        boost::uint8_t IsRequest;
        
        boost::uint8_t source_;
        boost::uint8_t reserve_;
        boost::uint16_t peer_version_;
        boost::uint8_t error_code_;
    };

    template <boost::uint8_t action>
    struct ServerPacketT
        : PacketT<action>
        , ServerPacket
    {
    };

}

#endif  // _PROTOCOL_SERVER_PACKET_H_
