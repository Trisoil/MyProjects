//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#include "Common.h"
#include "UdpServerListenHandler.h"
#include "RequestParser.h"
#include "MemoryCache.h"
#include "UdpServerManager.h"
#include "SessionManager.h"
#include "ResponseSender.h"
#include "ErrorResponseTask.h"
#include "SubPieceResponseTask.h"
#include "Session.h"
#include "../Statistics/UdpServerStatistics.h"

namespace super_node
{
    UdpServerListenHandler::UdpServerListenHandler(boost::shared_ptr<MemoryCache> memory_cache, boost::shared_ptr<SessionManager> session_manager, size_t response_senders_count)
        : memory_cache_(memory_cache), session_manager_(session_manager)
    {
        guid_.generate();
        statistics_.reset(new UdpServerStatistics());
        
        response_senders_count_ = response_senders_count;
        if (response_senders_count_ == 0 || response_senders_count_ > 10)
        {
            response_senders_count_ = UdpServerConfig::DefaultResponseSendersCount;
        }
    }

    void UdpServerListenHandler::Start(boost::shared_ptr<UdpServerManager> udp_server)
    {
        server_ = udp_server;
        statistics_->Start();
        time_counter_.reset();

        response_senders_.resize(response_senders_count_);
        for(size_t i = 0; i < response_senders_.size(); ++i)
        {
            response_senders_[i].reset(new ResponseSender(i, server_));
            response_senders_[i]->Start();
        }

        fallback_sender_index_ = 0;
    }

    void UdpServerListenHandler::Stop()
    {
        statistics_->Stop();
        
        for(size_t i = 0; i < response_senders_.size(); ++i)
        {
            response_senders_[i]->Stop();
        }

        response_senders_.clear();

        server_.reset();
    }

    void UdpServerListenHandler::OnUdpRecv(protocol::Packet const & packet)
    {
        if (!server_)
        {
            return;
        }

        if (packet.PacketAction == protocol::RequestSubPiecePacketFromSN::Action)
        {
            statistics_->OnRequestForSubPieceReceived();
            SessionManager::Type error_type = session_manager_->TryAddSession(packet.end_point, packet.transaction_id_);
            if (error_type == SessionManager::NewSessionCreated || error_type == SessionManager::SessionAlreadyExists)
            {
                if (error_type == SessionManager::NewSessionCreated)
                {
                    statistics_->OnNewSessionAccepted();
                }

                std::map<boost::uint16_t, std::set<boost::uint16_t> > request_in_sn_type;

                protocol::RequestSubPiecePacketFromSN const & request_subpiece_packet =
                    static_cast<protocol::RequestSubPiecePacketFromSN const &>(packet);

                RequestParser::ParseFromPeerToSN(request_subpiece_packet, request_in_sn_type);

                size_t sub_piece_cnt = 0;

                for (std::map<boost::uint16_t, std::set<boost::uint16_t> >::const_iterator iter = request_in_sn_type.begin();
                    iter != request_in_sn_type.end(); ++iter)
                {
                    memory_cache_->AsyncReadBlock(request_subpiece_packet.resource_name_, iter->first, *(iter->second.begin()),
                        boost::bind(&UdpServerListenHandler::HandleReadBlock, shared_from_this(), _1, _2,
                        request_subpiece_packet.resource_id_, request_subpiece_packet.transaction_id_,
                        iter->first, iter->second,
                        request_subpiece_packet.protocol_version_, request_subpiece_packet.end_point));

                    statistics_->OnBlockSubpieceDensityStat(*(iter->second.begin()), *(iter->second.rbegin()), iter->second.size());

                    sub_piece_cnt += iter->second.size();
                }

                if (sub_piece_cnt)
                    statistics_->OnResourceSubpieceDensityStat(
                            request_in_sn_type.begin()->first * (BlockData::MaxBlockSize / SUB_PIECE_SIZE) + *request_in_sn_type.begin()->second.begin(),
                            request_in_sn_type.rbegin()->first * (BlockData::MaxBlockSize / SUB_PIECE_SIZE) + *request_in_sn_type.rbegin()->second.rbegin(),
                            sub_piece_cnt
                        );
            }
            else if (error_type == SessionManager::MaxSessionNumberReached)
            {
                const protocol::RequestSubPiecePacketFromSN & sn_packet = dynamic_cast<const protocol::RequestSubPiecePacketFromSN&>(packet);

                boost::shared_ptr<ResponseTask> error_response(
                    new ErrorResponseTask(
                        sn_packet.end_point, 
                        sn_packet.transaction_id_, 
                        sn_packet.resource_id_, 
                        guid_, 
                        protocol::ErrorPacket::PPV_CONNECT_CONNECTION_FULL, 
                        sn_packet.protocol_version_));
                
                AddResponseTask(error_response);

                statistics_->OnNewSessionRejected();
            } 
            else 
            {
                statistics_->OnDuplicateRequestReceived();
            }
        }
        else if (packet.PacketAction == protocol::CloseSessionPacket::Action)
        {
            session_manager_->CloseSession(packet.end_point);
            statistics_->OnClientClosedSession();
        }
        else
        {
            statistics_->OnUnknownPacketReceived();
            LOG4CPLUS_WARN(Loggers::UdpServer(), "UdpServer received illegal packet.");
        }

        if (time_counter_.elapse() >= UdpServerStatistics::StatisticsIntervalInSeconds*1000)
        {
            statistics_->ReportStatus(session_manager_);
            time_counter_.reset();
        }
    }

    void UdpServerListenHandler::HandleReadBlock(const ErrorCode & error_code, const boost::shared_ptr<BlockData> & block,
        const RID & resource_id, boost::uint32_t transaction_id, boost::uint16_t block_index,
        const std::set<boost::uint16_t> & subpiece_indexs, boost::uint16_t dest_protocol_version,
        const boost::asio::ip::udp::endpoint & end_point)
    {
        if (!server_)
        {
            return;
        }

        if (error_code == ErrorCodes::Success)
        {
            for(std::set<boost::uint16_t>::const_iterator iter = subpiece_indexs.begin(); iter != subpiece_indexs.end(); ++iter)
            {
                protocol::SubPieceInfo sub_piece_info = RequestParser::ParseFromSNToPeer(block_index, *iter);
                boost::uint16_t subpiece_index = *iter;

                if (subpiece_index < block->GetSubPieceNumber())
                {
                    statistics_->OnSubPieceResponseSent();

                    boost::shared_ptr<ResponseTask> subpiece_response(
                        new SubPieceResponseTask(
                            end_point, 
                            block,
                            subpiece_index,
                            transaction_id,
                            resource_id,
                            guid_,
                            sub_piece_info,
                            dest_protocol_version));

                    AddResponseTask(subpiece_response);
                }
                else
                {
                    statistics_->OnError(ErrorCodes::ResourceNotFound);

                    boost::shared_ptr<ResponseTask> error_response(
                        new ErrorResponseTask(
                            end_point, 
                            transaction_id, 
                            resource_id, 
                            guid_, 
                            protocol::ErrorPacket::PPV_SUBPIECE_NO_RESOURCEID, 
                            dest_protocol_version));

                    AddResponseTask(error_response);
                }
            }
        }
        else
        {
            statistics_->OnError(error_code);

            boost::shared_ptr<ResponseTask> error_response(
                new ErrorResponseTask(
                end_point, 
                transaction_id, 
                resource_id, 
                guid_, 
                protocol::ErrorPacket::PPV_SUBPIECE_NO_RESOURCEID, 
                dest_protocol_version));

            AddResponseTask(error_response);
        }
    }

    size_t UdpServerListenHandler::GetResponseSenderIndex(boost::shared_ptr<ResponseTask> response_task)
    {
        boost::shared_ptr<Session> session = session_manager_->GetSession(response_task->GetTargetEndpoint());

        size_t thread_affinity_id(0);
        if (session)
        {
            thread_affinity_id = session->GetThreadAffinityId();
        }
        else
        {
            thread_affinity_id = fallback_sender_index_++;
        }

        return thread_affinity_id%response_senders_.size();
    }

    void UdpServerListenHandler::AddResponseTask(boost::shared_ptr<ResponseTask> response_task)
    {
        size_t sender_index = GetResponseSenderIndex(response_task);
        assert(sender_index < response_senders_.size());
        response_senders_[sender_index]->SendResponse(response_task);
    }
}
