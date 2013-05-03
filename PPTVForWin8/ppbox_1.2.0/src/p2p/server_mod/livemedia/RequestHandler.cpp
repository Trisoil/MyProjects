//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "pre.h"
#include "RequestHandler.h"
#include "SessionManagement/SessionManager.h"
#include "MemoryCache.h"
#include "ResponseHandler.h"
#include "channel_manager.h"
#include "channel_server.h"

namespace live_media
{
    RequestHandler::RequestHandler(boost::shared_ptr<ChannelManager> manager, boost::shared_ptr<SessionManager> session_manager)
    {
        manager_ = manager;
        session_manager_ = session_manager;
        server_ = manager->server_;
        memory_cache_.reset(new MemoryCache(manager));
        response_handler_.reset(new ResponseHandler(manager));

        memory_cache_->Start();
        response_handler_->Start();
    }

    void RequestHandler::Stop()
    {
        memory_cache_->Stop();
        response_handler_->Stop();
    }

    void RequestHandler::HandleCloseSession(const boost::asio::ip::udp::endpoint& peer)
    {
        ++(server_->receive_packet_statistics_->GetStatistics(RequestTypes::CloseSession));
        session_manager_->CloseSession(peer);
    }

    void RequestHandler::HandlePeerHeartBeat(const boost::asio::ip::udp::endpoint& peer, boost::uint32_t transaction_number, boost::uint8_t packet_action)
    {
        session_manager_->TryUpdateSession(peer, transaction_number, packet_action);
    }

    void RequestHandler::HandleConnectRequest(boost::shared_ptr<protocol::ConnectPacket> connect_request)
    {
        ++(server_->receive_packet_statistics_->GetStatistics(RequestTypes::Connect));

        ChannelPointer target_channel = manager_->GetChannel(connect_request->resource_id_);

        if (!target_channel && server_->GetServerConfig().open_channel_on_demand)
        {
            const server_config& s_cfg = server_->GetServerConfig();

            ChannelConfiguration conf;

            conf.guid = connect_request->resource_id_;
            conf.channel_fetch_base_url = std::string("http://") + s_cfg.fetch_domain + ":80/live/" + conf.guid.to_string() + "/";

            conf.max_upload_speed = s_cfg.max_upload_speed_per_channel;
            conf.max_keep_connections = s_cfg.max_keep_connections_per_channel;
            conf.max_upload_in_a_while = s_cfg.max_keep_connections_per_channel;

            manager_->AddChannel(conf);
        }

        SessionManager::Type result = session_manager_->TryAddSession(connect_request->end_point, connect_request->resource_id_, connect_request->transaction_id_);
        if (result == SessionManager::MaxSessionNumberReached)
        {
            return;
        }

        response_handler_->SendConnectResponse(connect_request);
    }

    void RequestHandler::HandleSubPieceRequest(boost::shared_ptr<protocol::LiveRequestSubPiecePacket> subpiece_request)
    {
        ++(server_->receive_packet_statistics_->GetStatistics(RequestTypes::RequestSubPiece));
        if (SessionManager::SessionAlreadyExists != session_manager_->TryUpdateSession(subpiece_request->end_point, subpiece_request->transaction_id_, protocol::LiveRequestSubPiecePacket::Action))
        {
            return;
        }
        
        assert(subpiece_request->request_sub_piece_count_ == subpiece_request->sub_piece_infos_.size());

        std::map<size_t, std::vector<uint16_t> > subpieces_per_block;
        for(size_t i = 0; i < subpiece_request->sub_piece_infos_.size(); ++i)
        {
            const protocol::LiveSubPieceInfo& subpiece = subpiece_request->sub_piece_infos_[i];
            size_t block_id = subpiece.GetBlockId();
            uint16_t subpiece_index = subpiece.GetSubPieceIndex();
            std::map<size_t, std::vector<uint16_t> >::iterator map_iter = subpieces_per_block.find(block_id);
            if (map_iter == subpieces_per_block.end())
            {
                std::vector<uint16_t> subpieces;
                subpieces.push_back(subpiece_index);
                subpieces_per_block.insert(std::make_pair(block_id, subpieces));
            }
            else
            {
                map_iter->second.push_back(subpiece_index);
            }
        }

        for (std::map<size_t, std::vector<uint16_t> >::const_iterator map_iter = subpieces_per_block.begin();
            map_iter != subpieces_per_block.end();
            ++map_iter)
        {
            size_t block_id = map_iter->first;
            const std::vector<uint16_t>& subpieces = map_iter->second;
            memory_cache_->AsyncGetBlock(subpiece_request->resource_id_, block_id, bind(&ResponseHandler::SendSubPieceResponse, response_handler_, subpiece_request, _1, _2, _3, subpieces, _4));
        }
    }

    void RequestHandler::HandleAnnounceRequest(boost::shared_ptr<protocol::LiveRequestAnnouncePacket> announce_request)
    {
        ++(server_->receive_packet_statistics_->GetStatistics(RequestTypes::RequestAnnounce));
        if (SessionManager::SessionAlreadyExists != session_manager_->TryUpdateSession(announce_request->end_point, announce_request->transaction_id_, protocol::LiveRequestAnnouncePacket::Action))
        {
            return;
        }

        std::map<size_t, boost::shared_ptr<BlockMetaData> > blocks_bitmap;
        if (!memory_cache_->TryGetConsecutiveBlocksBitmap(announce_request->resource_id_, announce_request->request_block_id_, blocks_bitmap))
        {
            return;
        }

        response_handler_->SendAnnounceResponse(announce_request, blocks_bitmap);
    }
}
