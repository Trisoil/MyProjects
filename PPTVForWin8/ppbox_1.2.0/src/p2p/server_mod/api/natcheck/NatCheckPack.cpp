//-------------------------------------------------------------
//     Copyright (c) 2012 PPLive Inc.  All rights reserved.
//-------------------------------------------------------------
#include "NatCheckPack.h"
#include <stdio.h>

namespace ns_pplive
{
    namespace ns_natcheck
    {
        NatCheckPack::NatCheckPack()
        {
        }

        NatCheckPack::~NatCheckPack()
        {
        }

        int NatCheckPack::PackSamRoute(boost::uint32_t transaction_id,boost::uint16_t query_times,
            boost::uint32_t local_ip,boost::uint16_t local_port,boost::uint32_t peer_version,
            const boost::asio::ip::udp::endpoint& endpoint_,UdpBuffer & send_buffer)
        {
            protocol::NatCheckSameRoutePacket packet;
            packet.PacketAction = protocol::NatCheckSameRoutePacket::Action;
            packet.transaction_id_ = transaction_id;
            packet.query_times_ = query_times;
            packet.request.local_ip_ = local_ip;
            packet.request.local_port_ = local_port;
            packet.end_point = endpoint_;
            PackSerialize(send_buffer, protocol::NatCheckSameRoutePacket::Action, packet);
            return 0;
        }

        int NatCheckPack::UnPackSamRoute(UdpBuffer& recv_buffer, boost::uint32_t transaction_id, 
            boost::uint8_t& error_code,boost::uint16_t& query_times,boost::asio::ip::udp::endpoint& detect_endpoint)
        {
            protocol::NatCheckSameRoutePacket packet;
            int ret = UnpackUnSerialize(recv_buffer,transaction_id,packet,error_code);            
            if (ret != 0)
            {
                return ret;
            }

            detect_endpoint.address(boost::asio::ip::address_v4(packet.response.detected_ip_));
            detect_endpoint.port(packet.response.detect_udp_port_);
            query_times = packet.query_times_;

            //packet的response里的其他字段，tool里不再需要了。如果有需要，这里可以继续解析

            return 0;
        }

        int NatCheckPack::PackDiffPort(boost::uint32_t transaction_id,boost::uint16_t query_times,
            boost::uint32_t local_ip,boost::uint16_t local_port,boost::uint32_t peer_version,
            const boost::asio::ip::udp::endpoint& endpoint_,UdpBuffer & send_buffer)
        {
            protocol::NatCheckDiffPortPacket packet;
            packet.PacketAction = protocol::NatCheckDiffPortPacket::Action;
            packet.transaction_id_ = transaction_id;
            packet.query_times_ = query_times;
            packet.request.local_ip_ = local_ip;
            packet.request.local_port_ = local_port;
            packet.end_point = endpoint_;
            PackSerialize(send_buffer, protocol::NatCheckDiffPortPacket::Action, packet);
            return 0;
        }

        int NatCheckPack::UnPackDiffPort(UdpBuffer& recv_buffer, boost::uint32_t transaction_id, 
            boost::uint8_t& error_code,boost::uint16_t& query_times,boost::asio::ip::udp::endpoint& detect_endpoint,boost::uint16_t& send_nc_port)
        {
            protocol::NatCheckDiffPortPacket packet;
            int ret = UnpackUnSerialize(recv_buffer,transaction_id,packet,error_code);            
            if (ret != 0)
            {
                return ret;
            }

            detect_endpoint.address(boost::asio::ip::address_v4(packet.response.detected_ip_));
            detect_endpoint.port(packet.response.detect_udp_port_);
            query_times = packet.query_times_;
            send_nc_port = packet.response.send_nc_port_;

            //packet的response里的其他字段，tool里不再需要了。如果有需要，这里可以继续解析

            return 0;
        }

        int NatCheckPack::PackDiffIp(boost::uint32_t transaction_id,boost::uint16_t query_times,
            boost::uint32_t local_ip,boost::uint16_t local_port,boost::uint32_t peer_version,
            const boost::asio::ip::udp::endpoint& endpoint_,UdpBuffer & send_buffer)
        {
            protocol::NatCheckDiffIpPacket packet;
            packet.PacketAction = protocol::NatCheckDiffIpPacket::Action;
            packet.transaction_id_ = transaction_id;
            packet.query_times_ = query_times;
            packet.request.local_ip_ = local_ip;
            packet.request.local_port_ = local_port;
            packet.end_point = endpoint_;
            PackSerialize(send_buffer, protocol::NatCheckDiffIpPacket::Action, packet);
            return 0;
        }


        int NatCheckPack::UnPackDiffIp(UdpBuffer& recv_buffer, boost::uint32_t transaction_id, 
            boost::uint8_t& error_code,boost::uint16_t& query_times,boost::asio::ip::udp::endpoint& detect_endpoint
            ,boost::uint32_t& send_nc_ip,boost::uint16_t& send_nc_port)
        {
            protocol::NatCheckDiffIpPacket packet;
            int ret = UnpackUnSerialize(recv_buffer,transaction_id,packet,error_code);            
            if (ret != 0)
            {
                return ret;
            }

            detect_endpoint.address(boost::asio::ip::address_v4(packet.response.detected_ip_));
            detect_endpoint.port(packet.response.detect_udp_port_);
            query_times = packet.query_times_;

            //packet的response里的其他字段，tool里不再需要了。如果有需要，这里可以继续解析
            send_nc_ip = packet.response.send_nc_ip_;
            send_nc_port = packet.response.send_nc_port_;

            return 0;
        }

        

        std::string NatCheckPack::GetErrMsg()
        {
            return err_msg_.str();
        }
    }
}