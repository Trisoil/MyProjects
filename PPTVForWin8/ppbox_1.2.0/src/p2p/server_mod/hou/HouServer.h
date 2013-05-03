//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#ifndef _HOU_HOU_SERVER_H
#define _HOU_HOU_SERVER_H

#include "UdpServer.h"
#include "Statistics.h"
#include "AsioServiceRunner.h"
#include <framework/timer/AsioTimerManager.h>
#include "HttpClientOnLibevent.h"
#include <log4cplus/logger.h>

using namespace framework::configure;


namespace hou
{
    class UdpRequest;
    class HttpConnectionsRecycle;
    class HostManager;
    class TimeIntervalLogger;

    class HouServer
        : public boost::enable_shared_from_this<HouServer>,
        public IUdpServerListener
    {
    public:
        static HouServer & Inst() 
        {
            if (!inst_)
            {
                inst_.reset(new HouServer());
            }

            return *inst_;
        }



        bool Start();
        void Stop();
        void SendPacket(boost::asio::ip::udp::endpoint & dest_ep, boost::asio::streambuf const & buffer);
        void HandleHttpResponse(boost::asio::ip::udp::endpoint & dest_ep, boost::asio::streambuf const & buffer, boost::shared_ptr<UdpRequest> udp_request, const HttpClientOnLibevent::ResponseType & response_type);
        std::string GetPacketString() const;
        unsigned short Port() const;
        boost::uint16_t ConcurrentAsyncReceiveNum() const;
        std::string GetVersion() const;
        static void TearDown() 
        {
            inst_.reset(); 
        }

        boost::shared_ptr<Statistics> GetStatistics(const std::string & host)const;

        std::string GetIpForwordHttpHeader() const
        {
            return ip_forword_http_header_;
        }

        std::string GetIpForwordUrlKey() const
        {
            return ip_forword_url_key_;
        }

    private:
        HouServer();
        HouServer(const HouServer&);
        void HandleStop();
        bool LoadConfig();
        void RequestWaitingQueueTimerHandler(const boost::system::error_code& error);
        void LogTimerHandler(const boost::system::error_code& error);
        void OnUdpRecv( boost::asio::ip::udp::endpoint const &src_ep, boost::asio::streambuf & buf);
        void CancelTimeOutRequest();

        
        
     private:
        static boost::shared_ptr<HouServer> inst_;
        boost::shared_ptr<boost::asio::io_service> main_ios_;
        boost::shared_ptr<boost::asio::io_service> second_ios_;
        boost::shared_ptr<AsioServiceRunner> main_io_thread_;
        boost::shared_ptr<AsioServiceRunner> second_io_thread_;
        boost::shared_ptr<HttpConnectionsRecycle> http_connections_recycle_;
        UdpServer::pointer udp_server_;
        unsigned short port_;
        boost::uint16_t concurrent_async_receive_num_;
        boost::uint16_t log_status_interval_in_second_;
        static std::map<std::string, boost::shared_ptr<Statistics> > statistics_map_;
        boost::shared_ptr<Statistics> all_statistics_;
        boost::shared_ptr<HostManager> host_manager_;
        boost::shared_ptr<TimeIntervalLogger> second_ios_delay_time_logger_;
        static const boost::uint32_t MaxRequestKeepTimeInSecond = 10;
        std::string ip_forword_http_header_;
        std::string ip_forword_url_key_;

    public:
        static const std::string version_;
        boost::uint32_t max_set_capacity_;
        boost::uint32_t max_queue_length_;
        boost::uint32_t http_request_timeout_in_second_;
         
        boost::shared_ptr<boost::asio::deadline_timer> log_timer_;
        boost::shared_ptr<boost::asio::deadline_timer> request_waiting_queue_timer_;
        
        std::set<boost::shared_ptr<hou::UdpRequest> > request_set_;
        std::deque<std::pair<std::string,boost::asio::ip::udp::endpoint> > request_waiting_queue_;
    };
}

#endif