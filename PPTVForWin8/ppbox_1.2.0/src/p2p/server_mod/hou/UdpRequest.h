//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#ifndef _HOU_UDP_REQUEST_H_
#define _HOU_UDP_REQUEST_H_

#include <framework/timer/AsioTimerManager.h>
#include "framework/timer/TimeCounter.h"
#include "count_object_allocate.h"
#include "HttpClientOnLibevent.h"

namespace hou
{
    class HostManager;
    class UdpRequest
        : public boost::enable_shared_from_this<UdpRequest>
        #ifdef SN_DUMP_OBJECT
        , public count_object_allocate<UdpRequest>
        #endif
    {
    public:
        UdpRequest(
            boost::asio::ip::udp::endpoint const & endpoint, 
            string const & request,
            boost::uint32_t http_request_timeout_seconds,
            boost::shared_ptr<boost::asio::io_service> io_service,
            boost::shared_ptr<HttpConnectionsRecycle> http_connections_recycle,
            boost::shared_ptr<HostManager> host_manager_);

        boost::uint32_t GetHttpResponseTime()const;
        boost::uint32_t GetResponseTime()const;
        void Execute();
        std::string GetHost()
        {
            return host_;
        }

        boost::uint32_t GetAliveTimeInSecond()
        {
            return time_counter_.elapse() / 1000;
        }

    private:
        bool ParseRequest();
        void SendHttpRequest();
        void OnHttpResponse(const std::string & response, const HttpClientOnLibevent::ResponseType & response_type, bool is_gzip, int error_code, boost::uint32_t response_time);
        void ParseHttpHeaders(const std::string & headers);

        static std::string UrlEncode(const string& url);
        static bool GetItem(const std::string & str, size_t & pos, const std::string & end_delim, std::string & iter);

    private:
        framework::timer::TimeCounter time_counter_;
        boost::asio::ip::udp::endpoint endpoint_;
        boost::asio::streambuf buf_send_;
        boost::shared_ptr<HttpClientOnLibevent> http_client_;
        boost::shared_ptr<boost::asio::io_service> io_service_;
        boost::shared_ptr<HttpConnectionsRecycle> http_connections_recycle_;
        boost::shared_ptr<HostManager> host_manager_;
        std::string host_;
        std::string address_;
        std::string request_;
        std::string path_;
        boost::uint32_t http_request_timeout_in_seconds_;
        std::map<std::string, std::string> http_headers_;

        static const std::string AcceptEncodingHeader;
        static const std::string SID;
        static const std::string Host;
        static const std::string Accept;
        static const std::string Connection;
        static const std::string Colon;
        static const std::string EndLine;

    public:
        boost::uint32_t response_time_;
        boost::uint32_t http_response_time_;
        boost::uint32_t second_ios_delay;
    };
}

#endif