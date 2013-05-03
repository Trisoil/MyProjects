//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#include "Common.h"
#include "UdpRequest.h"
#include "HouServer.h"
#include "HostManager.h"
#include "boost/algorithm/string.hpp"

namespace hou
{
    const string UdpRequest::AcceptEncodingHeader = "Accept-Encoding";
    const string UdpRequest::SID = "SID";
    const string UdpRequest::Host = "Host";
    const string UdpRequest::Accept = "Accept";
    const string UdpRequest::Connection = "Connection";
    const string UdpRequest::Colon = ":";
    const string UdpRequest::EndLine = "\r\n";

    UdpRequest::UdpRequest(
        boost::asio::ip::udp::endpoint const & endpoint,
        string const & request,
        boost::uint32_t http_request_timeout_in_seconds,
        boost::shared_ptr<boost::asio::io_service> io_service,
        boost::shared_ptr<HttpConnectionsRecycle> http_connections_recycle,
        boost::shared_ptr<HostManager> host_manager)
        : endpoint_(endpoint), request_(request), io_service_(io_service),
        http_request_timeout_in_seconds_(http_request_timeout_in_seconds),
        http_connections_recycle_(http_connections_recycle),
        host_manager_(host_manager)
    {
        response_time_ = 0;
        http_response_time_ = 0;
        second_ios_delay = 0;

        // Accept-Encoding缺省使用"gzip"，这是因为当前client过来的下载play的请求并没有指定Accept-Encoding，
        // 而事实上有很多play是需要压缩(>1KB)的。
        http_headers_.insert(std::make_pair(AcceptEncodingHeader, "gzip"));
        http_headers_.insert(std::make_pair(Accept, "*/*"));
        http_headers_.insert(std::make_pair(Connection, "close"));

        std::string ip_forword_http_header = HouServer::Inst().GetIpForwordHttpHeader();
        if (!ip_forword_http_header.empty())
        {
            http_headers_.insert(std::make_pair(ip_forword_http_header, endpoint_.address().to_string()));
        }
    }

    void UdpRequest::Execute()
    {
        boost::system::error_code const ec;
        if(ParseRequest())
        {
            SendHttpRequest();
        }
        else
        {
            LOG4CPLUS_WARN(Loggers::HouService(),"UdpRequest parse failed.");
            LOG4CPLUS_INFO(Loggers::Operations(), "|" << endpoint_.address().to_string() << "|" << host_ << path_ << "|" << "-1");
            io_service_->post(boost::bind(&HouServer::HandleHttpResponse, boost::ref(HouServer::Inst()), endpoint_, boost::ref(buf_send_), shared_from_this(), hou::HttpClientOnLibevent::ParseFailed));
        }
    }

    bool UdpRequest::ParseRequest()
    {
        std::string temp_host ;
        boost::system::error_code ec;

        size_t i = request_.find("//");
        if (i == string::npos)
            return false;

        size_t j = request_.find_first_of("/",i + 3);
        if (j == string::npos)
            return false;

        host_ = request_.substr(i + 2, j - (i + 2));

		if (!host_manager_->IsHostNameValid(host_))
		{
			return false;
		}
        address_ = host_manager_->GetHostByName(host_);
        if (address_.empty())
        {
            //该域名不能解析，不处理该请求
            return false;
        }
        http_headers_.insert(std::make_pair(Host, host_));

        size_t l = request_.find("HTTP",j + 1);
        if (l == string::npos)
            return false;

        path_ = request_.substr(j, l - j - 1);

        std::string ip_forword_url_key = HouServer::Inst().GetIpForwordUrlKey();
        if (!ip_forword_url_key.empty())
        {
            //因为目前客户端发的play请求用到&作为query string的分隔符，所以这里find & 特殊处理一下
            if (path_.find("?") != std::string::npos
                || path_.find("&") != std::string::npos)
            {
                path_ += "&";
            }
            else
            {
                path_ += "?";
            }

            path_ = path_ + ip_forword_url_key + "=" + UrlEncode(endpoint_.address().to_string());
        }

        size_t k = request_.find("\r\n");
        if (k == std::string::npos || k + 2 > request_.length())
        {
            return false;
        }

        ParseHttpHeaders(request_.substr(k + 2, request_.length() - k - 2));

        return true;
    }

    void UdpRequest::SendHttpRequest()
    {
        http_client_.reset(new HttpClientOnLibevent(http_connections_recycle_));
        http_client_->Create(address_, 80, http_request_timeout_in_seconds_,
            io_service_->wrap(boost::bind(&UdpRequest::OnHttpResponse, shared_from_this(), _1, _2,_3,_4, _5)));

        for (std::map<std::string, std::string>::const_iterator iter = http_headers_.begin();
            iter != http_headers_.end();
            ++iter)
        {
            if (iter->first != SID)
            {
                http_client_->AddHeadField(iter->first, iter->second);
            }
        }

        http_client_->AsyncRead(path_);
    }

    void UdpRequest::OnHttpResponse(const std::string & response_data, const HttpClientOnLibevent::ResponseType & response_type, bool is_gzip, int error_code, boost::uint32_t response_time)
    {
        http_response_time_ = response_time;
        response_time_ = time_counter_.elapse();

        if (response_type == HttpClientOnLibevent::Success)
        {
            std::ostringstream response_stream;

            if (http_headers_.find(SID) == http_headers_.end())
            {
                assert(false);
                return;
            }

            response_stream <<"HTTP/1.1 200 OK\r\n" << SID << ": " << http_headers_[SID] << "\r\n" << "Content-length:" << response_data.size();
            if (is_gzip)
            {
                response_stream << "\r\n" << "Content-Encoding: gzip" ;
            }

            response_stream << "\r\n\r\n" << response_data;

            std::string response = response_stream.str();
            buf_send_.prepare(response.size());
            memcpy((void *)boost::asio::buffer_cast<const char *>(*buf_send_.data().begin()),  response.c_str(), response.size() );
            buf_send_.commit(response.size());
            ++(HouServer::Inst().GetStatistics(Statistics::AllStatisticsString)->successful_response_);
            if (HouServer::Inst().GetStatistics(host_))
            {
                ++(HouServer::Inst().GetStatistics(host_)->successful_response_);
            }
        }
        else if(response_type == HttpClientOnLibevent::TimeOut)
        {
            ++(HouServer::Inst().GetStatistics(Statistics::AllStatisticsString)->http_request_time_out_);
            if (HouServer::Inst().GetStatistics(host_))
            {
                ++(HouServer::Inst().GetStatistics(host_)->http_request_time_out_);
            }
        }
        else if (response_type == HttpClientOnLibevent::ConnectionFailed)
        {
            ++(HouServer::Inst().GetStatistics(Statistics::AllStatisticsString)->http_connection_failed_);
            if (HouServer::Inst().GetStatistics(host_))
            {
                ++(HouServer::Inst().GetStatistics(host_)->http_connection_failed_);
            }
        }
        else if(response_type == HttpClientOnLibevent::DownloadFailed)
        {
            ++(HouServer::Inst().GetStatistics(Statistics::AllStatisticsString)->http_download_failed_);
            if (HouServer::Inst().GetStatistics(host_))
            {
                ++(HouServer::Inst().GetStatistics(host_)->http_download_failed_);
            }
        }

        LOG4CPLUS_INFO(Loggers::Operations(), "|" << endpoint_.address().to_string() << "|" << host_ << path_ << "|" << error_code);

        HouServer::Inst().HandleHttpResponse(endpoint_, buf_send_, shared_from_this(), response_type);
        http_client_.reset();
        //io_service_.reset();
    }

    boost::uint32_t UdpRequest::GetHttpResponseTime() const
    {
        return http_response_time_;
    }

    boost::uint32_t UdpRequest::GetResponseTime()const
    {
        return response_time_;
    }

    //this piece of code comes from peer (UrlCodec::Encode)
    std::string UdpRequest::UrlEncode(const string & url)
    {
        const static char hex_map[] = "0123456789ABCDEF";
        string encoded_url;
        for (boost::uint32_t i = 0; i < url.length(); ++i)
        {
            char c = url[i];
            // is alpha
            if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9')
            {
                encoded_url.push_back(c);
            }
            else
            {
                encoded_url.push_back('%');
                encoded_url.push_back(hex_map[(c & 0xFF) >> 4]);
                encoded_url.push_back(hex_map[c & 0x0F]);
            }
        }

        return encoded_url;
    }

    void UdpRequest::ParseHttpHeaders(const std::string & headers)
    {
        // headers不以\r\n开头，除非headers只有这两个字符

        if (headers.empty())
        {
            return;
        }

        size_t start_pos = 0;

        while(true)
        {
            std::string key;
            if (!GetItem(headers, start_pos, Colon, key))
            {
                break;
            }

            std::string value;
            if (!GetItem(headers, start_pos, EndLine, value))
            {
                break;
            }

            http_headers_[key] = value;
        }
    }

    bool UdpRequest::GetItem(const std::string & str, size_t & pos, const std::string & end_delim, std::string & item)
    {
        if (pos >= str.length())
        {
            return false;
        }

        size_t start_pos = pos;
        pos = str.find_first_of(end_delim, pos);
        if (pos == std::string::npos)
        {
            return false;
        }

        int item_length = pos - start_pos;
        if (item_length <= 0)
        {
            return false;
        }

        item = str.substr(start_pos, item_length);
        boost::trim(item);
        pos += end_delim.length();
        return true;
    }
}