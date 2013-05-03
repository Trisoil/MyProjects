//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#include "Common.h"
#include "HouServer.h"
#include "UdpRequest.h"
#include "Statistics.h"
#include "iostream"
#include "HostManager.h"
#include "TimeIntervalLogger.h"
#include <framework/configure/Config.h>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/cstdint.hpp> 
#include <boost/function.hpp>
#include <framework/timer/TickCounter.h>

#include <fstream>


using namespace framework::configure;

namespace hou
{
    const std::string HouServer::version_ = "1.0.0.7.x64"; 
    boost::shared_ptr<HouServer> HouServer::inst_;
    std::map<std::string, boost::shared_ptr<Statistics> > HouServer::statistics_map_;
    
    HouServer::HouServer()
    {
        http_connections_recycle_.reset(new HttpConnectionsRecycle());
    }

    bool HouServer:: Start()
    {
        LOG4CPLUS_INFO(Loggers::HouService(), "HouServer::Start()");
       
        if (!HttpClientOnLibevent::Init())
        {
            this->Stop();
            return false;
        }
        
        
        main_io_thread_.reset(new AsioServiceRunner("main_io_thread"));
        main_ios_ = main_io_thread_->Start();
        second_io_thread_.reset(new AsioServiceRunner("second_io_thread"));
        second_ios_ = second_io_thread_->Start();

        udp_server_ = UdpServer::Create(*main_ios_, shared_from_this());
        all_statistics_.reset(new Statistics(main_ios_));
        host_manager_.reset(new HostManager(second_ios_));
        second_ios_delay_time_logger_.reset(new TimeIntervalLogger("second_delay_time", second_ios_));

        if (!LoadConfig())
        {
            this->Stop();
            return false;
        }

        all_statistics_->StartTimer();
        host_manager_->StartTimer();
		host_manager_->StartClearTimer();

        for (std::map<std::string, boost::shared_ptr<Statistics> >::iterator iter = statistics_map_.begin();
            iter != statistics_map_.end(); iter++)
        {
            (iter->second)->StartTimer();
        }

        request_waiting_queue_timer_.reset(new boost::asio::deadline_timer(*main_ios_));
        log_timer_.reset(new boost::asio::deadline_timer(*main_ios_));

        request_waiting_queue_timer_->expires_from_now(boost::posix_time::milliseconds(100));
        request_waiting_queue_timer_->async_wait(boost::bind(&hou::HouServer::RequestWaitingQueueTimerHandler, this, _1));
        log_timer_->expires_from_now(boost::posix_time::seconds(log_status_interval_in_second_));
        log_timer_->async_wait(boost::bind(&hou::HouServer::LogTimerHandler, this, _1));
        
        if (!udp_server_->Listen(port_))
        {
            this->Stop();
            return false;
        }
        
        udp_server_->Recv(concurrent_async_receive_num_);
        LOG4CPLUS_INFO(Loggers::HouService(), "HouService started.");
        std::cout << "HouService started." << std::endl << std::endl;
        return true;
    }

    void HouServer::HandleStop()
    {
        main_io_thread_->Stop();
        second_io_thread_->Stop();
    }

    void HouServer::Stop()
    {
        LOG4CPLUS_INFO(Loggers::HouService(), "HouServer::Stop()");
        if (main_ios_)
        {
            main_ios_->post(&HttpClientOnLibevent::Exit);
            main_ios_->post(bind(&HouServer::HandleStop, shared_from_this()));
        }

        boost::system::error_code error_code;
        this->request_waiting_queue_timer_->cancel(error_code);
        this->log_timer_->cancel(error_code);
        udp_server_.reset();

        std::cout << "Service stopped." << std::endl;
    }
    
    unsigned short HouServer::Port() const
    { 
        return port_; 
    }

    boost::uint16_t HouServer::ConcurrentAsyncReceiveNum() const
    { 
        return concurrent_async_receive_num_ ;
    }

    std::string HouServer::GetVersion() const
    {
        return version_;
    }
    

    void HouServer::OnUdpRecv( boost::asio::ip::udp::endpoint  const & src_ep, boost::asio::streambuf & buf)
    {	
        ++(all_statistics_->packet_received_);
        string request;
        request.resize(buf.size());
        memcpy((void*)request.c_str(), boost::asio::buffer_cast<const char *>(*buf.data().begin()), buf.size());

        if(request_waiting_queue_.size() < max_queue_length_)
        {
            request_waiting_queue_.push_back(std::make_pair(request,src_ep));    
        }
        else
        {
            if (request_waiting_queue_.size() > 0)
            {
                request_waiting_queue_.pop_front();
                request_waiting_queue_.push_back(std::make_pair(request,src_ep));
            }
            ++(all_statistics_->packet_dropped_);
        }
    }


    void HouServer::HandleHttpResponse(boost::asio::ip::udp::endpoint & dest_ep, boost::asio::streambuf const & buffer, 
        boost::shared_ptr<UdpRequest> udp_request, const HttpClientOnLibevent::ResponseType & response_type)
    {
        if (response_type == HttpClientOnLibevent::Success)
        {
            all_statistics_->AddTimeData("response_time",udp_request->GetResponseTime());
            all_statistics_->AddTimeData("http_response_time", udp_request->GetHttpResponseTime());
            all_statistics_->AddTimeData("second_ios_delay", second_ios_delay_time_logger_->GetQueueLoadStatus().total_milliseconds());
            SendPacket(dest_ep, buffer); 

        } 
        else if (response_type == HttpClientOnLibevent::ParseFailed)
        {
            ++(HouServer::Inst().GetStatistics(Statistics::AllStatisticsString)->parsed_failed_);
        }
     
        if (HouServer::Inst().GetStatistics(udp_request->GetHost()))
        {
            ++(HouServer::Inst().GetStatistics(udp_request->GetHost())->packet_received_);
        }
        
        if (request_set_.find(udp_request) != request_set_.end())
        {
            request_set_.erase(udp_request);
        }
        else
        {
            LOG4CPLUS_ERROR(Loggers::HouService(), "Can't erase udprequest !! request set size: " << request_set_.size() <<
                "host: " << udp_request->GetHost() << "response type: " << response_type);
        }
    }

    void HouServer:: SendPacket(boost::asio::ip::udp::endpoint & dest_ep, boost::asio::streambuf const & buffer)
    {
        udp_server_->SendTo(dest_ep,buffer);
    }

    std::string HouServer::GetPacketString() const
    {
        return all_statistics_->GetStateString();
    }

    bool HouServer::LoadConfig()
    {
        int host_num = 0;

        framework::configure::Config conf("hou.conf");
        conf.register_module("HouServer")
            << CONFIG_PARAM_NAME_RDONLY("port", port_)
            << CONFIG_PARAM_NAME_RDONLY("concurrent_async_receive_num", concurrent_async_receive_num_)
            << CONFIG_PARAM_NAME_RDONLY("max_set_capacity", max_set_capacity_)
            << CONFIG_PARAM_NAME_RDONLY("max_queue_length", max_queue_length_)
            << CONFIG_PARAM_NAME_RDONLY("http_request_timeout_in_second", http_request_timeout_in_second_)
            << CONFIG_PARAM_NAME_RDONLY("log_status_interval_in_second", log_status_interval_in_second_)
            << CONFIG_PARAM_NAME_RDONLY("ip_forword_http_header", ip_forword_http_header_)
            << CONFIG_PARAM_NAME_RDONLY("ip_forword_url_key",ip_forword_url_key_);
        
        if (ip_forword_http_header_.empty() && ip_forword_url_key_.empty())
        {
            std::cout << "ip_forword_http_header_ and ip_forword_url_key_ read from config are both empty!" << std::endl;
            LOG4CPLUS_WARN(Loggers::HouService(), "ip_forword_http_header_ and ip_forword_url_key_ read from config are both empty!");
        }

        conf.register_module("Hosts")
            << CONFIG_PARAM_NAME_RDONLY("host_num", host_num);

        for (int i = 1;i <=  host_num; ++i )
        {

            try
            {
                std::string host_name;
                std::string host_key("host_");
                host_key += boost::lexical_cast<std::string>(i);
                conf.register_module("Hosts")
                    << CONFIG_PARAM_NAME_RDONLY(host_key.c_str(), host_name);

                boost::shared_ptr<Statistics> statistics(new Statistics(main_ios_));
                if (!statistics)
                {
                    LOG4CPLUS_ERROR(Loggers::HouService(), "HouServer started failed. New statistics instance failed.");
                    return false;
                }
                
                statistics_map_[host_name] = statistics;
                host_manager_->ResolveHostName(host_name);
            }
            catch (boost::bad_lexical_cast & e)
            {
                std::cout << "lexical_cast exception." << std::endl;
                LOG4CPLUS_WARN(Loggers::HouService(), "lexical_cast exception. error_message: " << e.what());
                return false;
            }
        }
    
        boost::shared_ptr<Statistics> statistics(new Statistics(main_ios_));
        if (!statistics)
        {
            LOG4CPLUS_ERROR(Loggers::HouService(), "HouServer started failed. New statistics instance failed.");
            return false;
        }

        statistics_map_[std::string("unknown_host")] = statistics;
        return true;
    }

    void HouServer::RequestWaitingQueueTimerHandler(const boost::system::error_code& err)
    {
        while(request_set_.size() < max_set_capacity_ && (!request_waiting_queue_.empty()))
        {
            boost::shared_ptr<UdpRequest> new_udp_request(
                new UdpRequest(
                request_waiting_queue_.front().second, 
                request_waiting_queue_.front().first,
                http_request_timeout_in_second_, 
                main_ios_, 
                http_connections_recycle_,
                host_manager_));
            request_waiting_queue_.pop_front();
            //note thatthe ececute will call back to erase the request 
            //we shoulde insert the request before execute because otherwise the execute may callback before
            //we insert the request which result in no chance to erase the request.
            request_set_.insert(new_udp_request);


            second_ios_->post(boost::bind(&UdpRequest::Execute, new_udp_request));
            second_ios_->post(boost::bind(&TimeIntervalLogger::Touch, second_ios_delay_time_logger_));
        }

        
        boost::function<void ()> func;
        func = &hou::HttpClientOnLibevent::Run;
        second_ios_->post(func);

        http_connections_recycle_->Recycle();
        CancelTimeOutRequest();
        request_waiting_queue_timer_->expires_from_now(boost::posix_time::milliseconds(100));
        request_waiting_queue_timer_->async_wait(boost::bind(&hou::HouServer::RequestWaitingQueueTimerHandler, this, _1));
    }

    void HouServer::CancelTimeOutRequest()
    {
        for (std::set<boost::shared_ptr<UdpRequest> >::iterator iter = request_set_.begin();
            iter != request_set_.end(); )
        {
            if ((*iter)->GetAliveTimeInSecond() > MaxRequestKeepTimeInSecond)
            {
                LOG4CPLUS_WARN(Loggers::HouService(), "request timeout for 10 seconds, already removed: " << (*iter)->GetHost() );
                request_set_.erase(iter++);
            }
            else
            {
                iter++;
            }
        }
    }

    void HouServer::LogTimerHandler(const boost::system::error_code& error)
    {
        StatisticsReporter reporter;
        reporter.DeleteFileIfExist();
        reporter.CreateNewFile();

        all_statistics_->LogStatus();

        if (!reporter.OpenFile())
        {
            return ;
        }

        reporter.AddStatus("service_status", "version", version_);
        reporter.AddStatus("service_status", "udp_port" , port_);
        reporter.AddStatus("service_status", "processing_set_size", request_set_.size());
        reporter.AddStatus("service_status", "waiting_queue_length", request_waiting_queue_.size());

        for (std::map<std::string, boost::shared_ptr<Statistics> >::iterator iter = statistics_map_.begin();
            iter != statistics_map_.end(); iter++)
        {
            reporter.AddStatus("packet_num", iter->first, iter->second->packet_received_.GetPeriodNum()  / Statistics::statistics_interval_in_seconds);
       
            LOG4CPLUS_INFO(Loggers::HttpService(), iter->first << " : http time out: " << iter->second->http_request_time_out_.GetPeriodNum() / iter->second->statistics_interval_in_seconds << " " << iter->second->GetHttpTimeOutRate() 
                << "% http connect fail: " << iter->second->http_connection_failed_.GetPeriodNum() / iter->second->statistics_interval_in_seconds<< " " << iter->second->GetHttpConnectFailRate() 
                << "% http download fail:" << iter->second->http_download_failed_.GetPeriodNum() / iter->second->statistics_interval_in_seconds<< " " << iter->second->GetDownloadFailRate() << "%");
        }

        LOG4CPLUS_INFO(Loggers::HttpService(), "request_set_length: " << request_set_.size() << " waiting_queue_length: " << request_waiting_queue_.size());
        
        {
            boost::mutex::scoped_lock lock(HostManager::map_mutex_);
            std::map<std::string, boost::shared_ptr<Host> > host_map = host_manager_->GetHostMap();
            for (std::map<std::string, boost::shared_ptr<Host> >::iterator map_iter = host_map.begin();
                map_iter != host_map.end(); map_iter++)
            {
                if (map_iter->second)
                {
                    std::vector<std::string> hosts = (map_iter->second)->GetHosts();
                    for(std::vector<std::string>::iterator vec_iter = hosts.begin();
                        vec_iter != hosts.end(); vec_iter++)
                        reporter.AddStatus("hosts", "h_" + map_iter->first, *vec_iter);
                }
                else
                {
                    //将解析失败的域名记录到 status.xml 中
                    reporter.AddStatus("resolve_fail_host", "name", map_iter->first);
                }
            }
        }

        reporter.SaveFile();
    
        
        log_timer_->expires_from_now(boost::posix_time::seconds(log_status_interval_in_second_));
        log_timer_->async_wait(boost::bind(&hou::HouServer::LogTimerHandler, this, _1));
    }

    boost::shared_ptr<Statistics> HouServer::GetStatistics(const std::string & host)const
    {
        if (host == Statistics::AllStatisticsString)
        {
            return all_statistics_;
        }

        std::map<std::string, boost::shared_ptr<Statistics> >::const_iterator iter = statistics_map_.find(host);
        if(iter == statistics_map_.end())
        {
            LOG4CPLUS_WARN(Loggers::HouService() , "receive unknown packet! host: " << host);
            return statistics_map_["unknown_host"];
        }

        return iter->second;
    }

    
}