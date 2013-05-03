#pragma once

#include "UdpIndexServerHandler.h"

namespace udp_index_proxy
{
    class UdpIndexCoreServerHandler
        : public UdpIndexServerHandler
        , public boost::enable_shared_from_this<UdpIndexCoreServerHandler>
    {
    public:

        typedef boost::shared_ptr<UdpIndexCoreServerHandler> p;

        static p Create();
        static p Create(boost::asio::ip::udp::endpoint server_endpoint);

    public:

        virtual bool IsCore() const { return true; }

    protected:

        virtual void OnQueryHttpServerByRIDFailed(Node::p request_node, QueryHttpServerByRidResponsePacket::p index_response);
        virtual void OnQueryRidByUrlFailed(Node::p request_node, QueryRidByUrlResponsePacket::p index_response);
        virtual void OnQueryRidByContentFailed(Node::p request_node, QueryRidByContentResponsePacket::p index_response);

    private:

        UdpIndexCoreServerHandler();
        UdpIndexCoreServerHandler(boost::asio::ip::udp::endpoint server_endpoint);
    };
}