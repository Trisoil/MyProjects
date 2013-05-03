//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "pre.h"
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include "packet_statistics.h"


namespace live_media
{
    const string RequestTypes::CloseSession = "close_session_packet";
    const string RequestTypes::Connect = "connect_packet";
    const string RequestTypes::RequestAnnounce = "request_announce_packet";
    const string RequestTypes::Announce = "announce_packet";
    const string RequestTypes::RequestSubPiece = "request_subpiece_packet";

    const string ResponseTypes::Connect = "connect_packet";
    const string ResponseTypes::Announce = "announce_packet";
    const string ResponseTypes::Subpiece = "subpiece_packet";

    void Statistics::StartTimer()
    {
        timer_.reset(new boost::asio::deadline_timer(*io_service_));
        timer_->expires_from_now(boost::posix_time::seconds(statistics_interval_in_seconds_));
        
        try
        {
            timer_->async_wait(boost::bind(&Statistics::OnTimer, shared_from_this()));
        }
        catch (const std::exception & e)
        {
            std::cout << e.what() << std::endl;
        }
        
        
    }

    void PacketStatistics::ResetCounter()
    {
        for (CounterMap::iterator iter = counter_map_.begin();
            iter != counter_map_.end(); iter++)
        {
            (iter->second).SetCounter();
        }
    }

    void Statistics::OnTimer()
    {
        ResetCounter();
        timer_->expires_from_now(boost::posix_time::seconds(statistics_interval_in_seconds_));
        timer_->async_wait(boost::bind(&Statistics::OnTimer, shared_from_this()));
    }

    void PacketStatistics::LogStatus()
    {
        result_map_.clear();
        for (CounterMap::iterator iter = counter_map_.begin();
            iter != counter_map_.end(); iter++)
        {
            result_map_[iter->first] = ((iter->second).GetPeriodNum() / statistics_interval_in_seconds_);
        }
    }

    void HitsStatistics::LogStatus()
    {
        result_map_.clear();

        std::map<int, size_t> frequency;
        GetFrequency(frequency, min_, max_, interval_);

        std::map<int, size_t>::const_iterator prev = frequency.begin();
        for(std::map<int, size_t>::const_iterator i = frequency.begin();
            i != frequency.end();
            prev=i++)
        {
            std::ostringstream range_stream;
            if (i == frequency.begin())
            {
                range_stream<<"(-OO, "<<i->first<<"]";
            }
            else 
            {
                range_stream<<"["<<prev->first<<", ";

                std::map<int, size_t>::const_iterator next = i;
                ++next;
                if (next == frequency.end())
                {
                    range_stream<<"+00)";
                }
                else
                {
                    range_stream<<i->first<<"]";
                }
            }

            std::ostringstream item_name;

            if (i->first < 0)
            {
                item_name << "n_";
                if (i->first > -100)
                {
                    item_name << "0";
                }

                item_name << abs(i->first);
            }
            else
            {
                item_name << "p_";
                if (i->first < 100)
                {
                    item_name << "0";
                }

                if(i->first == 0)
                {
                    item_name << "0";
                }

                item_name << i->first;
            }

            result_map_[item_name.str()] = i->second;
        }
    }

    void HitsStatistics::GetFrequency(std::map<int, size_t>& frequency, int min, int max, size_t interval)
    {
        assert(min < max);
        assert(interval > 0);
        assert((max - min)%interval == 0);

        frequency.clear();

        for(int upper_bound = min; upper_bound <= static_cast<int>(max+interval); upper_bound+= interval)
        {
            frequency.insert(std::make_pair(upper_bound, 0));
        }

        for(std::map<int,size_t>::iterator i = hits_count_.begin();
            i != hits_count_.end(); 
            ++i)
        {
            if (i->first < min)
            {
                frequency[min] += i->second;
                continue;
            }

            if (i->first >= max)
            {
                frequency[max+interval] += i->second;
                continue;
            }

            size_t slot_index = 1 + (i->first - min)/interval;
            frequency[min + slot_index*interval] += i->second;
        }
    }

    void PacketStatistics::AddRequestSubpieceNumData(boost::uint32_t request_subpiece_num)
    {
        subpiece_num_per_request_->AddData(request_subpiece_num);
    }

    bool StatisticsReporter::OpenFile()
    {
        status_file = new tinyxml::TiXmlDocument(file_name_.c_str());
        if (status_file->LoadFile())
        {
            root_node_ = status_file->FirstChild("live_status")->ToElement();
            return true;
        }
        
        return false;
    }

    void StatisticsReporter::SaveFile()
    {
        if (status_file)
        {
            status_file->SaveFile();
            delete status_file;
            status_file = NULL;
        }
    }

    bool StatisticsReporter::AddStatus(const std::string & node_name, const std::string & item_name, const boost::uint32_t & item_value)
    {
        try
        {
            if (nodes_.find(node_name) == nodes_.end())
            {
                tinyxml::TiXmlElement node_to_add(node_name);
                tinyxml::TiXmlElement *node = root_node_->InsertEndChild(node_to_add)->ToElement();
                nodes_[node_name] = node;
            }

            nodes_[node_name]->InsertEndChild(tinyxml::TiXmlElement(item_name))->ToElement()->InsertEndChild(tinyxml::TiXmlText(boost::lexical_cast<std::string>(item_value)));
            return true;
        }
        catch (boost::bad_lexical_cast &)
        {
            std::cout << "lexical_cast exception." << std::endl;
            LOG4CPLUS_WARN(Loggers::UdpServer(), "lexical_cast exception.");
            return false;
        }
    }

    SmoothData::SmoothData(const std::string & item_name, boost::uint32_t max_keep_data_size)
        :item_name_(item_name), max_keep_data_size_(max_keep_data_size)
    {
        smooth_data_ = 0;
    }

    void SmoothData::AddData(const boost::uint32_t & data)
    {
        if (datas_.size() >= max_keep_data_size_)
        {
            datas_.pop_front();
        }

        datas_.push_back(data);
    }

    boost::uint32_t SmoothData::GetSmoothData() const
    {
        if (datas_.size() == 0)
        {
            return 0;
        }

        boost::uint32_t sum = 0;
        for (std::deque<boost::uint32_t>::const_iterator iter = datas_.begin();
            iter != datas_.end(); iter++)
        {
            sum += *iter;
        }

        return sum / datas_.size();
    }

    void SpeedStatistics::AddData(boost::uint32_t data)
    {
        current_cycle_send_ += data;
    }

    boost::uint32_t SpeedStatistics::GetCurentCycleSpeedSend()
    {
        return current_cycle_send_.GetPeriodNum() / statistics_interval_in_seconds_;
    }

    void SpeedStatistics::ResetCounter()
    {
        current_cycle_send_.SetCounter();
    }
}