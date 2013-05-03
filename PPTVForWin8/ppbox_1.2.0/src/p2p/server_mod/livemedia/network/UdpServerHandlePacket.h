// UdpServerHandlePacket.h

#ifndef _NETWORK_UDP_SERVER_HANDLE_PACKET_H_
#define _NETWORK_UDP_SERVER_HANDLE_PACKET_H_

#include "network/UdpServer.h"
//#include "protocol/PeerPacket.h"

#include <util/archive/LittleEndianBinaryIArchive.h>
#include <util/archive/LittleEndianBinaryOArchive.h>

#include <iostream>

namespace network
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
        packet.end_point_ = recv_buffer.end_point();
        ((protocol::Packet &)packet).PacketAction = PacketType::Action;
        ia >> packet;
        if (ia) {
            handler_->OnUdpRecv(packet);
        }
    }

    template <typename PacketType>
    int UdpServer::send_packet(
        PacketType const & packet)
    {
        UdpBuffer send_buffer;// = *new UdpBuffer;
		/*
        if (!&send_buffer) {
            return false;
        }
		*/
		assert(boost::asio::ip::udp::endpoint() != packet.end_point_);
        send_buffer.end_point(packet.end_point_);
        send_buffer.commit(sizeof(boost::uint32_t));
        OUdpArchive oa(send_buffer);
        boost::uint8_t action = PacketType::Action;
        oa << action << packet;
        if (oa) {
			//if (PacketType::Action == 0x55) {
			//	std::cout << "Send RequestSubPiece to " << packet.end_point << std::endl;
			//}
			int b_size = send_buffer.size();
            UdpSendTo(send_buffer);
			return b_size;
//			return send_buffer.size();
        } else {
			return -1;
		}
    }

}

#endif //_NETWORK_UDP_SERVER_HANDLE_PACKET_H_
