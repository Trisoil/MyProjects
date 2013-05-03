//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _PROTOCOL_STATISTIC_PACKET_H_
#define _PROTOCOL_STATISTIC_PACKET_H_

#pragma once

#include "protocol/Packet.h"
#include "struct/Structs.h"

namespace protocol
{
    struct StatisticPacket
        : Packet
    {
        enum PEER_ACTION
        {
            PEER_ONLINE                 = 0x11,
            PEER_QUERY_URL              = 0x12,
            PEER_QUERY_CONTENT          = 0x13,
            PEER_QUERY_URL_SUCCESS      = 0x21,
            PEER_QUERY_CONTENT_SUCCESS  = 0x22,
            PEER_ADD_RESOURCE           = 0x23,
        };

        template <typename Archive>
        void serialize(Archive & ar)
        {
            Packet::serialize(ar);
            ar & peer_version_;
            ar & time_stamp_;
            ar & peer_guid_;
            ar & peer_action_;
        }
        StatisticPacket()
        {
        }
        boost::uint32_t peer_version_;
        boost::uint64_t time_stamp_;
        Guid peer_guid_;
        PEER_ACTION peer_action_;
    };

    template <boost::uint8_t action>
    struct StatisticPacketT
        : PacketT<action>
        , StatisticPacket
    {
    };

    /**
    *@brief StatisticIndexPeerPacket
    */
    struct StatisticIndexPeerPacket
        : StatisticPacketT<0xD1>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            StatisticPacket::serialize(ar);
        }

        StatisticIndexPeerPacket(boost::uint32_t transaction_id, boost::uint32_t peer_version, boost::uint64_t time_stamp,
            const Guid & peer_guid, const PEER_ACTION & peer_action, const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            time_stamp_ = time_stamp;
            peer_guid_ = peer_guid;
            peer_action_ = peer_action;
            end_point = endpoint_;
        }
    };

    /**
    *@brief StatisticIndexUrlPacket
    */
    struct StatisticIndexUrlPacket
        : StatisticPacketT<0xD2>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            StatisticPacket::serialize(ar);
            ar & statistic_index_url_;
        }

        StatisticIndexUrlPacket(boost::uint32_t transaction_id, boost::uint32_t peer_version, boost::uint64_t time_stamp,
            const Guid & peer_guid, const PEER_ACTION & peer_action, const std::string & url, 
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            time_stamp_ = time_stamp;
            peer_guid_ = peer_guid;
            peer_action_ = peer_action;
            statistic_index_url_ = url;
            end_point = endpoint_;
        }
        std::string statistic_index_url_;

    };

    /**
    *@brief StatisticIndexContentPacket
    */
    struct StatisticIndexContentPacket
        : StatisticPacketT<0xD3>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            StatisticPacket::serialize(ar);
            ar & content_id_;
        }

        StatisticIndexContentPacket(boost::uint32_t transaction_id, boost::uint32_t peer_version, 
            boost::uint64_t time_stamp, const Guid & peer_guid, const PEER_ACTION & peer_action, 
            const Guid & content_id, const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            time_stamp_ = time_stamp;
            peer_guid_ = peer_guid;
            peer_action_ = peer_action;
            content_id_ = content_id;
            end_point = endpoint_;
        }
        Guid content_id_;

    };

    /**
    *@brief StatisticIndexResourcePacket
    */
    struct StatisticIndexResourcePacket
        : StatisticPacketT<0xD4>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            StatisticPacket::serialize(ar);
            ar & resource_id_;
        }

        StatisticIndexResourcePacket(boost::uint32_t transaction_id, boost::uint32_t peer_version, 
            boost::uint64_t time_stamp, const Guid & peer_guid, const PEER_ACTION & peer_action, 
            const Guid & resource_id, const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            time_stamp_ = time_stamp;
            peer_guid_ = peer_guid;
            peer_action_ = peer_action;
            resource_id_ = resource_id;
            end_point = endpoint_;
        }
        Guid resource_id_;

    };

    /**
    *@brief StatisticStringPacket
    */
    struct StatisticStringPacket
        : StatisticPacketT<0xD5>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            StatisticPacket::serialize(ar);
            ar & data_;
        }

        StatisticStringPacket(boost::uint32_t transaction_id, boost::uint32_t peer_version, boost::uint64_t time_stamp,
            const Guid & peer_guid, const PEER_ACTION & peer_action, const std::string & data, 
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            time_stamp_ = time_stamp;
            peer_guid_ = peer_guid;
            peer_action_ = peer_action;
            data_ = data;
            end_point = endpoint_;
        }
        std::string data_;

    };
}

#endif  // _PROTOCOL_STATISTIC_PACKET_H_
