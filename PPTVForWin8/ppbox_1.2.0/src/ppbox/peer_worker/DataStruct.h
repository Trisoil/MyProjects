// DataStruct.h

#ifndef _PPBOX_AGENT_DATA_STRUCT_H_
#define _PPBOX_AGENT_DATA_STRUCT_H_

#pragma pack(push, 1)

typedef struct _UUID {
    boost::uint32_t  Data1;
    boost::uint16_t Data2;
    boost::uint16_t Data3;
    boost::uint8_t  Data4[8];
} UUID;

#include <string.h>

typedef UUID RID;

const boost::uint16_t BITMAP_SIZE = 50;

const boost::uint16_t UINT8_MAX_VALUE = 256;

const boost::uint16_t LIVEDOWNLOADER_MAX_COUNT = 64;

const boost::uint16_t MAX_P2P_DOWNLOADER_COUNT = 100;


const boost::uint16_t MAX_IP_COUNT = 10;

//////////////////////////////////////////////////////////////////////////
// 公共结构

struct SPEED_INFO
{
    boost::uint32_t StartTime;               // 开始时刻
    boost::uint32_t TotalDownloadBytes;      // 总共下载的字节数
    boost::uint32_t TotalUploadBytes;        // 总共上传时字节数
    boost::uint32_t NowDownloadSpeed;        // 当前下载速度 <5s>
    boost::uint32_t NowUploadSpeed;          // 当前上传速度 <5s>
    boost::uint32_t MinuteDownloadSpeed;     // 最近一分钟平均下载速度 <60s>
    boost::uint32_t MinuteUploadSpeed;       // 最近一分钟平均上传速度 <60s>
    boost::uint32_t AvgDownloadSpeed;        // 历史平均下载速度
    boost::uint32_t AvgUploadSpeed;          // 历史平均上传速度

    SPEED_INFO()
    {
        memset(this, 0, sizeof(SPEED_INFO));
    }
};

struct SPEED_INFO_EX : SPEED_INFO
{
    boost::uint32_t RecentDownloadSpeed;     // 当前下载速度 <20s>
    boost::uint32_t RecentUploadSpeed;       // 当前上传速度 <20s>
    boost::uint32_t SecondDownloadSpeed;     // 当前1s的下载速度
    boost::uint32_t SecondUploadSpeed;       // 当前1s的上传速度

    SPEED_INFO_EX()
    {
        memset(this, 0, sizeof(SPEED_INFO_EX));
    }
};

/*
struct CANDIDATE_PEER_INFO
{
    boost::uint32_t IP;                      // 自己的主IP
    boost::uint16_t UdpPort;                 // 自己的本地UDP Port
    boost::uint16_t TcpPort;                 // 自己的本地TCP Port
    boost::uint32_t DetectIP;                // 自己从服务器获得的探测 IP
    boost::uint16_t DetectUdpPort;           // 自己从服务器获得的探测 UDP Port
    boost::uint32_t StunIP;                  // 自己选择的 Stun Peer 的IP
    boost::uint16_t StunUdpPort;             // 自己选择的 Stun Peer 的 Port
    boost::uint8_t  Reversed[4];
};
*/
//typedef CANDIDATE_PEER_INFO CANDIDATE_PEER_INFO;

/*
struct TRACKER_INFO
{
    boost::uint16_t Length;
    boost::uint8_t ModNo;
    boost::uint32_t IP;
    boost::uint16_t Port;
    boost::uint8_t Type;
}
*/

struct TRACKER_INFO
{
    boost::uint16_t Length;
    boost::uint8_t ModNo;
    boost::uint32_t IP;
    boost::uint16_t Port;
    boost::uint8_t Type; // 1 UDP

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
        if (this != &tracker)
        {
            Length = tracker.Length;
            ModNo = tracker.ModNo;
            IP = tracker.IP;
            Port = tracker.Port;
            Type = tracker.Type;
        }
        return *this;
    }
};

struct STATISTIC_TRACKER_INFO
{
    // 静态
    TRACKER_INFO TrackerInfo; // 该Tracker的基本信息

    // 动态信息
    boost::uint16_t CommitRequestCount;       // 该Tracker Commit 的请求次数
    boost::uint16_t CommitResponseCount;      // 该Tracker Commit 的响应次数
    boost::uint16_t KeepAliveRequestCount;    // 该Tracker KeepAlive 的请求次数
    boost::uint16_t KeepAliveResponseCount;   // 该Tracker KeepAlive 的响应次数
    boost::uint16_t ListRequestCount;         // 该Tracker List 的请求次数
    boost::uint16_t ListResponseCount;        // 该Tracker List 的响应次数
    boost::uint8_t  LastListReturnPeerCount;  // 上一次成功的List的返回的Peer数
    boost::uint8_t  IsSubmitTracker;          // 在该Group中是否为当前选定Tracker
    boost::uint8_t  ErrorCode;                // 上次Tracker返回的错误码
    boost::uint16_t KeepAliveInterval;        // 上一次从服务器返回的Submit间隔

    STATISTIC_TRACKER_INFO()
    {
        Clear();
    }
    STATISTIC_TRACKER_INFO(const TRACKER_INFO & tracker_info)
    {
        Clear();
        TrackerInfo = tracker_info;
    }
    void Clear()
    {
        memset(this, 0, sizeof(STATISTIC_TRACKER_INFO));
    }
};

struct STATISTIC_INDEX_INFO
{
    boost::uint32_t IP;
    boost::uint16_t Port;
    boost::uint8_t  Type;
    boost::uint16_t QueryRIDByUrlRequestCount;
    boost::uint16_t QueryRIDByUrlResponseCount;
    boost::uint16_t QueryHttpServersByRIDRequestCount;
    boost::uint16_t QueryHttpServersByRIDResponseCount;
    boost::uint16_t QueryTrackerListRequestCount;
    boost::uint16_t QureyTrackerListResponseCount;
    boost::uint16_t AddUrlRIDRequestCount;
    boost::uint16_t AddUrlRIDResponseCount;
};

//struct PEER_DOWNLOAD_INFO
//{
//    boost::uint8_t  IsDownloading;
//    boost::uint32_t OnlineTime;
//    boost::uint16_t AvgDownload;
//    boost::uint16_t NowDownload;
//    boost::uint16_t AvgUpload;
//    boost::uint16_t NowUpload;
//};

struct PEER_INFO
{
    boost::uint8_t download_connected_count_;
    boost::uint8_t upload_connected_count_;
    boost::uint32_t mine_upload_speed_;
    boost::uint32_t max_upload_speed_;
    boost::uint32_t rest_playable_time_;
    boost::uint8_t lost_rate_;
    boost::uint8_t redundancy_rate_;
};

struct PIECE_INFO_EX
{
    boost::uint16_t BlockIndex;
    boost::uint16_t PieceIndexInBlock;
    boost::uint16_t SubPieceIndexInPiece;

    PIECE_INFO_EX()
    {
        memset(this, 0, sizeof(PIECE_INFO_EX));
    }

    PIECE_INFO_EX(
        boost::uint16_t block_index, 
        boost::uint16_t piece_index_in_block, 
        boost::uint16_t sub_piece_index_in_piece)
        : BlockIndex(block_index)
        , PieceIndexInBlock(piece_index_in_block)
        , SubPieceIndexInPiece(sub_piece_index_in_piece)
    {
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

//////////////////////////////////////////////////////////////////////////
// AppStop时统计上传

//////////////////////////////////////////////////////////////////////////
// StatisticModule 结构
//   共享内存名: PPVIDEO_<PID>
struct STASTISTIC_INFO
{
    // 速度相关信息
    SPEED_INFO SpeedInfo;                    // 网络 字节数 和 速度

    // IP 相关信息
    CANDIDATE_PEER_INFO LocalPeerInfo;      // 自己 的基本IP端口信息
    boost::uint8_t  LocalIpCount;           // 本地通过API 获得的本地IP数 
    boost::uint32_t LocalIPs[MAX_IP_COUNT]; // 本地通过API 获得的IP; (连续存放)

    // 自己其他相关信息
    boost::uint32_t LocalPeerVersion;       // 自己内核版本号
    boost::uint8_t  LocalPeerType;          // 自己的Peer类型

    // TrackerServer相关信息
    boost::uint8_t  TrackerCount;           // Tracker的个数
    boost::uint8_t  GroupCount;             // Group 的, 也是MOD的除数
    STATISTIC_TRACKER_INFO TrackerInfos[UINT8_MAX_VALUE];    // Tracker, (连续存放)

    // IndexServer 相关信息
    STATISTIC_INDEX_INFO StatisticIndexInfo;    // IndexServer的相关信息

    // P2PDownloader 相关信息
    boost::uint8_t  P2PDownloaderCount;         // 有多少个正在P2P下载的资源
    RID    P2PDownloaderRIDs[UINT8_MAX_VALUE];  // 正在P2P下载的资源RID; 如果为 GUID_NULL 表示空; (不连续)

    // DownloadDriver 相关信息
    boost::uint8_t  DownloadDriverCount;                // 正在下载的视频
    boost::uint32_t DownloadDriverIDs[UINT8_MAX_VALUE]; // 正在下载的视频 驱动器ID; 如果为 0 表示不存在; (不连续)

    //停止时数据上传相关
    boost::uint32_t TotalP2PDownloadBytes;
    boost::uint32_t TotalOtherServerDownloadBytes;

    //下载中 数据下载 实时相关信息
    boost::uint32_t TotalHttpNotOriginalDataBytes;      // 实时 下载的纯数据 字节数
    boost::uint32_t TotalP2PDataBytes;                  // 实时 P2P下载的纯数据 字节数
    boost::uint32_t TotalHttpOriginalDataBytes;         // 实时 原生下载的纯数据 字节数
    boost::uint32_t CompleteCount;                      // 已经完成的DD数

    boost::uint32_t TotalUploadCacheRequestCount;       // 总共的上传Cache请求数
    boost::uint32_t TotalUploadCacheHitCount;           // 总共的上传Cache命中数

    boost::uint16_t HttpProxyPort;                      // HTTP实际代理端口
    boost::uint32_t MaxHttpDownloadSpeed;               // HTTP最大下载速度
    boost::uint16_t IncomingPeersCount;                 // 总共连入的Peer个数
    boost::uint16_t DownloadDurationInSec;              // 下载总共持续时长(秒)
    boost::uint32_t BandWidth;                          // 预测的带宽

    boost::uint32_t  GlobalWindowSize;

    boost::uint16_t  GlobalRequestSendCount;             // 每秒发出的请求数

    boost::uint16_t  MemoryPoolLeftSize;                     // 内存池剩余大小

    // 统计BHO和FlOAT的点击次数
    // protocol::ACTION_COUNT_INFO ActionCountInfo;

    // LiveDownloadDriver 相关信息
    boost::uint8_t  LiveDownloadDriverCount;               // 正在下载的直播视频
    boost::uint32_t LiveDownloadDriverIDs[LIVEDOWNLOADER_MAX_COUNT];// 正在下载的直播视频 驱动器ID; 如果为 0 表示不存在; (不连续)

    boost::uint8_t   Resersed[945 - 4 * LIVEDOWNLOADER_MAX_COUNT];                      // 保留字段
};

//////////////////////////////////////////////////////////////////////////
// P2PDownloaderStatistic 结构
//   共享内存名: P2PDOWNLOADER_<PID>_<RID>
//   NOTE: RID格式{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}
struct PEER_DOWNLOAD_INFO
{
    boost::uint8_t IsDownloading;
    boost::uint32_t OnlineTime;
    boost::uint16_t AvgDownload;
    boost::uint16_t NowDownload;
    boost::uint16_t AvgUpload;
    boost::uint16_t NowUpload;

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

struct P2P_CONNECTION_INFO
{
    UUID PeerGuid;                          // 该Peer的PeerGuid
    SPEED_INFO SpeedInfo;                   // 该Peer 的速度信息
    boost::uint32_t PeerVersion;            // 对方Peer的内核版本号
    boost::uint8_t  PeerType;               // 对方Peer的内核类型
    CANDIDATE_PEER_INFO PeerInfo;           // 该Peer的 IP信息
    PEER_DOWNLOAD_INFO PeerDownloadInfo;    // 对方的相关 下载 信息
    boost::uint8_t  BitMap[BITMAP_SIZE];    // 对方的BitMap
    boost::uint32_t RTT_Count;              // Udp包的个数
    boost::uint16_t RTT_Now;                // 当前的rtt
    boost::uint16_t RTT_Average;            // 总共的平均rtt; 可画出所有udp包的收到时间分布图
    boost::uint16_t RTT_Max;                // 收到的udp包中rtt最长的
    boost::uint32_t RTT_Total;              // 总共的rtt
    boost::uint16_t ElapseTime;             // 当前超时时间
    boost::uint8_t  WindowSize;             // 窗口大小
    boost::uint8_t  AssignedSubPieceCount;  // 当前预分配SubPiece数
    boost::uint32_t AverageDeltaTime;       // Average Delta Time
    boost::uint32_t SortedValue;            // Sorted Value

    //////////////////////////////////////////////////////////////////////////
    boost::uint8_t  IsRidInfoValid;         // 0-Invalid; 1-Valid
    boost::uint16_t Sent_Count;
    boost::uint16_t Requesting_Count;
    boost::uint16_t Received_Count; 
    boost::uint8_t  Reserved[200-7];        // 保留字段

    P2P_CONNECTION_INFO()
    {
        Clear();
    }

    void Clear()
    {
        memset(this, 0, sizeof(P2P_CONNECTION_INFO));
    }
};

struct P2PDOWNLOADER_STATISTIC_INFO
{
    RID ResourceID;                 //对应的RID
    SPEED_INFO SpeedInfo;

    // 资源相关信息
    boost::uint32_t FileLength;
    boost::uint16_t BlockNum;
    boost::uint16_t BlockSize;

    // IPPool信息
    boost::uint16_t IpPoolPeerCount;        // 备选IP    
    boost::uint8_t  ExchangingPeerCount;    // 正在交换信息的IP    
    boost::uint8_t  ConnectingPeerCount;    // 正在连接的IP     

    // 算法相关信息
    boost::uint16_t TotalWindowSize;                // 总窗口大小
    boost::uint16_t TotalAssignedSubPieceCount;     // 当前与分配的总SubPiece数
    boost::uint16_t TotalUnusedSubPieceCount_;      // 冗余的Subpieces数
    boost::uint16_t TotalRecievedSubPieceCount_;    // 收到的Subpiece数
    boost::uint16_t TotalRequestSubPieceCount_;     // 发出的Subpiece请求数
    boost::uint16_t SubPieceRetryRate;              // 冗余率: 冗余 / 收到
    boost::uint16_t UDPLostRate;                    // 丢包率: (发出 - 收到) / 发出

    boost::uint32_t TotalP2PDataBytes;              // 当前P2PDownloader下载的有效字节数
    boost::uint16_t FullBlockPeerCount;             // 在已经连接的Peer中，资源全满的Peer个数

    boost::uint32_t TotalUnusedSubPieceCount;       // 冗余的Subpieces数
    boost::uint32_t TotalRecievedSubPieceCount;     // 收到的Subpiece数
    boost::uint32_t TotalRequestSubPieceCount;      // 发出的Subpiece请求数
    boost::uint32_t NonConsistentSize;              // 当前下载的最后一个piece和第一个piece之间的距离

    boost::uint8_t Reserved[978];                   // 保留

    boost::uint16_t PeerCount;                      // Peer的
    P2P_CONNECTION_INFO P2PConnections[];           // 变长; (连续存放)
};

//////////////////////////////////////////////////////////////////////////
// DownloadDriverStatistic 结构
//   共享内存名： DOWNLOADDRIVER_<PID>_<DownloadDriverID>

struct HTTP_DOWNLOADER_INFO
{
    boost::uint8_t  Url[256];
    boost::uint8_t  ReferUrl[256];
    boost::uint8_t  RedirectUrl[256];       // 发生301或者302 重定向的Url

    SPEED_INFO SpeedInfo;

    PIECE_INFO_EX DownloadingPieceEx;       // 现在正在请求的 PieceEx
    PIECE_INFO_EX StartPieceEx;             // 从哪个Subpiece开始下载没断开连接过;
    boost::uint32_t LastConnectedTime;      // 最近建立HTTP连接的时刻
    boost::uint32_t LastRequestPieceTime;   // 最近请求PieceEx的时刻

    boost::uint16_t LastHttpStatusCode;     // 上次HTTP请求的返回值 (例如200.206)
    boost::uint16_t RetryCount;             // Http重试次数
    boost::uint8_t  IsSupportRange;         // 是否支持Range

    boost::uint8_t  IsDeath;                // 是否是死的
    boost::uint8_t  Resersed[400];          // 保留字段

    HTTP_DOWNLOADER_INFO()
    {
        Clear();
    }

    void Clear()
    {
        memset(this, 0, sizeof(HTTP_DOWNLOADER_INFO));
    }
};

struct DOWNLOADDRIVER_STATISTIC_INFO
{
    boost::uint32_t DownloadDriverID;
    SPEED_INFO SpeedInfo;

    boost::uint8_t OriginalUrl[256];
    boost::uint8_t OriginalReferUrl[256];

    RID ResourceID;                         // 资源RID (可以为全0)
    boost::uint32_t FileLength;             // 文件的长度
    boost::uint32_t BloskSize;              // Block的大小
    boost::uint16_t BlockCount;             // Block的个数

    boost::uint32_t TotalHttpDataBytes;     // 所有HttpDownloader下载的有效字节数
    boost::uint32_t TotalLocalDataBytes;    // 所有本地已经下载过的有效字节数
    boost::uint8_t FileName[256];           // 文件名,(TCHAR*)

    boost::uint8_t IsHidden;                // 是否隐藏(不在界面上显示进度)

    boost::uint8_t SourceType;              // 标识是否是客户端
    boost::uint8_t WebUrl[256];             // va 传进来的 WebUrl

    boost::uint8_t StateMachineType;
    boost::uint8_t StateMachineState[14];

    boost::uint32_t PlayingPosition;
    boost::uint32_t DataRate;
    boost::uint8_t http_state;
    boost::uint8_t p2p_state;
    boost::uint8_t timer_using_state;
    boost::uint8_t timer_state;

    boost::uint8_t Resersed[451];           // 保留字段

    boost::uint8_t  HttpDownloaderCount;
    HTTP_DOWNLOADER_INFO HttpDownloaders[]; // 变长; (连续存放)
};

struct LIVE_DOWNLOADDRIVER_STATISTIC_INFO
{
    boost::uint32_t LiveDownloadDriverID;
    SPEED_INFO LiveHttpSpeedInfo;
    SPEED_INFO LiveP2PSpeedInfo;
    SPEED_INFO LiveP2PSubPieceSpeedInfo;
    boost::uint8_t http_state;
    boost::uint8_t p2p_state;
    boost::uint8_t  OriginalUrl[256];              // CDN IP
    boost::uint16_t LastHttpStatusCode;            // 上次HTTP请求的返回值 (例如200.206)
    boost::uint32_t TotalP2PDataBytes;             // 当前P2PDownloader下载的有效字节数
    boost::uint32_t TotalRecievedSubPieceCount;    // 收到的Subpiece数
    boost::uint32_t TotalRequestSubPieceCount;     // 发出的Subpiece请求数
    boost::uint32_t TotalAllRequestSubPieceCount;     // 发出的Subpiece请求数
    boost::uint32_t TotalUnusedSubPieceCount;      // 冗余的Subpieces数
    boost::uint16_t IpPoolPeerCount;               // 备选IP
    boost::uint32_t DataRate;                      // 码流率
    boost::uint16_t CacheSize;                      // 已缓存大小
    boost::uint32_t CacheFirstBlockId;             // 已缓存的第一片Block ID
    boost::uint32_t CacheLastBlockId;              // 已缓存的最后一片Block ID
    boost::uint32_t PlayingPosition;               // 播放点
    boost::uint32_t LeftCapacity;                  // 内存池剩余
    boost::int32_t  RestPlayTime;                  // 剩余时间
    RID       ResourceID;                          // 资源RID
    boost::uint8_t  IsPlayingPositionBlockFull; // 正在推给播放器的这一片Block是否满
    boost::uint32_t LivePointBlockId;              // 直播点的Block ID
    boost::uint32_t DataRateLevel;                  // 码流等级
    boost::int32_t P2PFailedTimes;                  // P2P下载失败的次数
    boost::uint8_t HttpSpeedStatus;                 // Http速度状态(0, 1, 2分别代表fast, checking, slow)
    boost::uint8_t HttpStatus;                      // Http状态(unknown, good, bad)
    boost::uint8_t P2PStatus;                       // P2P状态(unknown, good, bad)
    boost::uint32_t JumpTimes;                      // 跳跃了多少次
    boost::uint32_t NumOfChecksumFailedPieces;      // 校验失败的piece个数
    boost::uint8_t Is3200P2PSlow;                   // 3200状态下P2P是否慢
    RID ChannelID;                                  // 频道ID
    boost::uint32_t TotalUdpServerDataBytes;        // 从UdpServer下载的字节数
    boost::uint8_t PmsStatus;                       // 0代表正常，1代表不正常
    boost::uint32_t UniqueID;                       // 播放器的ID
    SPEED_INFO UdpServerSpeedInfo;                  // UdpServer速度
    boost::uint8_t IsPaused;                        // 是否暂停，0代表播放，1代表暂停
    boost::uint8_t IsReplay;                        // 是否回拖，0代表不回拖，1代表回拖
    boost::uint32_t MissingSubPieceCountOfFirstBlock;  // 第一个不满的Block中空的SubPiece个数
    boost::uint32_t ExistSubPieceCountOfFirstBlock;  // 第一个不满的Block中存在的SubPiece个数
    boost::uint32_t P2PPeerSpeedInSecond;        // P2P Peer一秒的速度
    boost::uint32_t P2PUdpServerSpeedInSecond;  // UdpServer一秒的速度

    boost::uint8_t Reserved[896];

    boost::uint16_t PeerCount;                     // Peer的
    P2P_CONNECTION_INFO P2PConnections[MAX_P2P_DOWNLOADER_COUNT];  // 变长; (连续存放)
};

struct PEER_UPLOAD_INFO
{
    PEER_UPLOAD_INFO()
    {
        Clear();
    }

    boost::uint32_t ip;
    boost::uint16_t port;
    boost::uint32_t upload_speed;
    PEER_INFO peer_info;
    boost::uint8_t resersed[126 - sizeof(PEER_INFO)];

    void Clear()
    {
        memset(this, 0, sizeof(PEER_UPLOAD_INFO));
    }
};

struct UPLOAD_INFO
{
    UPLOAD_INFO()
    {
        Clear();
    }

    boost::uint8_t peer_upload_count;
    boost::uint8_t reserve;
    boost::uint32_t upload_speed;
    boost::uint32_t actual_speed_limit;
    boost::uint32_t upload_subpiece_count;
    boost::uint8_t resersed[116];
    PEER_UPLOAD_INFO peer_upload_info[256];

    void Clear()
    {
        memset(this, 0, sizeof(UPLOAD_INFO));
    }
};

#pragma pack(pop)

#endif // _PPBOX_AGENT_DATA_STRUCT_H_
