//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "SubPieceResponseTask.h"
#include "UdpServerManager.h"

namespace super_node
{
    SubPieceResponseTask::SubPieceResponseTask(
        const boost::asio::ip::udp::endpoint& endpoint,
        boost::shared_ptr<BlockData> block_data,
        boost::uint16_t subpiece_index,
        boost::uint32_t transaction_id,
        const RID& resource_identifier,
        const framework::string::Uuid& server_guid,
        const protocol::SubPieceInfo& subpiece_info, 
        boost::uint16_t dest_protocol_version)
        : ResponseTask(endpoint, transaction_id, resource_identifier, server_guid, dest_protocol_version),
        subpiece_info_(subpiece_info)
    {
        subpiece_index_ = subpiece_index;
        block_data_ = block_data;
        assert(subpiece_index_ < block_data_->GetSubPieceNumber());
    }

    void SubPieceResponseTask::Execute(boost::shared_ptr<UdpServerManager> udp_server_manager)
    {
        boost::uint16_t subpiece_length = SUB_PIECE_SIZE;
        if (subpiece_index_ + 1 == block_data_->GetSubPieceNumber() && 
            block_data_->Size() % SUB_PIECE_SIZE)
        {
            subpiece_length = block_data_->Size() % SUB_PIECE_SIZE;
        }

        protocol::SubPieceBuffer tmp_buf(new protocol::SubPieceContent, subpiece_length);
        block_data_->GetSubPiece(subpiece_index_, tmp_buf);
        protocol::SubPiecePacket subpiece_packet(transaction_id_, resource_identifier_, server_guid_,
            subpiece_info_, static_cast<boost::uint16_t>(tmp_buf.Length()), tmp_buf, GetTargetEndpoint());

        udp_server_manager->SendPacket(subpiece_packet, dest_protocol_version_);
    }
}
