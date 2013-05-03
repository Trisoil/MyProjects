#pragma once

#include "protocal/IndexPacket.h"
#include "framework/timer/TimeCounter.h"
#include "framework/timer/Timer.h"

using namespace framework;
using namespace framework::timer;

namespace udp_index_proxy
{

    class Node 
        : public boost::noncopyable
    {
    public:
        typedef boost::shared_ptr<Node> p;
        static p Create(boost::asio::ip::udp::endpoint peer_endpoint, RequestServerPacket::p peer_request_packet)
        {
            return p(new Node(peer_endpoint, peer_request_packet));
        }
    public:
        boost::asio::ip::udp::endpoint PeerEndpoint;
        RequestServerPacket::p PeerRequestPacket;
        TimeCounter::count_value_type TimeCount;
    private:
        Node(boost::asio::ip::udp::endpoint peer_endpoint, RequestServerPacket::p peer_request_packet)
            : PeerEndpoint(peer_endpoint)
            , PeerRequestPacket(peer_request_packet)
            , TimeCount(TimeCounter::GetTimeCount())
        {
        }
    };

}