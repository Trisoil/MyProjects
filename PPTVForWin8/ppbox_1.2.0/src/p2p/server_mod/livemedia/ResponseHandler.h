//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#ifndef _RESPONSE_HANDLER_H_
#define _RESPONSE_HANDLER_H_

namespace live_media
{
    class ResponseSender;
    class ResponseTask;
    class ChannelManager;

    class ResponseHandler
        : public boost::enable_shared_from_this<ResponseHandler>
    {
    public:
        ResponseHandler(boost::shared_ptr<ChannelManager> manager);

        void Start();

        void Stop();

        void SendConnectResponse(boost::shared_ptr<protocol::ConnectPacket> connect_request);

        void SendSubPieceResponse(boost::shared_ptr<protocol::LiveRequestSubPiecePacket> subpiece_request, ErrorCode error_code, const channel_id& channel_identifier, size_t block_id, const std::vector<uint16_t>& subpieces, boost::shared_ptr<BlockData> block_data);

        void SendAnnounceResponse(boost::shared_ptr<protocol::LiveRequestAnnouncePacket> announce_request, const std::map<size_t, boost::shared_ptr<BlockMetaData> >& blocks_bitmap);

    private:
        void AddResponseTask(boost::shared_ptr<ResponseTask> response_task);

        size_t GetResponseSenderIndex(boost::shared_ptr<ResponseTask> response_task);

    private:
        std::vector<boost::shared_ptr<ResponseSender> > response_senders_;
        size_t fallback_sender_index_;
        boost::shared_ptr<ChannelManager> manager_;
    };
}

#endif //_RESPONSE_HANDLER_H_