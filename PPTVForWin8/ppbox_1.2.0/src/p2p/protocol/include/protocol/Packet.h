//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

// Packet.h

#ifndef _PROTOCOL_PACKET_H_
#define _PROTOCOL_PACKET_H_

#include "struct/Structs.h"
#include "struct/Base.h"

namespace protocol
{
    const boost::uint16_t PEER_VERSION_V3 = 0x0103;
    const boost::uint16_t PEER_VERSION_V4 = 0x0104;
    const boost::uint16_t PEER_VERSION_V5 = 0x0105;
    const boost::uint16_t PEER_VERSION_V6 = 0x0106;
    const boost::uint16_t PEER_VERSION_V7 = 0x0107;
    const boost::uint16_t PEER_VERSION_V8 = 0x0108;
    const boost::uint16_t PEER_VERSION_V9 = 0x0109;
    const boost::uint16_t PEER_VERSION_V10 = 0x010a;
    const boost::uint16_t PEER_VERSION_V11 = 0x010b;
    const boost::uint16_t PEER_VERSION_V12 = 0x010c;
    const boost::uint16_t PEER_VERSION = PEER_VERSION_V12;

    struct Packet
    {
        static boost::uint32_t NewTransactionID()
        {
            static boost::uint32_t glocal_transaction_id = 0;
            glocal_transaction_id ++;
            return glocal_transaction_id;
        }

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & transaction_id_;
        }

        virtual boost::uint32_t length() const
        {
            //sizeof(boost::uint8_t)表示action�?表示校验�?
            return sizeof(transaction_id_) + sizeof(boost::uint8_t) + 4;
        }

        virtual ~Packet(){}

        boost::asio::ip::udp::endpoint end_point;
        boost::uint8_t PacketAction;
        boost::uint32_t transaction_id_;
    };

    template <boost::uint8_t action>
    struct PacketT
    {
        virtual ~PacketT(){}
        static const boost::uint8_t Action = action;
    };

}

#endif  // _PROTOCOL_PACKET_H_
