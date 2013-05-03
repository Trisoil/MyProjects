#pragma once

#include "protocal/IndexPacket.h"
#include "framework/timer/TimeCounter.h"
#include "framework/timer/Timer.h"
#include "RequestNode.h"
#include <hash_map>
#include <map>

using namespace framework;
using namespace framework::timer;

namespace udp_index_proxy
{

    class UdpIndexServerHandler
        : public boost::noncopyable
        , public boost::enable_shared_from_this<UdpIndexServerHandler>
        , public framework::timer::ITimerListener
    {
    public:

        typedef boost::shared_ptr<UdpIndexServerHandler> p;

    protected:

    public:

        UdpIndexServerHandler();
        UdpIndexServerHandler(boost::asio::ip::udp::endpoint server_endpoint);
        virtual ~UdpIndexServerHandler();

    public:

        void Start();
        void Stop();
        bool IsRunning() const { return is_running_; }
        virtual bool IsCore() const = 0;

    public:

        //void SetServerEndPoint(const boost::asio::ip::udp::endpoint& server_endpoint) { server_endpoint_ = server_endpoint; }
        boost::asio::ip::udp::endpoint GetServerEndpoint() const { return server_endpoint_; }

        virtual void OnPeerRequest(boost::asio::ip::udp::endpoint peer_endpoint, RequestServerPacket::p request_packet);
        virtual void OnIndexResponse(boost::asio::ip::udp::endpoint index_endpoint, ResponseServerPacket::p response_packet);

    protected:

        virtual bool OnStart();
        virtual void OnStop();
        virtual void OnTimerElapsed(boost::shared_ptr<Timer> pointer, u_int times);

        virtual void OnPacketNotResponsed(UINT32 proxy_id, Node::p request_node);

        virtual void OnQueryHttpServerByRIDFailed(Node::p request_node, QueryHttpServerByRidResponsePacket::p index_response);
        virtual void OnQueryRidByUrlFailed(Node::p request_node, QueryRidByUrlResponsePacket::p index_response);
        virtual void OnQueryRidByContentFailed(Node::p request_node, QueryRidByContentResponsePacket::p index_response);
        virtual void OnQueryTrackerListFailed(Node::p request_node, QueryTrackerListResponsePacket::p index_response);
        virtual void OnQueryStunServerListFailed(Node::p request_node, QueryStunServerListResponsePacket::p index_response);
        virtual void OnAddRidUrlFailed(Node::p request_node, AddRidUrlResponsePacket::p index_response);

        virtual void OnQueryHttpServerByRIDSucceed(Node::p request_node, QueryHttpServerByRidResponsePacket::p index_response);
        virtual void OnQueryRidByUrlSucceed(Node::p request_node, QueryRidByUrlResponsePacket::p index_response);
        virtual void OnQueryRidByContentSucceed(Node::p request_node, QueryRidByContentResponsePacket::p index_response);
        virtual void OnQueryTrackerListSucceed(Node::p request_node, QueryTrackerListResponsePacket::p index_response);
        virtual void OnQueryStunServerListSucceed(Node::p request_node, QueryStunServerListResponsePacket::p index_response);
        virtual void OnAddRidUrlSucceed(Node::p request_node, AddRidUrlResponsePacket::p index_response);

    protected:

        typedef std::set< pair<TimeCounter::count_value_type, UINT32> >  TimeTransIDSet;
        typedef stdext::hash_map<UINT32, Node::p> TransIDNodeMap;

    protected:

        Node::p GetNode(UINT32 proxy_tid) const;

    protected:

        bool is_running_;

        boost::asio::ip::udp::endpoint server_endpoint_;

        TransIDNodeMap all_packets_by_tid_;

        TimeTransIDSet all_packets_by_time_;

        framework::timer::PeriodicTimer::p clear_timer_;

        UINT32 timeout_interval_;
        UINT32 check_timeout_interval_;

    protected:

        const static UINT32 TIMEOUT_INTERVAL_IN_MS = 5000;
        const static UINT32 CHECK_TIMEOUT_INTERVAL_IN_MS = 3000;

    };
}
