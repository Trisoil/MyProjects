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
        // ��̬
        protocal::TRACKER_INFO TrackerInfo; // ��Tracker�Ļ�����Ϣ

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
    };

    inline ostream& operator << (ostream& os, const PIECE_INFO_EX& info)
    {
        return os << "BlockIndex: " << info.BlockIndex << ", PieceIndexInBlock: " << info.PieceIndexInBlock
            << ", SubPieceIndexInBlock: " << info.SubPieceIndexInPiece;
    }

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
        RID    P2PDownloaderRIDs[UINT8_MAX_VALUE]; // ����P2P���ص���ԴRID; ���Ϊ GUID_NULL ��ʾ��; (������)

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
        UINT8   Resersed[958];                      // �����ֶ�

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

        UINT8  Reserved[200];                  // �����ֶ�

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
        RID    ResourceID;                 //��Ӧ��RID
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
        UINT16 TotalUnusedSubPieceCount;      // �����Subpieces��
        UINT16 TotalRecievedSubPieceCount;    // �յ���Subpiece��
        UINT16 TotalRequestSubPieceCount;     // ������Subpiece������
        UINT16 SubPieceRetryRate;             // ������: ���� / �յ�
        UINT16 UDPLostRate;                   // ������: (���� - �յ�) / ����

        UINT32 TotalP2PDataBytes;             // ��ǰP2PDownloader���ص���Ч�ֽ���
        UINT16 FullBlockPeerCount;            // ���Ѿ����ӵ�Peer�У���Դȫ����Peer����
        UINT8 Reserved[994];                  // ����

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
        UINT8  Resersed[400];               // �����ֶ�

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

        RID       ResourceID;                 // ��ԴRID (����Ϊȫ0)
        UINT32    FileLength;                    // �ļ��ĳ���
        UINT32    BloskSize;                  // Block�Ĵ�С
        UINT16    BlockCount;                 // Block�ĸ���

        UINT32 TotalHttpDataBytes;          // ����HttpDownloader���ص���Ч�ֽ���
        UINT32 TotalLocalDataBytes;         // ���б����Ѿ����ع�����Ч�ֽ���
        UINT8 Resersed[992];                // �����ֶ�

        UINT8  HttpDownloaderCount;
        HTTP_DOWNLOADER_INFO HttpDownloaders[]; // �䳤; (�������)
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
