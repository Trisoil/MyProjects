#ifndef PROTOCOL_BOOSTRAPPACKET_H
#define PROTOCOL_BOOSTRAPPACKET_H

#include <protocol/ServerPacket.h>

#include <util/serialization/stl/vector.h>

using namespace boost;

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
            //IsRequest = 0;
        }

        //request
        QueryServerListPacket(
            boost::uint32_t tid, 
            boost::uint32_t peer_version,
            Guid peer_guid,
            const REGION_INFO& region_info,
            const std::vector<SERVER_TYPE_INFO>& server_type_list,
            boost::asio::ip::udp::endpoint end_point)
        {
            transaction_id_ = tid;
            peer_version_ = peer_version;
            request.peer_guid_ = peer_guid;
            request.region_info_ = region_info;
            request.server_type_list_ = server_type_list;
            end_point_ = end_point;
            IsRequest = 1;
        }

        //response
        QueryServerListPacket(
            boost::uint32_t transaction_id,
            const REGION_INFO& region_info,
            const std::vector<SERVER_LIST>& server_list,
            boost::asio::ip::udp::endpoint endpoint_)
        {
            transaction_id_ = transaction_id;
             response.region_info_ = region_info;
             response.server_list_ = server_list;
            end_point_ = endpoint_;
            IsRequest = 0;
        }
        QueryServerListPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            boost::asio::ip::udp::endpoint endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            end_point_ = endpoint_;
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
            //IsRequest = 0;
        }

        //request
        QueryTrackerListPacket(
            boost::uint32_t transaction_id, 
            boost::uint32_t peer_version,
            Guid peer_guid,
            boost::asio::ip::udp::endpoint endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            request.peer_guid_ = peer_guid;
            end_point_ = endpoint_;
            IsRequest = 1;
        }

        //response
        QueryTrackerListPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            boost::uint16_t tracker_group_count,
            std::vector<TRACKER_INFO> tracker_info,
            boost::asio::ip::udp::endpoint endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.tracker_group_count_ = tracker_group_count;
            response.tracker_info_ = tracker_info;
            end_point_ = endpoint_;
            IsRequest = 0;
        }
        QueryTrackerListPacket(
            boost::uint16_t tracker_group_count,
            std::vector<TRACKER_INFO> tracker_info,
            boost::asio::ip::udp::endpoint endpoint_)
        {
            QueryTrackerListPacket(0, 0, tracker_group_count, tracker_info, endpoint_);
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
            //IsRequest = 0;
        }

        //request
        QueryStunServerListPacket(
            boost::uint32_t transaction_id, 
            boost::uint32_t peer_version,
            boost::asio::ip::udp::endpoint endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            end_point_ = endpoint_;
            IsRequest = 1;
        }

        //response
        QueryStunServerListPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            std::vector<STUN_SERVER_INFO> stun_server_info,
            boost::asio::ip::udp::endpoint endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.stun_infos_ = stun_server_info;
            end_point_ = endpoint_;
            IsRequest = 0;
        }
        QueryStunServerListPacket(std::vector<STUN_SERVER_INFO> stun_server_info, boost::asio::ip::udp::endpoint endpoint_)
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
            //IsRequest = 0;
        }

        //request
        QueryIndexServerListPacket(
            boost::uint32_t transaction_id, 
            boost::uint32_t peer_version,
            Guid peer_guid,
            boost::asio::ip::udp::endpoint endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            request.peer_guid_ = peer_guid;
            end_point_ = endpoint_;
            IsRequest = 1;
        }

        //response
        QueryIndexServerListPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            boost::asio::ip::udp::endpoint endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            end_point_ = endpoint_;
            IsRequest = 0;
        }
        QueryIndexServerListPacket(
            boost::uint32_t transaction_id,
            const std::vector<uint8_t>& mod_index_map,
            const std::vector<INDEX_SERVER_INFO>& index_servers,
            boost::asio::ip::udp::endpoint endpoint_)
        {
            assert(mod_index_map.size() == INDEX_LIST_MOD_COUNT);
            transaction_id = transaction_id;
            //response.mod_index_map_ = mod_index_map;
            for (uint32_t i = 0; i < INDEX_LIST_MOD_COUNT && i < mod_index_map.size(); ++i)
            {
                response.mod_index_map_[i] = mod_index_map[i];
            }
            response.index_servers_ = index_servers;
            end_point_ = endpoint_;
            IsRequest = 0;
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

    template <typename PacketHandler>
    inline void register_bootstrap_packet(
        PacketHandler & handler)
    {
        handler.template register_packet<QueryServerListPacket>();
        handler.template register_packet<QueryTrackerListPacket>();
        handler.template register_packet<QueryStunServerListPacket>();
        handler.template register_packet<QueryIndexServerListPacket>();
    }

}

#endif//PROTOCOL_BOOSTRAPPACKET_H
