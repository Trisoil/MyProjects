//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _REGISTER_SUPER_NODE_PACKET_
#define _REGISTER_SUPER_NODE_PACKET_

#include "protocol/Protocol.h"

using protocol::ErrorPacket;
using protocol::SubPiecePacket;
using protocol::RequestSubPiecePacketFromSN;
using protocol::CloseSessionPacket;


namespace super_node
{
    template <typename PacketHandler>
    void RegisterSuperNodePacket(
        PacketHandler & handler)
    {
        handler.template register_packet<SubPiecePacket>();
        handler.template register_packet<ErrorPacket>();
        handler.template register_packet<RequestSubPiecePacketFromSN>();
        handler.template register_packet<CloseSessionPacket>();
    }
}
#endif