#pragma once

#include "framework/network/HttpClient.h"

namespace framework
{
	namespace network
	{
		class FlvStartRangeHttpClient
			: public framework::network::HttpClient
		{
		public:
			typedef boost::shared_ptr<FlvStartRangeHttpClient> p;
			static p create(string url, string refer_url="", u_int range_begin=0, u_int range_end=0);
			static p create(string domain, u_short port, string request, string refer_url="", u_int range_begin=0, u_int range_end=0);
		public:
			// 构造		
			FlvStartRangeHttpClient(string domain, u_short port, string request, string refer_url, u_int range_begin, u_int range_end);
		public:
			// 操作
			void HttpGet();
			void HttpRecv(size_t length);
			void HttpRecvSubPiece();
		public:
			// 属性
			bool IsBogusAcceptRange() const;
		protected:
			// 消息
			virtual void HandleWriteRequest(const boost::asio::error& err, size_t bytes_transferred);
			void HandleReadHttpHeader(const boost::asio::error& err, size_t bytes_transferred);
			void HandleReadHttp(const boost::asio::error& err, size_t bytes_transferred, size_t buffer_length, size_t file_offset, size_t content_offset);
			void HandleReadHttp13Bytes(const boost::asio::error& err, size_t bytes_transferred, size_t buffer_length, size_t file_offset, size_t content_offset, HttpResponse::p http_response);
		};
	}
}

