#pragma once

#include "framework/network/util.h"
#include "protocal/structs.h"
#include "base/guid.h"
#include "base/func.h"


namespace base
{
	const size_t SUB_PIECE_SIZE = 1024;
	const size_t SUB_PIECE_COUNT_PER_PIECE = 128;
	const size_t PIECE_SIZE = SUB_PIECE_SIZE * SUB_PIECE_COUNT_PER_PIECE;
	const size_t BLOCK_MIN_SIZE = 2*1024*1024;
	const size_t BLOCK_MAX_COUNT = 50;

	enum Rid_From
	{
		RID_BY_URL,
		RID_BY_CONTENT,
        RID_BY_PLAY_URL
	};

	using protocal::SUB_PIECE_INFO;

	struct TrackerInfo
	{
		string mod_;
		string count_;
		boost::asio::ip::udp::endpoint endpoint_;
	};

	//////////////////////////////////////////////////////////////////////////
	struct SubPieceInfo
	{

		u_int block_index_;
		u_int subpiece_index_;

		explicit SubPieceInfo(u_int block_index = 0, u_int subpiece_index = 0 )
			: block_index_(block_index), subpiece_index_(subpiece_index) {}

		explicit SubPieceInfo(const SUB_PIECE_INFO& subpiece_info)
			: block_index_(subpiece_info.BlockIndex), subpiece_index_(subpiece_info.SubPieceIndex)
		{
		}

		SubPieceInfo& operator=(const SUB_PIECE_INFO& subpiece_info)
		{
			block_index_ = subpiece_info.BlockIndex;
			subpiece_index_ = subpiece_info.SubPieceIndex;
			return *this;
		}

		SUB_PIECE_INFO GetSubPieceInfoStruct() const
		{
			SUB_PIECE_INFO subpiece_info_struct;
			subpiece_info_struct.BlockIndex = block_index_;
			subpiece_info_struct.SubPieceIndex = subpiece_index_;
			return subpiece_info_struct;
		}

		inline static SubPieceInfo MakeByPosition(size_t position, size_t block_size)
		{
			SubPieceInfo subpiece_info;
            subpiece_info.block_index_ = (block_size == 0 ? 0 : position / block_size);
            subpiece_info.subpiece_index_ = (block_size == 0 ? 0 : (position % block_size) / SUB_PIECE_SIZE);
			return subpiece_info;
		}

		inline bool operator < (const SubPieceInfo& n) const
		{
			if(block_index_ != n.block_index_)
				return block_index_ < n.block_index_;
			else return subpiece_index_ < n.subpiece_index_;
		}

		inline bool operator <= (const SubPieceInfo& n) const
		{
			if(block_index_ != n.block_index_)
				return block_index_ < n.block_index_;
			else return subpiece_index_ <= n.subpiece_index_;
		}

		inline bool operator == (const SubPieceInfo& n) const
		{
			return block_index_ == n.block_index_ && subpiece_index_ == n.subpiece_index_;
		}

		inline u_int GetPieceIndex() const
		{
			return subpiece_index_ / SUB_PIECE_COUNT_PER_PIECE;
		}

		inline u_int GetSubPieceIndexInFile(size_t block_size) const
		{
			assert( block_size % PIECE_SIZE == 0 );
			u_int subpiece_count_in_block = block_size / SUB_PIECE_SIZE;
			return subpiece_count_in_block*block_index_ + subpiece_index_;
		}

		inline u_int GetSubPieceIndexInBlock() const
		{
			return subpiece_index_;
		}

		inline u_int GetSubPieceIndexInPiece() const
		{
			return subpiece_index_ % SUB_PIECE_COUNT_PER_PIECE;
		}

        inline size_t GetPosition(size_t block_size) const
        {
            assert( block_size % PIECE_SIZE == 0 );
            return block_index_*block_size + subpiece_index_*SUB_PIECE_SIZE;
        }

		inline size_t GetEndPosition(size_t block_size) const
		{
			assert( block_size % PIECE_SIZE == 0 );
			return GetPosition(block_size) + SUB_PIECE_SIZE;
		}
	};

	inline ostream& operator << (ostream& os, const SubPieceInfo& n)
	{
		os << "(" << n.block_index_ << "|sp:" << n.subpiece_index_ << ")";
		return os;
	}

	//////////////////////////////////////////////////////////////////////////
	struct PieceInfo
	{
		u_int block_index_;
		u_int piece_index_;

		explicit PieceInfo(u_int block_index = 0, u_int piece_index = 0)
			: block_index_(block_index), piece_index_(piece_index)
		{
		}

		static inline PieceInfo MakeByPosition(size_t position, size_t block_size)
		{
			PieceInfo piece_info;
            piece_info.block_index_ = (block_size == 0 ? 0 : position / block_size);
            piece_info.piece_index_ =(block_size == 0 ? 0 : (position % block_size) / PIECE_SIZE);
			return piece_info;
		}

		inline bool operator < (const PieceInfo& n) const
		{
			if(block_index_ != n.block_index_)
				return block_index_ < n.block_index_;
			else return piece_index_ < n.piece_index_;
		}

		inline bool operator == (const PieceInfo& n) const
		{
			return block_index_ == n.block_index_ && piece_index_ == n.piece_index_;
		}

		inline u_int GetPieceIndexInFile(size_t block_size) const
		{
			assert( block_size % PIECE_SIZE == 0 );
			u_int piece_count_in_block = block_size / PIECE_SIZE;
			return block_index_*piece_count_in_block + piece_index_;
		}

		inline size_t GetPosition(size_t block_size) const
		{
			return block_index_*block_size + piece_index_*PIECE_SIZE;
		}

		inline size_t GetEndPosition(size_t block_size) const
		{
			assert( block_size % PIECE_SIZE == 0 );
			return block_index_*block_size + piece_index_*PIECE_SIZE + PIECE_SIZE;
		}

		inline size_t GetBlockEndPosition(size_t block_size) const
		{
			assert( block_size % PIECE_SIZE == 0 );
			return (block_index_+1)*block_size;
		}
	};

	inline ostream& operator << (ostream& os, const PieceInfo& n)
	{
		os << "(" << n.block_index_ << "|p:" << n.piece_index_ << ")";
		return os;
	}

	//////////////////////////////////////////////////////////////////////////
	struct PieceInfoEx : PieceInfo
	{
		u_short subpiece_index_;		// SubPiece相对于 Piece的位置

		explicit PieceInfoEx(u_int block_index = 0, u_int piece_index = 0, u_short subpiece_index = 0)
			: PieceInfo(block_index, piece_index), subpiece_index_(subpiece_index)
		{
		}

		inline size_t GetPosition(size_t block_size) const
		{
			return PieceInfo::GetPosition(block_size) + subpiece_index_*SUB_PIECE_SIZE;
		}

		inline PieceInfo GetPieceInfo() const
		{
			return PieceInfo(block_index_, piece_index_);
		}
	};

	inline ostream& operator << (ostream& os, const PieceInfoEx& n)
	{
		os << "(" << n.block_index_ << "|p:" << n.piece_index_ << "|sp:" << n.subpiece_index_ << ")";
		return os;
	}

	//////////////////////////////////////////////////////////////////////////
	struct RidInfo
	{
		RID rid_;
		size_t file_length_;
		size_t block_size_;
		u_int block_count_;
		vector<MD5> block_md5_s_;

		explicit RidInfo() : file_length_(0), block_size_(0), block_count_(0) {}

		bool HasRid() const { return ! rid_.IsEmpty(); }
		RID GetRID() const { return rid_; }
		size_t GetFileLength() const { return file_length_; }
		size_t GetBlockSize() const { return block_size_; }
		u_int GetBlockCount() const { return block_count_; }

		void InitByFileLength(size_t file_length)
		{
			file_length_ = file_length;

			if( file_length_ <= BLOCK_MIN_SIZE * BLOCK_MAX_COUNT )
			{	// 启动位置
				block_size_ = BLOCK_MIN_SIZE;
				block_count_ = UpperDiv<u_int>(file_length_, block_size_);
			}
			else
			{	
				block_size_ = UpperDiv<u_int>( file_length_, BLOCK_MAX_COUNT );
				block_size_ = UpperDiv<u_int>( block_size_, PIECE_SIZE ) * PIECE_SIZE;
				block_count_ = UpperDiv<u_int>( file_length_, block_size_);
			}
			assert( block_size_ %  PIECE_SIZE == 0 );
		}
		inline bool operator == (const RidInfo& n) const
		{
			return rid_ == n.rid_ && file_length_ == n.file_length_
				&& block_size_ == n.block_size_ && block_count_ == n.block_count_
				&& block_md5_s_ == n.block_md5_s_ ;
		}
	};

	inline ostream& operator << (ostream& os, const RidInfo& rid_info)
	{
        return os << " ResourceID: "<<rid_info.rid_<<
            ", FileLength: " << rid_info.file_length_<<
            ", BlockSize: " << rid_info.block_size_<<
            ", BlockCount: " << rid_info.block_count_ <<
            ", BlockMD5s: [" << rid_info.block_md5_s_ << "]";
	}

	//////////////////////////////////////////////////////////////////////////
	struct SocketAddr : public protocal::SOCKET_ADDR
	{
		bool operator < (const SocketAddr& socket_addr ) const
		{
			if( IP != socket_addr.IP ) return IP < socket_addr.IP;
			else return Port < socket_addr.Port;
		}

		bool operator ! () const
		{
			return IP == 0 && Port == 0;
		}

		bool operator != (const SocketAddr& socket_addr) const
		{
			return ! (*this == socket_addr);
		}

		SocketAddr& operator = (const SOCKET_ADDR& socket_addr)
		{
			IP = socket_addr.IP;
			Port = socket_addr.Port;
			return *this;
		}

		explicit SocketAddr(const SOCKET_ADDR& socket_addr)
		{
			IP = socket_addr.IP;
			Port = socket_addr.Port;
		}

		explicit SocketAddr(u_int ip, u_short port)
		{
			IP = ip;
			Port = port;
		}

		explicit SocketAddr(const boost::asio::ip::udp::endpoint& end_point)
		{
			IP = end_point.address().to_v4().to_ulong();
			Port = end_point.port();
		}
	};

	inline ostream& operator << (ostream& os, const SocketAddr& socket_addr)
	{
		boost::asio::ip::udp::endpoint ep = network::IpPortToUdpEndpoint(socket_addr.IP, socket_addr.Port);
		return os << ep.address().to_string() << ":" << ep.port();
	}

	//////////////////////////////////////////////////////////////////////////
	struct PeerAddr : public protocal::PEER_ADDR
	{
		bool operator < (const PeerAddr& peer_addr ) const
		{
			if( IP != peer_addr.IP ) return IP < peer_addr.IP;
			else if( UdpPort != peer_addr.UdpPort ) return UdpPort < peer_addr.UdpPort;
			else return TcpPort < peer_addr.TcpPort;
		}

		bool operator ! () const
		{
			return IP == 0 && UdpPort == 0 && TcpPort == 0;
		}

		bool operator != (const PeerAddr& peer_addr) const
		{
			return ! (*this == peer_addr);
		}

		PeerAddr& operator = (const PEER_ADDR& peer_addr)
		{
			IP = peer_addr.IP;
			UdpPort = peer_addr.UdpPort;
			TcpPort = peer_addr.TcpPort;
			return *this;
		}

		explicit PeerAddr(const PEER_ADDR& peer_addr)
		{
			IP = peer_addr.IP;
			UdpPort = peer_addr.UdpPort;
			TcpPort = peer_addr.TcpPort;
		}

		explicit PeerAddr(u_int ip, u_short udp_port, u_short tcp_port)
		{
			IP = ip;
			UdpPort = udp_port;
			TcpPort = tcp_port;
		}
	};

	inline ostream& operator << (ostream& os, const PeerAddr& peer_addr)
	{
		boost::asio::ip::address_v4 ip(peer_addr.IP);
		os << "(" << ip << ":" << peer_addr.UdpPort << ":" << peer_addr.TcpPort << ")";
		return os;
	}


	//////////////////////////////////////////////////////////////////////////
	struct CandidatePeerInfo : public protocal::CANDIDATE_PEER_INFO
	{ 
		bool NeedStunInvoke(UINT32 LocalDetectedIP) const
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

		boost::asio::ip::udp::endpoint GetConnectEndPoint(UINT32 LocalDetectedIP) const 
		{
			if( LocalDetectedIP==0 || LocalDetectedIP!=DetectIP)
				return framework::network::IpPortToUdpEndpoint(DetectIP, DetectUdpPort);
			else
				return framework::network::IpPortToUdpEndpoint(IP, UdpPort);
		}

		boost::asio::ip::udp::endpoint GetStunEndPoint() const
		{
			return framework::network::IpPortToUdpEndpoint(StunIP, StunUdpPort);
		}

		boost::asio::ip::udp::endpoint GetLanUdpEndPoint() const
		{
			return framework::network::IpPortToUdpEndpoint(IP, UdpPort);
		}

		boost::asio::ip::tcp::endpoint GetLanTcpEndPoint() const
		{
			return framework::network::IpPortToTcpEndpoint(IP, TcpPort);
		}

		const PeerAddr& GetPeerInfo() const
		{
			return *(PeerAddr*)((char*)this);
		}

		const SocketAddr& GetSelfSocketAddr() const
		{
			return *(SocketAddr*)((char*)this);
		}

		const SocketAddr& GetDetectSocketAddr() const
		{
			return *(SocketAddr*)(((char*)this) + sizeof(protocal::PEER_ADDR) );
		}

		const SocketAddr& GetKeySocketAddr(UINT32 LocalDetectedIP) const
		{
			if( LocalDetectedIP==0 || LocalDetectedIP!=DetectIP)
				return GetDetectSocketAddr();
			else
				return GetSelfSocketAddr();
		}

		const SocketAddr& GetStunSocketAddr() const
		{
			return *(SocketAddr*)( ((char*)this) + sizeof(protocal::PEER_ADDR) + sizeof(protocal::SOCKET_ADDR) );
		}

		bool operator < (const CandidatePeerInfo& peer ) const
		{
			if( GetDetectSocketAddr() != peer.GetDetectSocketAddr() )
				return GetDetectSocketAddr() < peer.GetDetectSocketAddr();
			else if( GetPeerInfo() != peer.GetPeerInfo() )
				return GetPeerInfo() < peer.GetPeerInfo();
			else
				return GetStunSocketAddr() < peer.GetStunSocketAddr();
			return true;
		}

		bool operator ! () const
		{
			return ! GetPeerInfo() && ! GetDetectSocketAddr() && ! GetStunSocketAddr();
		}

		CandidatePeerInfo& operator = (const CANDIDATE_PEER_INFO& candidate_peer_info)
		{
			IP = candidate_peer_info.IP;
			UdpPort = candidate_peer_info.UdpPort;
			TcpPort = candidate_peer_info.TcpPort;
			DetectIP = candidate_peer_info.DetectIP;
			DetectUdpPort = candidate_peer_info.DetectUdpPort;
			StunIP = candidate_peer_info.StunIP;
			StunUdpPort = candidate_peer_info.StunUdpPort;
			return *this;
		}

		explicit CandidatePeerInfo()
		{
			CandidatePeerInfo(0,0,0);
		}

		explicit CandidatePeerInfo(const CANDIDATE_PEER_INFO& candidate_peer_info)
		{
			IP = candidate_peer_info.IP;
			UdpPort = candidate_peer_info.UdpPort;
			TcpPort = candidate_peer_info.TcpPort;
			DetectIP = candidate_peer_info.DetectIP;
			DetectUdpPort = candidate_peer_info.DetectUdpPort;
			StunIP = candidate_peer_info.StunIP;
			StunUdpPort = candidate_peer_info.StunUdpPort;
		}

		explicit CandidatePeerInfo(u_int ip, u_short udp_port, u_short tcp_port, 
			u_int detect_ip=0, u_short detect_udp_port=0,
			u_int stun_ip=0, u_short stun_udp_port=0)
		{
			IP = ip;
			UdpPort = udp_port;
			TcpPort = tcp_port;
			DetectIP = detect_ip;
			DetectUdpPort = detect_udp_port;
			StunIP = stun_ip;
			StunUdpPort = stun_udp_port;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	struct UrlInfo
	{
		u_short type_;
		string url_;
		string refer_url_;

		enum URL_INFO_TYPE
		{
			HTTP = 0,
            FLASH_MEDIA = 1
		};

		inline bool operator < (const UrlInfo& n) const
		{
			if( type_ != n.type_ )
				return type_ < n.type_;
			//if( url_ != n.url_ )
				return url_ < n.url_;
			//else return refer_url_ < n.refer_url_;
		}

		inline bool operator == (const UrlInfo& n) const
		{
			return type_==n.type_ && url_==n.url_; //&& refer_url_==n.refer_url_;
		}
		explicit UrlInfo() : type_(0) {}
        explicit UrlInfo(const string& url, const string& refer) : type_(0), url_(url), refer_url_(refer) {}
        UrlInfo(const UrlInfo& info) { if (this != &info) { type_ = info.type_; url_ = info.url_; refer_url_ = info.refer_url_; } }

		u_int getlength()
		{
			return url_.length() + refer_url_.length() + 7;
		}

        string GetIdentifiableUrl()
        {
            // youtube
            if (url_.find("googlevideo.com/get_video?video_id=") != string::npos)
            {
                u_int i = url_.find("&signature=");
                if (i != string::npos) return url_.substr(0, i);
            }
            return url_;
        }
	};

	inline ostream& operator << (ostream& os, const UrlInfo& url_info)
	{
		os << "(" << url_info.type_ << ", " << url_info.url_ << ", " << url_info.refer_url_ << ")";
		return os;
	}

	struct ResourceInfo
	{
		RID rid_;
		u_int file_length_;
		u_short block_num_;
		u_int block_size_;
		vector<MD5> md5_s_;
		vector<UrlInfo> url_info_s_;
	};

	inline ostream& operator << (ostream& os, const ResourceInfo& info)
	{
        os << "(" << "ResourceID: " << info.rid_  << ", FileLength: " << info.file_length_ 
            << ", BlockNumber: " << info.block_num_ << ", BlockSize: " << info.block_size_
            << ", BlockMD5s: [" << info.md5_s_ << "], UrlInfos: [" << info.url_info_s_ << "] )";
		return os;
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



}