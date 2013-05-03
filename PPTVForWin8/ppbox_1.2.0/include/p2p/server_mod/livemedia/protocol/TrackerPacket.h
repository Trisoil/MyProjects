
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
				ar & request.ResourceID;
				ar & request.PeerGuid;
				ar & request.RequestPeerCount;
			} else{
				ar & response.ResourceID;
				ar & util::serialization::make_sized<boost::uint16_t>(response.peer_infos_); 
			}
		}

		ListPacket()
		{
			IsRequest = 0;
		}

		//request
		ListPacket(
			boost::uint32_t transaction_id, 
			boost::uint32_t peer_version,
			RID resource_id, 
			Guid peer_guid, 
			boost::uint16_t request_peer_count,
			boost::asio::ip::udp::endpoint endpoint_)
		{
			transaction_id_ = transaction_id;
			peer_version_ = peer_version;
			request.ResourceID = resource_id;
			request.PeerGuid = peer_guid;
			request.RequestPeerCount = request_peer_count;
			end_point_ = endpoint_;
			IsRequest = 1;
		}
		
		//response
		ListPacket(
			boost::uint32_t transaction_id,
			boost::uint8_t error_code,
			RID resource_id,
			std::vector<CandidatePeerInfo> candidate_peer_info,
			boost::asio::ip::udp::endpoint endpoint_)
		{
			transaction_id_ = transaction_id;
			error_code_ = error_code;
			response.ResourceID = resource_id;
			response.peer_infos_ = candidate_peer_info;
			end_point_ = endpoint_;
			IsRequest = 0;
		}

		struct Request {
			RID ResourceID;
			Guid PeerGuid;
			boost::uint16_t RequestPeerCount;
		} request;
		struct Response {
			RID ResourceID;
			std::vector<CandidatePeerInfo> peer_infos_;
		} response;
	};

	/**
	*@brief Peer发向TrackerServer的 Commit 包和TrackerServer回给Peer的 Commit 包
	*/
	struct CommitPacket
		: public ServerPacketT<0x32>
	{
		template <typename Archive>
		void serialize(Archive & ar)
		{
			ServerPacket::serialize(ar);
			if (IsRequest) {
				ar & request.PeerGuid;
				ar & request.UdpPort;
				ar & request.TcpPort;
				ar & request.StunPeerIP;
				ar & request.StunPeerUdpPort;
				//ar & request.RealIPCount;
				ar & util::serialization::make_sized<boost::uint8_t>(request.real_ips_); 
				//ar & request.RidCount;
				ar & util::serialization::make_sized<boost::uint8_t>(request.rids_); 
				if (PEER_VERSION >= 0x00000002) {
					ar & request.StunDetectUdpPort;
				}
			} else {
				ar & response.KeepAliveInterval;
				ar & response.DetectedIP;
				ar & response.DetectedUdpPort;
			}
		}
		CommitPacket()
		{
			IsRequest = 0;

		}

		//request
		CommitPacket(
			boost::uint32_t transaction_id, 
			boost::uint32_t peer_version,
			Guid peer_guid, 
			boost::uint16_t udp_port,
			boost::uint16_t tcp_port,
			boost::uint32_t stun_peer_ip,
			boost::uint16_t stun_peer_udpport,
			std::vector<boost::uint32_t> real_ips,
			std::vector<RID> resource_ids,
			boost::uint16_t stun_detected_udp_port,
			boost::asio::ip::udp::endpoint endpoint_)
		{
			transaction_id_ = transaction_id;
			peer_version_ = peer_version;
			request.PeerGuid = peer_guid;
			request.UdpPort = udp_port;
			request.TcpPort = tcp_port;
			request.StunPeerIP = stun_peer_ip;
			request.StunPeerUdpPort = stun_peer_udpport;
			request.real_ips_ = real_ips;
			request.rids_ = resource_ids;
			request.StunDetectUdpPort = stun_detected_udp_port;
			end_point_ = endpoint_;
			IsRequest = 1;
		}

		//response
		CommitPacket(
			boost::uint32_t transaction_id, 
			boost::uint8_t error_code,
			boost::uint16_t keepalive_interval,
			boost::uint32_t detected_ip,
			boost::uint16_t detected_udp_port,
			boost::asio::ip::udp::endpoint endpoint_)
		{
			transaction_id_ = transaction_id;
			error_code_ = error_code;
			response.KeepAliveInterval = keepalive_interval;
			response.DetectedIP = detected_ip;
			response.DetectedUdpPort = detected_udp_port;
			end_point_ = endpoint_;
			IsRequest = 0;
		}

		struct Request {
			Guid PeerGuid;
			boost::uint16_t UdpPort;
			boost::uint16_t TcpPort;
			boost::uint32_t StunPeerIP;
			boost::uint16_t StunPeerUdpPort;
			//boost::uint8_t RealIPCount;
			std::vector<boost::uint32_t> real_ips_;
			//boost::uint8_t RidCount;
			std::vector<RID> rids_;
			boost::uint16_t StunDetectUdpPort;
		} request;
		struct Response {
			boost::uint16_t KeepAliveInterval;
			boost::uint32_t DetectedIP;
			boost::uint16_t DetectedUdpPort;
		} response;

	};

	/**
	*@brief Peer发向TrackerServer的 KeepAlive 包和TrackerServer回给Peer的 KeepAlive 包
	*/
	struct KeepAlivePacket
		: public ServerPacketT<0x33>
	{
		template <typename Archive>
		void serialize(Archive & ar)
		{
			ServerPacket::serialize(ar);
			if(IsRequest){
				ar & request.PeerGuid;
				ar & request.StunPeerIP;
				ar & request.StunPeerUdpPort;
				ar & request.StunDetectUdpPort;
			}else{
				ar & response.KeepALiveInterval;
				ar & response.DetectedIP;
				ar & response.DetectedUdpPort;
				ar & response.ResourceCount;
			}
		}

		KeepAlivePacket()
		{
			IsRequest = 0;
		}

		//request
		KeepAlivePacket(
			boost::uint32_t transaction_id, 
			boost::uint32_t peer_version,
			Guid peer_guid, 
			boost::uint32_t stun_peer_ip,
			boost::uint16_t stun_peer_udpport,
			boost::uint16_t stun_detected_udp_port,
			boost::asio::ip::udp::endpoint endpoint_)
		{
			transaction_id_ = transaction_id;
			peer_version_ = peer_version;
			request.PeerGuid = peer_guid;
			request.StunPeerIP = stun_peer_ip;
			request.StunPeerUdpPort = stun_peer_udpport;
			request.StunDetectUdpPort = stun_detected_udp_port;
			end_point_ = endpoint_;
			IsRequest = 1;
		}

		//request
		KeepAlivePacket(
			boost::uint32_t transaction_id,
			boost::uint8_t error_code,
			boost::uint16_t keepalive_interval,
			boost::uint32_t detected_ip,
			boost::uint16_t detected_udp_port,
			boost::uint16_t resource_count,
			boost::asio::ip::udp::endpoint endpoint_)
		{
			transaction_id_ = transaction_id;
			error_code_ = error_code;
			response.KeepALiveInterval = keepalive_interval;
			response.DetectedIP = detected_ip;
			response.DetectedUdpPort = detected_udp_port;
			response.ResourceCount = resource_count;
			end_point_ = endpoint_;
			IsRequest = 0;
		}

	public :
		struct Request {
			Guid PeerGuid;
			boost::uint32_t StunPeerIP;
			boost::uint16_t StunPeerUdpPort;
			boost::uint16_t StunDetectUdpPort;
		} request;
		struct Response {
			boost::uint16_t KeepALiveInterval;
			boost::uint32_t DetectedIP;
			boost::uint16_t DetectedUdpPort;
			boost::uint16_t ResourceCount;
		} response;
	};

	/**
	*@brief Peer发向TrackerServer的 Leave 包
	*/
	struct LeavePacket
		: public ServerPacketT<0x34>
	{
		LeavePacket()
		{
		}
		LeavePacket(
			boost::uint32_t transaction_id, 
			boost::uint32_t peer_version,
			Guid peer_guid)
		{
			transaction_id_ = transaction_id;
			peer_version_ = peer_version;
			PeerGuid = peer_guid;
		}

		template <typename Archive>
		void serialize(Archive & ar)
		{
			ServerPacket::serialize(ar);
			ar & PeerGuid;
		}
		Guid PeerGuid;
	};

	/**
	*@brief Peer发向TrackerServer的 Report 包和TrackerServer回给Peer的 KeepAlive 包
	*/
	struct ReportPacket
		: public ServerPacketT<0x35>
	{
		ReportPacket()
		{
			IsRequest = 0;
		}

		//request
		ReportPacket(
			boost::uint32_t transaction_id, 
			boost::uint32_t peer_version,
			Guid peer_guid, 
			boost::uint16_t local_resource_count, 
			boost::uint16_t server_resource_count, 
			boost::uint16_t udp_port, 
			boost::uint16_t tcp_port, 
			boost::uint32_t stun_peer_ip, 
			boost::uint16_t stun_peer_udpport, 
			boost::uint16_t stun_detected_udp_port, 
			std::vector<boost::uint32_t> real_ips, 
			std::vector<REPORT_RESOURCE_STRUCT> resource_ids,
			boost::uint8_t nat_type, 
			boost::uint8_t upload_priority, 
			boost::uint8_t idle_time_in_mins,
			boost::asio::ip::udp::endpoint endpoint_)
		{
			transaction_id_ = transaction_id;
			peer_version_ = peer_version;
			request.PeerGuid = peer_guid;
			request.LocalResourceCount = local_resource_count;
			request.ServerResourceCount = server_resource_count;
			request.UdpPort = udp_port;
			request.TcpPort = tcp_port;
			request.StunPeerIP = stun_peer_ip;
			request.StunPeerUdpPort = stun_peer_udpport;
			request.StunDetectedUdpPort = stun_detected_udp_port;
			request.real_ips_ = real_ips;
			request.resource_ids_ = resource_ids;
			request.PeerNatType = nat_type;
			request.UploadPriority = upload_priority;
			request.IdleTimeInMins = idle_time_in_mins;
			end_point_ = endpoint_;
			IsRequest = 1;
		}
		//response
		ReportPacket(
			boost::uint32_t transaction_id,
			boost::uint8_t error_code,
			boost::uint16_t keepalive_interval,
			boost::uint32_t detected_ip,
			boost::uint16_t detected_udp_port,
			boost::uint16_t resource_count,
			boost::asio::ip::udp::endpoint endpoint_)
		{
			transaction_id_ = transaction_id;
			error_code_ = error_code;
			response.KeepAliveInterval = keepalive_interval;
			response.DetectedIP = detected_ip;
			response.DetectedUdpPort = detected_udp_port;
			response.ResourceResCount = resource_count;
			end_point_ = endpoint_;
			IsRequest = 0;
		}

		template <typename Archive>
		void serialize(Archive & ar)
		{
			ServerPacket::serialize(ar);
			if(IsRequest){
				ar & request.PeerGuid;
				ar & request.LocalResourceCount;
				ar & request.ServerResourceCount;
				ar & request.PeerNatType;
				ar & request.UploadPriority;
				ar & request.IdleTimeInMins;
				ar & framework::container::make_array( request.Reversed );
				ar & request.UdpPort;
				ar & request.TcpPort;
				ar & request.StunPeerIP;
				ar & request.StunPeerUdpPort;
				ar & request.StunDetectedUdpPort;
				ar & util::serialization::make_sized<boost::uint8_t>(request.real_ips_); 
				ar & util::serialization::make_sized<boost::uint8_t>(request.resource_ids_); 
			}else{
				ar & response.KeepAliveInterval;
				ar & response.DetectedIP;
				ar & response.DetectedUdpPort;
				ar & response.ResourceResCount;
			}
		}

		struct Request {
			Guid PeerGuid;
			boost::uint16_t LocalResourceCount;
			boost::uint16_t ServerResourceCount;
			boost::uint16_t UdpPort;
			boost::uint16_t TcpPort;
			boost::uint32_t StunPeerIP;
			boost::uint16_t StunPeerUdpPort;
			boost::uint16_t StunDetectedUdpPort;
			std::vector<boost::uint32_t> real_ips_;
			std::vector<REPORT_RESOURCE_STRUCT> resource_ids_;
			boost::uint8_t  PeerNatType;
			boost::uint8_t UploadPriority;
			boost::uint8_t IdleTimeInMins;
			boost::uint8_t  Reversed[3];
		} request;
		struct Response {
			boost::uint16_t KeepAliveInterval;
			boost::uint32_t DetectedIP;
			boost::uint16_t DetectedUdpPort;
			boost::uint16_t ResourceResCount;
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
				ar & request.ResourceID;
			}
			else{
				ar & response.ResourceID;
				ar & response.PeerCount;
			}
		}

		QueryPeerCountPacket()
		{
			IsRequest = 0;
		}

		//request
		QueryPeerCountPacket(
			boost::uint32_t transaction_id, 
			boost::uint32_t peer_version,
			Guid resource_id,
			boost::asio::ip::udp::endpoint endpoint_)
		{
			transaction_id_ = transaction_id;
			peer_version_ = peer_version;
			request.ResourceID = resource_id;
			end_point_ = endpoint_;
			IsRequest = 1;
		}

		//response
		QueryPeerCountPacket(
			boost::uint32_t transaction_id, 
			boost::uint8_t error_code,
			Guid resource_id,
			boost::uint32_t peer_count,
			boost::asio::ip::udp::endpoint endpoint_)
		{
			transaction_id_ = transaction_id;
			error_code_ = error_code;
			response.ResourceID = resource_id;
			response.PeerCount = peer_count;
			end_point_ = endpoint_;
			IsRequest = 0;

		}
		struct Request {
			Guid ResourceID;
		} request;
		struct Response {
			Guid ResourceID;
			boost::uint32_t PeerCount;
		} response;

	};

	struct QueryBatchPeerCountPacket
		: public ServerPacketT<0x37>
	{
		struct RESOURCE_PEER_COUNT_INFO
		{
			RID ResourceID;
			boost::uint32_t PeerCount;

			template <typename Archive>
			void serialize(Archive & ar)
			{
				ar & ResourceID & PeerCount; 
			}
		};

		template <typename Archive>
		void serialize(Archive & ar)
		{
			ServerPacket::serialize(ar);
			if (IsRequest) {
				ar & util::serialization::make_sized<boost::uint8_t>(request.resources_); 
			} else {
				ar & util::serialization::make_sized<boost::uint8_t>(response.resource_peer_counts_); 
			}
		}

		QueryBatchPeerCountPacket()
		{
			IsRequest = 0;
		}

		//request
		QueryBatchPeerCountPacket(
			boost::uint32_t transaction_id, 
			const std::vector<RID>& resource_ids,
			boost::asio::ip::udp::endpoint endpoint_)
		{
			transaction_id_ = transaction_id;
			request.resources_ = resource_ids;
			end_point_ = endpoint_;
			IsRequest = 1;
		}

		//response
		QueryBatchPeerCountPacket(
			boost::uint32_t transaction_id,
			boost::uint8_t error_code,
			boost::asio::ip::udp::endpoint endpoint_)
		{
			transaction_id_ = transaction_id;
			error_code_ = error_code;
			end_point_ = endpoint_;
			IsRequest = 0;
		}
		QueryBatchPeerCountPacket(
			boost::uint32_t transaction_id,
			const std::vector<RESOURCE_PEER_COUNT_INFO>& resource_peer_counts,
			boost::asio::ip::udp::endpoint endpoint_)
		{
			transaction_id_ = transaction_id;
			response.resource_peer_counts_ = resource_peer_counts;
			end_point_ = endpoint_;
			IsRequest = 0;
		}


		struct Request {
			std::vector<RID> resources_;
		} request;
		struct Response {
			std::vector<RESOURCE_PEER_COUNT_INFO> resource_peer_counts_;
		} response;
	};


	template <typename PacketHandler>
	void register_tracker_packet(
		PacketHandler & handler)
	{
		handler.template register_packet<ListPacket>();
		handler.template register_packet<CommitPacket>();
		handler.template register_packet<KeepAlivePacket>();
		handler.template register_packet<LeavePacket>();
		handler.template register_packet<ReportPacket>();
		handler.template register_packet<QueryPeerCountPacket>();
		handler.template register_packet<QueryBatchPeerCountPacket>();
	}

}

#endif // _PROTOCOL_TRACKET_PACKET_H_
