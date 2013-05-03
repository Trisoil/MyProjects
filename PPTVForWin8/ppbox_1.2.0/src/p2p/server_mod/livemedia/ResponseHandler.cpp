//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "pre.h"
#include "ResponseHandler.h"
#include "ResponseTask.h"
#include "ResponseSender.h"
#include "channel_manager.h"
#include "channel_server.h"
#include "SessionManagement/SessionManager.h"
#include "SessionManagement/Session.h"

namespace live_media
{
    ResponseHandler::ResponseHandler(boost::shared_ptr<ChannelManager> manager)
    {
        manager_ = manager;
    }

    void ResponseHandler::Start()
    {
        response_senders_.resize(manager_->server_->GetServerConfig().response_senders_count);
        for(size_t i = 0; i < response_senders_.size(); ++i)
        {
            response_senders_[i].reset(new ResponseSender(i, manager_));
            response_senders_[i]->Start();
        }

        fallback_sender_index_ = 0;
    }

    void ResponseHandler::Stop()
    {
        for(size_t i = 0; i < response_senders_.size(); ++i)
        {
            response_senders_[i]->Stop();
        }

        response_senders_.clear();
    }

    void ResponseHandler::SendConnectResponse(boost::shared_ptr<protocol::ConnectPacket> connect_request)
    {
        AddResponseTask(boost::shared_ptr<ResponseTask>(new ConnectResponseTask(connect_request)));
    }

    void ResponseHandler::SendAnnounceResponse(boost::shared_ptr<protocol::LiveRequestAnnouncePacket> announce_request, const std::map<size_t, boost::shared_ptr<BlockMetaData> >& blocks_bitmap)
    {
        ChannelPointer target_channel = manager_->GetChannel(announce_request->resource_id_);
        size_t channel_step_size = 0;
        if (target_channel)
        {
            channel_step_size = target_channel->channel_config_.channel_step_time;
        }

        AddResponseTask(boost::shared_ptr<ResponseTask>(new AnnounceResponseTask(announce_request, blocks_bitmap, channel_step_size)));
    }

    void ResponseHandler::SendSubPieceResponse(boost::shared_ptr<protocol::LiveRequestSubPiecePacket> subpiece_request, ErrorCode error_code, const channel_id& channel_identifier, size_t block_id, const std::vector<uint16_t>& subpieces, boost::shared_ptr<BlockData> block_data)
    {
        if (error_code != ErrorCodes::Success || !block_data)
        {
            return;
        }

        boost::shared_ptr<ResponseTask> response_task(new SubPieceResponseTask(subpiece_request, block_id, subpieces, block_data));
        AddResponseTask(response_task);
    }

    size_t ResponseHandler::GetResponseSenderIndex(boost::shared_ptr<ResponseTask> response_task)
    {
        boost::shared_ptr<SessionManager> session_manager = manager_->server_->GetSessionManager();
        boost::shared_ptr<Session> session = session_manager->GetSession(response_task->GetTargetEndpoint());

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

    void ResponseHandler::AddResponseTask(boost::shared_ptr<ResponseTask> response_task)
    {
        size_t sender_index = GetResponseSenderIndex(response_task);
        assert(sender_index < response_senders_.size());
        response_senders_[sender_index]->SendResponse(response_task);
    }
}