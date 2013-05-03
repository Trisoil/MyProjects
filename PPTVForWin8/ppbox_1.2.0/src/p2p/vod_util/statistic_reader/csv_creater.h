#include "stdafx.h"
#include "tinyxml/tinyxml.h"
#include "statistic/StatisticStructs.h"
#include "statistic/StatisticsData.h"

using namespace statistic;

class CsvCreater
{
public:
    void CreateCsvFile(const std::vector<boost::shared_ptr<StatisticsData> >& statistic_datas);
    void SimpleWriteToCSVFile(const std::vector<boost::shared_ptr<StatisticsData> >& statistic_datas, const char* path);

private:
    void WriteStatisticDataToCsvFile(std::ostringstream& oss, const boost::shared_ptr<StatisticsData>& statistic_data);

    void WriteDownloadDriverStatisticInfoToCsvFile(std::ostringstream& oss,
        const DOWNLOADDRIVER_STATISTIC_INFO& downloader_driver_statistic_info);

    void WriteLiveStatisticInfoToCsvFile(std::ostringstream& oss,
        const LIVE_DOWNLOADDRIVER_STATISTIC_INFO& downloader_driver_statistic_info);

    void WriteP2PDownloaderStatisticsInfoToCsvFile(std::ostringstream& oss,
        const P2PDOWNLOADER_STATISTIC_INFO& p2p_statistic_info);

    // 之所以没有重载DOWNLOADDRIVER_STATISTIC_INFO的<<操作符，是因为peer中已经重载了，但是不是我们想要的结果
    // 只好写一个函数来输出到ostream中
    void OutputDownloaderStatisticInfoToOstream(std::ostream& os, const DOWNLOADDRIVER_STATISTIC_INFO& downloader_driver_statistic_info);

};


// peer中已经重载了CandidatePeerInfo的<<操作符
void OutputCandidatePeerInfoToOstream(std::ostream& os, const protocol::CandidatePeerInfo& info);
// peer中已经重载了PIECE_INFO_EX的<<操作符
void OutputPieceInfoExToOstream(std::ostream& os, const PIECE_INFO_EX& info);

std::ostream& operator << (std::ostream& os, const SPEED_INFO& speed_info);
std::ostream& operator << (std::ostream& os, const STATISTIC_TRACKER_INFO& info);
std::ostream& operator << (std::ostream& os, const STATISTIC_INDEX_INFO& info);

std::ostream& operator << (std::ostream& os, const STASTISTIC_INFO& info);
std::ostream& operator << (std::ostream& os, const P2P_CONNECTION_INFO& info);
std::ostream& operator << (std::ostream& os, const P2PDOWNLOADER_STATISTIC_INFO& info);
std::ostream& operator << (std::ostream& os, const HTTP_DOWNLOADER_INFO& info);

std::ostream& operator << (std::ostream& os, const LIVE_DOWNLOADDRIVER_STATISTIC_INFO& info);

std::ostream& operator << (std::ostream& os, const protocol::PEER_DOWNLOAD_INFO& info);
std::ostream& operator << (std::ostream& os, const protocol::TRACKER_INFO& info);
