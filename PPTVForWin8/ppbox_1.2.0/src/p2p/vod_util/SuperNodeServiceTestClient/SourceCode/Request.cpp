//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "Request.h"

namespace super_node_test
{
    Request::Request(int peer_id)
        : peer_id_(peer_id)
    {
    }

    GetSubpiecesRequest::GetSubpiecesRequest(int peer_id, const RID& rid, const string& resource_name, const std::vector<SubPieceInfo>& subpieces)
        : Request(peer_id), rid_(rid), resource_name_(resource_name), subpieces_(subpieces)
    {
    }

    template <typename PacketType>
    void BuildPacketBufferHelper(PacketType const & packet, UdpBuffer& udp_buffer)
    {
        typedef util::archive::LittleEndianBinaryOArchive<boost::uint8_t> OUdpArchive;

        assert(boost::asio::ip::udp::endpoint() != packet.end_point);
        udp_buffer.end_point(packet.end_point);
        udp_buffer.commit(sizeof(uint32_t));
        OUdpArchive oa(udp_buffer);
        boost::uint8_t action = PacketType::Action;
        oa << action << packet;
        if (oa)
        {
            boost::uint32_t & chk_sum = const_cast<boost::uint32_t &>(*boost::asio::buffer_cast<boost::uint32_t const *>(*udp_buffer.data().begin()));
            chk_sum = check_sum_new(util::buffers::sub_buffers(udp_buffer.data(), 4));
        }
    }
    
    void GetSubpiecesRequest::BuildPacketBuffer(UdpBuffer& udp_buffer, const boost::asio::ip::udp::endpoint& service_endpoint, boost::uint32_t transaction_id) const
    {
        protocol::RequestSubPiecePacketFromSN request_subpiece(transaction_id, rid_, resource_name_, subpieces_, service_endpoint, 20);
        BuildPacketBufferHelper(request_subpiece, udp_buffer);
    }

    CloseSessionRequest::CloseSessionRequest(int peer_id)
        : Request(peer_id)
    {
    }

    void CloseSessionRequest::BuildPacketBuffer(UdpBuffer& udp_buffer, const boost::asio::ip::udp::endpoint& service_endpoint, boost::uint32_t transaction_id) const
    {
        protocol::CloseSessionPacket close_session(transaction_id, protocol::PEER_VERSION, service_endpoint);
        BuildPacketBufferHelper(close_session, udp_buffer);
    }
}