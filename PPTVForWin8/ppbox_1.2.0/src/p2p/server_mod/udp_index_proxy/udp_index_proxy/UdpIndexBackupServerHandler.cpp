#include "stdafx.h"
#include "UdpIndexBackupServerHandler.h"
#include "UdpIndexProxyModule.h"

namespace udp_index_proxy
{
    UdpIndexBackupServerHandler::UdpIndexBackupServerHandler()
    {
    }

    UdpIndexBackupServerHandler::UdpIndexBackupServerHandler(boost::asio::ip::udp::endpoint server_endpoint)
        : UdpIndexServerHandler(server_endpoint)
    {
    }

    UdpIndexBackupServerHandler::p UdpIndexBackupServerHandler::Create()
    {
        return UdpIndexBackupServerHandler::p(new UdpIndexBackupServerHandler());
    }

    UdpIndexBackupServerHandler::p UdpIndexBackupServerHandler::Create(boost::asio::ip::udp::endpoint server_endpoint)
    {
        return UdpIndexBackupServerHandler::p(new UdpIndexBackupServerHandler(server_endpoint));
    }


    void UdpIndexBackupServerHandler::OnQueryHttpServerByRIDSucceed(Node::p request_node, QueryHttpServerByRidResponsePacket::p index_response)
    {
        UdpIndexProxyModule::Inst()->OnBackupQueryHttpServerByRIDSucceed(request_node, index_response);
        UdpIndexServerHandler::OnQueryHttpServerByRIDSucceed(request_node, index_response);
    }
    void UdpIndexBackupServerHandler::OnQueryRidByUrlSucceed(Node::p request_node, QueryRidByUrlResponsePacket::p index_response)
    {
        UdpIndexProxyModule::Inst()->OnBackupQueryRidByUrlSucceed(request_node, index_response);
        UdpIndexServerHandler::OnQueryRidByUrlSucceed(request_node, index_response);
    }
    void UdpIndexBackupServerHandler::OnQueryRidByContentSucceed(Node::p request_node, QueryRidByContentResponsePacket::p index_response)
    {
        UdpIndexProxyModule::Inst()->OnBackupQueryRidByContentSucceed(request_node, index_response);
        UdpIndexServerHandler::OnQueryRidByContentSucceed(request_node, index_response);
    }

}
