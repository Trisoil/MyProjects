// packet-ppvideo.c
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define DEBUG

#include <epan/packet.h>
#include <epan/prefs.h>
#include <epan/proto.h>
//#include <epan/guid-utils.h>
#include <epan/conversation.h>
#include <string.h>
#include <windows.h>
#include <time.h>

// PPVideo Ports
//    Tracker Listening Port
//static guint ppvideo_tracker_port_cfg = 8888;
//static guint ppvideo_tracker_port = 8888;
//    Index Listening Port
//static guint ppvideo_index_port_cfg = 9999;
//static guint ppvideo_index_port = 9999;
//    Peer Listening Port
//static guint ppvideo_peer_port_cfg = 9000;
//static guint ppvideo_peer_port = 9000;

// Peer to Server Request - PeerVersion
static guint16 ppvideo_peer_version = 1;

// PPVideo Protocol Handle
static int proto_ppvideo_index = -1;
static int proto_ppvideo_tracker = -1;
static int proto_ppvideo_peer = -1;
static int proto_ppvideo_peer_tcp = -1;
static int proto_ppvideo_stun = -1;
static int proto_ppvideo_notify = -1;
static int proto_ppvideo_statistic = -1;
static int proto_ppvideo_data_collector = -1;
static int proto_ppvideo_live_peer = -1;
static int proto_ppvideo = -1;
static int proto_ppvideo_tcp = -1;


// PPVideo Dissector Handle
static dissector_handle_t ppvideo_index_handle;
static dissector_handle_t ppvideo_tracker_handle;
static dissector_handle_t ppvideo_peer_handle;
static dissector_handle_t ppvideo_stun_handle;
static dissector_handle_t ppvideo_statistic_handle;
static dissector_handle_t ppvideo_data_collector_handle;
static dissector_handle_t ppvideo_handle;

// PPVideo Protocol Field
static int hf_ppvideo_HttpServers = -1;
static int hf_ppvideo_HttpServer = -1;
static int hf_ppvideo_HttpServer_Length = -1;
static int hf_ppvideo_HttpServer_Type = -1;
static int hf_ppvideo_HttpServer_UrlLength = -1;
static int hf_ppvideo_HttpServer_UrlString = -1;
static int hf_ppvideo_HttpServer_ReferLength = -1;
static int hf_ppvideo_HttpServer_ReferString = -1;

static int hf_ppvideo_ServerTypeInfos = -1;
static int hf_ppvideo_ServerTypeCount = -1;
static int hf_ppvideo_ServerTypeInfo = -1;
static int hf_ppvideo_ServerTypeInfo_Connect_Type = -1;
static int hf_ppvideo_ServerTypeInfo_Server_Type = -1;
static int hf_ppvideo_ServerTypeInfo_Level = -1;

static int hf_ppvideo_UrlInfo = -1;
static int hf_ppvideo_UrlInfo_Length = -1;
static int hf_ppvideo_UrlInfo_Type = -1;
static int hf_ppvideo_UrlInfo_UrlLength = -1;
static int hf_ppvideo_UrlInfo_UrlString = -1;
static int hf_ppvideo_UrlInfo_ReferLength = -1;
static int hf_ppvideo_UrlInfo_ReferString = -1;

static int hf_ppvideo_RegionInfo = -1;
static int hf_ppvideo_RegionInfo_PublicIP = -1;
static int hf_ppvideo_RegionInfo_Rigion_Code = -1;
static int hf_ppvideo_RegionInfo_Version = -1;

static int hf_ppvideo_ServerListCount = -1;
static int hf_ppvideo_ServerLists = -1;
static int hf_ppvideo_ServerList = -1;
static int hf_ppvideo_ServerList_Padding_Info_Count = -1;
static int hf_ppvideo_ServerList_Padding_Info = -1;

static int hf_ppvideo_ServerDatas = -1;
static int hf_ppvideo_ServerData = -1;
static int hf_ppvideo_ServerData_Count = -1;
static int hf_ppvideo_ServerData_IP = -1;
static int hf_ppvideo_ServerData_Port = -1;
static int hf_ppvideo_ServerData_ModID = -1;
static int hf_ppvideo_ServerData_ConnectType = -1;

static int hf_ppvideo_ResourceInfos = -1;
static int hf_ppvideo_ResourceInfo = -1;
static int hf_ppvideo_ResourceInfo_Length = -1;
static int hf_ppvideo_ResourceInfo_ResourceID = -1;
static int hf_ppvideo_ResourceInfo_FileLength = -1;
static int hf_ppvideo_ResourceInfo_BlockCount = -1;
static int hf_ppvideo_ResourceInfo_BlockSize = -1;
static int hf_ppvideo_ResourceInfo_BlockMD5s = -1;
static int hf_ppvideo_ResourceInfo_BlockMD5 = -1;
static int hf_ppvideo_ResourceInfo_HttpServerCount = -1;

static int hf_ppvideo_PlayHistoryItems = -1;
static int hf_ppvideo_PlayHistoryItem = -1;
static int hf_ppvideo_PushTaskParam = -1;
static int hf_ppvideo_PushTaskParam_Task_Type = -1;
static int hf_ppvideo_PushTaskParam_Protect_Time_Interval_In_Seconds = -1;
static int hf_ppvideo_PushTaskParam_Min_Download_Speed_In_KBps = -1;
static int hf_ppvideo_PushTaskParam_Max_Download_Speed_In_KBps_When_Idle = -1;
static int hf_ppvideo_PushTaskParam_Max_Download_Speed_In_KBps_When_Normal = -1;
static int hf_ppvideo_PushTaskParam_Bandwidth_Ratio_When_Idle = -1;
static int hf_ppvideo_PushTaskParam_Bandwidth_Ratio_When_Normal = -1;

static int hf_ppvideo_RidInfo = -1;

static int hf_ppvideo_TrackerInfos = -1;
static int hf_ppvideo_TrackerInfo = -1;
static int hf_ppvideo_TrackerInfo_Length = -1;
static int hf_ppvideo_TrackerInfo_ModNo = -1;
static int hf_ppvideo_TrackerInfo_IP = -1;
static int hf_ppvideo_TrackerInfo_Port = -1;
static int hf_ppvideo_TrackerInfo_Type = -1;
static int hf_ppvideo_TrackerInfo_StationNo = -1;
static int hf_ppvideo_TrackerInfo_Reserve = -1;
static int hf_ppvideo_TrackerType = -1;

static int hf_ppvideo_StunServerInfos = -1;
static int hf_ppvideo_StunServerInfo = -1;
static int hf_ppvideo_StunServerInfo_Length = -1;
static int hf_ppvideo_StunServerInfo_IP = -1;
static int hf_ppvideo_StunServerInfo_Port = -1;
static int hf_ppvideo_StunServerInfo_Type = -1;

static int hf_ppvideo_SNInfos = -1;
static int hf_ppvideo_SNServerInfo = -1;
static int hf_ppvideo_SNServerInfo_IP = -1;
static int hf_ppvideo_SNServerInfo_Port = -1;
static int hf_ppvideo_SNServerInfo_Priority = -1;

static int hf_ppvideo_CandidatePeerInfos = -1;
static int hf_ppvideo_CandidatePeerInfo = -1;
static int hf_ppvideo_CandidatePeerInfo_IP = -1;
static int hf_ppvideo_CandidatePeerInfo_UdpPort = -1;
static int hf_ppvideo_CandidatePeerInfo_PeerVersion = -1;
static int hf_ppvideo_CandidatePeerInfo_DetectedIP = -1;
static int hf_ppvideo_CandidatePeerInfo_DetectedPort = -1;
static int hf_ppvideo_CandidatePeerInfo_StunIP = -1;
static int hf_ppvideo_CandidatePeerInfo_StunUdpPort = -1;
static int hf_ppvideo_CandidatePeerInfo_PeerNatType = -1;
static int hf_ppvideo_CandidatePeerInfo_UploadPriority = -1;
static int hf_ppvideo_CandidatePeerInfo_IdleTimeInMins = -1;
static int hf_ppvideo_CandidatePeerInfo_TrackerPriority = -1;

static int hf_ppvideo_FileInfo = -1;
static int hf_ppvideo_FileInfo_ResourceID = -1;
static int hf_ppvideo_FileInfo_PeerCount = -1;

static int hf_ppvideo_PeerDownloadInfo = -1;
static int hf_ppvideo_PeerDownloadInfo_IsDownloading = -1;
static int hf_ppvideo_PeerDownloadInfo_OnlineTime = -1;
static int hf_ppvideo_PeerDownloadInfo_AvgDownloadSpeed = -1;
static int hf_ppvideo_PeerDownloadInfo_NowDownloadSpeed = -1;
static int hf_ppvideo_PeerDownloadInfo_AvgUploadSpeed = -1;
static int hf_ppvideo_PeerDownloadInfo_NowUploadSpeed = -1;
//tcp add
static int hf_ppvideo_PeerDownloadInfo_Reserved1 = -1;
static int hf_ppvideo_PeerDownloadInfo_Reserved2 = -1;

static int hf_ppvideo_SubPieceInfos = -1;
static int hf_ppvideo_SubPieceInfo = -1;
static int hf_ppvideo_SubPieceInfo_BlockIndex = -1;
static int hf_ppvideo_SubPieceInfo_SubPieceIndex = -1;

static int hf_ppvideo_LiveBufferMap = -1;

static int hf_ppvideo_BasicInfo = -1;
static int hf_ppvideo_BasicInfo_IsResponse = -1;
static int hf_ppvideo_BasicInfo_Reserved = -1;

static int hf_ppvideo_File_Length = -1;
static int hf_ppvideo_Block_Count = -1;
static int hf_ppvideo_Block_Count_16 = -1;
static int hf_ppvideo_Block_Size = -1;
static int hf_ppvideo_Block_MD5s = -1;
static int hf_ppvideo_Connected_Peers_Count = -1;
static int hf_ppvideo_Active_Peers_Count = -1;
static int hf_ppvideo_Pooled_Peers_Count = -1;

static int hf_ppvideo_Report_Speed = -1;

static int hf_ppvideo_CheckSum = -1;
static int hf_ppvideo_NewCheckSum = -1;
static int hf_ppvideo_Action = -1;
static int hf_ppvideo_Tcp_Action = -1;
static int hf_ppvideo_TransactionID = -1;
static int hf_ppvideo_IsRequest = -1;
static int hf_ppvideo_PeerVersion = -1;
static int hf_ppvideo_Reserve = -1;
static int hf_ppvideo_ErrorCode = -1;
static int hf_ppvideo_ResourceID = -1;
static int hf_ppvideo_HttpServerCount = -1;
static int hf_ppvideo_SessionID = -1;
static int hf_ppvideo_UrlLength = -1;
static int hf_ppvideo_UrlString = -1;
static int hf_ppvideo_ReferLength = -1;
static int hf_ppvideo_ReferString = -1;
static int hf_ppvideo_DetectedIP = -1;
static int hf_ppvideo_DetectedUdpPort = -1;
static int hf_ppvideo_FileLength = -1;
static int hf_ppvideo_BlockCount = -1;
static int hf_ppvideo_BlockSize = -1;
static int hf_ppvideo_BlockMD5s = -1;
static int hf_ppvideo_BlockMD5 = -1;
static int hf_ppvideo_PeerGuid = -1;
static int hf_ppvideo_Status = -1;
static int hf_ppvideo_TrackerGroupCount = -1;
static int hf_ppvideo_TrackerCount = -1;
static int hf_ppvideo_StunServerCount = -1;
static int hf_ppvideo_SNCount = -1;
static int hf_ppvideo_NotifyServerCount = -1;
static int hf_ppvideo_RequestPeerCount = -1;
static int hf_ppvideo_RequestIP = -1;
static int hf_ppvideo_CandidatePeerCount = -1;
static int hf_ppvideo_UdpPort = -1;
static int hf_ppvideo_TcpPort = -1;
static int hf_ppvideo_StunPeerIP = -1;
static int hf_ppvideo_StunPeerUdpPort = -1;
static int hf_ppvideo_RealIPCount = -1;
static int hf_ppvideo_RealIP = -1;
static int hf_ppvideo_RealIPs = -1;
static int hf_ppvideo_UploadBWKbs = -1;
static int hf_ppvideo_UploadLimitKbs = -1;
static int hf_ppvideo_UploadSpeedKbs = -1;
static int hf_ppvideo_bufmapNos = -1;
static int hf_ppvideo_RequestBlockID = -1;
static int hf_ppvideo_ResourceCount = -1;
static int hf_ppvideo_ResourceIDs = -1;
static int hf_ppvideo_KeepAliveInterval = -1;
static int hf_ppvideo_StunDetectedUdpPort = -1;
static int hf_ppvideo_StunDetectedIp = -1;
static int hf_ppvideo_ConfigStringLength = -1;
static int hf_ppvideo_ConfigString = -1;
static int hf_ppvideo_UsedDiskSize = -1;
static int hf_ppvideo_UploadBandWidthKBs = -1;
static int hf_ppvideo_AvgUploadSpeedKBs = -1;
static int hf_ppvideo_PlayHistoryItemCount = -1;
static int hf_ppvideo_VideoNameLength = -1;
static int hf_ppvideo_VideoName = -1;
static int hf_ppvideo_ContinuouslyPlayedDuration = -1;
static int hf_ppvideo_DownloadedSegmentNum = -1;
static int hf_ppvideo_ChannelIDLength = -1;
static int hf_ppvideo_ChannelID = -1;
static int hf_ppvideo_PushWaitIntervalInSeconds = -1;
static int hf_ppvideo_PushTaskItemCount = -1;

static int hf_ppvideo_ProtocolVersion = -1;
static int hf_ppvideo_PeerErrorCode = -1;
static int hf_ppvideo_ErrorInfoLength = -1;
static int hf_ppvideo_ErrorInfo = -1;
static int hf_ppvideo_SendOffTime = -1;
static int hf_ppvideo_ConnectType = -1;
static int hf_ppvideo_BlockMap = -1;
static int hf_ppvideo_PieceCount = -1;
static int hf_ppvideo_Priority = -1;
static int hf_ppvideo_SubPieceLength = -1;
static int hf_ppvideo_SubPieceContent = -1;
static int hf_ppvideo_PeerCount = -1;
static int hf_ppvideo_IpPoolSize = -1;
static int hf_ppvideo_ResourceNameLength = -1;
static int hf_ppvideo_ResourceName = -1;

static int hf_ppvideo_DownloadConnectedCount = -1;
static int hf_ppvideo_UploadConnectedCount = -1;
static int hf_ppvideo_UploadSpeed = -1;
static int hf_ppvideo_MaxUploadSpeed = -1;
static int hf_ppvideo_RestPlayableTime = -1;
static int hf_ppvideo_LostRate = -1;
static int hf_ppvideo_RedundancyRate = -1;
static int hf_ppvideo_PeerInfoLength = -1;

static int hf_ppvideo_packet_type = -1;
static int hf_ppvideo_reversed = -1;
static int hf_ppvideo_protocal_version = -1;
static int hf_ppvideo_request_buffermap_piece_id = -1;
static int hf_ppvideo_upload_bandwidth = -1;
static int hf_ppvideo_request_announce_reserved = -1;
static int hf_ppvideo_piece_info_count = -1;
static int hf_ppvideo_header_piece_id = -1;
static int hf_ppvideo_interval = -1;
static int hf_ppvideo_start_piece_info_id = -1;
static int hf_ppvideo_subpiece_No = -1;
static int hf_ppvideo_start_piece_id = -1;
static int hf_ppvideo_buffer_map_length = -1;
static int hf_ppvideo_buffer_map = -1;
static int hf_ppvideo_piece_count = -1;
static int hf_ppvideo_live_priority = -1;

static int hf_ppvideo_SubPieceInfo_PieceID = -1;

static int hf_ppvideo_CandidatePeerInfoMine = -1;
static int hf_ppvideo_PeerVersionMine = -1;
static int hf_ppvideo_PeerTypeMine = -1;
static int hf_ppvideo_CandidatePeerInfoHis = -1;

static int hf_ppvideo_ResourceReportStructs = -1;
static int hf_ppvideo_ResourceReportStruct = -1;
static int hf_ppvideo_ResourceReportStruct_ResourceID = -1;
static int hf_ppvideo_ResourceReportStruct_Type = -1;
static int hf_ppvideo_NatType = -1;
static int hf_ppvideo_UploadPriority = -1;
static int hf_ppvideo_IdleTimeInMins = -1;
static int hf_ppvideo_Report_Reserved = -1;
static int hf_ppvideo_InternalTcpPort = -1;
static int hf_ppvideo_ReportResourceCount = -1;
static int hf_ppvideo_LocalResourceCount = -1;
static int hf_ppvideo_ServerResourceCount = -1;

static int hf_ppvideo_ContentMD5 = -1;
static int hf_ppvideo_ContentBytes = -1;
static int hf_ppvideo_Reserved = -1;

static int hf_ppvideo_DataLength = -1;
static int hf_ppvideo_Data = -1;

static int hf_ppvideo_TimeStamp = -1;
static int hf_ppvideo_PeerAction = -1;

static int hf_ppvideo_ResourcePeerCount = -1;

static int hf_ppvideo_IndexServerInfos = -1;
static int hf_ppvideo_IndexServerInfo = -1;
static int hf_ppvideo_IndexServerInfo_Length = -1;
static int hf_ppvideo_IndexServerInfo_IP = -1;
static int hf_ppvideo_IndexServerInfo_Port = -1;
static int hf_ppvideo_IndexServerInfo_Type = -1;
static int hf_ppvideo_IndexServerCount = -1;
static int hf_ppvideo_ModIndexMap = -1;

static int hf_ppvideo_StringCount = -1;
static int hf_ppvideo_StringArray = -1;
static int hf_ppvideo_StringLength = -1;
static int hf_ppvideo_StringData = -1;

static int hf_ppvideo_NeedReport = -1;
static int hf_ppvideo_IntervalTime = -1;
static int hf_ppvideo_DataCollectorInfo = -1;
static int hf_ppvideo_DataCollectorInfo_Length = -1;
static int hf_ppvideo_DataCollectorInfo_IP = -1;
static int hf_ppvideo_DataCollectorInfo_Port = -1;
static int hf_ppvideo_DataCollectorInfo_Type = -1;

static int hf_ppvideo_KernelVersion = -1;
static int hf_ppvideo_UploadPicProbability = -1;

static int hf_ppvideo_DataOnAppStop = -1;
static int hf_ppvideo_DataOnAppStop_MaxUploadSpeedInKBps = -1;
static int hf_ppvideo_DataOnAppStop_MaxDownloadSpeedInKBps = -1;
static int hf_ppvideo_DataOnAppStop_DiskUsedSizeInMB = -1;
static int hf_ppvideo_DataOnAppStop_UploadCacheHitRate = -1;
static int hf_ppvideo_DataOnAppStop_UploadDataBytesInMB = -1;
static int hf_ppvideo_DataOnAppStop_DownloadDataBytesInMB = -1;
static int hf_ppvideo_DataOnAppStop_DownloadDurationInSec = -1;
static int hf_ppvideo_DataOnAppStop_RunningDurationInMin = -1;
static int hf_ppvideo_DataOnAppStop_CpuPeakValue = -1;
static int hf_ppvideo_DataOnAppStop_MemPeakValue = -1;
static int hf_ppvideo_DataOnAppStop_IncomingPeersCount = -1;
static int hf_ppvideo_DataOnAppStop_FlvFolderPath_Length = -1;
static int hf_ppvideo_DataOnAppStop_FlvFolderPath = -1;

static int hf_ppvideo_DataOnDownloadStopEx = -1;
static int hf_ppvideo_DataOnDownloadStopEx_FlvUrl = -1;
static int hf_ppvideo_DataOnDownloadStopEx_FlvUrl_Length = -1;
static int hf_ppvideo_DataOnDownloadStopEx_FlvReferUrl = -1;
static int hf_ppvideo_DataOnDownloadStopEx_FlvReferUrl_Length = -1;
static int hf_ppvideo_DataOnDownloadStopEx_DataItemsCount = -1;
static int hf_ppvideo_DataOnDownloadStopEx_DataItemsSize = -1;
static int hf_ppvideo_DataOnDownloadStopEx_DataItems = -1;
static int hf_ppvideo_DataOnDownloadStopEx_DataItem = -1;
static int hf_ppvideo_DataOnDownloadStopEx_DataItem_Size = -1;
static int hf_ppvideo_DataOnDownloadStopEx_DataItem_Type = -1;
static int hf_ppvideo_DataOnDownloadStopEx_DataItem_Value_U8 = -1;
static int hf_ppvideo_DataOnDownloadStopEx_DataItem_Value_U16 = -1;
static int hf_ppvideo_DataOnDownloadStopEx_DataItem_Value_U32 = -1;
static int hf_ppvideo_DataOnDownloadStopEx_DataItem_Value_Default = -1;
//! PlayStop

// notify
static int hf_ppvideo_notify_TaskID = -1;
static int hf_ppvideo_notify_Duration = -1;
static int hf_ppvideo_notify_RestTime = -1;
static int hf_ppvideo_notify_TaskType = -1;
static int hf_ppvideo_notify_TaskContentLen = -1;
static int hf_ppvideo_notify_TaskContent = -1;
static int hf_ppvideo_notify_PeerOnline = -1;
static int hf_ppvideo_notify_TaskNum = -1;
static int hf_ppvideo_notify_JoinRet = -1;
static int hf_ppvideo_notify_NodeCount = -1;

static int hf_ppvideo_notify_NodeInfo = -1;
static int hf_ppvideo_notify_TaskInfo = -1;

static int hf_ppvideo_notify_TaskComplete = -1;

static int hf_ppvideo_ResourcePeerCountInfoCount = -1;
static int hf_ppvideo_ResourcePeerCountInfos = -1;
static int hf_ppvideo_ResourcePeerCountInfo = -1;
static int hf_ppvideo_ResourcePeerCountInfo_peer_count = -1;
static int hf_ppvideo_sub_piece_info = -1;
static int hf_ppvideo_MagicNumber = -1;
static int hf_ppvideo_CommandID = -1;
static int hf_ppvideo_InternalCommandReserve = -1;

// push
static int hf_ppvideo_used_disk_size = -1;
static int hf_ppvideo_upload_bandwidth_kbs = -1;
static int hf_ppvideo_avg_upload_speed_kbs = -1;
static int hf_ppvideo_Push_Wait_Interval_In_Sec = -1;
static int hf_ppvideo_PlayTasks = -1;
static int hf_ppvideo_PushTaskItem = -1;

// Data Type
enum DataCollectionType
{
    DATA_IP_POOL_SIZE,
    DATA_CONNECTED_PEERS_COUNT,
    DATA_FULL_PEERS_COUNT,
    DATA_IS_DOWNLOAD_FINISHED,
    DATA_AVG_DOWNLOAD_SPEED_IN_KPS,
    DATA_MAX_HTTP_DOWNLOAD_SPEED_IN_KPS,
    DATA_MAX_P2P_DOWNLOAD_SPEED_IN_KPS,
    DATA_AVG_HTTP_SPEED_IN_KPS,
    DATA_AVG_P2P_SPEED_IN_KPS,
    DATA_HTTP_DATA_BYTES,
    DATA_P2P_DATA_BYTES,
    DATA_PACKET_LOST_RATE,
    DATA_REDUNDANT_RATE,
    DATA_DOWNLOAD_STATUS,
    DATA_FLV_FILE_LENGTH,
    DATA_FLV_DATA_RATE,
    DATA_FLV_TIME_DURATION_IN_SECONDS,
    DATA_HAS_P2P,
    // size
    DATA_TYPE_COUNT,
};

static const value_string data_collection_types[] = 
{
    { DATA_IP_POOL_SIZE,                        "IpPoolSize" },
    { DATA_CONNECTED_PEERS_COUNT,               "ConnectedPeersCount" },
    { DATA_FULL_PEERS_COUNT,                    "FullPeersCount" },
    { DATA_IS_DOWNLOAD_FINISHED,                "IsDownloadFinished" },
    { DATA_AVG_DOWNLOAD_SPEED_IN_KPS,           "AverageDownloadSpeed (KBps)" },
    { DATA_MAX_HTTP_DOWNLOAD_SPEED_IN_KPS,      "MaxHttpDownloadSpeed (KBps)" },
    { DATA_MAX_P2P_DOWNLOAD_SPEED_IN_KPS,       "MaxP2PDownloadSPeed (KBps)" },
    { DATA_AVG_HTTP_SPEED_IN_KPS,               "AverageHttpSpeed (KBps)" },
    { DATA_AVG_P2P_SPEED_IN_KPS,                "AverageP2PSpeed (KBps)" },
    { DATA_HTTP_DATA_BYTES,                     "HttpDataBytes (Bytes)" },
    { DATA_P2P_DATA_BYTES,                      "P2PDataBytes (Bytes)" },
    { DATA_PACKET_LOST_RATE,                    "PacketLostRate (percent)" },
    { DATA_REDUNDANT_RATE,                      "RedundateRate (percent)" },
    { DATA_DOWNLOAD_STATUS,                     "DownloadStatus (0:Range,1:NonRange; 1:P2P,2:HTTP,3:P2P&HTTP)" },
    { DATA_FLV_FILE_LENGTH,                     "FlvFileLength (Bytes)" },
    { DATA_FLV_DATA_RATE,                       "FlvDataRate (Bps)" },
    { DATA_FLV_TIME_DURATION_IN_SECONDS,        "FlvTimeDuration (Seconds)" },
    { DATA_HAS_P2P,                             "HasP2P (Boolean)" },
};


// Report Type
#define REPORT_ADD              1
#define REPORT_DEL              0

static const value_string report_types[] = 
{
    { REPORT_ADD,              "Add" },
    { REPORT_DEL,              "Del" },
    { 0, NULL }
};

// HttpServer Type
#define TYPE_HTTP_SERVER        0
#define TYPE_FLASH_MEDIA_SERVER 1

static const value_string http_server_types[] = 
{
    { TYPE_HTTP_SERVER,        "Http Server"        },
    { TYPE_FLASH_MEDIA_SERVER, "Flash Media Server" },
    { 0, NULL }
};

// TrackerServer Type
#define TYPE_TCP_TRACKER  0
#define TYPE_UDP_TRACKER  1
#define TYPE_UDPSERVER_TRACKER  2

static const value_string tracker_info_types[] =
{
    { TYPE_TCP_TRACKER,  "Tcp Tracker" },
    { TYPE_UDP_TRACKER,  "Udp Tracker" },
	{ TYPE_UDPSERVER_TRACKER, "UdpServer Tracker" },
    { 0, NULL }
};

#define TYPE_TRACKER_FOR_LISTING 0
#define TYPE_lIVE_TRACKER_FOR_LISTING 1

static const value_string listing_tracker_info_types[] =
{
    { TYPE_TRACKER_FOR_LISTING,  "Vod Tracker For Listing" },
    { TYPE_lIVE_TRACKER_FOR_LISTING,  "Live Tracker For Listing" },
	{ 0, NULL }
};

// IndexServer Type
#define TYPE_TCP_INDEX_SERVER  0
#define TYPE_UDP_INDEX_SERVER  1

static const value_string index_server_info_types[] =
{
    { TYPE_TCP_INDEX_SERVER,  "Tcp Index Server" },
    { TYPE_UDP_INDEX_SERVER,  "Udp Index Server" },
    { 0, NULL }
};

// StunServer Type
#define TYPE_TCP_STUN_SERVER 0
#define TYPE_UDP_STUN_SERVER 1

static const value_string stun_server_info_types[] =
{
    { TYPE_TCP_STUN_SERVER,    "TCP Stun Server" },
    { TYPE_UDP_STUN_SERVER,    "UDP Stun Server" },
    { 0, NULL }
};

// DataCollector Type
#define TYPE_TCP_DATA_COLLECTOR 0
#define TYPE_UDP_DATA_COLLECTOR 1

static const value_string data_collector_info_types[] =
{
    { TYPE_TCP_DATA_COLLECTOR,     "TCP Data Collector" },
    { TYPE_UDP_DATA_COLLECTOR,     "UDP Data Collector" },
};

// Peer Error Code
#define E_PPV_EXCHANGE_NO_RESOURCEID      0x0011
#define E_PPV_EXCHANGE_NOT_DOWNLOADING    0x0012
#define E_PPV_CONNECT_NO_RESOURCEID       0x0021
#define E_PPV_CONNECT_CONNECITION_FULL    0x0022
#define E_PPV_ANNOUNCE_NO_RESOURCEID      0x0031
#define E_PPV_SUBPIECE_NO_RESOURCEID      0x0041
#define E_PPV_SUBPIECE_SUBPIECE_NOT_FOUND 0x0042
#define E_PPV_RIDINFO_NO_RESOURCEID		  0x0051

static const value_string peer_error_codes[] = 
{
    { E_PPV_EXCHANGE_NO_RESOURCEID,       "PPV_EXCHANGE_NO_RESOURCEID"      },
    { E_PPV_EXCHANGE_NOT_DOWNLOADING,     "PPV_EXCHANGE_NOT_DOWNLOADING"    },
    { E_PPV_CONNECT_NO_RESOURCEID,        "PPV_CONNECT_NO_RESOURCEID"       },
    { E_PPV_CONNECT_CONNECITION_FULL,     "PPV_CONNECT_CONNECITION_FULL"    },
    { E_PPV_ANNOUNCE_NO_RESOURCEID,       "PPV_ANNOUNCE_NO_RESOURCEID"      },
    { E_PPV_SUBPIECE_NO_RESOURCEID,       "PPV_SUBPIECE_NO_RESOURCEID"      },
    { E_PPV_SUBPIECE_SUBPIECE_NOT_FOUND,  "PPV_SUBPIECE_SUBPIECE_NOT_FOUND" },
	{ E_PPV_RIDINFO_NO_RESOURCEID,  	  "PPV_RIDINFO_NO_RESOURCEID" },
    { 0, NULL }
};

// Nat Type
#define NAT_ERROR                 0xFF
#define NAT_FULL_CONE             0x00
#define NAT_IP_RESTRICTED         0x01
#define NAT_IP_PORT_RESTRICTED    0x02
#define NAT_SYMMETRIC             0x03
#define NAT_PUBLIC                0x04

static const value_string nat_types[] = 
{
    { NAT_ERROR,                "NAT_ERROR"             },
    { NAT_FULL_CONE,            "NAT_FULL_CONE"         },
    { NAT_IP_RESTRICTED,        "NAT_IP_RESTRICTED"     },
    { NAT_IP_PORT_RESTRICTED,   "NAT_IP_PORT_RESTRICTED"},
    { NAT_SYMMETRIC,            "NAT_SYMMETRIC"         },
    { NAT_PUBLIC,               "NAT_PUBLIC"            },
};

// action name
//    Index
#define AID_QUERY_HTTP_SERVER_BY_RID           0x11
#define AID_QUERY_RID_BY_URL                   0x12
#define AID_ADD_RID_URL                        0x13
#define AID_QUERY_TRACKER_LIST                 0x14
#define AID_QUERY_STUN_SERVER_LIST             0x15
#define AID_QUERY_RID_BY_CONTENT               0x16 // PeerVersion >= 4
#define AID_QUERY_INDEX_SERVER_LIST            0x17 // PeerVersion >= 5
#define AID_QUERY_TESTURL_LIST                 0x18
#define AID_QUERY_KEYWORD_LIST                 0x19
#define AID_QUERY_PUSH_TASK					   0x1A
#define AID_REPORT_PUSH_TASK_COMPLETED		   0X1B
#define AID_QUERY_PUSH_TASK_V2				   0x1C
#define AID_QUERY_TRACKER_FOR_LISTING          0x40
#define AID_QUERY_NEED_REPORT                  0x20 // PeerVersion >= 6
#define AID_QUERY_DATA_COLLECTIOR              0x21
#define AID_QUERY_UPLOAD_PIC_PROBABILITY       0x22
#define AID_QUERY_SERVER_LIST			       0x23
#define AID_QUERY_CACHE_URL_BY_RID			   0x29 //冲突冲突，所以改成了29
#define AID_QUERY_NOTIFY_SERVER_LIST           0x24
#define AID_QUERY_CONFIG_STRING				   0x25
#define AID_QUERY_LIVE_TRACKER_LIST			   0x26
#define AID_QUERY_SN_LIST					   0x27
#define AID_QUERY_VIP_SN_LIST				   0x28

//    Tracker
#define AID_LIST                               0x31
#define AID_COMMIT                             0x32
#define AID_KEEP_ALIVE                         0x33
#define AID_LEAVE                              0x34
#define AID_REPORT                             0x35
#define AID_QUERY_PEER_COUNT                   0x36 // PeerVersion >= 4
#define AID_QUERY_BATCH_PEER_COUNT             0x37
#define AID_INTERNAL_COMMAND                   0x38
#define AID_QUERY_PEER_RESOURCE                0x39
#define AID_LIST_TCP						   0x3B
#define AID_LIST_WITH_IP					   0x3C
#define AID_LIST_TCP_WITH_IP_PACKET			   0x3D

//    Peer
#define AID_ERROR                              0x51
#define AID_CONNECT                            0x52
#define AID_REQUEST_ANNOUCE                    0x53
#define AID_ANNOUCE                            0x54
#define AID_REQUEST_SUB_PIECE_OLD			   0x55
#define AID_REQUEST_SUB_PIECE				   0x5B
#define AID_SUB_PIECE                          0x56
#define AID_PEER_EXCHANGE                      0x57
#define RID_INFO_REQUEST					   0x58
#define RID_INFO_RESPONSE					   0x59
#define REPORT_SPEED						   0x5A
#define CLOSS_SN_SESSION				       0x5C
#define REQUEST_SUBPIECE_FROM_SN     	       0x5D
//    Live Peer
#define AID_LIVE_REQUEST_ANNOUCE               0xC0
#define AID_LIVE_ANNOUCE               		   0xC1
#define AID_LIVE_REQUEST_SUB_PIECE             0xC2
#define AID_LIVE_SUB_PIECE                     0xC3
#define AID_PEER_INFO                          0xC4

//    Stun
#define AID_STUN_HANDSHAKE                     0x71
#define AID_STUN_KEEP_ALIVE                    0x72
#define AID_STUN_INVOKE                        0x73
//    Notify
#define AID_NOTIFY_JOIN_REQUEST                0xA0
#define AID_NOTIFY_JOIN_RESPONSE               0xA1
#define AID_NOTIFY_KEEPALIVE_REQUEST           0xA2
#define AID_NOTIFY_KEEPALIVE_RESPONSE          0xA3
#define AID_NOTIFY_TASK_REQUEST                0xA4
#define AID_NOTIFY_TASK_RESPONSE               0xA5
#define AID_NOTIFY_LEAVE                       0xA6
//    Statistic
#define AID_STATISTIC_INDEX_PEER               0xD1
#define AID_STATISTIC_INDEX_URL                0xD2
#define AID_STATISTIC_INDEX_CONTENT            0xD3
#define AID_STATISTIC_INDEX_RESOURCE           0xD4
#define AID_STATISTIC_STRING				   0xD5
//    DataCollection
#define AID_DATA_ON_APP_STOP                   0xB1
#define AID_DATA_ON_DOWNLOAD_STOP              0xB2
#define AID_DATA_ON_DOWNLOAD_STOP_EX           0xB3

//    Peer_tcp
#define AID_REQUEST_ANNOUCE_TCP                    0xB0
#define AID_ANNOUCE_TCP                            0xB1
#define AID_REQUEST_SUB_PIECE_TCP				   0xB2
#define AID_SUB_PIECE_TCP                          0xB3
#define REPORT_SPEED_TCP						   0xB4

static const value_string ppvideo_tcp_protocol_names[] =
{
	  {AID_REQUEST_ANNOUCE_TCP,         "AnnouceRequest"},
	  {AID_ANNOUCE_TCP,                 "AnnounceResponse"},
	  {AID_REQUEST_SUB_PIECE_TCP,	     "RequestSubPiecePacket"},
	  {AID_SUB_PIECE_TCP ,              "SubPiecePacket"},
	  {REPORT_SPEED_TCP,                "ReportSpeedPacket"},
	  
	  { 0, NULL },
};
static const value_string ppvideo_protocol_names[] =
{
    // index
    { AID_QUERY_HTTP_SERVER_BY_RID,  "QueryHttpServerByRID" },
    { AID_QUERY_RID_BY_URL,          "QueryRidByUrl"        },
    { AID_ADD_RID_URL,               "AddRidUrl"            },
    { AID_QUERY_TRACKER_LIST,        "QueryTrackerList"     },
	{ AID_QUERY_LIVE_TRACKER_LIST,   "QueryLiveTrackerList" },
	{ AID_QUERY_SN_LIST,   			 "QuerySNList"          },
	{ AID_QUERY_VIP_SN_LIST,         "QueryVipSNList"       },
    { AID_QUERY_STUN_SERVER_LIST,    "QueryStunServerList"  },
    { AID_QUERY_RID_BY_CONTENT,      "QueryRidByContent"    }, // PeerVersion >= 4
    { AID_QUERY_INDEX_SERVER_LIST,   "QueryIndexServerList" }, // PeerVersion >= 5
    { AID_QUERY_TESTURL_LIST,        "QueryTestUrlList"     },
    { AID_QUERY_KEYWORD_LIST,        "QueryKeyWordList"     },
	{ AID_QUERY_PUSH_TASK,        	 "QueryPushTask"     	},
	{ AID_QUERY_PUSH_TASK_V2,        "QueryPushTaskV2"     	},
	{ AID_QUERY_TRACKER_FOR_LISTING, "QueryTrackerForListing"},
	{ AID_REPORT_PUSH_TASK_COMPLETED, "ReportPushTaskCompleted"},
    { AID_QUERY_NEED_REPORT,         "QueryNeedReport"      }, // PeerVersion >= 6
    { AID_QUERY_DATA_COLLECTIOR,     "QueryDataCollector"   },
    { AID_QUERY_UPLOAD_PIC_PROBABILITY, "QueryUploadPicProbabilty" },
	{ AID_QUERY_SERVER_LIST, 		 "QueryServerList" 		},
	{ AID_QUERY_NOTIFY_SERVER_LIST,   "QueryNotifyList" 		},
	{ AID_QUERY_CONFIG_STRING,      "QueryConfigString" }, 
    // tracker
    { AID_LIST,                      "List" },
	{ AID_LIST_TCP,                  "ListTCP" },
    { AID_LIST_WITH_IP,              "ListWithIp" },
	{ AID_LIST_TCP_WITH_IP_PACKET,   "ListTCPWithIp" },
    { AID_COMMIT,                    "Commit" },
    { AID_KEEP_ALIVE,                "KeepAlive" },
    { AID_LEAVE,                     "Leave" },
    { AID_REPORT,                    "Report" },
    { AID_QUERY_PEER_COUNT,          "QueryPeerCount" },
	{ AID_QUERY_BATCH_PEER_COUNT,	 "QueryBatchPeerCount"},
	{ AID_INTERNAL_COMMAND,	 		 "InternalCommand"},
	{ AID_QUERY_PEER_RESOURCE,	     "QueryPeerResource"},
    // peer
    { AID_ERROR,                     "Error" },
    { AID_CONNECT,                   "Connect" },
    { AID_REQUEST_ANNOUCE,           "RequestAnnouce" },
    { AID_ANNOUCE,                   "Announce"},
	{ AID_REQUEST_SUB_PIECE_OLD,     "RequestSubPieceOld" },
    { AID_REQUEST_SUB_PIECE,         "RequestSubPiece" },
    { AID_SUB_PIECE,                 "SubPiece" },
    { AID_PEER_EXCHANGE,             "PeerExchange" },
	{ RID_INFO_REQUEST,              "RidInfoRequest" },
	{ RID_INFO_RESPONSE,             "RidInfoResponse" },
	{ REPORT_SPEED,                  "ReportSpeed" },
	{ CLOSS_SN_SESSION,				 "CloseSNSession" },
	{ REQUEST_SUBPIECE_FROM_SN,      "RequestSubPieceFromSN"},
	// live peer
	{ AID_LIVE_REQUEST_ANNOUCE,		 "LiveRequestAnnouce" },
    { AID_LIVE_ANNOUCE,              "LiveAnnounce" },
    { AID_LIVE_REQUEST_SUB_PIECE,    "LiveRequestSubPiece" },
    { AID_LIVE_SUB_PIECE,            "LiveSubPiece"},
    { AID_PEER_INFO,                 "PeerInfo"},
    // stun
    { AID_STUN_HANDSHAKE,            "StunHandShake" },
    { AID_STUN_KEEP_ALIVE,           "StunKeepAlive" },
    { AID_STUN_INVOKE,               "StunInvoke" },
    // Statistic                                                   PeerVersion >= 4
    { AID_STATISTIC_INDEX_PEER,      "StatisticIndexPeer" }, 
    { AID_STATISTIC_INDEX_URL,       "StatisticIndexUrl" },
    { AID_STATISTIC_INDEX_RESOURCE,  "StatisticIndexResource" },
    { AID_STATISTIC_INDEX_CONTENT,   "StatisticIndexContent" },
	{ AID_STATISTIC_STRING,   		 "StatisticString" },
    // Data Collection                                             PeerVersion >= 6
    { AID_DATA_ON_APP_STOP,          "DataCollectionOnAppStop" },
    { AID_DATA_ON_DOWNLOAD_STOP,     "DataCollectionOnDownloadStop" },
    { AID_DATA_ON_DOWNLOAD_STOP_EX,  "DataCollectionOnDownloadStopEx" }, 
    // notify
    { AID_NOTIFY_JOIN_REQUEST,        "NotifyJoinRequest" },
    { AID_NOTIFY_JOIN_RESPONSE,       "NotifyJoinResponse" },
    { AID_NOTIFY_KEEPALIVE_REQUEST,   "NotifyKeepAliveRequest" }, 
    { AID_NOTIFY_KEEPALIVE_RESPONSE,  "NotifyKeepAliveResponse" },
    { AID_NOTIFY_TASK_REQUEST,        "NotifyTaskRequest" },
    { AID_NOTIFY_TASK_RESPONSE,       "NotifyTaskResponse" }, 
    { AID_NOTIFY_LEAVE,               "NotifyLeave" },
    // END
    { 0, NULL },
};

// Peer Action
#define P_AID_PEER_ONLINE                  0x11
#define P_AID_PEER_QUERY_URL               0x12
#define P_AID_PEER_QUERY_CONTENT           0x13
#define P_AID_PEER_QUERY_URL_SUCCESS       0x21
#define P_AID_PEER_QUERY_CONTENT_SUCCESS   0x22
#define P_AID_PEER_ADD_RESOURCE            0x23

static const value_string peer_action_names[] =
{
    { P_AID_PEER_ONLINE,                "PEER_ONLINE" },
    { P_AID_PEER_QUERY_URL,             "PEER_QUERY_URL" },
    { P_AID_PEER_QUERY_CONTENT,         "PEER_QUERY_CONTENT" },
    { P_AID_PEER_QUERY_URL_SUCCESS,     "PEER_QUERY_URL_SUCCESS" },
    { P_AID_PEER_QUERY_CONTENT_SUCCESS, "PEER_QUERY_CONTENT_SUCCESS" },
    { P_AID_PEER_ADD_RESOURCE,          "PEER_ADD_RESOURCE" },
    // END
    { 0, NULL }
};

// kernel version
typedef struct KernelVersionTag 
{
    guint8  Major;
    guint8  Minor;
    guint8  Micro;
    guint8  Extra;
} KernelVersion;

// BasicInfo
#define BASIC_INFO_RESPONSE_FLAG 0x01
#define BASIC_INFO_RESERVED_FLAG 0xFE

// protocol fields
static hf_register_info hf_ppvideo[] =
{
	{
        &hf_ppvideo_used_disk_size,
        {
            "Used Disk Size         ", "ppvideo.useddisksize",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_upload_bandwidth_kbs,
        {
            "Upload Bandwidth kbs   ", "ppvideo.uploadbandwidthkbs",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_avg_upload_speed_kbs,
        {
            "Avg Upload Speed kbs   ", "ppvideo.avguploadspeedkbs",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_Push_Wait_Interval_In_Sec,
        {
            "Push Wait Interval(sec)", "ppvideo.pushwaitintervalinsec",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_PlayTasks,
        {
            "Push Tasks             ", "ppvideo.push.pushtasks",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_PushTaskItem,
        {
            "Push Task Item         ", "ppvideo.push.pushtask",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_HttpServers,
        {
            "Http Servers           ", "ppvideo.httpservers",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_HttpServer,
        {
            "Http Server           ", "ppvideo.httpserver",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_HttpServer_Length,
        {
            "Length                ", "ppvideo.httpserver.length",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_HttpServer_Type,
        {
            "Type                  ", "ppvideo.httpserver.type",
            FT_UINT8, BASE_DEC,
            VALS(http_server_types), 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_HttpServer_UrlLength,
        {
            "Url Length            ", "ppvideo.httpserver.urllength",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_HttpServer_UrlString,
        {
            "Url                   ", "ppvideo.httpserver.url",
            FT_STRING, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_HttpServer_ReferLength,
        {
            "Refer Length          ", "ppvideo.httpserver.referlength",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_HttpServer_ReferString,
        {
            "Refer Url             ", "ppvideo.httpserver.refer",
            FT_STRING, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
	
	{
        &hf_ppvideo_ServerTypeInfos,
        {
            "Server Type Infos     ", "ppvideo.servertypeinfos",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_ServerTypeInfo,
        {
            "Server Type Info      ", "ppvideo.servertypeinfo",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ServerTypeCount,
        {
            "Server Type Count     ", "ppvideo.servertypeinfo.count",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ServerTypeInfo_Connect_Type,
        {
            "Connect Type          ", "ppvideo.servertypeinfo.connecttype",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ServerTypeInfo_Server_Type,
        {
            "Server Type           ", "ppvideo.servertypeinfo.servertype",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ServerTypeInfo_Level,
        {
            "Lever                 ", "ppvideo.servertypeinfo.level",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	
	{
        &hf_ppvideo_ServerListCount,
        {
            "Server List Count     ", "ppvideo.serverlist.serverlistcount",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_ServerLists,
        {
            "Server Lists          ", "ppvideo.serverlists",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_ServerList,
        {
            "Server List           ", "ppvideo.serverlist",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_ServerData_Count,
        {
            "Server Data Count     ", "ppvideo.serverlist.count",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_ServerList_Padding_Info_Count,
        {
            "Padding Info Count    ", "ppvideo.serverlist.paddinginfocount",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_ServerList_Padding_Info,
        {
            "Padding Info          ", "ppvideo.serverlist.paddinginfo",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_ServerDatas,
        {
            "Server Datas          ", "ppvideo.serverdatas",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_ServerData,
        {
            "Server Data           ", "ppvideo.serverdata",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_ServerData_IP,
        {
            "IP                    ", "ppvideo.serverdata.ip",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_ServerData_Port,
        {
            "Port                  ", "ppvideo.serverdata.port",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_ServerData_ModID,
        {
            "ModID                 ", "ppvideo.serverdata.modid",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_ServerData_ConnectType,
        {
            "Connect Type          ", "ppvideo.serverdata.connecttype",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	
	{
        &hf_ppvideo_UrlInfo,
        {
            "Http Server           ", "ppvideo.urlinfo",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_UrlInfo_Length,
        {
            "Length                ", "ppvideo.urlinfo.length",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_UrlInfo_Type,
        {
            "Type                  ", "ppvideo.urlinfo.type",
            FT_UINT8, BASE_DEC,
            VALS(http_server_types), 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_UrlInfo_UrlLength,
        {
            "Url Length            ", "ppvideo.urlinfo.urllength",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_UrlInfo_UrlString,
        {
            "Url                   ", "ppvideo.urlinfo.url",
            FT_STRING, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_UrlInfo_ReferLength,
        {
            "Refer Length          ", "ppvideo.urlinfo.referlength",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_UrlInfo_ReferString,
        {
            "Refer Url             ", "ppvideo.urlinfo.refer",
            FT_STRING, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },

	{
        &hf_ppvideo_RegionInfo,
        {
            "Region Info           ", "ppvideo.regioninfo",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_RegionInfo_PublicIP,
        {
            "Public Ip             ", "ppvideo.regioninfo.publicip",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_RegionInfo_Rigion_Code,
        {
            "Region Code           ", "ppvideo.regioninfo.regioncode",
            FT_UINT32, BASE_DEC,
            VALS(http_server_types), 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_RegionInfo_Version,
        {
            "Version               ", "ppvideo.regioninfo.version",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	
    {
        &hf_ppvideo_ResourceReportStructs,
        {
            "ResReport Structs     ", "ppvideo.report.structs",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ResourceReportStruct,
        {
            "ResReport Struct      ", "ppvideo.report.struct",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ResourceReportStruct_ResourceID,
        {
            "ResReport Struct RID  ", "ppvideo.report.struct.rid",
            FT_GUID, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ResourceReportStruct_Type,
        {
            "ResReport Struct Type ", "ppvideo.report.struct.type",
            FT_UINT8, BASE_DEC,
            VALS(report_types), 0x0, NULL, HFILL
        }
    },

    {
        &hf_ppvideo_ResourceInfos,
        {
            "Resource Infos        ", "ppvideo.resourceinfos",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ResourceInfo,
        {
            "Resource Info         ", "ppvideo.resourceinfo",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ResourceInfo_Length,
        {
            "Length                ", "ppvideo.resourceinfo.length",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ResourceInfo_ResourceID,
        {
            "ResourceID            ", "ppvideo.resourceinfo.resourceid",
            FT_GUID, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ResourceInfo_FileLength,
        {
            "FileLength            ", "ppvideo.resourceinfo.filelength",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ResourceInfo_BlockCount,
        {
            "BlockCount            ", "ppvideo.resourceinfo.blockcount",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ResourceInfo_BlockSize,
        {
            "BlockSize             ", "ppvideo.resourceinfo.blocksize",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_PlayHistoryItems,
        {
            "Play History Items   ", "ppvideo.playhistoryitems",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_PlayHistoryItem,
        {
            "Play History Item    ", "ppvideo.playhistoryitem",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_PushTaskParam,
        {
            "Push Task Param      ", "ppvideo.pushtaskparam",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_PushTaskParam_Task_Type,
        {
            "Task Type            ", "ppvideo.pushtaskparam.tasktype",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_PushTaskParam_Protect_Time_Interval_In_Seconds,
        {
            "Protect Time Interval in Seconds", "ppvideo.pushtaskparam.protecttimeintervalinseconds",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_PushTaskParam_Min_Download_Speed_In_KBps,
        {
            "Min Download Speed in Kbps", "ppvideo.pushtaskparam.mindowmloadspeedinkbps",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_PushTaskParam_Max_Download_Speed_In_KBps_When_Idle,
        {
            "Max Download Speed in Kbps When Idle", "ppvideo.pushtaskparam.maxdownloadspeedinkbpswhenidel",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_PushTaskParam_Max_Download_Speed_In_KBps_When_Normal,
        {
            "Max Download Speed in Kbps When Normal", "ppvideo.pushtaskparam.maxdownloadspeedinkbpswhennormal",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_PushTaskParam_Bandwidth_Ratio_When_Idle,
        {
            "Bandwith Ratio When Idle", "ppvideo.pushtaskparam.bandwithratiowhenidle",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_PushTaskParam_Bandwidth_Ratio_When_Normal,
        {
            "Bandwith Ratio When Normal", "ppvideo.pushtaskparam.bandwithratiowhennormal",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	
    {
        &hf_ppvideo_ResourceInfo_BlockMD5s,
        {
            "All Block MD5s        ", "ppvideo.resourceinfo.blockmd5s",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ResourceInfo_BlockMD5,
        {
            "BlockMD5              ", "ppvideo.resourceinfo.blockmd5",
            FT_GUID, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ResourceInfo_HttpServerCount,
        {
            "Http Server Count     ", "ppvideo.resourceinfo.httpservercount",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },

    {
        &hf_ppvideo_RidInfo,
        {
            "Rid      Info         ", "ppvideo.ridinfo",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
	
    {
        &hf_ppvideo_TrackerInfos,
        {
            "Tracker Infos         ", "ppvideo.trackerinfos",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_TrackerInfo,
        {
            "Tracker Info          ", "ppvideo.trackerinfo",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_TrackerInfo_Length,
        {
            "Length                ", "ppvideo.trackerinfo.length",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_TrackerInfo_ModNo,
        {
            "Mod Number            ", "ppvideo.trackerinfo.modno",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_TrackerInfo_IP,
        {
            "IP                    ", "ppvideo.trackerinfo.ip",
            FT_IPv4, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_TrackerInfo_Port,
        {
            "Port                  ", "ppvideo.trackerinfo.port",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_TrackerInfo_Type,
        {
            "Type                  ", "ppvideo.trackerinfo.type",
            FT_UINT8, BASE_DEC,
            VALS(tracker_info_types), 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_TrackerInfo_StationNo,
        {
            "Station Number        ", "ppvideo.trackerinfo.stationno",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_TrackerInfo_Reserve,
        {
            "Reserve               ", "ppvideo.trackerinfo.reserve",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_TrackerType,
        {
            "Tracker Type          ", "ppvideo.trackerinfo.trackertype",
            FT_UINT8, BASE_DEC,
            VALS(listing_tracker_info_types), 0x0, NULL, HFILL
        }
    },


    // Stun Server
    {
        &hf_ppvideo_StunServerInfos,
        {
            "StunServers           ", "ppvideo.stunserverinfos",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_StunServerInfo,
        {
            "StunServer            ", "ppvideo.stunserverinfo",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_StunServerInfo_Length,
        {
            "Length                ", "ppvideo.stunserverinfo.length",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_StunServerInfo_IP,
        {
            "IP                    ", "ppvideo.stunserverinfo.ip",
            FT_IPv4, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_StunServerInfo_Port,
        {
            "Port                  ", "ppvideo.stunserverinfo.port",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_StunServerInfo_Type,
        {
            "Type                  ", "ppvideo.stunserverinfo.type",
            FT_UINT8, BASE_DEC,
            VALS(stun_server_info_types), 0x0, NULL, HFILL
        }
    },

	// sn
	{
        &hf_ppvideo_SNInfos,
        {
            "Super Node Servers    ", "ppvideo.supernodeinfos",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_SNServerInfo,
        {
            "SuperNode Server      ", "ppvideo.supernodeserverinfo",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_SNServerInfo_IP,
        {
            "IP                    ", "ppvideo.snserverinfo.ip",
            FT_IPv4, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_SNServerInfo_Port,
        {
            "Port                  ", "ppvideo.snserverinfo.port",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_SNServerInfo_Priority,
        {
            "Priority              ", "ppvideo.snserverinfo.priority",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	
    {
        &hf_ppvideo_CandidatePeerInfos,
        {
            "All Candiate Peers    ", "ppvideo.peerinfos",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_CandidatePeerInfo,
        {
            "Candidate Peer Info   ", "ppvideo.peerinfo",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_CandidatePeerInfo_IP,
        {
            "IP                    ", "ppvideo.peerinfo.ip",
            FT_IPv4, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_CandidatePeerInfo_UdpPort,
        {
            "UDP Port              ", "ppvideo.peerinfo.udpport",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_CandidatePeerInfo_PeerVersion,
        {
            "Peer Version          ", "ppvideo.peerinfo.peerversion",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_CandidatePeerInfo_DetectedIP,
        {
            "Detected IP           ", "ppvideo.peerinfo.detectedip",
            FT_IPv4, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_CandidatePeerInfo_DetectedPort,
        {
            "Detected Port         ", "ppvideo.peerinfo.detectedport",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_CandidatePeerInfo_StunIP,
        {
            "Stun IP               ", "ppvideo.peerinfo.stunip",
            FT_IPv4, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_CandidatePeerInfo_StunUdpPort,
        {
            "Stun UDP Port         ", "ppvideo.peerinfo.stunport",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_CandidatePeerInfo_PeerNatType,
        {
            "Peer Nat Type         ", "ppvideo.peerinfo.peernattype",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_CandidatePeerInfo_UploadPriority,
        {
            "Upload Priority       ", "ppvideo.peerinfo.uploadpriority",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_CandidatePeerInfo_IdleTimeInMins,
        {
            "Idle Time In Mins     ", "ppvideo.peerinfo.idletimeinmins",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_CandidatePeerInfo_TrackerPriority,
        {
            "Tracker Priority      ", "ppvideo.peerinfo.trackerpriority",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },


    {
        &hf_ppvideo_FileInfo,
        {
            "File Info             ", "ppvideo.fileinfo",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_FileInfo_ResourceID,
        {
            "Resource ID           ", "ppvideo.fileinfo.resourceid",
            FT_GUID, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_FileInfo_PeerCount,
        {
            "Peer Count            ", "ppvideo.fileinfo.peercount",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },


    {
        &hf_ppvideo_PeerDownloadInfo,
        {
            "Peer Download Info    ", "ppvideo.peerdownloadinfo",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_PeerDownloadInfo_IsDownloading,
        {
            "Is Downloading        ", "ppvideo.peerdownloadinfo.isdownloading",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_PeerDownloadInfo_OnlineTime,
        {
            "Online Time           ", "ppvideo.peerdownloadinfo.onlinetime",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_PeerDownloadInfo_AvgDownloadSpeed,
        {
            "Avg Download Speed    ", "ppvideo.peerdownloadinfo.avgdownloadspeed",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_PeerDownloadInfo_NowDownloadSpeed,
        {
            "Now Download Speed    ", "ppvideo.peerdownloadinfo.nowdownloadspeed",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_PeerDownloadInfo_AvgUploadSpeed,
        {
            "Avg Upload Speed      ", "ppvideo.peerdownloadinfo.avguploadspeed",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_PeerDownloadInfo_NowUploadSpeed,
        {
            "Now Upload Speed      ", "ppvideo.peerdownloadinfo.nowuploadspeed",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_PeerDownloadInfo_Reserved1,
        {
            "Reserved1             ", "ppvideo.peerdownloadinfo.reserved1",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_PeerDownloadInfo_Reserved2,
        {
            "Reserved2             ", "ppvideo.peerdownloadinfo.reserved2",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },


    {
        &hf_ppvideo_SubPieceInfos,
        {
            "All SubPiece Infos        ", "ppvideo.subpieceinfos",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_SubPieceInfo,
        {
            "Sub Piece Info        ", "ppvideo.subpieceinfo",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_LiveBufferMap,
        {
            "Live Buffer Map       ", "ppvideo.livebuffermap",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_SubPieceInfo_BlockIndex,
        {
            "Block Index           ", "ppvideo.subpieceinfo.blockindex",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_SubPieceInfo_SubPieceIndex,
        {
            "SubPiece Index       ", "ppvideo.subpieceinfo.subpieceindex",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },


    {
        &hf_ppvideo_BasicInfo,
        {
            "Basic Info            ", "ppvideo.basicinfo",
            FT_UINT8, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_BasicInfo_IsResponse,
        {
            "IsResponse            ", "ppvideo.basicinfo.isresponse",
            FT_BOOLEAN, 8,
            NULL, BASIC_INFO_RESPONSE_FLAG,
            NULL, HFILL
        }
    },
    {
        &hf_ppvideo_BasicInfo_Reserved,
        {
            "Reserved              ", "ppvideo.basicinfo.reserved",
            FT_BOOLEAN, 8,
            NULL, BASIC_INFO_RESERVED_FLAG,
            NULL, HFILL
        }
    },
	
	{
        &hf_ppvideo_File_Length,
        {
            "File Length           ", "ppvideo.filelength",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_Block_Count,
        {
            "Block Count           ", "ppvideo.blockcount",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_Block_Count_16,
        {
            "Block Count           ", "ppvideo.blockcount16",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_Block_Size,
        {
            "Block_Size            ", "ppvideo.blocksize",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_Block_MD5s,
        {
            "Block MD5s            ", "ppvideo.blockmd5s",
            //FT_GUID, BASE_NONE,
			FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_Connected_Peers_Count,
        {
            "Connected Peers Count ", "ppvideo.connectedpeerscount",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_Active_Peers_Count,
        {
            "Active Peers Count    ", "ppvideo.activepeerscount",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_Pooled_Peers_Count,
        {
            "Pooled Peers Count    ", "ppvideo.pooledpeerscount",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	
	{
        &hf_ppvideo_Report_Speed,
        {
            "Report Speed          ", "ppvideo.reportspeed",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },

    {
        &hf_ppvideo_CheckSum,
        {
            "Check Sum             ", "ppvideo.checksum",
            FT_UINT32, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_NewCheckSum,
        {
            "New Check Sum         ", "ppvideo.newchecksum",
            FT_UINT32, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_Action,
        {
            "Action                ", "ppvideo.action",
            FT_UINT8, BASE_HEX,
            VALS(ppvideo_protocol_names), 0x0, NULL, HFILL
        }
    },
    {
    	  &hf_ppvideo_Tcp_Action,
    	  {
    	  	  "Action                ","ppvideo_tcp.action",
    	  	  FT_UINT8,BASE_HEX,
    	  	  VALS(ppvideo_tcp_protocol_names),0x0,NULL,HFILL
    	  }
    },
    {
        &hf_ppvideo_TransactionID,
        {
            "Transaction ID        ", "ppvideo.transactionid",
            FT_UINT32, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_IsRequest,
        {
            "IsRequest             ", "ppvideo.isrequest",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_PeerVersion,
        {
            "Peer Version          ", "ppvideo.peerversion",
            FT_UINT16, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_Reserve,
        {
            "Reserve               ", "ppvideo.reserve",
            FT_UINT16, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ErrorCode,
        {
            "Error Code            ", "ppvideo.errorcode",
            FT_UINT8, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ResourceID,
        {
            "Resource ID           ", "ppvideo.resourceid",
            FT_GUID, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_HttpServerCount,
        {
            "Http Server Count     ", "ppvideo.httpservercount",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_SessionID,
        {
            "Session ID            ", "ppvideo.sessionid",
            FT_UINT32, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_UrlLength,
        {
            "Url Length            ", "ppvideo.urllength",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ReferLength,
        {
            "Refer Length          ", "ppvideo.referlength",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_DataLength,
        {
            "Data Length           ", "ppvideo.datalength",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_Data,
        {
            "Data                  ", "ppvideo.data",
            FT_STRING, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_UrlString,
        {
            "Url                   ", "ppvideo.url",
            FT_STRING, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_ConfigString,
        {
            "Config String         ", "ppvideo.configstring",
            FT_STRING, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ReferString,
        {
            "Refer Url             ", "ppvideo.referurl",
            FT_STRING, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DetectedIP,
        {
            "Detected IP           ", "ppvideo.detectedip",
            FT_IPv4, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DetectedUdpPort,
        {
            "Detected UDP Port     ", "ppvideo.detectedudpport",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_FileLength,
        {
            "File Length           ", "ppvideo.filelength",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_BlockCount,
        {
            "Block Count           ", "ppvideo.blockcount",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_BlockSize,
        {
            "Block Size            ", "ppvideo.blocksize",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_BlockMD5s,
        {
            "All Block MD5         ", "ppvideo.blockmd5s",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_BlockMD5,
        {
            "Block MD5            ", "ppvideo.blockmd5",
            FT_GUID, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_PeerGuid,
        {
            "Peer GUID             ", "ppvideo.peerguid",
            FT_GUID, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_Status,
        {
            "Status                ", "ppvideo.status",
            FT_UINT8, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_TrackerGroupCount,
        {
            "Tracker Group Count   ", "ppvideo.trackergroupcount",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_TrackerCount,
        {
            "Tracker Count         ", "ppvideo.trackercount",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_StunServerCount,
        {
            "StunServer Count      ", "ppvideo.stunservercount",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_SNCount,
        {
            "SN Count              ", "ppvideo.sncount",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
   },
	{
        &hf_ppvideo_ConfigStringLength,
        {
            "Config String Length  ", "ppvideo.configstringlength",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_UsedDiskSize,
        {
            "Used Disk Size        ", "ppvideo.push.useddisksize",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_UploadBandWidthKBs,
        {
            "Upload Bandwidth KBs  ", "ppvideo.push.uploadBandwidthkbs",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_AvgUploadSpeedKBs,
        {
            "Avg Upload Speed KBs  ", "ppvideo.push.avguploadspeedkbs",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_PlayHistoryItemCount,
        {
            "Play History Item Count", "ppvideo.push.playhistoryitemcount",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_VideoNameLength,
        {
            "Video Name Length     ", "ppvideo.push.videonamelength",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_ContinuouslyPlayedDuration,
        {
            "Continuously Play Dura", "ppvideo.push.continuouslyplayedduration",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_DownloadedSegmentNum,
        {
            "Downloaded Segment Num", "ppvideo.push.downloadedsegmentnum",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_ChannelIDLength,
        {
            "Channel ID Length     ", "ppvideo.push.channelidlength",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_PushWaitIntervalInSeconds,
        {
            "Push Wait Interval(s) ", "ppvideo.push.pushwaitintervalinseconds",
            FT_INT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_PushTaskItemCount,
        {
            "Push Task Item Count  ", "ppvideo.push.pushtaskitemcount",
            FT_INT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_VideoName,
        {
            "Video Name            ", "ppvideo.push.videoname",
            FT_STRING, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_ChannelID,
        {
            "Channel ID            ", "ppvideo.push.channelid",
            FT_STRING, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_NotifyServerCount,
        {
            "NotifyServer Count      ", "ppvideo.notifyservercount",
                FT_UINT16, BASE_DEC,
                NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_RequestPeerCount,
        {
            "Request Peer Count    ", "ppvideo.requestpeercount",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_RequestIP,
        {
            "Request Peer IP       ", "ppvideo.requestpeerip",
            FT_IPv4, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_CandidatePeerCount,
        {
            "Candidate Peer Count  ", "ppvideo.candidatepeercount",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_UdpPort,
        {
            "UDP Port              ", "ppvideo.udpport",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_TcpPort,
        {
            "TCP Port              ", "ppvideo.tcpport",   //Upnp Tcp Port
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_StunPeerIP,
        {
            "Stun Peer IP          ", "ppvideo.stunpeerip",
            FT_IPv4, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_StunPeerUdpPort,
        {
            "Stun Peer UDP Port    ", "ppvideo.stunpeerudpport",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_RealIPCount,
        {
            "Real IP Count         ", "ppvideo.realipcount",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_RealIP,
        {
            "Real IP               ", "ppvideo.realip",
            FT_IPv4, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_RealIPs,
        {
            "All Real IPs          ", "ppvideo.realips",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_UploadBWKbs,
        {
            "Upload Bandwidth(kbs) ", "ppvideo.uploadbw",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_UploadLimitKbs,
        {
            "Upload Limit(kbs)     ", "ppvideo.uploadlimit",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_UploadSpeedKbs,
        {
            "Upload Speed(kbs)     ", "ppvideo.uploadspeed",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_RequestBlockID,
        {
            "Request Block ID      ", "ppvideo.requestblockid",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_bufmapNos,
        {
            "All Buffer Map Numbers", "ppvideo.buffermapnumbers",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ResourceCount,
        {
            "Resource Count        ", "ppvideo.resourcecount",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ResourceIDs,
        {
            "All Resource IDs          ", "ppvideo.resourceids",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_KeepAliveInterval,
        {
            "Keep Alive Interval   ", "ppvideo.keepaliveinterval",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_StunDetectedUdpPort,
        {
            "Stun Detected UDP Port", "ppvideo.stundetectedudpport",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
    	&hf_ppvideo_StunDetectedIp,
    	{
    	    "Stun Dedeted IP", "ppvideo.stundetectedip",
	    FT_IPv4, BASE_NONE,
	    NULL, 0X0, NULL, HFILL
    	}
    },

    {
        &hf_ppvideo_ProtocolVersion,
        {
            "Protocol Version      ", "ppvideo.protocolversion",
            FT_UINT16, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_packet_type,
        {
            "Packet Type           ", "ppvideo.packettype",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_reversed,
        {
            "Reversed              ", "ppvideo.reversed",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_protocal_version,
        {
            "Protocol Version      ", "ppvideo.protocolversion",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_PeerErrorCode,
        {
            "Peer Error Code       ", "ppvideo.peererrorcode",
            FT_UINT16, BASE_DEC,
            VALS(peer_error_codes), 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ErrorInfoLength,
        {
            "Error Info Length     ", "ppvideo.errorinfolength",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ErrorInfo,
        {
            "Error Info            ", "ppvideo.errorinfo",
            FT_STRING, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_SendOffTime,
        {
            "Send Off Time         ", "ppvideo.sendofftime",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ConnectType,
        {
            "Connect Type          ", "ppvideo.connecttype",
            FT_UINT8, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_IpPoolSize,
        {
            "IP Pool Size          ", "ppvideo.ippoolsize",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_BlockMap,
        {
            "BlockMap              ", "ppvideo.blockmap",
            FT_BYTES, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_PieceCount,
        {
            "Piece Count           ", "ppvideo.piececount",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_Priority,
        {
            "Priority              ", "ppvideo.priority",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_sub_piece_info,
        {
            "Sub Piece Info        ", "ppvideo.live.subpieceinfo",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_SubPieceLength,
        {
            "Sub Piece Length      ", "ppvideo.subpiecelength",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_SubPieceContent,
        {
            "Sub Piece Content     ", "ppvideo.subpiececontent",
            FT_BYTES, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_PeerCount,
        {
            "Peer Count            ", "ppvideo.peercount",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_ResourceNameLength,
        {
            "Resource Name Length  ", "ppvideo.resourcenamelength",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_ResourceName,
        {
            "Resource Name         ", "ppvideo.resourcename",
            FT_STRING, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },

    {
        &hf_ppvideo_CandidatePeerInfoMine,
        {
            "Candidate Peer Info Mine  ", "ppvideo.candidatepeerinfomine",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_CandidatePeerInfoHis,
        {
            "Candidate Peer Info His   ", "ppvideo.candidatepeerinfohis",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_PeerVersionMine,
        {
            "PeerVersion Mine      ", "ppvideo.peerversionmine",
            FT_UINT32, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_PeerTypeMine,
        {
            "Connect Type          ", "ppvideo.peertypemine",
            FT_UINT8, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },

    {
        &hf_ppvideo_Report_Reserved,
        {
            "Report Reserved       ", "ppvideo.tracker.report.reserved",
            FT_UINT8, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_UploadPriority,
        {
            "Upload Priority       ", "ppvideo.tracker.uploadpriority",
            FT_UINT8, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_IdleTimeInMins,
        {
            "Idle Time In Mins     ", "ppvideo.tracker.idletimeinmins",
            FT_UINT8, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_InternalTcpPort,
        {
            "Internal Tcp Port     ", "ppvideo.tracker.internaltcpport",
            FT_UINT8, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_NatType,
        {
            "NAT Type              ", "ppvideo.tracker.report.nattype",
            FT_UINT8, BASE_HEX,
            VALS(nat_types), 0x00, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ReportResourceCount,
        {
            "Report Resource Count ", "ppvideo.tracker.report.reportcount",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_LocalResourceCount,
        {
            "Local Resource Count  ", "ppvideo.tracker.report.localresourcecount",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ServerResourceCount,
        {
            "Server Resource Count ", "ppvideo.tracker.report.serverresourcecount",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ContentMD5,
        {
            "Content Sense MD5     ", "ppvideo.index.contentmd5",
            FT_GUID, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ContentBytes,
        {
            "Content Bytes Length  ", "ppvideo.index.contentbytes",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_Reserved,
        {
            "Reserved              ", "ppvideo.reserved",
            FT_BYTES, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_PeerAction,
        {
            "Peer Action           ", "ppvideo.peeraction",
            FT_UINT8, BASE_HEX,
            VALS(peer_action_names), 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_TimeStamp,
        {
            "TimeStamp             ", "ppvideo.timestamp",
            FT_ABSOLUTE_TIME, BASE_DEC,     // TODO: improve
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ResourcePeerCount,
        {
            "PeerCount             ", "ppvideo.tracker.peercount",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_DownloadConnectedCount,
        {
            "Download Connect Count", "ppvideo.livepeer.downloadconnectedcount",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_UploadConnectedCount,
        {
            "Upload Connected Count", "ppvideo.livepeer.uploadconnectedcount",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_UploadSpeed,
        {
            "Upload Speed          ", "ppvideo.livepeer.uploadspeed",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_MaxUploadSpeed,
        {
            "Max Upload Speed      ", "ppvideo.livepeer.maxuploadspeed",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_RestPlayableTime,
        {
            "Rest Playable Time    ", "ppvideo.livepeer.restplayabletime",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_LostRate,
        {
            "Lost Rate             ", "ppvideo.livepeer.lostrate",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_RedundancyRate,
        {
            "Redundancy Rate       ", "ppvideo.livepeer.redundancyrate",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_PeerInfoLength,
        {
            "Peer Info Length      ", "ppvideo.livepeer.peerinfolength",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },

    // QueryIndexServerList, PeerVersion >= 5
    {
        &hf_ppvideo_IndexServerCount,
        {
            "IndexServerCount      ", "ppvideo.indexservercount",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_IndexServerInfos,
        {
            "IndexServerInfos      ", "ppvideo.indexserverinfos",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_IndexServerInfo,
        {
            "IndexServerInfo       ", "ppvideo.indexserverinfo",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_IndexServerInfo_Length,
        {
            "Length                ", "ppvideo.indexserver.length",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_IndexServerInfo_IP,
        {
            "IP                    ", "ppvideo.indexserver.ip",
            FT_IPv4, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_IndexServerInfo_Port,
        {
            "Port                  ", "ppvideo.indexserver.port",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_IndexServerInfo_Type,
        {
            "Type                  ", "ppvideo.indexserver.type",
            FT_UINT8, BASE_DEC,
            VALS(index_server_info_types), 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_ModIndexMap,
        {
            "Mod Index Server Map  ", "ppvideo.modindexmap",
            FT_BYTES, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },

    {
        &hf_ppvideo_StringCount,
        {
            "String Count          ", "ppvideo.stringcount",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_StringArray,
        {
            "String Array          ", "ppvideo.stringarray",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_StringLength,
        {
            "Length                ", "ppvideo.stringlength",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_StringData,
        {
            "Data                  ", "ppvideo.stringdata",
            FT_STRING, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
	

    {
        &hf_ppvideo_NeedReport,
        {
            "Need Report           ", "ppvideo.needreport",
            FT_BOOLEAN, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_IntervalTime,
        {
            "Need Report           ", "ppvideo.intervaltime",
            FT_BOOLEAN, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataCollectorInfo,
        {
            "Data Collector        ", "ppvideo.datacollector",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataCollectorInfo_IP,
        {
            "IP Address            ", "ppvideo.datacollector.ip",
            FT_IPv4, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataCollectorInfo_Length,
        {
            "Length                ", "ppvideo.datacollector.length",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataCollectorInfo_Port,
        {
            "Port                  ", "ppvideo.datacollector.port",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataCollectorInfo_Type,
        {
            "Type                  ", "ppvideo.datacollector.type",
            FT_UINT8, BASE_DEC,
            VALS(data_collector_info_types), 0x0, NULL, HFILL
        }
    },

    {
        &hf_ppvideo_KernelVersion,
        {
            "Kernel Version        ", "ppvideo.datacollector.kernelversion",
            FT_UINT32, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_UploadPicProbability,
        {
            "Upload Pic Probability", "ppvideo.index.uploadpicprobability",
            FT_FLOAT, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },

    // Data Collection
    {
        &hf_ppvideo_DataOnAppStop,
        {
            "Data On App Stop      ", "ppvideo.data.onappstop",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnAppStop_MaxUploadSpeedInKBps,
        {
            "MaxUploadSpeed(KBps)  ", "ppvideo.data.onappstop.maxuploadspeed",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnAppStop_MaxDownloadSpeedInKBps,
        {
            "MaxDownloadSpeed(KBps)", "ppvideo.data.onappstop.maxdownloadspeed",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnAppStop_DiskUsedSizeInMB ,
        {
            "DiskUsedSize(MB)      ", "ppvideo.data.onappstop.diskusedsize",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnAppStop_UploadCacheHitRate ,
        {
            "UploadCacheHitRate    ", "ppvideo.data.onappstop.uploadcachehitrate",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnAppStop_UploadDataBytesInMB ,
        {
            "UploadData (MB)       ", "ppvideo.data.onappstop.uploaddata",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnAppStop_DownloadDataBytesInMB ,
        {
            "DownloadData (MB)     ", "ppvideo.data.onappstop.downloaddata",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnAppStop_DownloadDurationInSec ,
        {
            "DownloadDuration (Sec)", "ppvideo.data.onappstop.downloadduration",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnAppStop_RunningDurationInMin ,
        {
            "RunningDuration (Sec) ", "ppvideo.data.onappstop.runningduration",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnAppStop_CpuPeakValue ,
        {
            "CPU Peak Usage        ", "ppvideo.data.onappstop.cpupeakvalue",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnAppStop_MemPeakValue ,
        {
            "Memory Peak Usage (MB)", "ppvideo.data.onappstop.mempeakvalue",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnAppStop_IncomingPeersCount ,
        {
            "Incoming Peers Count  ", "ppvideo.data.onappstop.incomingpeerscount",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnAppStop_FlvFolderPath_Length,
        {
            "FlvFolderPathLength   ", "ppvideo.data.onappstop.flvfolderpath.length",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnAppStop_FlvFolderPath ,
        {
            "Flv Folder Path       ", "ppvideo.data.onappstop.flvfolderpath",
            FT_STRING, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnDownloadStopEx ,
        {
            "Data On Download Stop ", "ppvideo.data.ondownloadstop",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnDownloadStopEx_FlvUrl_Length,
        {
            "Flv Url Length        ", "ppvideo.data.ondownloadstop.flvurllength",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnDownloadStopEx_FlvUrl ,
        {
            "Flv Url               ", "ppvideo.data.ondownloadstop.flvurl",
            FT_STRING, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnDownloadStopEx_FlvReferUrl ,
        {
            "Flv Refer Url         ", "ppvideo.data.ondownloadstop.flvreferurl",
            FT_STRING, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnDownloadStopEx_FlvReferUrl_Length,
        {
            "Flv Refer Url Length  ", "ppvideo.data.ondownloadstop.flvreferurllength",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnDownloadStopEx_DataItemsCount ,
        {
            "Data Items Count       ", "ppvideo.data.ondownloadstop.dataitemscount",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnDownloadStopEx_DataItemsSize,
        {
            "Data Items Size       ", "ppvideo.data.ondownloadstop.dataitemssize",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnDownloadStopEx_DataItems ,
        {
            "Data Items            ", "ppvideo.data.ondownloadstop.dataitems",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnDownloadStopEx_DataItem ,
        {
            "Data Item             ", "ppvideo.data.ondownloadstop.dataitem",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnDownloadStopEx_DataItem_Size,
        {
            "Data Item Size        ", "ppvideo.data.ondownloadstop.dataitemsize",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnDownloadStopEx_DataItem_Type ,
        {
            "Data Type             ", "ppvideo.data.ondownloadstop.datatype",
            FT_UINT8, BASE_DEC,
            VALS(data_collection_types), 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnDownloadStopEx_DataItem_Value_U8 ,
        {
            "Data Value            ", "ppvideo.data.ondownloadstop.datavalue",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnDownloadStopEx_DataItem_Value_U16 ,
        {
            "Data Value            ", "ppvideo.data.ondownloadstop.datavalue",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnDownloadStopEx_DataItem_Value_U32 ,
        {
            "Data Value            ", "ppvideo.data.ondownloadstop.datavalue",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_DataOnDownloadStopEx_DataItem_Value_Default,
        {
            "Data Value            ", "ppvideo.data.ondownloadstop.datavalue",
            FT_BYTES, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },
/* -----------------------NOTIFY----------------------- */
    {
        &hf_ppvideo_notify_TaskID,
        {
            "Task ID               ", "ppvideo.notify.taskid",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_notify_Duration,
        {
            "Duration              ", "ppvideo.notify.duration",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_notify_RestTime,
        {
            "RestTime              ", "ppvideo.notify.resttime",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_notify_TaskType,
        {
            "TaskType              ", "ppvideo.notify.tasktype",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_notify_TaskContentLen,
        {
            "TaskContentLen        ", "ppvideo.notify.contentlen",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_notify_TaskContent,
        {
            "TaskContent           ", "ppvideo.notify.content",
            FT_STRING, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_notify_PeerOnline,
        {
            "PeerOnline            ", "ppvideo.notify.peeronline",
                FT_UINT32, BASE_DEC,
                NULL, 0x0, NULL, HFILL
        }
    },
    {
        &hf_ppvideo_notify_TaskNum,
        {
            "TaskNum               ", "ppvideo.notify.tasknum",
                FT_UINT16, BASE_DEC,
                NULL, 0x0, NULL, HFILL
        }
    },

    {
        &hf_ppvideo_notify_JoinRet,
        {
            "Join Ret              ", "ppvideo.report.struct.type",
                FT_UINT8, BASE_DEC,
                NULL, 0x0, NULL, HFILL
        }
    },

    {
        &hf_ppvideo_notify_NodeCount,
        {
            "NodeCount             ", "ppvideo.notify.nodecount",
                FT_UINT8, BASE_DEC,
                NULL, 0x0, NULL, HFILL
        }
    },

    {
        &hf_ppvideo_notify_NodeInfo,
        {
            "NodeInfo              ", "ppvideo.notify.nodeinfo",
                FT_NONE, BASE_NONE,
                NULL, 0x0, NULL, HFILL
        }
    },

    {
        &hf_ppvideo_notify_TaskInfo,
        {
            "TaskInfo              ", "ppvideo.notify.taskinfo",
                FT_NONE, BASE_NONE,
                NULL, 0x0, NULL, HFILL
        }
    },

    {
        &hf_ppvideo_notify_TaskComplete,
        {
            "TaskComplete          ", "ppvideo.notify.taskcomplete",
                FT_UINT32, BASE_DEC,
                NULL, 0x0, NULL, HFILL
        }
    },
	
	{
        &hf_ppvideo_ResourcePeerCountInfoCount,
        {
            "Peer Count Info Count", "ppvideo.tracker.peercountinfo.count",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_ResourcePeerCountInfos,
        {
            "Peer Count Infos     ", "ppvideo.tracker.peercountinfos",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_ResourcePeerCountInfo,
        {
            "Peer Count Info      ", "ppvideo.tracker.peercountinfo",
            FT_NONE, BASE_NONE,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_request_buffermap_piece_id,
        {
            "Request Buffer Map ID ", "ppvideo.livepeer.buffermapid",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_MagicNumber,
        {
            "Magic Number          ", "ppvideo.tracker.magicnumber",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_CommandID,
        {
            "Command ID            ", "ppvideo.tracker.commandid",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_InternalCommandReserve,
        {
            "Reserve               ", "ppvideo.tracker.internalcommandreserve",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	
	{
        &hf_ppvideo_upload_bandwidth,
        {
            "Upload Bandwidth      ", "ppvideo.livepeer.uploadbandwidth",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_request_announce_reserved,
        {
            "Reserved              ", "ppvideo.livepeer.requestannouncereserved",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_piece_info_count,
        {
            "Block Count           ", "ppvideo.livepeer.peercount",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_interval,
        {
            "Interval              ", "ppvideo.livepeer.interval",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_header_piece_id,
        {
            "Header Piece ID       ", "ppvideo.livepeer.headerpieceid",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_start_piece_info_id,
        {
            "Start Piece Info ID   ", "ppvideo.livepeer.startpieceinfoid",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_subpiece_No,
        {
            "SubPiece Number       ", "ppvideo.livepeer.subpieceno",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_start_piece_id,
        {
            "Start piece ID       ", "ppvideo.livepeer.startpieceid",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_buffer_map_length,
        {
            "Buffer Map Length    ", "ppvideo.livepeer.buffermaplength",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_buffer_map,
        {
            "Buffer Map           ", "ppvideo.livepeer.buffermap",
            FT_BYTES, BASE_HEX,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_live_priority,
        {
            "Live Priority        ", "ppvideo.livepeer.priority",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_piece_count,
        {
            "Piece Info Count      ", "ppvideo.livepeer.pieceinfocount",
            FT_UINT8, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_SubPieceInfo_PieceID,
        {
            "SubPiece ID          ", "ppvideo.subpieceinfo.pieceid",
            FT_UINT32, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },
	{
        &hf_ppvideo_piece_count,
        {
            "Piece Count          ", "ppvideo.subpieceinfo.piececount",
            FT_UINT16, BASE_DEC,
            NULL, 0x0, NULL, HFILL
        }
    },

	
	
 };

// info 
static char info[1024];

// sub trees
static gint ett_ppvideo_tcp = -1;
static gint ett_ppvideo = -1;
static gint ett_ppvideo_index = -1;
static gint ett_ppvideo_tracker = -1;
static gint ett_ppvideo_stun = -1;
static gint ett_ppvideo_peer = -1;
static gint ett_ppvideo_peer_tcp = -1;
static gint ett_live_ppvideo_peer = -1;
static gint ett_ppvideo_statistic = -1;
static gint ett_ppvideo_http_server = -1;
static gint ett_ppvideo_url_info = -1;
static gint ett_ppvideo_region_info = -1;
static gint ett_ppvideo_resource_info = -1;
static gint ett_ppvideo_rid_info = -1;
static gint ett_ppvideo_tracker_info = -1;
static gint ett_ppvideo_stun_server_info = -1;
static gint ett_ppvideo_peer_info = -1;
static gint ett_ppvideo_file_info = -1;
static gint ett_ppvideo_download_info = -1;
static gint ett_ppvideo_subpiece_info = -1;
static gint ett_ppvideo_LiveBufferMap = -1;
static gint ett_ppvideo_blockmd5s = -1;
static gint ett_ppvideo_http_servers = -1;
static gint ett_ppvideo_server_type_infos = -1;
static gint ett_ppvideo_server_type_info = -1;
static gint ett_ppvideo_server_lists = -1;
static gint ett_ppvideo_server_list = -1;
static gint ett_ppvideo_server_datas = -1;
static gint ett_ppvideo_server_data = -1;
static gint ett_ppvideo_push_task_param = -1;
static gint ett_ppvideo_peer_infos = -1;
static gint ett_ppvideo_play_history_items = -1;
static gint ett_ppvideo_play_history_item = -1;
static gint ett_ppvideo_tracker_infos = -1;
static gint ett_ppvideo_stun_server_infos = -1;
static gint ett_ppvideo_sn_server_infos = -1;
static gint ett_ppvideo_sn_server_info = -1;
static gint ett_ppvideo_realips = -1;
static gint ett_ppvideo_bufmaplens = -1;
static gint ett_ppvideo_resource_ids = -1;
static gint ett_ppvideo_subpiece_infos = -1;
static gint ett_ppvideo_report_structs = -1;
static gint ett_ppvideo_report_struct = -1;
static gint ett_ppvideo_index_server_infos = -1;
static gint ett_ppvideo_index_server_info = -1;
static gint ett_ppvideo_stringarray = -1;
static gint ett_ppvideo_data_collector = -1;
static gint ett_ppvideo_data_collector_info = -1;
static gint ett_ppvideo_data_on_download_stop_ex_item_array = -1;
// notify
static gint ett_ppvideo_notify = -1;
static gint ett_ppvideo_notify_nodeinfo = -1;
static gint ett_ppvideo_notify_taskinfo = -1;
static gint ett_ppvideo_resource_peer_count_info = -1;
static gint ett_ppvideo_resource_peer_count_infos = -1;
static gint ett_ppvideo_push_task_items = -1;
static gint ett_ppvideo_push_task_item = -1;

static gint* ett_ppvideo_all[] =
{
	&ett_ppvideo_tcp,
    &ett_ppvideo,
    &ett_ppvideo_index,
    &ett_ppvideo_tracker,
    &ett_ppvideo_stun,
    &ett_ppvideo_peer,
	&ett_ppvideo_peer_tcp,
	&ett_live_ppvideo_peer,
    &ett_ppvideo_statistic,
    &ett_ppvideo_http_server,
	&ett_ppvideo_url_info,
	&ett_ppvideo_region_info,
    &ett_ppvideo_resource_info,
	&ett_ppvideo_rid_info,
    &ett_ppvideo_tracker_info,
    &ett_ppvideo_stun_server_info,
    &ett_ppvideo_peer_info,
    &ett_ppvideo_file_info,
    &ett_ppvideo_download_info,
    &ett_ppvideo_subpiece_info,
	&ett_ppvideo_LiveBufferMap,
    &ett_ppvideo_blockmd5s,
    &ett_ppvideo_http_servers,
	&ett_ppvideo_server_type_infos,
	&ett_ppvideo_server_type_info,
	&ett_ppvideo_server_lists,
	&ett_ppvideo_server_list,
	&ett_ppvideo_server_datas,
	&ett_ppvideo_server_data,
	&ett_ppvideo_push_task_param,
    &ett_ppvideo_peer_infos,
    &ett_ppvideo_play_history_items,
    &ett_ppvideo_play_history_item,
    &ett_ppvideo_tracker_infos,
    &ett_ppvideo_stun_server_infos,
	&ett_ppvideo_sn_server_info,
	&ett_ppvideo_sn_server_infos,
    &ett_ppvideo_realips,
    &ett_ppvideo_resource_ids,
    &ett_ppvideo_subpiece_infos,
    &ett_ppvideo_report_structs,
    &ett_ppvideo_report_struct,
    &ett_ppvideo_index_server_infos,
    &ett_ppvideo_index_server_info,
    &ett_ppvideo_stringarray,
    &ett_ppvideo_data_collector,
    &ett_ppvideo_data_collector_info,
    &ett_ppvideo_data_on_download_stop_ex_item_array,

    // notify
    &ett_ppvideo_notify,
    &ett_ppvideo_notify_nodeinfo,
    &ett_ppvideo_notify_taskinfo,
	
	&ett_ppvideo_resource_peer_count_info,
	&ett_ppvideo_resource_peer_count_infos,
	&ett_ppvideo_bufmaplens,
	
	&ett_ppvideo_push_task_items,
	&ett_ppvideo_push_task_item,
	
	
};

gboolean check_sum(tvbuff_t *tvb)
{
    static const gint32  CHECKSUM_LENGTH = 4;
    static const gint32 BLOCK_SIZE = 8;
    static const gint32 BLOCK_SIZE_HALF = 4;
    
    if (tvb != NULL)
    {
        guint32 header, crc;
        gint i, length;
        const guint8 *data;

        header = tvb_get_letohl(tvb, 0);
        length = tvb_length_remaining(tvb, 0);
        data = tvb_get_ptr(tvb, 0, length);

        if (length == -1) return FALSE;
        
        crc = (guint32)0x15926326184LL;
        for (i = CHECKSUM_LENGTH; i + BLOCK_SIZE < length; i += BLOCK_SIZE)
        {
            crc ^= ((*(guint32*)(data + i)) ^ (crc << 15) ^ (*(guint32*)(data + i + BLOCK_SIZE_HALF)) ^ (crc >> 7));
        }
        while (i < length) 
        {
            crc ^= ((crc >> 13) ^ (data[i++]&0xFF) ^ (crc << 7));
        }
        
        return crc == header;
    }
    return FALSE;
}

gboolean check_sum2(tvbuff_t *tvb)
{
    static const gint32  CHECKSUM_LENGTH = 4;
    static const gint32 BLOCK_SIZE = 8;
    static const gint32 BLOCK_SIZE_HALF = 4;
    
    if (tvb != NULL)
    {
        guint32 header, crc;
        gint i, length;
        const guint8 *data;

        header = tvb_get_letohl(tvb, 0);
        length = tvb_length_remaining(tvb, 0);
        data = tvb_get_ptr(tvb, 0, length);

        if (length == -1) return FALSE;
        
        crc = (guint32)0x20110312312LL;
        for (i = CHECKSUM_LENGTH; i + BLOCK_SIZE < length; i += BLOCK_SIZE)
        {
            crc ^= ((*(guint32*)(data + i)) ^ (crc << 14) ^ (*(guint32*)(data + i + BLOCK_SIZE_HALF)) ^ (crc >> 6));
        }
        while (i < length) 
        {
            crc ^= ((crc >> 13) ^ (data[i++]&0xFF) ^ (crc << 7));
        }
        
        return crc == header;
    }
    return FALSE;
}

void dissect_ppvideo_HttpServer(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint* offset)
{
    if (tree && offset)
    {
        proto_item *item;
        proto_tree *http_server_tree;
//        guint offset;
        guint url_length;
        guint refer_length;

        item = proto_tree_add_item(tree, hf_ppvideo_HttpServer, tvb, 0, -1, FALSE);
        http_server_tree = proto_item_add_subtree(item, ett_ppvideo_http_server);

        //offset = 0;
        proto_tree_add_item(http_server_tree, hf_ppvideo_HttpServer_Length, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(http_server_tree, hf_ppvideo_HttpServer_Type, tvb, *offset, 1, TRUE); *offset += 1;
        url_length = tvb_get_letohs(tvb, *offset);
        proto_tree_add_item(http_server_tree, hf_ppvideo_HttpServer_UrlLength, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(http_server_tree, hf_ppvideo_HttpServer_UrlString, tvb, *offset, url_length, TRUE); *offset += url_length;
        refer_length = tvb_get_letohs(tvb, *offset);
        proto_tree_add_item(http_server_tree, hf_ppvideo_HttpServer_ReferLength, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(http_server_tree, hf_ppvideo_HttpServer_ReferString, tvb, *offset, refer_length, TRUE); *offset += refer_length;

    }
}

void dissect_ppvideo_ServerTypeInfo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint* offset)
{
    if (tree && offset)
    {
        proto_item *server_type_info_item;
        proto_tree *server_type_info_tree;

        server_type_info_item = proto_tree_add_item(tree, hf_ppvideo_ServerTypeInfo, tvb, 0, -1, FALSE);
        server_type_info_tree = proto_item_add_subtree(server_type_info_item, ett_ppvideo_server_type_info);

        //offset = 0;
        proto_tree_add_item(server_type_info_tree, hf_ppvideo_ServerTypeInfo_Connect_Type, tvb, *offset, 1, TRUE); *offset += 1;
        proto_tree_add_item(server_type_info_tree, hf_ppvideo_ServerTypeInfo_Server_Type, tvb, *offset, 1, TRUE); *offset += 1;
        proto_tree_add_item(server_type_info_tree, hf_ppvideo_ServerTypeInfo_Level, tvb, *offset, 2, TRUE); *offset += 2;

    }
}
void dissect_ppvideo_ServerData(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint* offset)
{
    if (tree && offset)
    {
        proto_item *server_data_item;
        proto_tree *server_data_tree;

        server_data_item = proto_tree_add_item(tree, hf_ppvideo_ServerData, tvb, 0, -1, FALSE);
        server_data_tree = proto_item_add_subtree(server_data_item, ett_ppvideo_server_data);

        //offset = 0;
		proto_tree_add_item(server_data_tree, hf_ppvideo_ServerData_IP, tvb, *offset, 4, TRUE); *offset += 4;
		proto_tree_add_item(server_data_tree, hf_ppvideo_ServerData_Port, tvb, *offset, 2, TRUE); *offset += 2;
		proto_tree_add_item(server_data_tree, hf_ppvideo_ServerData_ModID, tvb, *offset, 1, TRUE); *offset += 1;
		proto_tree_add_item(server_data_tree, hf_ppvideo_ServerData_ConnectType, tvb, *offset, 1, TRUE); *offset += 1;
    }
}
void dissect_ppvideo_ServerDatas(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset, guint16 server_data_count)
{
    if (tree && server_data_count > 0)
    {
        proto_item *server_data_item;
        proto_tree *server_data_tree;
        guint16 i;
        
        server_data_item = proto_tree_add_item(tree, hf_ppvideo_ServerDatas, tvb, *offset, -1, FALSE);
        server_data_tree = proto_item_add_subtree(server_data_item, ett_ppvideo_server_datas);
        for (i = 0; i < server_data_count; i++)
        {
            dissect_ppvideo_ServerData(tvb, pinfo, server_data_tree, offset);
        }
    }
}
void dissect_ppvideo_ServerList(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint* offset)
{
    if (tree && offset)
    {
        proto_item *server_list_item;
        proto_tree *server_list_tree;
		guint16 server_data_count;
		guint16 padding_info_count;
		guint16 i;

        server_list_item = proto_tree_add_item(tree, hf_ppvideo_ServerList, tvb, 0, -1, FALSE);
        server_list_tree = proto_item_add_subtree(server_list_item, ett_ppvideo_server_list);

        //offset = 0;
		dissect_ppvideo_ServerTypeInfo(tvb, pinfo, server_list_tree, offset);
		server_data_count = tvb_get_letohs(tvb, *offset);
		proto_tree_add_item(server_list_tree, hf_ppvideo_ServerData_Count, tvb, *offset, 2, TRUE); *offset += 2;
		dissect_ppvideo_ServerDatas(tvb, pinfo, server_list_tree, offset, server_data_count);
		padding_info_count = tvb_get_letohs(tvb, *offset);
		proto_tree_add_item(server_list_tree, hf_ppvideo_ServerList_Padding_Info_Count, tvb, *offset, 2, TRUE); *offset += 2;
		for (i = 0; i < padding_info_count; ++i)
		{
			proto_tree_add_item(server_list_tree, hf_ppvideo_ServerList_Padding_Info, tvb, *offset, 1, TRUE); *offset += 1;
		}
		
		
        

    }
}



void dissect_ppvideo_UrlInfo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint* offset)
{
    if (tree && offset)
    {
        proto_item *item;
        proto_tree *url_info_tree;
//        guint offset;
        guint16 url_length;
        guint16 refer_length;

        item = proto_tree_add_item(tree, hf_ppvideo_UrlInfo, tvb, 0, -1, FALSE);
        url_info_tree = proto_item_add_subtree(item, ett_ppvideo_url_info);

        //offset = 0;
        proto_tree_add_item(url_info_tree, hf_ppvideo_UrlInfo_Length, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(url_info_tree, hf_ppvideo_UrlInfo_Type, tvb, *offset, 1, TRUE); *offset += 1;
        url_length = tvb_get_letohs(tvb, *offset);
        proto_tree_add_item(url_info_tree, hf_ppvideo_UrlInfo_UrlLength, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(url_info_tree, hf_ppvideo_UrlInfo_UrlString, tvb, *offset, url_length, TRUE); *offset += url_length;
        refer_length = tvb_get_letohs(tvb, *offset);
        proto_tree_add_item(url_info_tree, hf_ppvideo_UrlInfo_ReferLength, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(url_info_tree, hf_ppvideo_UrlInfo_ReferString, tvb, *offset, refer_length, TRUE); *offset += refer_length;

    }
}

void dissect_ppvideo_BlockMD5s(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset, guint16 block_count)
{
    if (tree && block_count > 0)
    {
        proto_item *blockmd5s_item;
        proto_tree *blockmd5s_tree;
        guint i;
        
        blockmd5s_item = proto_tree_add_item(tree, hf_ppvideo_BlockMD5s, tvb, *offset, -1, FALSE);
        blockmd5s_tree = proto_item_add_subtree(blockmd5s_item, ett_ppvideo_blockmd5s);
        for (i = 0; i < block_count; i++)
        {
            proto_tree_add_item(blockmd5s_tree, hf_ppvideo_BlockMD5, tvb, *offset, 16, TRUE); *offset += 16;
        }
    }
}

void dissect_ppvideo_HttpServers(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset, guint16 http_server_count)
{
    if (tree && http_server_count > 0)
    {
        proto_item *http_servers_item;
        proto_tree *http_servers_tree;
        guint16 i;
        
        http_servers_item = proto_tree_add_item(tree, hf_ppvideo_HttpServers, tvb, *offset, -1, FALSE);
        http_servers_tree = proto_item_add_subtree(http_servers_item, ett_ppvideo_http_servers);
        for (i = 0; i < http_server_count; i++)
        {
            dissect_ppvideo_HttpServer(tvb, pinfo, http_servers_tree, offset);
        }
    }
}

void dissect_ppvideo_ServerTypeInfos(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset, guint8 server_type_info_count)
{
    if (tree && server_type_info_count > 0)
    {
        proto_item *server_type_info_item;
        proto_tree *server_type_info_tree;
        guint8 i;
        
        server_type_info_item = proto_tree_add_item(tree, hf_ppvideo_ServerTypeInfos, tvb, *offset, -1, FALSE);
        server_type_info_tree = proto_item_add_subtree(server_type_info_item, ett_ppvideo_server_type_infos);
        for (i = 0; i < server_type_info_count; i++)
        {
            dissect_ppvideo_ServerTypeInfo(tvb, pinfo, server_type_info_tree, offset);
        }
    }
}



void dissect_ppvideo_ServerLists(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset, guint8 server_list_count)
{
    if (tree && server_list_count > 0)
    {
        proto_item *server_list_item;
        proto_tree *server_list_tree;
        guint8 i;
        
        server_list_item = proto_tree_add_item(tree, hf_ppvideo_ServerLists, tvb, *offset, -1, FALSE);
        server_list_tree = proto_item_add_subtree(server_list_item, ett_ppvideo_server_lists);
        for (i = 0; i < server_list_count; i++)
        {
            dissect_ppvideo_ServerList(tvb, pinfo, server_list_tree, offset);
        }
    }
}



void dissect_ppvideo_ResourceInfo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    if (tree)
    {
        proto_item *resource_info_item;
        proto_tree *resource_info_tree;

       // guint offset;
        guint http_server_count;
        guint16 block_count;

        resource_info_item = proto_tree_add_item(tree, hf_ppvideo_ResourceInfo, tvb, *offset, -1, FALSE);
        resource_info_tree = proto_item_add_subtree(resource_info_item, ett_ppvideo_resource_info);

        //offset = 0;
        proto_tree_add_item(resource_info_tree, hf_ppvideo_ResourceInfo_Length, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(resource_info_tree, hf_ppvideo_ResourceInfo_ResourceID, tvb, *offset, 16, TRUE); *offset += 16;
        proto_tree_add_item(resource_info_tree, hf_ppvideo_ResourceInfo_FileLength, tvb, *offset, 4, TRUE); *offset += 4;
        block_count = tvb_get_letohs(tvb, *offset);
        proto_tree_add_item(resource_info_tree, hf_ppvideo_ResourceInfo_BlockCount, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(resource_info_tree, hf_ppvideo_ResourceInfo_BlockSize, tvb, *offset, 4, TRUE); *offset += 4;

        dissect_ppvideo_BlockMD5s(tvb, pinfo, resource_info_tree, offset, block_count);

        http_server_count = tvb_get_letohs(tvb, *offset);
        proto_tree_add_item(resource_info_tree, hf_ppvideo_ResourceInfo_HttpServerCount, tvb, *offset, 2, TRUE); *offset += 2;
        dissect_ppvideo_HttpServers(tvb, pinfo, resource_info_tree, offset, http_server_count);

        // PeerVersion >= 4
        if (ppvideo_peer_version >= 4)
        {
            // content md5
            // content bytes
        }
    }
}

void dissect_ppvideo_RidInfo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    if (tree)
    {
        proto_item *rid_info_item;
        proto_tree *rid_info_tree;
		guint32 block_count;

        rid_info_item = proto_tree_add_item(tree, hf_ppvideo_RidInfo, tvb, *offset, -1, FALSE);
        rid_info_tree = proto_item_add_subtree(rid_info_item, ett_ppvideo_rid_info);

		
		proto_tree_add_item(rid_info_tree, hf_ppvideo_ResourceID, tvb, *offset, 16, TRUE); *offset += 16;
		proto_tree_add_item(rid_info_tree, hf_ppvideo_File_Length, tvb, *offset, 4, TRUE); *offset += 4;
					
		
		block_count = tvb_get_letohl(tvb, *offset);
		proto_tree_add_item(rid_info_tree, hf_ppvideo_Block_Count, tvb, *offset, 4, TRUE); *offset += 4;
					
		proto_tree_add_item(rid_info_tree, hf_ppvideo_Block_Size, tvb, *offset, 4, TRUE); *offset += 4;
		dissect_ppvideo_BlockMD5s(tvb, pinfo, rid_info_tree, offset, block_count);
    }
}

void dissect_ppvideo_PushTaskParam(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    if (tree)
    {
        proto_item *push_task_param_item;
        proto_tree *push_task_param_tree;

        push_task_param_item = proto_tree_add_item(tree, hf_ppvideo_PushTaskParam, tvb, *offset, -1, FALSE);
        push_task_param_tree = proto_item_add_subtree(push_task_param_item, ett_ppvideo_push_task_param);
		
		proto_tree_add_item(push_task_param_tree, hf_ppvideo_PushTaskParam_Task_Type, tvb, *offset, 2, TRUE); *offset += 2;
		proto_tree_add_item(push_task_param_tree, hf_ppvideo_PushTaskParam_Protect_Time_Interval_In_Seconds, tvb, *offset, 2, TRUE); *offset += 2;
		proto_tree_add_item(push_task_param_tree, hf_ppvideo_PushTaskParam_Min_Download_Speed_In_KBps, tvb, *offset, 2, TRUE); *offset += 2;			
		proto_tree_add_item(push_task_param_tree, hf_ppvideo_PushTaskParam_Max_Download_Speed_In_KBps_When_Idle, tvb, *offset, 2, TRUE); *offset += 2;
		proto_tree_add_item(push_task_param_tree, hf_ppvideo_PushTaskParam_Max_Download_Speed_In_KBps_When_Normal, tvb, *offset, 2, TRUE); *offset += 2;
		proto_tree_add_item(push_task_param_tree, hf_ppvideo_PushTaskParam_Bandwidth_Ratio_When_Idle, tvb, *offset, 1, TRUE); *offset += 1;			
		proto_tree_add_item(push_task_param_tree, hf_ppvideo_PushTaskParam_Bandwidth_Ratio_When_Normal, tvb, *offset, 1, TRUE); *offset += 1;
    }
}

void dissect_ppvideo_RegionInfo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    if (tree)
    {
        proto_item *region_info_item;
        proto_tree *region_info_tree;

        region_info_item = proto_tree_add_item(tree, hf_ppvideo_RegionInfo, tvb, *offset, -1, FALSE);
        region_info_tree = proto_item_add_subtree(region_info_item, ett_ppvideo_region_info);

		
		proto_tree_add_item(region_info_tree, hf_ppvideo_RegionInfo_PublicIP, tvb, *offset, 4, TRUE); *offset += 4;
		proto_tree_add_item(region_info_tree, hf_ppvideo_RegionInfo_Rigion_Code, tvb, *offset, 4, TRUE); *offset += 4;
		proto_tree_add_item(region_info_tree, hf_ppvideo_RegionInfo_Version, tvb, *offset, 2, TRUE); *offset += 2;
		
    }
}

void dissect_ppvideo_TrackerInfo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    if (tree)
    {
        proto_item *tracker_info_item;
        proto_tree *tracker_info_tree;

        tracker_info_item = proto_tree_add_item(tree, hf_ppvideo_TrackerInfo, tvb, *offset, -1, FALSE);
        tracker_info_tree = proto_item_add_subtree(tracker_info_item, ett_ppvideo_tracker_info);

        proto_tree_add_item(tracker_info_tree, hf_ppvideo_TrackerInfo_StationNo, tvb, *offset, 1, TRUE); *offset += 1;
        proto_tree_add_item(tracker_info_tree, hf_ppvideo_TrackerInfo_Reserve, tvb, *offset, 1, TRUE); *offset += 1;
        proto_tree_add_item(tracker_info_tree, hf_ppvideo_TrackerInfo_ModNo, tvb, *offset, 1, TRUE); *offset += 1;
        proto_tree_add_item(tracker_info_tree, hf_ppvideo_TrackerInfo_IP, tvb, *offset, 4, TRUE); *offset += 4;
        proto_tree_add_item(tracker_info_tree, hf_ppvideo_TrackerInfo_Port, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(tracker_info_tree, hf_ppvideo_TrackerInfo_Type, tvb, *offset, 1, TRUE); *offset += 1;
    }
}

void dissect_ppvideo_TrackerInfos(tvbuff_t *tvb, packet_info* pinfo, proto_tree *tree, guint * offset, guint16 tracker_count)
{
    if (tree && tracker_count > 0)
    {
        proto_item *tracker_infos_item;
        proto_tree *tracker_infos_tree;
        guint16 i;

        tracker_infos_item = proto_tree_add_item(tree, hf_ppvideo_TrackerInfos, tvb, *offset, -1, FALSE);
        tracker_infos_tree = proto_item_add_subtree(tracker_infos_item, ett_ppvideo_tracker_infos);

        for (i = 0; i < tracker_count; i++)
        {
            dissect_ppvideo_TrackerInfo(tvb, pinfo, tracker_infos_tree, offset);
        }
    }
}

void dissect_ppvideo_NotifyServerInfos(tvbuff_t *tvb, packet_info* pinfo, proto_tree *tree, guint * offset, guint16 notifyserver_count)
{
    if (tree && notifyserver_count > 0)
    {
        guint16 i;
        for (i = 0; i < notifyserver_count; i++)
        {
            proto_tree_add_item(tree, hf_ppvideo_StunServerInfo_IP, tvb, *offset, 4, TRUE); *offset += 4;
            proto_tree_add_item(tree, hf_ppvideo_StunServerInfo_Port, tvb, *offset, 2, TRUE); *offset += 2;
        }
    }
}

void dissect_ppvideo_PlayHistoryItem(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    if (tree)
    {
		guint16 video_name_length;
        proto_item *play_history_info_item;
        proto_tree *play_history_info_tree;

        play_history_info_item = proto_tree_add_item(tree, hf_ppvideo_PlayHistoryItem, tvb, *offset, -1, FALSE);
        play_history_info_tree = proto_item_add_subtree(play_history_info_item, ett_ppvideo_play_history_item);

        video_name_length = tvb_get_letohs(tvb, *offset);
		proto_tree_add_item(play_history_info_tree, hf_ppvideo_VideoNameLength, tvb, *offset, 2, TRUE); *offset += 2;
		proto_tree_add_item(play_history_info_tree, hf_ppvideo_VideoName, tvb, *offset, video_name_length, TRUE); *offset += video_name_length;
		proto_tree_add_item(play_history_info_tree, hf_ppvideo_ContinuouslyPlayedDuration, tvb, *offset, 4, TRUE); *offset += 4;
		proto_tree_add_item(play_history_info_tree, hf_ppvideo_DownloadedSegmentNum, tvb, *offset, 2, TRUE); *offset += 2;
    }
}

void dissect_ppvideo_PlayHistoryItems(tvbuff_t *tvb, packet_info* pinfo, proto_tree *tree, guint * offset, guint32 play_history_item_count)
{
    if (tree && play_history_item_count > 0)
    {
        proto_item *play_history_infos_item;
        proto_tree *play_history_infos_tree;
        guint32 i;

        play_history_infos_item = proto_tree_add_item(tree, hf_ppvideo_PlayHistoryItems, tvb, *offset, -1, FALSE);
        play_history_infos_tree = proto_item_add_subtree(play_history_infos_item, ett_ppvideo_play_history_items);

        for (i = 0; i < play_history_item_count; i++)
        {
            dissect_ppvideo_PlayHistoryItem(tvb, pinfo, play_history_infos_tree, offset);
        }
    }
}

void dissect_ppvideo_PushTaskItem(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    if (tree)
    {
		guint16 url_length, refer_length, channel_id_length;
		
        proto_item *push_task_item;
        proto_tree *push_task_tree;

        push_task_item = proto_tree_add_item(tree, hf_ppvideo_PushTaskItem, tvb, *offset, -1, FALSE);
        push_task_tree = proto_item_add_subtree(push_task_item, ett_ppvideo_push_task_item);

        dissect_ppvideo_RidInfo(tvb, pinfo, tree, offset);
			
        url_length = tvb_get_letohs(tvb, *offset);
		proto_tree_add_item(push_task_tree, hf_ppvideo_UrlInfo_UrlLength, tvb, *offset, 2, TRUE); *offset += 2;
		proto_tree_add_item(push_task_tree, hf_ppvideo_UrlInfo_UrlString, tvb, *offset, url_length, TRUE); *offset += url_length;
		refer_length = tvb_get_letohs(tvb, *offset);
		proto_tree_add_item(push_task_tree, hf_ppvideo_UrlInfo_ReferLength, tvb, *offset, 2, TRUE); *offset += 2;
		proto_tree_add_item(push_task_tree, hf_ppvideo_UrlInfo_ReferString, tvb, *offset, refer_length, TRUE); *offset += refer_length;
		channel_id_length = tvb_get_letohs(tvb, *offset);
		proto_tree_add_item(push_task_tree, hf_ppvideo_ChannelIDLength, tvb, *offset, 2, TRUE); *offset += 2;
		proto_tree_add_item(push_task_tree, hf_ppvideo_ChannelID, tvb, *offset, channel_id_length, TRUE); *offset += channel_id_length;
    }
}

void dissect_ppvideo_PushTaskItems(tvbuff_t *tvb, packet_info* pinfo, proto_tree *tree, guint * offset, guint32 push_task_item_count)
{
 	if (tree && push_task_item_count > 0)
    {
        proto_item *push_tasks_item;
        proto_tree *push_tasks_tree;
        guint32 i;

        push_tasks_item = proto_tree_add_item(tree, hf_ppvideo_PlayTasks, tvb, *offset, -1, FALSE);
        push_tasks_tree = proto_item_add_subtree(push_tasks_item, ett_ppvideo_push_task_items);

        for (i = 0; i < push_task_item_count; i++)
        {
            dissect_ppvideo_PushTaskItem(tvb, pinfo, push_tasks_tree, offset);
        }
    }
}

void dissect_ppvideo_StunServerInfo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    if (tree)
    {
        proto_item *stun_server_info_item;
        proto_tree *stun_server_info_tree;

        stun_server_info_item = proto_tree_add_item(tree, hf_ppvideo_StunServerInfo, tvb, *offset, -1, FALSE);
        stun_server_info_tree = proto_item_add_subtree(stun_server_info_item, ett_ppvideo_stun_server_info);

        proto_tree_add_item(stun_server_info_tree, hf_ppvideo_StunServerInfo_Length, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(stun_server_info_tree, hf_ppvideo_StunServerInfo_IP, tvb, *offset, 4, TRUE); *offset += 4;
        proto_tree_add_item(stun_server_info_tree, hf_ppvideo_StunServerInfo_Port, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(stun_server_info_tree, hf_ppvideo_StunServerInfo_Type, tvb, *offset, 1, TRUE); *offset += 1;
    }
}

void dissect_ppvideo_StunServerInfos(tvbuff_t *tvb, packet_info* pinfo, proto_tree *tree, guint * offset, guint16 stun_server_count)
{
    if (tree && stun_server_count > 0)
    {
        proto_item *stun_server_infos_item;
        proto_tree *stun_server_infos_tree;
        guint16 i;

        stun_server_infos_item = proto_tree_add_item(tree, hf_ppvideo_StunServerInfos, tvb, *offset, -1, FALSE);
        stun_server_infos_tree = proto_item_add_subtree(stun_server_infos_item, ett_ppvideo_stun_server_infos);

        for (i = 0; i < stun_server_count; i++)
        {
            dissect_ppvideo_StunServerInfo(tvb, pinfo, stun_server_infos_tree, offset);
        }
    }
}

void dissect_ppvideo_SNServerInfo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    if (tree)
    {
        proto_item *sn_server_info_item;
        proto_tree *sn_server_info_tree;

        sn_server_info_item = proto_tree_add_item(tree, hf_ppvideo_SNServerInfo, tvb, *offset, -1, FALSE);
        sn_server_info_tree = proto_item_add_subtree(sn_server_info_item, ett_ppvideo_sn_server_info);

        proto_tree_add_item(sn_server_info_tree, hf_ppvideo_SNServerInfo_IP, tvb, *offset, 4, TRUE); *offset += 4;
        proto_tree_add_item(sn_server_info_tree, hf_ppvideo_SNServerInfo_Port, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(sn_server_info_tree, hf_ppvideo_SNServerInfo_Priority, tvb, *offset, 1, TRUE); *offset += 1;
    }
}

void dissect_ppvideo_SNInfos(tvbuff_t *tvb, packet_info* pinfo, proto_tree *tree, guint * offset, guint16 sn_count)
{
	if (tree && sn_count > 0)
	{
		proto_item *sn_infos_item;
        proto_tree *sn_infos_tree;
        guint16 i;
		
		sn_infos_item = proto_tree_add_item(tree, hf_ppvideo_SNInfos, tvb, *offset, -1, FALSE);
        sn_infos_tree = proto_item_add_subtree(sn_infos_item, ett_ppvideo_sn_server_infos);

        for (i = 0; i < sn_count; i++)
        {
            dissect_ppvideo_SNServerInfo(tvb, pinfo, sn_infos_tree, offset);
        }
	}
}

void dissect_ppvideo_IndexServerInfo(tvbuff_t *tvb, packet_info* pinfo, proto_tree *tree, guint *offset)
{
    if (tree)
    {
        proto_item *index_server_info_item;
        proto_tree *index_server_info_tree;

        index_server_info_item = proto_tree_add_item(tree, hf_ppvideo_IndexServerInfo, tvb, *offset, -1, FALSE);
        index_server_info_tree = proto_item_add_subtree(index_server_info_item, ett_ppvideo_index_server_info);

        proto_tree_add_item(index_server_info_tree, hf_ppvideo_IndexServerInfo_Length, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(index_server_info_tree, hf_ppvideo_IndexServerInfo_IP, tvb, *offset, 4, TRUE); *offset += 4;
        proto_tree_add_item(index_server_info_tree, hf_ppvideo_IndexServerInfo_Port, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(index_server_info_tree, hf_ppvideo_IndexServerInfo_Type, tvb, *offset, 1, TRUE); *offset += 1;
    }
}

void dissect_ppvideo_IndexServerInfos(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset, guint8 index_server_count)
{
    if (tree && index_server_count > 0)
    {
        proto_item *index_server_infos_item;
        proto_tree *index_server_infos_tree;
        guint8 i;

        index_server_infos_item = proto_tree_add_item(tree, hf_ppvideo_IndexServerInfos, tvb, *offset, -1, FALSE);
        index_server_infos_tree = proto_item_add_subtree(index_server_infos_item, ett_ppvideo_index_server_infos);

        for (i = 0; i < index_server_count; i++)
        {
            dissect_ppvideo_IndexServerInfo(tvb, pinfo, index_server_infos_tree, offset);
        }
    }
}

void dissect_ppvideo_DataCollectorInfo(tvbuff_t *tvb, packet_info* pinfo, proto_tree *tree, guint *offset)
{
    if (tree)
    {
        proto_item *data_collector_info_item;
        proto_tree *data_collector_info_tree;

        data_collector_info_item = proto_tree_add_item(tree, hf_ppvideo_DataCollectorInfo, tvb, *offset, -1, FALSE);
        data_collector_info_tree = proto_item_add_subtree(data_collector_info_item, ett_ppvideo_data_collector_info);

        proto_tree_add_item(data_collector_info_tree, hf_ppvideo_DataCollectorInfo_Length, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(data_collector_info_tree, hf_ppvideo_DataCollectorInfo_IP, tvb, *offset, 4, TRUE); *offset += 4;
        proto_tree_add_item(data_collector_info_tree, hf_ppvideo_DataCollectorInfo_Port, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(data_collector_info_tree, hf_ppvideo_DataCollectorInfo_Type, tvb, *offset, 1, TRUE); *offset += 1;
    }
}

void dissect_ppvideo_CandidatePeerInfoEx(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset, int hfindex)
{
    if (tree)
    {
        proto_item *peer_info_item;
        proto_tree *peer_info_tree;

        peer_info_item = proto_tree_add_item(tree, hfindex, tvb, *offset, -1, FALSE);
        peer_info_tree = proto_item_add_subtree(peer_info_item, ett_ppvideo_peer_info);

        proto_tree_add_item(peer_info_tree, hf_ppvideo_CandidatePeerInfo_IP, tvb, *offset, 4, TRUE); *offset += 4;
        proto_tree_add_item(peer_info_tree, hf_ppvideo_CandidatePeerInfo_UdpPort, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(peer_info_tree, hf_ppvideo_CandidatePeerInfo_PeerVersion, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(peer_info_tree, hf_ppvideo_CandidatePeerInfo_DetectedIP, tvb, *offset, 4, TRUE); *offset += 4;
        proto_tree_add_item(peer_info_tree, hf_ppvideo_CandidatePeerInfo_DetectedPort, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(peer_info_tree, hf_ppvideo_CandidatePeerInfo_StunIP, tvb, *offset, 4, TRUE); *offset += 4;
        proto_tree_add_item(peer_info_tree, hf_ppvideo_CandidatePeerInfo_StunUdpPort, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(peer_info_tree, hf_ppvideo_CandidatePeerInfo_PeerNatType, tvb, *offset, 1, TRUE); *offset += 1;
		proto_tree_add_item(peer_info_tree, hf_ppvideo_CandidatePeerInfo_UploadPriority, tvb, *offset, 1, TRUE); *offset += 1;
		proto_tree_add_item(peer_info_tree, hf_ppvideo_CandidatePeerInfo_IdleTimeInMins, tvb, *offset, 1, TRUE); *offset += 1;
		proto_tree_add_item(peer_info_tree, hf_ppvideo_CandidatePeerInfo_TrackerPriority, tvb, *offset, 1, TRUE); *offset += 1;
    }
}

void dissect_ppvideo_StringEx(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint* offset, int length_index, int string_index)
{
    if (tree)
    {
        guint16 string_length = tvb_get_letohs(tvb, *offset);
        proto_tree_add_item(tree, length_index, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(tree, string_index, tvb, *offset, string_length, TRUE); *offset += string_length;
    }
}

void dissect_ppvideo_String(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    dissect_ppvideo_StringEx(tvb, pinfo, tree, offset, hf_ppvideo_StringLength, hf_ppvideo_StringData);
}

void dissect_ppvideo_StringArray(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    if (tree)
    {
        // read count
        guint16 string_count = tvb_get_letohs(tvb, *offset);
        proto_tree_add_item(tree, hf_ppvideo_StringCount, tvb, *offset, 2, TRUE); *offset += 2;

        // add to tree
        if (string_count > 0)
        {
            guint16 i;
            proto_item *string_array_item;
            proto_tree *string_array_tree;

            string_array_item = proto_tree_add_item(tree, hf_ppvideo_StringArray, tvb, *offset, -1, FALSE);
            string_array_tree = proto_item_add_subtree(string_array_item, ett_ppvideo_stringarray);

            // add strings
            for (i = 0; i < string_count; ++i)
            {
                dissect_ppvideo_String(tvb, pinfo, string_array_tree, offset);
            }
        }
    }
}

void dissect_ppvideo_CandidatePeerInfo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    dissect_ppvideo_CandidatePeerInfoEx(tvb, pinfo, tree, offset, hf_ppvideo_CandidatePeerInfo);
}

void dissect_ppvideo_CandidatePeerInfoMine(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    dissect_ppvideo_CandidatePeerInfoEx(tvb, pinfo, tree, offset, hf_ppvideo_CandidatePeerInfoMine);
}

void dissect_ppvideo_CandidatePeerInfoHis(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    dissect_ppvideo_CandidatePeerInfoEx(tvb, pinfo, tree, offset, hf_ppvideo_CandidatePeerInfoHis);
}

void dissect_ppvideo_CandidatePeerInfos(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset, guint16 peer_count)
{
    if (tree && peer_count > 0)
    {
        proto_item *peer_infos_item;
        proto_tree *peer_infos_tree;
        guint16 i;

        peer_infos_item = proto_tree_add_item(tree, hf_ppvideo_CandidatePeerInfos, tvb, *offset, -1, FALSE);
        peer_infos_tree = proto_item_add_subtree(peer_infos_item, ett_ppvideo_peer_infos);

        for (i = 0; i < peer_count; i++)
        {
            dissect_ppvideo_CandidatePeerInfo(tvb, pinfo, peer_infos_tree, offset);
        }
    }
}

void dissect_ppvideo_FileInfo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    if (tree)
    {
        proto_item *file_info_item;
        proto_tree *file_info_tree;
        guint16 peer_count;

        file_info_item = proto_tree_add_item(tree, hf_ppvideo_FileInfo, tvb, *offset, -1, FALSE);
        file_info_tree = proto_item_add_subtree(file_info_item, ett_ppvideo_file_info);

        proto_tree_add_item(file_info_tree, hf_ppvideo_FileInfo_ResourceID, tvb, *offset, 16, TRUE); *offset += 16;
        peer_count = tvb_get_letohs(tvb, *offset);
        proto_tree_add_item(file_info_tree, hf_ppvideo_FileInfo_PeerCount, tvb, *offset, 2, TRUE); *offset += 2;

        dissect_ppvideo_CandidatePeerInfos(tvb, pinfo, file_info_tree, offset, peer_count);
    }
}

void dissect_ppvideo_PeerDownloadInfo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    if (tree)
    {
        proto_item *download_info_item;
        proto_tree *download_info_tree;

        download_info_item = proto_tree_add_item(tree, hf_ppvideo_PeerDownloadInfo, tvb, *offset, -1, FALSE);
        download_info_tree = proto_item_add_subtree(download_info_item, ett_ppvideo_download_info);

        proto_tree_add_item(download_info_tree, hf_ppvideo_PeerDownloadInfo_IsDownloading, tvb, *offset, 1, TRUE); *offset += 1;
        proto_tree_add_item(download_info_tree, hf_ppvideo_PeerDownloadInfo_OnlineTime, tvb, *offset, 4, TRUE); *offset += 4;
        proto_tree_add_item(download_info_tree, hf_ppvideo_PeerDownloadInfo_AvgDownloadSpeed, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(download_info_tree, hf_ppvideo_PeerDownloadInfo_NowDownloadSpeed, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(download_info_tree, hf_ppvideo_PeerDownloadInfo_AvgUploadSpeed, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(download_info_tree, hf_ppvideo_PeerDownloadInfo_NowUploadSpeed, tvb, *offset, 2, TRUE); *offset += 2;
		proto_tree_add_item(download_info_tree, hf_ppvideo_PeerDownloadInfo_Reserved1, tvb, *offset, 2, TRUE); *offset += 2;
		proto_tree_add_item(download_info_tree, hf_ppvideo_PeerDownloadInfo_Reserved2, tvb, *offset, 1, TRUE); *offset += 1;
    }
}

void dissect_ppvideo_SubPieceInfo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    if (tree)
    {
        proto_item *subpiece_info_item;
        proto_tree *subpiece_info_tree;

        subpiece_info_item = proto_tree_add_item(tree, hf_ppvideo_SubPieceInfo, tvb, *offset, -1, FALSE);
        subpiece_info_tree = proto_item_add_subtree(subpiece_info_item, ett_ppvideo_subpiece_info);

        proto_tree_add_item(subpiece_info_tree, hf_ppvideo_SubPieceInfo_BlockIndex, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(subpiece_info_tree, hf_ppvideo_SubPieceInfo_SubPieceIndex, tvb, *offset, 2, TRUE); *offset += 2;
    }
}

void dissect_ppvideo_LiveBufferMap(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    if (tree)
    {
		guint16 bufferMapLen;
        proto_item *live_buffer_map_item;
        proto_tree *live_buffer_map_tree;

        live_buffer_map_item = proto_tree_add_item(tree, hf_ppvideo_LiveBufferMap, tvb, *offset, -1, FALSE);
        live_buffer_map_tree = proto_item_add_subtree(live_buffer_map_item, ett_ppvideo_LiveBufferMap);
		
		proto_tree_add_item(live_buffer_map_tree, hf_ppvideo_start_piece_id, tvb, *offset, 4, TRUE); *offset += 4;	
		proto_tree_add_item(live_buffer_map_tree, hf_ppvideo_interval, tvb, *offset, 2, TRUE); *offset += 2;
		bufferMapLen = tvb_get_letohs(tvb, *offset);
        proto_tree_add_item(live_buffer_map_tree, hf_ppvideo_buffer_map_length, tvb, *offset, 2, TRUE); *offset += 2;
		proto_tree_add_item(live_buffer_map_tree, hf_ppvideo_buffer_map, tvb, *offset, bufferMapLen, TRUE);
    }
}

void dissect_ppvideo_LiveSubPieceInfo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    if (tree)
    {
        proto_item *subpiece_info_item;
        proto_tree *subpiece_info_tree;

        subpiece_info_item = proto_tree_add_item(tree, hf_ppvideo_SubPieceInfo, tvb, *offset, -1, FALSE);
        subpiece_info_tree = proto_item_add_subtree(subpiece_info_item, ett_ppvideo_subpiece_info);

        proto_tree_add_item(subpiece_info_tree, hf_ppvideo_SubPieceInfo_PieceID, tvb, *offset, 4, TRUE); *offset += 4;
        proto_tree_add_item(subpiece_info_tree, hf_ppvideo_SubPieceInfo_SubPieceIndex, tvb, *offset, 2, TRUE); *offset += 2;
    }
}

void dissect_ppvideo_ResourcePeerCountInfo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    if (tree)
    {
        proto_item *resource_peer_count_info_item;
        proto_tree *resource_peer_count_info_tree;

        resource_peer_count_info_item = proto_tree_add_item(tree, hf_ppvideo_ResourcePeerCountInfo, tvb, *offset, -1, FALSE);
        resource_peer_count_info_tree = proto_item_add_subtree(resource_peer_count_info_item, ett_ppvideo_resource_peer_count_info);

        proto_tree_add_item(resource_peer_count_info_tree, hf_ppvideo_ResourceID, tvb, *offset, 16, TRUE); *offset += 16;
        proto_tree_add_item(resource_peer_count_info_tree, hf_ppvideo_ResourcePeerCountInfo_peer_count, tvb, *offset, 4, TRUE); *offset += 4;
    }
}

void dissect_ppvideo_SubPieceInfos(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset, guint16 piece_count)
{
    if (tree && piece_count > 0)
    {
        proto_item *subpiece_infos_item;
        proto_tree *subpiece_infos_tree;
        guint16 i;

        subpiece_infos_item = proto_tree_add_item(tree, hf_ppvideo_SubPieceInfos, tvb, *offset, -1, FALSE);
        subpiece_infos_tree = proto_item_add_subtree(subpiece_infos_item, ett_ppvideo_subpiece_infos);

        for (i = 0; i < piece_count; i++)
        {
            dissect_ppvideo_SubPieceInfo(tvb, pinfo, subpiece_infos_tree, offset);
        }
    }
}

dissect_ppvideo_LiveSubPieceInfos(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset, guint8 piece_count)
{
    if (tree && piece_count > 0)
    {
        proto_item *subpiece_infos_item;
        proto_tree *subpiece_infos_tree;
        guint8 i;

        subpiece_infos_item = proto_tree_add_item(tree, hf_ppvideo_SubPieceInfos, tvb, *offset, -1, FALSE);
        subpiece_infos_tree = proto_item_add_subtree(subpiece_infos_item, ett_ppvideo_subpiece_infos);

        for (i = 0; i < piece_count; i++)
        {
            dissect_ppvideo_LiveSubPieceInfo(tvb, pinfo, subpiece_infos_tree, offset);
        }
    }
}


void dissect_ppvideo_RealIPs(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint* offset, guint8 realip_count)
{
    if (tree && realip_count > 0)
    {
        proto_item *ips_item;
        proto_tree *ips_tree;
        guint8 i;

        ips_item = proto_tree_add_item(tree, hf_ppvideo_RealIPs, tvb, *offset, -1, FALSE);
        ips_tree = proto_item_add_subtree(ips_item, ett_ppvideo_realips);

        for (i = 0; i < realip_count; i++)
        {
            proto_tree_add_item(ips_tree, hf_ppvideo_RealIP, tvb, *offset, 4, TRUE); *offset += 4;
        }
    }
}

void dissect_ppvideo_ResourceIDs(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint* offset, guint8 resource_count)
{
    if (tree && resource_count > 0)
    {
        proto_item *resources_item;
        proto_tree *resources_tree;
        guint8 i;

        resources_item = proto_tree_add_item(tree, hf_ppvideo_ResourceIDs, tvb, *offset, -1, FALSE);
        resources_tree = proto_item_add_subtree(resources_item, ett_ppvideo_resource_ids);

        for (i = 0; i < resource_count; i++)
        {
            proto_tree_add_item(resources_tree, hf_ppvideo_ResourceID, tvb, *offset, 16, TRUE); *offset += 16;
        }
    }
}

void dissect_ppvideo_ResourcePeerCountInfos(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint* offset, guint8 resource_peer_counts_info_count)
{
    if (tree && resource_peer_counts_info_count > 0)
    {
        proto_item *resource_peer_count_info_item;
        proto_tree *resource_peer_count_info_tree;
        guint8 i;

        resource_peer_count_info_item = proto_tree_add_item(tree, hf_ppvideo_ResourcePeerCountInfos, tvb, *offset, -1, FALSE);
        resource_peer_count_info_tree = proto_item_add_subtree(resource_peer_count_info_item, ett_ppvideo_resource_peer_count_infos);

        for (i = 0; i < resource_peer_counts_info_count; i++)
        {
            dissect_ppvideo_ResourcePeerCountInfo(tvb, pinfo, resource_peer_count_info_tree, offset);
        }
    }
}

void dissect_ppvideo_ResourceReportStruct(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint* offset)
{
    if (tree)
    {
//        proto_item *resource_report_struct_item;
//        proto_tree *resource_report_struct_tree;

//        resource_report_struct_item = proto_tree_add_item(tree, hf_ppvideo_ResourceReportStruct, tvb, *offset, -1, FALSE);
//        resource_report_struct_tree = proto_item_add_subtree(resource_report_struct_item, ett_ppvideo_report_struct);

        proto_tree_add_item(tree, hf_ppvideo_ResourceReportStruct_ResourceID, tvb, *offset, 16, TRUE); *offset += 16;
        proto_tree_add_item(tree, hf_ppvideo_ResourceReportStruct_Type, tvb, *offset, 1, TRUE); *offset += 1;
    }
}

void dissect_ppvideo_ResourceReportStructs(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint* offset, guint8 report_count)
{
    if (tree && report_count > 0)
    {
        proto_item *resource_report_structs_item;
        proto_tree *resource_report_structs_tree;
        guint8 i;

        resource_report_structs_item = proto_tree_add_item(tree, hf_ppvideo_ResourceReportStructs, tvb, *offset, -1, FALSE);
        resource_report_structs_tree = proto_item_add_subtree(resource_report_structs_item, ett_ppvideo_report_structs);

        for (i = 0; i < report_count; i++)
        {
            dissect_ppvideo_ResourceReportStruct(tvb, pinfo, resource_report_structs_tree, offset);
        }
    }
}

void dissect_ppvideo_data_DataItem(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    if (tree)
    {
        guint8 item_size;
        // item type
        proto_tree_add_item(tree, hf_ppvideo_DataOnDownloadStopEx_DataItem_Type, tvb, *offset, 1, TRUE); *offset += 1;
        // item size
        item_size = tvb_get_guint8(tvb, *offset);
        proto_tree_add_item(tree, hf_ppvideo_DataOnDownloadStopEx_DataItem_Size, tvb, *offset, 1, TRUE); *offset += 1;
        // item value
        switch (item_size)
        {
        case sizeof(guint8):
            proto_tree_add_item(tree, hf_ppvideo_DataOnDownloadStopEx_DataItem_Value_U8, tvb, *offset, item_size, TRUE); *offset += item_size;
            break;
        case sizeof(guint16):
            proto_tree_add_item(tree, hf_ppvideo_DataOnDownloadStopEx_DataItem_Value_U16, tvb, *offset, item_size, TRUE); *offset += item_size;
            break;
        case sizeof(guint32):
            proto_tree_add_item(tree, hf_ppvideo_DataOnDownloadStopEx_DataItem_Value_U32, tvb, *offset, item_size, TRUE); *offset += item_size;
            break;
        default:
            proto_tree_add_item(tree, hf_ppvideo_DataOnDownloadStopEx_DataItem_Value_Default, tvb, *offset, item_size, TRUE); *offset += item_size;
            break;
        }
    }
}


// data item array
void dissect_ppvideo_data_DataItemArray(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    //!
    if (tree)
    {
        proto_item *data_item_array_item;
        proto_tree *data_item_array_tree;
        guint8 data_items_count;
        guint32 data_items_size;
        guint8 i;

        data_items_count = tvb_get_guint8(tvb, *offset);
        data_items_size = tvb_get_letohl(tvb, *offset + sizeof(guint8));

        data_item_array_item = proto_tree_add_item(tree, hf_ppvideo_DataOnDownloadStopEx_DataItems, tvb, *offset, data_items_size + sizeof(guint8) + sizeof(guint32), FALSE);
        data_item_array_tree = proto_item_add_subtree(data_item_array_item, ett_ppvideo_data_on_download_stop_ex_item_array);

        // data item count
        proto_tree_add_item(data_item_array_tree, hf_ppvideo_DataOnDownloadStopEx_DataItemsCount, tvb, *offset, 1, TRUE); *offset += 1;

        // data size
        proto_tree_add_item(data_item_array_tree, hf_ppvideo_DataOnDownloadStopEx_DataItemsSize, tvb, *offset, 4, TRUE); *offset += 4;

        // items
        for (i = 0; i < data_items_count; ++i)
        {
            dissect_ppvideo_data_DataItem(tvb, pinfo, data_item_array_tree, offset);
        }
    }
}


// notify
void dissect_ppvideo_notify_Node(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    if (tree)
    {
        proto_tree_add_item(tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
        proto_tree_add_item(tree, hf_ppvideo_CandidatePeerInfo_IP, tvb, *offset, 4, TRUE); *offset += 4;
        proto_tree_add_item(tree, hf_ppvideo_CandidatePeerInfo_UdpPort, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(tree, hf_ppvideo_CandidatePeerInfo_DetectedIP, tvb, *offset, 4, TRUE); *offset += 4;
        proto_tree_add_item(tree, hf_ppvideo_CandidatePeerInfo_DetectedPort, tvb, *offset, 2, TRUE); *offset += 2;
        proto_tree_add_item(tree, hf_ppvideo_CandidatePeerInfo_StunIP, tvb, *offset, 4, TRUE); *offset += 4;
        proto_tree_add_item(tree, hf_ppvideo_CandidatePeerInfo_StunUdpPort, tvb, *offset, 2, TRUE); *offset += 2;
    }
}


void dissect_ppvideo_notify_NodeInfo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset, guint8 node_count)
{
    if (tree && node_count > 0)
    {
        proto_item *node_item;
        proto_tree *node_tree;
        guint8 i;

        node_item = proto_tree_add_item(tree, hf_ppvideo_notify_NodeInfo, tvb, *offset, -1, FALSE);
        node_tree = proto_item_add_subtree(node_item, ett_ppvideo_notify_nodeinfo);

        for (i=0; i<node_count; i++)
        {
            dissect_ppvideo_notify_Node(tvb, pinfo, node_tree, offset);
        }
    }
}

void dissect_ppvideo_notify_Task(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    proto_tree_add_item(tree, hf_ppvideo_notify_TaskID, tvb, *offset, 4, TRUE); *offset += 4;
    proto_tree_add_item(tree, hf_ppvideo_notify_TaskComplete, tvb, *offset, 4, TRUE); *offset += 4;
}

void dissect_ppvideo_notify_TaskInfo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset, guint16 task_num)
{
    if (tree && task_num > 0)
    {
        proto_item *task_item;
        proto_tree *task_tree;
        guint16 i;

        task_item = proto_tree_add_item(tree, hf_ppvideo_notify_TaskInfo, tvb, *offset, -1, FALSE);
        task_tree = proto_item_add_subtree(task_item, ett_ppvideo_notify_taskinfo);

        for (i=0; i<task_num; i++)
        {
            dissect_ppvideo_notify_Task(tvb, pinfo, task_tree, offset);
        }
    }
}

// play stop info array
void dissect_ppvideo_data_PlayStopInfoArray(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset)
{
    //!
    if (tree)
    {
    }
}

#define COPY_ADDRESS_OVERRIDE(to, from) { \
    if ((to)->len < (from)->len) { \
        g_free((to)->data); \
        (to)->data = g_malloc((from)->len); \
    } \
    (to)->type = (from)->type; \
    (to)->len = (from)->len; \
    memcpy((to)->data, (from)->data, (from)->len); \
}

inline void update_address(address* addr, const address* update)
{
    static gboolean init = FALSE;
    if (init == FALSE)
    {
        COPY_ADDRESS(addr, update); // g_malloc
        init = TRUE;
    }
    else if (ADDRESSES_EQUAL(addr, update) == FALSE)
    {
        //COPY_ADDRESS_OVERRIDE(addr, update);
        COPY_ADDRESS(addr, update);
    }
}
void set_ppvideo_protocol_column(packet_info *pinfo, const char* pname, guint8 action, gint is_request, gboolean is_Tcp)
{
    static address local_addr;

    #define PPV_IN    "<<--- "
    #define PPV_OUT   "--->> "
    #define PPV_INOUT "----- "

    if (check_col(pinfo->cinfo, COL_PROTOCOL))
    {
        col_clear(pinfo->cinfo, COL_PROTOCOL);
        col_set_str(pinfo->cinfo, COL_PROTOCOL, pname);
    }

    if (check_col(pinfo->cinfo, COL_INFO))
    {
        info[0] = 0;
        if (is_request == 1)
        {
            strcpy(info, PPV_OUT);
            update_address(&local_addr, &(pinfo->net_src));
        }
        else if (is_request == 0)
        {
            strcpy(info, PPV_IN);
            update_address(&local_addr, &(pinfo->net_dst));
        }
        else if (ADDRESSES_EQUAL(&local_addr, &(pinfo->net_src)) == TRUE)
        {
            strcpy(info, PPV_OUT);
        }
        else if (ADDRESSES_EQUAL(&local_addr, &(pinfo->net_dst)) == TRUE)
        {
            strcpy(info, PPV_IN);
        }
        else
        {
            strcpy(info, PPV_INOUT);
        }         
        if(FALSE == is_Tcp)
          strcat(info, val_to_str(action, ppvideo_protocol_names, "Unknown (0x%02x)"));
        if(TRUE == is_Tcp)
        	strcat(info, val_to_str(action, ppvideo_tcp_protocol_names, "Unknown (0x%02x)"));

        col_clear(pinfo->cinfo, COL_INFO);
        col_set_str(pinfo->cinfo, COL_INFO, info);
    }

}

gboolean is_index_action(guint8 action)
{
    return ((action > 0x10 && action < 0x30) || (action >= 0x40 && action < 0x50))
    ;
}
gboolean is_tracker_action(guint8 action)
{
    return (action > 0x30 && action < 0x40)
    ;
}
gboolean is_peer_action(guint8 action)
{
    return (action > 0x50 && action < 0x60)
    ;
}
gboolean is_peer_tcp_action(guint8 action)
{
    return (action >= 0xB0 && action <= 0xB4)
    ;
}
gboolean is_live_peer_action(guint8 action)
{
    return (action >= 0xC0 && action < 0xD0)
    ;
}
gboolean is_stun_action(guint8 action)
{
    return (action > 0x70 && action < 0x80)
    ;
}
gboolean is_notify_action(guint8 action)
{
    return (action >= 0xA0 && action < 0xB0)
    ;
}
gboolean is_data_collector_action(guint8 action)
{
    return (action > 0xB0 && action < 0xC0)
    ;
}
gboolean is_statistic_action(guint8 action)
{
    return (action > 0xD0 && action < 0xE0)
    ;
}
gboolean is_server_action(guint8 action)
{
    return is_index_action(action)
        || is_tracker_action(action)
        || is_stun_action(action)
        || is_statistic_action(action)
        || is_data_collector_action(action)
    ;
}
void dissect_ppvideo_peer_server_header(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint8 action, guint* offset, guint8 *is_request, guint8 *error_code)
{
    if (tree && is_server_action(action))
    {
        guint8 is_request_ = tvb_get_guint8(tvb, *offset);
        if (is_request) *is_request = is_request_;
        proto_tree_add_item(tree, hf_ppvideo_IsRequest, tvb, *offset, 1, TRUE); *offset += 1;

        if (is_request_)
        {
            ppvideo_peer_version = tvb_get_letohs(tvb, *offset);
            proto_tree_add_item(tree, hf_ppvideo_PeerVersion, tvb, *offset, 2, TRUE); *offset += 2;
			proto_tree_add_item(tree, hf_ppvideo_Reserve, tvb, *offset, 2, TRUE); *offset += 2;
        }
        else
        {
            guint8 error_code_ = tvb_get_guint8(tvb, *offset);
            if (error_code) *error_code = error_code_;
            proto_tree_add_item(tree, hf_ppvideo_ErrorCode, tvb, *offset, 1, TRUE); *offset += 1;
        }
    }
}

void dissect_ppvideo_timestamp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint* offset)
{
    if (tree)
    {
        guint64 timestamp;
        nstime_t time_info;

        timestamp = tvb_get_letoh64(tvb, *offset);
        time_info.secs = timestamp;
        time_info.nsecs = 0;

        proto_tree_add_time(tree, hf_ppvideo_TimeStamp, tvb, *offset, 8, &time_info);  *offset += 8;
    }
}

void dissect_ppvideo_statitistic_common_header(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint8 action, guint* offset)
{
    if (tree && is_statistic_action(action))
    {
        dissect_ppvideo_timestamp(tvb, pinfo, tree, offset);
        proto_tree_add_item(tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
        proto_tree_add_item(tree, hf_ppvideo_PeerAction, tvb, *offset, 1, TRUE); *offset += 1;
    }
}

void dissect_ppvideo_data_collector_common_header(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint8 action, guint* offset)
{
    if (tree && is_data_collector_action(action))
    {
        // version
        proto_item *kernel_version_item = NULL;
        KernelVersion* kernel_version = NULL;

        kernel_version = (KernelVersion*)tvb_get_ptr(tvb, *offset, sizeof(KernelVersion));
		kernel_version_item = proto_tree_add_item(tree, hf_ppvideo_KernelVersion, tvb, *offset, sizeof(KernelVersion), TRUE);
		proto_item_append_text(kernel_version_item, " (%d, %d, %d, %d)", 
		    kernel_version->Major, kernel_version->Minor, kernel_version->Micro, kernel_version->Extra);
		*offset += sizeof(KernelVersion);

		// PeerGuid
		proto_tree_add_item(tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
    }
}

void dissect_ppvideo_index(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint* offset, guint8 action) 
{
    proto_item *ppvideo_index_item = NULL;
    proto_tree *ppvideo_index_tree = NULL;

    if (tree)
    {
        ppvideo_index_item = proto_tree_add_item(tree, proto_ppvideo_index, tvb, *offset, -1, FALSE);
        ppvideo_index_tree = proto_item_add_subtree(ppvideo_index_item, ett_ppvideo_index);
    }

    if (ppvideo_index_tree && is_index_action(action))
    {
        guint8 is_request;
        guint8 error_code;

        dissect_ppvideo_peer_server_header(tvb, pinfo, ppvideo_index_tree, action, offset, &is_request, &error_code);
        set_ppvideo_protocol_column(pinfo, "PPV-Index", action, is_request, FALSE);

        switch (action)
        {
            case AID_QUERY_HTTP_SERVER_BY_RID:
                if (is_request)
                {
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_ResourceID, tvb, *offset, 16, TRUE); *offset += 16;
                }
                else if (!error_code)
                {
                    guint16 http_server_count;
                    
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_ResourceID, tvb, *offset, 16, TRUE); *offset += 16;

                    http_server_count = tvb_get_letohs(tvb, *offset);
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_HttpServerCount, tvb, *offset, 2, TRUE); *offset += 2;

                    dissect_ppvideo_HttpServers(tvb, pinfo, ppvideo_index_tree, offset, http_server_count);
                }
                break;
                
            case AID_QUERY_RID_BY_URL:
                if (is_request)
                {
                    guint16 url_length;
                    guint16 refer_length;
                    
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_SessionID, tvb, *offset, 4, TRUE); *offset += 4;

                    url_length = tvb_get_letohs(tvb, *offset);
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_UrlLength, tvb, *offset, 2, TRUE); *offset += 2;
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_UrlString, tvb, *offset, url_length, TRUE); *offset += url_length;

                    refer_length = tvb_get_letohs(tvb, *offset);
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_ReferLength, tvb, *offset, 2, TRUE); *offset += 2;
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_ReferString, tvb, *offset, refer_length, TRUE); *offset += refer_length;

                    if (ppvideo_peer_version >= 4)
                    {
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_Reserved, tvb, *offset, 10, TRUE); *offset += 10;
                    }
                }
                else if (!error_code)
                {
                    guint16 block_count;

                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_SessionID, tvb, *offset, 4, TRUE); *offset += 4;
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_ResourceID, tvb, *offset, 16, TRUE); *offset += 16;
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_DetectedIP, tvb, *offset, 4, TRUE); *offset += 4;
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_DetectedUdpPort, tvb, *offset, 2, TRUE); *offset += 2;
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_FileLength, tvb, *offset, 4, TRUE); *offset += 4;
                    block_count = tvb_get_letohs(tvb, *offset);
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_BlockCount, tvb, *offset, 2, TRUE); *offset += 2;
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_BlockSize, tvb, *offset, 4, TRUE); *offset += 4;

                    dissect_ppvideo_BlockMD5s(tvb, pinfo, ppvideo_index_tree, offset, block_count);

                    // PeerVersion >= 4
                    if (ppvideo_peer_version >= 4)
                    {
                        // Content MD5
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_ContentMD5, tvb, *offset, 16, TRUE); *offset += 16;
                        // Content Bytes
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_ContentBytes, tvb, *offset, 4, TRUE); *offset += 4;
                    }
                }
                break;

			case AID_QUERY_LIVE_TRACKER_LIST:
                if (is_request)
                {
                    // empty
                    if (ppvideo_peer_version >= 4)
                    {
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
                    }
                }
                else if (!error_code)
                {
                    guint16 tracker_count;
                    
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_TrackerGroupCount, tvb, *offset, 2, TRUE); *offset += 2;
                    tracker_count = tvb_get_letohs(tvb, *offset);
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_TrackerCount, tvb, *offset, 2, TRUE); *offset += 2;

                    dissect_ppvideo_TrackerInfos(tvb, pinfo, ppvideo_index_tree, offset, tracker_count);
                }
                break;
				
            case AID_QUERY_TRACKER_LIST:
                if (is_request)
                {
                    // empty
                    if (ppvideo_peer_version >= 4)
                    {
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
                    }
                }
                else if (!error_code)
                {
                    guint16 tracker_count;
                    
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_TrackerGroupCount, tvb, *offset, 2, TRUE); *offset += 2;
                    tracker_count = tvb_get_letohs(tvb, *offset);
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_TrackerCount, tvb, *offset, 2, TRUE); *offset += 2;

                    dissect_ppvideo_TrackerInfos(tvb, pinfo, ppvideo_index_tree, offset, tracker_count);
                }
                break;

            case AID_QUERY_STUN_SERVER_LIST:
                if (is_request)
                {
                    // empty
                }
                else if (!error_code)
                {
                    guint16 stun_server_count;
                    stun_server_count = tvb_get_letohs(tvb, *offset);
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_StunServerCount, tvb, *offset, 2, TRUE); *offset += 2;

                    dissect_ppvideo_StunServerInfos(tvb, pinfo, ppvideo_index_tree, offset, stun_server_count);
                }
                break;
				
			case AID_QUERY_SN_LIST:
			case AID_QUERY_VIP_SN_LIST:
				if (is_request)
				{
					
				}
				else if (!error_code)
				{
					guint16 sn_count;
					sn_count = tvb_get_letohs(tvb, *offset);
					proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_SNCount, tvb, *offset, 2, TRUE); *offset += 2;
					
					dissect_ppvideo_SNInfos(tvb, pinfo, ppvideo_index_tree, offset, sn_count);
				}
				break;
                
            case AID_ADD_RID_URL:
                if (is_request)
                {
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;

                    dissect_ppvideo_ResourceInfo(tvb, pinfo, ppvideo_index_tree, offset);

                    if (ppvideo_peer_version >= 4)
                    {
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_ContentMD5, tvb, *offset, 16, TRUE); *offset += 16;
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_ContentBytes, tvb, *offset, 4, TRUE); *offset += 4;
                    }
                }
                else if (!error_code)
                {
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_Status, tvb, *offset, 1, TRUE); *offset += 1;
                }
                break;

            case AID_QUERY_RID_BY_CONTENT:
                if (ppvideo_peer_version >= 4)
                {
                    if (is_request)
                    {
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_ContentMD5, tvb, *offset, 16, TRUE); *offset += 16;
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_ContentBytes, tvb, *offset, 4, TRUE); *offset += 4;
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_FileLength, tvb, *offset, 4, TRUE); *offset += 4;
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_Reserved, tvb, *offset, 10, TRUE); *offset += 10;
                    }
                    else if (!error_code)
                    {
                        guint16 block_count;
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_SessionID, tvb, *offset, 4, TRUE); *offset += 4;
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_ResourceID, tvb, *offset, 16, TRUE); *offset += 16;
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_DetectedIP, tvb, *offset, 4, TRUE); *offset += 4;
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_DetectedUdpPort, tvb, *offset, 2, TRUE); *offset += 2;
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_FileLength, tvb, *offset, 4, TRUE); *offset += 4;

                        block_count = tvb_get_letohs(tvb, *offset);
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_BlockCount, tvb, *offset, 2, TRUE); *offset += 2;
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_BlockSize, tvb, *offset, 4, TRUE); *offset += 4;
                        dissect_ppvideo_BlockMD5s(tvb, pinfo, ppvideo_index_tree, offset, block_count);

                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_ContentMD5, tvb, *offset, 16, TRUE); *offset += 16;
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_ContentBytes, tvb, *offset, 4, TRUE); *offset += 4;
                    }
                }
                break;

            case AID_QUERY_INDEX_SERVER_LIST:
                if (ppvideo_peer_version >= 5)
                {
                    if (is_request)
                    {
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
                    }
                    else if (!error_code)
                    {
                        guint8 index_server_count;

                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_ModIndexMap, tvb, *offset, 256, TRUE); *offset += 256;

                        index_server_count = tvb_get_guint8(tvb, *offset);
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_IndexServerCount, tvb, *offset, 1, TRUE); *offset += 1;
                        dissect_ppvideo_IndexServerInfos(tvb, pinfo, ppvideo_index_tree, offset, index_server_count);
                    }
                }
                break;

            case AID_QUERY_TESTURL_LIST:
                if (ppvideo_peer_version >= 5)
                {
                    if (is_request)
                    {
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
                    }
                    else if (!error_code)
                    {
                        dissect_ppvideo_StringArray(tvb, pinfo, ppvideo_index_tree, offset);
                    }
                }
                break;

            case AID_QUERY_PUSH_TASK:
                
                if (is_request)
                {
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
					proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_used_disk_size, tvb, *offset, 4, TRUE); *offset += 4;
					proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_upload_bandwidth_kbs, tvb, *offset, 4, TRUE); *offset += 4;
					proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_avg_upload_speed_kbs, tvb, *offset, 4, TRUE); *offset += 4;
                }
                else if (!error_code)
                {
					guint16 url_length;
					guint16 refer_length;
                    
					dissect_ppvideo_RidInfo(tvb, pinfo, ppvideo_index_tree, offset);
										
					url_length = tvb_get_letohs(tvb, *offset);
					proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_UrlInfo_UrlLength, tvb, *offset, 2, TRUE); *offset += 2;
					proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_UrlInfo_UrlString, tvb, *offset, url_length, TRUE); *offset += url_length;
					refer_length = tvb_get_letohs(tvb, *offset);
					proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_UrlInfo_ReferLength, tvb, *offset, 2, TRUE); *offset += 2;
					proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_UrlInfo_ReferString, tvb, *offset, refer_length, TRUE); *offset += refer_length;
					
					dissect_ppvideo_PushTaskParam(tvb, pinfo, ppvideo_index_tree, offset);
					
                }
				else if (error_code == 1)
				{
					proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_Push_Wait_Interval_In_Sec, tvb, *offset, 4, TRUE); *offset += 4;
				}
                break;
			
			case AID_REPORT_PUSH_TASK_COMPLETED:
				if (is_request)
				{
					proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
					dissect_ppvideo_RidInfo(tvb, pinfo, ppvideo_index_tree, offset);
				}
				else
				{
					dissect_ppvideo_RidInfo(tvb, pinfo, ppvideo_index_tree, offset);
				}
				break;
			case AID_QUERY_KEYWORD_LIST:
                if (ppvideo_peer_version >= 5)
                {
                    if (is_request)
                    {
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
                    }
                    else if (!error_code)
                    {
                        dissect_ppvideo_StringArray(tvb, pinfo, ppvideo_index_tree, offset);
                    }
                }
                break;

            case AID_QUERY_NEED_REPORT:
                if (ppvideo_peer_version >= 6)
                {
                    if (is_request)
                    {
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
                    }
                    else if (!error_code)
                    {
						guint8 need_report = tvb_get_guint8(tvb, *offset);
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_NeedReport, tvb, *offset, 1, TRUE); *offset += 1;
						if (need_report != 0)
						{
							proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_IntervalTime, tvb, *offset, 1, TRUE); *offset += 1;
						}
						
                    }
                }
                break;

            case AID_QUERY_DATA_COLLECTIOR:
                if (ppvideo_peer_version >= 6)
                {
                    if (is_request)
                    {
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
                    }
                    else if (!error_code)
                    {
                        dissect_ppvideo_DataCollectorInfo(tvb, pinfo, ppvideo_index_tree, offset);
                    }
                }
                break;

            case AID_QUERY_UPLOAD_PIC_PROBABILITY:
                if (ppvideo_peer_version >= 6)
                {
                    if (is_request)
                    {
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
                    }
                    else if (!error_code)
                    {
                        proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_UploadPicProbability, tvb, *offset, 4, TRUE); *offset += 4;
                    }
                }
                break;
				
			case AID_QUERY_SERVER_LIST:
                if (is_request)
                {
					guint8 server_type_info_count;
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
					dissect_ppvideo_RegionInfo(tvb, pinfo, ppvideo_index_tree, offset);
					
					server_type_info_count = tvb_get_guint8(tvb, *offset);
					proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_ServerTypeCount, tvb, *offset, 1, TRUE); *offset += 1;
					dissect_ppvideo_ServerTypeInfos(tvb, pinfo, ppvideo_index_tree, offset, server_type_info_count);
                }
                else if (!error_code)
                {
					guint8 server_list_count;
                    dissect_ppvideo_RegionInfo(tvb, pinfo, ppvideo_index_tree, offset);
					
					server_list_count = tvb_get_guint8(tvb, *offset);
					proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_ServerListCount, tvb, *offset, 1, TRUE); *offset += 1;
					dissect_ppvideo_ServerLists(tvb, pinfo, ppvideo_index_tree, offset, server_list_count);
                }
                break;
				
			case AID_QUERY_CACHE_URL_BY_RID:
                if (is_request)
                {
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
					dissect_ppvideo_RidInfo(tvb, pinfo, ppvideo_index_tree, offset);
					dissect_ppvideo_UrlInfo(tvb, pinfo, ppvideo_index_tree, offset);
										
                }
                else if (!error_code)
                {
                    guint16 notify_server_count;
                    notify_server_count = tvb_get_letohs(tvb, *offset);
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_StunServerCount, tvb, *offset, 2, TRUE); *offset += 2;
                    dissect_ppvideo_NotifyServerInfos(tvb, pinfo, ppvideo_index_tree, offset, notify_server_count);
                }
				break;
            case AID_QUERY_NOTIFY_SERVER_LIST:
                if (is_request)
                {
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
                }
                else if (!error_code)
                {
                    guint16 notify_server_count;
                    notify_server_count = tvb_get_letohs(tvb, *offset);
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_StunServerCount, tvb, *offset, 2, TRUE); *offset += 2;
                    dissect_ppvideo_NotifyServerInfos(tvb, pinfo, ppvideo_index_tree, offset, notify_server_count);
                }
				break;
				
			case AID_QUERY_CONFIG_STRING:
				if (!is_request)
				{
					guint16 length;
					length = tvb_get_letohs(tvb, *offset);
					proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_ConfigStringLength, tvb, *offset, 2, TRUE); *offset += 2;
					proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_ConfigString, tvb, *offset, length, TRUE); *offset += length;
				}
				break;			

			case AID_QUERY_PUSH_TASK_V2:
				if (is_request)
				{
					guint32 play_history_item_count;
					proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
					proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_UsedDiskSize, tvb, *offset, 4, TRUE); *offset += 4;
					proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_UploadBandWidthKBs, tvb, *offset, 4, TRUE); *offset += 4;
					proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_AvgUploadSpeedKBs, tvb, *offset, 4, TRUE); *offset += 4;
					play_history_item_count = tvb_get_letohl(tvb, *offset);
					proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_PlayHistoryItemCount, tvb, *offset, 4, TRUE); *offset += 4;
					dissect_ppvideo_PlayHistoryItems(tvb, pinfo, ppvideo_index_tree, offset, play_history_item_count);
				}
				else if (!error_code)
				{
					guint32 push_task_item_count;
					push_task_item_count = tvb_get_letohl(tvb, *offset);
					proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_PushTaskItemCount, tvb, *offset, 4, TRUE); *offset += 4;
					dissect_ppvideo_PushTaskItems(tvb, pinfo, ppvideo_index_tree, offset, push_task_item_count);
					dissect_ppvideo_PushTaskParam(tvb, pinfo, ppvideo_index_tree, offset);
				}
				else
				{
					proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_PushWaitIntervalInSeconds, tvb, *offset, 4, TRUE); *offset += 4;
				}
				break;
			case AID_QUERY_TRACKER_FOR_LISTING:
			
				proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_TrackerType, tvb, *offset, 1, TRUE); *offset += 1;
				
				if (is_request)
                {
                    // proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
                }
                else if (!error_code)
                {
                    guint16 tracker_count;
                    
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_TrackerGroupCount, tvb, *offset, 2, TRUE); *offset += 2;
                    tracker_count = tvb_get_letohs(tvb, *offset);
                    proto_tree_add_item(ppvideo_index_tree, hf_ppvideo_TrackerCount, tvb, *offset, 2, TRUE); *offset += 2;

                    dissect_ppvideo_TrackerInfos(tvb, pinfo, ppvideo_index_tree, offset, tracker_count);
                }
                break;
            default:
                break;
        }
    }

}

void dissect_ppvideo_tracker(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint* offset, guint8 action) 
{
    proto_item *ppvideo_tracker_item = NULL;
    proto_tree *ppvideo_tracker_tree = NULL;

    if (tree)
    {
        ppvideo_tracker_item = proto_tree_add_item(tree, proto_ppvideo_tracker, tvb, *offset, -1, FALSE);
        ppvideo_tracker_tree = proto_item_add_subtree(ppvideo_tracker_item, ett_ppvideo_tracker);
    }

    if (ppvideo_tracker_tree && is_tracker_action(action))
    {
        guint8 is_request;
        guint8 error_code;

		if (action == AID_INTERNAL_COMMAND)
		{
			set_ppvideo_protocol_column(pinfo, "PPV-Tracker", action, -1, FALSE);
		}
		else
		{
			dissect_ppvideo_peer_server_header(tvb, pinfo, ppvideo_tracker_tree, action, offset, &is_request, &error_code);
			set_ppvideo_protocol_column(pinfo, "PPV-Tracker", action, is_request, FALSE);
		}

        switch (action)
        {
            case AID_LIST:
            case AID_LIST_WITH_IP:
			case AID_LIST_TCP:
			case AID_LIST_TCP_WITH_IP_PACKET:
                if (is_request)
                {
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_ResourceID, tvb, *offset, 16, TRUE); *offset += 16;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_RequestPeerCount, tvb, *offset, 2, TRUE); *offset += 2;
					if (action == AID_LIST_TCP_WITH_IP_PACKET || action == AID_LIST_WITH_IP )
					{
						proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_RequestIP, tvb, *offset, 4, TRUE); *offset += 4;
					}
                }
                else if (!error_code)
                {
                    guint16 peer_count;

                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_ResourceID, tvb, *offset, 16, TRUE); *offset += 16;
                    peer_count = tvb_get_letohs(tvb, *offset);
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_CandidatePeerCount, tvb, *offset, 2, TRUE); *offset += 2;

                    dissect_ppvideo_CandidatePeerInfos(tvb, pinfo, ppvideo_tracker_tree, offset, peer_count);
                }
                break;
                
            case AID_COMMIT:
                if (is_request)
                {
                    guint8 realip_count;
                    guint8 resource_count;

                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_UdpPort, tvb, *offset, 2, TRUE); *offset += 2;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_TcpPort, tvb, *offset, 2, TRUE); *offset += 2;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_StunPeerIP, tvb, *offset, 4, TRUE); *offset += 4;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_StunPeerUdpPort, tvb, *offset, 2, TRUE); *offset += 2;
					
					realip_count = tvb_get_guint8(tvb, *offset);
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_RealIPCount, tvb, *offset, 1, TRUE); *offset += 1;
                    dissect_ppvideo_RealIPs(tvb, pinfo, ppvideo_tracker_tree, offset, realip_count);

                    resource_count = tvb_get_guint8(tvb, *offset);
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_ResourceCount, tvb, *offset, 1, TRUE);*offset += 1;
                    dissect_ppvideo_ResourceIDs(tvb, pinfo, ppvideo_tracker_tree, offset, resource_count);

                    if (ppvideo_peer_version >= 2) {
                        proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_StunDetectedUdpPort, tvb, *offset, 2, TRUE); *offset += 2;
                    }
                }
                else if (!error_code)
                {
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_KeepAliveInterval, tvb, *offset, 2, TRUE); *offset += 2;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_DetectedIP, tvb, *offset, 4, TRUE); *offset += 4;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_DetectedUdpPort, tvb, *offset, 2, TRUE); *offset += 2;
                }
                break;

            case AID_REPORT:
                if (is_request)
                {
                    guint8 realip_count;
                    guint8 report_count;

                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_LocalResourceCount, tvb, *offset, 2, TRUE); *offset += 2;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_ServerResourceCount, tvb, *offset, 2, TRUE); *offset += 2;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_NatType, tvb, *offset, 1, TRUE); *offset += 1;
					proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_UploadPriority, tvb, *offset, 1, TRUE); *offset += 1;
					proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_IdleTimeInMins, tvb, *offset, 1, TRUE); *offset += 1;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_Report_Reserved, tvb, *offset, 1, TRUE); *offset += 1;
					proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_InternalTcpPort, tvb, *offset, 2, TRUE); *offset += 2;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_UdpPort, tvb, *offset, 2, TRUE); *offset += 2;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_TcpPort, tvb, *offset, 2, TRUE); *offset += 2;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_StunPeerIP, tvb, *offset, 4, TRUE); *offset += 4;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_StunPeerUdpPort, tvb, *offset, 2, TRUE); *offset += 2;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_StunDetectedUdpPort, tvb, *offset, 2, TRUE); *offset += 2;
					if (ppvideo_peer_version >= 0x106)
					{
						proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_UploadBWKbs, tvb, *offset, 4, TRUE); *offset += 4;
						proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_UploadLimitKbs, tvb, *offset, 4, TRUE); *offset += 4;
						proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_UploadSpeedKbs, tvb, *offset, 4, TRUE); *offset += 4;
					}

                    realip_count = tvb_get_guint8(tvb, *offset);
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_RealIPCount, tvb, *offset, 1, TRUE); *offset += 1;
                    dissect_ppvideo_RealIPs(tvb, pinfo, ppvideo_tracker_tree, offset, realip_count);

                    report_count = tvb_get_guint8(tvb, *offset);
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_ReportResourceCount, tvb, *offset, 1, TRUE); *offset += 1;
                    dissect_ppvideo_ResourceReportStructs(tvb, pinfo, ppvideo_tracker_tree, offset, report_count);
					if (ppvideo_peer_version >= 0x010b)
					{
						proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_StunDetectedIp, tvb, *offset, 4, TRUE); *offset += 4;
					}
                }
                else if (!error_code)
                {
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_KeepAliveInterval, tvb, *offset, 2, TRUE); *offset += 2;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_DetectedIP, tvb, *offset, 4, TRUE); *offset += 4;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_DetectedUdpPort, tvb, *offset, 2, TRUE); *offset += 2;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_ResourceCount, tvb, *offset, 2, TRUE); *offset += 2;
                }
                break;
                
            case AID_KEEP_ALIVE:
                if (is_request)
                {
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_StunPeerIP, tvb, *offset, 4, TRUE); *offset += 4;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_StunPeerUdpPort, tvb, *offset, 2, TRUE); *offset += 2;
                    if (ppvideo_peer_version >= 2) 
                    {
                        proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_StunDetectedUdpPort, tvb, *offset, 2, TRUE); *offset += 2;
                    }
                }
                else if (!error_code)
                {
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_KeepAliveInterval, tvb, *offset, 2, TRUE); *offset += 2;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_DetectedIP, tvb, *offset, 4, TRUE); *offset += 4;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_DetectedUdpPort, tvb, *offset, 2, TRUE); *offset += 2;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_ResourceCount, tvb, *offset, 2, TRUE); *offset += 2; //! 长度跟COMMIT中的不一样
                }
                break;

            case AID_LEAVE:
                if (is_request)
                {
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
                }
                else if (!error_code)
                {
                    // no such packet
                }
                break;

            case AID_QUERY_PEER_COUNT:
                if (is_request)
                {
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_ResourceID, tvb, *offset, 16, TRUE); *offset += 16;
                }
                else if (!error_code)
                {
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_ResourceID, tvb, *offset, 16, TRUE); *offset += 16;
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_ResourcePeerCount, tvb, *offset, 4, TRUE); *offset += 4;
                }
                break;
			case AID_QUERY_BATCH_PEER_COUNT:
				if (is_request)
                {
					guint8 resource_count = tvb_get_guint8(tvb, *offset);
					proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_ResourceCount, tvb, *offset, 1, TRUE);*offset += 1;
					dissect_ppvideo_ResourceIDs(tvb, pinfo, ppvideo_tracker_tree, offset, resource_count);
                }
                else if (!error_code)
                {
					guint8 resource_peer_counts_info_count = tvb_get_guint8(tvb, *offset);
                    proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_ResourcePeerCountInfoCount, tvb, *offset, 1, TRUE); *offset += 1;
                    dissect_ppvideo_ResourcePeerCountInfos(tvb, pinfo, ppvideo_tracker_tree, offset, resource_peer_counts_info_count);
                }
                break;
			case AID_INTERNAL_COMMAND:
				proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_MagicNumber, tvb, *offset, 2, TRUE); *offset += 2;
				proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_CommandID, tvb, *offset, 2, TRUE); *offset += 2;
				proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_InternalCommandReserve, tvb, *offset, 4, TRUE); *offset += 4;
				break;
			case AID_QUERY_PEER_RESOURCE:
				if (is_request)
				{
					proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
				}
				else
				{
					guint8 resource_count;
					proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
					resource_count = tvb_get_guint8(tvb, *offset);
					proto_tree_add_item(ppvideo_tracker_tree, hf_ppvideo_ResourceCount, tvb, *offset, 1, TRUE);*offset += 1;
					dissect_ppvideo_ResourceIDs(tvb, pinfo, ppvideo_tracker_tree, offset, resource_count);
				}
        }
    }
    
}

void dissect_ppvideo_stun(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset, guint8 action)
{
    proto_item *ppvideo_stun_item = NULL;
    proto_tree *ppvideo_stun_tree = NULL;

    if (tree)
    {
        ppvideo_stun_item = proto_tree_add_item(tree, proto_ppvideo_stun, tvb, *offset, -1, FALSE);
        ppvideo_stun_tree = proto_item_add_subtree(ppvideo_stun_item, ett_ppvideo_stun);
    }

    if (ppvideo_stun_tree && is_stun_action(action))
    {
        guint8 is_request;
        guint8 error_code;

        dissect_ppvideo_peer_server_header(tvb, pinfo, ppvideo_stun_tree, action, offset, &is_request, &error_code);
        set_ppvideo_protocol_column(pinfo, "PPV-Stun", action, is_request + 2, FALSE); // trick, use address to determine direction

        switch (action)
        {
        case AID_STUN_HANDSHAKE:
            if (is_request)
            {
                // empty
            }
            else if (!error_code)
            {
                proto_tree_add_item(ppvideo_stun_tree, hf_ppvideo_KeepAliveInterval, tvb, *offset, 2, TRUE); *offset += 2;
                if (ppvideo_peer_version >= 2) 
                {
                    proto_tree_add_item(ppvideo_stun_tree, hf_ppvideo_DetectedIP, tvb, *offset, 4, TRUE); *offset += 4;
                    proto_tree_add_item(ppvideo_stun_tree, hf_ppvideo_DetectedUdpPort, tvb, *offset, 2, TRUE); *offset += 2;
                }
            }
            break;
        case AID_STUN_KEEP_ALIVE:
            if (is_request)
            {
                // empty
            }
            else if (!error_code)
            {
                // no response
            }
            break;
        case AID_STUN_INVOKE:
            if (is_request)
            {
                proto_tree_add_item(ppvideo_stun_tree, hf_ppvideo_ResourceID, tvb, *offset, 16, TRUE); *offset += 16;
                proto_tree_add_item(ppvideo_stun_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
                proto_tree_add_item(ppvideo_stun_tree, hf_ppvideo_SendOffTime, tvb, *offset, 4, TRUE); *offset += 4;
                dissect_ppvideo_CandidatePeerInfoMine(tvb, pinfo, ppvideo_stun_tree, offset);
                //proto_tree_add_item(ppvideo_stun_tree, hf_ppvideo_PeerVersionMine, tvb, *offset, 4, TRUE); *offset += 4;
                proto_tree_add_item(ppvideo_stun_tree, hf_ppvideo_PeerTypeMine, tvb, *offset, 1, TRUE); *offset += 1;
                dissect_ppvideo_CandidatePeerInfoHis(tvb, pinfo, ppvideo_stun_tree, offset);
                dissect_ppvideo_PeerDownloadInfo(tvb, pinfo, ppvideo_stun_tree, offset);
            }
            else if (!error_code)
            {
                // no response
            }
            break;
        }
    }
}

void dissect_ppvideo_notify(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint *offset, guint8 action)
{
    proto_item *ppvideo_notify_item = NULL;
    proto_tree *ppvideo_notify_tree = NULL;

    if (tree)
    {
        ppvideo_notify_item = proto_tree_add_item(tree, proto_ppvideo_notify, tvb, *offset, -1, FALSE);
        ppvideo_notify_tree = proto_item_add_subtree(ppvideo_notify_item, ett_ppvideo_notify);
    }

    if (ppvideo_notify_tree)
    {
        set_ppvideo_protocol_column(pinfo, "PPV-Notify", action, 2, FALSE);

        switch (action)
        {
        case AID_NOTIFY_JOIN_REQUEST:
            {
                proto_tree_add_item(ppvideo_notify_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
                proto_tree_add_item(ppvideo_notify_tree, hf_ppvideo_CandidatePeerInfo_IP, tvb, *offset, 4, TRUE); *offset += 4;
                proto_tree_add_item(ppvideo_notify_tree, hf_ppvideo_CandidatePeerInfo_UdpPort, tvb, *offset, 2, TRUE); *offset += 2;
                proto_tree_add_item(ppvideo_notify_tree, hf_ppvideo_CandidatePeerInfo_DetectedIP, tvb, *offset, 4, TRUE); *offset += 4;
                proto_tree_add_item(ppvideo_notify_tree, hf_ppvideo_CandidatePeerInfo_DetectedPort, tvb, *offset, 2, TRUE); *offset += 2;
                proto_tree_add_item(ppvideo_notify_tree, hf_ppvideo_CandidatePeerInfo_StunIP, tvb, *offset, 4, TRUE); *offset += 4;
                proto_tree_add_item(ppvideo_notify_tree, hf_ppvideo_CandidatePeerInfo_StunUdpPort, tvb, *offset, 2, TRUE); *offset += 2;
                proto_tree_add_item(ppvideo_notify_tree, hf_ppvideo_NatType, tvb, *offset, 2, TRUE); *offset += 2;

                break;
            }
        case AID_NOTIFY_JOIN_RESPONSE:
            {
                guint8 node_count;
                proto_tree_add_item(ppvideo_notify_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
                proto_tree_add_item(ppvideo_notify_tree, hf_ppvideo_notify_JoinRet, tvb, *offset, 1, TRUE); *offset += 1;

                node_count = tvb_get_guint8(tvb, *offset);
                proto_tree_add_item(ppvideo_notify_tree, hf_ppvideo_notify_NodeCount, tvb, *offset, 1, TRUE); *offset += 1;

                dissect_ppvideo_notify_NodeInfo(tvb, pinfo, ppvideo_notify_tree, offset, node_count);

                break;
            }
        case AID_NOTIFY_KEEPALIVE_REQUEST:
            {
                guint16 task_num;
                proto_tree_add_item(ppvideo_notify_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
                proto_tree_add_item(ppvideo_notify_tree, hf_ppvideo_notify_PeerOnline, tvb, *offset, 4, TRUE); *offset += 4;

                task_num = tvb_get_letohs(tvb, *offset);
                proto_tree_add_item(ppvideo_notify_tree, hf_ppvideo_notify_TaskNum, tvb, *offset, 2, TRUE); *offset += 2;

                dissect_ppvideo_notify_TaskInfo(tvb, pinfo, ppvideo_notify_tree, offset, task_num);
                break;
            }
        case AID_NOTIFY_KEEPALIVE_RESPONSE:
            {
                // empty
                break;
            }
        case AID_NOTIFY_TASK_REQUEST:
            {
                guint16 notify_contentlen;

                proto_tree_add_item(ppvideo_notify_tree, hf_ppvideo_notify_TaskID, tvb, *offset, 4, TRUE); *offset += 4;
                proto_tree_add_item(ppvideo_notify_tree, hf_ppvideo_notify_Duration, tvb, *offset, 2, TRUE); *offset += 2;
                proto_tree_add_item(ppvideo_notify_tree, hf_ppvideo_notify_RestTime, tvb, *offset, 4, TRUE); *offset += 4;
                proto_tree_add_item(ppvideo_notify_tree, hf_ppvideo_notify_TaskType, tvb, *offset, 2, TRUE); *offset += 2;
                notify_contentlen = tvb_get_letohs(tvb, *offset);
                proto_tree_add_item(ppvideo_notify_tree, hf_ppvideo_notify_TaskContentLen, tvb, *offset, 2, TRUE); *offset += 2;
                proto_tree_add_item(ppvideo_notify_tree, hf_ppvideo_notify_TaskContent, tvb, *offset, notify_contentlen, TRUE); *offset += notify_contentlen;
                break;
            }
        case AID_NOTIFY_TASK_RESPONSE:
            {
                proto_tree_add_item(ppvideo_notify_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
                proto_tree_add_item(ppvideo_notify_tree, hf_ppvideo_notify_TaskID, tvb, *offset, 4, TRUE); *offset += 4;
                break;
            }
        case AID_NOTIFY_LEAVE:
            {
                proto_tree_add_item(ppvideo_notify_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
                break;
            }
        default:
            break;
        }
    }
}

void dissect_ppvideo_statistic(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint* offset, guint8 action) 
{
    proto_item *ppvideo_statistic_item = NULL;
    proto_tree *ppvideo_statistic_tree = NULL;

    if (tree)
    {
        ppvideo_statistic_item = proto_tree_add_item(tree, proto_ppvideo_statistic, tvb, *offset, -1, FALSE);
        ppvideo_statistic_tree = proto_item_add_subtree(ppvideo_statistic_item, ett_ppvideo_statistic);
    }

    if (ppvideo_statistic_tree && is_statistic_action(action))
    {
        guint8 is_request;
        guint8 error_code;

        dissect_ppvideo_peer_server_header(tvb, pinfo, ppvideo_statistic_tree, action, offset, &is_request, &error_code);
        set_ppvideo_protocol_column(pinfo, "PPV-Statistic", action, is_request, FALSE);

        dissect_ppvideo_statitistic_common_header(tvb, pinfo, ppvideo_statistic_tree, action, offset);

        switch (action)
        {
        case AID_STATISTIC_INDEX_PEER:
            if (is_request)
            {
                // nothing
            }
            else if (!error_code)
            {
                // nothing
            }
            break;

        case AID_STATISTIC_INDEX_URL:
            if (is_request)
            {
                guint16 url_length = tvb_get_letohs(tvb, *offset);
                proto_tree_add_item(ppvideo_statistic_tree, hf_ppvideo_UrlLength, tvb, *offset, 2, TRUE); *offset += 2;
                proto_tree_add_item(ppvideo_statistic_tree, hf_ppvideo_UrlString, tvb, *offset, url_length, TRUE); *offset += url_length;
            }
            else if (!error_code)
            {
                // nothing
            }
            break;

        case AID_STATISTIC_INDEX_RESOURCE:
            if (is_request)
            {
                proto_tree_add_item(ppvideo_statistic_tree, hf_ppvideo_ResourceID, tvb, *offset, 16, TRUE); *offset += 16;
            }
            else if (!error_code)
            {
                // nothing
            }
            break;

        case AID_STATISTIC_INDEX_CONTENT:
            if (is_request)
            {
                proto_tree_add_item(ppvideo_statistic_tree, hf_ppvideo_ContentMD5, tvb, *offset, 16, TRUE); *offset += 16;
            }
            else if (!error_code)
            {
                // nothing
            }
            break;
			
		case AID_STATISTIC_STRING:
            if (is_request)
            {
				guint16 data_length = tvb_get_letohs(tvb, *offset);
                proto_tree_add_item(ppvideo_statistic_tree, hf_ppvideo_DataLength, tvb, *offset, 2, TRUE); *offset += 2;
                proto_tree_add_item(ppvideo_statistic_tree, hf_ppvideo_Data, tvb, *offset, data_length, TRUE); *offset += data_length;
            }
            else if (!error_code)
            {
                // nothing
            }
            break;
            
        default:
            break;
        }
    }
}

void dissect_request_sub_piece(tvbuff_t *tvb, packet_info *pinfo, proto_tree *ppvideo_peer_tree, guint* offset)
{
	  guint16 piece_count;
    piece_count = tvb_get_letohs(tvb, *offset);
    proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_PieceCount, tvb, *offset, 2, TRUE); *offset += 2;

    dissect_ppvideo_SubPieceInfos(tvb, pinfo, ppvideo_peer_tree, offset, piece_count);
		proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_Priority, tvb, *offset, 2, TRUE); *offset += 2;
}

void dissect_sub_piece(tvbuff_t *tvb, packet_info *pinfo, proto_tree *ppvideo_peer_tree, guint *offset)
{ 
	  guint16 subpiece_length;
    dissect_ppvideo_SubPieceInfo(tvb, pinfo, ppvideo_peer_tree, offset);
				
		subpiece_length = tvb_get_letohs(tvb, *offset);
    proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_SubPieceLength, tvb, *offset, 2, TRUE); *offset += 2;
    proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_SubPieceContent, tvb, *offset, subpiece_length, TRUE); *offset +=subpiece_length;
}

void dissect_ppvideo_peer(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint* offset, guint8 action) 
{
    proto_item *ppvideo_peer_item = NULL;
    proto_tree *ppvideo_peer_tree = NULL;

    /*if (tree)
    {
        ppvideo_peer_item = proto_tree_add_item(tree, proto_ppvideo_peer, tvb, *offset, -1, FALSE);
        ppvideo_peer_tree = proto_item_add_subtree(ppvideo_peer_item, ett_ppvideo_peer);
    }*/

    if (tree && is_peer_action(action))
    {
        // peer common header
		guint16 protocol_version;
		protocol_version = tvb_get_letohs(tvb, *offset);
		if (protocol_version >= 263 && action == AID_CONNECT)
		{
			guint8 connect_type;
			connect_type = tvb_get_guint8(tvb, *offset + 69);
			
			if (connect_type != 1 && connect_type != 2)  // 点播和notify
			{
				ppvideo_peer_item = proto_tree_add_item(tree, proto_ppvideo_peer, tvb, *offset, -1, FALSE);
				ppvideo_peer_tree = proto_item_add_subtree(ppvideo_peer_item, ett_ppvideo_peer);
			
				set_ppvideo_protocol_column(pinfo, "PPV-Peer", action, -1, FALSE);
			}
			else  // 直播peer 和 udpserver
			{
				ppvideo_peer_item = proto_tree_add_item(tree, proto_ppvideo_live_peer, tvb, *offset, -1, FALSE);
				ppvideo_peer_tree = proto_item_add_subtree(ppvideo_peer_item, ett_live_ppvideo_peer);
		
				set_ppvideo_protocol_column(pinfo, "PPV-Live-Peer", action, -1, FALSE);
			}			
		}
		else
		{
			ppvideo_peer_item = proto_tree_add_item(tree, proto_ppvideo_peer, tvb, *offset, -1, FALSE);
			ppvideo_peer_tree = proto_item_add_subtree(ppvideo_peer_item, ett_ppvideo_peer);
			
			set_ppvideo_protocol_column(pinfo, "PPV-Peer", action, -1, FALSE);
		}
			
		proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_ProtocolVersion, tvb, *offset, 2, TRUE); *offset += 2;
		if (action != 0x5C)
		{
			proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_Reserve, tvb, *offset, 2, TRUE); *offset += 2;
			proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_ResourceID, tvb, *offset, 16, TRUE); *offset += 16;
		}
		if (action != 0x5B && action != 0x5C && action != 0x5D)
		{
			proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_PeerGuid, tvb, *offset, 16, TRUE); *offset += 16;
		}

        // action
        switch (action)
        {
            case AID_ERROR:
            {
                guint16 error_info_length;
                
                proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_PeerErrorCode, tvb, *offset, 2, TRUE); *offset += 2;
                error_info_length = tvb_get_letohs(tvb, *offset);
                proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_ErrorInfoLength, tvb, *offset, 2, TRUE); *offset += 2;
                proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_ErrorInfo, tvb, *offset, error_info_length, TRUE); *offset += error_info_length;

                break;
            }
            case AID_CONNECT:
            {

                proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_BasicInfo, tvb, *offset, 1, TRUE); *offset += 1;
                //proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_BasicInfo_IsResponse, tvb, *offset, 1, TRUE);
                //proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_BasicInfo_Reserved, tvb, *offset, 1, TRUE); *offset += 1;

                proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_SendOffTime, tvb, *offset, 4, TRUE);*offset += 4;
                proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_PeerVersion, tvb, *offset, 4, TRUE); *offset += 4;

                dissect_ppvideo_CandidatePeerInfo(tvb, pinfo, ppvideo_peer_tree, offset);

                proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_ConnectType, tvb, *offset, 1, TRUE); *offset += 1;

                dissect_ppvideo_PeerDownloadInfo(tvb, pinfo, ppvideo_peer_tree, offset);
				
				proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_IpPoolSize, tvb, *offset, 2, TRUE); *offset += 2;

                break;
            }
            case AID_REQUEST_ANNOUCE:
            {
                // empty
                break;
            }
            case AID_ANNOUCE:
            {
                dissect_ppvideo_PeerDownloadInfo(tvb, pinfo, ppvideo_peer_tree, offset);

                proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_BlockMap, tvb, *offset, -1, TRUE); 

                break;
            }
            case AID_REQUEST_SUB_PIECE_OLD:
            {
                guint16 piece_count;

                piece_count = tvb_get_letohs(tvb, *offset);
                proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_PieceCount, tvb, *offset, 2, TRUE); *offset += 2;

                dissect_ppvideo_SubPieceInfos(tvb, pinfo, ppvideo_peer_tree, offset, piece_count);
                break;
            }
			case AID_REQUEST_SUB_PIECE:
            {
                dissect_request_sub_piece(tvb, pinfo, ppvideo_peer_tree, offset);

                break;
            }
            case AID_SUB_PIECE:
            {
                dissect_sub_piece(tvb, pinfo, ppvideo_peer_tree, offset);
                break;
            }
            case AID_PEER_EXCHANGE:
            {
                guint8 peer_count;

                proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_BasicInfo, tvb, *offset, 1, TRUE);
                proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_BasicInfo_IsResponse, tvb, *offset, 1, TRUE);
                proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_BasicInfo_Reserved, tvb, *offset, 1, TRUE); *offset += 1;

                peer_count = tvb_get_guint8(tvb, *offset);
                proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_PeerCount, tvb, *offset, 1, TRUE); *offset += 1;

                dissect_ppvideo_CandidatePeerInfos(tvb, pinfo, ppvideo_peer_tree, offset, peer_count);
                break;
            }
			case RID_INFO_REQUEST:
            {
                break;
            }
			case RID_INFO_RESPONSE:
            {
				guint16 block_count;
				
				proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_File_Length, tvb, *offset, 4, TRUE); *offset += 4;
				block_count = tvb_get_letohs(tvb, *offset);
				proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_Block_Count_16, tvb, *offset, 2, TRUE); *offset += 2;				
				proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_Block_Size, tvb, *offset, 4, TRUE); *offset += 4;
				
				dissect_ppvideo_BlockMD5s(tvb, pinfo, ppvideo_peer_tree, offset, block_count);		
                
				proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_Connected_Peers_Count, tvb, *offset, 1, TRUE); *offset += 1;
				proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_Active_Peers_Count, tvb, *offset, 1, TRUE); *offset += 1;
				proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_Pooled_Peers_Count, tvb, *offset, 2, TRUE); *offset += 2;
                break;
            }
			case REPORT_SPEED:
            {
				proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_Report_Speed, tvb, *offset, 4, TRUE); *offset += 4;
                break;
            }
			case REQUEST_SUBPIECE_FROM_SN:
            {
                guint16 piece_count, resource_name_length;
                
                resource_name_length = tvb_get_letohs(tvb, *offset);
                proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_ResourceNameLength, tvb, *offset, 2, TRUE); *offset += 2;
                proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_ResourceName, tvb, *offset, resource_name_length, TRUE); *offset += resource_name_length;


                piece_count = tvb_get_letohs(tvb, *offset);
                proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_PieceCount, tvb, *offset, 2, TRUE); *offset += 2;

                dissect_ppvideo_SubPieceInfos(tvb, pinfo, ppvideo_peer_tree, offset, piece_count);
				proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_Priority, tvb, *offset, 2, TRUE); *offset += 2;

                break;
            }
			
        }
    }

}

guint dissect_ppvideo_tcp_peer(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint* offset, guint8 action) 
{
    proto_item *ppvideo_peer_item = NULL;
    proto_tree *ppvideo_peer_tree = NULL;
	
	  guint32 block_count;
	
	  guint orig_offset;

    orig_offset = *offset;
	
    ppvideo_peer_item = proto_tree_add_item(tree, proto_ppvideo_peer_tcp, tvb, *offset, -1, FALSE);
    ppvideo_peer_tree = proto_item_add_subtree(ppvideo_peer_item, ett_ppvideo_peer_tcp);
		
		set_ppvideo_protocol_column(pinfo, "PPV-TCP-Peer", action, -1,TRUE);
		
		if (action != REPORT_SPEED_TCP)
		{
			proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_ResourceID, tvb, *offset, 16, TRUE); *offset += 16;
		}
        // action
        switch (action)
        {
			case AID_REQUEST_ANNOUCE_TCP:
            {
                // empty
                break;
            }
			case AID_ANNOUCE_TCP:
            {
                dissect_ppvideo_PeerDownloadInfo(tvb, pinfo, ppvideo_peer_tree, offset);
				
				        block_count = tvb_get_letohl(tvb, *offset);
				        proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_BlockCount, tvb, *offset, 4, TRUE); *offset += 4;
				
				        proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_BlockMap, tvb, *offset, (block_count + 7) / 8, TRUE); *offset += (block_count + 7) / 8;
				        break;

            }
			case AID_REQUEST_SUB_PIECE_TCP:
            {
                dissect_request_sub_piece(tvb, pinfo, ppvideo_peer_tree, offset);

                break;
            }
			
			case AID_SUB_PIECE_TCP:
            {
                dissect_sub_piece(tvb, pinfo, ppvideo_peer_tree, offset);
                break;
            }
			
			case REPORT_SPEED_TCP:
            {
				        proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_Report_Speed, tvb, *offset, 4, TRUE); *offset += 4;
                break;
            }			
        }
    
    return *offset - orig_offset;
}

void dissect_ppvideo_live_peer(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint* offset, guint8 action) 
{
    proto_item *ppvideo_live_peer_item = NULL;
    proto_tree *ppvideo_live_peer_tree = NULL;

    if (tree)
    {
        ppvideo_live_peer_item = proto_tree_add_item(tree, proto_ppvideo_live_peer, tvb, *offset, -1, FALSE);
        ppvideo_live_peer_tree = proto_item_add_subtree(ppvideo_live_peer_item, ett_live_ppvideo_peer);
    }

    if (ppvideo_live_peer_tree && is_live_peer_action(action))
    {
        // peer common header
		set_ppvideo_protocol_column(pinfo, "PPV-Live-Peer", action, -1, FALSE);
		
		proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_protocal_version, tvb, *offset, 2, TRUE); *offset += 2;
		
		if (action != AID_PEER_INFO)
		{
			proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_reversed, tvb, *offset, 2, TRUE); *offset += 2;
			proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_ResourceID, tvb, *offset, 16, TRUE); *offset += 16;
		}
		
        // action
        switch (action)
        {
			case AID_LIVE_REQUEST_ANNOUCE:
            {
				proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_request_buffermap_piece_id, tvb, *offset, 4, TRUE); *offset += 4;
				proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_upload_bandwidth, tvb, *offset, 4, TRUE); *offset += 4;
                proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_request_announce_reserved, tvb, *offset, 4, TRUE); *offset += 4;
                break;
            }
            case AID_LIVE_ANNOUCE:
            {
				guint16 i, piece_info_count;
				proto_item *ppvideo_buffermaplen_item = NULL;
				proto_tree *ppvideo_buffermaplen_tree = NULL;
				proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_RequestBlockID, tvb, *offset, 4, TRUE); *offset += 4;
				piece_info_count = tvb_get_letohs(tvb, *offset);
                proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_piece_info_count, tvb, *offset, 2, TRUE); *offset += 2;
				//proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_interval, tvb, *offset, 1, TRUE); *offset += 1;
				//proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_header_piece_id, tvb, *offset, 4, TRUE); *offset += 4;
				//proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_start_piece_info_id, tvb, *offset, 4, TRUE); *offset += 4;
				
				ppvideo_buffermaplen_item = proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_bufmapNos, tvb, *offset, -1, FALSE);
				ppvideo_buffermaplen_tree = proto_item_add_subtree(ppvideo_buffermaplen_item, ett_ppvideo_bufmaplens);
				
				for (i = 0; i < piece_info_count; ++i)
				{
					proto_tree_add_item(ppvideo_buffermaplen_tree, hf_ppvideo_subpiece_No, tvb, *offset, 2, TRUE); *offset += 2;
				}
				
				dissect_ppvideo_LiveBufferMap(tvb, pinfo, ppvideo_live_peer_tree, offset);
				
                break;
            }
            case AID_LIVE_REQUEST_SUB_PIECE:
            {
                guint8 piece_count;

				
                piece_count = tvb_get_guint8(tvb, *offset);
                proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_piece_count, tvb, *offset, 1, TRUE); *offset += 1;

                dissect_ppvideo_LiveSubPieceInfos(tvb, pinfo, ppvideo_live_peer_tree, offset, piece_count);
				proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_live_priority, tvb, *offset, 2, TRUE); *offset += 2;

                break;
            }
            case AID_LIVE_SUB_PIECE:
            {
				guint16 len;
				dissect_ppvideo_LiveSubPieceInfo(tvb, pinfo, ppvideo_live_peer_tree, offset);
				len = tvb_get_letohs(tvb, *offset);
                proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_SubPieceLength, tvb, *offset, 2, TRUE); *offset += 2;
                proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_SubPieceContent, tvb, *offset, len, TRUE);
                break;
            }
			case AID_PEER_INFO:
			{
				guint32 peer_info_length = tvb_get_letohl(tvb, *offset);
				proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_PeerInfoLength, tvb, *offset, 4, TRUE); *offset += 4;
				
				if (peer_info_length >= 1)
				{
					proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_DownloadConnectedCount, tvb, *offset, 1, TRUE); *offset += 1;
					peer_info_length -= 1;
				}
				else
				{
					return;
				}
				
				if (peer_info_length >= 1)
				{
					proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_UploadConnectedCount, tvb, *offset, 1, TRUE); *offset += 1;
					peer_info_length -= 1;
				}
				else
				{
					return;
				}
				
				if (peer_info_length >= 4)
				{
					proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_UploadSpeed, tvb, *offset, 4, TRUE); *offset += 4;
					peer_info_length -= 4;
				}
				else
				{
					return;
				}
				
				if (peer_info_length >= 4)
				{
					proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_MaxUploadSpeed, tvb, *offset, 4, TRUE); *offset += 4;
					peer_info_length -= 4;
				}
				else
				{
					return;
				}
				
				if (peer_info_length >= 4)
				{
					proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_RestPlayableTime, tvb, *offset, 4, TRUE); *offset += 4;
					peer_info_length -= 4;
				}
				else
				{
					return;
				}
				
				if (peer_info_length >= 1)
				{
					proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_LostRate, tvb, *offset, 1, TRUE); *offset += 1;
					peer_info_length -= 1;
				}
				else
				{
					return;
				}
				
				if (peer_info_length >= 1)
				{
					proto_tree_add_item(ppvideo_live_peer_tree, hf_ppvideo_RedundancyRate, tvb, *offset, 1, TRUE); *offset += 1;
					peer_info_length -= 1;
				}
				else
				{
					return;
				}
				
				break;
			}
			/*case AID_LIVE_ERROR_RESPOND:
            {
                guint16 error_info_length;
                
                proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_PeerErrorCode, tvb, *offset, 2, TRUE); *offset += 2;
                error_info_length = tvb_get_letohs(tvb, *offset);
                proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_ErrorInfoLength, tvb, *offset, 2, TRUE); *offset += 2;
                proto_tree_add_item(ppvideo_peer_tree, hf_ppvideo_ErrorInfo, tvb, *offset, error_info_length, TRUE); *offset += error_info_length;

                break;
            }*/
        }
    }
}

void dissect_ppvideo_data_collector(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint* offset, guint8 action)
{
    proto_item *ppvideo_data_collector_item = NULL;
    proto_tree *ppvideo_data_collector_tree = NULL;

    if (tree)
    {
        ppvideo_data_collector_item = proto_tree_add_item(tree, proto_ppvideo_data_collector, tvb, *offset, -1, FALSE);
        ppvideo_data_collector_tree = proto_item_add_subtree(ppvideo_data_collector_item, ett_ppvideo_data_collector);
    }

    if (ppvideo_data_collector_tree && action > 0xB0 && action < 0xC0)
    {
        guint8 is_request;
        guint8 error_code;

        dissect_ppvideo_peer_server_header(tvb, pinfo, ppvideo_data_collector_tree, action, offset, &is_request, &error_code);
        set_ppvideo_protocol_column(pinfo, "PPV-Data", action, is_request, FALSE);

        dissect_ppvideo_data_collector_common_header(tvb, pinfo, ppvideo_data_collector_tree, action, offset);

        switch (action)
        {
        case AID_DATA_ON_APP_STOP:
            if (is_request)
            {
                // data on app stop
                proto_tree_add_item(ppvideo_data_collector_tree, hf_ppvideo_DataOnAppStop_MaxUploadSpeedInKBps, tvb, *offset, 2, TRUE); *offset += 2;
                proto_tree_add_item(ppvideo_data_collector_tree, hf_ppvideo_DataOnAppStop_MaxDownloadSpeedInKBps, tvb, *offset, 2, TRUE); *offset += 2;
                proto_tree_add_item(ppvideo_data_collector_tree, hf_ppvideo_DataOnAppStop_DiskUsedSizeInMB, tvb, *offset, 2, TRUE); *offset += 2;
                proto_tree_add_item(ppvideo_data_collector_tree, hf_ppvideo_DataOnAppStop_UploadCacheHitRate, tvb, *offset, 1, TRUE); *offset += 1;
                proto_tree_add_item(ppvideo_data_collector_tree, hf_ppvideo_DataOnAppStop_UploadDataBytesInMB, tvb, *offset, 2, TRUE); *offset += 2;
                proto_tree_add_item(ppvideo_data_collector_tree, hf_ppvideo_DataOnAppStop_DownloadDataBytesInMB, tvb, *offset, 2, TRUE); *offset += 2;
                proto_tree_add_item(ppvideo_data_collector_tree, hf_ppvideo_DataOnAppStop_DownloadDurationInSec, tvb, *offset, 2, TRUE); *offset += 2;
                proto_tree_add_item(ppvideo_data_collector_tree, hf_ppvideo_DataOnAppStop_RunningDurationInMin, tvb, *offset, 2, TRUE); *offset += 2;
                proto_tree_add_item(ppvideo_data_collector_tree, hf_ppvideo_DataOnAppStop_CpuPeakValue, tvb, *offset, 1, TRUE); *offset += 1;
                proto_tree_add_item(ppvideo_data_collector_tree, hf_ppvideo_DataOnAppStop_MemPeakValue, tvb, *offset, 1, TRUE); *offset += 1;
                proto_tree_add_item(ppvideo_data_collector_tree, hf_ppvideo_DataOnAppStop_IncomingPeersCount, tvb, *offset, 2, TRUE); *offset += 2;
                // flv folder path
                dissect_ppvideo_StringEx(tvb, pinfo, ppvideo_data_collector_tree, offset, 
                    hf_ppvideo_DataOnAppStop_FlvFolderPath_Length, hf_ppvideo_DataOnAppStop_FlvFolderPath);
            }
            else if (!error_code)
            {
                // nothing
            }
            break;

        case AID_DATA_ON_DOWNLOAD_STOP:
            if (is_request)
            {
                //!
            }
            else if (!error_code)
            {
                // nothing
            }
            break;

        case AID_DATA_ON_DOWNLOAD_STOP_EX:
            if (is_request)
            {
                // flv url
                dissect_ppvideo_StringEx(tvb, pinfo, ppvideo_data_collector_tree, offset, 
                    hf_ppvideo_DataOnDownloadStopEx_FlvUrl_Length, hf_ppvideo_DataOnDownloadStopEx_FlvUrl);
                // flv refer url
                dissect_ppvideo_StringEx(tvb, pinfo, ppvideo_data_collector_tree, offset,
                    hf_ppvideo_DataOnDownloadStopEx_FlvReferUrl_Length, hf_ppvideo_DataOnDownloadStopEx_FlvReferUrl);
                // data item array
                dissect_ppvideo_data_DataItemArray(tvb, pinfo, ppvideo_data_collector_tree, offset);
                // play stop info array
                dissect_ppvideo_data_PlayStopInfoArray(tvb, pinfo, ppvideo_data_collector_tree, offset);
            }
            else if (!error_code)
            {
                // nothing
            }

        default:
            break;
        }
    }

}

void dissect_ppvideo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, gboolean is_new_version)
{
    if (tree)
    {
        guint8 action;
        guint offset;

        proto_item *ppvideo_item;
        proto_tree *ppvideo_tree;

        offset = 0;

        ppvideo_item = proto_tree_add_item(tree, proto_ppvideo, tvb, offset, -1, FALSE);
        ppvideo_tree = proto_item_add_subtree(ppvideo_item, ett_ppvideo);
		
		if (is_new_version)
		{
			proto_tree_add_item(ppvideo_tree, hf_ppvideo_NewCheckSum, tvb, offset, 4, TRUE); offset += 4;
		}
		else
		{
			proto_tree_add_item(ppvideo_tree, hf_ppvideo_CheckSum, tvb, offset, 4, TRUE); offset += 4;
		}
        action = tvb_get_guint8(tvb, offset);
        proto_tree_add_item(ppvideo_tree, hf_ppvideo_Action, tvb, offset, 1, TRUE); offset += 1;
        proto_tree_add_item(ppvideo_tree, hf_ppvideo_TransactionID, tvb, offset, 4, TRUE); offset += 4;


        if ((action > 0x10 && action < 0x30) || (action >= 0x40 && action < 0x50))
        {
            dissect_ppvideo_index(tvb, pinfo, tree, &offset, action);
        }
        else if (action > 0x30 && action < 0x40)
        {
            dissect_ppvideo_tracker(tvb, pinfo, tree, &offset, action);
        }
        else if (action > 0x50 && action <= 0x60)
        {
            dissect_ppvideo_peer(tvb, pinfo, tree, &offset, action);
        }
        else if (action > 0x70 && action < 0x80)
        {
            dissect_ppvideo_stun(tvb, pinfo, tree, &offset, action);
        }
        else if (action >= 0xA0 && action < 0xB0)
        {
            dissect_ppvideo_notify(tvb, pinfo, tree, &offset, action);
        }
        else if (action > 0xD0 && action < 0xE0) // PeerVersion >= 4
        {
            dissect_ppvideo_statistic(tvb, pinfo, tree, &offset, action);
        }
        else if (action > 0xB0 && action < 0xC0) // PeerVersion >= 6
        {
            dissect_ppvideo_data_collector(tvb, pinfo, tree, &offset, action);
        }
		else if (action >= 0xC0 && action < 0xD0) //live peer
		{
			dissect_ppvideo_live_peer(tvb, pinfo, tree, &offset, action);
		}
    }
}

gint find_DCRLF(tvbuff_t *tvb, gint offset, int len, gint *next_offset)
{
	gint eob_offset;
	gint eol_offset;
	gint et_offset;
	int linelen,i;
  gint8 dcrlf[4] = {'\r','\n','\r','\n'};

	if (len == -1)
		len = tvb_length_remaining(tvb, offset);
	/*
	 * XXX - what if "len" is still -1, meaning "offset is past the
	 * end of the tvbuff"?
	 */
	eob_offset = offset + len;
	eol_offset = offset;
	et_offset = offset;
	while( tvb_length_remaining(tvb,et_offset) > 0){
		eol_offset = tvb_find_guint8(tvb,et_offset,-1,(const guint8)'\r');
		if((eol_offset == -1) || (eol_offset + 3 >= eob_offset)){
			break;
		}
		i = 1;
		while(i < 4){
			if(tvb_get_guint8(tvb,eol_offset + i ) != dcrlf[i]){
				et_offset = eol_offset + i;
				break;
			}
			i++;
		}
		if(i == 4){
			linelen = eol_offset - offset;
			*next_offset = eol_offset + i;
			return linelen;
		}
	}
		
	return -1;
}

gboolean do_ppvideo_tcp_reassembly(tvbuff_t *tvb,packet_info *pinfo, const guint offset,guint* length)
{
	
	guint next_offset;
	guint next_offset_save;
	gint reported_length_remaining,length_remaining;//
	int linelen;
	
		next_offset = offset;
		reported_length_remaining =tvb_reported_length_remaining(tvb, next_offset);
		if(reported_length_remaining < 1) {
			pinfo->desegment_offset = next_offset;
			pinfo->desegment_len = DESEGMENT_ONE_MORE_SEGMENT;
			return FALSE;
		}
		length_remaining = tvb_length_remaining(tvb,next_offset);
			 

			/*
			 * Request one more byte if we cannot find a
			 * terminater (i.e. a CRLF).
			 */
		linelen = find_DCRLF(tvb, next_offset, -1, &next_offset);
		if (linelen == -1 && length_remaining >= reported_length_remaining) {
				/*
				 * Not enough data; ask for one more
				 * byte.
				 */
			pinfo->desegment_offset = next_offset;
			pinfo->desegment_len = DESEGMENT_ONE_MORE_SEGMENT;
			return FALSE;
		} 
		next_offset_save = next_offset;
	*length = next_offset - offset;
	return TRUE;
}

gint dissect_ppvideo_tcp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,guint offset)
{
	if(tree){
	  guint8 action;
	  guint length = 0;
	  guint dissect_length; 
	
	  proto_item *ppvideo_item;
    proto_tree *ppvideo_tree;

  
	  action = tvb_get_guint8(tvb, offset);
	  if(!is_peer_tcp_action(action)){
	  	return -2;
	  }
	  if(!do_ppvideo_tcp_reassembly(tvb,pinfo,offset,&length)){
		  return -1;
		  //we need more date to dissect
	  }
	  ppvideo_item = proto_tree_add_item(tree, proto_ppvideo_tcp, tvb, offset, -1, FALSE);
    ppvideo_tree = proto_item_add_subtree(ppvideo_item, ett_ppvideo_tcp);
	
		proto_tree_add_item(ppvideo_tree, hf_ppvideo_Tcp_Action, tvb, offset, 1, TRUE); offset += 1;
    proto_tree_add_item(ppvideo_tree, hf_ppvideo_TransactionID, tvb, offset, 4, TRUE); offset += 4;
    proto_tree_add_item(ppvideo_tree, hf_ppvideo_ProtocolVersion, tvb, offset, 2, TRUE); offset += 2;
    if(action >= 0xB0 && action <=0xB4){
    	dissect_length = dissect_ppvideo_tcp_peer(tvb, pinfo, tree, &offset, action);
    }
    if((dissect_length+11) != length){
    	gchar exra_info[1024] = {0};
    	g_snprintf(exra_info,1024," dirrerent length, dissect_length = %d , length = %d ",dissect_length, length);
    	col_append_str(pinfo->cinfo, COL_INFO, exra_info);
    }
    return ((dissect_length + 11) > length) ? (dissect_length + 11) : length ; 
  }
	return -2;
}



gboolean dissect_ppvideo_heuristic( tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree )
{
    if ( check_sum(tvb) )
    {
        dissect_ppvideo(tvb, pinfo, tree, FALSE);
        return TRUE;
    }
	else if (check_sum2(tvb))
	{
		dissect_ppvideo(tvb, pinfo, tree, TRUE);
        return TRUE;
	}
    return FALSE;
}

gboolean dissect_ppvideo_tcp_heuristic(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	guint offset=0;
	guint len;
	while(offset < tvb_reported_length(tvb)){
		len=dissect_ppvideo_tcp(tvb,pinfo,tree,offset);
		if(-2 == len){
			return FALSE;
		}
		if(-1 == len){
			break;
		}
		offset = offset + len; 
	}
	return TRUE;
}

void proto_reg_handoff_ppvideo(void)
{
    static gint inited = FALSE;
    static const char udp_port[] = "udp.port";

#ifdef DEBUG
#endif

    if (inited)
    {
//        dissector_delete(udp_port, ppvideo_index_port, ppvideo_index_handle);
//        dissector_delete(udp_port, ppvideo_tracker_port, ppvideo_tracker_handle);
//        dissector_delete(udp_port, ppvideo_peer_port, ppvideo_peer_handle);
//        dissector_delete(udp_port, ppvideo_peer_port, ppvideo_handle);
//    heur_dissector_delete("udp", dissect_ppvideo_heuristic, proto_ppvideo);

        // configuration
//        ppvideo_index_port = ppvideo_index_port_cfg;
//        ppvideo_tracker_port = ppvideo_tracker_port_cfg;
//        ppvideo_peer_port = ppvideo_peer_port_cfg;
    }
    else
    {
//        ppvideo_index_handle = create_dissector_handle(dissect_ppvideo_index, proto_ppvideo_index);
//        ppvideo_tracker_handle = create_dissector_handle(dissect_ppvideo_tracker, proto_ppvideo_tracker);
//        ppvideo_peer_handle = create_dissector_handle(dissect_ppvideo_peer, proto_ppvideo_peer);
//        ppvideo_handle = create_dissector_handle(dissect_ppvideo, proto_ppvideo);
        
        inited = TRUE;
    }

//    dissector_add(udp_port, ppvideo_index_port, ppvideo_index_handle);
//    dissector_add(udp_port, ppvideo_tracker_port, ppvideo_tracker_handle);
//    dissector_add(udp_port, ppvideo_peer_port, ppvideo_peer_handle);
//    dissector_add(udp_port, ppvideo_peer_port, ppvideo_handle);
    heur_dissector_add("udp", dissect_ppvideo_heuristic, proto_ppvideo);
    heur_dissector_add("tcp", dissect_ppvideo_tcp_heuristic, proto_ppvideo);
    

}

void proto_register_ppvideo(void)
{

//    module_t *ppvideo_module;


    proto_ppvideo = proto_register_protocol(
        "PPVideo",
        "PPVideo",
        "ppv"
    );
    
    proto_ppvideo_tcp =proto_register_protocol(
        "PPVideoTCP",
        "PPVideoTCP",
        "ppv_tcp"
    );    
    
    proto_ppvideo_index = proto_register_protocol(
        "PPVideo Index", 
        "PPVideo Index", 
        "ppv_index"
    );

    proto_ppvideo_tracker = proto_register_protocol(
        "PPVideo Tracker",
        "PPVideo Tracker",
        "ppv_tracker"
    );

    proto_ppvideo_peer = proto_register_protocol(
        "PPVideo Peer",
        "PPVideo Peer",
        "ppv_peer"
    );
	
	proto_ppvideo_peer_tcp = proto_register_protocol(
	    "PPVideo Peer TCP",
		  "PPVideo Peer TCP",
		  "ppv_peer_tcp"
	);

    proto_ppvideo_stun = proto_register_protocol(
        "PPVideo Stun",
        "PPVideo Stun",
        "ppv_stun"
    );

    proto_ppvideo_notify = proto_register_protocol(
        "PPVideo Notify",
        "PPVideo Notify",
        "ppv_notify"
    );

    proto_ppvideo_statistic = proto_register_protocol(
        "PPVideo Statistic",
        "PPVideo Statistic",
        "ppv_stat"
    );

    proto_ppvideo_data_collector = proto_register_protocol(
        "PPVideo DataCollection",
        "PPVideo DataCollection",
        "ppv_data"
    );
	
	proto_ppvideo_live_peer = proto_register_protocol(
        "PPVideo Live Peer",
        "PPVideo Live Peer",
        "ppv_live_peer"
    );

    proto_register_field_array(proto_ppvideo, hf_ppvideo, array_length(hf_ppvideo));
    proto_register_field_array(proto_ppvideo_tcp, hf_ppvideo, array_length(hf_ppvideo));
    proto_register_field_array(proto_ppvideo_index, hf_ppvideo, array_length(hf_ppvideo));
    proto_register_field_array(proto_ppvideo_tracker, hf_ppvideo, array_length(hf_ppvideo));
    proto_register_field_array(proto_ppvideo_stun, hf_ppvideo, array_length(hf_ppvideo));
    proto_register_field_array(proto_ppvideo_notify, hf_ppvideo, array_length(hf_ppvideo));
    proto_register_field_array(proto_ppvideo_peer, hf_ppvideo, array_length(hf_ppvideo));
	proto_register_field_array(proto_ppvideo_peer_tcp, hf_ppvideo, array_length(hf_ppvideo));
    proto_register_field_array(proto_ppvideo_statistic, hf_ppvideo, array_length(hf_ppvideo));
    proto_register_field_array(proto_ppvideo_data_collector, hf_ppvideo, array_length(hf_ppvideo));
	proto_register_field_array(proto_ppvideo_live_peer, hf_ppvideo, array_length(hf_ppvideo));

    proto_register_subtree_array(ett_ppvideo_all, array_length(ett_ppvideo_all));

//    ppvideo_index_module = prefs_register_protocol(proto_ppvideo_index, proto_reg_handoff_ppvideo);
//    ppvideo_tracker_module = prefs_register_protocol(proto_ppvideo_tracker, proto_reg_handoff_ppvideo);
//    ppvideo_peer_module = prefs_register_protocol(proto_ppvideo_peer, proto_reg_handoff_ppvideo);
//    ppvideo_module = prefs_register_protocol(proto_ppvideo, proto_reg_handoff_ppvideo);
/*
    prefs_register_uint_preference(
        ppvideo_module, "udp.ppvideo_port",
        "PPVideo Local UDP Port",
        "Set UDP Port for PPVideo",
        10, &ppvideo_peer_port_cfg
    );

    prefs_register_uint_preference(
        ppvideo_tracker_module, "udp.ppvideo_tracker_port",
        "PPVideo Tracker Server UDP Port",
        "Set UDP Port for Tracker Server",
        10, &ppvideo_tracker_port_cfg
    );

    prefs_register_uint_preference(
        ppvideo_peer_module, "udp.ppvideo_peer_port",
        "PPVideo Peer UDP Port",
        "Set UDP Port for Peer",
        10, &ppvideo_peer_port_cfg
    );
*/

}

