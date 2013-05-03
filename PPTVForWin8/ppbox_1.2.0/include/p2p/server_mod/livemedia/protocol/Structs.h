#ifndef PROTOCOL_STRUCTS_H
#define PROTOCOL_STRUCTS_H

#include <util/serialization/stl/vector.h>
#include <framework/string/Uuid.h>

typedef framework::string::Uuid Guid;
typedef Guid RID;

namespace protocol
{

    struct String
		: std::string
    {
        template <typename Archive>
        void serialize(Archive & ar)
        {
            boost::uint16_t len = size();
            ar & len;
            resize(len);
            ar & framework::container::make_array(&operator[](0),len);
        }
    };

    struct PEER_DOWNLOAD_INFO
    {
        boost::uint8_t IsDownloading;
        boost::uint32_t OnlineTime;
        boost::uint16_t AvgDownload;
        boost::uint16_t NowDownload;
        boost::uint16_t AvgUpload;
        boost::uint16_t NowUpload;

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & IsDownloading & OnlineTime & AvgDownload & NowDownload & AvgUpload & NowUpload;
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

    // @since 0x0000000A
    struct PEER_COUNT_INFO
    {
        boost::uint8_t  ConnectedPeersCount;
        boost::uint8_t  ActivePeersCount;
        boost::uint16_t PooledPeersCount;

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & ConnectedPeersCount & ActivePeersCount & PooledPeersCount; 
        }

        PEER_COUNT_INFO() {
            ConnectedPeersCount = 0;
            ActivePeersCount = 0;
            PooledPeersCount = 0;
        }
    };

    struct TRACKER_INFO
    {
        boost::uint16_t Length;
        boost::uint8_t ModNo;
        boost::uint32_t IP;
        boost::uint16_t Port;
        boost::uint8_t Type; // 1 UDP

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & Length & ModNo & IP & Port & Type ; 
        }

        bool operator == (const TRACKER_INFO& n) const
        {
            return 0 == memcmp(this, &n, sizeof(TRACKER_INFO));
        }
        bool operator < (const TRACKER_INFO& n) const
        {
            return memcmp(this, &n, sizeof(TRACKER_INFO)) < 0;
        }
        TRACKER_INFO& operator = (const TRACKER_INFO& tracker)
        {
            assert((unsigned long)(&tracker) % 4 == 0);
            if (this != &tracker) {
                Length = tracker.Length;
                ModNo = tracker.ModNo;
                IP = tracker.IP;
                Port = tracker.Port;
                Type = tracker.Type;
            }
            return *this;
        }
    };

    struct STUN_SERVER_INFO
    {
        boost::uint16_t Length;
        boost::uint32_t IP;
        boost::uint16_t Port;
        boost::uint8_t Type;

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & Length & IP & Port & Type; 
        }

        bool operator == (const STUN_SERVER_INFO& n) const
        {
            return 0 == memcmp(this, &n, sizeof(STUN_SERVER_INFO));
        }
        bool operator < (const STUN_SERVER_INFO& n) const
        {
            return memcmp(this, &n, sizeof(STUN_SERVER_INFO)) < 0;
        }
    };

    struct REPORT_RESOURCE_STRUCT
    {
		Guid ResourceID;
        boost::uint8_t Type;

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & ResourceID & Type ; 
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // Statistic Packet Structs

    struct STATISTIC_HEADER
    {
        boost::uint64_t TimeStamp;
        Guid PeerGuid;
        boost::uint8_t PeerAction;

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & TimeStamp & PeerGuid & PeerAction ; 
        }
    };

    struct STATISTIC_INDEX_URL
    {
        std::vector<boost::uint8_t> UrlString;

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & util::serialization::make_sized<boost::uint16_t>(UrlString) ; 
        }
    };

    struct STATISTIC_INDEX_CONTENT
    {
        Guid ContentID;

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & ContentID  ; 
        }
    };

    struct STATISTIC_INDEX_RESOURCE
    {
        Guid ResourceID;

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & ResourceID  ; 
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // QueryIndexServerList

    struct INDEX_SERVER_INFO
    {
        boost::uint16_t Length;
        boost::uint32_t IP;
        boost::uint16_t Port;
        boost::uint8_t Type;

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & Length & IP & Port & Type  ; 
        }

        INDEX_SERVER_INFO() : Type(1) {}

        INDEX_SERVER_INFO(boost::uint32_t ip, boost::uint16_t port, boost::uint8_t type = 1)
        {
            this->Length = sizeof(INDEX_SERVER_INFO);
            this->IP = ip;
            this->Port = port;
            this->Type = type;
        }

        bool operator < (const INDEX_SERVER_INFO& n) const
        {
            return memcmp(this, &n, sizeof(INDEX_SERVER_INFO)) < 0;
        }
    };

    const boost::uint32_t INDEX_LIST_MOD_COUNT = 256u;

    struct DATACOLLECTION_SERVER_INFO
    {
        boost::uint16_t Length;
        boost::uint32_t IP;
        boost::uint16_t Port;
        boost::uint8_t Type;

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & Length & IP & Port & Type ; 
        }

        DATACOLLECTION_SERVER_INFO() : Type(1) {}

        DATACOLLECTION_SERVER_INFO(boost::uint32_t ip, boost::uint16_t port, boost::uint8_t type = 1)
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

    //////////////////////////////////////////////////////////////////////////
    // RREQUEST_DATACOLLECTION_PACKET
    struct VERSION_INFO {
        boost::uint8_t Major;
        boost::uint8_t Minor;
        boost::uint8_t Micro;
        boost::uint8_t Extra;

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & Major & Minor & Micro & Extra; 
        }

        VERSION_INFO() { memset(this, 0, sizeof(VERSION_INFO)); }
        VERSION_INFO(boost::uint8_t major, boost::uint8_t minor, boost::uint8_t micro, boost::uint8_t extra)
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

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & KernelVersion & PeerGuid; 
        }
    };

    enum DOWNLOAD_STATUS_TYPE //: boost::uint8_t
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
        boost::uint16_t  MaxUploadSpeedInKBps;
        boost::uint16_t  MaxDownloadSpeedInKBps;
        boost::uint16_t  DiskUsedSizeInMB;
        boost::uint8_t   UploadCacheHitRate;
        boost::uint16_t  UploadDataBytesInMB;
        boost::uint16_t  DownloadDataBytesInMB;
        boost::uint16_t  DownloadDurationInSecond;
        boost::uint16_t  RunningDurationInMinute;
        boost::uint8_t   CpuPeakValue;
        boost::uint8_t   MemPeakValueInMB;
        boost::uint16_t  IncomingPeersCount;

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & MaxUploadSpeedInKBps & MaxDownloadSpeedInKBps & DiskUsedSizeInMB & UploadCacheHitRate & UploadDataBytesInMB & DownloadDataBytesInMB
                & DownloadDurationInSecond & RunningDurationInMinute & CpuPeakValue & MemPeakValueInMB & IncomingPeersCount ; 
        }

        DATA_ONAPPSTOP_INFO() { memset(this, 0, sizeof(DATA_ONAPPSTOP_INFO)); }
    };


    struct PLATFORM_INFO
    {
        boost::uint8_t Platform;
        boost::uint8_t SystemVersion;
        boost::uint8_t SystemLanguage;

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & Platform & SystemVersion & SystemLanguage  ; 
        }

        PLATFORM_INFO() { memset(this, 0xFF, sizeof(PLATFORM_INFO)); }
    };


    struct ACTION_COUNT_INFO
    {
        static boost::uint32_t const ACTION_NUMBER = 15;
        boost::uint32_t ActionCount[ACTION_NUMBER * 2];

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar&framework::container::make_array(ActionCount, ACTION_NUMBER * 2); 
        }

        ACTION_COUNT_INFO()
        {
            memset(this, 0xFF, sizeof(ACTION_COUNT_INFO));
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // Data On Download Stop
    struct PLAY_STOP_INFO
    {
        boost::uint32_t  FileOffset;
        boost::uint16_t  DurationInSecond;
        boost::uint16_t  TimeOffsetInSecond;

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & FileOffset & DurationInSecond & TimeOffsetInSecond  ; 
        }

        PLAY_STOP_INFO() { memset(this, 0, sizeof(PLAY_STOP_INFO)); }
    };

    struct DATA_ONDOWNLOADSTOP_INFO
    {
        boost::uint16_t  IPPoolSize;
        boost::uint8_t   ConnectedPeersCount;
        boost::uint8_t   FullPeersCount;
        boost::uint8_t   IsDownloadFinished;
        boost::uint16_t  AvgDownloadSpeedInKBps;
        boost::uint16_t  MaxHttpDownloadSpeedInKBps;
        boost::uint16_t  MaxP2PDownloadSpeedInKBps;
        boost::uint16_t  AvgHttpSpeedInKBps;
        boost::uint16_t  AvgP2PSpeedInKBps;
        boost::uint32_t  HttpDataBytes;
        boost::uint32_t  P2PDataBytes;
        boost::uint8_t   PacketLostRate;
        boost::uint8_t   RedundantRate;
        boost::uint8_t   DownloadStatus;
        boost::uint32_t  FlvFileLength;
        boost::uint32_t  FlvDataRate;
        boost::uint16_t  FlvTimeDurationInSecond;

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & IPPoolSize & ConnectedPeersCount & FullPeersCount & IsDownloadFinished & AvgDownloadSpeedInKBps & MaxHttpDownloadSpeedInKBps
                & MaxP2PDownloadSpeedInKBps & AvgHttpSpeedInKBps & HttpDataBytes & P2PDataBytes & PacketLostRate & RedundantRate & DownloadStatus 
                & FlvFileLength & FlvDataRate & FlvTimeDurationInSecond; 
        }

        DATA_ONDOWNLOADSTOP_INFO() { memset(this, 0, sizeof(DATA_ONDOWNLOADSTOP_INFO)); }
    };

    struct DATA_ONDOWNLOADSTOP_INFO_EX : public DATA_ONDOWNLOADSTOP_INFO
    {
        boost::uint8_t  HasP2P;
        RID Rid;

        boost::uint16_t IPPoolSize_s1;
        boost::uint16_t IPPoolSize_s2;
        boost::uint16_t IPPoolSize_s3;
        boost::uint8_t  ConnectedPeersCount_s1;
        boost::uint8_t  ConnectedPeersCount_s2;
        boost::uint8_t  ConnectedPeersCount_s3;
        boost::uint16_t P2PDownloadSpeedInKBps_s1;
        boost::uint16_t P2PDownloadSpeedInKBps_s2;
        boost::uint16_t P2PDownloadSpeedInKBps_s3;
        boost::uint16_t ConnectSendCount_s1;
        boost::uint16_t ConnectSendCount_s2;
        boost::uint16_t ConnectSendCount_s3;
        boost::uint16_t ConnectErrFullCount_s1;
        boost::uint16_t ConnectErrFullCount_s2;
        boost::uint16_t ConnectErrFullCount_s3;
        boost::uint16_t ConnectErrOtherCount_s1;
        boost::uint16_t ConnectErrOtherCount_s2;
        boost::uint16_t ConnectErrOtherCount_s3;

        DATA_ONDOWNLOADSTOP_INFO_EX() { memset(this, 0, sizeof(DATA_ONDOWNLOADSTOP_INFO_EX)); }
    };

    // 
    struct DATA_ITEM
    {
        boost::uint8_t  ITEM_TYPE;
        std::vector<boost::uint8_t> DATA;

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & ITEM_TYPE & util::serialization::make_sized<boost::uint8_t>(DATA); 
        }
    };

    struct DATA_ITEM_ARRAY
    {
        boost::uint8_t     ITEMS_COUNT;
        boost::uint32_t    ITEMS_SIZE;
        //      boost::uint8_t     ITEMS[];
        std::vector<boost::uint8_t> ITEMS;

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & ITEMS_COUNT
                & ITEMS_SIZE;
            util::serialization::serialize_collection(ar, ITEMS, ITEMS_COUNT * ITEMS_SIZE); 
        }
    };

    enum DATA_TYPE //: boost::uint8_t
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
        //       PLAY_STOP_INFO PlayStopInfos[];
        std::vector<PLAY_STOP_INFO> PlayStopInfos;

        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & util::serialization::make_sized<boost::uint8_t>(PlayStopInfos); 
        }
    };

	enum FORCE_MODE_TYPE
	{
		FORCE_MODE_NORMAL,
		FORCE_MODE_P2P_ONLY,
		FORCE_MODE_HTTP_ONLY,
		FORCE_MODE_P2P_TEST
	};

    enum TASK_TYPE //: boost::uint16_t
    {
        TASK_GENERIC,            // 一般任务
        TASK_OPEN_SERVICE,       // 开放服务
		TASK_TEST_OPENSERVICE,   // 测试下载
    };

	std::string TaskTypeString(TASK_TYPE type);

    struct PUSH_TASK_PARAM
    {
        boost::uint16_t TaskType;

        boost::uint16_t ProtectTimeIntervalInSeconds;

        boost::uint16_t MinDownloadSpeedInKBps;
        boost::uint16_t MaxDownloadSpeedInKBpsWhenIdle;
        boost::uint16_t MaxDownloadSpeedInKBpsWhenNormal;

        boost::uint8_t BandwidthRatioWhenIdle; // v = BandwidthRatioWhenIdle * 1.0 / 255;
        boost::uint8_t BandwidthRatioWhenNormal;

		template <typename Archive>
		void serialize(Archive & ar)
		{
			ar & TaskType & ProtectTimeIntervalInSeconds & MinDownloadSpeedInKBps & MaxDownloadSpeedInKBpsWhenIdle
				& MaxDownloadSpeedInKBpsWhenNormal & BandwidthRatioWhenIdle	& BandwidthRatioWhenNormal; 
		}

        PUSH_TASK_PARAM() {
            memset(this, 0, sizeof(PUSH_TASK_PARAM));
        }
    };

    inline std::ostream& operator << (std::ostream& os, const PUSH_TASK_PARAM& param)
    {
        return os
            << "{"
            << "TaskType: '" << TaskTypeString((TASK_TYPE)param.TaskType) << "', "
            << "ProtectionTimeIntervalInSeconds: " << param.ProtectTimeIntervalInSeconds << ", "
            << "MinDownloadSpeedInKBps: " << param.MinDownloadSpeedInKBps << ", "
            << "MaxDownloadSpeedInKBpsWhenIdle: " << param.MaxDownloadSpeedInKBpsWhenIdle << ", "
            << "MaxDownloadSpeedInKBpsWhenNormal: " << param.MaxDownloadSpeedInKBpsWhenNormal << ", "
            << "BandwidthRatioWhenIdle: " << (boost::uint32_t)param.BandwidthRatioWhenIdle << " / " << (param.BandwidthRatioWhenIdle * 100.0 
            / 255) << "%, "
            << "BandwidthRatioWhenNormal: " << (boost::uint32_t)param.BandwidthRatioWhenNormal << " / " << (param.BandwidthRatioWhenNormal *
            100.0 / 255) << "% "
            << "}"
            ;
    }

	struct NodeInfo
	{
		Guid PeerGuid;
		boost::uint32_t InternalIP;
		boost::uint16_t InternalPort;
		boost::uint32_t DetectIP;
		boost::uint16_t DetectPort;
		boost::uint32_t StunIP;
		boost::uint16_t StunPort;

		template <typename Archive>
		void serialize(Archive & ar)
		{
			ar & PeerGuid & InternalIP & InternalPort & DetectIP & DetectPort & StunIP
				& StunPort; 
		}

		NodeInfo() { memset(this, 0, sizeof(NodeInfo)); }
	};

	struct TASK_INFO
	{
		boost::uint32_t TaskID;
		boost::uint32_t CompleteCount;

		template <typename Archive>
		void serialize(Archive & ar)
		{
			ar & TaskID & CompleteCount; 
		}

		TASK_INFO() { memset(this, 0, sizeof(TASK_INFO)); }
	};


	/*struct JOIN_RET_INFO
	{
		Guid  peer_guid_;//自己的guid
		boost::uint8_t  ret_;
		//UINT8  is_server_node_;//1表示服务器，0表示其他客户端
		boost::uint8_t  node_count_;
		NodeInfo  node_vec_[];
	};*/

	/*struct NOTIFY_SERVER_INFO
	{
		UINT32 IP;
		UINT16 Port;

		bool operator == (const NOTIFY_SERVER_INFO& n) const
		{
			return 0 == memcmp(this, &n, sizeof(NOTIFY_SERVER_INFO));
		}
		bool operator < (const NOTIFY_SERVER_INFO& n) const
		{
			return memcmp(this, &n, sizeof(NOTIFY_SERVER_INFO)) < 0;
		}
		NOTIFY_SERVER_INFO& operator = (const NOTIFY_SERVER_INFO& notify)
		{
			if (this != &notify) {
				IP = notify.IP;
				Port = notify.Port;
			}
			return *this;
		}
	};*/

	struct NOTIFY_SERVER_INFO
	{
		boost::uint32_t IP;
		boost::uint16_t Port;
		
		template <typename Archive>
		void serialize(Archive & ar)
		{
			ar & IP & Port; 
		}

		NOTIFY_SERVER_INFO() { memset(this, 0, sizeof(TASK_INFO)); }

	};
	struct REGION_INFO
	{
		boost::uint32_t  PublicIP;
		boost::uint32_t  RigionCode;
		boost::uint16_t  Version;

		template <typename Archive>
			void serialize(Archive & ar)
		{
			ar & PublicIP & RigionCode & Version; 
		}

		REGION_INFO() {
			memset(this, 0, sizeof(REGION_INFO));
		}
	};

	enum CONNECT_TYPE //: boost::uint8_t
	{
		CONNECT_TCP,
		CONNECT_UDP,
	};  

	enum SERVER_TYPE //: boost::uint8_t
	{
		SERVER_STUN = 0x01,
		SERVER_CACHE,
		SERVER_INDEX,
		SERVER_STATISTIC,
		SERVER_DATA_COLLECTION,
		SERVER_TRACKER_COMMIT,
		SERVER_TRACKER_LIST,
	};

	struct SERVER_TYPE_INFO
	{
		boost::uint8_t         ConnectType;
		boost::uint8_t         ServerType;
		boost::uint16_t        Level;

		template <typename Archive>
		void serialize(Archive & ar)
		{
			ar & ConnectType & ServerType & Level; 
		}

		/*SERVER_TYPE_INFO(
			CONNECT_TYPE conn_type, 
			SERVER_TYPE server_type, 
			boost::uint16_t lev) {
				ConnectType = conn_type;
				ServerType = server_type;
				Level = lev;
		}*/
		SERVER_TYPE_INFO()
		{
			memset(this, 0, sizeof(SERVER_TYPE_INFO));
		}
	};
	struct SERVER_DATA
	{
		boost::uint32_t        IP;
		boost::uint16_t        Port;
		boost::uint8_t         ModID;
		boost::uint8_t         ConnectType;

		template <typename Archive>
		void serialize(Archive & ar)
		{
			ar&IP&Port&ModID&ConnectType; 
		}

		SERVER_DATA()
		{
			memset(this, 0, sizeof(SERVER_DATA));
		}

		SERVER_DATA(
			boost::uint32_t ip, 
			boost::uint16_t port, 
			boost::uint8_t mod_id, 
			CONNECT_TYPE conn_type)
		{
			IP = ip;
			Port = port;
			ModID = mod_id;
			ConnectType = conn_type;
		}

		friend bool operator < (
			const SERVER_DATA& a, 
			const SERVER_DATA& b)
		{
			return memcmp(&a, &b, sizeof(SERVER_DATA)) < 0;
		}

	};
	struct SERVER_LIST
	{
		SERVER_TYPE_INFO    ServerTypeInfo;
		std::vector<SERVER_DATA> ServerArray;
		std::vector<boost::uint8_t>       PaddingInfo;


		template <typename Archive>
		void serialize(Archive & ar)
		{
			ar & ServerTypeInfo 
				& util::serialization::make_sized<boost::uint16_t>(ServerArray)
				&util::serialization::make_sized<boost::uint16_t>(PaddingInfo); 
		}


		SERVER_LIST() {
		}

	};


	

	struct CANDIDATE_PEER_INFO
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
		boost::uint8_t  Reversed[1];

		bool operator ==(const CANDIDATE_PEER_INFO& n) const
		{
			return 0 == memcmp(this, &n, sizeof(CANDIDATE_PEER_INFO));
		}
		/*
		bool operator < (const CANDIDATE_PEER_INFO& n) const
		{
		return true;
		}*/

		CANDIDATE_PEER_INFO()
		{
			IP = 0;
			UdpPort = 0;
			TcpPort = 0;
			DetectIP = 0;
			DetectUdpPort = 0;
			StunIP = 0;
			StunUdpPort = 0;
			PeerNatType = -1;
			UploadPriority = 0;
			IdleTimeInMins = 0;
			Reversed[0] = 0;
		}
	};

	struct PEER_EXCHANGE_PACKET
	{
		boost::uint8_t BasicInfo;
		boost::uint8_t PeerCount;
		CANDIDATE_PEER_INFO CandidatePeerInfo[];
		bool operator ==(const PEER_EXCHANGE_PACKET& n) const
		{
			return 0 == memcmp(this, &n, sizeof(PEER_EXCHANGE_PACKET));
		}
	};

	struct CONNECT_PACKET
	{
		boost::uint8_t BasicInfo;
		boost::uint32_t SendOffTime;
		boost::uint32_t PeerVersion;
		CANDIDATE_PEER_INFO CandidatePeerInfo;
		boost::uint8_t  PeerType;
		PEER_DOWNLOAD_INFO PeerDownloadInfo;

		/*bool operator ==(const CONNECT_PACKET& n) const
		{
			return 0 == memcmp(this, &n, sizeof(CONNECT_PACKET));
		}*/
	};

	
	struct CONNECT_PACKET_EX : public CONNECT_PACKET
	{
		boost::uint16_t IpPoolSize;
	};
	
}

#endif
