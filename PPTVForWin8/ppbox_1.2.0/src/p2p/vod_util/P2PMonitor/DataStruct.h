#include <windows.h>
#include <boost/shared_array.hpp>
#include <iostream>
#include "statistic/StatisticStructs.h"
using namespace std;

#pragma once

namespace p2p_monitor
{

#pragma pack(push, 1)


    //////////////////////////////////////////////////////////////////////////
    // Constants


typedef GUID MONITOR_RID;

    const int BITMAP_SIZE = 50;

    const int UINT8_MAX_VALUE = 256;
    
    const int LIVEDOWNLOADER_MAX_COUNT = 128;

    const int MAX_IP_COUNT = 10;

    const int MONITOR_MAX_P2P_DOWNLOADER_COUNT = 100;

    template <typename StructWithTailEmptySizeArray, typename TailElement>
    class VarSizeStruct // 基类不能有大小为0的数组
    {
    public:

        typedef VarSizeStruct<StructWithTailEmptySizeArray, TailElement> Type;

    public:

        VarSizeStruct(int tail_element_count = 0)
            : tail_element_count_(tail_element_count)
            , data_(new byte[ sizeof(StructWithTailEmptySizeArray) + tail_element_count * sizeof(TailElement) ])
        {
            Clear();
        }
        
        StructWithTailEmptySizeArray& operater()
        {
            return Ref();
        }

        StructWithTailEmptySizeArray& Ref()
        {
            return *(StructWithTailEmptySizeArray*)data_.get();
        }

        void Clear()
        {
            memset(data_.get(), 0, Size());
        }

        int Size() const
        {
            return sizeof(StructWithTailEmptySizeArray) + tail_element_count_ * sizeof(TailElement);
        }

        int MaxTailElementCount() const
        {
            return tail_element_count_;
        }

    private:

        boost::shared_array<byte> data_;

        int tail_element_count_;
    };

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

        void operator = (statistic::SPEED_INFO si)
        {
            StartTime = si.StartTime;
            TotalDownloadBytes = si.TotalDownloadBytes;
            TotalUploadBytes = si.TotalUploadBytes;
            NowDownloadSpeed = si.NowDownloadSpeed;
            NowUploadSpeed = si.NowUploadSpeed;
            MinuteDownloadSpeed = si.MinuteDownloadSpeed;
            MinuteUploadSpeed = si.MinuteUploadSpeed;
            AvgDownloadSpeed = si.AvgDownloadSpeed;
            AvgUploadSpeed = si.AvgUploadSpeed;
        }
    };

    struct SPEED_INFO_EX : SPEED_INFO
    {
        UINT32 RecentDownloadSpeed;     // 当前下载速度 <20s>
        UINT32 RecentUploadSpeed;       // 当前上传速度 <20s>
        UINT32 SecondDownloadSpeed;     // 当前1s的下载速度
        UINT32 SecondUploadSpeed;       // 当前1s的上传速度

        SPEED_INFO_EX()
        {
            memset(this, 0, sizeof(SPEED_INFO_EX));
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
    //typedef protocol::CANDIDATE_PEER_INFO CANDIDATE_PEER_INFO;

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

    struct TRACKER_INFO
    {
        UINT16 Length;
        UINT8 ModNo;
        UINT32 IP;
        UINT16 Port;
        UINT8 Type; // 1 UDP

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

    struct STATISTIC_TRACKER_INFO
    {
        // 静态
        TRACKER_INFO TrackerInfo; // 该Tracker的基本信息

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
        PIECE_INFO_EX(statistic::PIECE_INFO_EX pie)
        {
            BlockIndex = pie.BlockIndex;
            PieceIndexInBlock = pie.PieceIndexInBlock;
            SubPieceIndexInPiece = pie.SubPieceIndexInPiece;
        }
        PIECE_INFO_EX operator = (statistic::PIECE_INFO_EX pie)
        {
            PIECE_INFO_EX piece_info_ex;
            piece_info_ex.BlockIndex = pie.BlockIndex;
            piece_info_ex.PieceIndexInBlock = pie.PieceIndexInBlock;
            piece_info_ex.SubPieceIndexInPiece = pie.SubPieceIndexInPiece;
            return piece_info_ex;
        }
    };

    inline ostream& operator << (ostream& os, const PIECE_INFO_EX& info)
    {
        return os << "BlockIndex: " << info.BlockIndex << ", PieceIndexInBlock: " << info.PieceIndexInBlock
            << ", SubPieceIndexInBlock: " << info.SubPieceIndexInPiece;
    }

    struct CANDIDATE_PEER_INFO
    {
        UINT32 IP;
        UINT16 UdpPort;
        UINT16 TcpPort;
        UINT32 DetectIP;
        UINT16 DetectUdpPort;
        UINT32 StunIP;
        UINT16 StunUdpPort;
        UINT8  PeerNatType;
        UINT8  UploadPriority;
        UINT8  IdleTimeInMins;
        UINT8  TrackerPriority;

        bool operator ==(const CANDIDATE_PEER_INFO& n) const
        {
            return 0 == memcmp(this, &n, sizeof(CANDIDATE_PEER_INFO));
        }
        /*
        bool operator < (const CANDIDATE_PEER_INFO& n) const
        {
        return true;
        }*/

        void operator = (protocol::CandidatePeerInfo info)
        {
            IP = info.IP;
            UdpPort = info.UdpPort;
            TcpPort = info.PeerVersion;
            DetectIP = info.DetectIP;
            DetectUdpPort = info.DetectUdpPort;
            StunIP = info.StunIP;
            StunUdpPort = info.StunUdpPort;
            PeerNatType = info.PeerNatType;
            UploadPriority = info.UploadPriority;
            IdleTimeInMins = info.IdleTimeInMins;
            TrackerPriority = info.TrackerPriority;
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
            PeerNatType = -1;
            UploadPriority = 0;
            IdleTimeInMins = 0;
            TrackerPriority = 0;
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
        MONITOR_RID    P2PDownloaderRIDs[UINT8_MAX_VALUE]; // 正在P2P下载的资源RID; 如果为 GUID_NULL 表示空; (不连续)

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
        UINT32  BandWidth;                          // 预测的带宽

        UINT32  GlobalWindowSize;                   // 全局window size

		UINT16  GlobalRequestSendCount;				//每秒发出的请求数
		UINT16  MemoryPoolLeftSize;					//内存池剩余大小

        // LiveDownloadDriver 相关信息
        UINT8  LiveDownloadDriverCount;               // 正在下载的直播视频
        UINT32 LiveDownloadDriverIDs[LIVEDOWNLOADER_MAX_COUNT];// 正在下载的直播视频 驱动器ID; 如果为 0 表示不存在; (不连续)

        // 统计BHO和FlOAT的点击次数
        //protocol::ACTION_COUNT_INFO ActionCountInfo;
        UINT8   Resersed[946 - 1 - LIVEDOWNLOADER_MAX_COUNT * 4];                      // 保留字段

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

        void operator = (const protocol::PEER_DOWNLOAD_INFO& info)
        {
            IsDownloading = info.IsDownloading;
            OnlineTime = info.OnlineTime;
            AvgDownload = info.AvgDownload;
            NowDownload = info.NowDownload;
            AvgUpload = info.AvgUpload;
            NowUpload = info.NowUpload;
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

    struct PEER_INFO
    {
        UINT8 download_connected_count_;
        UINT8 upload_connected_count_;
        UINT32 mine_upload_speed_;
        UINT32 max_upload_speed_;
        UINT32 rest_playable_time_;
        UINT8 lost_rate_;
        UINT8 redundancy_rate_;

        PEER_INFO()
        {
            download_connected_count_ = 0;
            upload_connected_count_ = 0;
            mine_upload_speed_ = 0;
            max_upload_speed_ = 0;
            rest_playable_time_ = 0;
            lost_rate_ = 0;
            redundancy_rate_ = 0;
        }

        void operator = (statistic::PEER_INFO info)
        {
            download_connected_count_ = info.download_connected_count_;
            upload_connected_count_ = info.upload_connected_count_;
            mine_upload_speed_ = info.mine_upload_speed_;
            max_upload_speed_ = info.max_upload_speed_;
            rest_playable_time_ = info.rest_playable_time_;
            lost_rate_ = info.lost_rate_;
            redundancy_rate_ = info.redundancy_rate_;
        }

        void Clear()
        {
            memset(this, 0, sizeof(PEER_INFO));
        }
    };

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

        //////////////////////////////////////////////////////////////////////////
        UINT8  IsRidInfoValid;                 // 0-Invalid; 1-Valid
        UINT16 Sent_Count;
        UINT16 Requesting_Count;
        UINT16 Received_Count; 
        UINT16 AssignedLeftSubPieceCount;
        UINT32 LastLivePieceID;
        UINT32 FirstLiveBlockID;
        UINT8  ConnectType;

        PEER_INFO RealTimePeerInfo;
        UINT32 ActualAssignedSubPieceCount;  // 当前1秒钟分配的SubPiece数
        UINT32 RequestSubPieceCount;         // 当前1秒钟发出的SubPiece请求数
        UINT32 SupplySubPieceCount;          // 可供下载的SubPiece数(我没有但是对方有的SuPiece数)
        UINT32 TimeOfNoResponse;             // 上一次收到该peer的包到现在为止过了多久
        UINT8  Reserved[150];                  // 保留字段

        void operator = (statistic::P2P_CONNECTION_INFO pci)
        {
            boost::array<boost::uint8_t, sizeof(UUID)> test = pci.PeerGuid.to_bytes();
            PeerGuid.Data1 = pci.PeerGuid.to_bytes()[0] + pci.PeerGuid.to_bytes()[1] * 256
                + pci.PeerGuid.to_bytes()[2] * 256 * 256 + pci.PeerGuid.to_bytes()[3] * 256 * 256 * 256;
            PeerGuid.Data2 = pci.PeerGuid.to_bytes()[4] + pci.PeerGuid.to_bytes()[5] * 256;
            PeerGuid.Data3 = pci.PeerGuid.to_bytes()[6] + pci.PeerGuid.to_bytes()[7] * 256;
            PeerGuid.Data4[0] = pci.PeerGuid.to_bytes()[8];
            PeerGuid.Data4[1] = pci.PeerGuid.to_bytes()[9];
            PeerGuid.Data4[2] = pci.PeerGuid.to_bytes()[10];
            PeerGuid.Data4[3] = pci.PeerGuid.to_bytes()[11];
            PeerGuid.Data4[4] = pci.PeerGuid.to_bytes()[12];
            PeerGuid.Data4[5] = pci.PeerGuid.to_bytes()[13];
            PeerGuid.Data4[6] = pci.PeerGuid.to_bytes()[14];
            PeerGuid.Data4[7] = pci.PeerGuid.to_bytes()[15];
            SpeedInfo = pci.SpeedInfo;
            PeerVersion = pci.PeerVersion;
            PeerType = pci.Reserve;
            PeerInfo = pci.PeerInfo;
            PeerDownloadInfo = pci.PeerDownloadInfo;
            for (int i = 0; i < BITMAP_SIZE; ++i)
            {
                BitMap[i] = pci.BitMap[i];
            }
            RTT_Count = pci.RTT_Count;
            RTT_Now = pci.RTT_Now;
            RTT_Average = pci.RTT_Average;
            RTT_Max = pci.RTT_Max;
            RTT_Total = pci.RTT_Total;
            ElapseTime = pci.ElapseTime;
            WindowSize = pci.WindowSize;
            AssignedSubPieceCount = pci.AssignedSubPieceCount;
            AverageDeltaTime = pci.AverageDeltaTime;
            SortedValue = pci.SortedValue;
            IsRidInfoValid = pci.IsRidInfoValid;
            Sent_Count = pci.Sent_Count;
            Requesting_Count = pci.Requesting_Count;
            Received_Count = pci.Received_Count;
            AssignedLeftSubPieceCount = pci.AssignedLeftSubPieceCount;
            LastLivePieceID = pci.LastLiveBlockId;
            FirstLiveBlockID = pci.FirstLiveBlockId;
            ConnectType = pci.ConnectType;
            RealTimePeerInfo = pci.RealTimePeerInfo;
            ActualAssignedSubPieceCount = pci.ActualAssignedSubPieceCount;
            RequestSubPieceCount = pci.RequestSubPieceCount;
            SupplySubPieceCount = pci.SupplySubPieceCount;
            TimeOfNoResponse = pci.TimeOfNoResponse;
        }
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
        MONITOR_RID    ResourceID;                 //对应的RID
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
        UINT16 TotalUnusedSubPieceCount_;      // 冗余的Subpieces数
        UINT16 TotalRecievedSubPieceCount_;    // 收到的Subpiece数
        UINT16 TotalRequestSubPieceCount_;     // 发出的Subpiece请求数
        UINT16 SubPieceRetryRate;             // 冗余率: 冗余 / 收到
        UINT16 UDPLostRate;                   // 丢包率: (发出 - 收到) / 发出

        UINT32 TotalP2PDataBytes;             // 当前P2PDownloader下载的有效字节数
        UINT16 FullBlockPeerCount;            // 在已经连接的Peer中，资源全满的Peer个数

        UINT32 TotalUnusedSubPieceCount;      // 冗余的Subpieces数
        UINT32 TotalRecievedSubPieceCount;    // 收到的Subpiece数
        UINT32 TotalRequestSubPieceCount;     // 发出的Subpiece请求数
        UINT32 NonConsistentSize;             // 当前下载的最后一个piece和第一个piece之间的距离
        UINT16 ConnectCount;
        UINT16 KickCount;
        UINT32 empty_subpiece_distance;

        SPEED_INFO PeerSpeedInfo;
        SPEED_INFO SnSpeedInfo;
        UINT32 TotalP2PSnDataBytes;

        UINT8 Reserved[894];                  // 保留

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
        UINT8  IsPause;                   // 是否暂停
        UINT8  Resersed[399];               // 保留字段

        HTTP_DOWNLOADER_INFO()
        {
            Clear();
        }

        void Clear()
        {
            memset(this, 0, sizeof(HTTP_DOWNLOADER_INFO));
        }

        HTTP_DOWNLOADER_INFO(statistic::HTTP_DOWNLOADER_INFO hdi)
        {
            for (int i = 0; i < 256; ++i)
            {
                Url[i] = hdi.Url[i];
                ReferUrl[i] = hdi.ReferUrl[i];
                RedirectUrl[i] = hdi.RedirectUrl[i];
            }
            SpeedInfo = hdi.SpeedInfo;
            DownloadingPieceEx = hdi.DownloadingPieceEx;
            StartPieceEx = hdi.StartPieceEx;
            LastConnectedTime = hdi.LastConnectedTime;
            LastRequestPieceTime = hdi.LastRequestPieceTime;
            LastHttpStatusCode = hdi.LastHttpStatusCode;
            RetryCount = hdi.RetryCount;
            IsSupportRange = hdi.IsSupportRange;
            IsDeath = hdi.IsDeath;
        }
    };

    struct DOWNLOADDRIVER_STATISTIC_INFO
    {
        UINT32 DownloadDriverID;
        SPEED_INFO SpeedInfo;

        UINT8  OriginalUrl[256];
        UINT8  OriginalReferUrl[256];

        MONITOR_RID       ResourceID;                 // 资源RID (可以为全0)
        UINT32    FileLength;                    // 文件的长度
        UINT32    BloskSize;                  // Block的大小
        UINT16    BlockCount;                 // Block的个数

        UINT32 TotalHttpDataBytesWithoutRedundance;          // 所有HttpDownloader下载的有效字节数
        UINT32 TotalLocalDataBytes;         // 所有本地已经下载过的有效字节数
        UINT8 FileName[256];                // 文件名,(TCHAR*)

        UINT8 IsHidden;                      // 是否隐藏(不在界面上显示进度)

        UINT8 SourceType;  // 标识是否是客户端
        UINT8 Resersed1[256]; //保留字段

        UINT8   StateMachineType;
        UINT8   StateMachineState[14];

        UINT32 PlayingPosition;
        UINT32 DataRate;
        UINT8 http_state;
        UINT8 p2p_state;
        UINT8 timer_using_state;
        UINT8 timer_state;
        
        INT32 t;
        INT32 b;
        INT32 speed_limit;

        UINT32 TotalHttpDataBytesWithRedundance; // 所有HttpDownloader下载的有效字节数, 包含冗余
        UINT8 sn_state;
       
        UINT8 Resersed[434];                // 保留字段

        UINT8  HttpDownloaderCount;
        HTTP_DOWNLOADER_INFO HttpDownloaders[]; // 变长; (连续存放)

        DOWNLOADDRIVER_STATISTIC_INFO(statistic::DOWNLOADDRIVER_STATISTIC_INFO dsi)
        {
            DownloadDriverID = dsi.DownloadDriverID;
            SpeedInfo = dsi.SpeedInfo;
            for (int i = 0; i < 256; ++i)
            {
                OriginalUrl[i] = dsi.OriginalUrl[i];
                OriginalReferUrl[i] = dsi.OriginalReferUrl[i];
            }
            //ResourceID = dsi.ResourceID.to_bytes();
            FileLength = dsi.FileLength;
            BloskSize = dsi.BlockSize;
            BlockCount = dsi.BlockCount;
            TotalHttpDataBytesWithoutRedundance = dsi.TotalHttpDataBytesWithoutRedundance;
            TotalLocalDataBytes = dsi.TotalLocalDataBytes;
            for (int i = 0; i < 256; ++i)
            {
                FileName[i] = dsi.FileName[i];
            }
            IsHidden = dsi.IsHidden;
            SourceType = dsi.SourceType;
         
            StateMachineType = dsi.StateMachineType;
            for (int i = 0; i < 14; ++i)
            {
                StateMachineState[i] = dsi.StateMachineState[i];
            }
            PlayingPosition = dsi.PlayingPosition;
            DataRate = dsi.DataRate;
            http_state = dsi.http_state;
            p2p_state = dsi.p2p_state;
            timer_using_state = dsi.timer_using_state;
            timer_state = dsi.timer_state;
            t = dsi.t;
            b = dsi.b;
            speed_limit = dsi.speed_limit;
           
            HttpDownloaderCount = dsi.HttpDownloaderCount;
            for (int i = 0; i < HttpDownloaderCount; ++i)
            {
                HttpDownloaders[i] = dsi.HttpDownloaders[i];
            }
            sn_state=dsi.sn_state;
        }
        DOWNLOADDRIVER_STATISTIC_INFO(){}
    };

    typedef VarSizeStruct<DOWNLOADDRIVER_STATISTIC_INFO, HTTP_DOWNLOADER_INFO> DOWNLOADDRIVER_STATISTIC_INFO_EX;

    inline ostream& operator << (ostream& os, const DOWNLOADDRIVER_STATISTIC_INFO& info)
    {
        return os << "DownloadDriverID: " << info.DownloadDriverID 
            << ", OriginalUrl: " << info.OriginalUrl
            << ", OriginalReferUrl: " << info.OriginalReferUrl
            << ", HttpDownloaderCount: " << info.HttpDownloaderCount;
    }

    //////////////////////////////////////////////////////////////////////////
    // LiveDownloadDriverStatistic 结构
    // 共享内存名： LIVEDOWNLOADDRIVER_<PID>_<DownloadDriverID>
    struct LIVE_DOWNLOADDRIVER_STATISTIC_INFO
    {
        UINT32 LiveDownloadDriverID;
        SPEED_INFO LiveHttpSpeedInfo;
        SPEED_INFO LiveP2PSpeedInfo;
        SPEED_INFO LiveP2PSubPieceSpeedInfo;
        UINT8 http_state;
        UINT8 p2p_state;
        UINT8  OriginalUrl[256];              // CDN IP
        UINT16 LastHttpStatusCode;            // 上次HTTP请求的返回值 (例如200.206)
        UINT32 TotalP2PDataBytes;             // 当前P2PDownloader下载的有效字节数
        UINT32 TotalRecievedSubPieceCount;    // 收到的Subpiece数
        UINT32 TotalRequestSubPieceCount;     // 发出的Subpiece请求数
        UINT32 TotalAllRequestSubPieceCount;  // 发出的Subpiece请求数
        UINT32 TotalUnusedSubPieceCount;      // 冗余的Subpieces数
        UINT16 IpPoolPeerCount;               // 备选IP
        UINT32 DataRate;                      // 码流率
        UINT16 CacheSize;                      // 已缓存大小
        UINT32 CacheFirstPieceID;             // 已缓存的第一片Piece ID
        UINT32 CacheLastPieceID;              // 已缓存的最后一片Piece ID
        UINT32 PlayingPosition;               // 播放点
        UINT32 LeftCapacity;                  // 内存池剩余
        INT32  RestPlayTime;                  // 剩余时间
        MONITOR_RID    ResourceID;                 // 资源RID
        UINT8  IsGlobalPlayPositionPieceFull; // 正在推给播放器的那片piece是否为满的
        UINT32 LivePointBlockId;              // 直播点的Block ID
        INT32 DataRateLevel;                  // 码流等级
        INT32 P2PFailedTimes;                  // P2P下载失败的次数
        UINT8 HttpSpeedStatus;                 // Http速度状态(0, 1, 2分别代表fast, checking, slow)
        UINT8 HttpStatus;                      // Http状态(unknown, good, bad)
        UINT8 P2PStatus;                       // P2P状态(unknown, good, bad)
        UINT32 JumpTimes;                      // 跳跃次数
        UINT32 NumOfChecksumPieces;            // 校验失败的piece数
        UINT8 Is3200P2PSlow;                   // 3200状态下P2P是否慢
        MONITOR_RID   ChannelID;                       // 频道ID
        UINT32 TotalUdpServerDataBytes;        // 从UdpServer下载的字节数
        UINT8 PmsStatus;                       // PMS是否正常
        UINT32 UniqueID;                       // 播放器ID
        SPEED_INFO UdpServerSpeedInfo;         // UdpServer速度信息
        UINT8 IsPaused;                        // 是否暂停，0代表播放，1代表暂停
        UINT8 IsReplay;                        // 是否回拖，0代表不回拖，1代表回拖
        UINT32 MissingSubPieceCountOfFirstBlock;  // 第一个不满的Block中空的SubPiece个数
        UINT32 ExistSubPieceCountOfFirstBlock;  // 第一个不满的Block中存在的SubPiece个数
        UINT32 P2PPeerSpeedInSecond;           // P2P Peer一秒的速度
        UINT32 P2PUdpServerSpeedInSecond;      // UdpServer一秒的速度

        UINT8 Reserved[896];

        UINT16 PeerCount;                     // Peer的
        P2P_CONNECTION_INFO P2PConnections[];  // 变长; (连续存放)

        void Clear()
        {
            memset(this, 0, sizeof(LIVE_DOWNLOADDRIVER_STATISTIC_INFO));
        }
    };

    typedef VarSizeStruct<LIVE_DOWNLOADDRIVER_STATISTIC_INFO, P2P_CONNECTION_INFO> LIVE_DOWNLOADDRIVER_STATISTIC_INFO_EX;

    struct PEER_UPLOAD_INFO
    {
        UINT32 ip;
        UINT16 port;
        UINT32 upload_speed;
        PEER_INFO peer_info;
        UINT8 resersed[110];

        void Clear()
        {
            memset(this, 0, sizeof(PEER_UPLOAD_INFO));
        }
    };

    struct UPLOAD_INFO
    {
        UINT8 peer_upload_count;
        UINT8 speed_limit;
        UINT32 upload_speed;
        UINT32 actual_speed_limit;
        UINT32 upload_subpiece_count;
        UINT8 resersed[116];
        PEER_UPLOAD_INFO peer_upload_info[256];
    };

#pragma pack(pop)
    }
