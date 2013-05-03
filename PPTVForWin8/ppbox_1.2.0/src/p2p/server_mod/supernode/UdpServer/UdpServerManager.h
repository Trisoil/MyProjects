//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#ifndef _UDP_SERVER_MANAGER_
#define _UDP_SERVER_MANAGER_

#include "protocol/UdpServer.h"
#include "SuperNodeServiceStruct.h"

namespace super_node
{
    class UdpServerManager
        : public count_object_allocate<UdpServerManager>
    {
    public:
        UdpServerManager(boost::shared_ptr<boost::asio::io_service> io_service, boost::shared_ptr<protocol::IUdpServerListener> listener)
            :io_service_(io_service),udp_server_listener_(listener)
        {
            
        }
        bool Start(const UdpServerConfig& config);
        void Stop();

        template <typename PacketType>
        bool SendPacket(PacketType const & packet, boost::uint16_t dest_protocol_version)
        {
            return udp_server_->send_packet(packet, dest_protocol_version);
        }

        void UpdateConfig();

    private:
        boost::shared_ptr<protocol::UdpServer> udp_server_;
        boost::shared_ptr<boost::asio::io_service> io_service_;
        boost::shared_ptr<protocol::IUdpServerListener> udp_server_listener_;
        boost::uint16_t port_;
    };
}

#endif