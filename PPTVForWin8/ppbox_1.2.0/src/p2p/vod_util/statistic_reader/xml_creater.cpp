#include "stdafx.h"
#include "xml_creater.h"
#include <framework/string/Convert.h>

using namespace statistic;

void XmlCreater::CreateXmlFile(const std::vector<boost::shared_ptr<StatisticsData> >& statistic_datas, const char* path)
{
    TiXmlDocument my_document;
    TiXmlElement *root_element = new TiXmlElement("StatisticsDatas");
    my_document.LinkEndChild(root_element);
    root_element->SetAttribute("version", "1.0");

    for(size_t i = 0; i < statistic_datas.size(); ++i)
    {
        TiXmlElement *ElementStatisticData = new TiXmlElement("StatisticData");
        root_element->LinkEndChild(ElementStatisticData);
        ElementStatisticData->SetAttribute("ID", i);
        WriteStatisticsDataToXmlFile(ElementStatisticData, statistic_datas[i]);
    }
    my_document.SaveFile(path);
}

template<typename T>
void XmlCreater::AddChild(TiXmlElement* parent, const string& child_name, const T & v)
{
    TiXmlElement* child = new TiXmlElement(child_name.c_str());
    parent->LinkEndChild(child);
    char tmp[100];
    sprintf(tmp, "%d", v);
    TiXmlText* content = new TiXmlText(tmp);
    child->LinkEndChild(content);
}

void XmlCreater::AddDoubleChild(TiXmlElement* parent, const string& child_name, const double & v)
{
    TiXmlElement* child = new TiXmlElement(child_name.c_str());
    parent->LinkEndChild(child);
    char tmp[100];
    sprintf(tmp, "%f", v);
    TiXmlText* content = new TiXmlText(tmp);
    child->LinkEndChild(content);
}

void XmlCreater::AddStringChild(TiXmlElement* parent, const string& child_name, const string & v)
{
    TiXmlElement* child = new TiXmlElement(child_name.c_str());
    parent->LinkEndChild(child);
    TiXmlText* content = new TiXmlText(v.c_str());
    child->LinkEndChild(content);
}

void XmlCreater::WriteSpeedInfoToXmlFile(TiXmlElement *element, SPEED_INFO speed_info, std::string name)
{
    TiXmlElement *ElementSpeedInfo = new TiXmlElement(name.c_str());
    element->LinkEndChild(ElementSpeedInfo);

    AddChild(ElementSpeedInfo, "StartTime", speed_info.StartTime);
    AddChild(ElementSpeedInfo, "TotalDownloadBytes", speed_info.TotalDownloadBytes);
    AddChild(ElementSpeedInfo, "TotalUploadBytes", speed_info.TotalUploadBytes);
    AddChild(ElementSpeedInfo, "NowDownloadSpeed", speed_info.NowDownloadSpeed);
    AddChild(ElementSpeedInfo, "NowUploadSpeed", speed_info.NowUploadSpeed);
    AddChild(ElementSpeedInfo, "MinuteDownloadSpeed", speed_info.MinuteDownloadSpeed);
    AddChild(ElementSpeedInfo, "MinuteUploadSpeed", speed_info.MinuteUploadSpeed);
    AddChild(ElementSpeedInfo, "AvgDownloadSpeed", speed_info.AvgDownloadSpeed);
    AddChild(ElementSpeedInfo, "AvgUploadSpeed", speed_info.AvgUploadSpeed);
}

void XmlCreater::WriteCandidatePeerInfoToXmlFile(TiXmlElement *element, protocol::CandidatePeerInfo LocalPeerInfo, std::string name)
{
    TiXmlElement *ElementLocalPeerInfo = new TiXmlElement(name.c_str());
    element->LinkEndChild(ElementLocalPeerInfo);

    AddChild(ElementLocalPeerInfo, "IP", LocalPeerInfo.IP);
    AddChild(ElementLocalPeerInfo, "UdpPort", LocalPeerInfo.UdpPort);
    AddChild(ElementLocalPeerInfo, "PeerVersion", LocalPeerInfo.PeerVersion);
    AddChild(ElementLocalPeerInfo, "DetectIP", LocalPeerInfo.DetectIP);
    AddChild(ElementLocalPeerInfo, "DetectUdpPort", LocalPeerInfo.DetectUdpPort);
    AddChild(ElementLocalPeerInfo, "StunIP", LocalPeerInfo.StunIP);
    AddChild(ElementLocalPeerInfo, "StunUdpPort", LocalPeerInfo.StunUdpPort);
    AddChild(ElementLocalPeerInfo, "PeerNatType", LocalPeerInfo.PeerNatType);
    AddChild(ElementLocalPeerInfo, "UploadPriority", LocalPeerInfo.UploadPriority);
    AddChild(ElementLocalPeerInfo, "IdleTimeInMins", LocalPeerInfo.IdleTimeInMins);
    AddChild(ElementLocalPeerInfo, "TrackerPriority", LocalPeerInfo.TrackerPriority);
}

void XmlCreater::WriteTrackerInfoToXmlFile(TiXmlElement *element, protocol::TRACKER_INFO TrackerInfo)
{
    TiXmlElement *ElementTrackerInfo = new TiXmlElement("TrackerInfo");
    element->LinkEndChild(ElementTrackerInfo);

    AddChild(ElementTrackerInfo, "StationNo", TrackerInfo.StationNo);
    AddChild(ElementTrackerInfo, "ModNo", TrackerInfo.ModNo);
    AddChild(ElementTrackerInfo, "IP", TrackerInfo.IP);
    AddChild(ElementTrackerInfo, "Port", TrackerInfo.Port);
    AddChild(ElementTrackerInfo, "Type", TrackerInfo.Type);
}

void XmlCreater::WriteStatisticTrackerInfoToXmlFile(TiXmlElement *element, STATISTIC_TRACKER_INFO StatisticTrackerInfo)
{
    TiXmlElement *ElementStatisticTrackerInfo = new TiXmlElement("StatisticTrackerInfo");
    element->LinkEndChild(ElementStatisticTrackerInfo);

    WriteTrackerInfoToXmlFile(ElementStatisticTrackerInfo, StatisticTrackerInfo.TrackerInfo);
    AddChild(ElementStatisticTrackerInfo, "CommitRequestCount", StatisticTrackerInfo.CommitRequestCount);
    AddChild(ElementStatisticTrackerInfo, "CommitResponseCount", StatisticTrackerInfo.CommitResponseCount);
    AddChild(ElementStatisticTrackerInfo, "KeepAliveRequestCount", StatisticTrackerInfo.KeepAliveRequestCount);
    AddChild(ElementStatisticTrackerInfo, "KeepAliveResponseCount", StatisticTrackerInfo.KeepAliveResponseCount);
    AddChild(ElementStatisticTrackerInfo, "ListRequestCount", StatisticTrackerInfo.ListRequestCount);
    AddChild(ElementStatisticTrackerInfo, "ListResponseCount", StatisticTrackerInfo.ListResponseCount);
    AddChild(ElementStatisticTrackerInfo, "LastListReturnPeerCount", StatisticTrackerInfo.LastListReturnPeerCount);
    AddChild(ElementStatisticTrackerInfo, "IsSubmitTracker", StatisticTrackerInfo.IsSubmitTracker);
    AddChild(ElementStatisticTrackerInfo, "ErrorCode", StatisticTrackerInfo.ErrorCode);
    AddChild(ElementStatisticTrackerInfo, "KeepAliveInterval", StatisticTrackerInfo.KeepAliveInterval);
}



void XmlCreater::WriteStatisticIndexInfoToXmlFile(TiXmlElement *element, STATISTIC_INDEX_INFO StatisticIndexInfo)
{
    TiXmlElement *ElementStatisticIndexInfo = new TiXmlElement("StatisticIndexInfo");
    element->LinkEndChild(ElementStatisticIndexInfo);

    AddChild(ElementStatisticIndexInfo, "IP", StatisticIndexInfo.IP);
    AddChild(ElementStatisticIndexInfo, "Port", StatisticIndexInfo.Port);
    AddChild(ElementStatisticIndexInfo, "Type", StatisticIndexInfo.Type);
    AddChild(ElementStatisticIndexInfo, "QueryRIDByUrlRequestCount", StatisticIndexInfo.QueryRIDByUrlRequestCount);
    AddChild(ElementStatisticIndexInfo, "QueryRIDByUrlResponseCount", StatisticIndexInfo.QueryRIDByUrlResponseCount);
    AddChild(ElementStatisticIndexInfo, "QueryHttpServersByRIDRequestCount", StatisticIndexInfo.QueryHttpServersByRIDRequestCount);
    AddChild(ElementStatisticIndexInfo, "QueryHttpServersByRIDResponseCount", StatisticIndexInfo.QueryHttpServersByRIDResponseCount);
    AddChild(ElementStatisticIndexInfo, "QueryTrackerListRequestCount", StatisticIndexInfo.QueryTrackerListRequestCount);
    AddChild(ElementStatisticIndexInfo, "QureyTrackerListResponseCount", StatisticIndexInfo.QureyTrackerListResponseCount);
    AddChild(ElementStatisticIndexInfo, "AddUrlRIDRequestCount", StatisticIndexInfo.AddUrlRIDRequestCount);
    AddChild(ElementStatisticIndexInfo, "AddUrlRIDResponseCount", StatisticIndexInfo.AddUrlRIDResponseCount);
}

void XmlCreater::WriteStatisticInfoToXmlFile(TiXmlElement *element, const STASTISTIC_INFO& statistic_info)
{
    TiXmlElement *ElementStatisticInfo = new TiXmlElement("StatisticInfo");
    element->LinkEndChild(ElementStatisticInfo);

    WriteSpeedInfoToXmlFile(ElementStatisticInfo, statistic_info.SpeedInfo, "SpeedInfo");
    WriteCandidatePeerInfoToXmlFile(ElementStatisticInfo, statistic_info.LocalPeerInfo, "LocalPeerInfo");

    AddChild(ElementStatisticInfo, "LocalIpCount", statistic_info.LocalIpCount);
    TiXmlElement* ElementLocalIps = new TiXmlElement("LocalIps");
    ElementStatisticInfo->LinkEndChild(ElementLocalIps);
    for (int i = 0; i < statistic_info.LocalIpCount; ++i)
    {
        AddChild(ElementLocalIps, "LocalIP", statistic_info.LocalIPs[i]);
    }
    AddChild(ElementStatisticInfo, "LocalPeerVersion", statistic_info.LocalPeerVersion);
    AddChild(ElementStatisticInfo, "Reverse", statistic_info.Reverse);
    AddChild(ElementStatisticInfo, "TrackerCount", statistic_info.TrackerCount);
    AddChild(ElementStatisticInfo, "GroupCount", statistic_info.GroupCount);
    TiXmlElement* ElementStatisticTrackerInfos = new TiXmlElement("StatisticTrackerInfos");
    ElementStatisticInfo->LinkEndChild(ElementStatisticTrackerInfos);
    for (int i = 0; i < statistic_info.TrackerCount; ++i)
    {
        WriteStatisticTrackerInfoToXmlFile(ElementStatisticTrackerInfos, statistic_info.TrackerInfos[i]);
    }
    WriteStatisticIndexInfoToXmlFile(ElementStatisticInfo, statistic_info.StatisticIndexInfo);
    AddChild(ElementStatisticInfo, "P2PDownloaderCount", statistic_info.P2PDownloaderCount);
    TiXmlElement* ElementP2PDownloaderRIDs = new TiXmlElement("P2PDownloaderRIDs");
    ElementStatisticInfo->LinkEndChild(ElementP2PDownloaderRIDs);
    for (int i = 0; i < UINT8_MAX_VALUE; ++i)
    {
        if (statistic_info.P2PDownloaderRIDs[i].is_empty())
            continue;

        AddStringChild(ElementP2PDownloaderRIDs, "P2PDownloaderRID", statistic_info.P2PDownloaderRIDs[i].to_string());
    }
    AddChild(ElementStatisticInfo, "DownloadDriverCount", statistic_info.DownloadDriverCount);
    TiXmlElement* ElementDownloadDriverIDs = new TiXmlElement("DownloadDriverIDs");
    ElementStatisticInfo->LinkEndChild(ElementDownloadDriverIDs);
    for (int i = 0; i < UINT8_MAX_VALUE; ++i)
    {
        if (statistic_info.DownloadDriverIDs[i] == 0)
            continue;

        AddChild(ElementDownloadDriverIDs, "DownloadDriverID", statistic_info.DownloadDriverIDs[i]);
    }
    AddChild(ElementStatisticInfo, "TotalP2PDownloadBytes", statistic_info.TotalP2PDownloadBytes);
    AddChild(ElementStatisticInfo, "TotalOtherServerDownloadBytes", statistic_info.TotalOtherServerDownloadBytes);
    AddChild(ElementStatisticInfo, "TotalHttpNotOriginalDataBytes", statistic_info.TotalHttpNotOriginalDataBytes);
    AddChild(ElementStatisticInfo, "TotalP2PDataBytes", statistic_info.TotalP2PDataBytes);
    AddChild(ElementStatisticInfo, "TotalHttpOriginalDataBytes", statistic_info.TotalHttpOriginalDataBytes);
    AddChild(ElementStatisticInfo, "CompleteCount", statistic_info.CompleteCount);
    AddChild(ElementStatisticInfo, "TotalUploadCacheRequestCount", statistic_info.TotalUploadCacheRequestCount);
    AddChild(ElementStatisticInfo, "TotalUploadCacheHitCount", statistic_info.TotalUploadCacheHitCount);
    AddChild(ElementStatisticInfo, "HttpProxyPort", statistic_info.HttpProxyPort);
    AddChild(ElementStatisticInfo, "MaxHttpDownloadSpeed", statistic_info.MaxHttpDownloadSpeed);
    AddChild(ElementStatisticInfo, "IncomingPeersCount", statistic_info.IncomingPeersCount);
    AddChild(ElementStatisticInfo, "DownloadDurationInSec", statistic_info.DownloadDurationInSec);
    AddChild(ElementStatisticInfo, "BandWidth", statistic_info.BandWidth);
    AddChild(ElementStatisticInfo, "GlobalWindowSize", statistic_info.GlobalWindowSize);
    AddChild(ElementStatisticInfo, "GlobalRequestSendCount", statistic_info.GlobalRequestSendCount);
    AddChild(ElementStatisticInfo, "MemoryPoolLeftSize", statistic_info.MemoryPoolLeftSize);
    AddChild(ElementStatisticInfo, "LiveDownloadDriverCount", statistic_info.LiveDownloadDriverCount);
    TiXmlElement* ElementLiveDownloadDriverIDs = new TiXmlElement("LiveDownloadDriverIDs");
    ElementStatisticInfo->LinkEndChild(ElementLiveDownloadDriverIDs);
    for (int i = 0; i < LIVEDOWNLOADER_MAX_COUNT; ++i)
    {
        if (statistic_info.LiveDownloadDriverIDs[i] == 0)
            continue;

        AddChild(ElementLiveDownloadDriverIDs, "LiveDownloadDriverID", statistic_info.LiveDownloadDriverIDs[i]);
    }
}

void XmlCreater::WritePieceInfoExToXmlFile(TiXmlElement *element, const PIECE_INFO_EX& piece_info_ex, std::string name)
{
    TiXmlElement *ElementPieceInfoEx = new TiXmlElement(name.c_str());
    element->LinkEndChild(ElementPieceInfoEx);

    AddChild(ElementPieceInfoEx, "BlockIndex", piece_info_ex.BlockIndex);
    AddChild(ElementPieceInfoEx, "PieceIndexInBlock", piece_info_ex.PieceIndexInBlock);
    AddChild(ElementPieceInfoEx, "PieceIndexInBlock", piece_info_ex.PieceIndexInBlock);
}

void XmlCreater::WriteHttpDownloadInfoToXmlFile(TiXmlElement *element, const HTTP_DOWNLOADER_INFO& http_download_info)
{
    TiXmlElement *ElementHttpDownloadInfo = new TiXmlElement("HttpDownloadStatisticInfo");
    element->LinkEndChild(ElementHttpDownloadInfo);

    std::string tmp = "";
    for (int i = 0; i < sizeof(http_download_info.Url) / sizeof(http_download_info.Url[0]);
        ++i)
    {
        tmp += http_download_info.Url[i];
    }
    AddStringChild(ElementHttpDownloadInfo, "OriginalUrl", tmp);

    tmp = "";
    for (int i = 0; i < sizeof(http_download_info.ReferUrl) / sizeof(http_download_info.ReferUrl[0]);
        ++i)
    {
        tmp += http_download_info.ReferUrl[i];
    }
    AddStringChild(ElementHttpDownloadInfo, "ReferUrl", tmp);

    tmp = "";
    for (int i = 0; i < sizeof(http_download_info.RedirectUrl) / sizeof(http_download_info.RedirectUrl[0]);
        ++i)
    {
        tmp += http_download_info.RedirectUrl[i];
    }
    AddStringChild(ElementHttpDownloadInfo, "RedirectUrl", tmp);
    WriteSpeedInfoToXmlFile(ElementHttpDownloadInfo, http_download_info.SpeedInfo, "SpeedInfo");
    WritePieceInfoExToXmlFile(ElementHttpDownloadInfo, http_download_info.DownloadingPieceEx, "DownloadingPieceEx");
    WritePieceInfoExToXmlFile(ElementHttpDownloadInfo, http_download_info.StartPieceEx, "StartPieceEx");
    AddChild(ElementHttpDownloadInfo, "LastConnectedTime", http_download_info.LastConnectedTime);
    AddChild(ElementHttpDownloadInfo, "LastRequestPieceTime", http_download_info.LastRequestPieceTime);
    AddChild(ElementHttpDownloadInfo, "LastHttpStatusCode", http_download_info.LastHttpStatusCode);
    AddChild(ElementHttpDownloadInfo, "RetryCount", http_download_info.RetryCount);
    AddChild(ElementHttpDownloadInfo, "IsSupportRange", http_download_info.IsSupportRange);
    AddChild(ElementHttpDownloadInfo, "IsDeath", http_download_info.IsDeath);
    AddChild(ElementHttpDownloadInfo, "IsPause", http_download_info.IsPause);
}

int utf16_length( char * buf )
{
  int len = 0;
  boost::uint16_t * p = (boost::uint16_t *)buf;
  while( *p != 0 ) {
    len += 2;
    if( len > 250 )
      break;
    p++;
  }

  return len;
}

void XmlCreater::WriteDownloadDriverStatisticInfoToXmlFile(TiXmlElement *element, const DOWNLOADDRIVER_STATISTIC_INFO& downloader_driver_statistic_info)
{
    TiXmlElement *ElementDownloadDriverStatisticInfo = new TiXmlElement("DownloaderDirverStatisticInfo");
    element->LinkEndChild(ElementDownloadDriverStatisticInfo);

    AddChild(ElementDownloadDriverStatisticInfo, "DownloadDriverID", downloader_driver_statistic_info.DownloadDriverID);
    WriteSpeedInfoToXmlFile(ElementDownloadDriverStatisticInfo, downloader_driver_statistic_info.SpeedInfo, "SpeedInfo");

    std::string tmp = "";
    for (int i = 0; i < sizeof(downloader_driver_statistic_info.OriginalUrl) / sizeof(downloader_driver_statistic_info.OriginalUrl[0]);
        ++i)
    {
        tmp += downloader_driver_statistic_info.OriginalUrl[i];
    }
    AddStringChild(ElementDownloadDriverStatisticInfo, "OriginalUrl", tmp);

    tmp = "";
    for (int i = 0; i < sizeof(downloader_driver_statistic_info.OriginalReferUrl) / sizeof(downloader_driver_statistic_info.OriginalReferUrl[0]);
        ++i)
    {
        tmp += downloader_driver_statistic_info.OriginalReferUrl[i];
    }
    AddStringChild(ElementDownloadDriverStatisticInfo, "OriginalReferUrl", tmp);

    AddStringChild(ElementDownloadDriverStatisticInfo, "ResourceID", downloader_driver_statistic_info.ResourceID.to_string());
    AddChild(ElementDownloadDriverStatisticInfo, "FileLength", downloader_driver_statistic_info.FileLength);
    AddChild(ElementDownloadDriverStatisticInfo, "BlockSize", downloader_driver_statistic_info.BlockSize);
    AddChild(ElementDownloadDriverStatisticInfo, "BlockCount", downloader_driver_statistic_info.BlockCount);
    AddChild(ElementDownloadDriverStatisticInfo, "TotalHttpDataBytes", downloader_driver_statistic_info.TotalHttpDataBytes);
    AddChild(ElementDownloadDriverStatisticInfo, "TotalLocalDataBytes", downloader_driver_statistic_info.TotalLocalDataBytes);

    wchar_t *wfilename = (wchar_t*)(downloader_driver_statistic_info.FileName);
    //    char *cfilename = (char *)malloc(MB_CUR_MAX);
    //    wcstombs(cfilename, wfilename, MB_CUR_MAX);
    
#ifdef BOOST_WINDOWS_API
    char utf8name[MAX_PATH] = "";
    WideCharToMultiByte(CP_UTF8, 0, wfilename, -1, utf8name, MAX_PATH, NULL, NULL);
    string filename;
    if (strlen(utf8name) == 0)
    {
        filename = "unknown";
    }
    else
    {
        filename = utf8name;
    }
#else
    iconv_t cd = iconv_open("UTF-8", "UNICODE");
    char *utf8name = (char *)malloc(255);
    memset( utf8name , 0 , 255 );
    int inlen = utf16_length((char*)wfilename);
    int outlen = 255;
    int new_outlen = outlen;
    char * new_utf8name = utf8name;
    int retv = iconv(cd, (char**)&wfilename, (size_t *)&inlen, &new_utf8name, (size_t *)&outlen); 
    iconv_close( cd );
    string filename;
    if ( strlen((char*) utf8name ) == 0 )
    {
      filename = "unknwon";
    }
    else
    {
      filename.assign( utf8name , new_outlen - outlen);
    }
#endif
    AddStringChild(ElementDownloadDriverStatisticInfo, "FileName", filename );

    AddChild(ElementDownloadDriverStatisticInfo, "IsHidden", downloader_driver_statistic_info.IsHidden);
    AddChild(ElementDownloadDriverStatisticInfo, "SourceType", downloader_driver_statistic_info.SourceType);

    tmp = "";
    for (int i = 0; i < sizeof(downloader_driver_statistic_info.WebUrl) / sizeof(downloader_driver_statistic_info.WebUrl[0]);
        ++i)
    {
        tmp += downloader_driver_statistic_info.WebUrl[i];
    }
    AddStringChild(ElementDownloadDriverStatisticInfo, "WebUrl", tmp);

    AddChild(ElementDownloadDriverStatisticInfo, "StateMachineType", downloader_driver_statistic_info.StateMachineType);

    tmp = "";
    for (int i = 0; i < sizeof(downloader_driver_statistic_info.StateMachineState) / sizeof(downloader_driver_statistic_info.StateMachineState[0]);
        ++i)
    {
        tmp += downloader_driver_statistic_info.StateMachineState[i];
    }
    AddStringChild(ElementDownloadDriverStatisticInfo, "StateMachineState", tmp);

    AddChild(ElementDownloadDriverStatisticInfo, "PlayingPosition", downloader_driver_statistic_info.PlayingPosition);
    AddChild(ElementDownloadDriverStatisticInfo, "DataRate", downloader_driver_statistic_info.DataRate);
    AddChild(ElementDownloadDriverStatisticInfo, "http_state", downloader_driver_statistic_info.http_state);
    AddChild(ElementDownloadDriverStatisticInfo, "p2p_state", downloader_driver_statistic_info.p2p_state);
    AddChild(ElementDownloadDriverStatisticInfo, "timer_using_state", downloader_driver_statistic_info.timer_using_state);
    AddChild(ElementDownloadDriverStatisticInfo, "timer_state", downloader_driver_statistic_info.timer_state);
    AddChild(ElementDownloadDriverStatisticInfo, "t", downloader_driver_statistic_info.t);
    AddChild(ElementDownloadDriverStatisticInfo, "b", downloader_driver_statistic_info.b);
    AddChild(ElementDownloadDriverStatisticInfo, "speed_limit", downloader_driver_statistic_info.speed_limit);
    AddChild(ElementDownloadDriverStatisticInfo, "HttpDownloaderCount", downloader_driver_statistic_info.HttpDownloaderCount);
    TiXmlElement* ElementHttpDownloaders = new TiXmlElement("HttpDownloaders");
    ElementDownloadDriverStatisticInfo->LinkEndChild(ElementHttpDownloaders);
    for (int i = 0; i < downloader_driver_statistic_info.HttpDownloaderCount; ++i)
    {
        WriteHttpDownloadInfoToXmlFile(ElementHttpDownloaders, downloader_driver_statistic_info.HttpDownloaders[i]);
    }
}

void XmlCreater::WritePeerDownloadInfo(TiXmlElement *element, const protocol::PEER_DOWNLOAD_INFO& peer_download_info)
{
    TiXmlElement *ElementPeerDownloadInfo = new TiXmlElement("PeerDownloadInfo");
    element->LinkEndChild(ElementPeerDownloadInfo);

    AddChild(ElementPeerDownloadInfo, "IsDownloading", peer_download_info.IsDownloading);
    AddChild(ElementPeerDownloadInfo, "OnlineTime", peer_download_info.OnlineTime);
    AddChild(ElementPeerDownloadInfo, "AvgDownload", peer_download_info.AvgDownload);
    AddChild(ElementPeerDownloadInfo, "NowDownload", peer_download_info.NowDownload);
    AddChild(ElementPeerDownloadInfo, "AvgUpload", peer_download_info.AvgUpload);
    AddChild(ElementPeerDownloadInfo, "NowUpload", peer_download_info.NowUpload);
}

void XmlCreater::WritePeerConnectionInfoToXmlFile(TiXmlElement *element, const P2P_CONNECTION_INFO& peer_connection_info)
{
    TiXmlElement *ElementPeerConnectionInfo = new TiXmlElement("PeerConnectionInfo");
    element->LinkEndChild(ElementPeerConnectionInfo);

    AddStringChild(ElementPeerConnectionInfo, "PeerGuid", peer_connection_info.PeerGuid.to_string());
    WriteSpeedInfoToXmlFile(ElementPeerConnectionInfo, peer_connection_info.SpeedInfo, "SpeedInfo");
    AddChild(ElementPeerConnectionInfo, "PeerVersion", peer_connection_info.PeerVersion);
    AddChild(ElementPeerConnectionInfo, "Reserve", peer_connection_info.Reserve);
    WriteCandidatePeerInfoToXmlFile(ElementPeerConnectionInfo, peer_connection_info.PeerInfo, "PeerInfo");
    WritePeerDownloadInfo(ElementPeerConnectionInfo, peer_connection_info.PeerDownloadInfo);

    std::string tmp = "";
    tmp.resize(8 * sizeof(peer_connection_info.BitMap) / sizeof(peer_connection_info.BitMap[0]));
    int tmp_length = 0;
    for (int i = 0; i < sizeof(peer_connection_info.BitMap) / sizeof(peer_connection_info.BitMap[0]); ++i)
    {
        for (int j = 7; j >= 0; --j)
        {
            boost::uint8_t bit = (((peer_connection_info.BitMap[i] >> j) % 2) == 1);
            if (bit == 0)
            {
                tmp[tmp_length++] = '0';
            }
            else
            {
                tmp[tmp_length++] = '1';
            }
        }
    }
    AddStringChild(ElementPeerConnectionInfo, "BitMap", tmp);
    AddChild(ElementPeerConnectionInfo, "RTT_Count", peer_connection_info.RTT_Count);
    AddChild(ElementPeerConnectionInfo, "RTT_Now", peer_connection_info.RTT_Now);
    AddChild(ElementPeerConnectionInfo, "RTT_Average", peer_connection_info.RTT_Average);
    AddChild(ElementPeerConnectionInfo, "RTT_Max", peer_connection_info.RTT_Max);
    AddChild(ElementPeerConnectionInfo, "RTT_Total", peer_connection_info.RTT_Total);
    AddChild(ElementPeerConnectionInfo, "ElapseTime", peer_connection_info.ElapseTime);
    AddChild(ElementPeerConnectionInfo, "WindowSize", peer_connection_info.WindowSize);
    AddChild(ElementPeerConnectionInfo, "AssignedSubPieceCount", peer_connection_info.AssignedSubPieceCount);
    AddChild(ElementPeerConnectionInfo, "AverageDeltaTime", peer_connection_info.AverageDeltaTime);
    AddChild(ElementPeerConnectionInfo, "SortedValue", peer_connection_info.SortedValue);
    AddChild(ElementPeerConnectionInfo, "IsRidInfoValid", peer_connection_info.IsRidInfoValid);
    AddChild(ElementPeerConnectionInfo, "Sent_Count", peer_connection_info.Sent_Count);
    AddChild(ElementPeerConnectionInfo, "Requesting_Count", peer_connection_info.Requesting_Count);
    AddChild(ElementPeerConnectionInfo, "Received_Count", peer_connection_info.Received_Count);
    AddChild(ElementPeerConnectionInfo, "AssignedLeftSubPieceCount", peer_connection_info.AssignedLeftSubPieceCount);
    AddChild(ElementPeerConnectionInfo, "LastLiveBlockId", peer_connection_info.LastLiveBlockId);
    AddChild(ElementPeerConnectionInfo, "FirstLiveBlockId", peer_connection_info.FirstLiveBlockId);
}

void XmlCreater::WriteLiveStatisticInfoToXmlFile(TiXmlElement *element, const LIVE_DOWNLOADDRIVER_STATISTIC_INFO& downloader_driver_statistic_info)
{
    TiXmlElement *ElementLiveStatisticInfo = new TiXmlElement("LiveDownloaderDirverStatisticInfo");
    element->LinkEndChild(ElementLiveStatisticInfo);

    AddChild(ElementLiveStatisticInfo, "LiveDownloadDriverID", downloader_driver_statistic_info.LiveDownloadDriverID);
    WriteSpeedInfoToXmlFile(ElementLiveStatisticInfo, downloader_driver_statistic_info.LiveHttpSpeedInfo, "LiveHttpSpeedInfo");
    WriteSpeedInfoToXmlFile(ElementLiveStatisticInfo, downloader_driver_statistic_info.LiveP2PSpeedInfo, "LiveP2PSpeedInfo");
    WriteSpeedInfoToXmlFile(ElementLiveStatisticInfo, downloader_driver_statistic_info.LiveP2PSubPieceSpeedInfo, "LiveP2PSubPieceSpeedInfo");
    AddChild(ElementLiveStatisticInfo, "http_state", downloader_driver_statistic_info.http_state);
    AddChild(ElementLiveStatisticInfo, "p2p_state", downloader_driver_statistic_info.p2p_state);

    std::string tmp = "";
    for (int i = 0; i < sizeof(downloader_driver_statistic_info.OriginalUrl) / sizeof(downloader_driver_statistic_info.OriginalUrl[0]);
        ++i)
    {
        tmp += downloader_driver_statistic_info.OriginalUrl[i];
    }
    AddStringChild(ElementLiveStatisticInfo, "OriginalUrl", tmp);

    AddChild(ElementLiveStatisticInfo, "LastHttpStatusCode", downloader_driver_statistic_info.LastHttpStatusCode);
    AddChild(ElementLiveStatisticInfo, "TotalP2PDataBytes", downloader_driver_statistic_info.TotalP2PDataBytes);
    AddChild(ElementLiveStatisticInfo, "TotalRecievedSubPieceCount", downloader_driver_statistic_info.TotalRecievedSubPieceCount);
    AddChild(ElementLiveStatisticInfo, "TotalRequestSubPieceCount", downloader_driver_statistic_info.TotalRequestSubPieceCount);
    AddChild(ElementLiveStatisticInfo, "TotalAllRequestSubPieceCount", downloader_driver_statistic_info.TotalAllRequestSubPieceCount);
    AddChild(ElementLiveStatisticInfo, "TotalUnusedSubPieceCount", downloader_driver_statistic_info.TotalUnusedSubPieceCount);
    AddChild(ElementLiveStatisticInfo, "IpPoolPeerCount", downloader_driver_statistic_info.IpPoolPeerCount);
    AddChild(ElementLiveStatisticInfo, "DataRate", downloader_driver_statistic_info.DataRate);
    AddChild(ElementLiveStatisticInfo, "CacheSize", downloader_driver_statistic_info.CacheSize);
    AddChild(ElementLiveStatisticInfo, "CacheFirstBlockId", downloader_driver_statistic_info.CacheFirstBlockId);
    AddChild(ElementLiveStatisticInfo, "CacheLastBlockId", downloader_driver_statistic_info.CacheLastBlockId);
    AddChild(ElementLiveStatisticInfo, "PlayingPosition", downloader_driver_statistic_info.PlayingPosition);
    AddChild(ElementLiveStatisticInfo, "LeftCapacity", downloader_driver_statistic_info.LeftCapacity);
    AddChild(ElementLiveStatisticInfo, "RestPlayTime", downloader_driver_statistic_info.RestPlayTime);
    AddStringChild(ElementLiveStatisticInfo, "ResourceID", downloader_driver_statistic_info.ResourceID.to_string());
    AddChild(ElementLiveStatisticInfo, "IsPlayingPositionBlockFull", downloader_driver_statistic_info.IsPlayingPositionBlockFull);
    AddChild(ElementLiveStatisticInfo, "LivePointBlockId", downloader_driver_statistic_info.LivePointBlockId);
    AddChild(ElementLiveStatisticInfo, "DataRateLevel", downloader_driver_statistic_info.DataRateLevel);
    AddChild(ElementLiveStatisticInfo, "P2PFailedTimes", downloader_driver_statistic_info.P2PFailedTimes);
    AddChild(ElementLiveStatisticInfo, "HttpSpeedStatus", downloader_driver_statistic_info.HttpSpeedStatus);
    AddChild(ElementLiveStatisticInfo, "HttpStatus", downloader_driver_statistic_info.HttpStatus);
    AddChild(ElementLiveStatisticInfo, "P2PStatus", downloader_driver_statistic_info.P2PStatus);
    AddChild(ElementLiveStatisticInfo, "JumpTimes", downloader_driver_statistic_info.JumpTimes);
    AddChild(ElementLiveStatisticInfo, "NumOfChecksumFailedPieces", downloader_driver_statistic_info.NumOfChecksumFailedPieces);
    AddChild(ElementLiveStatisticInfo, "Is3200P2PSlow", downloader_driver_statistic_info.Is3200P2PSlow);
    AddStringChild(ElementLiveStatisticInfo, "ChannelID", downloader_driver_statistic_info.ChannelID.to_string());
    AddChild(ElementLiveStatisticInfo, "TotalUdpServerDataBytes", downloader_driver_statistic_info.TotalUdpServerDataBytes);
    AddChild(ElementLiveStatisticInfo, "PmsStatus", downloader_driver_statistic_info.PmsStatus);
    AddChild(ElementLiveStatisticInfo, "UniqueID", downloader_driver_statistic_info.UniqueID);
    WriteSpeedInfoToXmlFile(ElementLiveStatisticInfo, downloader_driver_statistic_info.UdpServerSpeedInfo, "LiveUdpServerSpeedInfo");
    AddChild(ElementLiveStatisticInfo, "IsPaused", downloader_driver_statistic_info.IsPaused);
    AddChild(ElementLiveStatisticInfo, "IsReplay", downloader_driver_statistic_info.IsReplay);
    AddChild(ElementLiveStatisticInfo, "PeerCount", downloader_driver_statistic_info.PeerCount);
    TiXmlElement* ElementP2PConnections = new TiXmlElement("P2PConnections");
    ElementLiveStatisticInfo->LinkEndChild(ElementP2PConnections);
    for (int i = 0; i < downloader_driver_statistic_info.PeerCount; ++i)
    {
        WritePeerConnectionInfoToXmlFile(ElementP2PConnections, downloader_driver_statistic_info.P2PConnections[i]);
    }
}

void XmlCreater::WriteP2PDownloaderStatisticsInfoToXmlFile(TiXmlElement *element, const P2PDOWNLOADER_STATISTIC_INFO& p2p_statistic_info)
{
    TiXmlElement *ElementP2PStatisticInfo = new TiXmlElement("P2PDownloadStatisticInfo");
    element->LinkEndChild(ElementP2PStatisticInfo);

    AddStringChild(ElementP2PStatisticInfo, "ResourceID", p2p_statistic_info.ResourceID.to_string());
    WriteSpeedInfoToXmlFile(ElementP2PStatisticInfo, p2p_statistic_info.SpeedInfo, "SpeedInfo");
    AddChild(ElementP2PStatisticInfo, "FileLength", p2p_statistic_info.FileLength);
    AddChild(ElementP2PStatisticInfo, "BlockNum", p2p_statistic_info.BlockNum);
    AddChild(ElementP2PStatisticInfo, "BlockSize", p2p_statistic_info.BlockSize);
    AddChild(ElementP2PStatisticInfo, "IpPoolPeerCount", p2p_statistic_info.IpPoolPeerCount);
    AddChild(ElementP2PStatisticInfo, "ExchangingPeerCount", p2p_statistic_info.ExchangingPeerCount);
    AddChild(ElementP2PStatisticInfo, "ConnectingPeerCount", p2p_statistic_info.ConnectingPeerCount);
    AddChild(ElementP2PStatisticInfo, "TotalWindowSize", p2p_statistic_info.TotalWindowSize);
    AddChild(ElementP2PStatisticInfo, "TotalAssignedSubPieceCount", p2p_statistic_info.TotalAssignedSubPieceCount);
    AddChild(ElementP2PStatisticInfo, "TotalUnusedSubPieceCount_", p2p_statistic_info.TotalUnusedSubPieceCount_);
    AddChild(ElementP2PStatisticInfo, "TotalRecievedSubPieceCount_", p2p_statistic_info.TotalRecievedSubPieceCount_);
    AddChild(ElementP2PStatisticInfo, "TotalRequestSubPieceCount_", p2p_statistic_info.TotalRequestSubPieceCount_);
    AddChild(ElementP2PStatisticInfo, "SubPieceRetryRate", p2p_statistic_info.SubPieceRetryRate);
    AddChild(ElementP2PStatisticInfo, "UDPLostRate", p2p_statistic_info.UDPLostRate);
    AddChild(ElementP2PStatisticInfo, "TotalP2PPeerDataBytes", p2p_statistic_info.TotalP2PPeerDataBytes);
    AddChild(ElementP2PStatisticInfo, "TotalP2PSnDataBytes", p2p_statistic_info.TotalP2PSnDataBytes);
    AddChild(ElementP2PStatisticInfo, "FullBlockPeerCount", p2p_statistic_info.FullBlockPeerCount);
    AddChild(ElementP2PStatisticInfo, "TotalUnusedSubPieceCount", p2p_statistic_info.TotalUnusedSubPieceCount);
    AddChild(ElementP2PStatisticInfo, "TotalRecievedSubPieceCount", p2p_statistic_info.TotalRecievedSubPieceCount);
    AddChild(ElementP2PStatisticInfo, "TotalRequestSubPieceCount", p2p_statistic_info.TotalRequestSubPieceCount);
    AddChild(ElementP2PStatisticInfo, "NonConsistentSize", p2p_statistic_info.NonConsistentSize);
    AddChild(ElementP2PStatisticInfo, "ConnectCount", p2p_statistic_info.ConnectCount);
    AddChild(ElementP2PStatisticInfo, "KickCount", p2p_statistic_info.KickCount);
    AddChild(ElementP2PStatisticInfo, "EmptySubpieceDistance", p2p_statistic_info.empty_subpiece_distance);

    boost::uint32_t requesting_count = 0;
    for (int i = 0; i < p2p_statistic_info.PeerCount; ++i)
    {
        requesting_count += p2p_statistic_info.P2PConnections[i].Requesting_Count;
    }

    double lost_rate = (p2p_statistic_info.TotalRequestSubPieceCount - requesting_count - p2p_statistic_info.TotalUnusedSubPieceCount) * 100.0
        / (p2p_statistic_info.TotalRequestSubPieceCount - requesting_count + 0.000001);
    AddDoubleChild(ElementP2PStatisticInfo, "LostRate", lost_rate);

    double re_rate = (p2p_statistic_info.TotalUnusedSubPieceCount - p2p_statistic_info.TotalRecievedSubPieceCount) * 100.0
        / (p2p_statistic_info.TotalUnusedSubPieceCount + 0.000001);
    AddDoubleChild(ElementP2PStatisticInfo, "ReRate", re_rate);

    AddChild(ElementP2PStatisticInfo, "PeerCount", p2p_statistic_info.PeerCount);
    TiXmlElement* ElementP2PConnections = new TiXmlElement("P2PConnections");
    ElementP2PStatisticInfo->LinkEndChild(ElementP2PConnections);
    for (int i = 0; i < p2p_statistic_info.PeerCount; ++i)
    {
        WritePeerConnectionInfoToXmlFile(ElementP2PConnections, p2p_statistic_info.P2PConnections[i]);
    }
}

void XmlCreater::WriteStatisticsDataToXmlFile(TiXmlElement *ElementStatisticData, const boost::shared_ptr<StatisticsData>& statistic_data)
{
    WriteStatisticInfoToXmlFile(ElementStatisticData, statistic_data->GetStatisticsInfo());
    TiXmlElement* ElementVodDownloadDriverStatisticses = new TiXmlElement("VodDownloadDriverStatisticses");
    ElementStatisticData->LinkEndChild(ElementVodDownloadDriverStatisticses);
    for (int i = 0; i < statistic_data->GetVodDownloadDriverStatistics().size(); ++i)
    {
        WriteDownloadDriverStatisticInfoToXmlFile(ElementVodDownloadDriverStatisticses, statistic_data->GetVodDownloadDriverStatistics()[i]->GetDownloadDriverStatisticsInfo());
    }
    TiXmlElement* ElementLiveDownloadDriverStatisticses = new TiXmlElement("LiveDownloadDriverStatisticses");
    ElementStatisticData->LinkEndChild(ElementLiveDownloadDriverStatisticses);
    for (int i = 0; i < statistic_data->GetLiveDownloadDriverStatistics().size(); ++i)
    {
        WriteLiveStatisticInfoToXmlFile(ElementLiveDownloadDriverStatisticses, statistic_data->GetLiveDownloadDriverStatistics()[i]->GetDownloadDriverStatisticsInfo());
    }
    TiXmlElement* ElementP2PDownloaderStatisticses = new TiXmlElement("P2PDownloaderStatisticses");
    ElementStatisticData->LinkEndChild(ElementP2PDownloaderStatisticses);
    for (int i = 0; i < statistic_data->GetP2PDownloaderStatistics().size(); ++i)
    {
        WriteP2PDownloaderStatisticsInfoToXmlFile(ElementP2PDownloaderStatisticses, statistic_data->GetP2PDownloaderStatistics()[i]->GetDownloaderStatisticsInfo());
    }
}
