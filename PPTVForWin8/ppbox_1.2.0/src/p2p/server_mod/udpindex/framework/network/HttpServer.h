#pragma once

#include "framework/timer/Timer.h"
#include "framework/timer/TimeCounter.h"
#include "framework/network/HttpAcceptor.h"
#include "framework/network/HttpRequest.h"

namespace framework
{
	namespace network
	{
		interface IHttpServerListener
		{
			typedef boost::shared_ptr<IHttpServerListener> p;
			virtual void OnHttpRecvSucced(HttpRequest::p http_request) = 0;
			virtual void OnHttpRecvFailed(u_int error_code) = 0;
			virtual void OnHttpRecvTimeout() = 0;
			virtual void OnTcpSendSucced(size_t length) = 0;
			virtual void OnTcpSendFailed() = 0;
			virtual void OnClose() = 0;

            virtual ~IHttpServerListener() {}
		};

		class HttpServer 
			: public boost::noncopyable
			, public boost::enable_shared_from_this<HttpServer>
			, public framework::timer::ITimerListener
		{
			friend class HttpAcceptor;
		public:
			typedef boost::shared_ptr<HttpServer> p;
			static p create() { return p(new HttpServer);}
		private:
			// 构造
			HttpServer() : is_open_(false), recv_timeout_(60*1000), will_close_(false), socket_(MainThread::IOS()), sended_bytes_(0)
				{
					notice_window_close_.is_recieved = true;
				}
		public:
			// 操作
			void HttpRecv();
			void HttpRecvTillClose();
			void HttpSendHeader(size_t content_length, string content_type = "html/text");
            void HttpSendKeepAliveHeader(size_t content_length, string content_type = "html/text");
            void HttpSend403Header();
			void HttpSendHeader(string header_string);
			void HttpSendBuffer(const byte* data, size_t length);
			void HttpSendBuffer(const Buffer& buffer);
			void HttpSendContent(const byte* data, size_t length, string content_type = "html/text");
			void HttpSendContent(const string& text, string content_type = "html/text");
			void TcpSend(Buffer b);
			void WillClose();		// 将缓冲区中数据发送完成再断开连接
			void Close();
			void DelayClose();
			boost::asio::ip::tcp::endpoint GetEndPoint() const;
		public:
			// 属性
			void SetRecvTimeout(u_int recv_timeout) { recv_timeout_ = recv_timeout; }
			void SetListener(IHttpServerListener::p handler) { handler_ = handler; }
			u_int GetSendPendingCount() const { return send_set_.size(); }
		protected:
			// 消息
			void HandleHttpRecv(const boost::system::error_code& err, size_t bytes_transferred);
			void HandleHttpRecvTillClose(const boost::system::error_code& err, size_t bytes_transferred, Buffer buffer);
			void HandleHttpRecvTimeout();
			void HandleCloseTimerElapsed();
			void HandleTcpSend(const boost::system::error_code& error, size_t bytes_transferred, Buffer b);
			virtual void OnTimerElapsed(framework::timer::Timer::p pointer, u_int times);
		private:
			// 变量
			boost::asio::ip::tcp::socket socket_;
			boost::asio::streambuf request_;
			u_int sended_bytes_;
			// 发送缓冲区
			std::set<Buffer> send_set_;
			// 接受超时
			framework::timer::OnceTimer::Timer::p recv_timer_;
			framework::timer::OnceTimer::Timer::p close_timer_;
			framework::timer::PeriodicTimer::p notice_window_close_timer_;
			u_int recv_timeout_;
			// 状态维护
			bool is_open_;
			bool will_close_;
			// 消息接受者
			IHttpServerListener::p handler_;

			struct notice_window_close
			{
				bool is_recieved;
				framework::timer::TimeCounter last_send_time;
			} notice_window_close_;
		};
	}
}