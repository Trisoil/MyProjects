//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef LIVE_MEDIA_TEST_REQUEST_H
#define LIVE_MEDIA_TEST_REQUEST_H

#include <deque>

using namespace protocol;

namespace live_media_test
{
    class Request
    {
    public:
        virtual void BuildPacketBuffer(UdpBuffer& udp_buffer, const boost::asio::ip::udp::endpoint& service_endpoint, boost::uint32_t transaction_id) const = 0;

        int GetPeerId() const { return peer_id_; }
    protected:
        Request(int peer_id);

    private:
        int peer_id_; 
    };

    class GetSubpiecesRequest: public Request
    {
    public:
        GetSubpiecesRequest(int peer_id, const RID& channel_id, const std::vector<LiveSubPieceInfo>& subpieces);

        void BuildPacketBuffer(UdpBuffer& udp_buffer, const boost::asio::ip::udp::endpoint& service_endpoint, boost::uint32_t transaction_id) const;
    private:
        RID channel_id_;
        std::vector<LiveSubPieceInfo> subpieces_;
    };

    class CloseSessionRequest: public Request
    {
    public:
        CloseSessionRequest(int peer_id);
        
        void BuildPacketBuffer(UdpBuffer& udp_buffer, const boost::asio::ip::udp::endpoint& service_endpoint, boost::uint32_t transaction_id) const;
    };

    class ConnectRequest: public Request
    {
    public:
        ConnectRequest(int peer_id, const Guid& peer_guid, const RID& channel_id);
        void BuildPacketBuffer(UdpBuffer& udp_buffer, const boost::asio::ip::udp::endpoint& service_endpoint, boost::uint32_t transaction_id) const;

    private:
        RID channel_id_;
        Guid peer_guid_;
    };

    class AnnouceRequest: public Request
    {
    public:
        AnnouceRequest(int peer_id, const RID& channel_id, size_t request_block_id);
        void BuildPacketBuffer(UdpBuffer& udp_buffer, const boost::asio::ip::udp::endpoint& service_endpoint, boost::uint32_t transaction_id) const;

    private:
        RID channel_id_;
        size_t request_block_id_;
    };
}

#endif //LIVE_MEDIA_TEST_REQUEST_H