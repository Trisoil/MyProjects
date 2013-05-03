#pragma once

#include "UdpIndexServerHandler.h"

namespace udp_index_proxy
{
    class UdpIndexBackupServerHandler
        : public UdpIndexServerHandler
        , public boost::enable_shared_from_this<UdpIndexBackupServerHandler>
    {
    public:

        typedef boost::shared_ptr<UdpIndexBackupServerHandler> p;

        static p Create();
        static p Create(boost::asio::ip::udp::endpoint server_endpoint);

    public:

        virtual bool IsCore() const { return false; }

        virtual void OnQueryHttpServerByRIDSucceed(Node::p request_node, QueryHttpServerByRidResponsePacket::p index_response);
        virtual void OnQueryRidByUrlSucceed(Node::p request_node, QueryRidByUrlResponsePacket::p index_response);
        virtual void OnQueryRidByContentSucceed(Node::p request_node, QueryRidByContentResponsePacket::p index_response);

    private:

        UdpIndexBackupServerHandler();
        UdpIndexBackupServerHandler(boost::asio::ip::udp::endpoint server_endpoint);

    };
}