//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "pre.h"
#include "ResponseTask.h"
#include "channel_server.h"

namespace live_media
{
    ConnectResponseTask::ConnectResponseTask(boost::shared_ptr<protocol::ConnectPacket> connect_request)
        : ResponseTask(connect_request->end_point)
    {
        connect_request_ = connect_request;
    }

    void ConnectResponseTask::Execute(ChannelServer* server)
    {
        if (connect_request_->basic_info_!= 0) 
        {
            return;
        }

        protocol::ConnectPacket response;
        response.end_point = connect_request_->end_point;
        response.transaction_id_ = connect_request_->transaction_id_;
        response.resource_id_ = connect_request_->resource_id_;
        response.peer_version_ = protocol::PEER_VERSION;

        response.peer_guid_ = server->GetMyId();
        response.basic_info_ = 1;
        response.send_off_time_ = connect_request_->send_off_time_;
        response.connect_type_ = protocol::CONNECT_LIVE_UDPSERVER;
        response.ip_pool_size_ = connect_request_->ip_pool_size_;

        response.peer_info_ = server->GetMyselfInfo().my_info_;
        response.peer_download_info_ = server->GetMyselfInfo().my_download_info_;

        server->send_packet(response, protocol::ConnectPacket::Action);
    }

    SubPieceResponseTask::SubPieceResponseTask(boost::shared_ptr<protocol::LiveRequestSubPiecePacket> subpiece_request, size_t block_id, const std::vector<uint16_t>& subpieces, boost::shared_ptr<BlockData> block_data)
        : ResponseTask(subpiece_request->end_point), subpieces_(subpieces)
    {
        subpiece_request_ = subpiece_request;
        block_id_ = block_id;
        block_data_ = block_data;
    }

    void SubPieceResponseTask::Execute(ChannelServer* server)
    {
        for(size_t i = 0; i < subpieces_.size(); ++i)
        {
            SendSubPiece(server, subpieces_[i]);
        }
    }

    void SubPieceResponseTask::SendSubPiece(ChannelServer* server, boost::uint16_t subpiece_index)
    {
        if (subpiece_index >= block_data_->GetSubPiecesCount())
        {
            assert(false);
            return;
        }

        protocol::LiveSubPiecePacket response;

        response.resource_id_ = subpiece_request_->resource_id_;
        response.protocol_version_ = protocol::PEER_VERSION;
        response.reserve_ = 0;
        response.end_point = subpiece_request_->end_point;
        response.transaction_id_ = subpiece_request_->transaction_id_;
        response.sub_piece_info_ = protocol::LiveSubPieceInfo(block_id_, subpiece_index);

        response.sub_piece_length_ = block_data_->GetSubPieceSize(subpiece_index);
        boost::uint8_t * buf = response.sub_piece_content_->get_buffer();
        memcpy(buf, block_data_->GetSubPieceContent(subpiece_index), response.sub_piece_length_);

        server->send_packet(response, protocol::LiveSubPiecePacket::Action);
    }

    AnnounceResponseTask::AnnounceResponseTask(
        boost::shared_ptr<protocol::LiveRequestAnnouncePacket> announce_request, 
        const std::map<size_t, boost::shared_ptr<BlockMetaData> >& blocks_bitmap, 
        size_t channel_step_size)
        : ResponseTask(announce_request->end_point)
    {
        announce_request_ = announce_request;
        blocks_bitmap_ = blocks_bitmap;
        channel_step_size_ = channel_step_size;
    }

    void AnnounceResponseTask::BuildAnnounceMap(protocol::LiveAnnounceMap& announce_map)
    {
        announce_map.block_info_count_ = 0;
        announce_map.subpiece_nos_.clear();
        announce_map.subpiece_map_.clear();

        if(!blocks_bitmap_.size()) 
        {
            return;
        }

        size_t total_bits = 0;
        for (std::map<size_t, boost::shared_ptr<BlockMetaData> >::iterator iter = blocks_bitmap_.begin(); 
            iter != blocks_bitmap_.end();
            ++iter) 
        {
            boost::shared_ptr<BlockMetaData> block_meta_data = iter->second;

            size_t sub_piece_count = block_meta_data->GetSubPiecesCount();
            size_t piece_total_bits = block_meta_data->GetPiecesCount();
            total_bits += piece_total_bits;

            // 在这里检测一下是否超过了最大的长度
            if(total_bits > int(BlockData::LiveSubPieceSizeInBytes * 0.3 * 8)) 
            {
                break;
            }

            ++announce_map.block_info_count_;
            announce_map.subpiece_nos_.insert(std::make_pair(block_meta_data->GetBlockId(), sub_piece_count));

            boost::dynamic_bitset<boost::uint8_t> bitset;

            for(size_t j = 0; j < piece_total_bits; j++) 
            {
                bitset.push_back(true);
            }

            announce_map.subpiece_map_.insert(std::make_pair(block_meta_data->GetBlockId(), bitset));
        }

        assert(announce_map.subpiece_map_.size() == announce_map.subpiece_nos_.size());
    }

    void AnnounceResponseTask::Execute(ChannelServer* server)
    {
        protocol::LiveAnnouncePacket response;
        response.end_point = announce_request_->end_point;
        response.transaction_id_ = announce_request_->transaction_id_;
        response.live_announce_map_.request_block_id_ = announce_request_->request_block_id_;
        response.resource_id_ = announce_request_->resource_id_;
        response.protocol_version_ = protocol::PEER_VERSION;
        response.reserve_ = 0;

        response.live_announce_map_.live_interval_ = channel_step_size_;

        BuildAnnounceMap(response.live_announce_map_);

        server->send_packet(response, protocol::LiveAnnouncePacket::Action);
    }
}