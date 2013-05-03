#include "stdafx.h"

#include "UdpIndexCoreServerHandler.h"
#include "RequestNode.h"
#include "UdpIndexProxyModule.h"

namespace udp_index_proxy
{
    UdpIndexCoreServerHandler::UdpIndexCoreServerHandler()
    {
    }

    UdpIndexCoreServerHandler::UdpIndexCoreServerHandler(boost::asio::ip::udp::endpoint server_endpoint)
        : UdpIndexServerHandler(server_endpoint)
    {
    }

    UdpIndexCoreServerHandler::p UdpIndexCoreServerHandler::Create()
    {
        return UdpIndexCoreServerHandler::p(new UdpIndexCoreServerHandler());
    }

    UdpIndexCoreServerHandler::p UdpIndexCoreServerHandler::Create(boost::asio::ip::udp::endpoint server_endpoint)
    {
        return UdpIndexCoreServerHandler::p(new UdpIndexCoreServerHandler(server_endpoint));
    }

    void UdpIndexCoreServerHandler::OnQueryHttpServerByRIDFailed(Node::p request_node, QueryHttpServerByRidResponsePacket::p index_response)
    {
        UdpIndexProxyModule::Inst()->OnCoreQueryHttpServerByRIDFailed(request_node);
    }

    void UdpIndexCoreServerHandler::OnQueryRidByUrlFailed(Node::p request_node, QueryRidByUrlResponsePacket::p index_response)
    {
        UdpIndexProxyModule::Inst()->OnCoreQueryRidByUrlFailed(request_node);
    }

    void UdpIndexCoreServerHandler::OnQueryRidByContentFailed(Node::p request_node, QueryRidByContentResponsePacket::p index_response)
    {
        UdpIndexProxyModule::Inst()->OnCoreQueryRidByContentFailed(request_node);
    }

}