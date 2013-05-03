//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#include "Common.h"
#include "Statistics.h"

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>


namespace hou
{
    const string Statistics::AllStatisticsString = "all";
    void Statistics::StartTimer()
    {
        timer_.reset(new boost::asio::deadline_timer(*io_service_));
        timer_->expires_from_now(boost::posix_time::seconds(statistics_interval_in_seconds));
        timer_->async_wait(boost::bind(&hou::Statistics::ResetCounter, shared_from_this()));
    }

    void Statistics::ResetCounter()
    {
        packet_dropped_.SetCounter();
        successful_response_.SetCounter();
        packet_received_.SetCounter();
        parsed_failed_.SetCounter();
        http_request_time_out_.SetCounter();
        http_download_failed_.SetCounter();
        http_connection_failed_.SetCounter();

        timer_->expires_from_now(boost::posix_time::seconds(statistics_interval_in_seconds));
        timer_->async_wait(boost::bind(&hou::Statistics::ResetCounter, shared_from_this()));
    }

    boost::uint32_t Statistics::GetAverage(const std::string name )
    {
        boost::mutex::scoped_lock lock(data_map_mutex_);
        if (datas_map_.find(name) == datas_map_.end())
        {
            return 0;
        }

        const std::deque<boost::uint32_t> & datas = (datas_map_.find(name)->second);
        boost::uint32_t total = 0;
        for (std::deque<boost::uint32_t>::const_iterator iter = datas.begin();
            iter != datas.end(); iter++)
        {
            total += *iter;
        }

        return datas.size() > 0 ? total / datas.size() : 0;
    }

    std::string Statistics::GetStateString()
    {
        std::ostringstream oss;
        oss << "Packet Received: " << packet_received_.GetAllNum() << std::endl
            << "Successful Response: " << successful_response_.GetAllNum() << std::endl
            << "packet Dropped: " << packet_dropped_.GetAllNum() << std::endl
            << "Parse Failed: " << parsed_failed_.GetAllNum() << std::endl
            << "Download Failed: " << http_download_failed_.GetAllNum() << std::endl 
            << "Http Connection Failed: " << http_connection_failed_.GetAllNum() << std::endl
            << "Http Time Out:" << http_request_time_out_.GetAllNum() << std::endl << std::endl;

        oss << "Packet Received In 1 sec: " << packet_received_.GetPeriodNum() / statistics_interval_in_seconds<< std::endl
            << "Successful Response In 1 sec: " << successful_response_.GetPeriodNum() / statistics_interval_in_seconds << std::endl
            << "packet Dropped In 1 sec: " << packet_dropped_.GetPeriodNum() / statistics_interval_in_seconds  << std::endl
            << "Parse Failed In 1 sec: " << parsed_failed_.GetPeriodNum() / statistics_interval_in_seconds << std::endl
            << "Download Failed In 1 sec: " << http_download_failed_.GetPeriodNum() / statistics_interval_in_seconds << std::endl
            << "Http Connection Failed In 1 sec" << http_download_failed_.GetPeriodNum() / statistics_interval_in_seconds << std::endl
            << "Http Request Time Out In 1 sec: " << http_request_time_out_.GetPeriodNum() / statistics_interval_in_seconds<< std::endl
            << "Http Time Out Rate: " << GetHttpTimeOutRate() << "%" << std::endl
            << "Packet Dropped Rate: " << GetPacketDropRate() << "%" << std::endl
            << "Everage Response Time: " << GetAverage("response_time")<< "ms" << std::endl
            << "Everage Http Response Time: " << GetAverage("http_response_time") << "ms" << std::endl 
            << "Everage Second Ios Delay: " << GetAverage("second_ios_delay") << "ms"  << std::endl << std::endl;

        return oss.str();        
    }
    
    int Statistics::GetPacketDropRate()
    {
        return packet_received_.GetPeriodNum() == 0 ? 0 : (packet_dropped_.GetPeriodNum() * 100 / packet_received_.GetPeriodNum());
    }

    int Statistics::GetHttpTimeOutRate()
    {
        int total_num = GetTotalPeriodResponse();
        return total_num == 0 ? 0 : (http_request_time_out_.GetPeriodNum() * 100 / total_num);
    }

    int Statistics::GetDownloadFailRate()
    {
        int total_num = GetTotalPeriodResponse();
        return (total_num == 0 ? 0 : (http_download_failed_.GetPeriodNum() * 100 / total_num));
    }

    int Statistics::GetHttpConnectFailRate()
    {
        int total_num = GetTotalPeriodResponse();
        return (total_num == 0 ? 0 : (http_connection_failed_.GetPeriodNum() * 100 / total_num));
    }
    
    int Statistics::GetTotalPeriodResponse()
    {
        return successful_response_.GetPeriodNum() + http_request_time_out_.GetPeriodNum() + http_connection_failed_.GetPeriodNum() + http_download_failed_.GetPeriodNum();
    }
    void Statistics::LogStatus()
    {
        StatisticsReporter reporter;
        if (!reporter.OpenFile())
        {
            return;
        }
        
        reporter.AddStatus("all_status", "packet_received", packet_received_.GetAllNum());
        reporter.AddStatus("all_status", "successful_response", successful_response_.GetAllNum());
        reporter.AddStatus("all_status", "parse_failed", parsed_failed_.GetAllNum());
        reporter.AddStatus("all_status", "packet_dropped", packet_dropped_.GetAllNum());
        reporter.AddStatus("all_status", "http_time_out", http_request_time_out_.GetAllNum());
        reporter.AddStatus("all_status", "http_connection_failed", http_connection_failed_.GetAllNum());
        reporter.AddStatus("all_status", "http_download_failed", http_download_failed_.GetAllNum());
        reporter.AddStatus("period_num", "packet_received", packet_received_.GetPeriodNum() / statistics_interval_in_seconds);
        reporter.AddStatus("period_num", "successful_response", successful_response_.GetPeriodNum() / statistics_interval_in_seconds);
        reporter.AddStatus("period_num", "parse_failed", parsed_failed_.GetPeriodNum() / statistics_interval_in_seconds);
        reporter.AddStatus("period_num", "packet_dropped", packet_dropped_.GetPeriodNum() / statistics_interval_in_seconds);
        reporter.AddStatus("period_num", "http_time_out", http_request_time_out_.GetPeriodNum() / statistics_interval_in_seconds);
        reporter.AddStatus("period_num", "http_connection_failed", http_connection_failed_.GetPeriodNum() / statistics_interval_in_seconds);
        reporter.AddStatus("period_num", "http_download_failed", http_download_failed_.GetPeriodNum() / statistics_interval_in_seconds);
        reporter.AddStatus("period_num", "average_response_time", GetAverage("response_time"));
        reporter.AddStatus("period_num", "average_http_response_time", GetAverage("http_response_time"));
        reporter.AddStatus("period_num", "second_ios_delay", GetAverage("second_delay_time"));

        reporter.SaveFile();
    }

    void Statistics::AddTimeData(const std::string & item_name, boost::uint32_t time)
    {
        boost::mutex::scoped_lock lock(data_map_mutex_);
        if(datas_map_.find(item_name) == datas_map_.end())
        {
            AddDataItem(item_name);
        }

        std::map<std::string, std::deque<boost::uint32_t> >::iterator iter = datas_map_.find(item_name);
        if (iter->second.size() >= max_size_of_response_time_datas)
        {
            iter->second.pop_front();
        }

        iter->second.push_back(time);
    }

    StatisticsReporter::StatisticsReporter()
    {
        status_file = new tinyxml::TiXmlDocument("status.xml");
    }

    void StatisticsReporter::CreateNewFile()
    {
        tinyxml::TiXmlDeclaration *XmlDec = new tinyxml::TiXmlDeclaration("1.0", "utf-8", "yes");
        status_file->LinkEndChild(XmlDec);
        root_node_ = status_file->InsertEndChild(tinyxml::TiXmlElement("hou_status"))->ToElement();
        SaveFile();
    }

    bool StatisticsReporter::OpenFile()
    {
        try
        {   
            boost::filesystem::path path(boost::filesystem::current_path().string());
            path /= std::string("status.xml");

            if (!boost::filesystem::exists(path))
            {
                LOG4CPLUS_ERROR(Loggers::HouService(), "status.xml does not exist when openfile");
                return false;
            }
        }
        catch (boost::filesystem::basic_filesystem_error<boost::filesystem::path> &e)
        {
            LOG4CPLUS_ERROR(Loggers::HouService(), "boost::filesystem::exists() throw an eception. error_message: " << e.what() );
            return false;
        }

        if (status_file)
        {
            status_file->LoadFile();
            if (status_file->FirstChild("hou_status"))
            {
                root_node_ = status_file->FirstChild("hou_status")->ToElement();
                return true;
            }
            else
            {
                LOG4CPLUS_ERROR(Loggers::HouService(), "load statux.xml throw an exception, cant't find root node hou_status.");
                return false;
            }
        }
        
        return false;
    }

    void StatisticsReporter::DeleteFileIfExist()
    {
        try
        {   
            boost::filesystem::path path(boost::filesystem::current_path().string());
            path /= std::string("status.xml");
            if (boost::filesystem::exists(path))
            {
                boost::filesystem::remove(path);
            }
        }
        catch (boost::filesystem::basic_filesystem_error<boost::filesystem::path> & e)
        {
            std::cout << "boost::filesystem::exists() throw an eception. error_message: " << e.what() << std::endl;
            LOG4CPLUS_WARN(Loggers::HouService(), "boost::filesystem::exists() throw an eception. error_message: " << e.what());
            return;
        }
    }

    void StatisticsReporter::SaveFile()
    {
        if (status_file)
        {
            status_file->SaveFile();
        }
    }


    bool StatisticsReporter::AddStatus(const std::string & node_name, const std::string & item_name, const std::string & item_value)
    {
       
        if (nodes_.find(node_name) == nodes_.end())
        {
            tinyxml::TiXmlElement node_to_add(node_name);
            tinyxml::TiXmlElement *node = root_node_->InsertEndChild(node_to_add)->ToElement();
            nodes_[node_name] = node;
        }

        nodes_[node_name]->InsertEndChild(tinyxml::TiXmlElement(item_name))->ToElement()->InsertEndChild(tinyxml::TiXmlText(item_value));
        return true;
    }

    bool StatisticsReporter::AddStatus(const std::string & node_name, const std::string & item_name, const boost::uint32_t item_value)
    {
        try
        {   
            AddStatus(node_name, item_name, boost::lexical_cast<std::string>(item_value));
        }
        catch (boost::filesystem::basic_filesystem_error<boost::filesystem::path> &e)
        {
            LOG4CPLUS_ERROR(Loggers::HouService(), "boost::filesystem::exists() throw an eception. error_message: " << e.what() );
            return false;
        }
        
        return true;
    }

    void Statistics::AddDataItem(const std::string & item_name)
    {
        if(datas_map_.find(item_name) == datas_map_.end())
        {
            std::deque<boost::uint32_t> datas;
            datas_map_[item_name] = datas;
        }
        else
        {
            LOG4CPLUS_WARN(Loggers::HouService(), "Data item: " << item_name << " is already exist.");
        }
    }
}
