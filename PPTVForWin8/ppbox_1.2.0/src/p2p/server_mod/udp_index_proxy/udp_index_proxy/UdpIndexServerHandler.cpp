#include "stdafx.h"
#include "UdpIndexProxyModule.h"
#include "UdpIndexServerHandler.h"
#include "protocal/IndexPacket.h"
#include "Config.h"

using namespace framework;

namespace udp_index_proxy
{
    UdpIndexServerHandler::UdpIndexServerHandler()
        : is_running_(false)
    {

    }
    UdpIndexServerHandler::UdpIndexServerHandler(boost::asio::ip::udp::endpoint server_endpoint)
        : is_running_(false)
        , server_endpoint_(server_endpoint)
    {
    }
    UdpIndexServerHandler::~UdpIndexServerHandler()
    {
    }

    void UdpIndexServerHandler::OnPeerRequest(boost::asio::ip::udp::endpoint peer_endpoint, RequestServerPacket::p request_packet)
    {
        if (request_packet)
        {
            UINT32 proxy_tid = protocal::Packet::NewTransactionID();
            // store mapping
            Node::p node = Node::Create(peer_endpoint, request_packet);
            all_packets_by_tid_[proxy_tid] = node;
            RequestServerPacket::p proxy_packet = RequestServerPacket::ParseFromBinary(request_packet->GetBuffer().Clone());
            proxy_packet->SetTransactionID(proxy_tid);
            // by time
            all_packets_by_time_.insert( make_pair(node->TimeCount, proxy_tid) );
            // redirect
            UdpIndexProxyModule::Inst()->DoSendPacket(server_endpoint_, proxy_packet);
        }
    }

    void UdpIndexServerHandler::OnIndexResponse(boost::asio::ip::udp::endpoint index_endpoint, ResponseServerPacket::p response_packet)
    {
        if (response_packet)
        {
            UINT32 proxy_tid = response_packet->GetTransactionID();
            if (all_packets_by_tid_.find(proxy_tid) != all_packets_by_tid_.end())
            {
                Node::p node = all_packets_by_tid_[proxy_tid];
                UINT32 peer_tid = node->PeerRequestPacket->GetTransactionID();
                response_packet->SetTransactionID(peer_tid);
                all_packets_by_tid_.erase(proxy_tid);
                all_packets_by_time_.erase( make_pair(node->TimeCount, proxy_tid) );
                // send back
                // check
                switch (response_packet->GetAction())
                {
                case protocal::QueryHttpServerByRidResponsePacket::ACTION:
                    {
                        QueryHttpServerByRidResponsePacket::p res = 
                            QueryHttpServerByRidResponsePacket::ParseFromBinary(response_packet->GetBuffer());
                        if (res) 
                        {
                            if (response_packet->ErrorCode() != 0) OnQueryHttpServerByRIDFailed(node, res);
                            else OnQueryHttpServerByRIDSucceed(node, res);
                        }
                        break;
                    }
                case protocal::QueryRidByUrlResponsePacket::ACTION:
                    {
                        QueryRidByUrlResponsePacket::p res =
                            QueryRidByUrlResponsePacket::ParseFromBinary(response_packet->GetBuffer());
                        if (res) 
                        {
                            if (response_packet->ErrorCode() != 0) OnQueryRidByUrlFailed(node, res);
                            else OnQueryRidByUrlSucceed(node, res);
                        }
                        break;
                    }
                case protocal::AddRidUrlResponsePacket::ACTION:
                    {
                        AddRidUrlResponsePacket::p res = 
                            AddRidUrlResponsePacket::ParseFromBinary(response_packet->GetBuffer());
                        if (res) 
                        {
                            if (response_packet->ErrorCode() != 0) OnAddRidUrlFailed(node, res);
                            else OnAddRidUrlSucceed(node, res);
                        }
                        break;
                    }
                case protocal::QueryTrackerListResponsePacket::ACTION:
                    {
                        QueryTrackerListResponsePacket::p res = 
                            QueryTrackerListResponsePacket::ParseFromBinary(response_packet->GetBuffer());
                        if (res) 
                        {
                            if (response_packet->ErrorCode() != 0) OnQueryTrackerListFailed(node, res);
                            else OnQueryTrackerListSucceed(node, res);
                        }
                        break;
                    }
                case protocal::QueryStunServerListResponsePacket::ACTION:
                    {
                        QueryStunServerListResponsePacket::p res =
                            QueryStunServerListResponsePacket::ParseFromBinary(response_packet->GetBuffer());
                        if (res) 
                        {
                            if (response_packet->ErrorCode() != 0) OnQueryStunServerListFailed(node, res);
                            else OnQueryStunServerListSucceed(node, res);
                        }
                        break;
                    }
                case protocal::QueryRidByContentResponsePacket::ACTION:
                    {
                        QueryRidByContentResponsePacket::p res =
                            QueryRidByContentResponsePacket::ParseFromBinary(response_packet->GetBuffer());
                        if (res) 
                        {
                            if (response_packet->ErrorCode() != 0) OnQueryRidByContentFailed(node, res);
                            else OnQueryRidByContentSucceed(node, res);
                        }
                        break;
                    }
                }
            }
        }
    }

    void UdpIndexServerHandler::Start()
    {
        if (true == is_running_)
            return ;

        timeout_interval_ = Config::Inst().GetInteger("index_proxy.timeout_interval_in_millisec", TIMEOUT_INTERVAL_IN_MS);
        check_timeout_interval_ = Config::Inst().GetInteger("index_proxy.check_timeout_interval_in_millisec", CHECK_TIMEOUT_INTERVAL_IN_MS);

        clear_timer_ = framework::timer::PeriodicTimer::create(check_timeout_interval_, shared_from_this());

        if (OnStart() == false)
            return ;

        clear_timer_->Start();

        is_running_ = true;
    }

    void UdpIndexServerHandler::Stop()
    {
        if (false == is_running_)
            return ;

        OnStop();

        if (clear_timer_) { clear_timer_->Stop(); clear_timer_.reset(); }

        is_running_ = false;
    }

    bool UdpIndexServerHandler::OnStart()
    {
        return true;
    }

    void UdpIndexServerHandler::OnStop()
    {

    }
    
    Node::p UdpIndexServerHandler::GetNode(UINT32 proxy_tid) const
    {
        TransIDNodeMap::const_iterator it = all_packets_by_tid_.find(proxy_tid);
        if (it != all_packets_by_tid_.end())
        {
            return it->second;
        }
        return Node::p();
    }

    void UdpIndexServerHandler::OnPacketNotResponsed(UINT32 proxy_tid, Node::p request_node)
    {
        if (!request_node)
            return ;

        all_packets_by_tid_.erase(proxy_tid);
        all_packets_by_time_.erase( make_pair(request_node->TimeCount, proxy_tid) );
    }

    void UdpIndexServerHandler::OnQueryHttpServerByRIDFailed(Node::p request_node, QueryHttpServerByRidResponsePacket::p index_response)
    {
        UdpIndexProxyModule::Inst()->DoSendPacket(request_node->PeerEndpoint, index_response);
    }

    void UdpIndexServerHandler::OnQueryRidByUrlFailed(Node::p request_node, QueryRidByUrlResponsePacket::p index_response)
    {
        UdpIndexProxyModule::Inst()->DoSendPacket(request_node->PeerEndpoint, index_response);
    }

    void UdpIndexServerHandler::OnQueryTrackerListFailed(Node::p request_node, QueryTrackerListResponsePacket::p index_response)
    {
        UdpIndexProxyModule::Inst()->DoSendPacket(request_node->PeerEndpoint, index_response);
    }

    void UdpIndexServerHandler::OnQueryStunServerListFailed(Node::p request_node, QueryStunServerListResponsePacket::p index_response)
    {
        UdpIndexProxyModule::Inst()->DoSendPacket(request_node->PeerEndpoint, index_response);
    }

    void UdpIndexServerHandler::OnQueryRidByContentFailed(Node::p request_node, QueryRidByContentResponsePacket::p index_response)
    {
        UdpIndexProxyModule::Inst()->DoSendPacket(request_node->PeerEndpoint, index_response);
    }

    void UdpIndexServerHandler::OnAddRidUrlFailed(Node::p request_node, AddRidUrlResponsePacket::p index_response)
    {
        UdpIndexProxyModule::Inst()->DoSendPacket(request_node->PeerEndpoint, index_response);
    }

    void UdpIndexServerHandler::OnQueryHttpServerByRIDSucceed(Node::p request_node, QueryHttpServerByRidResponsePacket::p index_response)
    {
        UdpIndexProxyModule::Inst()->DoSendPacket(request_node->PeerEndpoint, index_response);
    }
    void UdpIndexServerHandler::OnQueryRidByUrlSucceed(Node::p request_node, QueryRidByUrlResponsePacket::p index_response)
    {
        UdpIndexProxyModule::Inst()->DoSendPacket(request_node->PeerEndpoint, index_response);
    }
    void UdpIndexServerHandler::OnQueryRidByContentSucceed(Node::p request_node, QueryRidByContentResponsePacket::p index_response)
    {
        UdpIndexProxyModule::Inst()->DoSendPacket(request_node->PeerEndpoint, index_response);
    }
    void UdpIndexServerHandler::OnQueryTrackerListSucceed(Node::p request_node, QueryTrackerListResponsePacket::p index_response)
    {
        UdpIndexProxyModule::Inst()->DoSendPacket(request_node->PeerEndpoint, index_response);
    }
    void UdpIndexServerHandler::OnQueryStunServerListSucceed(Node::p request_node, QueryStunServerListResponsePacket::p index_response)
    {
        UdpIndexProxyModule::Inst()->DoSendPacket(request_node->PeerEndpoint, index_response);
    }
    void UdpIndexServerHandler::OnAddRidUrlSucceed(Node::p request_node, AddRidUrlResponsePacket::p index_response)
    {
        UdpIndexProxyModule::Inst()->DoSendPacket(request_node->PeerEndpoint, index_response);
    }

    void UdpIndexServerHandler::OnTimerElapsed(boost::shared_ptr<Timer> pointer, u_int times)
    {
        if (pointer == clear_timer_)
        {
            // check 
            TimeCounter::count_value_type last = TimeCounter::GetTimeCount() - timeout_interval_;
            STL_FOR_EACH_CONST(TimeTransIDSet, all_packets_by_time_, iter)
            {
                if (iter->first >= last) break;
                UINT32 proxy_tid = iter->second;
                Node::p node = GetNode(proxy_tid);
                OnPacketNotResponsed(proxy_tid, node);
            }
        }
    }
}
