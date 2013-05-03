#include "stdafx.h"
#include "framework/network/HttpAcceptor.h"
#include "framework/network/HttpServer.h"

namespace framework
{
	namespace network
	{
		bool HttpAcceptor::Listen(u_short port)
		{
			if(is_open_ == true)
				return false;
			boost::system::error_code error;
			acceptor_.open(boost::asio::ip::tcp::v4(), error);
			if (error)
			{
				LOG(__ERROR, "http_acceptor", __FUNCTION__ << " Open Error: " << error.message());
				// 出现错误
				return false;
			}
			
			acceptor_.bind(
				boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port), 
				error
			);

			if (!error)
			{	// 绑定成功
                boost::system::error_code err;
                acceptor_.listen(0, err);
                if (!err)
                {
				    port_ = port;
				    is_open_ = true;
				    return true;
                }
                else
                {
                    LOG(__ERROR, "http_acceptor", __FUNCTION__ << " Listen Error: " << err.message());
                    return false;
                }
			}
			else
            {
                LOG(__ERROR, "http_acceptor", __FUNCTION__ << " Bind Error: " << error.message());
                // 出现错误
				return false;
			}
		}

		void HttpAcceptor::TcpAccept()
		{
			if( is_open_ == false )
				return;

			HttpServer::p http_server_for_accept = HttpServer::create();

			acceptor_.async_accept( 
				http_server_for_accept->socket_,
				boost::bind(
					&HttpAcceptor::HandleAccept, 
					shared_from_this(),
					http_server_for_accept,
					boost::asio::placeholders::error
				)
			);
			
		}

		void HttpAcceptor::Close()
		{
            boost::system::error_code err;
			acceptor_.close(err);
			is_open_ = false;
            if (err)
            {
                LOG(__ERROR, "http_acceptor", __FUNCTION__ << " Close Error: " << err.message());
            }
		}

		void HttpAcceptor::HandleAccept(HttpServer::p http_server_for_accept, const boost::system::error_code& err)
		{
			if( is_open_ == false )
				return;

			if( !err )
			{
				try
				{
					// 由于ASIO自己原因，获得EndPoint可能失败
					//boost::asio::ip::tcp::endpoint end_point = http_server_for_accept->GetEndPoint();

					http_server_for_accept->is_open_ = true;
					if( handler_ )
					{
						MainThread::IOS().post(
							boost::bind( &IHttpAcceptorListener::OnHttpAccept, handler_, http_server_for_accept)
						);
					}
				}
				catch(...)
				{	// 错误处理
					assert(0);
					if( handler_ )
					{
						MainThread::IOS().post(
							boost::bind( &IHttpAcceptorListener::OnHttpAcceptFailed, handler_)
						);
					}
				}
			}
			else
			{	// 错误处理
				assert(0);
				if( handler_ )
				{
					MainThread::IOS().post(
						boost::bind( &IHttpAcceptorListener::OnHttpAcceptFailed, handler_)
					);
				}
			}

			TcpAccept();
		}
	}
}
