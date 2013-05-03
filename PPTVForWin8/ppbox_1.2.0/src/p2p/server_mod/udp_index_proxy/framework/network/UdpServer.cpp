#include "stdafx.h"
#include "framework/network/UdpServer.h"
#include "framework/network/util.h"
#include "framework/log.h"

namespace framework
{
	namespace network
	{
		bool UdpServer::Listen(u_short port)
		{
			if( is_open_ == true )
			{
				//LOG(__ERROR, LOG_TYPE_NETWORK, "this UdpServer is open, so can not listen again");
				return false;
			}
			boost::system::error_code error;
			socket_.open(boost::asio::ip::udp::v4(), error);
			if (error)
			{
				return false;
			}
			socket_.bind(
				boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port),		// endpoint
				error										// errorhandle
			);

			if (!error)
			{	// 绑定成功
				is_open_ = true;
				port_ = port;
				return true;
			}
			else
			{	// 出现错误
				return false;
			}
		}

		void UdpServer::Recv(u_int count)
		{
			for( u_int i = 0; i < count; i ++ )
				UdpRecvFrom();
		}
		
		void UdpServer::UdpRecvFrom()
		{
			if( is_open_ == false ) return;

			UDPRecvBuffer recv_buffer;
			recv_buffers_.push_back( recv_buffer );
			socket_.async_receive_from(
				boost::asio::buffer(recv_buffer.data_.get(), UDPRecvBuffer::UDP_RECV_BUFFER_SIZE), 
				*recv_buffer.remote_endpoint_,
				boost::bind(
					&UdpServer::HandleUdpRecvFrom, 
					shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred,
					recv_buffer 
				)
			);
		}

		bool UdpServer::UdpSendTo(const Buffer& send_buffer, const boost::asio::ip::udp::endpoint& remote_endpoint)
		{
			if( is_open_ == false ) return false;

			LOG(__DEBUG, "network", "UdpServer::UdpSendTo "<<remote_endpoint<<" Length="<<send_buffer.length_);

			socket_.async_send_to(
				boost::asio::buffer(send_buffer.data_.get(), send_buffer.length_), 
				remote_endpoint,
				boost::bind(
					&UdpServer::HandleUdpSendTo, 
					shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred,
					remote_endpoint,
					send_buffer
				)
			);	

			return true;
		}

		void UdpServer::HandleUdpRecvFrom(const boost::system::error_code& error, std::size_t bytes_transferred, UDPRecvBuffer& recv_buffer)
		{
			if( !error )
			{	// UDP 收包正确，向上层次发送消息
				assert(bytes_transferred <= UDPRecvBuffer::UDP_RECV_BUFFER_SIZE);
				
				Buffer buffer(recv_buffer.data_, bytes_transferred);
				
				if( handler_ )
				{
					MainThread::IOS().post(
						boost::bind( &IUdpServerListener::OnUdpRecv, handler_, *recv_buffer.remote_endpoint_, buffer)
					);
				}
			}
			
			if( is_open_ == false ) return;
			// 继续收包
			socket_.async_receive_from(
				boost::asio::buffer(recv_buffer.data_.get(), UDPRecvBuffer::UDP_RECV_BUFFER_SIZE), 
				*recv_buffer.remote_endpoint_,
				boost::bind(
					&UdpServer::HandleUdpRecvFrom, 
					shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred,
					recv_buffer 
				)
			);
		}

		void UdpServer::HandleUdpSendTo(const boost::system::error_code& error, std::size_t bytes_transferred, boost::asio::ip::udp::endpoint& remote_endpoint, Buffer& send_buffer)
		{
			if( !error )
			{	// 发包正确
				LOG(__DEBUG, "network", "UdpServer::HandleUdpSendTo Succeed "<<remote_endpoint<<" Length="<<bytes_transferred);
			}
			else
			{	// 发包出错
				LOG(__DEBUG, "network", "UdpServer::HandleUdpSendTo Failed "<<remote_endpoint<<" Length="<<bytes_transferred);
			}
		}

		u_short UdpServer::GetUdpPort() const
		{
			if( is_open_ == false ) return 0;
			return port_;
		}

		void UdpServer::Close()
		{
			// ???忽略socket关闭错误，不抛异常
			socket_.close(boost::system::error_code());
			is_open_ = false;
			recv_buffers_.clear();
		}
	}
}

