//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_REQUEST_PARSER_H
#define SUPER_NODE_REQUEST_PARSER_H

#include "protocol/Protocol.h"

namespace super_node
{
    class RequestParser
    {
    public:
        static void ParseFromPeerToSN(const protocol::RequestSubPiecePacketFromSN & packet,
            std::map<boost::uint16_t, std::set<boost::uint16_t> > & request_in_sn_type);
        static protocol::SubPieceInfo ParseFromSNToPeer(boost::uint16_t peer_block_index, boost::uint16_t peer_subpiece_index);

    private:
        static const size_t PeerBlockSize;
    };
}

#endif  // SUPER_NODE_REQUEST_PARSER_H