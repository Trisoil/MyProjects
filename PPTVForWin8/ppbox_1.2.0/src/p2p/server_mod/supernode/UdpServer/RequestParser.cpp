//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "protocol/Protocol.h"
#include "RequestParser.h"

namespace super_node
{
    const size_t RequestParser::PeerBlockSize = 2 * 1024 * 1024;

    void RequestParser::ParseFromPeerToSN(const protocol::RequestSubPiecePacketFromSN & packet,
        std::map<boost::uint16_t, std::set<boost::uint16_t> > & request_in_sn_type)
    {
        for (boost::uint16_t i = 0; i < packet.subpiece_infos_.size(); ++i)
        {
            size_t offset_in_resource = packet.subpiece_infos_[i].block_index_ * PeerBlockSize
                + packet.subpiece_infos_[i].subpiece_index_ * SUB_PIECE_SIZE;

            boost::uint16_t block_index = offset_in_resource / BlockData::MaxBlockSize;
            boost::uint16_t subpiece_index = static_cast<boost::uint16_t>((offset_in_resource % BlockData::MaxBlockSize) / SUB_PIECE_SIZE);

            if (request_in_sn_type.find(block_index) == request_in_sn_type.end())
            {
                std::set<boost::uint16_t> subpiece_indexs;
                subpiece_indexs.insert(subpiece_index);
                request_in_sn_type.insert(std::make_pair(block_index, subpiece_indexs));
            }
            else
            {
                if (request_in_sn_type[block_index].find(subpiece_index) == request_in_sn_type[block_index].end())
                {
                    request_in_sn_type[block_index].insert(subpiece_index);
                }
            }
        }
    }

    protocol::SubPieceInfo RequestParser::ParseFromSNToPeer(boost::uint16_t block_index, boost::uint16_t subpiece_index)
    {
        size_t offset_in_resource = block_index * BlockData::MaxBlockSize + subpiece_index * SUB_PIECE_SIZE;
        boost::uint16_t peer_block_index = offset_in_resource / PeerBlockSize;
        boost::uint16_t peer_subpiece_index = static_cast<boost::uint16_t>((offset_in_resource % PeerBlockSize) / SUB_PIECE_SIZE);
        return protocol::SubPieceInfo(peer_block_index, peer_subpiece_index);
    }
}
