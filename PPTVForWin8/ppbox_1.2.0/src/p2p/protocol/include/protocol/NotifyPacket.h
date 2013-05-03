//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

// NotifyPacket.h

#ifndef _PROTOCOL_NOTIFY_PACKET_H_
#define _PROTOCOL_NOTIFY_PACKET_H_

#include "protocol/Packet.h"
#include <util/serialization/stl/vector.h>
#include "struct/Structs.h"

namespace protocol
{
    //////////////////////////////////////////////////////////////////////////
    // JoinRequestPacket - 请求加入包协议
    struct JoinRequestPacket
        : PacketT<0xA0>
        , Packet
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            Packet::serialize(ar);
            ar & peer_guid_;
            ar & internal_ip_;
            ar & internal_port_;
            ar & detect_ip_;
            ar & detect_port_;
            ar & stun_ip_;
            ar & stun_port_;
            ar & nat_type_;
        }
        JoinRequestPacket()
        {
        }

        JoinRequestPacket(boost::uint32_t transaction_id, const Guid & peer_guid,
            boost::uint32_t internal_ip, boost::uint16_t internal_port, boost::uint32_t detect_ip,
            boost::uint16_t detect_port, boost::uint32_t stun_ip, boost::uint16_t stun_port, boost::uint16_t nat_type,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_guid_ = peer_guid;
            internal_ip_ = internal_ip;
            internal_port_ = internal_port;
            detect_ip_ = detect_ip;
            detect_port_ = detect_port;
            stun_ip_ = stun_ip;
            stun_port_ = stun_port;
            nat_type_ = nat_type;
            end_point = endpoint_;
            // PacketAction = Action;

        }

        Guid peer_guid_;
        boost::uint32_t internal_ip_;
        boost::uint16_t internal_port_;
        boost::uint32_t detect_ip_;
        boost::uint16_t detect_port_;
        boost::uint32_t stun_ip_;
        boost::uint16_t stun_port_;
        boost::uint16_t nat_type_;
    };

    //////////////////////////////////////////////////////////////////////////
    // JoinResponsePacket - 请求返回包协议
    struct JoinResponsePacket
        : PacketT<0xA1>
        , Packet
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            Packet::serialize(ar);
            ar & peer_guid_;
            ar & ret_;
            ar & util::serialization::make_sized<boost::uint8_t>(node_vec_);
        }

        JoinResponsePacket()
        {
        }
        JoinResponsePacket(boost::uint32_t transaction_id, const Guid & peer_guid,
            boost::uint8_t ret, const std::vector<NodeInfo>& node_vec,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_guid_ = peer_guid;
            ret_ = ret;
            node_vec_ = node_vec;
            end_point = endpoint_;
        }

        Guid peer_guid_;
        boost::uint8_t ret_;
        std::vector<NodeInfo> node_vec_;
    };

    //////////////////////////////////////////////////////////////////////////
    // NotifyKeepAliveRequestPacket - 心跳包协议
    struct NotifyKeepAliveRequestPacket
        : PacketT<0xA2>
        , Packet
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            Packet::serialize(ar);
            ar & peer_guid_;
            ar & peer_online_;
            ar & util::serialization::make_sized<boost::uint16_t>(task_info_);

        }

        NotifyKeepAliveRequestPacket()
        {
        }
        NotifyKeepAliveRequestPacket(boost::uint32_t transaction_id, const Guid & peer_guid,
            boost::uint32_t peer_online, const std::vector<TASK_INFO>& task_info,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_guid_ = peer_guid;
            peer_online_ = peer_online;
            task_info_ = task_info;
            end_point = endpoint_;
        }

        Guid peer_guid_;
        boost::uint32_t peer_online_;
        std::vector<TASK_INFO> task_info_;
    };

    //////////////////////////////////////////////////////////////////////////
    // NotifyKeepAliveResponsePacket - 心跳回包协议
    struct NotifyKeepAliveResponsePacket
        : PacketT<0xA3>
        , Packet
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            Packet::serialize(ar);
        }

        NotifyKeepAliveResponsePacket()
        {
        }
        NotifyKeepAliveResponsePacket(boost::uint32_t transaction_id, const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            end_point = endpoint_;
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // NotifyRequestPacket - 通知包协议
    struct NotifyRequestPacket
        : PacketT<0xA4>
        , Packet
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            Packet::serialize(ar);
            ar & task_id_;
            ar & duration_;
            ar & rest_time_;
            ar & task_type_;
            ar & util::serialization::make_sized<boost::uint16_t>(buffer_);
        }

        NotifyRequestPacket()
        {
        }
        NotifyRequestPacket(boost::uint32_t transaction_id, boost::uint32_t task_id, boost::uint16_t duration, 
            boost::int32_t rest_time, boost::uint16_t task_type, const std::string & buf, 
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            task_id_ = task_id;
            duration_ = duration;
            rest_time_ = rest_time;
            task_type_ = task_type;
            buffer_ = buf;
            end_point = endpoint_;
        }

        boost::uint32_t task_id_;
        boost::uint16_t duration_;
        boost::int32_t rest_time_;
        boost::uint16_t task_type_;
        std::string buffer_;

    };

    ///////////////////////////////////////////////////////////////////////////////////
    // NotifyResponsePacket - 通知回包协议
    struct NotifyResponsePacket
        : PacketT<0xA5>
        , Packet
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            Packet::serialize(ar);
            ar & peer_guid_;
            ar & task_id_;
        }

        NotifyResponsePacket()
        {
        }
        NotifyResponsePacket(boost::uint32_t transaction_id, const Guid & peer_guid, boost::uint32_t task_id,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_guid_ = peer_guid;
            task_id_ = task_id;
            end_point = endpoint_;
        }

        Guid peer_guid_;
        boost::uint32_t task_id_;
    };

    ///////////////////////////////////////////////////////////////////////////////////
    // PeerLeavePacket - 离线包协议
    struct PeerLeavePacket
        : PacketT<0xA6>
        , Packet
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            Packet::serialize(ar);
            ar & peer_guid_;
        }

        PeerLeavePacket()
        {
        }
        PeerLeavePacket(boost::uint32_t transaction_id, const Guid & peer_guid,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_guid_ = peer_guid;
            end_point = endpoint_;
        }

        Guid peer_guid_;
    };

    template <typename PacketHandler>
    inline void register_notify_packet(
        PacketHandler & handler)
    {
        handler.template register_packet<JoinRequestPacket>();
        handler.template register_packet<JoinResponsePacket>();
        handler.template register_packet<NotifyKeepAliveRequestPacket>();
        handler.template register_packet<NotifyKeepAliveResponsePacket>();
        handler.template register_packet<NotifyRequestPacket>();
        handler.template register_packet<NotifyResponsePacket>();
        handler.template register_packet<PeerLeavePacket>();
    }
}

#endif  // _PROTOCOL_NOTIFY_PACKET_H_
