#pragma once

#include "base/guid.h"
#include "protocal/structs.h"
#include "VarSizeStruct.h"

#pragma pack(push, 1)

namespace statistic
{
    //////////////////////////////////////////////////////////////////////////
    // Constants

    const u_int BITMAP_SIZE = 50;

    const u_int UINT8_MAX_VALUE = 256;

    const u_int MAX_IP_COUNT = 10;

    //////////////////////////////////////////////////////////////////////////
    // 公共结构

    struct SPEED_INFO
    {
        UINT32 StartTime;               // 开始时刻
        UINT32 TotalDownloadBytes;      // 总共下载的字节数
        UINT32 TotalUploadBytes;        // 总共上传时字节数
        UINT32 NowDownloadSpeed;        // 当前下载速度 <5s>
        UINT32 NowUploadSpeed;          // 当前上传速度 <5s>
        UINT32 MinuteDownloadSpeed;     // 最近一分钟平均下载速度 <60s>
        UINT32 MinuteUploadSpeed;       // 最近一分钟平均上传速度 <60s>
        UINT32 AvgDownloadSpeed;        // 历史平均下载速度
        UINT32 AvgUploadSpeed;          // 历史平均上传速度

        SPEED_INFO()
        {
            memset(this, 0, sizeof(SPEED_INFO));
        }
    };

    /*
    struct CANDIDATE_PEER_INFO
    {
        UINT32 IP;                      // 自己的主IP
        UINT16 UdpPort;                 // 自己的本地UDP Port
        UINT16 TcpPort;                 // 自己的本地TCP Port
        UINT32 DetectIP;                // 自己从服务器获得的探测 IP
        UINT16 DetectUdpPort;           // 自己从服务器获得的探测 UDP Port
        UINT32 StunIP;                  // 自己选择的 Stun Peer 的IP
        UINT16 StunUdpPort;             // 自己选择的 Stun Peer 的 Port
        UINT8  Reversed[4];
    };
    */
    //typedef protocal::CANDIDATE_PEER_INFO CANDIDATE_PEER_INFO;

    /*
    struct TRACKER_INFO
    {
        UINT16 Length;
        UINT8 ModNo;
        UINT32 IP;
        UINT16 Port;
        UINT8 Type;
    }
    */
    struct STATISTIC_TRACKER_INFO
    {
        // 静态
        protocal::TRACKER_INFO TrackerInfo; // 该Tracker的基本信息

        // 动态信息
        UINT16 CommitRequestCount;       // 该Tracker Commit 的请求次数
        UINT16 CommitResponseCount;      // 该Tracker Commit 的响应次数
        UINT16 KeepAliveRequestCount;    // 该Tracker KeepAlive 的请求次数
        UINT16 KeepAliveResponseCount;   // 该Tracker KeepAlive 的响应次数
        UINT16 ListRequestCount;         // 该Tracker List 的请求次数
        UINT16 ListResponseCount;        // 该Tracker List 的响应次数
        UINT8  LastListReturnPeerCount;  // 上一次成功的List的返回的Peer数
        UINT8  IsSubmitTracker;          // 在该Group中是否为当前选定Tracker
        UINT8  ErrorCode;                // 上次Tracker返回的错误码
        UINT16 KeepAliveInterval;        // 上一次从服务器返回的Submit间隔

        STATISTIC_TRACKER_INFO()
        {
            Clear();
        }
        STATISTIC_TRACKER_INFO(const TRACKER_INFO& tracker_info)
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
        UINT32 IP;
        UINT16 Port;
        UINT8  Type;
        UINT16 QueryRIDByUrlRequestCount;
        UINT16 QueryRIDByUrlResponseCount;
        UINT16 QueryHttpServersByRIDRequestCount;
        UINT16 QueryHttpServersByRIDResponseCount;
        UINT16 QueryTrackerListRequestCount;
        UINT16 QureyTrackerListResponseCount;
        UINT16 AddUrlRIDRequestCount;
        UINT16 AddUrlRIDResponseCount;
    };

    //struct PEER_DOWNLOAD_INFO
    //{
    //    UINT8  IsDownloading;
    //    UINT32 OnlineTime;
    //    UINT16 AvgDownload;
    //    UINT16 NowDownload;
    //    UINT16 AvgUpload;
    //    UINT16 NowUpload;
    //};

    struct PIECE_INFO_EX
    {
        UINT16 BlockIndex;
        UINT16 PieceIndexInBlock;
        UINT16 SubPieceIndexInPiece;

        PIECE_INFO_EX()
        {
            memset(this, 0, sizeof(PIECE_INFO_EX));
        }
        PIECE_INFO_EX(UINT16 block_index, UINT16 piece_index_in_block, UINT16 sub_piece_index_in_piece)
            : BlockIndex(block_index)
            , PieceIndexInBlock(piece_index_in_block)
            , SubPieceIndexInPiece(sub_piece_index_in_piece)
        {
        }
    };

    inline ostream& operator << (ostream& os, const PIECE_INFO_EX& info)
    {
        return os << "BlockIndex: " << info.BlockIndex << ", PieceIndexInBlock: " << info.PieceIndexInBlock
            << ", SubPieceIndexInBlock: " << info.SubPieceIndexInPiece;
    }

    //////////////////////////////////////////////////////////////////////////
    // AppStop时统计上传

    //////////////////////////////////////////////////////////////////////////
    // StatisticModule 结构
    //   共享内存名: PPVIDEO_<PID>
    struct STASTISTIC_INFO
    {
        // 速度相关信息
        SPEED_INFO SpeedInfo;                  // 网络 字节数 和 速度

        // IP 相关信息
        CANDIDATE_PEER_INFO LocalPeerInfo;     // 自己 的基本IP端口信息
        UINT8  LocalIpCount;                   // 本地通过API 获得的本地IP数 
        UINT32 LocalIPs[MAX_IP_COUNT];         // 本地通过API 获得的IP; (连续存放)

        // 自己其他相关信息
        UINT32 LocalPeerVersion;               // 自己内核版本号
        UINT8  LocalPeerType;                  // 自己的Peer类型

        // TrackerServer相关信息
        UINT8  TrackerCount;                       // Tracker的个数
        UINT8  GroupCount;                         // Group 的, 也是MOD的除数
        STATISTIC_TRACKER_INFO TrackerInfos[UINT8_MAX_VALUE]; // Tracker, (连续存放)

        // IndexServer 相关信息
        STATISTIC_INDEX_INFO StatisticIndexInfo;   // IndexServer的相关信息

        // P2PDownloader 相关信息
        UINT8  P2PDownloaderCount;                 // 有多少个正在P2P下载的资源
        RID    P2PDownloaderRIDs[UINT8_MAX_VALUE]; // 正在P2P下载的资源RID; 如果为 GUID_NULL 表示空; (不连续)

        // DownloadDriver 相关信息
        UINT8  DownloadDriverCount;               // 正在下载的视频    
        UINT32 DownloadDriverIDs[UINT8_MAX_VALUE];// 正在下载的视频 驱动器ID; 如果为 0 表示不存在; (不连续)

        //停止时数据上传相关
        UINT32 TotalP2PDownloadBytes;
        UINT32 TotalOtherServerDownloadBytes;

        //下载中 数据下载 实时相关信息
        UINT32  TotalHttpNotOriginalDataBytes;        // 实时 下载的纯数据 字节数
        UINT32  TotalP2PDataBytes;                    // 实时 P2P下载的纯数据 字节数
        UINT32  TotalHttpOriginalDataBytes;            // 实时 原生下载的纯数据 字节数
        UINT32  CompleteCount;                        // 已经完成的DD数

        UINT32  TotalUploadCacheRequestCount;       // 总共的上传Cache请求数
        UINT32  TotalUploadCacheHitCount;           // 总共的上传Cache命中数

        UINT16  HttpProxyPort;                      // HTTP实际代理端口
        UINT32  MaxHttpDownloadSpeed;               // HTTP最大下载速度
        UINT16  IncomingPeersCount;                 // 总共连入的Peer个数
        UINT16  DownloadDurationInSec;              // 下载总共持续时长(秒)
        UINT8   Resersed[958];                      // 保留字段

        STASTISTIC_INFO()
        {
            Clear();
        }

        void Clear()
        {
            memset(this, 0, sizeof(STASTISTIC_INFO)); // Important!
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // P2PDownloaderStatistic 结构
    //   共享内存名: P2PDOWNLOADER_<PID>_<RID>
    //   NOTE: RID格式{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}

    struct P2P_CONNECTION_INFO
    {
        GUID   PeerGuid;                       // 该Peer的PeerGuid
        SPEED_INFO SpeedInfo;                  // 该Peer 的速度信息
        UINT32 PeerVersion;                    // 对方Peer的内核版本号
        UINT8  PeerType;                       // 对方Peer的内核类型
        CANDIDATE_PEER_INFO PeerInfo;          // 该Peer的 IP信息
        PEER_DOWNLOAD_INFO PeerDownloadInfo;   // 对方的相关 下载 信息
        UINT8  BitMap[BITMAP_SIZE];            // 对方的BitMap
        UINT32 RTT_Count;                      // Udp包的个数
        UINT16 RTT_Now;                        // 当前的rtt
        UINT16 RTT_Average;                    // 总共的平均rtt; 可画出所有udp包的收到时间分布图
        UINT16 RTT_Max;                        // 收到的udp包中rtt最长的
        UINT32 RTT_Total;                      // 总共的rtt
        UINT16 ElapseTime;                     // 当前超时时间
        UINT8  WindowSize;                     // 窗口大小
        UINT8  AssignedSubPieceCount;          // 当前预分配SubPiece数
        UINT32 AverageDeltaTime;               // Average Delta Time
        UINT32 SortedValue;                    // Sorted Value

        UINT8  Reserved[200];                  // 保留字段

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
        RID    ResourceID;                 //对应的RID
        SPEED_INFO SpeedInfo;

        // 资源相关信息
        UINT32 FileLength;
        UINT16 BlockNum;
        UINT16 BlockSize;

        // IPPool信息
        UINT16 IpPoolPeerCount;        // 备选IP    
        UINT8  ExchangingPeerCount;    // 正在交换信息的IP    
        UINT8  ConnectingPeerCount;    // 正在连接的IP     

        // 算法相关信息
        UINT16 TotalWindowSize;               // 总窗口大小
        UINT16 TotalAssignedSubPieceCount;    // 当前与分配的总SubPiece数
        UINT16 TotalUnusedSubPieceCount;      // 冗余的Subpieces数
        UINT16 TotalRecievedSubPieceCount;    // 收到的Subpiece数
        UINT16 TotalRequestSubPieceCount;     // 发出的Subpiece请求数
        UINT16 SubPieceRetryRate;             // 冗余率: 冗余 / 收到
        UINT16 UDPLostRate;                   // 丢包率: (发出 - 收到) / 发出

        UINT32 TotalP2PDataBytes;             // 当前P2PDownloader下载的有效字节数
        UINT16 FullBlockPeerCount;            // 在已经连接的Peer中，资源全满的Peer个数
        UINT8 Reserved[994];                  // 保留

        UINT16 PeerCount;                     // Peer的
        P2P_CONNECTION_INFO P2PConnections[]; // 变长; (连续存放)
    };

    typedef VarSizeStruct<P2PDOWNLOADER_STATISTIC_INFO, P2P_CONNECTION_INFO> P2PDOWNLOADER_STATISTIC_INFO_EX;


    //////////////////////////////////////////////////////////////////////////
    // DownloadDriverStatistic 结构
    //   共享内存名： DOWNLOADDRIVER_<PID>_<DownloadDriverID>

    struct HTTP_DOWNLOADER_INFO
    {
        UINT8  Url[256];
        UINT8  ReferUrl[256];
        UINT8  RedirectUrl[256]; // 发生301或者302 重定向的Url

        SPEED_INFO SpeedInfo;

        PIECE_INFO_EX DownloadingPieceEx;   // 现在正在请求的 PieceEx
        PIECE_INFO_EX StartPieceEx;         // 从哪个Subpiece开始下载没断开连接过;
        UINT32 LastConnectedTime;           // 最近建立HTTP连接的时刻
        UINT32 LastRequestPieceTime;        // 最近请求PieceEx的时刻

        UINT16 LastHttpStatusCode;          // 上次HTTP请求的返回值 (例如200.206)
        UINT16 RetryCount;                  // Http重试次数
        UINT8  IsSupportRange;              // 是否支持Range

        UINT8  IsDeath;                     // 是否是死的
        UINT8  Resersed[400];               // 保留字段

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
        UINT32 DownloadDriverID;
        SPEED_INFO SpeedInfo;

        UINT8  OriginalUrl[256];
        UINT8  OriginalReferUrl[256];

        RID       ResourceID;                 // 资源RID (可以为全0)
        UINT32    FileLength;                    // 文件的长度
        UINT32    BloskSize;                  // Block的大小
        UINT16    BlockCount;                 // Block的个数

        UINT32 TotalHttpDataBytes;          // 所有HttpDownloader下载的有效字节数
        UINT32 TotalLocalDataBytes;         // 所有本地已经下载过的有效字节数
        UINT8 Resersed[992];                // 保留字段

        UINT8  HttpDownloaderCount;
        HTTP_DOWNLOADER_INFO HttpDownloaders[]; // 变长; (连续存放)
    };

    typedef VarSizeStruct<DOWNLOADDRIVER_STATISTIC_INFO, HTTP_DOWNLOADER_INFO> DOWNLOADDRIVER_STATISTIC_INFO_EX;

    inline ostream& operator << (ostream& os, const DOWNLOADDRIVER_STATISTIC_INFO& info)
    {
        return os << "DownloadDriverID: " << info.DownloadDriverID 
            << ", OriginalUrl: " << info.OriginalUrl
            << ", OriginalReferUrl: " << info.OriginalReferUrl
            << ", HttpDownloaderCount: " << info.HttpDownloaderCount;
    }
}

#pragma pack(pop)
