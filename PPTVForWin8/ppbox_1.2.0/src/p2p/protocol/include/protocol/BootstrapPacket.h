//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef PROTOCOL_BOOSTRAPPACKET_H
#define PROTOCOL_BOOSTRAPPACKET_H

#include "protocol/ServerPacket.h"

#include <util/serialization/stl/vector.h>

namespace protocol
{
       /**
    *@brief Peer发向IndexServer的 QueryTrackerList 包
    * IndexServer回给Peer的 QueryTrackerList 包
    */
    struct QueryServerListPacket
        :public ServerPacketT<0x23>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest) {
                ar & request.peer_guid_;
                ar & request.region_info_;
                ar & util::serialization::make_sized<boost::uint8_t>(request.server_type_list_);
            } else {
                ar & response.region_info_;
                ar & util::serialization::make_sized<boost::uint8_t>(response.server_list_);
            }
        }

        QueryServerListPacket()
        {
            // IsRequest = 0;
        }

        // request
        QueryServerListPacket(
            boost::uint32_t transaction_id,
            boost::uint32_t peer_version,
            const Guid & peer_guid,
            const REGION_INFO& region_info,
            const std::vector<SERVER_TYPE_INFO>& server_type_list,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            request.peer_guid_ = peer_guid;
            request.region_info_ = region_info;
            request.server_type_list_ = server_type_list;
            end_point = endpoint_;
            IsRequest = 1;
        }

        // response
        QueryServerListPacket(
            boost::uint32_t transaction_id,
            const REGION_INFO& region_info,
            const std::vector<SERVER_LIST>& server_list,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
             response.region_info_ = region_info;
             response.server_list_ = server_list;
            end_point = endpoint_;
            IsRequest = 0;
        }
        QueryServerListPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            end_point = endpoint_;
            IsRequest = 0;
        }

        struct Response {
            REGION_INFO region_info_;
            std::vector<SERVER_LIST> server_list_;
        } response;
        struct Request {
            Guid peer_guid_;
            REGION_INFO region_info_;
            std::vector<SERVER_TYPE_INFO> server_type_list_;
        } request;


    private:
        QueryServerListPacket(QueryServerListPacket const &);
    };

    /**
    *@brief Peer发向IndexServer的 QueryTrackerList 包
    * IndexServer回给Peer的 QueryTrackerList 包
    */
    struct QueryTrackerListPacket
        :public ServerPacketT<0x14>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest) {
                ar & request.peer_guid_;
            } else {
                ar & response.tracker_group_count_
                    & util::serialization::make_sized<boost::uint16_t>(response.tracker_info_);
            }
        }

        QueryTrackerListPacket()
        {
            // IsRequest = 0;
        }

        // request
        QueryTrackerListPacket(
            boost::uint32_t transaction_id,
            boost::uint32_t peer_version,
            const Guid & peer_guid,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            request.peer_guid_ = peer_guid;
            end_point = endpoint_;
            IsRequest = 1;
        }

        // response
        QueryTrackerListPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            boost::uint16_t tracker_group_count,
            const std::vector<TRACKER_INFO> & tracker_info,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.tracker_group_count_ = tracker_group_count;
            response.tracker_info_ = tracker_info;
            end_point = endpoint_;
            IsRequest = 0;
        }

        struct Request {
            Guid peer_guid_;
        } request;
        struct Response {
            boost::uint16_t tracker_group_count_;
            std::vector<TRACKER_INFO> tracker_info_;
        } response;

    private:
        QueryTrackerListPacket(QueryTrackerListPacket const &);
    };

    /**
    *@brief Peer发向IndexServer的 QueryStunServerList 包
    * IndexServer回给Peer的 QueryStunServerList 包
    */
    struct QueryStunServerListPacket
        :public ServerPacketT<0x15>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest) {
            } else {
                ar & util::serialization::make_sized<boost::uint16_t>(response.stun_infos_);
            }
        }

        QueryStunServerListPacket()
        {
            // IsRequest = 0;
        }

        // request
        QueryStunServerListPacket(
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
        QueryStunServerListPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            const std::vector<STUN_SERVER_INFO> & stun_server_info,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.stun_infos_ = stun_server_info;
            end_point = endpoint_;
            IsRequest = 0;
        }
        QueryStunServerListPacket(std::vector<STUN_SERVER_INFO> & stun_server_info, boost::asio::ip::udp::endpoint & endpoint_)
        {
            QueryStunServerListPacket(0, 0, stun_server_info, endpoint_);
        }

        struct Request {
        } request;
        struct Response {
            std::vector<STUN_SERVER_INFO> stun_infos_;
        } response;

    private:
        QueryStunServerListPacket(QueryStunServerListPacket const &);
    };

    /**
    * 查询IndexServerList结果及其返回结果
    * @version
    */

    struct QueryIndexServerListPacket
        :public ServerPacketT<0x17>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest) {
                ar & request.peer_guid_;
            } else {
                ar & framework::container::make_array(response.mod_index_map_, INDEX_LIST_MOD_COUNT)
                    & util::serialization::make_sized<boost::uint8_t>(response.index_servers_);
            }
        }

        QueryIndexServerListPacket()
        {
            // IsRequest = 0;
        }

        // request
        QueryIndexServerListPacket(
            boost::uint32_t transaction_id,
            boost::uint32_t peer_version,
            const Guid & peer_guid,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            request.peer_guid_ = peer_guid;
            end_point = endpoint_;
            IsRequest = 1;
        }

        // response
        QueryIndexServerListPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            end_point = endpoint_;
            IsRequest = 0;
        }
        QueryIndexServerListPacket(
            boost::uint32_t transaction_id,
            const std::vector<boost::uint8_t>& mod_index_map,
            const std::vector<INDEX_SERVER_INFO>& index_servers,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            assert(mod_index_map.size() == INDEX_LIST_MOD_COUNT);
            transaction_id_ = transaction_id;
            
            memset(response.mod_index_map_, 0, INDEX_LIST_MOD_COUNT);

            for (boost::uint32_t i = 0; i < INDEX_LIST_MOD_COUNT && i < mod_index_map.size(); ++i)
            {
                response.mod_index_map_[i] = mod_index_map[i];
            }
			
            response.index_servers_ = index_servers;
            end_point = endpoint_;
            IsRequest = 0;
            error_code_ = 0;
        }

        struct Request {
            Guid peer_guid_;
        } request;
        struct Response {
            boost::uint8_t mod_index_map_[INDEX_LIST_MOD_COUNT];
            std::vector<INDEX_SERVER_INFO> index_servers_;
        } response;

    private:
        QueryIndexServerListPacket(QueryIndexServerListPacket const &);
    };

    /**
    *@brief Peer发向BootstrapServer的 QueryConfigString 包
    * BootstrapServer回给Peer的 QueryConfigString 包
    */
    struct QueryConfigStringPacket
        :public ServerPacketT<0x25>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest) {
            } else {
                ar & util::serialization::make_sized<boost::uint16_t>(response.config_string_);
            }
        }

        QueryConfigStringPacket()
        {
            // IsRequest = 0;
        }

        // request
        QueryConfigStringPacket(
            boost::uint32_t transaction_id,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            end_point = endpoint_;
            IsRequest = 1;
        }

        // response
        QueryConfigStringPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            const std::string & config_string,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.config_string_ = config_string;
            end_point = endpoint_;
            IsRequest = 0;
        }

        struct Response {
            std::string config_string_;
        } response;
    };

    /**
    *@brief Peer发向IndexServer的 QueryTrackerList 包
    * IndexServer回给Peer的 QueryTrackerList 包
    */
    struct QueryLiveTrackerListPacket
        :public ServerPacketT<0x26>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest) {
                ar & request.peer_guid_;
            } else {
                ar & response.tracker_group_count_
                    & util::serialization::make_sized<boost::uint16_t>(response.tracker_info_);
            }
        }

        QueryLiveTrackerListPacket()
        {
        }

        // request
        QueryLiveTrackerListPacket(
            boost::uint32_t transaction_id,
            boost::uint32_t peer_version,
            const Guid& peer_guid,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            request.peer_guid_ = peer_guid;
            end_point = endpoint_;
            IsRequest = 1;
        }

        // response
        QueryLiveTrackerListPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            boost::uint16_t tracker_group_count,
            const std::vector<TRACKER_INFO>& tracker_info,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.tracker_group_count_ = tracker_group_count;
            response.tracker_info_ = tracker_info;
            end_point = endpoint_;
            IsRequest = 0;
        }

        struct Request {
            Guid peer_guid_;
        } request;
        struct Response {
            boost::uint16_t tracker_group_count_;
            std::vector<TRACKER_INFO> tracker_info_;
        } response;
    };

    struct QuerySnListPacket
        : public ServerPacketT<0x27>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest)
            {
            }
            else
            {
                ar & util::serialization::make_sized<boost::uint16_t>(response.super_node_infos_);
            }
        }

        QuerySnListPacket()
        {

        }

        // request
        QuerySnListPacket(
            boost::uint32_t transaction_id,
            boost::uint16_t peer_version,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            end_point = endpoint_;
            IsRequest = 1;
        }

        // response
        QuerySnListPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            const std::vector<SuperNodeInfo>& super_node_infos,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.super_node_infos_ = super_node_infos;
            end_point = endpoint_;
            IsRequest = 0;
        }

        // response
        QuerySnListPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            end_point = endpoint_;
            IsRequest = 0;
        }

        struct Response
        {
            std::vector<SuperNodeInfo> super_node_infos_;
        } response;
    };

    /**
    *@brief Peer发向BootStrap的 QueryTrackerForListingPacket 包
    * BootStrap回给Peer的 QueryTrackerForListingPacket 包
    */
    struct QueryTrackerForListingPacket
        :public ServerPacketT<0x40>
    {
        enum TrackerType
        {
            VOD_TRACKER_FOR_LISTING = 0,
            LIVE_TRACKER_FOR_LISTING = 1,
            MAX_TRACKER_FOR_LISTING_TYPE = 2,
        };

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);

            ar & tracker_type_;

            if (!IsRequest)
            {
                ar & response.tracker_group_count_
                    & util::serialization::make_sized<boost::uint16_t>(response.tracker_info_);
            }

            assert(tracker_type_ < MAX_TRACKER_FOR_LISTING_TYPE);
        }

        QueryTrackerForListingPacket()
        {
        }

        // request
        QueryTrackerForListingPacket(
            boost::uint32_t transaction_id,
            boost::uint32_t peer_version,
            boost::uint8_t tracker_type,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            assert(tracker_type < MAX_TRACKER_FOR_LISTING_TYPE);

            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            tracker_type_ = tracker_type;
            end_point = endpoint_;
            IsRequest = 1;
        }

        // response
        QueryTrackerForListingPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            boost::uint8_t tracker_type,
            boost::uint16_t tracker_group_count,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            assert(tracker_type < MAX_TRACKER_FOR_LISTING_TYPE);

            transaction_id_ = transaction_id;
            error_code_ = error_code;
            tracker_type_ = tracker_type;
            response.tracker_group_count_ = tracker_group_count;
            end_point = endpoint_;
            IsRequest = 0;
        }

        // response
        QueryTrackerForListingPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            boost::uint8_t tracker_type,
            boost::uint16_t tracker_group_count,
            const std::vector<TRACKER_INFO> & tracker_info,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            assert(tracker_type < MAX_TRACKER_FOR_LISTING_TYPE);

            transaction_id_ = transaction_id;
            error_code_ = error_code;
            tracker_type_ = tracker_type;
            response.tracker_group_count_ = tracker_group_count;
            response.tracker_info_ = tracker_info;
            end_point = endpoint_;
            IsRequest = 0;
        }

        boost::uint8_t tracker_type_;

        struct Response
        {
            boost::uint16_t tracker_group_count_;
            std::vector<TRACKER_INFO> tracker_info_;
        } response;
    };

    struct QueryVipSnListPacket
        : public ServerPacketT<0x28>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest)
            {
            }
            else
            {
                ar & util::serialization::make_sized<boost::uint16_t>(response.super_node_infos_);
            }
        }

        QueryVipSnListPacket()
        {

        }

        // request
        QueryVipSnListPacket(
            boost::uint32_t transaction_id,
            boost::uint16_t peer_version,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            end_point = endpoint_;
            IsRequest = 1;
        }

        // response
        QueryVipSnListPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            const std::vector<SuperNodeInfo>& super_node_infos,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.super_node_infos_ = super_node_infos;
            end_point = endpoint_;
            IsRequest = 0;
        }

        // response
        QueryVipSnListPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            end_point = endpoint_;
            IsRequest = 0;
        }

        struct Response
        {
            std::vector<SuperNodeInfo> super_node_infos_;
        } response;
    };

    struct QueryUdpServerListPacket
        : public ServerPacketT<0x29>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest)
            {
            }
            else
            {
                ar & util::serialization::make_sized<boost::uint16_t>(response.udpserver_infos_);
            }
        }

        QueryUdpServerListPacket()
        {

        }

        // request
        QueryUdpServerListPacket(
            boost::uint32_t transaction_id,
            boost::uint16_t peer_version,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            end_point = endpoint_;
            IsRequest = 1;
        }

        // response
        QueryUdpServerListPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            const std::vector<UdpServerInfo>& udpserver_infos,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.udpserver_infos_ = udpserver_infos;
            end_point = endpoint_;
            IsRequest = 0;
        }

        // response
        QueryUdpServerListPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            end_point = endpoint_;
            IsRequest = 0;
        }

        struct Response
        {
            std::vector<UdpServerInfo> udpserver_infos_;
        } response;
    };

    template <typename PacketHandler>
    inline void register_bootstrap_packet(
        PacketHandler & handler)
    {
        handler.template register_packet<QueryServerListPacket>();
        handler.template register_packet<QueryTrackerListPacket>();
        handler.template register_packet<QueryStunServerListPacket>();
        handler.template register_packet<QueryIndexServerListPacket>();
        handler.template register_packet<QueryConfigStringPacket>();
        handler.template register_packet<QueryLiveTrackerListPacket>();
        handler.template register_packet<QuerySnListPacket>();
        handler.template register_packet<QueryTrackerForListingPacket>();
        handler.template register_packet<QueryVipSnListPacket>();
        handler.template register_packet<QueryUdpServerListPacket>();
    }

}

#endif  // PROTOCOL_BOOSTRAPPACKET_H
