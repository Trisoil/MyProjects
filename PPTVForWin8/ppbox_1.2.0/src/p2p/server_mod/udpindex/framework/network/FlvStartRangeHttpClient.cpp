#include "stdafx.h"
#include "framework/network/FlvStartRangeHttpClient.h"
#include "framework/network/uri.h"
#include "framework/Log.h"

#include <boost/lexical_cast.hpp>

#define NETHTTP_DEBUG(s) LOG(__DEBUG, "nethttp", s)
#define NETHTTP_INFO(s) LOG(__INFO, "nethttp", s)
#define NETHTTP_EVENT(s) LOG(__EVENT, "nethttp", s)
#define NETHTTP_WARN(s) LOG(__WARN, "nethttp", s)
#define NETHTTP_ERROR(s) LOG(__ERROR, "nethttp", s)

namespace framework
{
	namespace network
	{
		FlvStartRangeHttpClient::FlvStartRangeHttpClient(string domain, u_short port, string request, string refer_url, u_int range_begin, u_int range_end)
			: HttpClient(domain, port, request, refer_url, range_begin, range_end)
		{
		}

		FlvStartRangeHttpClient::p FlvStartRangeHttpClient::create(string url, string refer_url, u_int range_begin, u_int range_end)
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
			return FlvStartRangeHttpClient::p(new FlvStartRangeHttpClient(host, port, path, refer_url, range_begin, range_end));
		}

		FlvStartRangeHttpClient::p FlvStartRangeHttpClient::create(string domain, u_short port, string request, string refer_url, u_int range_begin, u_int range_end)
		{
			return FlvStartRangeHttpClient::p(new FlvStartRangeHttpClient(domain, port, request, refer_url, range_begin, range_end));
		}

		void FlvStartRangeHttpClient::HttpGet()
		{
			if( is_open_ == false) return;

			assert( get_count_ == 0 );
			get_count_ ++;

			assert( request_info_ );

			if( !request_info_)
			{
				return;
			}

			string request_string = request_info_.ToFlvStartRangeString();
			LOG(__EVENT, "packet", "HTTP GET " << request_string );

			assert( is_requesting_ == false );
			if( is_requesting_ == true ) return;
			is_requesting_ = true;

			boost::asio::async_write(
				socket_, 
				boost::asio::buffer(request_string),
				boost::bind(
					&FlvStartRangeHttpClient::HandleWriteRequest, 
					boost::static_pointer_cast<FlvStartRangeHttpClient>(shared_from_this()),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred
				)
			);
			NETHTTP_INFO("FlvStartRangeHttpClient::HttpGet async_write " << request_string);
			LOG(__INFO, "test", "FlvStartRangeHttpClient::HttpGet async_write " << request_string);
		}

		void FlvStartRangeHttpClient::HandleWriteRequest(const boost::asio::error& err, size_t bytes_transferred)
		{
			if( false == is_open_ ) return;
			// 发包成功
			if( !err)
			{
				NETHTTP_INFO("HttpClient::HandleWriteRequest Succed "<<bytes_transferred);
				// 发送 HTTP请求 成功，再接受
				boost::asio::async_read_until(
					socket_, 
					response_, 
						boost::regex("\r\n\r\n"),
						boost::bind(
							&FlvStartRangeHttpClient::HandleReadHttpHeader, 
							boost::static_pointer_cast<FlvStartRangeHttpClient>(shared_from_this()),
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
				NETHTTP_INFO("HttpClient::HandleWriteRequest Error because "<<err.what());
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

		void FlvStartRangeHttpClient::HandleReadHttpHeader(const boost::asio::error& err, size_t bytes_transferred)
		{
			if( false == is_open_ ) return;

			assert( is_requesting_ == true );
			if( is_requesting_ == false ) return;
			is_requesting_ = false;

			assert(recv_timer_);
			recv_timer_->Stop();

			if( !err )
			{
				NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttpHeader Succed " << bytes_transferred);
				assert(bytes_transferred <= response_.size());
				// 接受Http头部成功
				string response_string;
				size_t header_length;
				Buffer buffer(bytes_transferred);
				istream is(&response_);
				is.read((char*)buffer.data_.get(), bytes_transferred);
				response_string.assign((const char*)buffer.data_.get(), bytes_transferred);
				http_response_ = HttpResponse::ParseFromBufferByFlvStart(response_string, request_info_.range_begin_, header_length);
				if( ! http_response_ )
				{
					// 出错，向handler_发送 接受头部出错消息 
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnRecvHttpHeaderFailed, handler_, 1)
						);
					NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttpHeader post IHttpClientListener::OnRecvHttpHeaderFailed " << 1);
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
					NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttpHeader post IHttpClientListener::OnRecvHttpHeaderFailed " << 2);
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
					NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttpHeader post IHttpClientListener::OnRecvHttpHeaderFailed " << 3);
					Close();
					return;
				}

#undef max
				if (request_info_.range_begin_ == 0 || http_response_->GetStatusCode() != 200 || http_response_->GetContentLength() < 13)
				{	// 汇报接受头部成功消息, 不延误13个字节
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnRecvHttpHeaderSucced, handler_, http_response_)
						);
					NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttpHeader post IHttpClientListener::OnRecvHttpHeaderSucced 0");
				}
				else
				{	// 往后再过滤13个字节
					assert(http_response_->HasContentLength());
					content_length_ = http_response_->GetContentLength();

					content_offset_ = 0;
					file_offset_ = http_response_->GetRangeBegin();

					if( content_offset_ + 13 > content_length_)
					{
						assert(0);
						return;
					}
					u_int network_length = 13;
					if( network_length <= response_.size() )
					{
						Buffer buffer(network_length);
						istream is(&response_);
						is.read((char*)buffer.data_.get(), network_length);
						MainThread::IOS().post(
							boost::bind( &IHttpClientListener::OnRecvHttpHeaderSucced, handler_, http_response_)
							);
						NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttpHeader post IHttpClientListener::OnRecvHttpHeaderSucced 1");
						// file_offset 不能动
						content_offset_ += network_length;
						return;
					}
					else
					{
						network_length -= response_.size();
					}

					LOG(__INFO, "network", "async_read length= "<<13 << " network_length="<<network_length<<" response.size()="<<response_.size());
					boost::asio::async_read(
						socket_,
						response_,
						//boost::asio::buffer( buffer.data_.get(), length),
						boost::asio::transfer_at_least(network_length),
						boost::bind( 
							&FlvStartRangeHttpClient::HandleReadHttp13Bytes,
							boost::static_pointer_cast<FlvStartRangeHttpClient>(shared_from_this()),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred,
							13,
							file_offset_,
							content_offset_,
							http_response_
						)
					);
					assert(recv_timer_);
					recv_timer_->ReStart();
					NETHTTP_INFO("FlvStartRangeHttpClient::HttpRecv async_read "<<13<<" "<<file_offset_<<" "<<content_offset_);

					// file_offset 不能动
					content_offset_ += 13;
				}
			}
			else if( err == boost::asio::error::operation_aborted )
			{
				NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttpHeader Error because operation_aborted");
				if( handler_ )
				{
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnRecvHttpHeaderFailed, handler_, 104)
						);
					NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttpHeader post IHttpClientListener::OnRecvHttpHeaderFailed "<<104);
				}
				Close();
			}
			else
			{
				NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttpHeader Error because "<<err.what());
				// 出错，向handler_发送 接受头部出错消息 
				if( handler_ )
				{
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnRecvHttpHeaderFailed, handler_, 4)
					);
					NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttpHeader post IHttpClientListener::OnRecvHttpHeaderFailed "<<4);
				}
				Close();
				return;
			}
		}

		void FlvStartRangeHttpClient::HandleReadHttp13Bytes(const boost::asio::error& err, size_t bytes_transferred, size_t buffer_length, size_t file_offset, size_t content_offset, HttpResponse::p http_response)
		{
			assert(buffer_length == 13);
			assert(file_offset == 0);
			assert(content_offset > 13);
			LOG(__INFO, "network", "HandleReadHttp13Bytes "<<bytes_transferred);
			if( false == is_open_ ) return;

			assert(recv_timer_);
			recv_timer_->Stop();

			if( !err || err == boost::asio::error::eof )
			{
				NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttp13Bytes Succed "<<buffer_length<<" "<<file_offset<<" "<<content_offset);
				assert( response_.size() >= buffer_length );
				Buffer buffer(buffer_length);
				istream is(&response_);
				is.read((char*)buffer.data_.get(), buffer_length);
				// 消息
				MainThread::IOS().post(
					boost::bind( &IHttpClientListener::OnRecvHttpHeaderSucced, handler_, http_response_)
				);
				NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttp13Bytes post IHttpClientListener::OnRecvHttpHeaderSucced 1");
			}
			else if( err == boost::asio::error::operation_aborted )
			{	// 异步操作被取消
				NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttp13Bytes Error because operation_aborted");
				if( handler_ )
				{
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnRecvHttpHeaderFailed, handler_, 101)
					);
				}
				NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttp13Bytes post IHttpClientListener::OnRecvHttpHeaderFailed "<<101);
				Close();
			}
			else
			{
				NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttp13Bytes Error because "<<err.what());
				// 出错，向handler_发送 接受数据出错消息 
				if( handler_ )
				{
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnRecvHttpHeaderFailed, handler_, 1)
					);
				}
				NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttp13Bytes post IHttpClientListener::OnRecvHttpHeaderFailed "<<1);
				Close();
			}
		}

		void FlvStartRangeHttpClient::HttpRecv(size_t length)
		{
			if( false == is_open_ ) return;

			if( content_offset_ >= content_length_)
			{
				// 接受完成 
				MainThread::IOS().post(
					boost::bind( &IHttpClientListener::OnComplete, handler_)
					);
				NETHTTP_INFO("FlvStartRangeHttpClient::HttpRecv post IHttpClientListener::OnComplete");
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
				NETHTTP_INFO("FlvStartRangeHttpClient::HttpRecv post IHttpClientListener::OnRecvHttpDataSucced "<<file_offset_<<" "<<content_offset_);
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
			LOG(__INFO, "network", "async_read length= "<<length << " network_length="<<network_length<<" response.size()="<<response_.size());
			boost::asio::async_read(
				socket_,
				response_,
				//boost::asio::buffer( buffer.data_.get(), length),
				boost::asio::transfer_at_least(network_length),
				boost::bind( 
					&FlvStartRangeHttpClient::HandleReadHttp,
					boost::static_pointer_cast<FlvStartRangeHttpClient>(shared_from_this()),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred,
					length,
					file_offset_,
					content_offset_
				)
			);
			assert(recv_timer_);
			recv_timer_->ReStart();
			NETHTTP_INFO("FlvStartRangeHttpClient::HttpRecv async_read "<<length<<" "<<file_offset_<<" "<<content_offset_);

			//response_offset_ += length;
			file_offset_ += length;
			content_offset_ += length;
		}

		void FlvStartRangeHttpClient::HttpRecvSubPiece()
		{
			// 一个SubPiece 正好是1k,故直接调用 HttpRecvRange
			HttpRecv(1024);
		}

		void FlvStartRangeHttpClient::HandleReadHttp(const boost::asio::error& err, size_t bytes_transferred, size_t buffer_length, size_t file_offset, size_t content_offset)
		{
			LOG(__INFO, "network", "HandleReadHttp "<<bytes_transferred);
			if( false == is_open_ ) return;

			assert( is_requesting_ == true );
			if( is_requesting_ == false ) return;
			is_requesting_ = false;

			assert(recv_timer_);
			recv_timer_->Stop();

			if( ! err )
			{
				NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttp Succed "<<buffer_length<<" "<<file_offset<<" "<<content_offset);
				assert(buffer_length>0);
				assert( response_.size() >= buffer_length );
				Buffer buffer(buffer_length);
				istream is(&response_);
				is.read((char*)buffer.data_.get(), buffer_length);
				// 消息
				MainThread::IOS().post(
					boost::bind( &IHttpClientListener::OnRecvHttpDataSucced, handler_, buffer, file_offset, content_offset)
					);
				NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttp post IHttpClientListener::OnRecvHttpDataSucced "<<file_offset<<" "<<content_offset);
			}
			else if( err == boost::asio::error::operation_aborted )
			{	// 异步操作被取消
				NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttp Error because operation_aborted");
				if( handler_ )
				{
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnRecvHttpDataFailed, handler_, 101)
						);
				}
				NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttp post IHttpClientListener::OnRecvHttpDataFailed "<<101);
				Close();
			}
			else if(err == boost::asio::error::eof)
			{	// 
				NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttp Error because eof");
				if( response_.size() > 0 )
				{
					Buffer buffer(response_.size());
					istream is(&response_);
					is.read((char*)buffer.data_.get(), response_.size());
					// 消息
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnRecvHttpDataPartial, handler_, buffer, file_offset, content_offset)
						);
					NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttp post IHttpClientListener::OnRecvHttpDataPartial "<<file_offset<<" "<<content_offset);
				}
				MainThread::IOS().post(
					boost::bind( &IHttpClientListener::OnRecvHttpDataFailed, handler_, 2)
					);
				NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttp post IHttpClientListener::OnRecvHttpDataFailed "<<2);
				Close();
			}
			else
			{
				NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttp Error because "<<err.what());
				// 出错，向handler_发送 接受数据出错消息 
				if( handler_ )
				{
					MainThread::IOS().post(
						boost::bind( &IHttpClientListener::OnRecvHttpDataFailed, handler_, 1)
						);
				}
				NETHTTP_INFO("FlvStartRangeHttpClient::HandleReadHttp post IHttpClientListener::OnRecvHttpDataFailed "<<1);
				Close();
			}
		}

		bool FlvStartRangeHttpClient::IsBogusAcceptRange() const
		{
			if( !http_response_ )
			{	// 没有获得回应，无法获得是否 假支持Range
				assert(0);
				return false;
			}
			return false;
		}
	}
}