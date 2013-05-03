#include "stdafx.h"
#include "framework/network/HttpServer.h"

namespace framework
{
	namespace network
	{
		void HttpServer::HttpRecv()
		{
			if( is_open_ == false ) 
				return;

            string delim("\r\n\r\n");
			boost::asio::async_read_until(
				socket_, 
				request_, 
				//boost::regex("\r\n\r\n"),
                delim,
				boost::bind(
					&HttpServer::HandleHttpRecv, 
					shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred
				)
			);
			recv_timer_ = framework::timer::OnceTimer::create(recv_timeout_, shared_from_this());
			recv_timer_->Start();

			notice_window_close_timer_ = framework::timer::PeriodicTimer::create(1000, shared_from_this());
			notice_window_close_timer_->Start();

		}

		void HttpServer::HttpRecvTillClose()
		{
			if( is_open_ == false ) 
				return;

			Buffer buffer(1024);

            LOG(__EVENT, "httpserver", "HttpServer::HttpRecvTillClose ep: " << socket_.remote_endpoint());

			boost::asio::async_read(
				socket_,
				boost::asio::buffer( buffer.data_.get(), buffer.length_),
				boost::bind( 
					&HttpServer::HandleHttpRecvTillClose,
					shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred,
					buffer
				)
			);
		}

		void HttpServer::HandleHttpRecvTillClose(const boost::system::error_code& err, size_t bytes_transferred, Buffer buffer)
		{
			if( is_open_ == false )
				return;

			if( ! err )
			{
                LOG(__EVENT, "httpserver", "HttpServer::HandleHttpRecvTillClose GetBuffer: " << string((const char*)buffer.data_.get(), buffer.length_));
				HttpRecvTillClose();
			}
            else
			{
                LOG(__EVENT, "httpserver", "HttpServer::HandleHttpRecvTillClose err: " << err.message());
				if( handler_ )
				{
					MainThread::IOS().post(
						boost::bind( &IHttpServerListener::OnHttpRecvFailed, handler_, 2 )
					);
				}

				Close();
			}
		}

		void HttpServer::HandleHttpRecv(const boost::system::error_code& err, size_t bytes_transferred)
		{
			recv_timer_->Stop();

			if( is_open_ == false )
				return;

			if( ! err )
			{
				string request_string;
				copy(istreambuf_iterator<char>(&request_), istreambuf_iterator<char>(), back_inserter(request_string));

                LOG(__EVENT, "httpserver", "HttpServer::HandleHttpRecv RequestString:\n" << request_string);

				HttpRequest::p http_request = HttpRequest::ParseFromBuffer(request_string);
				if( ! http_request )
				{
                    LOG(__WARN, "httpserver", "HttpServer::HandleHttpRecv Parse Error!");
                    // HTTP 头部出现问题
					if( handler_ )
					{
						MainThread::IOS().post(
							boost::bind( &IHttpServerListener::OnHttpRecvFailed, handler_, 1 )
						);
					}
				}
				else if( handler_ )
				{
					MainThread::IOS().post(
						boost::bind( &IHttpServerListener::OnHttpRecvSucced, handler_, http_request )
					);
				}
			}
			else
			{
                LOG(__EVENT, "httpserver", "HttpServer::HandleHttpRecv err: " << err.message());
				if( handler_ )
				{
					MainThread::IOS().post(
						boost::bind( &IHttpServerListener::OnHttpRecvFailed, handler_, 2 )
					);
				}

				Close();
			}
		}

		void HttpServer::OnTimerElapsed(framework::timer::Timer::p pointer, u_int times)
		{
			if( pointer == recv_timer_ )
			{
				HandleHttpRecvTimeout();
			}
			else if( pointer == close_timer_)
			{
				HandleCloseTimerElapsed();
			}
			else if( pointer == notice_window_close_timer_ )
			{
				if (false == notice_window_close_.is_recieved)
				{
                    if (notice_window_close_.last_send_time.GetElapsed() > 8*1000)
                    {
                        LOG(__EVENT, "httpserver", "HttpServer::OnTimerElapsed notice_window_close_.last_send_time.GetElapsed() > 20*1000, " << GetEndPoint());
                        Close();
                    }
				}
			} 
            else
            {
				assert(0);
            }
		}

		void HttpServer::HandleHttpRecvTimeout()
		{
			if( is_open_ == false )
				return;

			if( handler_ )
			{
				MainThread::IOS().post(
					boost::bind( &IHttpServerListener::OnHttpRecvTimeout, handler_ )
				);
			}
		}

		void HttpServer::TcpSend(Buffer buffer)
		{	
			if( is_open_ == false )
				return;

            LOG(__EVENT, "httpserver", __FUNCTION__ << " notice_window_close.Sync, is_recieved = " << notice_window_close_.is_recieved);
            //LOG(__EVENT, "httpserver", __FUNCTION__ << " buffer: " << string((const char*)(buffer.data_.get()), buffer.length_));
			if (notice_window_close_.is_recieved)
			{
				notice_window_close_.last_send_time.Sync();
				notice_window_close_.is_recieved = false;
			}

			assert( send_set_.find(buffer) == send_set_.end() );
			send_set_.insert(buffer);

			boost::asio::async_write(
				socket_,
				boost::asio::buffer( buffer.data_.get(), buffer.length_ ),
				boost::asio::transfer_at_least(buffer.length_),
				boost::bind( 
					&HttpServer::HandleTcpSend, 
					shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred,
					buffer
				)
			);
		}

		void HttpServer::HandleTcpSend(const boost::system::error_code& err, size_t bytes_transferred, Buffer buffer)
		{
			if( is_open_ == false )
				return;

			if( ! err )
			{
                LOG(__EVENT, "httpserver", __FUNCTION__ << " notice_window_close_.is_recieved = true ");
                // 发送成功
				assert(send_set_.find(buffer) != send_set_.end() );
				send_set_.erase(buffer);
				sended_bytes_ += bytes_transferred;

				notice_window_close_.is_recieved = true;


				if( handler_ )
				{
					MainThread::IOS().post(
						boost::bind( &IHttpServerListener::OnTcpSendSucced, handler_, bytes_transferred )
					);
				}

				if( send_set_.size() == 0 && will_close_ == true )
				{	
                    LOG(__DEBUG, "httpserver", "HttpServer::HandleTcpSend send_set_.size() == 0 && will_close_ == true");
                    // 在上层关闭后 已经将 所有要发送的发送完成
					DelayClose();
				}
			}
			else
            {
                LOG(__EVENT, "httpserver", "HttpServer::HandleTcpSend: " << socket_.remote_endpoint() << " err: " << err.message());
                // 发送失败
				if( handler_ )
				{
					MainThread::IOS().post(
						boost::bind( &IHttpServerListener::OnTcpSendFailed, handler_ )
					);
				}

				Close();
			}
		}

		void HttpServer::WillClose()
		{
			will_close_ = true;
			if( send_set_.size() == 0 )
				DelayClose();
		}

		void HttpServer::Close()
		{
			if( is_open_ == false) return;
			is_open_ = false;
			// ???忽略socket关闭错误，不抛异常
			socket_.close(boost::system::error_code());
			send_set_.clear();
			recv_timer_->Stop();
			notice_window_close_timer_->Stop();

			LOG(__EVENT, "proxy", "HttpServer Close sended_length = " <<sended_bytes_);

			if( handler_ )
			{
				MainThread::IOS().post(
					boost::bind( &IHttpServerListener::OnClose, handler_ )
				);
			}
		}

		void HttpServer::DelayClose()
		{
			if( is_open_ == false) return;
			if (close_timer_)
				return;
			close_timer_ = framework::timer::OnceTimer::create(1000, shared_from_this());
			close_timer_->Start();
		}

		void HttpServer::HandleCloseTimerElapsed()
		{
			if( is_open_ == false) return;
			Close();
		}

		boost::asio::ip::tcp::endpoint HttpServer::GetEndPoint() const
		{
			try
			{
				boost::asio::ip::tcp::endpoint end_point;
				if( is_open_ )
				{
					end_point = socket_.remote_endpoint();
				}
				return end_point;
			}
			catch (...)
			{	
				LOG(__ERROR, "httpserver", __FUNCTION__ <<"GetEndPoint Error.");
				return boost::asio::ip::tcp::endpoint();
			}
		}

		void HttpServer::HttpSendHeader(size_t content_length, string content_type)
		{
            LOG(__EVENT, "httpserver", "HttpServer::HttpSendHeader content_length: " << content_length << ", type " << content_type);

			stringstream response_stream;
			response_stream << "HTTP/1.0 200 OK\r\n";
			response_stream << "Content-Type: " << content_type << "\r\n";
			response_stream << "Content-Length: " << content_length << "\r\n";
			response_stream << "Connection: close\r\n";
			response_stream << "\r\n";
			string response_string = response_stream.str();

            LOG(__EVENT, "httpserver", "HttpServer::HttpServerSendHeader SimpleHeader: \n" << response_string);

			Buffer buffer(response_string.c_str(), response_string.length());
			TcpSend(buffer);
		}

        void HttpServer::HttpSendKeepAliveHeader(size_t content_length, string content_type)
        {
            LOG(__EVENT, "httpserver", "HttpServer::HttpSendKeepAliveHeader content_length: " << content_length << ", type " << content_type);

            stringstream response_stream;
            response_stream << "HTTP/1.0 200 OK\r\n";
            response_stream << "Content-Type: " << content_type << "\r\n";
            response_stream << "Content-Length: " << content_length << "\r\n";
            response_stream << "Connection: Keep-Alive\r\n";
            response_stream << "\r\n";
            string response_string = response_stream.str();

            LOG(__EVENT, "httpserver", "HttpServer::HttpServerSendHeader SimpleHeader: \n" << response_string);

            Buffer buffer(response_string.c_str(), response_string.length());
            TcpSend(buffer);
        }

		void HttpServer::HttpSendHeader(string header_string)
		{
			Buffer buffer(header_string.c_str(), header_string.length());
			TcpSend(buffer);
		}

		void HttpServer::HttpSendBuffer(const byte* data, size_t length)
		{
			Buffer buffer( data, length);
			HttpSendBuffer(buffer);
		}

		void HttpServer::HttpSendBuffer(const Buffer& buffer)
		{
			TcpSend(buffer);
		}

		void HttpServer::HttpSendContent(const byte* data, size_t length, string content_type)
		{
			HttpSendHeader(length, content_type);
			HttpSendBuffer(data, length);
			WillClose();
		}

		void HttpServer::HttpSendContent(const string& text, string content_type)
		{
			HttpSendContent( (const byte*)text.c_str(), text.length(), content_type );
		}

        void HttpServer::HttpSend403Header()
        {
            LOG(__EVENT, "httpserver", __FUNCTION__);

            stringstream response_stream;
            response_stream << "HTTP 403 Forbidden\r\n";
            response_stream << "Connection: Close\r\n";
            response_stream << "\r\n";
            string response_string = response_stream.str();

            LOG(__EVENT, "httpserver", "HttpServer::HttpServerSendHeader SimpleHeader: \n" << response_string);

            Buffer buffer(response_string.c_str(), response_string.length());
            TcpSend(buffer);
        }
	}
}
