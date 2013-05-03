#include "stdafx.h"
#include "csv_creater.h"

using namespace statistic;

void CsvCreater::CreateCsvFile(const std::vector<boost::shared_ptr<StatisticsData> >& statistic_datas)
{
    FILE * fp = fopen("test.csv", "w");
    std::ostringstream oss;

    for(size_t i = 0; i < statistic_datas.size(); ++i)
    {
        WriteStatisticDataToCsvFile(oss, statistic_datas[i]);
    }

    fwrite(oss.str().c_str(), oss.str().size(), 1, fp);

    fclose(fp);
}

void CsvCreater::WriteStatisticDataToCsvFile(std::ostringstream& oss, const boost::shared_ptr<StatisticsData>& statistic_data)
{
    oss << statistic_data->GetStatisticsInfo() << ',';
    oss << statistic_data->GetVodDownloadDriverStatistics().size() << ',';
    for (int i = 0; i < statistic_data->GetVodDownloadDriverStatistics().size(); ++i)
    {
        WriteDownloadDriverStatisticInfoToCsvFile(oss, statistic_data->GetVodDownloadDriverStatistics()[i]->GetDownloadDriverStatisticsInfo());
    }
    oss << statistic_data->GetLiveDownloadDriverStatistics().size() << ',';
    for (int i = 0; i < statistic_data->GetLiveDownloadDriverStatistics().size(); ++i)
    {
        WriteLiveStatisticInfoToCsvFile(oss, statistic_data->GetLiveDownloadDriverStatistics()[i]->GetDownloadDriverStatisticsInfo());
    }
    oss << statistic_data->GetP2PDownloaderStatistics().size() << ',';
    for (int i = 0; i < statistic_data->GetP2PDownloaderStatistics().size(); ++i)
    {
        WriteP2PDownloaderStatisticsInfoToCsvFile(oss, statistic_data->GetP2PDownloaderStatistics()[i]->GetDownloaderStatisticsInfo());
    }

    oss << "\r\n";
}
void CsvCreater::WriteDownloadDriverStatisticInfoToCsvFile(std::ostringstream& oss, const DOWNLOADDRIVER_STATISTIC_INFO& downloader_driver_statistic_info)
{
    OutputDownloaderStatisticInfoToOstream(oss, downloader_driver_statistic_info);
    oss << ',';
}

void CsvCreater::WriteLiveStatisticInfoToCsvFile(std::ostringstream& oss, const LIVE_DOWNLOADDRIVER_STATISTIC_INFO& downloader_driver_statistic_info)
{
    oss << downloader_driver_statistic_info << ',';
}

void CsvCreater::WriteP2PDownloaderStatisticsInfoToCsvFile(std::ostringstream& oss, const P2PDOWNLOADER_STATISTIC_INFO& p2p_statistic_info)
{
    oss << p2p_statistic_info << ',';
}

std::ostream& operator << (std::ostream& os, const SPEED_INFO& speed_info)
{
    return os << speed_info.StartTime << ','
        << speed_info.TotalDownloadBytes << ','
        << speed_info.TotalUploadBytes << ','
        << speed_info.NowDownloadSpeed << ','
        << speed_info.NowUploadSpeed << ','
        << speed_info.MinuteDownloadSpeed << ','
        << speed_info.MinuteUploadSpeed << ','
        << speed_info.AvgUploadSpeed;
}

std::ostream& operator << (std::ostream& os, const protocol::TRACKER_INFO& info)
{
    return os << info.StationNo << ','
        << info.ModNo << ','
        << info.IP << ','
        << info.Port << ','
        << info.Type;
}

std::ostream& operator << (std::ostream& os, const STATISTIC_TRACKER_INFO& info)
{
    return os << info.TrackerInfo << ','
        << info.CommitRequestCount << ','
        << info.CommitResponseCount << ','
        << info.KeepAliveRequestCount << ','
        << info.KeepAliveResponseCount << ','
        << info.ListRequestCount << ','
        << info.ListResponseCount << ','
        << info.LastListReturnPeerCount << ','
        << info.IsSubmitTracker << ','
        << info.ErrorCode << ','
        << info.KeepAliveInterval;
}

std::ostream& operator << (std::ostream& os, const STATISTIC_INDEX_INFO& info)
{
    return os << info.IP << ','
        << info.Port << ','
        << info.Type << ','
        << info.QueryRIDByUrlRequestCount << ','
        << info.QueryRIDByUrlResponseCount << ','
        << info.QueryHttpServersByRIDRequestCount << ','
        << info.QueryHttpServersByRIDResponseCount << ','
        << info.QueryTrackerListRequestCount << ','
        << info.QureyTrackerListResponseCount << ','
        << info.AddUrlRIDRequestCount << ','
        << info.AddUrlRIDResponseCount;
}

std::ostream& operator << (std::ostream& os, const STASTISTIC_INFO& info)
{
    os << info.SpeedInfo << ',';
    OutputCandidatePeerInfoToOstream(os, info.LocalPeerInfo);
    os << ',' << info.LocalIpCount << ',';

    for (int i = 0; i < sizeof(info.LocalIPs) / sizeof(info.LocalIPs[0]); ++i)
    {
        os << info.LocalIPs[i] << ',';
    }

    os << info.LocalPeerVersion << ','
        << info.Reverse << ','
        << info.TrackerCount << ','
        << info.GroupCount << ',';

    for (int i = 0; i < sizeof(info.TrackerInfos) / sizeof(info.TrackerInfos[0]); ++i)
    {
        os << info.TrackerInfos[i] << ',';
    }

    os << info.StatisticIndexInfo << ','
        << info.P2PDownloaderCount << ',';

    for (int i = 0; i < sizeof(info.P2PDownloaderRIDs) / sizeof(info.P2PDownloaderRIDs[0]); ++i)
    {
        os << info.P2PDownloaderRIDs[i] << ',';
    }

    os << info.DownloadDriverCount << ',';

    for (int i = 0; i < sizeof(info.DownloadDriverIDs) / sizeof(info.DownloadDriverIDs[0]); ++i)
    {
        os << info.DownloadDriverIDs[i] << ',';
    }

    os << info.TotalP2PDownloadBytes << ','
        << info.TotalOtherServerDownloadBytes << ','
        << info.TotalHttpNotOriginalDataBytes << ','
        << info.TotalP2PDataBytes << ','
        << info.TotalHttpOriginalDataBytes << ','
        << info.CompleteCount << ','
        << info.TotalUploadCacheRequestCount << ','
        << info.TotalUploadCacheHitCount << ','
        << info.HttpProxyPort << ','
        << info.MaxHttpDownloadSpeed << ','
        << info.IncomingPeersCount << ','
        << info.DownloadDurationInSec << ','
        << info.BandWidth << ','
        << info.GlobalWindowSize << ','
        << info.GlobalRequestSendCount << ','
        << info.MemoryPoolLeftSize << ','
        << info.LiveDownloadDriverCount << ',';

    for (int i = 0; i < sizeof(info.LiveDownloadDriverIDs) / sizeof(info.LiveDownloadDriverIDs[0]); ++i)
    {
        os << info.LiveDownloadDriverIDs[i] << ',';
    }

    for (int i = 0; i < sizeof(info.Resersed) / sizeof(info.Resersed[0]); ++i)
    {
        os << info.Resersed[i] << ',';
    }

    return os;
}

std::ostream& operator << (std::ostream& os, const P2P_CONNECTION_INFO& info)
{
    os << info.PeerGuid << ','
        << info.SpeedInfo << ','
        << info.PeerVersion << ','
        << info.Reserve << ',';
    OutputCandidatePeerInfoToOstream(os, info.PeerInfo);
    os << ',' << info.PeerDownloadInfo << ',';

    for (int i = 0; i < sizeof(info.BitMap) / sizeof(info.BitMap[0]); ++i)
    {
        os << info.BitMap[i] << ',';
    }

    os << info.RTT_Count << ','
        << info.RTT_Now << ','
        << info.RTT_Average << ','
        << info.RTT_Max << ','
        << info.RTT_Total << ','
        << info.ElapseTime << ','
        << info.WindowSize << ','
        << info.AssignedSubPieceCount << ','
        << info.AverageDeltaTime << ','
        << info.SortedValue << ','
        << info.IsRidInfoValid << ','
        << info.Sent_Count << ','
        << info.Requesting_Count << ','
        << info.Received_Count << ','
        << info.AssignedLeftSubPieceCount << ','
        << info.LastLiveBlockId << ','
        << info.FirstLiveBlockId << ',';

    for (int i = 0; i < sizeof(info.Reserved) / sizeof(info.Reserved[0]); ++i)
    {
        os << info.Reserved[i] << ',';
    }

    return os;
}

std::ostream& operator << (std::ostream& os, const P2PDOWNLOADER_STATISTIC_INFO& info)
{
    os << info.ResourceID << ','
        << info.SpeedInfo << ','
        << info.FileLength << ','
        << info.BlockNum << ','
        << info.BlockSize << ','
        << info.IpPoolPeerCount << ','
        << info.ExchangingPeerCount << ','
        << info.ConnectingPeerCount << ','
        << info.TotalWindowSize << ','
        << info.TotalAssignedSubPieceCount << ','
        << info.TotalUnusedSubPieceCount_ << ','
        << info.TotalRecievedSubPieceCount_ << ','
        << info.TotalRequestSubPieceCount_ << ','
        << info.SubPieceRetryRate << ','
        << info.UDPLostRate << ','
        << info.TotalP2PPeerDataBytes << ','
        << info.TotalP2PSnDataBytes << ','
        << info.FullBlockPeerCount << ','
        << info.TotalUnusedSubPieceCount << ','
        << info.TotalRecievedSubPieceCount << ','
        << info.TotalRequestSubPieceCount << ','
        << info.NonConsistentSize << ','
        << info.ConnectCount << ','
        << info.KickCount << ','
        << info.empty_subpiece_distance << ',';

    for (int i = 0; i < sizeof(info.Reserved) / sizeof(info.Reserved[0]); ++i)
    {
        os << info.Reserved[i] << ',';
    }

    os << info.PeerCount << ',';

    for (int i = 0; i < info.PeerCount; ++i)
    {
        os << info.P2PConnections[i] << ',';
    }

    return os;
}

std::ostream& operator << (std::ostream& os, const HTTP_DOWNLOADER_INFO& info)
{
    for (int i = 0; i < sizeof(info.Url) / sizeof(info.Url[0]); ++i)
    {
        os << info.Url[i] << ',';
    }

    for (int i = 0; i < sizeof(info.ReferUrl) / sizeof(info.ReferUrl[0]); ++i)
    {
        os << info.ReferUrl[i] << ',';
    }

    for (int i = 0; i < sizeof(info.RedirectUrl) / sizeof(info.RedirectUrl[0]); ++i)
    {
        os << info.RedirectUrl[i] << ',';
    }

    os << info.SpeedInfo << ',';
    OutputPieceInfoExToOstream(os, info.DownloadingPieceEx);
    os << ',';
    OutputPieceInfoExToOstream(os, info.StartPieceEx);
    os << ',' << info.LastConnectedTime << ','
        << info.LastRequestPieceTime << ','
        << info.LastHttpStatusCode << ','
        << info.RetryCount << ','
        << info.IsSupportRange << ','
        << info.IsDeath << ','
        << info.IsPause << ',';

    for (int i = 0; i < sizeof(info.Resersed) / sizeof(info.Resersed[0]); ++i)
    {
        os << info.Resersed[i] << ',';
    }

    return os;
}

std::ostream& operator << (std::ostream& os, const LIVE_DOWNLOADDRIVER_STATISTIC_INFO& info)
{
    os << info.LiveDownloadDriverID << ','
        << info.LiveHttpSpeedInfo << ','
        << info.LiveP2PSpeedInfo << ','
        << info.LiveP2PSubPieceSpeedInfo << ','
        << info.http_state << ','
        << info.p2p_state << ',';

    for (int i = 0; i < sizeof(info.OriginalUrl) / sizeof(info.OriginalUrl[0]); ++i)
    {
        os << info.OriginalUrl[i] << ',';
    }

    os << info.LastHttpStatusCode << ','
        << info.TotalP2PDataBytes << ','
        << info.TotalRecievedSubPieceCount << ','
        << info.TotalRequestSubPieceCount << ','
        << info.TotalAllRequestSubPieceCount << ','
        << info.TotalUnusedSubPieceCount << ','
        << info.IpPoolPeerCount << ','
        << info.DataRate << ','
        << info.CacheSize << ','
        << info.CacheFirstBlockId << ','
        << info.CacheLastBlockId << ','
        << info.PlayingPosition << ','
        << info.LeftCapacity << ','
        << info.RestPlayTime << ','
        << info.ResourceID << ','
        << info.IsPlayingPositionBlockFull << ','
        << info.LivePointBlockId << ','
        << info.DataRateLevel << ','
        << info.P2PFailedTimes << ','
        << info.HttpSpeedStatus << ','
        << info.HttpStatus << ','
        << info.P2PStatus << ','
        << info.JumpTimes << ','
        << info.NumOfChecksumFailedPieces << ','
        << info.Is3200P2PSlow << ','
        << info.ChannelID << ',';

    for (int i = 0; i < sizeof(info.Reserved) / sizeof(info.Reserved[0]); ++i)
    {
        os << info.Reserved[i] << ',';
    }

    os << info.PeerCount << ',';

    for (int i = 0; i < info.PeerCount; ++i)
    {
        os << info.P2PConnections[i] << ',';
    }

    return os;
}


std::ostream& operator << (std::ostream& os, const protocol::PEER_DOWNLOAD_INFO& info)
{
    return os << info.IsDownloading << ','
        << info.OnlineTime << ','
        << info.AvgDownload << ','
        << info.NowDownload << ','
        << info.AvgUpload << ','
        << info.NowUpload << ',';
}

void CsvCreater::OutputDownloaderStatisticInfoToOstream(std::ostream& os, const DOWNLOADDRIVER_STATISTIC_INFO& info)
{
    os << info.DownloadDriverID << ','
        << info.SpeedInfo << ',';

    for (int i = 0; i < sizeof(info.OriginalUrl) / sizeof(info.OriginalUrl[0]); ++i)
    {
        os << info.OriginalUrl[i];
    }
    os << ',';

    for (int i = 0; i < sizeof(info.OriginalReferUrl) / sizeof(info.OriginalReferUrl[0]); ++i)
    {
        os << info.OriginalReferUrl[i];
    }
    os << ',';

    os << info.ResourceID << ','
        << info.FileLength << ','
        << info.BlockSize << ','
        << info.BlockCount << ','
        << info.TotalHttpDataBytes << ','
        << info.TotalLocalDataBytes << ',';

    for (int i = 0; i < sizeof(info.FileName) / sizeof(info.FileName[0]); ++i)
    {
        os << info.FileName[i];
    }
    os << ',';

    os << info.IsHidden << ','
        << info.SourceType << ',';

    for (int i = 0; i < sizeof(info.WebUrl) / sizeof(info.WebUrl[0]); ++i)
    {
        os << info.WebUrl[i];
    }
    os << ',';

    os << info.StateMachineType << ',';

    for (int i = 0; i < sizeof(info.StateMachineState) / sizeof(info.StateMachineState[0]); ++i)
    {
        os << info.StateMachineState[i] << ',';
    }

    os << info.PlayingPosition << ','
        << info.DataRate << ','
        << info.http_state << ','
        << info.p2p_state << ','
        << info.timer_using_state << ','
        << info.timer_state << ','
        << info.t << ','
        << info.b << ','
        << info.speed_limit << ',';

    for (int i = 0; i < sizeof(info.Resersed) / sizeof(info.Resersed[0]); ++i)
    {
        os << info.Resersed[i] << ',';
    }

    os << info.HttpDownloaderCount << ',';

    for (int i = 0; i < info.HttpDownloaderCount; ++i)
    {
        os << info.HttpDownloaders[i] << ',';
    }
}

void OutputCandidatePeerInfoToOstream(std::ostream& os, const protocol::CandidatePeerInfo& info)
{
    os << info.IP << ','
        << info.UdpPort << ','
        << info.PeerVersion << ','
        << info.DetectIP << ','
        << info.DetectUdpPort << ','
        << info.StunIP << ','
        << info.StunUdpPort << ','
        << info.PeerNatType << ','
        << info.UploadPriority << ','
        << info.IdleTimeInMins << ','
        << info.TrackerPriority;
}

void OutputPieceInfoExToOstream(std::ostream& os, const PIECE_INFO_EX& info)
{
    os << info.BlockIndex << ',' << info.PieceIndexInBlock << ',' << info.SubPieceIndexInPiece;
}

int utf16_length2( char * buf )
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

void CsvCreater::SimpleWriteToCSVFile(const std::vector<boost::shared_ptr<StatisticsData> >& statistic_datas, const char* path)
{
    FILE * fp = fopen(path, "w");

    fprintf(fp, "%s,", "filename");
    fprintf(fp, "%s,", "filelength");
    fprintf(fp, "%s,", "datarate");
    fprintf(fp, "%s,", "CDNIP");
    fprintf(fp, "%s,", "totalsent");
    fprintf(fp, "%s,", "totalrecv");
    fprintf(fp, "%s,", "lossrate");
    fprintf(fp, "%s,", "redundency");
    fprintf(fp, "%s,", "httpdownload");
    fprintf(fp, "%s,", "p2ppeerdownload");
    fprintf(fp, "%s,", "p2psndownload");
    fprintf(fp, "%s,", "remaintime");
    fprintf(fp, "%s,", "httpspeed");
    fprintf(fp, "%s,", "p2pspeed");
    fprintf(fp, "%s,", "totalspeed");
    fprintf(fp, "%s,", "statemachine");
    fprintf(fp, "%s,", "numpeer-fullres");
    fprintf(fp, "%s,", "numpeer-connect");
    fprintf(fp, "%s,", "IPPool");
    fprintf(fp, "%s,", "ClientIP");
    fprintf(fp, "%s\n", "filename2");

    char this_rid[MAX_PATH];
    int last_pc_count = 0;
    for (int i = 0; i < statistic_datas.size(); ++i)
    {
        if (statistic_datas[i]->GetVodDownloadDriverStatistics().size() == 0)
        {
            continue;
        }

        statistic::DOWNLOADDRIVER_STATISTIC_INFO dd_info;

        if (i == 0)
        {
            int j;
            for (j = 0; j < statistic_datas[i]->GetVodDownloadDriverStatistics().size(); ++j)
            {
                dd_info = statistic_datas[i]->GetVodDownloadDriverStatistics()[j]->GetDownloadDriverStatisticsInfo();
                std::string rid = dd_info.ResourceID.to_string();
                sprintf(this_rid, "%s", rid.c_str());
                int http_downloader_index = 0;
                while (http_downloader_index < dd_info.HttpDownloaderCount && dd_info.HttpDownloaders[http_downloader_index].IsPause == 1)
                {
                    ++http_downloader_index;
                }
                if (http_downloader_index >= dd_info.HttpDownloaderCount)
                {
                    http_downloader_index = 0;
                }
                char *ip = (char*)(dd_info.HttpDownloaders[http_downloader_index].Url);

                string str_ip = ip;
                if (str_ip.find("ppsucai.pptv.com") == -1 && str_ip.find("synacast") == -1)
                {
                    break;
                }
            }
            if (j == statistic_datas[i]->GetVodDownloadDriverStatistics().size())
            {
                continue;
            }
        }
        else
        {
            int j;
            for (j = 0; j < statistic_datas[i]->GetVodDownloadDriverStatistics().size(); ++j)
            {
                char tmp_rid[MAX_PATH];
                dd_info = statistic_datas[i]->GetVodDownloadDriverStatistics()[j]->GetDownloadDriverStatisticsInfo();
                std::string rid = dd_info.ResourceID.to_string();
                sprintf(tmp_rid, "%s", rid.c_str());
                if (strlen(tmp_rid) != 0 && strlen(this_rid) == 0)
                {
                    strcat(this_rid, tmp_rid);
                }
                else
                {
                    if (strcmp(this_rid, tmp_rid) == 0)
                    {
                        break;
                    }
                }
            }
            if (j == statistic_datas[i]->GetVodDownloadDriverStatistics().size())
            {
                continue;
            }
        }

        time_t now = time(0);
        char str_now[MAX_PATH]; 

        wchar_t *wfilename = (wchar_t*)(dd_info.FileName);

#ifdef BOOST_WINDOWS_API
        char ansiname[MAX_PATH] = "";
        WideCharToMultiByte(CP_ACP, 0, wfilename, -1, ansiname, MAX_PATH, NULL, NULL);
        string filename;
        if (strlen(ansiname) == 0)
        {
            filename = "unknown";
        }
        else
        {
            filename = ansiname;
        }
#else
        iconv_t cd = iconv_open("UTF-8", "UNICODE");
        char *utf8name = (char *)malloc(255);
        memset( utf8name , 0 , 255 );
        int inlen = utf16_length2((char*)wfilename);
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
        //fprintf(fp, "%s", filename.c_str()); //文件名
        fprintf(fp, "%s", this_rid); //文件名
        fprintf(fp, ",%d", dd_info.FileLength);  //文件长度
        fprintf(fp, ",%d", dd_info.DataRate);    //码流

        int http_downloader_index = 0;
        while (http_downloader_index < dd_info.HttpDownloaderCount && dd_info.HttpDownloaders[http_downloader_index].IsPause == 1)
        {
            ++http_downloader_index;
        }
        if (http_downloader_index >= dd_info.HttpDownloaderCount)
        {
            http_downloader_index = 0;
        }


        char *ip = (char*)(dd_info.HttpDownloaders[http_downloader_index].Url);

        string tmp_ip = ip;
        int pos = tmp_ip.find('/', 8);
        string str_ip = tmp_ip.substr(7, pos - 7);

        if (dd_info.http_state == 3 && dd_info.p2p_state != 0)
        {
            fprintf(fp, ",");    //CDN IP
        }
        else
        {
            fprintf(fp, ",%s", str_ip.c_str());    //CDN IP
        }

        statistic::P2PDOWNLOADER_STATISTIC_INFO p2p_info;
        for (int j = 0; j < statistic_datas[i]->GetP2PDownloaderStatistics().size(); ++j)
        {
            char tmp_rid[MAX_PATH];
            std::string rid = statistic_datas[i]->GetP2PDownloaderStatistics()[j]->GetDownloaderStatisticsInfo().ResourceID.to_string();
            sprintf(tmp_rid, "%s", rid.c_str());
            if (strcmp(this_rid, tmp_rid) == 0)
            {
                p2p_info = statistic_datas[i]->GetP2PDownloaderStatistics()[j]->GetDownloaderStatisticsInfo();
                break;
            }
        }
        fprintf(fp, ",%d", p2p_info.TotalRequestSubPieceCount);        //总发送
        fprintf(fp, ",%d", p2p_info.TotalRecievedSubPieceCount);    //总收到

        int total_requestint_count = 0;
        for (int i=0; i<last_pc_count; i++)
        {
            total_requestint_count += p2p_info.P2PConnections[i].Requesting_Count;
        }

        double tmp_lost_rate = (p2p_info.TotalRequestSubPieceCount - total_requestint_count - p2p_info.TotalUnusedSubPieceCount)* 100.0
            / (p2p_info.TotalRequestSubPieceCount - total_requestint_count + 0.000001);
        fprintf(fp, ",%f", tmp_lost_rate);//丢包率
        double tmp_re_rate = (p2p_info.TotalUnusedSubPieceCount - p2p_info.TotalRecievedSubPieceCount) * 100.0 /
            (p2p_info.TotalUnusedSubPieceCount + 0.0000001);
        fprintf(fp, ",%f", tmp_re_rate);    //冗余率

        fprintf(fp, ",%d", dd_info.HttpDownloaders[http_downloader_index].SpeedInfo.TotalDownloadBytes / 1024);//http总下载
        fprintf(fp, ",%d", p2p_info.TotalP2PPeerDataBytes / 1024); //p2p总下载
        fprintf(fp, ",%d", p2p_info.TotalP2PSnDataBytes / 1024); 

        fprintf(fp, ",%d", dd_info.t / 1000);//剩余时间
        fprintf(fp, ",%d", dd_info.HttpDownloaders[http_downloader_index].SpeedInfo.NowDownloadSpeed / 1024); //http速度
        fprintf(fp, ",%d", p2p_info.SpeedInfo.NowDownloadSpeed / 1024);  //p2p速度
        fprintf(fp, ",%d", dd_info.HttpDownloaders[http_downloader_index].SpeedInfo.NowDownloadSpeed / 1024
            + p2p_info.SpeedInfo.NowDownloadSpeed / 1024);  //总下载速度

        fprintf(fp, ",%d", dd_info.http_state);//状态机
        fprintf(fp, "%d", dd_info.p2p_state);//状态机
        fprintf(fp, "%d", dd_info.timer_using_state);//状态机
        fprintf(fp, "%d", dd_info.timer_state);//状态机

        fprintf(fp, ",%d", p2p_info.FullBlockPeerCount);   //满资源数
        fprintf(fp, ",%d", p2p_info.PeerCount);              //连接上的peer数
        fprintf(fp, ",%d", p2p_info.IpPoolPeerCount);      //备选peer数
        in_addr addr;
        addr.s_addr = statistic_datas[i]->GetStatisticsInfo().LocalIPs[0];
        fprintf(fp, ",%s", inet_ntoa(addr));
        fprintf(fp, ",%s\n", filename.c_str()); //文件名

    }

    fclose(fp);
}

