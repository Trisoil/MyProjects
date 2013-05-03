//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

// StunServerPacket.h

#ifndef _PROTOCOL_STUN_SERVER_PACKET_H_
#define _PROTOCOL_STUN_SERVER_PACKET_H_

#include "protocol/ServerPacket.h"

namespace protocol
{
    /**
    *@brief Peer发向StunServer的 QueryStunServerList 包 和
    * StunServer回给Peer的 StunHandShakeResponsePacket包
    */
    struct StunHandShakePacket
        : public ServerPacketT<0x71>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest) {
            } else {
                ar & response.keep_alive_interval_;
                ar & response.detected_ip_;
                ar & response.detect_udp_port_;
            }
        }

        StunHandShakePacket()
        {
            // IsRequest = 0;
        }

        // request
        StunHandShakePacket(
            boost::uint32_t transaction_id,
            boost::uint32_t peer_version,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            end_point = endpoint_;
            IsRequest = 1;
        }

        // response
        StunHandShakePacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            boost::uint16_t keepalive_interval,
            boost::uint32_t detected_ip,
            boost::uint16_t detected_udp_port,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.keep_alive_interval_ = keepalive_interval;
            response.detected_ip_ = detected_ip;
            response.detect_udp_port_ = detected_udp_port;
            end_point = endpoint_;
            IsRequest = 0;
        }
        StunHandShakePacket(
            boost::uint16_t keepalive_interval,
            boost::uint32_t detected_ip,
            boost::uint16_t detected_udp_port,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            StunHandShakePacket(0, 0, keepalive_interval, detected_ip, detected_udp_port, endpoint_);
        }

        struct Request {
        } request;
        struct Response {
            boost::uint16_t keep_alive_interval_;
            boost::uint32_t detected_ip_;
            boost::uint16_t detect_udp_port_;
        } response;

    };


    /**
    *@brief Peer发向StunServer的 StunKPLPacket 包
    */

    struct StunKPLPacket
        :public ServerPacketT<0x72>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
        }

        StunKPLPacket(
            boost::uint32_t transaction_id,
            boost::uint32_t peer_version,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            end_point = endpoint_;
            IsRequest = 1;
        }

        StunKPLPacket()
        {
            IsRequest = 1;
        }

    };


    /**
    *@brief Peer发向StunServer的 StunInvoke 包
    */

    struct StunInvokePacket
        : public ServerPacketT<0x73>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            ar & resource_id_;
            ar & peer_guid_;
            ar & send_off_time_;
            ar & candidate_peer_info_mine_;
            ar & connect_type_;
            ar & candidate_peer_info_his_;
            ar & peer_download_info_mine_;
            ar & ip_pool_size_;
        }

        StunInvokePacket()
        {
            IsRequest = 1;
        }

        StunInvokePacket(
            boost::uint32_t transaction_id,
            const RID & resource_id,
            const Guid & peer_guid,
            boost::uint32_t send_off_time,
            const CandidatePeerInfo & peer_info_mine,
            boost::uint8_t connect_type,
            const CandidatePeerInfo & peer_info_his,
            const PEER_DOWNLOAD_INFO & peer_download_info,
            boost::uint16_t ip_pool_size,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            resource_id_ = resource_id;
            peer_guid_ = peer_guid;
            send_off_time_ = send_off_time;
            candidate_peer_info_mine_ = peer_info_mine;
            connect_type_ = connect_type;
            candidate_peer_info_his_ = peer_info_his;
            peer_download_info_mine_ = peer_download_info;
            ip_pool_size_ = ip_pool_size;
            end_point = endpoint_;
            IsRequest = 1;
        }

        RID resource_id_;
        Guid peer_guid_;
        boost::uint32_t send_off_time_;
        CandidatePeerInfo candidate_peer_info_mine_;
        boost::uint8_t connect_type_;
        CandidatePeerInfo candidate_peer_info_his_;
        PEER_DOWNLOAD_INFO peer_download_info_mine_;
        boost::uint16_t ip_pool_size_;
    };


    template <typename PacketHandler>
    void register_stun_packet(
        PacketHandler & handler)
    {
        handler.template register_packet<StunHandShakePacket>();
        handler.template register_packet<StunKPLPacket>();
        handler.template register_packet<StunInvokePacket>();
    }

}

#endif  // _PROTOCOL_STUN_SERVER_PACKET_H_
