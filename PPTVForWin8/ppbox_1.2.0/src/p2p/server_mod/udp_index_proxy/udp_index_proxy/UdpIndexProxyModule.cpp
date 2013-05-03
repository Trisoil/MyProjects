
#include "stdafx.h"
#include "UdpIndexProxyModule.h"
#include "UdpIndexCoreServerHandler.h"
#include "UdpIndexBackupServerHandler.h"
#include "protocal/IndexPacket.h"
#include "framework/network/util.h"
#include "Config.h"
#include <string>

using namespace protocal;
using namespace framework;
using namespace framework::network;

namespace udp_index_proxy
{

    UdpIndexProxyModule::p UdpIndexProxyModule::inst_(new UdpIndexProxyModule());

    UdpIndexProxyModule::UdpIndexProxyModule()
    {

    }

    void UdpIndexProxyModule::Start()
    {
        if (true == is_running_)
            return;

        // port
        local_udp_port_ = Config::Inst().GetInteger("index_proxy.local_udp_port", 4000);
        // receive count
        udp_server_receive_count_ = Config::Inst().GetInteger("index_proxy.udp_server_receive_count", 1000);
        // guid
        mock_guid_.Generate();

        // local address
        LoadLocalAddress();

        // index servers
        if (LoadIndexServers() == false)
        {
            CONSOLE_LOG("加载IndexServers失败");
            return ;
        }

        // UdpServer
        udp_server_ = framework::network::UdpServer::create(shared_from_this());
        if (udp_server_->Listen(local_udp_port_) == false)
        {
            CONSOLE_LOG("IndexProxyModule 在端口" << local_udp_port_ << "监听失败");
        }
        else
        {
            CONSOLE_LOG("IndexProxyModule 在端口" << local_udp_port_ << "监听成功");
        }
        udp_server_->Recv(udp_server_receive_count_);

        is_running_ = true;
    }

    void UdpIndexProxyModule::Stop()
    {
        if (false == is_running_)
            return ;
        is_running_ = false;

        udp_server_->Close();
    }

    bool UdpIndexProxyModule::ParseIndexServerEndpoint(boost::asio::ip::udp::endpoint& end_point, const string& host)
    {
        if (host.length() == 0)
        {
            CONSOLE_LOG("没有指定core_index_server");
            return false;
        }

        boost::asio::ip::udp::endpoint ep = ParseUdpEndpoint(host);
        if (ep.address().to_v4().to_ulong() == 0)
        {
            CONSOLE_LOG("core_index_server地址非法");
            return false;
        }
        if (ep.port() == 0)
        {
            CONSOLE_LOG("必须指定core_index_server端口");
            return false;
        }
        end_point = ep;

        return true;
    }

    void UdpIndexProxyModule::LoadLocalAddress()
    {
        local_ips_.clear();
        LoadLocalIPs(local_ips_);
        local_ips_.push_back(boost::asio::ip::address_v4::from_string("127.0.0.1").to_ulong());
    }

    bool UdpIndexProxyModule::IsSelfIndexServerEndpoint(const boost::asio::ip::udp::endpoint& end_point) const
    {
        if (end_point.port() == local_udp_port_)
        {
            if (std::find(local_ips_.begin(), local_ips_.end(), end_point.address().to_v4().to_ulong()) != local_ips_.end())
                return true;
        }
        return false;
    }

    bool UdpIndexProxyModule::LoadIndexServers()
    {
        // core
        string core_index = Config::Inst().GetValue("index_proxy.core_index_server");
        boost::asio::ip::udp::endpoint core_ep;
        if (!ParseIndexServerEndpoint(core_ep, core_index))
            return false;
        if (IsSelfIndexServerEndpoint(core_ep))
        {
            CONSOLE_LOG("不能使用指向自己的IndexServer");
            return false;
        }
        UdpIndexCoreServerHandler::p core_handler = UdpIndexCoreServerHandler::Create(core_ep);
        AddIndexServer(core_handler);

        // backup
        string backups = Config::Inst().GetValue("index_proxy.backup_index_server");
        std::vector<std::string> backup_eps;
        boost::algorithm::split(backup_eps, backups, boost::is_any_of(";$|-"));
        for (size_t i = 0; i < backup_eps.size(); ++i)
        {
            string backup_index = boost::algorithm::trim_copy(backup_eps[i]);
            if (backup_index.length() == 0)
                continue;

            boost::asio::ip::udp::endpoint backup_ep;
            if (ParseIndexServerEndpoint(backup_ep, backup_index) && !IsSelfIndexServerEndpoint(backup_ep))
            {
                UdpIndexBackupServerHandler::p backup_handler = UdpIndexBackupServerHandler::Create(backup_ep);
                AddIndexServer(backup_handler);
            }
            else
            {
                CONSOLE_LOG("不能使用 " << backup_index );
            }
        }
        return true;
    }

    void UdpIndexProxyModule::AddIndexServer(UdpIndexServerHandler::p index_handler)
    {
        if (index_handler)
        {
            index_servers_[index_handler->GetServerEndpoint()] = index_handler;
            CONSOLE_LOG("添加服务器：" << index_handler->GetServerEndpoint());
        }
    }

    void UdpIndexProxyModule::OnUdpRecv(boost::asio::ip::udp::endpoint& end_point, Buffer& buf)
    {
        if (is_running_ == false) return;

        if( protocal::Cryptography::Decrypt(buf) == false)
        {
            return;
        }

        // 检查包头部正确， 如果读取包头部失败，不处理
        protocal::Packet::p packet = protocal::Packet::ParseFromBinary(buf);
        if(packet)
        {
            UINT8 action = packet->GetAction();
            if (action > 0x10 && action < 0x30)
            {
                DispatchAction(end_point, action, buf);
            }
        }
    }

    UdpIndexServerHandler::p UdpIndexProxyModule::GetIndexServer(const boost::asio::ip::udp::endpoint& index_endpoint) const
    {
        UdpIndexServerHandler::p index_handler;
        if (HasIndexServer(index_endpoint))
        {
            index_handler = index_servers_.find(index_endpoint)->second;
        }
        return index_handler;
    }

    UdpIndexServerHandler::p UdpIndexProxyModule::GetCoreIndexServer() const
    {
        // TODO
        STL_FOR_EACH_CONST(IndexServerMap, index_servers_, iter)
        {
            if (iter->second->IsCore())
                return iter->second;
        }
        return UdpIndexServerHandler::p();
    }

    void UdpIndexProxyModule::DispatchAction(boost::asio::ip::udp::endpoint& end_point, UINT8 action, Buffer& buffer)
    {
        if (false == is_running_) return;

        ServerPacket::p server_packet = ServerPacket::ParseFromBinary(buffer);
        if (server_packet->IsRequest())
        {
            RequestServerPacket::p peer_request_packet = RequestServerPacket::ParseFromBinary(buffer);
            UdpIndexServerHandler::p core_server_handler = GetCoreIndexServer();
            if (core_server_handler && peer_request_packet)
            {
                core_server_handler->OnPeerRequest(end_point, peer_request_packet);
            }
        }
        else
        {
            ResponseServerPacket::p response_packet = ResponseServerPacket::ParseFromBinary(buffer);

            // find from index server map
            UdpIndexServerHandler::p server_handler = GetIndexServer(end_point);
            
            if (server_handler && response_packet)
            {
                server_handler->OnIndexResponse(end_point, response_packet);
            }
        }
    }

    void UdpIndexProxyModule::DoSendPacket(boost::asio::ip::udp::endpoint& end_point, protocal::Packet::p packet)
    {
        if( false == is_running_ )
            return;

        Buffer buf = packet->GetBuffer();

        // 添加校验
        if( protocal::Cryptography::Encrypt(buf) == false)
        {
            return;
        }

        udp_server_->UdpSendTo(buf, end_point);
    }

    void UdpIndexProxyModule::OnCoreQueryHttpServerByRIDFailed(Node::p request_node)
    {
        // 转发给backup
        STL_FOR_EACH_CONST(IndexServerMap, index_servers_, it)
        {
            UdpIndexServerHandler::p handler = it->second;
            if (handler && handler->IsCore() == false)
            {
                handler->OnPeerRequest(request_node->PeerEndpoint, request_node->PeerRequestPacket);
            }
        }
    }

    void UdpIndexProxyModule::OnCoreQueryRidByUrlFailed(Node::p request_node)
    {
        // 转发给backup
        STL_FOR_EACH_CONST(IndexServerMap, index_servers_, it)
        {
            UdpIndexServerHandler::p handler = it->second;
            if (handler && handler->IsCore() == false)
            {
                handler->OnPeerRequest(request_node->PeerEndpoint, request_node->PeerRequestPacket);
            }
        }
    }

    void UdpIndexProxyModule::OnCoreQueryRidByContentFailed(Node::p request_node)
    {
        // 转发给backup
        STL_FOR_EACH_CONST(IndexServerMap, index_servers_, it)
        {
            UdpIndexServerHandler::p handler = it->second;
            if (handler && handler->IsCore() == false)
            {
                handler->OnPeerRequest(request_node->PeerEndpoint, request_node->PeerRequestPacket);
            }
        }
    }

    void UdpIndexProxyModule::OnBackupQueryHttpServerByRIDSucceed(Node::p request_node, QueryHttpServerByRidResponsePacket::p response_packet)
    {
        // Add到Core
        // 没有足够信息
    }
    void UdpIndexProxyModule::OnBackupQueryRidByUrlSucceed(Node::p request_node, QueryRidByUrlResponsePacket::p response_packet)
    {
        QueryRidByUrlRequestPacket::p request_packet = 
            QueryRidByUrlRequestPacket::ParseFromBinary(request_node->PeerRequestPacket->GetBuffer());

        if (request_packet)
        {
            UrlInfo url_info;
            url_info.url_ = request_packet->GetUrlString();
            url_info.refer_url_ = request_packet->GetReferString();

            vector<UrlInfo> http_server;
            http_server.push_back(url_info);

            AddRidUrlRequestPacket::p add_request = AddRidUrlRequestPacket::CreatePacket(Packet::NewTransactionID(), PEER_VERSION, 
                mock_guid_, response_packet->GetResourceID(), response_packet->GetFileLength(), 
                response_packet->GetBlockSize(), response_packet->GetBlockMD5(), http_server, 
                response_packet->GetContentSenseMD5(), response_packet->GetContentBytes()
            );

            // Add到Core
            STL_FOR_EACH_CONST(IndexServerMap, index_servers_, it)
            {
                UdpIndexServerHandler::p handler = it->second;
                if (handler && handler->IsCore() == true)
                {
                    DoSendPacket(handler->GetServerEndpoint(), add_request);
                }
            }
        }

    }
    void UdpIndexProxyModule::OnBackupQueryRidByContentSucceed(Node::p request_node, QueryRidByContentResponsePacket::p response_packet)
    {
        // Add到Core
        // 没有足够信息
    }

}
