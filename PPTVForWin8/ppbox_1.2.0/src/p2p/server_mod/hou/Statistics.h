//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#ifndef _HOU_STATISTICS_H_
#define _HOU_STATISTICS_H_
#include "tinyxml/tinyxml.h"

namespace hou
{
    class Counter
    {
    public:
        Counter()
        {
            all_num_ = 0;
            period_num_ = 0;
            temp_period_num_ = 0;
        }

        Counter& operator++()
        {
            ++all_num_;
            ++temp_period_num_;
            return *this;
        }

        void SetCounter()
        {
            period_num_ = temp_period_num_;
            temp_period_num_ = 0;
        }

        boost::uint32_t GetAllNum()const
        {
            return all_num_;
        }

        boost::uint32_t GetPeriodNum()const
        {
            return period_num_;
        }
    private:
        boost::uint32_t all_num_;
        boost::uint32_t period_num_;
        boost::uint32_t temp_period_num_;
    };

    class StatisticsReporter
        :public boost::enable_shared_from_this<StatisticsReporter>
    {
    public:
        StatisticsReporter();

        ~StatisticsReporter()
        {
            delete status_file;
            status_file = NULL;
        }
        void DeleteFileIfExist();
        void CreateNewFile();
        bool AddStatus(const std::string & node_name, const std::string & item_name, const std::string & item_value);
        bool AddStatus(const std::string & node_name, const std::string & item_name, const boost::uint32_t item_value);
        bool OpenFile();
        void SaveFile();

    private:
        tinyxml::TiXmlDocument * status_file;
        tinyxml::TiXmlElement * root_node_;
        std::map<std::string , tinyxml::TiXmlElement *> nodes_;
    };

    class Statistics
        :public boost::enable_shared_from_this<Statistics>
    {
    public:
        Statistics(boost::shared_ptr<boost::asio::io_service> io_service)
            :io_service_(io_service)
        {
        }

        void StartTimer();
        std::string GetStateString();
        void AddTimeData(const std::string & item_name, boost::uint32_t response_time);
        void LogStatus();


    private:
        void ResetCounter();
        void AddDataItem(const std::string & item_name);

    public:
        int GetPacketDropRate();
        int GetHttpTimeOutRate();
        int GetDownloadFailRate();
        int GetHttpConnectFailRate();
        int GetTotalPeriodResponse();
        boost::uint32_t GetAverage(const std::string name );


    public:
        Counter packet_received_;
        Counter successful_response_;
        Counter parsed_failed_;
        Counter packet_dropped_;
        Counter http_download_failed_;
        Counter http_request_time_out_;
        Counter http_connection_failed_;
    
    private:
        boost::shared_ptr<boost::asio::io_service> io_service_;
        boost::shared_ptr<boost::asio::deadline_timer> timer_;
        std::map<std::string, std::deque<boost::uint32_t> > datas_map_;
        static const boost::uint32_t max_size_of_response_time_datas = 200;


    public:
        static const boost::uint32_t statistics_interval_in_seconds = 5;
        static const string AllStatisticsString;
        boost::mutex data_map_mutex_;

    };

}

#endif