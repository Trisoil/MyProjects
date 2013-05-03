//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

// UdpServerHandlePacket.h

#ifndef _NETWORK_UDP_SERVER_HANDLE_PACKET_H_
#define _NETWORK_UDP_SERVER_HANDLE_PACKET_H_

#include "protocol/PeerPacket.h"
#include <util/archive/LittleEndianBinaryIArchive.h>
#include <util/archive/LittleEndianBinaryOArchive.h>
#include "UdpServer.h"
#include <iostream>

namespace protocol
{

    typedef util::archive::LittleEndianBinaryIArchive<boost::uint8_t> IUdpArchive;
    typedef util::archive::LittleEndianBinaryOArchive<boost::uint8_t> OUdpArchive;

    template <typename PacketType>
    void UdpServer::register_packet()
    {
        boost::uint8_t action = PacketType::Action;
        packet_handlers_[action] =
            &UdpServer::handle_packet<PacketType>;
    }

    template <typename PacketType>
    void UdpServer::handle_packet(
        UdpBuffer & recv_buffer)
    {
        IUdpArchive ia(recv_buffer);
        PacketType packet;
        packet.end_point = recv_buffer.end_point();

        ((protocol::Packet &)packet).PacketAction = PacketType::Action;
        ia >> packet;
        if (ia) 
        {
            handler_->OnUdpRecv(packet);
        }
        else
        {
            boost::uint32_t ip = recv_buffer.end_point().address().to_v4().to_ulong();
            if (invalid_ip_count_.count(ip) == 0)
            {
                invalid_ip_count_[ip] = 0;
            }
            else
            {
                invalid_ip_count_[ip]++;
            }
        }
    }

    template <typename PacketType>
    bool UdpServer::send_packet(
        PacketType const & packet,
        boost::uint16_t dest_protocol_version)
    {
#ifdef BOOST_WINDOWS_API
        boost::shared_ptr<UdpBuffer> send_buffer_pointer = boost::shared_ptr<UdpBuffer>(new UdpBuffer());
        UdpBuffer& send_buffer = *send_buffer_pointer;
#else
        UdpBuffer send_buffer;
#endif

        assert(boost::asio::ip::udp::endpoint() != packet.end_point);
        send_buffer.end_point(packet.end_point);
        send_buffer.commit(sizeof(boost::uint32_t));
        OUdpArchive oa(send_buffer);
        boost::uint8_t action = PacketType::Action;
        oa << action << packet;
        if (oa)
        {
#ifdef BOOST_WINDOWS_API
            UdpAsyncSendTo(send_buffer_pointer, dest_protocol_version);
#else
            UdpSendTo(send_buffer, dest_protocol_version);
#endif
        }
        return oa;
    }
}

#endif  // _NETWORK_UDP_SERVER_HANDLE_PACKET_H_
