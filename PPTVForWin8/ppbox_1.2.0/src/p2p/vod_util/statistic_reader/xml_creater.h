#include "stdafx.h"
#include "tinyxml/tinyxml.h"
#include "statistic/StatisticStructs.h"
#include "statistic/StatisticsData.h"

using namespace statistic;

class XmlCreater
{
public:
    void CreateXmlFile(const std::vector<boost::shared_ptr<StatisticsData> >& statistic_datas, const char* path);

private:
    template<typename T>
    void AddChild(TiXmlElement* parent, const string& child_name, const T & v);
    void AddDoubleChild(TiXmlElement* parent, const string& child_name, const double & v);
    void AddStringChild(TiXmlElement* parent, const string& child_name, const string & v);

    void WriteStatisticsDataToXmlFile(TiXmlElement *ElementStatisticData, const boost::shared_ptr<StatisticsData>& statistic_data);
    void WriteDownloadDriverStatisticInfoToXmlFile(TiXmlElement *element, const DOWNLOADDRIVER_STATISTIC_INFO& downloader_driver_statistic_info);
    void WriteLiveStatisticInfoToXmlFile(TiXmlElement *element, const LIVE_DOWNLOADDRIVER_STATISTIC_INFO& downloader_driver_statistic_info);
    void WriteP2PDownloaderStatisticsInfoToXmlFile(TiXmlElement *element, const P2PDOWNLOADER_STATISTIC_INFO& p2p_statistic_info);

    void WriteSpeedInfoToXmlFile(TiXmlElement *element, SPEED_INFO speed_info, std::string name);
    void WriteCandidatePeerInfoToXmlFile(TiXmlElement *element, protocol::CandidatePeerInfo LocalPeerInfo, std::string name);
    void WriteTrackerInfoToXmlFile(TiXmlElement *element, protocol::TRACKER_INFO TrackerInfo);
    void WriteStatisticTrackerInfoToXmlFile(TiXmlElement *element, STATISTIC_TRACKER_INFO StatisticTrackerInfo);
    void WriteStatisticIndexInfoToXmlFile(TiXmlElement *element, STATISTIC_INDEX_INFO StatisticIndexInfo);
    void WriteStatisticInfoToXmlFile(TiXmlElement *element, const STASTISTIC_INFO& statistic_info);
    void WritePieceInfoExToXmlFile(TiXmlElement *element, const PIECE_INFO_EX& piece_info_ex, std::string name);
    void WriteHttpDownloadInfoToXmlFile(TiXmlElement *element, const HTTP_DOWNLOADER_INFO& http_download_info);
    void WritePeerDownloadInfo(TiXmlElement *element, const protocol::PEER_DOWNLOAD_INFO& peer_download_info);
    void WritePeerConnectionInfoToXmlFile(TiXmlElement *element, const P2P_CONNECTION_INFO& peer_connection_info);
};