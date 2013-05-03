//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#include "Common.h"
#include "UdpServerManager.h"
#include "RegisterSuperNodePacket.h"

namespace super_node
{
    bool UdpServerManager::Start(const UdpServerConfig& config)
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::UdpServer(), "UdpServerManager::Start()");
        udp_server_.reset(new protocol::UdpServer(*io_service_, udp_server_listener_));
        udp_server_->set_minimal_protocol_verion(config.minimal_protocol_version_);
        if ( false == udp_server_->Listen(config.port_) )
        {
            LOG4CPLUS_ERROR(Loggers::UdpServer(), "UdpServer listen failed, port " << config.port_
                << "has been used. ");
            return false;;
        }

        port_ = config.port_;

        RegisterSuperNodePacket(*udp_server_);
        udp_server_->Recv(config.recv_num_);
        LOG4CPLUS_INFO(Loggers::UdpServer(), "UdpServerManager started.");
        return true;
    }

    void UdpServerManager::Stop()
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::UdpServer(), "UdpServerManager::Stop()");
        udp_server_->Close();
        udp_server_.reset();
        udp_server_listener_.reset();
        LOG4CPLUS_INFO(Loggers::UdpServer(), "UdpServerManager stopped.");
    }

    void UdpServerManager::UpdateConfig()
    {
        UdpServerConfig new_config;
        new_config.LoadConfig();

        udp_server_->set_minimal_protocol_verion(new_config.minimal_protocol_version_);
        udp_server_->Recv(new_config.recv_num_);

        if (port_ != new_config.port_)
        {
            LOG4CPLUS_WARN(Loggers::UdpServer(), "The port in config file has been changed, "
                << "which is not supported when service is running.");
        }
    }
}