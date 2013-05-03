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
    class VarSizeStruct // ���಻���д�СΪ0������
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
    // �����ṹ

    struct SPEED_INFO
    {
        UINT32 StartTime;               // ��ʼʱ��
        UINT32 TotalDownloadBytes;      // �ܹ����ص��ֽ���
        UINT32 TotalUploadBytes;        // �ܹ��ϴ�ʱ�ֽ���
        UINT32 NowDownloadSpeed;        // ��ǰ�����ٶ� <5s>
        UINT32 NowUploadSpeed;          // ��ǰ�ϴ��ٶ� <5s>
        UINT32 MinuteDownloadSpeed;     // ���һ����ƽ�������ٶ� <60s>
        UINT32 MinuteUploadSpeed;       // ���һ����ƽ���ϴ��ٶ� <60s>
        UINT32 AvgDownloadSpeed;        // ��ʷƽ�������ٶ�
        UINT32 AvgUploadSpeed;          // ��ʷƽ���ϴ��ٶ�

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
        UINT32 RecentDownloadSpeed;     // ��ǰ�����ٶ� <20s>
        UINT32 RecentUploadSpeed;       // ��ǰ�ϴ��ٶ� <20s>
        UINT32 SecondDownloadSpeed;     // ��ǰ1s�������ٶ�
        UINT32 SecondUploadSpeed;       // ��ǰ1s���ϴ��ٶ�

        SPEED_INFO_EX()
        {
            memset(this, 0, sizeof(SPEED_INFO_EX));
        }
    };

    /*
    struct CANDIDATE_PEER_INFO
    {
    UINT32 IP;                      // �Լ�����IP
    UINT16 UdpPort;                 // �Լ��ı���UDP Port
    UINT16 TcpPort;                 // �Լ��ı���TCP Port
    UINT32 DetectIP;                // �Լ��ӷ�������õ�̽�� IP
    UINT16 DetectUdpPort;           // �Լ��ӷ�������õ�̽�� UDP Port
    UINT32 StunIP;                  // �Լ�ѡ��� Stun Peer ��IP
    UINT16 StunUdpPort;             // �Լ�ѡ��� Stun Peer �� Port
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
        // ��̬
        TRACKER_INFO TrackerInfo; // ��Tracker�Ļ�����Ϣ

        // ��̬��Ϣ
        UINT16 CommitRequestCount;       // ��Tracker Commit ���������
        UINT16 CommitResponseCount;      // ��Tracker Commit ����Ӧ����
        UINT16 KeepAliveRequestCount;    // ��Tracker KeepAlive ���������
        UINT16 KeepAliveResponseCount;   // ��Tracker KeepAlive ����Ӧ����
        UINT16 ListRequestCount;         // ��Tracker List ���������
        UINT16 ListResponseCount;        // ��Tracker List ����Ӧ����
        UINT8  LastListReturnPeerCount;  // ��һ�γɹ���List�ķ��ص�Peer��
        UINT8  IsSubmitTracker;          // �ڸ�Group���Ƿ�Ϊ��ǰѡ��Tracker
        UINT8  ErrorCode;                // �ϴ�Tracker���صĴ�����
        UINT16 KeepAliveInterval;        // ��һ�δӷ��������ص�Submit���

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
    // AppStopʱͳ���ϴ�

    //////////////////////////////////////////////////////////////////////////
    // StatisticModule �ṹ
    //   �����ڴ���: PPVIDEO_<PID>
    struct STASTISTIC_INFO
    {
        // �ٶ������Ϣ
        SPEED_INFO SpeedInfo;                  // ���� �ֽ��� �� �ٶ�

        // IP �����Ϣ
        CANDIDATE_PEER_INFO LocalPeerInfo;     // �Լ� �Ļ���IP�˿���Ϣ
        UINT8  LocalIpCount;                   // ����ͨ��API ��õı���IP�� 
        UINT32 LocalIPs[MAX_IP_COUNT];         // ����ͨ��API ��õ�IP; (�������)

        // �Լ����������Ϣ
        UINT32 LocalPeerVersion;               // �Լ��ں˰汾��
        UINT8  LocalPeerType;                  // �Լ���Peer����

        // TrackerServer�����Ϣ
        UINT8  TrackerCount;                       // Tracker�ĸ���
        UINT8  GroupCount;                         // Group ��, Ҳ��MOD�ĳ���
        STATISTIC_TRACKER_INFO TrackerInfos[UINT8_MAX_VALUE]; // Tracker, (�������)

        // IndexServer �����Ϣ
        STATISTIC_INDEX_INFO StatisticIndexInfo;   // IndexServer�������Ϣ

        // P2PDownloader �����Ϣ
        UINT8  P2PDownloaderCount;                 // �ж��ٸ�����P2P���ص���Դ
        MONITOR_RID    P2PDownloaderRIDs[UINT8_MAX_VALUE]; // ����P2P���ص���ԴRID; ���Ϊ GUID_NULL ��ʾ��; (������)

        // DownloadDriver �����Ϣ
        UINT8  DownloadDriverCount;               // �������ص���Ƶ    
        UINT32 DownloadDriverIDs[UINT8_MAX_VALUE];// �������ص���Ƶ ������ID; ���Ϊ 0 ��ʾ������; (������)

        //ֹͣʱ�����ϴ����
        UINT32 TotalP2PDownloadBytes;
        UINT32 TotalOtherServerDownloadBytes;

        //������ �������� ʵʱ�����Ϣ
        UINT32  TotalHttpNotOriginalDataBytes;        // ʵʱ ���صĴ����� �ֽ���
        UINT32  TotalP2PDataBytes;                    // ʵʱ P2P���صĴ����� �ֽ���
        UINT32  TotalHttpOriginalDataBytes;            // ʵʱ ԭ�����صĴ����� �ֽ���
        UINT32  CompleteCount;                        // �Ѿ���ɵ�DD��

        UINT32  TotalUploadCacheRequestCount;       // �ܹ����ϴ�Cache������
        UINT32  TotalUploadCacheHitCount;           // �ܹ����ϴ�Cache������

        UINT16  HttpProxyPort;                      // HTTPʵ�ʴ���˿�
        UINT32  MaxHttpDownloadSpeed;               // HTTP��������ٶ�
        UINT16  IncomingPeersCount;                 // �ܹ������Peer����
        UINT16  DownloadDurationInSec;              // �����ܹ�����ʱ��(��)
        UINT32  BandWidth;                          // Ԥ��Ĵ���

        UINT32  GlobalWindowSize;                   // ȫ��window size

		UINT16  GlobalRequestSendCount;				//ÿ�뷢����������
		UINT16  MemoryPoolLeftSize;					//�ڴ��ʣ���С

        // LiveDownloadDriver �����Ϣ
        UINT8  LiveDownloadDriverCount;               // �������ص�ֱ����Ƶ
        UINT32 LiveDownloadDriverIDs[LIVEDOWNLOADER_MAX_COUNT];// �������ص�ֱ����Ƶ ������ID; ���Ϊ 0 ��ʾ������; (������)

        // ͳ��BHO��FlOAT�ĵ������
        //protocol::ACTION_COUNT_INFO ActionCountInfo;
        UINT8   Resersed[946 - 1 - LIVEDOWNLOADER_MAX_COUNT * 4];                      // �����ֶ�

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
    // P2PDownloaderStatistic �ṹ
    //   �����ڴ���: P2PDOWNLOADER_<PID>_<RID>
    //   NOTE: RID��ʽ{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}
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
        GUID   PeerGuid;                       // ��Peer��PeerGuid
        SPEED_INFO SpeedInfo;                  // ��Peer ���ٶ���Ϣ
        UINT32 PeerVersion;                    // �Է�Peer���ں˰汾��
        UINT8  PeerType;                       // �Է�Peer���ں�����
        CANDIDATE_PEER_INFO PeerInfo;          // ��Peer�� IP��Ϣ
        PEER_DOWNLOAD_INFO PeerDownloadInfo;   // �Է������ ���� ��Ϣ
        UINT8  BitMap[BITMAP_SIZE];            // �Է���BitMap
        UINT32 RTT_Count;                      // Udp���ĸ���
        UINT16 RTT_Now;                        // ��ǰ��rtt
        UINT16 RTT_Average;                    // �ܹ���ƽ��rtt; �ɻ�������udp�����յ�ʱ��ֲ�ͼ
        UINT16 RTT_Max;                        // �յ���udp����rtt���
        UINT32 RTT_Total;                      // �ܹ���rtt
        UINT16 ElapseTime;                     // ��ǰ��ʱʱ��
        UINT8  WindowSize;                     // ���ڴ�С
        UINT8  AssignedSubPieceCount;          // ��ǰԤ����SubPiece��
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
        UINT32 ActualAssignedSubPieceCount;  // ��ǰ1���ӷ����SubPiece��
        UINT32 RequestSubPieceCount;         // ��ǰ1���ӷ�����SubPiece������
        UINT32 SupplySubPieceCount;          // �ɹ����ص�SubPiece��(��û�е��ǶԷ��е�SuPiece��)
        UINT32 TimeOfNoResponse;             // ��һ���յ���peer�İ�������Ϊֹ���˶��
        UINT8  Reserved[150];                  // �����ֶ�

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
        MONITOR_RID    ResourceID;                 //��Ӧ��RID
        SPEED_INFO SpeedInfo;

        // ��Դ�����Ϣ
        UINT32 FileLength;
        UINT16 BlockNum;
        UINT16 BlockSize;

        // IPPool��Ϣ
        UINT16 IpPoolPeerCount;        // ��ѡIP    
        UINT8  ExchangingPeerCount;    // ���ڽ�����Ϣ��IP    
        UINT8  ConnectingPeerCount;    // �������ӵ�IP     

        // �㷨�����Ϣ
        UINT16 TotalWindowSize;               // �ܴ��ڴ�С
        UINT16 TotalAssignedSubPieceCount;    // ��ǰ��������SubPiece��
        UINT16 TotalUnusedSubPieceCount_;      // �����Subpieces��
        UINT16 TotalRecievedSubPieceCount_;    // �յ���Subpiece��
        UINT16 TotalRequestSubPieceCount_;     // ������Subpiece������
        UINT16 SubPieceRetryRate;             // ������: ���� / �յ�
        UINT16 UDPLostRate;                   // ������: (���� - �յ�) / ����

        UINT32 TotalP2PDataBytes;             // ��ǰP2PDownloader���ص���Ч�ֽ���
        UINT16 FullBlockPeerCount;            // ���Ѿ����ӵ�Peer�У���Դȫ����Peer����

        UINT32 TotalUnusedSubPieceCount;      // �����Subpieces��
        UINT32 TotalRecievedSubPieceCount;    // �յ���Subpiece��
        UINT32 TotalRequestSubPieceCount;     // ������Subpiece������
        UINT32 NonConsistentSize;             // ��ǰ���ص����һ��piece�͵�һ��piece֮��ľ���
        UINT16 ConnectCount;
        UINT16 KickCount;
        UINT32 empty_subpiece_distance;

        SPEED_INFO PeerSpeedInfo;
        SPEED_INFO SnSpeedInfo;
        UINT32 TotalP2PSnDataBytes;

        UINT8 Reserved[894];                  // ����

        UINT16 PeerCount;                     // Peer��
        P2P_CONNECTION_INFO P2PConnections[]; // �䳤; (�������)
    };

    typedef VarSizeStruct<P2PDOWNLOADER_STATISTIC_INFO, P2P_CONNECTION_INFO> P2PDOWNLOADER_STATISTIC_INFO_EX;


    //////////////////////////////////////////////////////////////////////////
    // DownloadDriverStatistic �ṹ
    //   �����ڴ����� DOWNLOADDRIVER_<PID>_<DownloadDriverID>

    struct HTTP_DOWNLOADER_INFO
    {
        UINT8  Url[256];
        UINT8  ReferUrl[256];
        UINT8  RedirectUrl[256]; // ����301����302 �ض����Url

        SPEED_INFO SpeedInfo;

        PIECE_INFO_EX DownloadingPieceEx;   // ������������� PieceEx
        PIECE_INFO_EX StartPieceEx;         // ���ĸ�Subpiece��ʼ����û�Ͽ����ӹ�;
        UINT32 LastConnectedTime;           // �������HTTP���ӵ�ʱ��
        UINT32 LastRequestPieceTime;        // �������PieceEx��ʱ��

        UINT16 LastHttpStatusCode;          // �ϴ�HTTP����ķ���ֵ (����200.206)
        UINT16 RetryCount;                  // Http���Դ���
        UINT8  IsSupportRange;              // �Ƿ�֧��Range

        UINT8  IsDeath;                     // �Ƿ�������
        UINT8  IsPause;                   // �Ƿ���ͣ
        UINT8  Resersed[399];               // �����ֶ�

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

        MONITOR_RID       ResourceID;                 // ��ԴRID (����Ϊȫ0)
        UINT32    FileLength;                    // �ļ��ĳ���
        UINT32    BloskSize;                  // Block�Ĵ�С
        UINT16    BlockCount;                 // Block�ĸ���

        UINT32 TotalHttpDataBytesWithoutRedundance;          // ����HttpDownloader���ص���Ч�ֽ���
        UINT32 TotalLocalDataBytes;         // ���б����Ѿ����ع�����Ч�ֽ���
        UINT8 FileName[256];                // �ļ���,(TCHAR*)

        UINT8 IsHidden;                      // �Ƿ�����(���ڽ�������ʾ����)

        UINT8 SourceType;  // ��ʶ�Ƿ��ǿͻ���
        UINT8 Resersed1[256]; //�����ֶ�

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

        UINT32 TotalHttpDataBytesWithRedundance; // ����HttpDownloader���ص���Ч�ֽ���, ��������
        UINT8 sn_state;
       
        UINT8 Resersed[434];                // �����ֶ�

        UINT8  HttpDownloaderCount;
        HTTP_DOWNLOADER_INFO HttpDownloaders[]; // �䳤; (�������)

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
    // LiveDownloadDriverStatistic �ṹ
    // �����ڴ����� LIVEDOWNLOADDRIVER_<PID>_<DownloadDriverID>
    struct LIVE_DOWNLOADDRIVER_STATISTIC_INFO
    {
        UINT32 LiveDownloadDriverID;
        SPEED_INFO LiveHttpSpeedInfo;
        SPEED_INFO LiveP2PSpeedInfo;
        SPEED_INFO LiveP2PSubPieceSpeedInfo;
        UINT8 http_state;
        UINT8 p2p_state;
        UINT8  OriginalUrl[256];              // CDN IP
        UINT16 LastHttpStatusCode;            // �ϴ�HTTP����ķ���ֵ (����200.206)
        UINT32 TotalP2PDataBytes;             // ��ǰP2PDownloader���ص���Ч�ֽ���
        UINT32 TotalRecievedSubPieceCount;    // �յ���Subpiece��
        UINT32 TotalRequestSubPieceCount;     // ������Subpiece������
        UINT32 TotalAllRequestSubPieceCount;  // ������Subpiece������
        UINT32 TotalUnusedSubPieceCount;      // �����Subpieces��
        UINT16 IpPoolPeerCount;               // ��ѡIP
        UINT32 DataRate;                      // ������
        UINT16 CacheSize;                      // �ѻ����С
        UINT32 CacheFirstPieceID;             // �ѻ���ĵ�һƬPiece ID
        UINT32 CacheLastPieceID;              // �ѻ�������һƬPiece ID
        UINT32 PlayingPosition;               // ���ŵ�
        UINT32 LeftCapacity;                  // �ڴ��ʣ��
        INT32  RestPlayTime;                  // ʣ��ʱ��
        MONITOR_RID    ResourceID;                 // ��ԴRID
        UINT8  IsGlobalPlayPositionPieceFull; // �����Ƹ�����������Ƭpiece�Ƿ�Ϊ����
        UINT32 LivePointBlockId;              // ֱ�����Block ID
        INT32 DataRateLevel;                  // �����ȼ�
        INT32 P2PFailedTimes;                  // P2P����ʧ�ܵĴ���
        UINT8 HttpSpeedStatus;                 // Http�ٶ�״̬(0, 1, 2�ֱ����fast, checking, slow)
        UINT8 HttpStatus;                      // Http״̬(unknown, good, bad)
        UINT8 P2PStatus;                       // P2P״̬(unknown, good, bad)
        UINT32 JumpTimes;                      // ��Ծ����
        UINT32 NumOfChecksumPieces;            // У��ʧ�ܵ�piece��
        UINT8 Is3200P2PSlow;                   // 3200״̬��P2P�Ƿ���
        MONITOR_RID   ChannelID;                       // Ƶ��ID
        UINT32 TotalUdpServerDataBytes;        // ��UdpServer���ص��ֽ���
        UINT8 PmsStatus;                       // PMS�Ƿ�����
        UINT32 UniqueID;                       // ������ID
        SPEED_INFO UdpServerSpeedInfo;         // UdpServer�ٶ���Ϣ
        UINT8 IsPaused;                        // �Ƿ���ͣ��0�����ţ�1������ͣ
        UINT8 IsReplay;                        // �Ƿ���ϣ�0�������ϣ�1�������
        UINT32 MissingSubPieceCountOfFirstBlock;  // ��һ��������Block�пյ�SubPiece����
        UINT32 ExistSubPieceCountOfFirstBlock;  // ��һ��������Block�д��ڵ�SubPiece����
        UINT32 P2PPeerSpeedInSecond;           // P2P Peerһ����ٶ�
        UINT32 P2PUdpServerSpeedInSecond;      // UdpServerһ����ٶ�

        UINT8 Reserved[896];

        UINT16 PeerCount;                     // Peer��
        P2P_CONNECTION_INFO P2PConnections[];  // �䳤; (�������)

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
