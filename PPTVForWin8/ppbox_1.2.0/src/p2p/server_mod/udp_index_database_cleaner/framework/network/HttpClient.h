#pragma once

#include "framework/timer/Timer.h"
#include "framework/network/HttpRequest.h"
#include "framework/network/HttpResponse.h"

namespace framework
{
	namespace network
	{
		interface IHttpClientListener
		{
			typedef boost::shared_ptr<IHttpClientListener> p;

			virtual void OnConnectSucced() = 0;
			virtual void OnConnectFailed(u_int error_code) = 0; // 1-Url有问题 2-域名无法解析 3-域名解析出错 4-连接失败
			virtual void OnConnectTimeout() = 0;

			virtual void OnRecvHttpHeaderSucced(HttpResponse::p http_response) = 0;
			virtual void OnRecvHttpHeaderFailed(u_int error_code) = 0;
			virtual void OnRecvHttpDataSucced(Buffer buffer, size_t file_offset, size_t content_offset) = 0;
			virtual void OnRecvHttpDataPartial(Buffer buffer, size_t file_offset, size_t content_offset) = 0;
			virtual void OnRecvHttpDataFailed(u_int error_code) = 0;
			virtual void OnRecvTimeout() = 0;

			virtual void OnComplete() = 0;

            virtual ~IHttpClientListener() {}
		};

		class HttpClient
			: public boost::noncopyable
			, public boost::enable_shared_from_this<HttpClient>
			, public framework::timer::ITimerListener
		{
		public:
			typedef boost::shared_ptr<HttpClient> p;
			static p create(string url, string refer_url="", u_int range_begin=0, u_int range_end=0);
			static p create(string domain, u_short port, string request, string refer_url="", u_int range_begin=0, u_int range_end=0);
		public:
			// 构造		
			HttpClient(string domain, u_short port, string request, string refer_url, u_int range_begin, u_int range_end);
		public:
			// 操作
			virtual void Close();
			virtual void Connect();
			virtual void SetMethod(const string& method);
			//virtual void SetHeader(const string& key, const string& value);
			virtual void HttpGet();
			virtual void HttpGet(u_int range_begin, u_int range_end=0);
			virtual void HttpGet(string refer_url, u_int range_begin=0, u_int range_end=0);
			virtual void HttpGet(HttpRequest::p http_request_demo, u_int range_begin, u_int range_end=0);
			virtual void HttpGet(HttpRequest::p http_request_demo, string refer_url, u_int range_begin=0, u_int range_end=0);
			virtual void HttpGetByString(string request_string);
			virtual void HttpRecv(size_t length);
			virtual void HttpRecvSubPiece();
		public:
			// 属性
			virtual void SetHandler(IHttpClientListener::p handler) { handler_ = handler; }
			virtual void SetResolverTimeout(u_int resolver_timeout) { resolver_timeout_ = resolver_timeout; }
			virtual void SetConnectTimeout(u_int connect_timeout) { connect_timeout_ = connect_timeout; }
			virtual void SetRecvTimeout(u_int recv_timeout) { recv_timeout_ = recv_timeout; }
			virtual bool IsBogusAcceptRange() const;
		protected:
			// 消息
			virtual void HandleResolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
			virtual void HandleConnect(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
			virtual void HandleWriteRequest(const boost::system::error_code& err, size_t bytes_transferred);
			virtual void HandleReadHttpHeader(const boost::system::error_code& err, size_t bytes_transferred);
			virtual void HandleReadHttp(const boost::system::error_code& err, size_t bytes_transferred, size_t buffer_length, size_t file_offset, size_t content_offset);
			// 定时器消息
			virtual void OnTimerElapsed(framework::timer::Timer::p pointer, u_int times);
			virtual void HandleResolveTimeout();
			virtual void HandleConnectTimeout();
			virtual void HandleRecvTimeout();
		protected:
			// 变量
			boost::asio::ip::tcp::endpoint endpoint_;
			boost::asio::ip::tcp::socket socket_;
			boost::asio::ip::tcp::resolver resolver_;
			HttpRequestInfo request_info_;		
			HttpResponse::p http_response_;
			boost::asio::streambuf response_;
			size_t content_length_;
			size_t file_offset_;			// 当前读取位置 相对于 文件起始 的位置
			size_t content_offset_;			// 当前读取位置 相对于 Http 数据体部的位置
			// 定时器
			framework::timer::OnceTimer::p resolver_timer_;	// 域名解析 超时 定时器
			u_int resolver_timeout_;	// 域名解析 超时 定时时间，以毫秒为单位
			framework::timer::OnceTimer::p connect_timer_;	// 连接     超时 定时器
			u_int connect_timeout_;		// 链接     超时 定时时间，以毫秒为单位
			framework::timer::OnceTimer::p recv_timer_;		// 接受数据 超时 定时器
			u_int recv_timeout_;		// 接受数据 超时 定时时间，以毫秒为单位			
			// 消息接受者
			IHttpClientListener::p handler_;
			// 状态
			bool is_open_;
			bool is_requesting_;
			bool is_bogus_accept_range_;
			bool is_chunked_;

			// 保护
			u_int connect_count_;
			u_int get_count_;
		};
	}
}

