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

    // ֮����û������DOWNLOADDRIVER_STATISTIC_INFO��<<������������Ϊpeer���Ѿ������ˣ����ǲ���������Ҫ�Ľ��
    // ֻ��дһ�������������ostream��
    void OutputDownloaderStatisticInfoToOstream(std::ostream& os, const DOWNLOADDRIVER_STATISTIC_INFO& downloader_driver_statistic_info);

};


// peer���Ѿ�������CandidatePeerInfo��<<������
void OutputCandidatePeerInfoToOstream(std::ostream& os, const protocol::CandidatePeerInfo& info);
// peer���Ѿ�������PIECE_INFO_EX��<<������
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
