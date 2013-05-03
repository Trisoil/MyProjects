#include "stdafx.h"
#include "framework/network/HttpClient.h"
#include "framework/network/uri.h"
#include "framework/Log.h"

#define NETHTTP_DEBUG(s) LOG(__DEBUG, "nethttp", s)
#define NETHTTP_INFO(s) LOG(__INFO, "nethttp", s)
#define NETHTTP_EVENT(s) LOG(__EVENT, "nethttp", s)
#define NETHTTP_WARN(s) LOG(__WARN, "nethttp", s)
#define NETHTTP_ERROR(s) LOG(__ERROR, "nethttp", s)

namespace framework
{
	namespace network
	{
		HttpClient::HttpClient(string domain, u_short port, string request, string refer_url, u_int range_begin, u_int range_end)
			: resolver_timeout_(10*1000)
			, connect_timeout_(20*1000)
			, recv_timeout_(60*1000)
			, socket_(MainThread::IOS())
			, resolver_(MainThread::IOS())
			, file_offset_(0)
			, content_offset_(0)
			, is_open_(false)
			, is_requesting_(false)
			, connect_count_(0)
			, get_count_(0)
			, is_bogus_accept_range_(false)
			, is_chunked_(false)
		{
			request_info_.domain_ = domain;
			if( port == 80 )
				request_info_.host_ = domain;
			else
				request_info_.host_ = domain + ":" + boost::lexical_cast<string>(port);
			request_info_.port_ = port;
			request_info_.path_ = request;
			request_info_.refer_url_ = refer_url;
			request_info_.range_begin_ = range_begin;
			request_info_.range_end_ = range_end;
		}

		HttpClient::p HttpClient::create(string url, string refer_url, u_int range_begin, u_int range_end)
		{
			Uri uri(url);
			string host = uri.getdomain();
			u_short port;
			try {
				port = boost::lexical_cast<u_short>(uri.getport());
			} catch(...) {
				port = 80;
			}
			string path = uri.getrequest();
			return HttpClient::p(new HttpClient(host, port, path, refer_url, range_begin, range_end));
		}

		HttpClient::p HttpClient::create(string domain, u_short port, string request, string refer_url, u_int range_begin, u_int range_end)
		{
			return HttpClient::p(new HttpClient(domain, port, request, refer_url, range_begin, range_end));
		}

		void HttpClient::Close()
		{
			NETHTTP_INFO("HttpClient::Close");
			is_open_ = false;
            is_requesting_ = false;
			socket_.close(boost::system::error_code());
			resolver_.cancel();
			if( resolver_timer_ )
				resolver_timer_->Stop();
			if( connect_timer_ )
				connect_timer_->Stop();
			if( recv_timer_ )
				recv_timer_->Stop();
		}

		void HttpClient::OnTimerElapsed(framework::timer::Timer::p pointer, u_int times)
		{
			if( pointer == resolver_timer_ )
				HandleResolveTimeout();
			else if( pointer == connect_timer_ )
				HandleConnectTimeout();
			else if( pointer == recv_timer_ )
				HandleRecvTimeout();
		}

		void HttpClient::Connect()
		{	
			if( true == is_open_ ) return;
			is_open_ = true;

			assert( connect_count_ == 0 );
			connect_count_ ++;

			string domain = request_info_.domain_;
			if( domain == "" )
			{	// 出错，向handler_发送 Url 错误消息
				if( handler_)
				{
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnConnectFailed, handler_, 1)
					);
					NETHTTP_INFO("HttpClient::Connect post IHttpClientListener::OnConnectFailed 1");
				}
				Close();
			}

			boost::asio::ip::tcp::resolver::query query( domain, boost::lexical_cast<string>(request_info_.port_) );
			resolver_.async_resolve(
				query,
				boost::bind(
					&HttpClient::HandleResolve, 
					shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::iterator
				)
			);
			resolver_timer_ = framework::timer::OnceTimer::create(resolver_timeout_, shared_from_this());
			resolver_timer_->Start();	
			NETHTTP_INFO("HttpClient::Connect async_resolve "<< domain);
		}

		void HttpClient::HandleResolveTimeout()
		{
			if( false == is_open_ ) return;
			// 出错，向handler_发送 域名解析超时 错误消息
			if( handler_)
			{
				MainThread::IOS().post(
					boost::bind( &IHttpClientListener::OnConnectFailed, handler_, 2)
					);
			}
			NETHTTP_INFO("HttpClient::HandleResolveTimeout ");
			Close();
		}

		void HttpClient::HandleResolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
		{
			if( false == is_open_ ) return;

			assert(resolver_timer_);
			resolver_timer_->Stop();

			if (!err)
			{
				endpoint_ = *endpoint_iterator;
				NETHTTP_INFO("HttpClient::HandleResolve Succed "<<endpoint_);
				LOG(__INFO, "test", endpoint_);
				socket_.async_connect(
					endpoint_,
					boost::bind(
						&HttpClient::HandleConnect, 
						shared_from_this(),
						boost::asio::placeholders::error, 
						++endpoint_iterator
					)
				);
				connect_timer_ = framework::timer::OnceTimer::create(connect_timeout_, shared_from_this());
				connect_timer_->Start();
			}
			else if( err == boost::asio::error::operation_aborted )
			{	// 操作被取消
				if( handler_)
				{
					LOG(__INFO, "network", err);
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnConnectFailed, handler_, 103)
						);
					NETHTTP_INFO("HttpClient::HandleResolve post IHttpClientListener::OnConnectFailed 103"<< err.message());
				}
				Close();
			}
			else
			{	// 出错，向handler_发送 域名解析失败 错误消息
				NETHTTP_INFO("HttpClient::HandleResolve Error because "<< err.message());
				if( handler_)
				{
					LOG(__INFO, "network", err);
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnConnectFailed, handler_, 3)
						);
					NETHTTP_INFO("HttpClient::HandleResolve post IHttpClientListener::OnConnectFailed 3"<< err.message());
				}
				Close();
			}
		}

		void HttpClient::HandleConnectTimeout()
		{
			if( false == is_open_ ) return;
			// 出错，向handler_发送 连接超时 错误消息
			if( handler_)
			{
				MainThread::IOS().post(
					boost::bind( &IHttpClientListener::OnConnectTimeout, handler_)
				);
				NETHTTP_INFO("HttpClient::HandleConnectTimeout post IHttpClientListener::OnConnectTimeout");
			}
			Close();
		}

		void HttpClient::HandleConnect(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
		{
			if( false == is_open_ ) return;

			assert(connect_timer_);
			connect_timer_->Stop();

			if (!err)
			{
				NETHTTP_INFO("HttpClient::HandleConnect Succed"<< err.message());
				// 汇报连接成功消息
				MainThread::IOS().post(
					boost::bind( &IHttpClientListener::OnConnectSucced, handler_ )
				);
				NETHTTP_INFO("HttpClient::HandleConnect post IHttpClientListener::OnConnectSucced"<< err.message());
			}
			else if (endpoint_iterator != boost::asio::ip::tcp::resolver::iterator())
			{
				socket_.close(boost::system::error_code());
				endpoint_ = *endpoint_iterator;
				socket_.async_connect(
					endpoint_,
					boost::bind(
						&HttpClient::HandleConnect, 
						shared_from_this(),
						boost::asio::placeholders::error, 
						++endpoint_iterator
					)
				);
				assert(connect_timer_);
				connect_timer_->SetInterval(connect_timeout_);
				connect_timer_->ReStart();
				NETHTTP_INFO("HttpClient::HandleConnect async_connect "<< endpoint_);
			}
			else if( err == boost::asio::error::operation_aborted )
			{	// 异步操作被取消
				NETHTTP_INFO("HttpClient::HandleConnect Error because operation_aborted");
				if( handler_ )
				{
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnConnectFailed, handler_, 101 )
						);
				}
				Close();
			}
			else
			{
				NETHTTP_INFO("HttpClient::HandleConnect Error because "<<err.message());
				// 出错，向handler_发送 连接失败 错误消息
				if( handler_ )
				{
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnConnectFailed, handler_, 1 )
					);
				}
				Close();
			}
		}

		void HttpClient::HttpGetByString(string request_string)
		{
			if( is_open_ == false) return;

			assert( get_count_ == 0 );
			get_count_ ++;

			assert( request_info_ );

			if( !request_info_)
			{
				return;
			}

			LOG(__EVENT, "packet", "HTTP GET " << request_string );

			assert( is_requesting_ == false );
			if( is_requesting_ == true ) return;
			is_requesting_ = true;

			boost::asio::async_write(
				socket_, 
				boost::asio::buffer(request_string),
				boost::bind(
					&HttpClient::HandleWriteRequest, 
					shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred
				)
			);
			NETHTTP_INFO("HttpClient::HttpGet async_write " << request_string);
		}

		void HttpClient::HttpGet()
		{
			if( is_open_ == false) return;

			assert( get_count_ == 0 );
			get_count_ ++;

			assert( request_info_ );

			if( !request_info_)
			{
				return;
			}

			string request_string = request_info_.ToString();
            LOG(__EVENT, "http", "HttpClient::HttpGet " << socket_.remote_endpoint() << " Request:\n" << request_string );

			assert( is_requesting_ == false );
			if( is_requesting_ == true ) return;
			is_requesting_ = true;

			boost::asio::async_write(
				socket_, 
				boost::asio::buffer(request_string),
				boost::bind(
					&HttpClient::HandleWriteRequest, 
					shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred
				)
			);
			NETHTTP_INFO("HttpClient::HttpGet async_write " << request_string);
		}

		void HttpClient::SetMethod(const string& method)
		{
			request_info_.method_ = method;
		}

		void HttpClient::HttpGet(u_int range_begin, u_int range_end)
		{
			request_info_.range_begin_ = range_begin;
			request_info_.range_end_ = range_end;
			HttpGet();
		}

		void HttpClient::HttpGet(string refer_url, u_int range_begin, u_int range_end)
		{
			request_info_.refer_url_ = refer_url;
			request_info_.range_begin_ = range_begin;
			request_info_.range_end_ = range_end;
			HttpGet();
		}

		void HttpClient::HttpGet(HttpRequest::p http_request_demo, u_int range_begin, u_int range_end)
		{
			request_info_.http_request_demo_ = http_request_demo;
			request_info_.range_begin_ = range_begin;
			request_info_.range_end_ = range_end;
			HttpGet();
		}

		void HttpClient::HttpGet(HttpRequest::p http_request_demo, string refer_url, u_int range_begin, u_int range_end)
		{
			request_info_.http_request_demo_ = http_request_demo;
			request_info_.refer_url_ = refer_url;
			request_info_.range_begin_ = range_begin;
			request_info_.range_end_ = range_end;
			HttpGet();
		}

		void HttpClient::HandleRecvTimeout()
		{
			if( false == is_open_ ) return;
			// 出错，向handler_发送 接受超时 错误消息
			MainThread::IOS().post(
				boost::bind( &IHttpClientListener::OnRecvTimeout, handler_)
			);
			NETHTTP_INFO("HttpClient::HandleRecvTimeout post IHttpClientListener::OnRecvTimeout");
			Close();
		}

		void HttpClient::HandleWriteRequest(const boost::system::error_code& err, size_t bytes_transferred)
		{
			if( false == is_open_ ) return;
			// 发包成功
			if( !err)
			{
                string delim("\r\n\r\n");
				NETHTTP_INFO("HttpClient::HandleWriteRequest Succed "<<bytes_transferred);
				// 发送 HTTP请求 成功，再接受
				boost::asio::async_read_until(
					socket_, 
					response_, 
					//boost::regex("\r\n\r\n"),
                    delim,
					boost::bind(
					    &HttpClient::HandleReadHttpHeader, 
					    shared_from_this(),
					    boost::asio::placeholders::error,
					    boost::asio::placeholders::bytes_transferred
					)
				);
				recv_timer_ = framework::timer::OnceTimer::create(recv_timeout_, shared_from_this() );
				recv_timer_->Start();
				NETHTTP_INFO("HttpClient::HandleWriteRequest async_read_until");
			}
			else if( err == boost::asio::error::operation_aborted )
			{	// 异步操作被取消
				NETHTTP_INFO("HttpClient::HandleWriteRequest Error because operation_aborted");
				// 发送 HTTP请求 失败
				if( handler_ )
				{
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnRecvHttpHeaderFailed, handler_, 105)
						);
					NETHTTP_INFO("HttpClient::HandleWriteRequest post IHttpClientListener::OnRecvHttpHeaderFailed " << 105);
				}
				Close();
			}
			else
			{
				NETHTTP_INFO("HttpClient::HandleWriteRequest Error because "<<err.message());
				// 发送 HTTP请求 失败
				if( handler_ )
				{
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnRecvHttpHeaderFailed, handler_, 5)
					);
					NETHTTP_INFO("HttpClient::HandleWriteRequest post IHttpClientListener::OnRecvHttpHeaderFailed " << 5);
				}
				Close();
			}
		}

		void HttpClient::HandleReadHttpHeader(const boost::system::error_code& err, size_t bytes_transferred)
		{
			if( false == is_open_ ) return;

			assert( is_requesting_ == true );
			if( is_requesting_ == false ) return;
			is_requesting_ = false;

			assert(recv_timer_);
			recv_timer_->Stop();

			if( !err )
			{
				NETHTTP_INFO("HttpClient::HandleReadHttpHeader Succed " << bytes_transferred);
				assert(bytes_transferred <= response_.size());
				// 接受Http头部成功
				string response_string;
				size_t header_length;
				Buffer buffer(bytes_transferred);
				istream is(&response_);
				is.read((char*)buffer.data_.get(), bytes_transferred);
				response_string.assign((const char*)buffer.data_.get(), bytes_transferred);
				http_response_ = HttpResponse::ParseFromBuffer(response_string, header_length);
				if( ! http_response_ )
				{
					// 出错，向handler_发送 接受头部出错消息 
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnRecvHttpHeaderFailed, handler_, 1)
					);
					NETHTTP_INFO("HttpClient::HandleReadHttpHeader post IHttpClientListener::OnRecvHttpHeaderFailed " << 1);
					Close();
					return;
				}
				assert(header_length > 0);
				if( header_length == 0)
				{
					// 出错，向handler_发送 接受头部出错消息 
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnRecvHttpHeaderFailed, handler_, 2)
					);
					NETHTTP_INFO("HttpClient::HandleReadHttpHeader post IHttpClientListener::OnRecvHttpHeaderFailed " << 2);
					Close();
					return;
				}
				assert(header_length == bytes_transferred);
				if( header_length > bytes_transferred)
				{
					// 出错，向handler_发送 接受头部出错消息 
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnRecvHttpHeaderFailed, handler_, 3)
					);
					NETHTTP_INFO("HttpClient::HandleReadHttpHeader post IHttpClientListener::OnRecvHttpHeaderFailed " << 3);
					Close();
					return;
				}
				
				// 汇报接受头部成功消息
				MainThread::IOS().post(
					boost::bind( &IHttpClientListener::OnRecvHttpHeaderSucced, handler_, http_response_)
				);
				NETHTTP_INFO("HttpClient::HandleReadHttpHeader post IHttpClientListener::OnRecvHttpHeaderSucced ");
#undef max
				
				if( false == http_response_->HasContentLength() )
				{
					content_length_ = std::numeric_limits<u_int>::max();
					is_chunked_ = true;
				}
				else
					content_length_ = http_response_->GetContentLength();
				content_offset_ = 0;

				//房子tudou的假Range情况
				if (request_info_.range_begin_ != 0)
					file_offset_ = http_response_->GetRangeBegin();
			}
			else if( err == boost::asio::error::operation_aborted )
			{
				NETHTTP_INFO("HttpClient::HandleReadHttpHeader Error because operation_aborted");
				if( handler_ )
				{
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnRecvHttpHeaderFailed, handler_, 104)
						);
					NETHTTP_INFO("HttpClient::HandleReadHttpHeader post IHttpClientListener::OnRecvHttpHeaderFailed "<<104);
				}
				Close();
			}
			else
			{
				NETHTTP_INFO("HttpClient::HandleReadHttpHeader Error because "<<err.message());
				// 出错，向handler_发送 接受头部出错消息 
				if( handler_ )
				{
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnRecvHttpHeaderFailed, handler_, 4)
					);
					NETHTTP_INFO("HttpClient::HandleReadHttpHeader post IHttpClientListener::OnRecvHttpHeaderFailed "<<4);
				}
				Close();
				return;
			}
		}

		void HttpClient::HttpRecv(size_t length)
		{
			if( false == is_open_ ) return;

			if( content_offset_ >= content_length_)
			{
				// 接受完成 
				MainThread::IOS().post(
					boost::bind( &IHttpClientListener::OnComplete, handler_)
				);
                NETHTTP_INFO("HttpClient::HttpRecv post IHttpClientListener::OnComplete -> HttpClient.Close()");
				Close();
				return;
			}

			if( (content_offset_ + length) > content_length_ )
			{
				length = content_length_ - content_offset_;
			}

			size_t network_length = length;
			if( network_length <= response_.size() )
			{
				Buffer buffer(length);
				istream is(&response_);
				is.read((char*)buffer.data_.get(), length);
				MainThread::IOS().post(
					boost::bind( &IHttpClientListener::OnRecvHttpDataSucced, handler_, buffer, file_offset_, content_offset_)
				);
				NETHTTP_INFO("HttpClient::HttpRecv post IHttpClientListener::OnRecvHttpDataSucced "<<file_offset_<<" "<<content_offset_);
				file_offset_ += length;
				content_offset_ += length;
				return;
			}
			else
			{
				network_length -= response_.size();
			}

            assert( is_requesting_ == false );
			if( is_requesting_ == true ) return;
			is_requesting_ = true;
			NETHTTP_INFO("async_read length= "<<length << " network_length="<<network_length<<" response.size()="<<response_.size());
			boost::asio::async_read(
				socket_,
				response_,
				//boost::asio::buffer( buffer.data_.get(), length),
				boost::asio::transfer_at_least(network_length),
				boost::bind( 
					&HttpClient::HandleReadHttp, 
					shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred,
					length,
					file_offset_,
					content_offset_
				)
			);
		    // assert(recv_timer_); 出现 recv_timer_ == null
            if (!recv_timer_) recv_timer_ = framework::timer::OnceTimer::create(recv_timeout_, shared_from_this() );
			recv_timer_->ReStart();
			NETHTTP_INFO("HttpClient::HttpRecv async_read "<<length<<" "<<file_offset_<<" "<<content_offset_);

			//response_offset_ += length;
			file_offset_ += length;
			content_offset_ += length;
		}

		void HttpClient::HttpRecvSubPiece()
		{
			// 一个SubPiece 正好是1k,故直接调用 HttpRecvRange
			HttpRecv(1024);
		}

		void HttpClient::HandleReadHttp(const boost::system::error_code& err, size_t bytes_transferred, size_t buffer_length, size_t file_offset, size_t content_offset)
		{
			NETHTTP_INFO("HandleReadHttp " << bytes_transferred);
			if( false == is_open_ ) return;

			assert( is_requesting_ == true );
			if( is_requesting_ == false ) return;
			is_requesting_ = false;

			assert(recv_timer_);
			recv_timer_->Stop();

			if( ! err )
			{
				NETHTTP_INFO("HttpClient::HandleReadHttp Succed "<<buffer_length<<" "<<file_offset<<" "<<content_offset);
				assert(buffer_length>0);
				assert( response_.size() >= buffer_length );
				Buffer buffer(buffer_length);
				istream is(&response_);
				is.read((char*)buffer.data_.get(), buffer_length);
				// 消息
				MainThread::IOS().post(
					boost::bind( &IHttpClientListener::OnRecvHttpDataSucced, handler_, buffer, file_offset, content_offset)
				);
				NETHTTP_INFO("HttpClient::HandleReadHttp post IHttpClientListener::OnRecvHttpDataSucced "<<file_offset<<" "<<content_offset);
			}
			else if( err == boost::asio::error::operation_aborted )
			{	// 异步操作被取消
				NETHTTP_INFO("HttpClient::HandleReadHttp Error because operation_aborted");
				if( handler_ )
				{
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnRecvHttpDataFailed, handler_, 101)
						);
				}
				NETHTTP_INFO("HttpClient::HandleReadHttp post IHttpClientListener::OnRecvHttpDataFailed "<<101);
				Close();
			}
			else if(err == boost::asio::error::eof)
			{	// 
				NETHTTP_INFO("HttpClient::HandleReadHttp Error because eof");
				if( response_.size() > 0 )
				{
					Buffer buffer(response_.size());
					istream is(&response_);
					is.read((char*)buffer.data_.get(), response_.size());
					// 消息
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnRecvHttpDataPartial, handler_, buffer, file_offset, content_offset)
					);
					NETHTTP_INFO("HttpClient::HandleReadHttp post IHttpClientListener::OnRecvHttpDataPartial "<<file_offset<<" "<<content_offset);
				}
				if( is_chunked_ == false )
				{
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnRecvHttpDataFailed, handler_, 2)
					);
					NETHTTP_INFO("HttpClient::HandleReadHttp post IHttpClientListener::OnRecvHttpDataFailed "<<2);
				}
				else
				{
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnComplete, handler_)
					);
					NETHTTP_INFO("HttpClient::HttpRecv post IHttpClientListener::OnComplete, because it is chunked");
				}
				Close();
			}
			else
			{
				NETHTTP_INFO("HttpClient::HandleReadHttp Error because "<<err.message());
				// 出错，向handler_发送 接受数据出错消息 
				if( handler_ )
				{
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnRecvHttpDataFailed, handler_, 1)
					);
				}
				NETHTTP_INFO("HttpClient::HandleReadHttp post IHttpClientListener::OnRecvHttpDataFailed "<<1);
				Close();
			}
		}

		bool HttpClient::IsBogusAcceptRange() const
		{
			if( !http_response_ )
			{	// 没有获得回应，无法获得是否 假支持Range
				assert(0);
				return false;
			}

			if( request_info_.range_begin_ == 0 )
			{	// 不是带Range请求 不可能判定为假支持Range
				return false;
			}
			else
			{
				if( http_response_->GetStatusCode() == 403 )
				{	// 带Range请求 返回403
					return true;
				}
				if( http_response_->GetStatusCode() == 200 && http_response_->HasContentRange() == false )
				{	// 带Range请求 返回200 并且 没有 Content-Range 头部
					return true;
				}
			}
			return false;
		}
	}
}
