#pragma once

#include "framework/network/UdpServer.h"
#include "protocal/Packet.h"
#include "UdpIndexServerHandler.h"
#include "RequestNode.h"
#include <map>
using namespace std;

namespace udp_index_proxy
{

    class UdpIndexProxyModule
        : public boost::noncopyable
        , public boost::enable_shared_from_this<UdpIndexProxyModule>
        , public framework::network::IUdpServerListener
    {
    public:

        typedef boost::shared_ptr<UdpIndexProxyModule> p;

        static p Inst() { return inst_; }

    public:

        void Start();
        void Stop();
        bool IsRunning() const { return is_running_; }

        bool LoadIndexServers();

    public:

        void OnUdpRecv(boost::asio::ip::udp::endpoint& end_point, Buffer& buf);
        void DoSendPacket(boost::asio::ip::udp::endpoint& end_point, protocal::Packet::p packet);

        bool HasIndexServer(const boost::asio::ip::udp::endpoint& index_endpoint) const { return index_servers_.find(index_endpoint) != index_servers_.end(); }
        UdpIndexServerHandler::p GetIndexServer(const boost::asio::ip::udp::endpoint& index_endpoint) const;
        UdpIndexServerHandler::p GetCoreIndexServer() const;
        void AddIndexServer(UdpIndexServerHandler::p index_handler);

    public:

        void OnCoreQueryHttpServerByRIDFailed(Node::p request_node);
        void OnCoreQueryRidByUrlFailed(Node::p request_node);
        void OnCoreQueryRidByContentFailed(Node::p request_node);

        void OnBackupQueryHttpServerByRIDSucceed(Node::p request_node, QueryHttpServerByRidResponsePacket::p response_packet);
        void OnBackupQueryRidByUrlSucceed(Node::p request_node, QueryRidByUrlResponsePacket::p response_packet);
        void OnBackupQueryRidByContentSucceed(Node::p request_node, QueryRidByContentResponsePacket::p response_packet);

    protected:

        void DispatchAction(boost::asio::ip::udp::endpoint& end_point, UINT8 action, Buffer& buffer);
        bool ParseIndexServerEndpoint(boost::asio::ip::udp::endpoint& end_point, const string& host);
        void LoadLocalAddress();
        bool IsSelfIndexServerEndpoint(const boost::asio::ip::udp::endpoint& end_point) const;

    private:

        UdpIndexProxyModule();

        static p inst_;

    private:

        typedef map<boost::asio::ip::udp::endpoint, UdpIndexServerHandler::p> IndexServerMap;

    private:

		framework::network::UdpServer::p udp_server_;
        bool is_running_;

        u_short local_udp_port_;
        u_int udp_server_receive_count_;

        IndexServerMap index_servers_;

        Guid mock_guid_;

        vector<ULONG> local_ips_;
    };
}
