#pragma once

#include "protocol/Structs.h"

#include <framework/network/Endpoint.h>

using namespace std;

namespace protocol
{

    enum Rid_From
    {
        RID_BY_URL,
        RID_BY_CONTENT,
        RID_BY_PLAY_URL
    };

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
            const SocketAddr& socket_addr ) const
        {
            if( IP != socket_addr.IP )
                return IP < socket_addr.IP;
            else
                return Port < socket_addr.Port;
        }

        bool operator !() const
        {
            return IP == 0 && Port == 0;
        }

        friend bool operator ==(
            const SocketAddr & l, 
            const SocketAddr & r)
        {
            return l.IP == r.IP && l.Port == r.Port;
        }

        friend bool operator !=(
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
        boost::uint16_t TcpPort;

        PeerAddr()
        {
            IP = 0;
            UdpPort = 0;
            TcpPort = 0;
        }

        PeerAddr(
            boost::uint32_t ip, 
            boost::uint16_t udp_port, 
            boost::uint16_t tcp_port)
        {
            IP = ip;
            UdpPort = udp_port;
            TcpPort = tcp_port;
        }

        bool operator <(
            const PeerAddr& peer_addr ) const
        {
            if( IP != peer_addr.IP )
                return IP < peer_addr.IP;
            else if( UdpPort != peer_addr.UdpPort )
                return UdpPort < peer_addr.UdpPort;
            else
                return TcpPort < peer_addr.TcpPort;
        }

        bool operator !() const
        {
            return IP == 0 && UdpPort == 0 && TcpPort == 0;
        }

        friend bool operator==(
            PeerAddr const & l, 
            PeerAddr const & r)
        {
            return l.IP == r.IP && l.UdpPort == r.UdpPort && l.TcpPort == r.TcpPort;
        }

        friend bool  operator!=(
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
            os << "(" << ip << ":" << peer_addr.UdpPort << ":" << peer_addr.TcpPort << ")";
            return os;
        }
    };


    //////////////////////////////////////////////////////////////////////////
    struct CandidatePeerInfo
    { 
        boost::uint32_t IP;
        boost::uint16_t UdpPort;
        boost::uint16_t TcpPort;
        boost::uint32_t DetectIP;
        boost::uint16_t DetectUdpPort;
        boost::uint32_t StunIP;
        boost::uint16_t StunUdpPort;

        boost::uint8_t  PeerNatType;
        boost::uint8_t  UploadPriority;
        boost::uint8_t  IdleTimeInMins;
        boost::uint8_t  Reversed;

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & IP & UdpPort & TcpPort & DetectIP & DetectUdpPort & StunIP & StunUdpPort
                & PeerNatType & UploadPriority & IdleTimeInMins & Reversed;
        }

		CandidatePeerInfo( const CANDIDATE_PEER_INFO & pi )
		{
			// 
			this->IP = pi.IP;
			this->UdpPort = pi.UdpPort;
			this->TcpPort = pi.TcpPort;
			this->DetectIP = pi.DetectIP;
			this->DetectUdpPort = pi.DetectUdpPort;
			this->StunIP = pi.StunIP;
			this->StunUdpPort = pi.StunUdpPort;
			this->PeerNatType = pi.PeerNatType;
			this->UploadPriority = pi.UploadPriority;
			this->IdleTimeInMins = pi.IdleTimeInMins;
		}

        CandidatePeerInfo()
        {
            IP = 0;
            UdpPort = 0;
            TcpPort = 0;
            DetectIP = 0;
            DetectUdpPort = 0;
            StunIP = 0;
            StunUdpPort = 0;
            PeerNatType = (boost::uint8_t)-1;
            UploadPriority = 0;
            IdleTimeInMins = 0;
            Reversed = 0;
        }

        CandidatePeerInfo(
            boost::uint32_t ip, 
            boost::uint16_t udp_port, 
            boost::uint16_t tcp_port, 
            boost::uint32_t detect_ip=0, 
            boost::uint16_t detect_udp_port=0,
            boost::uint32_t stun_ip=0, 
            boost::uint16_t stun_udp_port=0)
        {
            IP = ip;
            UdpPort = udp_port;
            TcpPort = tcp_port;
            DetectIP = detect_ip;
            DetectUdpPort = detect_udp_port;
            StunIP = stun_ip;
            StunUdpPort = stun_udp_port;
            UploadPriority = 0;
            IdleTimeInMins = 0;
        }

        bool NeedStunInvoke(boost::uint32_t LocalDetectedIP) const
        {
            if( LocalDetectedIP==0 || LocalDetectedIP!=DetectIP)
            {
                if (DetectIP != IP && StunIP != 0)
                {
                    return true;
                }
            }

            return false;
        
        }

        boost::asio::ip::udp::endpoint GetConnectEndPoint(boost::uint32_t LocalDetectedIP) const 
        {
            if( LocalDetectedIP==0 || LocalDetectedIP!=DetectIP)
                return framework::network::Endpoint(DetectIP, DetectUdpPort);
            else
                return framework::network::Endpoint(IP, UdpPort);
        }

        boost::asio::ip::udp::endpoint GetStunEndPoint() const
        {
            return framework::network::Endpoint(StunIP, StunUdpPort);
        }

        boost::asio::ip::udp::endpoint GetLanUdpEndPoint() const
        {
            return framework::network::Endpoint(IP, UdpPort);
        }

        boost::asio::ip::tcp::endpoint GetLanTcpEndPoint() const
        {
            return framework::network::Endpoint(IP, TcpPort);
        }

        PeerAddr GetPeerInfo() const
        {
            PeerAddr addr(IP, UdpPort, TcpPort);
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
            if( LocalDetectedIP==0 || LocalDetectedIP!=DetectIP)
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

        bool operator ! () const
        {
            return !GetPeerInfo() && !GetDetectSocketAddr() && !GetStunSocketAddr();
        }
    };

	inline std::ostream& operator << (std::ostream& os, const CandidatePeerInfo& info)
	{
		return os << "Address: " << framework::network::Endpoint(info.IP, info.UdpPort).to_string()
			<< ", Detected Address: " << framework::network::Endpoint(info.DetectIP, info.DetectUdpPort).to_string()
			<< ", Stun Address: " << framework::network::Endpoint(info.StunIP, info.StunUdpPort).to_string() 
			<< ", TcpPort: " << info.TcpPort
			  << ", UploadPriority: " << (boost::uint32_t)info.UploadPriority
			;
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
/*
    struct PushTask
    {
        string url_;
        string refer_url_;
        storage::RidInfo rid_info_;
        PUSH_TASK_PARAM param_;
    };

	inline std::ostream& operator << (std::ostream& os, const PushTask& task)
    {
        return os 
            << "{"
            << "Url: '" << task.url_ << "', "
            << "ReferUrl: '" << task.refer_url_ << "', "
            << "storage::RidInfo: " << task.rid_info_ << ", "
            << "Param: " << task.param_ << ", "
            << "}"
            ;
    }
*/

}
