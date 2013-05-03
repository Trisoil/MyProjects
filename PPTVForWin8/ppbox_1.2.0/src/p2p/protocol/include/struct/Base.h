//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _PROTOCOL_BASH_H_
#define _PROTOCOL_BASH_H_

#pragma once

#include "struct/Structs.h"

#include <framework/network/Endpoint.h>

namespace protocol
{
	struct TrackerInfo
	{
		string mod_;
		string count_;
		boost::asio::ip::udp::endpoint endpoint_;
	};

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	struct SocketAddr
	{
		boost::uint32_t IP;
		boost::uint16_t Port;

		SocketAddr(
			boost::uint32_t ip = 0,
			boost::uint16_t port = 0)
			: IP(ip)
			, Port(port)
		{
		}

		explicit SocketAddr(
			const boost::asio::ip::udp::endpoint& end_point)
		{
			IP = end_point.address().to_v4().to_ulong();
			Port = end_point.port();
		}

		bool operator <(
			const SocketAddr& socket_addr) const
		{
			if (IP != socket_addr.IP)
				return IP < socket_addr.IP;
			else
				return Port < socket_addr.Port;
		}

		bool operator !() const
		{
			return IP == 0 && Port == 0;
		}

		friend bool operator == (
			const SocketAddr & l,
			const SocketAddr & r)
		{
			return l.IP == r.IP && l.Port == r.Port;
		}

		friend bool operator != (
			const SocketAddr & l,
			const SocketAddr & r)
		{
			return !(l == r);
		}

		friend std::ostream& operator << (
			std::ostream& os,
			const SocketAddr& socket_addr)
		{
			boost::asio::ip::udp::endpoint ep = framework::network::Endpoint(socket_addr.IP, socket_addr.Port);
			return os << ep.address().to_string() << ":" << ep.port();
		}
	};

	//////////////////////////////////////////////////////////////////////////
	struct PeerAddr
	{
		boost::uint32_t IP;
		boost::uint16_t UdpPort;

		PeerAddr()
		{
			IP = 0;
			UdpPort = 0;
		}

		PeerAddr(
			boost::uint32_t ip,
			boost::uint16_t udp_port)
		{
			IP = ip;
			UdpPort = udp_port;
		}

		bool operator <(
			const PeerAddr& peer_addr) const
		{
			if (IP != peer_addr.IP)
				return IP < peer_addr.IP;
			else
				return UdpPort < peer_addr.UdpPort;
		}

		bool operator !() const
		{
			return IP == 0 && UdpPort == 0 ;
		}

		friend bool operator == (
			PeerAddr const & l,
			PeerAddr const & r)
		{
			return l.IP == r.IP && l.UdpPort == r.UdpPort;
		}

		friend bool  operator != (
			PeerAddr const & l,
			PeerAddr const & r)
		{
			return !(l == r);
		}

		friend std::ostream& operator << (
			std::ostream& os,
			const PeerAddr& peer_addr)
		{
			boost::asio::ip::address_v4 ip(peer_addr.IP);
			os << "(" << ip << ":" << peer_addr.UdpPort << ")";
			return os;
		}
	};


	enum P4PLOACTIONINFO
	{
		TYPE_UNKNOWN=0,//因为ip无法识别，从而计算不出地域信息
		TYPE_SAME_CITY = 1,
		TYPE_SAME_PROVINCE,
		TYPE_SAME_ISP,
		TYPE_SAME_CONTRY,
		TYPE_DIF_COUNTRY,
	};

	//////////////////////////////////////////////////////////////////////////
	struct CandidatePeerInfo
	{
		boost::uint32_t IP;
		boost::uint16_t UdpPort;
		boost::uint16_t PeerVersion;
		boost::uint32_t DetectIP;
		boost::uint16_t DetectUdpPort;
		boost::uint32_t StunIP;
		boost::uint16_t StunUdpPort;

		boost::uint8_t  PeerNatType;
		boost::uint8_t  UploadPriority;
		boost::uint8_t  IdleTimeInMins;
		boost::uint8_t  TrackerPriority;

		//ck 20120917 经过讨论，发现在candidatepeerinfo里添加字段，将会导致共享内存的变化，带来比较大的麻烦，所以只好绕过

		////flash做p2p时候，不是用ip来连接其他节点，而是使用flashid
		//std::string FlashID;

		////用来标记地域相关信息,数值用P4PINFO表示
		//boost::uint8_t  LocationInfo;
		////保留位
		//boost::uint16_t Reserved;

		template <typename Archive>
		void serialize(Archive & ar)
		{
			ar & IP & UdpPort & PeerVersion & DetectIP & DetectUdpPort & StunIP & StunUdpPort
				& PeerNatType & UploadPriority & IdleTimeInMins & TrackerPriority;
		}

		static boost::uint32_t length()
		{
			//return len += sizeof(IP)+sizeof(UdpPort)+sizeof(PeerVersion)+sizeof(DetectIP)+sizeof(DetectUdpPort)+sizeof(StunIP)+sizeof(StunUdpPort)
			//len += sizeof(PeerNatType)+sizeof(UploadPriority)+sizeof(IdleTimeInMins)+sizeof(TrackerPriority);
			int len = 0;
			len += sizeof(boost::uint32_t) + sizeof(boost::uint16_t) + sizeof(boost::uint16_t) + sizeof(boost::uint32_t) + sizeof(boost::uint16_t) + sizeof(boost::uint32_t) + sizeof(boost::uint16_t);

			len += sizeof(boost::uint8_t) + sizeof(boost::uint8_t) + sizeof(boost::uint8_t) + sizeof(boost::uint8_t);
			return len;
		}



		// TODO(emma): 去掉默认构造函数，再添加拷贝构造函数
		CandidatePeerInfo()
		{
			IP = 0;
			UdpPort = 0;
			PeerVersion = 0;
			DetectIP = 0;
			DetectUdpPort = 0;
			StunIP = 0;
			StunUdpPort = 0;
			PeerNatType = (boost::uint8_t)-1;
			UploadPriority = 0;
			IdleTimeInMins = 0;
			TrackerPriority = 0;
		}

		CandidatePeerInfo(
			boost::uint32_t ip,
			boost::uint16_t udp_port,
			boost::uint16_t peer_version,
			boost::uint32_t detect_ip = 0,
			boost::uint16_t detect_udp_port = 0,
			boost::uint32_t stun_ip = 0,
			boost::uint16_t stun_udp_port = 0)
		{
			IP = ip;
			UdpPort = udp_port;
			PeerVersion = peer_version;
			DetectIP = detect_ip;
			DetectUdpPort = detect_udp_port;
			StunIP = stun_ip;
			StunUdpPort = stun_udp_port;
			UploadPriority = 0;
			IdleTimeInMins = 0;
		}

		CandidatePeerInfo(
			boost::uint32_t ip,
			boost::uint16_t udp_port,
			boost::uint16_t peer_version,
			boost::uint32_t detect_ip,
			boost::uint16_t detect_udp_port,
			boost::uint32_t stun_ip,
			boost::uint16_t stun_udp_port,
			boost::uint8_t  peer_nat_type,
			boost::uint8_t  upload_priority,
			boost::uint8_t  idle_time_in_mins,
			boost::uint8_t  tracker_priority)
		{
			IP = ip;
			UdpPort = udp_port;
			PeerVersion = peer_version;
			DetectIP = detect_ip;
			DetectUdpPort = detect_udp_port;
			StunIP = stun_ip;
			StunUdpPort = stun_udp_port;
			PeerNatType = peer_nat_type;
			UploadPriority = upload_priority;
			IdleTimeInMins = idle_time_in_mins;
			TrackerPriority = tracker_priority;
		}

		bool NeedStunInvoke(boost::uint32_t LocalDetectedIP) const
		{
			if (LocalDetectedIP == 0 || LocalDetectedIP != DetectIP)
			{
				if (DetectIP != IP && StunIP != 0)
				{
					return true;
				}
			}

			return false;

		}

		bool FromSameSubnet(const protocol::CandidatePeerInfo& peer_info) const
		{
			// 这里的判断不完全准确。同一子网内经不同路由连接internet的机器会被误判为来自不同子网。
			// 我们假设这样的模糊正确已经足够好
			return DetectIP != 0 && DetectIP == peer_info.DetectIP;
		}

		boost::asio::ip::udp::endpoint GetConnectEndPoint(boost::uint32_t LocalDetectedIP) const
		{
			if (DetectIP == 0)
			{
				return framework::network::Endpoint(IP, UdpPort);
			}
			else
			{
				if (LocalDetectedIP == DetectIP)
				{
					return framework::network::Endpoint(IP, UdpPort);
				}
				else
				{
					return framework::network::Endpoint(DetectIP, DetectUdpPort);
				}
			}
		}

		boost::asio::ip::udp::endpoint GetStunEndPoint() const
		{
			return framework::network::Endpoint(StunIP, StunUdpPort);
		}

		boost::asio::ip::udp::endpoint GetLanUdpEndPoint() const
		{
			return framework::network::Endpoint(IP, UdpPort);
		}

		PeerAddr GetPeerInfo() const
		{
			PeerAddr addr(IP, UdpPort);
			return addr;
		}

		SocketAddr GetSelfSocketAddr() const
		{
			SocketAddr addr(IP, UdpPort);
			return addr;
		}

		SocketAddr GetDetectSocketAddr() const
		{
			SocketAddr addr(DetectIP, DetectUdpPort);
			return addr;
		}

		SocketAddr GetKeySocketAddr(boost::uint32_t LocalDetectedIP) const
		{
			if (LocalDetectedIP == 0 || LocalDetectedIP != DetectIP)
				return GetDetectSocketAddr();
			else
				return GetSelfSocketAddr();
		}

		SocketAddr GetStunSocketAddr() const
		{
			SocketAddr addr(StunIP, StunUdpPort);
			return addr;
		}

		friend bool operator < (
			const CandidatePeerInfo& l,
			const CandidatePeerInfo& r)
		{
			if (l.GetDetectSocketAddr() != r.GetDetectSocketAddr())
				return l.GetDetectSocketAddr() < r.GetDetectSocketAddr();
			else if (l.GetPeerInfo() != r.GetPeerInfo())
				return l.GetPeerInfo() < r.GetPeerInfo();
			else
				return l.GetStunSocketAddr() < r.GetStunSocketAddr();
		}

		friend bool operator == (
			const CandidatePeerInfo& l, 
			const CandidatePeerInfo& r)
		{
			if(l.IP == r.IP && l.UdpPort == r.UdpPort && l.PeerVersion == r.PeerVersion &&
				l.DetectIP == r.DetectIP && l.DetectUdpPort == r.DetectUdpPort &&
				l.StunIP == r.StunIP && l.StunUdpPort == r.StunUdpPort &&
				l.UploadPriority == r.UploadPriority && l.IdleTimeInMins == r.IdleTimeInMins )
				return true;
			return false;
		}

		bool operator ! () const
		{
			return !GetPeerInfo() && !GetDetectSocketAddr() && !GetStunSocketAddr();
		}
	};

	//////////////////////////////////////////////////////////////////////////
	struct FlashPeerInfo
	{
		std::string FlashID;
		boost::uint32_t IP;
		boost::uint16_t HttpPort;
		boost::uint16_t PeerVersion;
		boost::uint8_t  UploadPriority;
		boost::uint8_t  IdleTimeInMins;
		boost::uint8_t  TrackerPriority;
		boost::uint64_t  Reserve;

		template <typename Archive>
		void serialize(Archive & ar)
		{
			ar & Reserve;
			ar & util::serialization::make_sized<boost::uint16_t>(FlashID);
			ar & IP & HttpPort & PeerVersion & UploadPriority & IdleTimeInMins & TrackerPriority;
		}

		boost::uint32_t length() const
		{
            boost::uint32_t len = sizeof(boost::uint16_t) + FlashID.size();
			len += sizeof(Reserve) +sizeof(IP) + sizeof(HttpPort) + sizeof(PeerVersion) + 
                sizeof(UploadPriority) + sizeof(IdleTimeInMins) + sizeof(TrackerPriority);
			return len;
		}

		FlashPeerInfo()
		{
			IP = 0;
			HttpPort = 0;
			PeerVersion = 0;
			UploadPriority = 0;
			IdleTimeInMins = 0;
			TrackerPriority = 0;

		}

		FlashPeerInfo(

			const std::string& flash_id,
			boost::uint32_t ip,
			boost::uint16_t udpport,
			boost::uint16_t peerversion,
			boost::uint8_t  uploadpriority,
			boost::uint8_t  idletimeinmins,
			boost::uint8_t  trackerpriority
			)
		{
			FlashID = flash_id;
			IP = ip;
			HttpPort = udpport;
			PeerVersion = peerversion;
			UploadPriority = uploadpriority;
			IdleTimeInMins = idletimeinmins;
			TrackerPriority = trackerpriority;
		}

		friend bool operator < (
			const FlashPeerInfo& l,
			const FlashPeerInfo& r)
		{
			if (l.FlashID != r.FlashID)
			{
				return l.FlashID < r.FlashID;
			}
			else if (l.IP != r.IP)
			{
				return l.IP < r.IP;
			}


		}

		friend bool operator == (
			const FlashPeerInfo& l, 
			const FlashPeerInfo& r)
		{
			if (l.FlashID == r.FlashID && l.IP == r.IP && l.HttpPort == r.HttpPort 
				&& l.PeerVersion == r.PeerVersion && l.UploadPriority == r.UploadPriority && l.IdleTimeInMins == r.IdleTimeInMins && l.TrackerPriority == r.TrackerPriority)
				return true;

			return false;

		}

	};

	inline std::ostream& operator << (std::ostream& os, const CandidatePeerInfo& info)
	{
		return os << "Address: " << framework::network::Endpoint(info.IP, info.UdpPort).to_string()
			<< ", Detected Address: " << framework::network::Endpoint(info.DetectIP, info.DetectUdpPort).to_string()
			<< ", Stun Address: " << framework::network::Endpoint(info.StunIP, info.StunUdpPort).to_string()
			<< ", PeerVersion: " << info.PeerVersion<<" trackerprority:"<<(boost::uint32_t)info.TrackerPriority
			<< ", UploadPriority: " << (boost::uint32_t)info.UploadPriority
			<<", PeerNatType:"<<short(info.PeerNatType);
	}

    inline std::ostream& operator << (std::ostream& os, const FlashPeerInfo& info)
    {
        return os << "Address: " << framework::network::Endpoint(info.IP, info.HttpPort).to_string()
            << ", PeerVersion: " << info.PeerVersion<<" trackerprority:"<<(boost::uint32_t)info.TrackerPriority
            << ", UploadPriority: " << (boost::uint32_t)info.UploadPriority
            <<", flashID:"<<info.FlashID;
    }


	enum MY_STUN_NAT_TYPE
	{
		TYPE_ERROR = -1,
		TYPE_FULLCONENAT = 0,
		TYPE_IP_RESTRICTEDNAT,
		TYPE_IP_PORT_RESTRICTEDNAT,
		TYPE_SYMNAT,
		TYPE_PUBLIC
	};

	struct PushTask
	{
		string url_;
		string refer_url_;
		RidInfo rid_info_;
		PUSH_TASK_PARAM param_;
	};

	inline std::ostream& operator << (std::ostream& os, const PushTask& task)
	{
		return os
			<< "{"
			<< "Url: '" << task.url_ << "', "
			<< "ReferUrl: '" << task.refer_url_ << "', "
			<< "protocol::RidInfo: " << task.rid_info_ << ", "
			<< "Param: " << task.param_ << ", "
			<< "}"
			;
	}

}

#endif  // _PROTOCOL_BASH_H_

