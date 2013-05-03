//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "Request.h"
#include <framework/timer/TickCounter.h>

namespace live_media_test
{
    Request::Request(int peer_id)
        : peer_id_(peer_id)
    {
    }

    GetSubpiecesRequest::GetSubpiecesRequest(int peer_id, const RID& channel_id, const std::vector<LiveSubPieceInfo>& subpieces)
        : Request(peer_id), channel_id_(channel_id), subpieces_(subpieces)
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
        protocol::LiveRequestSubPiecePacket live_request_subpiece(transaction_id, channel_id_, subpieces_, 20, service_endpoint);
        BuildPacketBufferHelper(live_request_subpiece, udp_buffer);
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

    ConnectRequest::ConnectRequest(int peer_id, const Guid& peer_guid, const RID& channel_id)
        : Request(peer_id), channel_id_(channel_id), peer_guid_(peer_guid)
    {
    }

    void ConnectRequest::BuildPacketBuffer(UdpBuffer& udp_buffer, const boost::asio::ip::udp::endpoint& service_endpoint, boost::uint32_t transaction_id) const
    {
        protocol::CandidatePeerInfo my_peer_info;
        protocol::PEER_DOWNLOAD_INFO my_download_info;

        protocol::ConnectPacket connect_packet(transaction_id, channel_id_, peer_guid_, protocol::PEER_VERSION,  
            0x00, framework::timer::TickCounter::tick_count(), protocol::PEER_VERSION,
            my_peer_info,
            protocol::CONNECT_LIVE_PEER,
            my_download_info,
            service_endpoint,
            500);

        BuildPacketBufferHelper(connect_packet, udp_buffer);
    }

    AnnouceRequest::AnnouceRequest(int peer_id, const RID& channel_id, size_t request_block_id)
        : Request(peer_id), channel_id_(channel_id), request_block_id_(request_block_id)
    {
    }

    void AnnouceRequest::BuildPacketBuffer(UdpBuffer& udp_buffer, const boost::asio::ip::udp::endpoint& service_endpoint, boost::uint32_t transaction_id) const
    {
        protocol::LiveRequestAnnouncePacket announce_request_packet(transaction_id, channel_id_, request_block_id_, 0, service_endpoint);
        BuildPacketBufferHelper(announce_request_packet, udp_buffer);
    }
}