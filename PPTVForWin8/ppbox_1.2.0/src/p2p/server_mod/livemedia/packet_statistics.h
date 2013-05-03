//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#ifndef _PACKET_STATISTICS_H_
#define _PACKET_STATISTICS_H_

#include "tinyxml/tinyxml.h"
#include "hash_map.h"

namespace live_media
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
            
        Counter & operator+=(boost::uint32_t num)
        {
            temp_period_num_ += num;
            all_num_ += num;
            return *this;
        }

        boost::uint32_t GetAllNum()const
        {
            return all_num_;
        }

        boost::uint32_t GetPeriodNum()const
        {
            return period_num_;
        }

        std::string GetName()const
        {
            return counter_name_;
        }

    private:
        boost::uint32_t all_num_;
        boost::uint32_t period_num_;
        boost::uint32_t temp_period_num_;
        std::string counter_name_;
    };

    class StatisticsReporter
        :public boost::enable_shared_from_this<StatisticsReporter>
    {
    public:
        StatisticsReporter()
            :file_name_("status.xml.tmp")
        {
        }

        StatisticsReporter(const std::string & file_name)
            :file_name_(file_name)
        {
        }

        bool AddStatus(const std::string & node_name, const std::string & item_name, const boost::uint32_t & item_value);
        bool OpenFile();
        void SaveFile();

    private:
        tinyxml::TiXmlDocument * status_file;
        std::string file_name_;
        std::map<std::string, tinyxml::TiXmlElement *> nodes_;
        tinyxml::TiXmlElement * root_node_;
    };

    class SmoothData
    {
    public:
        SmoothData(const std::string & item_name, boost::uint32_t max_keep_data_size);
        boost::uint32_t GetSmoothData() const;
        void AddData(const boost::uint32_t & data);

    private:
        std::string item_name_;
        boost::uint32_t max_keep_data_size_;
        std::deque<boost::uint32_t> datas_;
        boost::uint32_t smooth_data_;
    };

    class RequestTypes
    {
    public:
        const static string CloseSession;
        const static string Connect;
        const static string RequestAnnounce;
        const static string Announce;
        const static string RequestSubPiece;
    };

    class ResponseTypes
    {
    public:
        const static string Connect;
        const static string Announce;
        const static string Subpiece;
    };
    

    class Statistics
        :public boost::enable_shared_from_this<Statistics>
    {
    public:
        Statistics(boost::shared_ptr<boost::asio::io_service> io_service, const std::string & statistics_name)
            :io_service_(io_service), statistics_name_(statistics_name)
        {
            statistics_interval_in_seconds_ = 5;
        }

        Statistics(const std::string & name)
            :statistics_name_(name)
        {
            statistics_interval_in_seconds_ = 5;
        }
        
        void StartTimer();
        void OnTimer();
        virtual void ResetCounter() = 0;
        const std::map<std::string, boost::uint32_t> & GetMap() const
        {
            return result_map_;
        }
        
        std::string GetName() const
        {
            return statistics_name_;
        }

        virtual ~Statistics(){}

    public:
        boost::uint32_t statistics_interval_in_seconds_; 
        std::string statistics_name_;
        std::map<std::string, boost::uint32_t> result_map_;
    private:
        boost::shared_ptr<boost::asio::io_service> io_service_;
        boost::shared_ptr<boost::asio::deadline_timer> timer_;
    };


    class PacketStatistics
        :public Statistics
    {
        typedef hash_map<std::string, Counter> CounterMap;
    public:
        PacketStatistics(const std::string& packet_name, boost::shared_ptr<boost::asio::io_service> io_service)
            :Statistics(io_service, packet_name)
        {
            subpiece_num_per_request_.reset(new SmoothData("subpiece_num_per_request", 200));
        }

        void AddRequestSubpieceNumData(boost::uint32_t request_subpiece_num);
        void LogStatus();

        Counter& GetStatistics(const std::string& key)
        {
            assert(counter_map_.find(key) != counter_map_.end());
            return counter_map_[key];
        }

        void AddCounter(const std::string& counter_name)
        {
            if (counter_map_.find(counter_name) == counter_map_.end())
            {
                Counter tmp;
                counter_map_.insert(std::make_pair(counter_name,tmp));
            }
        }

    private:
        void ResetCounter();

    private:
        CounterMap counter_map_;
        boost::shared_ptr<SmoothData> subpiece_num_per_request_;
    };

    class HitsStatistics
        :public Statistics
    {
    public:
        HitsStatistics(const std::string& name, int min, int max, size_t interval)
            : Statistics(name)
        {
            min_ = min;
            max_ = max;
            interval_ = interval;
        }

        void AddHit(int diff, size_t count=1)
        {
            if (hits_count_.find(diff) == hits_count_.end())
            {
                hits_count_[diff] = 0;
            }

            hits_count_[diff] += count;
        }

        void ResetCounter()
        {
            hits_count_.clear();
        }

        //(-00, min), [min, min+interval), ...[max-interval, max), [max, +00)
        void GetFrequency(std::map<int, size_t>& frequency, int min, int max, size_t interval);

        void LogStatus();

    private:
        int min_;
        int max_;
        size_t interval_;
        std::map<int, size_t> hits_count_;
    };


    class SpeedStatistics
        :public Statistics
    {
    public:
        SpeedStatistics(boost::shared_ptr<boost::asio::io_service> io_service, const std::string & statistics_name)
            :Statistics(io_service, statistics_name)
        {
        }
        void ResetCounter();
        boost::uint32_t GetCurentCycleSpeedSend();
        void AddData(boost::uint32_t data);
        
    private:
        Counter current_cycle_send_;
    };
}

#endif //_PACKET_STATISTICS_H_