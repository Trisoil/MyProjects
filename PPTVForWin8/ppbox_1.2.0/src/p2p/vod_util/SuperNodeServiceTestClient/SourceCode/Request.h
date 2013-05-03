//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_TEST_REQUEST_H
#define SUPER_NODE_TEST_REQUEST_H

#include <deque>

using namespace protocol;

namespace super_node_test
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
        GetSubpiecesRequest(int peer_id, const RID& rid, const string& resource_name, const std::vector<SubPieceInfo>& subpieces);

        void BuildPacketBuffer(UdpBuffer& udp_buffer, const boost::asio::ip::udp::endpoint& service_endpoint, boost::uint32_t transaction_id) const;
    private:
        RID rid_;
        string resource_name_;
        std::vector<SubPieceInfo> subpieces_;
    };

    class CloseSessionRequest: public Request
    {
    public:
        CloseSessionRequest(int peer_id);
        
        void BuildPacketBuffer(UdpBuffer& udp_buffer, const boost::asio::ip::udp::endpoint& service_endpoint, boost::uint32_t transaction_id) const;
    };
}

#endif //SUPER_NODE_TEST_REQUEST_H