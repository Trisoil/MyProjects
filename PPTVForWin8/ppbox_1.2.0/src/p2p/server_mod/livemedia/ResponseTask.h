//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#ifndef _RESPONSE_TASK_H_
#define _RESPONSE_TASK_H_

namespace live_media
{
    class ChannelServer;

    class ResponseTask
    {
    public:
        ResponseTask(const boost::asio::ip::udp::endpoint& endpoint): endpoint_(endpoint){}
        const boost::asio::ip::udp::endpoint& GetTargetEndpoint() const { return endpoint_; }
        virtual void Execute(ChannelServer* server) = 0;
        virtual ~ResponseTask(){}

    private:
        boost::asio::ip::udp::endpoint endpoint_;
    };

    class ConnectResponseTask
        : public ResponseTask
    {
    private:
        boost::shared_ptr<protocol::ConnectPacket> connect_request_;
    public:
        ConnectResponseTask(boost::shared_ptr<protocol::ConnectPacket> connect_request);

        void Execute(ChannelServer* server);
    };

    class SubPieceResponseTask
        : public ResponseTask
    {
        boost::shared_ptr<protocol::LiveRequestSubPiecePacket> subpiece_request_;
        size_t block_id_;
        std::vector<uint16_t> subpieces_;
        boost::shared_ptr<BlockData> block_data_;

    public:
        SubPieceResponseTask(boost::shared_ptr<protocol::LiveRequestSubPiecePacket> subpiece_request, size_t block_id, const std::vector<uint16_t>& subpieces, boost::shared_ptr<BlockData> block_data);

        void Execute(ChannelServer* server);

    private:
        void SendSubPiece(ChannelServer* server, boost::uint16_t subpiece_index);
    };

    class AnnounceResponseTask
        : public ResponseTask
    {
    private:
        boost::shared_ptr<protocol::LiveRequestAnnouncePacket> announce_request_;
        std::map<size_t, boost::shared_ptr<BlockMetaData> > blocks_bitmap_;
        size_t channel_step_size_;
    public:
        AnnounceResponseTask(boost::shared_ptr<protocol::LiveRequestAnnouncePacket> announce_request, const std::map<size_t, boost::shared_ptr<BlockMetaData> >& blocks_bitmap, size_t channel_step_size);
        void Execute(ChannelServer* server);

    private:
        void BuildAnnounceMap(protocol::LiveAnnounceMap& announce_map);
    };
}

#endif //_RESPONSE_TASK_H_