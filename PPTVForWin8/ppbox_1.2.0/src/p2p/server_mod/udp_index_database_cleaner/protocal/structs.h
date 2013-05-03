#pragma once

#include "base/guid.h"
#include "framework/network/util.h"

using namespace framework::network;

#pragma pack(push, 1)

namespace protocal
{
	struct PACKET_HEADER
	{
		UINT8 Action;
		DWORD TransactionID;
	};

	struct SERVER_PACKET_HEADER
	{
		UINT8 IsRequest;
	};

	struct REQUEST_SERVER_PACKET_HEADER
	{
		UINT32 PeerVersion;
	};

	struct RESPONSE_SERVER_PACKET_HEADER
	{
		UINT8 ErrorCode;
	};

	struct PEER_PACKET_HEADER
	{
		UINT32 SequeceID;
		RID	ResourceID;
		Guid PeerGuid;
	};

	struct SOCKET_ADDR
	{
		UINT32 IP;
		UINT16 Port;

        SOCKET_ADDR(UINT32 ip = 0, UINT16 port = 0) 
            : IP(ip), Port(port) 
        {
        }

		bool operator ==(const SOCKET_ADDR& n) const
		{
			return 0 == memcmp(this, &n, sizeof(SOCKET_ADDR));
		}
	};

    inline ostream& operator << (ostream& os, const SOCKET_ADDR& socket_addr)
    {
        return os << IpPortToUdpEndpoint(socket_addr.IP, socket_addr.Port);
    }

	struct PEER_ADDR
	{
		UINT32 IP;
		UINT16 UdpPort;
		UINT16 TcpPort;

		bool operator ==(const PEER_ADDR& n) const
		{
			return 0 == memcmp(this, &n, sizeof(PEER_ADDR));
		}
	};

	struct CANDIDATE_PEER_INFO
	{
		UINT32 IP;
		UINT16 UdpPort;
		UINT16 TcpPort;
		UINT32 DetectIP;
		UINT16 DetectUdpPort;
		UINT32 StunIP;
		UINT16 StunUdpPort;
		UINT8  Reversed[4];

		bool operator ==(const CANDIDATE_PEER_INFO& n) const
		{
			return 0 == memcmp(this, &n, sizeof(CANDIDATE_PEER_INFO));
		}
		CANDIDATE_PEER_INFO()
		{
			IP = 0;
			UdpPort = 0;
			TcpPort = 0;
			DetectIP = 0;
			DetectUdpPort = 0;
			StunIP = 0;
			StunUdpPort = 0;
			Reversed[0] = 0;
			Reversed[1] = 0;
			Reversed[2] = 0;
			Reversed[3] = 0;
		}
	};

	inline ostream& operator << (ostream& os, const CANDIDATE_PEER_INFO& info)
	{
		return os << "Address: " << IpPortToUdpEndpoint(info.IP, info.UdpPort) 
			<< ", Detected Address: " << IpPortToUdpEndpoint(info.DetectIP, info.DetectUdpPort)
			<< ", Stun Address: " << IpPortToUdpEndpoint(info.StunIP, info.StunUdpPort) 
			<< ", TcpPort: " << info.TcpPort;
	}

	struct PEER_DOWNLOAD_INFO
	{
		UINT8 IsDownloading;
		UINT32 OnlineTime;
		UINT16 AvgDownload;
		UINT16 NowDownload;
		UINT16 AvgUpload;
		UINT16 NowUpload;

		bool operator ==(const PEER_DOWNLOAD_INFO& n) const
		{
			return 0 == memcmp(this, &n, sizeof(PEER_DOWNLOAD_INFO));
		}

		void operator =(const PEER_DOWNLOAD_INFO& n) 
		{
			IsDownloading = n.IsDownloading;
			OnlineTime = n.OnlineTime;
			AvgDownload = n.AvgDownload;
			NowDownload = n.NowDownload;
			AvgUpload = n.AvgUpload;
			NowUpload = n.NowUpload;
		}

		PEER_DOWNLOAD_INFO()
		{
			IsDownloading = 0;
			OnlineTime = 0;
			AvgDownload = 0;
			NowDownload = 0;
			AvgUpload = 0;
			NowUpload = 0;
		}
	};

	struct ERROR_PACKET
	{
		UINT16 ErrorCode;
		UINT16 ErrorInfoLength;
		UINT8 ErrorInfo[];
	};

	struct CONNECT_PACKET
	{
		UINT8 BasicInfo;
		UINT32 SendOffTime;
		UINT32 PeerVersion;
		CANDIDATE_PEER_INFO CandidatePeerInfo;
		UINT8 PeerType;
		PEER_DOWNLOAD_INFO PeerDownloadInfo;

		bool operator ==(const CONNECT_PACKET& n) const
		{
			return 0 == memcmp(this, &n, sizeof(CONNECT_PACKET));
		}
	};

    struct CONNECT_PACKET_EX : public CONNECT_PACKET
    {
        UINT16 IpPoolSize;
    };

	struct ANNOUNCE_PACKET
	{
		PEER_DOWNLOAD_INFO peer_download_info;
		UINT8  BlockMap[];
	};

	struct SUB_PIECE_INFO
	{
		UINT16 BlockIndex;
		UINT16 SubPieceIndex;

		bool operator ==(const SUB_PIECE_INFO& n) const
		{
			return 0 == memcmp(this, &n, sizeof(SUB_PIECE_INFO));
		}

	};

	struct REQUEST_SUBPIECE_PACKET
	{
		UINT16 PieceCount;
		SUB_PIECE_INFO SubPieceInfo[];
		bool operator ==(const REQUEST_SUBPIECE_PACKET& n) const
		{
			return 0 == memcmp(this, &n, sizeof(REQUEST_SUBPIECE_PACKET));
		}
	};

	struct SUBPIECE_PACKET
	{
		SUB_PIECE_INFO SubPieceInfo;
		UINT16 SubPieceLength;
		UINT8 SubPieceContent[];
		bool operator ==(const SUBPIECE_PACKET& n) const
		{
			return 0 == memcmp(this, &n, sizeof(SUBPIECE_PACKET));
		}
	};

	struct PEER_EXCHANGE_PACKET
	{
		UINT8 BasicInfo;
		UINT8 PeerCount;
		CANDIDATE_PEER_INFO CandidatePeerInfo[];
		bool operator ==(const PEER_EXCHANGE_PACKET& n) const
		{
			return 0 == memcmp(this, &n, sizeof(PEER_EXCHANGE_PACKET));
		}
	};

    struct PEER_RIDINFO_RESPONSE_PACKET
    {
        UINT32 FileLength;
        UINT16 BlockCount;
        UINT32 BlockSize;
        MD5    BlockMD5s[];
    };

	struct REQUEST_TRACKER_LIST_PACKET
	{
		RID ResourceID;
		Guid PeerGUID;
		UINT16 RequestPeerCount;
		bool operator ==(const REQUEST_TRACKER_LIST_PACKET& n) const
		{
			return 0 == memcmp(this, &n, sizeof(REQUEST_TRACKER_LIST_PACKET));
		}
	};

	struct RESPONSE_TRACKER_LIST_PACKET
	{
		RID ResourceID;
		UINT16 CandidatePeerCount;
		CANDIDATE_PEER_INFO CandidatePeerInfo[];
		bool operator ==(const RESPONSE_TRACKER_LIST_PACKET& n) const
		{
			return 0 == memcmp(this, &n, sizeof(RESPONSE_TRACKER_LIST_PACKET));
		}
	};

	struct REQUEST_TRACKER_COMMIT_PACKET
	{
		Guid PeerGUID;
		UINT16 UdpPort;
		UINT16 TcpPort;
		UINT32 StunPeerIP;
		UINT16 StunPeerUdpPort;
	};

	struct REQUEST_TRACKER_COMMIT_IP
	{
		UINT8 RealIPCount;
		UINT32 RealIPs[];
	};

	struct REQUEST_TRACKER_COMMIT_RESOURCE
	{
		UINT8 ResourceCount;
		RID ResourceIDs[];
	};

	struct REQUEST_TRACKER_COMMIT_STUN
	{
		UINT16 StunDetectUdpPort;
	};

	struct RESPONSE_TRACKER_COMMIT_PACKET
	{
		UINT16 KeepAliveInterval;
		UINT32 DetectedIP;
		UINT16 DetectedUdpPort;
	};

	struct REQUEST_TRACKER_KEEPALIVE_PACKET
	{
		Guid PeerGUID;
		UINT32 StunPeerIP;
		UINT16 StunPeerUdpPort;
		UINT16 StunDetectUdpPort;
	};

	struct RESPONSE_TRACKER_KEEPALIVE_PACKET
	{
		UINT16 KeepALiveInterval;
		UINT32 DetectedIP;
		UINT16 DetectedUdpPort;
		UINT16 ResourceCount;
	};

	struct REQUEST_TRACKER_LEAVE_PACKET
	{
		Guid PeerGUID;
	};

	struct REQUEST_INDEX_QUERY_HTTP_SERVER_BY_RID
	{
		RID ResourceID;
	};

	class HTTP_SERVER
	{
	public:
		UINT16 Length;
		UINT8 Type;
		string UrlString;
		string ReferString;
	};

	inline ostream& operator << (ostream& os, const HTTP_SERVER& http_server_)
	{
		os<<" type: "<<http_server_.Type<<" url: "<<http_server_.UrlString<< " refer: "<<http_server_.ReferString<<endl;
		return os;
	}

	class RESOURCE_INFO
	{
	public:
		UINT16 Length;
		RID ResourceID;
		UINT32 FileLength;
		UINT16 BlockNum;
		UINT32 BlockSize;
		vector<MD5> BlockMD5;
		vector<HTTP_SERVER> HttpServer;
	};

	struct REQUEST_INDEX_QUERY_RID_BY_URL
	{
		UINT32 SessionID;
	};

	struct REQUEST_INDEX_QUERY_RID_BY_URL_URLSTRING
	{
		UINT16 UrlLength;
		UINT8 UrlString[];
	};

	struct REQUEST_INDEX_QUERY_RID_BY_URL_REFERSTRING
	{
		UINT16 ReferLength;
		UINT8 ReferString[];
	};

	struct REQUEST_INDEX_QUERY_RID_BY_URL_EXTRA
	{
		Guid PeerGuid;
		UINT8 Reverce[10];
	};

	struct RESPONSE_INDEX_QUERY_RID_BY_URL
	{
		UINT32 SessionID;
		RID ResourceID;
		UINT32 DetectedIP;
		UINT16 detectedPort;
		UINT32 FileLength;
		UINT16 BlockNum;
		UINT32 BlockSize;
		MD5 BlockMD5[];
	};

	struct RESPONSE_INDEX_QUERY_RID_BY_URL_CONTENT
	{
		MD5 ContentSenseMD5;
		UINT32 ContentBytes;
	};

	struct RESPONSE_INDEX_ADD_RID_URL
	{
		UINT8 Status;
	};

	struct REQUEST_INDEX_ADD_RID_URL
	{
		Guid ContentSenseMD5;
		UINT32 ContentBytes;
	};

	struct REQUEST_INDEX_QUERY_RID_BY_CONTENT
	{
		MD5 ContentSenseMD5;
		UINT32 ContentBytes;
		UINT32 FileLength;
		Guid PeerGuid;
		UINT8 Reverce[10];
	};

	struct TRACKER_INFO
	{
		UINT16 Length;
		UINT8 ModNo;
		UINT32 IP;
		UINT16 Port;
		UINT8 Type;

		bool operator == (const TRACKER_INFO& n) const
		{
			return 0 == memcmp(this, &n, sizeof(TRACKER_INFO));
		}
		bool operator < (const TRACKER_INFO& n) const
		{
			return memcmp(this, &n, sizeof(TRACKER_INFO)) < 0;
		}
	};

	struct RESPONSE_INDEX_QUERY_TRACKER_LIST
	{
		UINT16 TrackerGroupCount;
		UINT16 TrackerCount;
		TRACKER_INFO TrackerInfo[];
	};

	struct REQUEST_INDEX_QUERY_TRACKER_LIST
	{
		Guid PeerGUID;
	};

	struct STUN_SERVER_INFO
	{
		UINT16 Length;
		UINT32 IP;
		UINT16 Port;
		UINT8 Type;

		bool operator == (const TRACKER_INFO& n) const
		{
			return 0 == memcmp(this, &n, sizeof(TRACKER_INFO));
		}
		bool operator < (const TRACKER_INFO& n) const
		{
			return memcmp(this, &n, sizeof(TRACKER_INFO)) < 0;
		}
	};

	inline ostream& operator << (ostream& os, const TRACKER_INFO& info)
	{
		boost::asio::ip::udp::endpoint ep = network::IpPortToUdpEndpoint(info.IP, info.Port);
        return os << ep << ", " << (u_int) info.ModNo << ", " << (info.Type ? "UDP" : "TCP");
	}
	
	inline ostream& operator << (ostream& os, const STUN_SERVER_INFO& info)
	{
		boost::asio::ip::udp::endpoint ep = network::IpPortToUdpEndpoint(info.IP, info.Port);
		return os << ep << ", " << (info.Type ? "UDP" : "TCP");
	}

	struct RESPONSE_INDEX_QUERY_STUN_SERVER_LIST
	{
		UINT16 StunServerCount;
		STUN_SERVER_INFO StunServerInfo[];
	};

	struct RESPONSE_STUN_HANDSHAKE
	{
		UINT16 KeepALiveInterval;
		UINT32 DetectedIP;
		UINT16 detectedPort;
	};

	struct REQUEST_STUN_INVOKE
	{
		RID ResourceID;
		Guid PeerGuid;
		UINT32 SendOffTime;
		CANDIDATE_PEER_INFO CandidatePeerInfoMine;
		UINT8 PeerTypeMine;
		CANDIDATE_PEER_INFO CandidatePeerInfoHis;
		PEER_DOWNLOAD_INFO PeerDownloadInfoMine;
	};

    struct REQUEST_STUN_INVOKE_EX : public REQUEST_STUN_INVOKE
    {
        UINT16 IpPoolSize;
    };

	struct REQUEST_TRACKER_REPORT_PACKET
	{
		Guid PeerGUID;
		UINT16 LocalResourceCount;
		UINT16 ServerResourceCount;
        UINT8  PeerNatType;
		UINT8  Reversed[5];
		UINT16 UdpPort;
		UINT16 TcpPort;
		UINT32 StunPeerIP;
		UINT16 StunPeerUdpPort;
		UINT16 StunDetectedUdpPort;
	};

	struct REQUEST_TRACKER_REPORT_IP
	{
		UINT8 RealIPCount;
		UINT32 RealIPs[];
	};

	struct REPORT_RESOURCE_STRUCT
	{
		Guid ResourceID;
		UINT8 Type;
	};

    inline ostream& operator << (ostream& os, const REPORT_RESOURCE_STRUCT& reports)
    {
        return os << (reports.Type == 1 ? "ADD" : (reports.Type == 0 ? "DEL" : "ERR")) 
            << " " << reports.ResourceID;
    }

	struct REQUEST_TRACKER_REPORT_RESOURCE
	{
		UINT8 ResourceCount;
		REPORT_RESOURCE_STRUCT ReportResources[];
	};

    struct REQUEST_TRACKER_QUERY_PEER_COUNT_PACKET
    {
        Guid ResourceID;
    };

    struct RESPONSE_TRACKER_QUERY_PEER_COUNT_PACKET
    {
        Guid ResourceID;
        UINT32 PeerCount;
    };

    //////////////////////////////////////////////////////////////////////////
    // Statistic Packet Structs

    struct STATISTIC_HEADER
    {
        UINT64 TimeStamp;
        Guid PeerGuid;
        UINT8 PeerAction;
    };

    struct STATISTIC_INDEX_URL
    {
        UINT16 UrlLength;
        UINT8 UrlString[];
    };

    struct STATISTIC_INDEX_CONTENT
    {
        Guid ContentID;
    };

    struct STATISTIC_INDEX_RESOURCE
    {
        Guid ResourceID;
    };

    //////////////////////////////////////////////////////////////////////////
    // QueryIndexServerList

    struct INDEX_SERVER_INFO
    {
        UINT16 Length;
        UINT32 IP;
        UINT16 Port;
        UINT8 Type;

		INDEX_SERVER_INFO() : Type(1) {}

        INDEX_SERVER_INFO(UINT32 ip, UINT16 port, UINT8 type = 1)
        {
            this->Length = sizeof(INDEX_SERVER_INFO);
            this->IP = ip;
            this->Port = port;
            this->Type = type;
        }

        bool operator == (const INDEX_SERVER_INFO& n) const
        {
            return 0 == memcmp(this, &n, sizeof(INDEX_SERVER_INFO));
        }
        bool operator < (const INDEX_SERVER_INFO& n) const
        {
            return memcmp(this, &n, sizeof(INDEX_SERVER_INFO)) < 0;
        }
    };

    struct REQUEST_INDEX_QUERY_INDEXSERVER_PACKET
    {
        Guid PeerGuid;
    };

    #define INDEX_LIST_MOD_COUNT 256

    struct RESPONSE_INDEX_QUERY_INDEXSERVER_PACKET
    {
        UINT8 ModIndexMap[INDEX_LIST_MOD_COUNT];
        UINT8 IndexServerCount;
        INDEX_SERVER_INFO IndexServerInfos[];
    };

    //////////////////////////////////////////////////////////////////////////
    // QueryTestUrlList
    struct STRING
    {
        UINT16 Length;
        UINT8 StringData[];

        string AsString() const {
            return string((const char*)StringData, Length);
        }
        void FromString(const string& str) {
            // be sure there is enough memory
            Length = str.length();
            memcpy(StringData, str.c_str(), str.length());
        }
	};

    struct REQUEST_INDEX_QUERY_STRING_LIST_PACKET
    {
        Guid PeerGuid;
    };

    struct RESPONSE_INDEX_QUERY_STRING_LIST_PACKET
    {
        UINT16 StringCount;
        //STRING StringList[];
    };

    //////////////////////////////////////////////////////////////////////////
    // QueryNeedReport
    struct REQUEST_INDEX_QUERY_NEEDREPORT_PACKET
    {
        Guid PeerGuid;
    };
    struct RESPONSE_INDEX_QUERY_NEEDREPORT_PACKET
    {
        UINT8 NeedReport;
    };

    //////////////////////////////////////////////////////////////////////////
    // QueryDataCollectionServer

    struct DATACOLLECTION_SERVER_INFO
    {
        UINT16 Length;
        UINT32 IP;
        UINT16 Port;
        UINT8 Type;

        DATACOLLECTION_SERVER_INFO() : Type(1) {}

        DATACOLLECTION_SERVER_INFO(UINT32 ip, UINT16 port, UINT8 type = 1)
        {
            this->Length = sizeof(DATACOLLECTION_SERVER_INFO);
            this->IP = ip;
            this->Port = port;
            this->Type = type;
        }

        bool operator == (const DATACOLLECTION_SERVER_INFO& n) const
        {
            return 0 == memcmp(this, &n, sizeof(DATACOLLECTION_SERVER_INFO));
        }
        bool operator < (const DATACOLLECTION_SERVER_INFO& n) const
        {
            return memcmp(this, &n, sizeof(DATACOLLECTION_SERVER_INFO)) < 0;
        }
    };

    struct REQUEST_INDEX_QUERY_DATACOLLECTION_PACKET
    {
        Guid PeerGuid;
    };
    struct RESPONSE_INDEX_QUERY_DATACOLLECTION_PACKET
    {
        DATACOLLECTION_SERVER_INFO DataCollectionServerInfo;
    };

    //////////////////////////////////////////////////////////////////////////
    // RREQUEST_DATACOLLECTION_PACKET
    struct VERSION_INFO {
        UINT8 Major;
        UINT8 Minor;
        UINT8 Micro;
        UINT8 Extra;

        VERSION_INFO() { memset(this, 0, sizeof(VERSION_INFO)); }
        VERSION_INFO(UINT8 major, UINT8 minor, UINT8 micro, UINT8 extra)
            : Major(major), Minor(minor), Micro(micro), Extra(extra)
        {}
    };
    inline int compare(const VERSION_INFO& a, const VERSION_INFO& b)
    {
        if (a.Major != b.Major) return a.Major < b.Major ? -1 : 1;
        if (a.Minor != b.Minor) return a.Minor < b.Minor ? -1 : 1;
        if (a.Micro != b.Micro) return a.Micro < b.Micro ? -1 : 1;
        if (a.Extra != b.Extra) return a.Extra < b.Extra ? -1 : 1;
        return 0;
    }
    inline bool operator < (const VERSION_INFO& a, const VERSION_INFO& b)
    {
        return compare(a, b) < 0;
    }
    inline bool operator == (const VERSION_INFO& a, const VERSION_INFO& b)
    {
        return compare(a, b) == 0;
    }
    struct REQUEST_DATACOLLECTION_PACKET
    {
        VERSION_INFO KernelVersion;
        Guid PeerGuid;
    };

    enum DOWNLOAD_STATUS_TYPE : UINT8
    {
        RANGE_P2P = 1,
        RANGE_HTTP = 2,
        RANGE_P2P_HTTP = 3,
        NORANGE_P2P = 11,
        NORANGE_HTTP = 12,
        NORANGE_P2P_HTTP = 13
    };

    //////////////////////////////////////////////////////////////////////////
    // Data On App STop
    struct DATA_ONAPPSTOP_INFO
    {
        UINT16  MaxUploadSpeedInKBps;
        UINT16  MaxDownloadSpeedInKBps;
        UINT16  DiskUsedSizeInMB;
        UINT8   UploadCacheHitRate;
        UINT16  UploadDataBytesInMB;
        UINT16  DownloadDataBytesInMB;
        UINT16  DownloadDurationInSecond;
        UINT16  RunningDurationInMinute;
        UINT8   CpuPeakValue;
        UINT8   MemPeakValueInMB;
        UINT16  IncomingPeersCount;

        DATA_ONAPPSTOP_INFO() { memset(this, 0, sizeof(DATA_ONAPPSTOP_INFO)); }
    };

    struct PLATFORM_INFO
    {
        UINT8 Platform;
        UINT8 SystemVersion;
        UINT8 SystemLanguage;

        PLATFORM_INFO() { memset(this, 0xFF, sizeof(PLATFORM_INFO)); }
    };

    const u_int ACTION_NUMBER = 15;

    struct ACTION_COUNT_INFO
    {
        UINT32 ActionCount[ACTION_NUMBER * 2];

        ACTION_COUNT_INFO() { memset(this, 0xFF, sizeof(ACTION_COUNT_INFO)); }
    };

    //////////////////////////////////////////////////////////////////////////
    // Data On Download Stop
    struct PLAY_STOP_INFO
    {
        UINT32  FileOffset;
        UINT16  DurationInSecond;
        UINT16  TimeOffsetInSecond;

        PLAY_STOP_INFO() { memset(this, 0, sizeof(PLAY_STOP_INFO)); }
    };

    struct DATA_ONDOWNLOADSTOP_INFO
    {
        UINT16  IPPoolSize;
        UINT8   ConnectedPeersCount;
        UINT8   FullPeersCount;
        UINT8   IsDownloadFinished;
        UINT16  AvgDownloadSpeedInKBps;
        UINT16  MaxHttpDownloadSpeedInKBps;
        UINT16  MaxP2PDownloadSpeedInKBps;
        UINT16  AvgHttpSpeedInKBps;
        UINT16  AvgP2PSpeedInKBps;
        UINT32  HttpDataBytes;
        UINT32  P2PDataBytes;
        UINT8   PacketLostRate;
        UINT8   RedundantRate;
        UINT8   DownloadStatus;
        UINT32  FlvFileLength;
        UINT32  FlvDataRate;
        UINT16  FlvTimeDurationInSecond;

        DATA_ONDOWNLOADSTOP_INFO() { memset(this, 0, sizeof(DATA_ONDOWNLOADSTOP_INFO)); }
    };

    struct DATA_ONDOWNLOADSTOP_INFO_EX : public DATA_ONDOWNLOADSTOP_INFO
    {
        UINT8  HasP2P;
        RID Rid;

        UINT16 IPPoolSize_s1;
        UINT16 IPPoolSize_s2;
        UINT16 IPPoolSize_s3;
        UINT8  ConnectedPeersCount_s1;
        UINT8  ConnectedPeersCount_s2;
        UINT8  ConnectedPeersCount_s3;
        UINT16 P2PDownloadSpeedInKBps_s1;
        UINT16 P2PDownloadSpeedInKBps_s2;
        UINT16 P2PDownloadSpeedInKBps_s3;
        UINT16 ConnectSendCount_s1;
        UINT16 ConnectSendCount_s2;
        UINT16 ConnectSendCount_s3;
        UINT16 ConnectErrFullCount_s1;
        UINT16 ConnectErrFullCount_s2;
        UINT16 ConnectErrFullCount_s3;
        UINT16 ConnectErrOtherCount_s1;
        UINT16 ConnectErrOtherCount_s2;
        UINT16 ConnectErrOtherCount_s3;

        DATA_ONDOWNLOADSTOP_INFO_EX() { memset(this, 0, sizeof(DATA_ONDOWNLOADSTOP_INFO_EX)); }
    };

    // 
    struct DATA_ITEM
    {
        UINT8  ITEM_TYPE;
        UINT8  ITEM_SIZE;
        UINT8  DATA[];
    };

    struct DATA_ITEM_ARRAY
    {
        UINT8     ITEMS_COUNT;
        UINT32    ITEMS_SIZE;
        UINT8     ITEMS[];
    };

    enum DATA_TYPE : UINT8
    {
        IP_POOL_SIZE,
        CONNECTED_PEERS_COUNT,
        FULL_PEERS_COUNT,
        IS_DOWNLOAD_FINISHED,
        AVG_DOWNLOAD_SPEED_IN_KPS,
        MAX_HTTP_DOWNLOAD_SPEED_IN_KPS,
        MAX_P2P_DOWNLOAD_SPEED_IN_KPS,
        AVG_HTTP_SPEED_IN_KPS,
        AVG_P2P_SPEED_IN_KPS,
        HTTP_DATA_BYTES,
        P2P_DATA_BYTES,
        PACKET_LOST_RATE,
        REDUNDANT_RATE,
        DOWNLOAD_STATUS,
        FLV_FILE_LENGTH,
        FLV_DATA_RATE,
        FLV_TIME_DURATION_IN_SECONDS,
        HAS_P2P,
        RESOURCE_ID,

        IP_POOL_SIZE_s1,
        IP_POOL_SIZE_s2,
        IP_POOL_SIZE_s3,
        CONNECTED_PEERS_COUNT_s1,
        CONNECTED_PEERS_COUNT_s2,
        CONNECTED_PEERS_COUNT_s3,
        P2P_DOWNLOAD_SPEED_IN_KPS_s1,
        P2P_DOWNLOAD_SPEED_IN_KPS_s2,
        P2P_DOWNLOAD_SPEED_IN_KPS_s3,
        CONNECT_SEND_COUNT_s1,
        CONNECT_SEND_COUNT_s2,
        CONNECT_SEND_COUNT_s3,
        CONNECT_ERR_FULL_COUNT_s1,
        CONNECT_ERR_FULL_COUNT_s2,
        CONNECT_ERR_FULL_COUNT_s3,
        CONNECT_ERR_OTHER_COUNT_s1,
        CONNECT_ERR_OTHER_COUNT_s2,
        CONNECT_ERR_OTHER_COUNT_s3,

        //
        DATA_TYPE_COUNT,
    };

    struct PLAY_STOP_INFO_ARRAY
    {
        UINT8   PlayStopCount;
        PLAY_STOP_INFO PlayStopInfos[];
    };

    //////////////////////////////////////////////////////////////////////////
    // ¸ÅÂÊ
    struct REQUEST_INDEX_QUERY_UPLOADPICPROBABLITY_PACKET
    {
        Guid PeerGuid;
    };
    struct RESPONSE_INDEX_QUERY_UPLOADPICPROBABLITY_PACKET
    {
        FLOAT UploadPicProbability;
    };

}

#pragma pack(pop)