//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------


/**
* @file
* @brief Peer和IndexServer相关协议包相关的基类
*/

#ifndef _PROTOCOL_TRACKET_PACKET_H_
#define _PROTOCOL_TRACKET_PACKET_H_

#include "protocol/ServerPacket.h"

#include <util/serialization/stl/vector.h>

namespace protocol
{
    //查询guid对应的tracker分组的算法
    inline boost::uint32_t GetGuidMod(const Guid& guid, boost::uint32_t mod)
    {
        boost::uint64_t buf[2];
        memcpy(&buf, &guid.data(), sizeof(guid.data()));
        buf[1] = framework::system::BytesOrder::little_endian_to_host_longlong(buf[1]);
        return static_cast<boost::uint32_t> (buf[1] % mod);
    }

    /**
    *@brief Peer发向TrackerServer的 List 包和TrackerServer回给Peer的 List 包
    */
    struct ListPacket
        : public ServerPacketT<0x31>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {

            ServerPacket::serialize(ar);
            if (IsRequest) {
                ar & request.resource_id_;
                ar & request.peer_guid_;
                ar & request.request_peer_count_;

                //PEER_VERSION_V12 新添加的字段，
                if (Archive::is_loading::value && peer_version_ < PEER_VERSION_V12)
                {
                    request.peer_nat_type_ = TYPE_ERROR;
                }
                else
                {
                    ar & request.peer_nat_type_;
                }

            } else{
                ar & response.resource_id_;
                ar & util::serialization::make_sized<boost::uint16_t>(response.peer_infos_);
            }
        }

        ListPacket()
        {
            // IsRequest = 1;
        }

		virtual boost::uint32_t length() const
		{
			boost::uint32_t length = ServerPacket::length();
			if (IsRequest) {
				length += sizeof(request.resource_id_) + sizeof(request.peer_guid_) + sizeof(request.request_peer_count_) + sizeof(request.peer_nat_type_);
			} else {
				length += sizeof(response.resource_id_) + sizeof(boost::uint16_t) + response.peer_infos_.size()*CandidatePeerInfo::length();
			}

			return length;
		}

        // request
        ListPacket(
            boost::uint32_t transaction_id,
            boost::uint32_t peer_version,
            const RID & resource_id,
            const Guid & peer_guid,
            boost::uint16_t request_peer_count,
            const boost::asio::ip::udp::endpoint & endpoint_,
            boost::uint8_t nat_type = TYPE_ERROR)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            request.resource_id_ = resource_id;
            request.peer_guid_ = peer_guid;
            request.request_peer_count_ = request_peer_count;
            end_point = endpoint_;
            request.peer_nat_type_ = nat_type;
            IsRequest = 1;
        }

        ListPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            const RID & resource_id,
            const std::vector<CandidatePeerInfo> & candidate_peer_info,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.resource_id_ = resource_id;
            response.peer_infos_=candidate_peer_info;
            end_point = endpoint_;
            IsRequest = 0;
        }

        //这个构造函数需要另外设置peer_infos_
        ListPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            const RID& resource_id,            
            const boost::asio::ip::udp::endpoint& endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.resource_id_ = resource_id;           
            end_point = endpoint_;
            IsRequest = 0;
        }

        struct Request {
            RID resource_id_;
            Guid peer_guid_;
            boost::uint16_t request_peer_count_;
            boost::uint8_t  peer_nat_type_;
        } request;
        struct Response {
            RID resource_id_;
            std::vector<CandidatePeerInfo> peer_infos_;
        } response;
    };

    /**
    *@brief Peer发向TrackerServer的 Leave 包
    */
    struct LeavePacket
        : public ServerPacketT<0x34>
    {
        LeavePacket(
            boost::uint32_t transaction_id,
            boost::uint32_t peer_version,
            const Guid & peer_guid,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            peer_guid_ = peer_guid;
            end_point = endpoint_;
            IsRequest = 1;
        }

        LeavePacket()
        {
            IsRequest = 1;
        }

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            ar & peer_guid_;
        }

		virtual boost::uint32_t length() const
		{
			boost::uint32_t length = ServerPacket::length();
			length += sizeof(peer_guid_);

			return length;
		}

        Guid peer_guid_;
    };

    /**
    *@brief Peer发向TrackerServer的 Report 包和TrackerServer回给Peer的 KeepAlive 包
    */
    struct ReportPacket
        : public ServerPacketT<0x35>
    {
        ReportPacket()
        {
        }

        // request
        ReportPacket(
            boost::uint32_t transaction_id,
            boost::uint32_t peer_version,
            const Guid & peer_guid,
            boost::uint16_t local_resource_count,
            boost::uint16_t server_resource_count,
            boost::uint16_t udp_port,
            boost::uint32_t stun_peer_ip,
            boost::uint16_t stun_peer_udpport,
            boost::uint32_t stun_detected_ip,
            boost::uint16_t stun_detected_udp_port,
            const std::vector<boost::uint32_t> & real_ips,
            const std::vector<REPORT_RESOURCE_STRUCT> & resource_ids,
            boost::uint8_t nat_type,
            boost::uint8_t upload_priority,
            boost::uint8_t idle_time_in_mins,
            boost::int32_t upload_bandwidth_kbs,
            boost::int32_t upload_limit_kbs,
            boost::int32_t upload_speed_kbs,
            const boost::asio::ip::udp::endpoint & endpoint_,
            boost::uint16_t internal_tcp_port,
            boost::uint16_t upnp_tcp_port,
            const std::vector<boost::uint32_t>& traceroute_ips)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            request.peer_guid_ = peer_guid;
            request.local_resource_count_ = local_resource_count;
            request.server_resource_count_ = server_resource_count;
            request.udp_port_ = udp_port;
            request.internal_tcp_port_ = internal_tcp_port;
            request.upnp_tcp_port_ = upnp_tcp_port;
            request.stun_peer_ip_ = stun_peer_ip;
            request.stun_peer_udp_port_ = stun_peer_udpport;
            request.stun_detected_ip_ = stun_detected_ip;
            request.stun_detected_udp_port_ = stun_detected_udp_port;
            request.real_ips_ = real_ips;
            request.resource_ids_ = resource_ids;
            request.peer_nat_type_ = nat_type;
            request.upload_priority_ = upload_priority;
            request.idle_time_in_mins_ = idle_time_in_mins;
            request.upload_bandwidth_kbs_ = upload_bandwidth_kbs;
            request.upload_limit_kbs_ = upload_limit_kbs;
            request.upload_speed_kbs_ = upload_speed_kbs;
            end_point = endpoint_;
            IsRequest = 1;
        }
        // response
        ReportPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            boost::uint16_t keepalive_interval,
            boost::uint32_t detected_ip,
            boost::uint16_t detected_udp_port,
            boost::uint16_t resource_count,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.keep_alive_interval_ = keepalive_interval;
            response.detected_ip_ = detected_ip;
            response.detected_udp_port_ = detected_udp_port;
            response.resource_count_ = resource_count;
            end_point = endpoint_;
            IsRequest = 0;
        }

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest){
                ar & request.peer_guid_;
                ar & request.local_resource_count_;
                ar & request.server_resource_count_;
                ar & request.peer_nat_type_;
                ar & request.upload_priority_;
                ar & request.idle_time_in_mins_;
                ar & request.reversed_;
                ar & request.internal_tcp_port_;
                ar & request.udp_port_;
                ar & request.upnp_tcp_port_;
                ar & request.stun_peer_ip_;
                ar & request.stun_peer_udp_port_;
                ar & request.stun_detected_udp_port_;

                if (Archive::is_loading::value && peer_version_ < PEER_VERSION_V6)
                {
                    // 旧版本协议中没有这几个字段，反序列化的时候设置为-1
                    request.upload_bandwidth_kbs_ = -1;
                    request.upload_limit_kbs_ = -1;
                    request.upload_speed_kbs_ = -1;
                }
                else
                {
                    ar & request.upload_bandwidth_kbs_;
                    ar & request.upload_limit_kbs_;
                    ar & request.upload_speed_kbs_;
                }

                ar & util::serialization::make_sized<boost::uint8_t>(request.real_ips_);
                ar & util::serialization::make_sized<boost::uint8_t>(request.resource_ids_);

                if (Archive::is_loading::value && peer_version_ < PEER_VERSION_V11)
                {
                    request.stun_detected_ip_ = 0;
                }
                else
                {
                    ar & request.stun_detected_ip_;
                }

                //PEER_VERSION_V12新添加，到tracker的tracerouteip，计划5个就够了。用于做精细化的p4p，例如小区内的节点返回
                if (Archive::is_loading::value && peer_version_ < PEER_VERSION_V12)
                {
                    request.traceroute_ips_.clear();
                }
                else
                {
                     ar & util::serialization::make_sized<boost::uint8_t>(request.traceroute_ips_);
                }

            }else{
                ar & response.keep_alive_interval_;
                ar & response.detected_ip_;
                ar & response.detected_udp_port_;
                ar & response.resource_count_;
            }
        }

		virtual boost::uint32_t length() const
		{
			boost::uint32_t length = ServerPacket::length();
			if (IsRequest){
				length += sizeof(request.peer_guid_);
				length += sizeof(request.local_resource_count_);
				length += sizeof(request.server_resource_count_);
				length += sizeof(request.peer_nat_type_);
				length += sizeof(request.upload_priority_);
				length += sizeof(request.idle_time_in_mins_);
				length += sizeof(request.reversed_);
				length += sizeof(request.internal_tcp_port_);
				length += sizeof(request.udp_port_);
				length += sizeof(request.upnp_tcp_port_);
				length += sizeof(request.stun_peer_ip_);
				length += sizeof(request.stun_peer_udp_port_);
				length += sizeof(request.stun_detected_udp_port_);
			    length += sizeof(request.upload_bandwidth_kbs_);
				length += sizeof(request.upload_limit_kbs_);
				length += sizeof(request.upload_speed_kbs_);
				length += sizeof(boost::uint8_t) + request.real_ips_.size()*sizeof(boost::uint32_t);
				length += sizeof(boost::uint8_t) + request.resource_ids_.size()*sizeof(REPORT_RESOURCE_STRUCT);
                length += sizeof(boost::uint8_t) + request.traceroute_ips_.size()*sizeof(boost::uint32_t);
			}else{
				length += sizeof(response.keep_alive_interval_) + sizeof(response.detected_ip_) + sizeof(response.detected_udp_port_) + sizeof(response.resource_count_);
			}

			return length;
		}		

        struct Request {
            Guid peer_guid_;
            boost::uint16_t local_resource_count_;
            boost::uint16_t server_resource_count_;
            boost::uint8_t  peer_nat_type_;
            boost::uint8_t  upload_priority_;
            boost::uint8_t  idle_time_in_mins_;
            boost::uint8_t  reversed_;
            boost::uint16_t  internal_tcp_port_;
            boost::uint16_t udp_port_;
            boost::uint16_t upnp_tcp_port_;
            boost::uint32_t stun_peer_ip_;
            boost::uint16_t stun_peer_udp_port_;
            boost::uint16_t stun_detected_udp_port_;
            boost::int32_t upload_bandwidth_kbs_; // 上传带宽
            boost::int32_t upload_limit_kbs_;     // 当前上传限速
            boost::int32_t upload_speed_kbs_;     // 当前上传速度
            std::vector<boost::uint32_t> real_ips_;
            std::vector<REPORT_RESOURCE_STRUCT> resource_ids_;
            boost::uint32_t stun_detected_ip_;
            std::vector<boost::uint32_t> traceroute_ips_;

        } request;
        struct Response {
            boost::uint16_t keep_alive_interval_;
            boost::uint32_t detected_ip_;
            boost::uint16_t detected_udp_port_;
            boost::uint16_t resource_count_;
        } response;
    };

   /**
    *@brief Peer发向TrackerServer的 FlashReport 包和TrackerServer回给Peer的 KeepAlive 包
    */
    struct FlashReportPacket
        : public ServerPacketT<0x3f>
    {
		boost::uint64_t reserve_1;
		boost::uint64_t reserve_2;

        FlashReportPacket()
        {
        }

        // request
        FlashReportPacket(
            boost::uint32_t transaction_id,
            boost::uint16_t peer_version,
			const std::string & flash_id,
            boost::uint16_t local_resource_count,
			boost::uint32_t reporter_ip,
            boost::uint16_t reporter_port,
			const std::vector<REPORT_RESOURCE_STRUCT> & resource_ids,
			const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            request.flash_id_ = flash_id;
            request.local_resource_count_ = local_resource_count;
			request.reporter_ip_ = reporter_ip;
            request.reporter_port_ = reporter_port;
			request.resource_ids_ = resource_ids;
            end_point = endpoint_;
            IsRequest = 1;
        }
        // response
        FlashReportPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            boost::uint16_t keepalive_interval,
            boost::uint16_t resource_count,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.keep_alive_interval_ = keepalive_interval;
            response.resource_count_ = resource_count;
            end_point = endpoint_;
            IsRequest = 0;
        }

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
			ar & reserve_1;
			ar & reserve_2;
            if (IsRequest){
                ar & util::serialization::make_sized<boost::uint16_t>(request.flash_id_);
                ar & request.local_resource_count_;
                ar & request.reporter_ip_;
				ar & request.reporter_port_;
				ar & util::serialization::make_sized<boost::uint8_t>(request.resource_ids_);
            }else{
                ar & response.keep_alive_interval_;
                ar & response.resource_count_;
            }
        }

		virtual boost::uint32_t length() const
		{
			boost::uint32_t length = ServerPacket::length();
			length += sizeof(reserve_1) + sizeof(reserve_2);
			if (IsRequest)
            {
                length += sizeof(boost::uint16_t) + request.flash_id_.size();
				length += sizeof(request.local_resource_count_);
				length += sizeof(request.reporter_ip_);
				length += sizeof(request.reporter_port_);
				length += sizeof(boost::uint8_t) + request.resource_ids_.size()*sizeof(REPORT_RESOURCE_STRUCT);
			}else{
				length += sizeof(response.keep_alive_interval_) + sizeof(response.resource_count_);
			}

			return length;
		}		

        struct Request {
			std::string flash_id_;
            boost::uint16_t local_resource_count_;
            boost::uint32_t reporter_ip_;
            boost::uint16_t reporter_port_;
            std::vector<REPORT_RESOURCE_STRUCT> resource_ids_;
        } request;
        struct Response {
            boost::uint16_t keep_alive_interval_;
            boost::uint16_t resource_count_;
        } response;
    };

    /**
    * @brief Peer向TrackerServer查询资源拥有的PeerCount 和 Tracker向Peer发送查询结果
    */
    struct QueryPeerCountPacket
        : public ServerPacketT<0x36>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest)
            {
                ar & request.resource_id_;
            }
            else{
                ar & response.resource_id_;
                ar & response.peer_count_;
            }
        }

		virtual boost::uint32_t length() const
		{
			boost::uint32_t length = ServerPacket::length();
			if (IsRequest)
			{
				length += sizeof(request.resource_id_);
			}
			else{
				length += sizeof(response.resource_id_);
				length += sizeof(response.peer_count_);
			}

			return length;
		}

        QueryPeerCountPacket()
        {
            // IsRequest = 0;
        }

        // request
        QueryPeerCountPacket(
            boost::uint32_t transaction_id,
            boost::uint32_t peer_version,
            const Guid & resource_id,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            request.resource_id_ = resource_id;
            end_point = endpoint_;
            IsRequest = 1;
        }

        // response
        QueryPeerCountPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            const Guid & resource_id,
            boost::uint32_t peer_count,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.resource_id_ = resource_id;
            response.peer_count_ = peer_count;
            end_point = endpoint_;
            IsRequest = 0;

        }
        struct Request {
            Guid resource_id_;
        } request;
        struct Response {
            Guid resource_id_;
            boost::uint32_t peer_count_;
        } response;

    };

    struct InternalCommandPacket
//        : Packet
//        , PacketT<0x38>
          : public ServerPacketT<0x38>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if(IsRequest)
            {
                ar & magic_number_;
                ar & command_id_;
                ar & reserved_;
            }
        }

		virtual boost::uint32_t length() const
		{
			boost::uint32_t length = ServerPacket::length();
			if(IsRequest)
			{
				length += sizeof(magic_number_) + sizeof(command_id_) + sizeof(reserved_);
			}

			return length;
		}

        InternalCommandPacket()
        {
            reserved_ = 0;
        }

        InternalCommandPacket(
            boost::uint32_t transaction_id,
            boost::uint16_t magic_number,
            boost::uint16_t command_id,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            magic_number_ = magic_number;
            command_id_ = command_id;
            end_point = endpoint_;
            reserved_ = 0;
            IsRequest = 1;
        }

        InternalCommandPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            const boost::asio::ip::udp::endpoint& endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            end_point = endpoint_;
            IsRequest = 0;
        }

        boost::uint16_t magic_number_;
        boost::uint16_t command_id_;
        boost::uint32_t reserved_;
    };

    //ck添加，查询一个peer拥有的rid，注意，这里不是所有的rid，因为一个peer的不同rid是在不同组的tracker上的。
    struct QueryPeerResourcesPacket
        : public ServerPacketT<0x39>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest)
            {
                ar & request.peer_guid_;
            }
            else{
                ar & response.peer_guid_;
                // ar & response.resources_;
                ar & util::serialization::make_sized<boost::uint8_t>(response.resources_);
            }
        }

		virtual boost::uint32_t length() const
		{
			boost::uint32_t length = ServerPacket::length();
			if (IsRequest)
			{
				length += sizeof(request.peer_guid_);
			}
			else{
				length += sizeof(response.peer_guid_);
				length += sizeof(boost::uint8_t) + response.resources_.size()*sizeof(RID);
			}

			return length;
		}

        QueryPeerResourcesPacket()
        {
            // IsRequest = 0;
        }

        // request
        QueryPeerResourcesPacket(
            boost::uint32_t transaction_id,
            boost::uint32_t peer_version,
            const Guid& peer_guid,
            const boost::asio::ip::udp::endpoint& endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            request.peer_guid_ = peer_guid;
            end_point = endpoint_;
            IsRequest = 1;
        }

        // response
        QueryPeerResourcesPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            const Guid& peer_guid,
            const std::vector<RID>& resources,
            const boost::asio::ip::udp::endpoint& endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.peer_guid_ = peer_guid;
            response.resources_ = resources;
            end_point = endpoint_;
            IsRequest = 0;

        }
        struct Request {
            Guid peer_guid_;
        } request;
        struct Response {
            Guid peer_guid_;
            std::vector<RID> resources_;           
        } response;

    };

    struct QueryTrackerStatisticPacket
        : public ServerPacketT<0x3a>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ServerPacket::serialize(ar);
            if (IsRequest)
            {
                //请求没有包体
            }
            else{
                // ar & response.tracker_statistic_;				
                ar & util::serialization::make_sized<boost::uint16_t>(response.tracker_statistic_);
            }
        }

		virtual boost::uint32_t length() const
		{
			boost::uint32_t length = ServerPacket::length();
			if (IsRequest)
			{
				//请求没有包体
			}
			else{				
				length += sizeof(boost::uint16_t) + sizeof(boost::uint16_t) + response.tracker_statistic_.size();
			}

			return length;
		}

        QueryTrackerStatisticPacket()
        {
            // IsRequest = 0;
        }

        // request
        QueryTrackerStatisticPacket(
            boost::uint32_t transaction_id,
            boost::uint32_t peer_version,
            const boost::asio::ip::udp::endpoint& endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            end_point = endpoint_;
            IsRequest = 1;
        }

        // response
        QueryTrackerStatisticPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            const std::string& tracker_statistic,
            const boost::asio::ip::udp::endpoint& endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.tracker_statistic_ = tracker_statistic;
            end_point = endpoint_;
            IsRequest = 0;

        }
        struct Request {			
        } request;
        struct Response {
            //用一个string作为回复，简单，易扩展，一了百了。
            std::string tracker_statistic_;
        } response;

    };


    struct ListTcpPacket
        : public ServerPacketT<0x3b>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {

            ServerPacket::serialize(ar);
            if (IsRequest) {
                ar & request.resource_id_;
                ar & request.peer_guid_;
                ar & request.request_peer_count_;
            } else{
                ar & response.resource_id_;
                ar & util::serialization::make_sized<boost::uint16_t>(response.peer_infos_);
            }
        }

		virtual boost::uint32_t length() const
		{
			boost::uint32_t length = ServerPacket::length();
			if (IsRequest) {
				length += sizeof(request.resource_id_) + sizeof(request.peer_guid_) + sizeof(request.request_peer_count_);
			} else{
				length += sizeof(response.resource_id_);
				length += sizeof(boost::uint16_t) + response.peer_infos_.size()*CandidatePeerInfo::length();
			}

			return length;
		}

        ListTcpPacket()
        {
            // IsRequest = 1;
        }

        // request
        ListTcpPacket(
            boost::uint32_t transaction_id,
            boost::uint32_t peer_version,
            const RID & resource_id,
            const Guid & peer_guid,
            boost::uint16_t request_peer_count,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            request.resource_id_ = resource_id;
            request.peer_guid_ = peer_guid;
            request.request_peer_count_ = request_peer_count;
            end_point = endpoint_;
            IsRequest = 1;

        }

        // response
        ListTcpPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            const RID& resource_id,
            const std::vector<CandidatePeerInfo>& candidate_peer_info,
            const boost::asio::ip::udp::endpoint& endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.resource_id_ = resource_id;
            response.peer_infos_ = candidate_peer_info;
            end_point = endpoint_;
            IsRequest = 0;
        }
        ListTcpPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            const RID& resource_id,            
            const boost::asio::ip::udp::endpoint& endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.resource_id_ = resource_id;
            //response.peer_infos_ = candidate_peer_info;
            end_point = endpoint_;
            IsRequest = 0;
        }

        struct Request {
            RID resource_id_;
            Guid peer_guid_;
            boost::uint16_t request_peer_count_;
        } request;
        struct Response {
            RID resource_id_;
            std::vector<CandidatePeerInfo> peer_infos_;
        } response;
    };


    //和list不同的是，这里制定了客户端的ip，而不是从socket里获取
    struct ListWithIpPacket
        : public ServerPacketT<0x3c>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {

            ServerPacket::serialize(ar);
            if (IsRequest) {
                ar & request.resource_id_;
                ar & request.peer_guid_;
                ar & request.request_peer_count_;
                ar & request.reqest_ip_;
                //PEER_VERSION_V12 新添加的字段，可以根据nat类型来决定返回，例如外网节点多返回内网，内网节点多返回外网
                if (Archive::is_loading::value && peer_version_ < PEER_VERSION_V12)
                {
                    request.peer_nat_type_ = TYPE_ERROR;
                }
                else
                {
                    ar & request.peer_nat_type_;
                }
            } else{
                ar & response.resource_id_;
                ar & util::serialization::make_sized<boost::uint16_t>(response.peer_infos_);
            }
        }

		virtual boost::uint32_t length() const
		{
			boost::uint32_t length = ServerPacket::length();
			if (IsRequest) {
				length += sizeof(request.resource_id_) + sizeof(request.request_peer_count_) + 
                    sizeof(request.reqest_ip_)+sizeof(request.peer_nat_type_);
			} else{
				length += sizeof(response.resource_id_);
				length += sizeof(boost::uint16_t) + response.peer_infos_.size()*CandidatePeerInfo::length();
			}

			return length;
		}

        ListWithIpPacket()
        {
            // IsRequest = 1;
        }

        // request
        ListWithIpPacket(
            boost::uint32_t transaction_id,
            boost::uint32_t peer_version,
            const RID & resource_id,
            const Guid & peer_guid,
            boost::uint16_t request_peer_count,
            boost::uint32_t reqest_ip,
            const boost::asio::ip::udp::endpoint & endpoint_,
            boost::uint8_t nat_type = TYPE_ERROR)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            request.resource_id_ = resource_id;
            request.peer_guid_ = peer_guid;
            request.request_peer_count_ = request_peer_count;
            request.reqest_ip_ = reqest_ip;
            end_point = endpoint_;
            request.peer_nat_type_ = nat_type;
            IsRequest = 1;
        }

        // response
        ListWithIpPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            const RID& resource_id,
            const std::vector<CandidatePeerInfo>& candidate_peer_info,
            const boost::asio::ip::udp::endpoint& endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.resource_id_ = resource_id;
            response.peer_infos_ = candidate_peer_info;
            end_point = endpoint_;
            IsRequest = 0;
        }

        ListWithIpPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            const RID& resource_id,            
            const boost::asio::ip::udp::endpoint& endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.resource_id_ = resource_id;            
            end_point = endpoint_;
            IsRequest = 0;
        }

        struct Request {
            RID resource_id_;
            Guid peer_guid_;
            boost::uint16_t request_peer_count_;
            boost::uint32_t reqest_ip_;
            boost::uint8_t peer_nat_type_;
        } request;
        struct Response {
            RID resource_id_;
            std::vector<CandidatePeerInfo> peer_infos_;
        } response;
    };

	//和list不同的是，这里制定了客户端的ip，而不是从socket里获取
	struct FlashListWithIpPacket
		: public ServerPacketT<0x3e>
	{
		boost::uint64_t  reserve_1;
        boost::uint64_t  reserve_2;
		template <typename Archive>
		void serialize(Archive & ar)
		{

			ServerPacket::serialize(ar);
			ar & reserve_1;
			ar & reserve_2;

			if (IsRequest)
			{
				ar & request.resource_id_;
				ar & util::serialization::make_sized<boost::uint16_t>(request.flash_id_);
				ar & request.request_peer_count_;
				ar & request.reqest_ip_;
			} 
			else
			{
				ar & response.resource_id_;
				ar & util::serialization::make_sized<boost::uint16_t>(response.peer_infos_);
			}

		}

		virtual boost::uint32_t length() const
		{
			boost::uint32_t len = ServerPacket::length() + sizeof(reserve_1)+sizeof(reserve_2);
			if (IsRequest) {
				len += sizeof(boost::uint16_t)+ request.flash_id_.size()
                    + sizeof(request.resource_id_) + sizeof(request.request_peer_count_); 
			} else{
				len += sizeof(response.resource_id_);
				len += sizeof(boost::uint16_t);
				for (unsigned i = 0; i < response.peer_infos_.size(); ++i)
				{
					len += ((response.peer_infos_)[i]).length();
				}
				
			}

			return len;
		}

		FlashListWithIpPacket()
		{
			// IsRequest = 1;
		}

		// request
		FlashListWithIpPacket(
			boost::uint32_t transaction_id,
			boost::uint32_t peer_version,
			const RID & resource_id,
			const std::string & flash_id,
			boost::uint16_t request_peer_count,
			boost::uint32_t reqest_ip,
			const boost::asio::ip::udp::endpoint & endpoint_)
		{
			transaction_id_ = transaction_id;
			peer_version_ = peer_version;
			request.resource_id_ = resource_id;
			request.flash_id_ = flash_id;
			request.request_peer_count_ = request_peer_count;
			request.reqest_ip_ = reqest_ip;
			end_point = endpoint_;
			IsRequest = 1;
		}

		// response
		FlashListWithIpPacket(
			boost::uint32_t transaction_id,
			boost::uint8_t error_code,
			const RID& resource_id,
			const std::vector<FlashPeerInfo>& candidate_peer_info,
			const boost::asio::ip::udp::endpoint& endpoint_)
		{
			transaction_id_ = transaction_id;
			error_code_ = error_code;
			response.resource_id_ = resource_id;
			response.peer_infos_ = candidate_peer_info;
			end_point = endpoint_;
			IsRequest = 0;
		}

		FlashListWithIpPacket(
			boost::uint32_t transaction_id,
			boost::uint8_t error_code,
			const RID& resource_id,            
			const boost::asio::ip::udp::endpoint& endpoint_)
		{
			transaction_id_ = transaction_id;
			error_code_ = error_code;
			response.resource_id_ = resource_id;            
			end_point = endpoint_;
			IsRequest = 0;
		}

		struct Request {
			RID resource_id_;
			std::string flash_id_;
			boost::uint16_t request_peer_count_;
			boost::uint32_t reqest_ip_;
		} request;
		struct Response {
			RID resource_id_;
			std::vector<FlashPeerInfo> peer_infos_;
		} response;
	};


    //和list不同的是，这里制定了客户端的ip，而不是从socket里获取
    struct ListTcpWithIpPacket
        : public ServerPacketT<0x3d>
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {

            ServerPacket::serialize(ar);
            if (IsRequest) {
                ar & request.resource_id_;
                ar & request.peer_guid_;
                ar & request.request_peer_count_;
                ar & request.reqest_ip_;
            } else{
                ar & response.resource_id_;
                ar & util::serialization::make_sized<boost::uint16_t>(response.peer_infos_);
            }
        }

		virtual boost::uint32_t length() const
		{
			boost::uint32_t length = ServerPacket::length();
			if (IsRequest) {
				length += sizeof(request.resource_id_) + sizeof(request.peer_guid_) + sizeof(request.request_peer_count_) + sizeof(request.reqest_ip_);
			} else{
				length += sizeof(response.resource_id_);
				length += sizeof(boost::uint16_t) + response.peer_infos_.size()*CandidatePeerInfo::length();
			}

			return length;
		}

        ListTcpWithIpPacket()
        {
            // IsRequest = 1;
        }

        // request
        ListTcpWithIpPacket(
            boost::uint32_t transaction_id,
            boost::uint32_t peer_version,
            const RID & resource_id,
            const Guid & peer_guid,
            boost::uint16_t request_peer_count,
            boost::uint32_t reqest_ip,
            const boost::asio::ip::udp::endpoint & endpoint_)
        {
            transaction_id_ = transaction_id;
            peer_version_ = peer_version;
            request.resource_id_ = resource_id;
            request.peer_guid_ = peer_guid;
            request.request_peer_count_ = request_peer_count;
            request.reqest_ip_ = reqest_ip;
            end_point = endpoint_;
            IsRequest = 1;
        }

        // response
        ListTcpWithIpPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            const RID& resource_id,
            const std::vector<CandidatePeerInfo>& candidate_peer_info,
            const boost::asio::ip::udp::endpoint& endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.resource_id_ = resource_id;
            response.peer_infos_ = candidate_peer_info;
            end_point = endpoint_;
            IsRequest = 0;
        }
        ListTcpWithIpPacket(
            boost::uint32_t transaction_id,
            boost::uint8_t error_code,
            const RID& resource_id,           
            const boost::asio::ip::udp::endpoint& endpoint_)
        {
            transaction_id_ = transaction_id;
            error_code_ = error_code;
            response.resource_id_ = resource_id;
            end_point = endpoint_;
            IsRequest = 0;
        }



        struct Request {
            RID resource_id_;
            Guid peer_guid_;
            boost::uint16_t request_peer_count_;
            boost::uint32_t reqest_ip_;
        } request;
        struct Response {
            RID resource_id_;
            std::vector<CandidatePeerInfo> peer_infos_;
        } response;
    };



    template <typename PacketHandler>
    void register_tracker_packet(
        PacketHandler & handler)
    {
        handler.template register_packet<ListPacket>();

        handler.template register_packet<LeavePacket>();
        handler.template register_packet<ReportPacket>();
		handler.template register_packet<FlashReportPacket>();

        // QueryPeerCountPacket内核不会发送，服务器使用
        handler.template register_packet<QueryPeerCountPacket>();

        handler.template register_packet<InternalCommandPacket>();

        //查问题使用的命令，内核不会发送
        handler.template register_packet<QueryPeerResourcesPacket>();

        //查询tracker 统计的后台命令
        handler.template register_packet<QueryTrackerStatisticPacket>();

        //查询tcp的端口
        handler.template register_packet<ListTcpPacket>();
        //带客户端ip，查询资源，用于tracker网管
        handler.template register_packet<ListWithIpPacket>();
		handler.template register_packet<FlashListWithIpPacket>();

        handler.template register_packet<FlashReportPacket>();

        //带客户端ip，查询资源，用于tracker代理
        handler.template register_packet<ListTcpWithIpPacket>();
    }

}

#endif  // _PROTOCOL_TRACKET_PACKET_H_
