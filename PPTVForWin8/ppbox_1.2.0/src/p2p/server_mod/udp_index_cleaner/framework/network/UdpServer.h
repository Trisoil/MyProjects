#pragma once

#include "framework/MainThread.h"

namespace framework
{
	namespace network
	{
		interface IUdpServerListener
		{
			typedef boost::shared_ptr<IUdpServerListener> p;
			virtual void OnUdpRecv(boost::asio::ip::udp::endpoint& end_point, Buffer& buf) = 0;

            virtual ~IUdpServerListener() {}
		};

		// UDP收包 的 缓冲类
		struct UDPRecvBuffer
		{
			// 每次UDP收包的最大字节数,因为UDP没有精确收包大小
			static const size_t UDP_RECV_BUFFER_SIZE = 5*1024;
			// 接受UDP发来的数据的缓冲区
			boost::shared_array<byte> data_;
			// 对方的endpoint
			boost::shared_ptr<boost::asio::ip::udp::endpoint> remote_endpoint_;
			// 构造函数
			UDPRecvBuffer() : data_(new byte[UDP_RECV_BUFFER_SIZE]), remote_endpoint_(new boost::asio::ip::udp::endpoint){};
		};

		class UdpServer : 
			public boost::noncopyable, 
			public boost::enable_shared_from_this<UdpServer>
		{
		public:
			typedef boost::shared_ptr<UdpServer> p;
            static p create(IUdpServerListener::p handler) { return p(new UdpServer(handler)); }
		private:
			UdpServer(IUdpServerListener::p handler) : handler_(handler), port_(0), socket_(MainThread::IOS()), is_open_(false) {}

		public:
			bool Listen(u_short port);
			void Recv(u_int count);
			void UdpRecvFrom();
			bool UdpSendTo(const Buffer& buffer, const boost::asio::ip::udp::endpoint& endpoint);
			void Close();

			u_short GetUdpPort() const;

		protected:
			
			void HandleUdpRecvFrom(const boost::system::error_code& error, std::size_t bytes_transferred, UDPRecvBuffer& recv_buffer);
			void HandleUdpSendTo(const boost::system::error_code& error, std::size_t bytes_transferred, boost::asio::ip::udp::endpoint& remote_endpoint, Buffer& send_buffer);

		private:
			// UDP收包 的 数据结构
			vector<UDPRecvBuffer> recv_buffers_;
			// 自己的监听 endpoint
			boost::asio::ip::udp::socket socket_;
			// 发送消息回去的句柄
			IUdpServerListener::p handler_;
			//
			bool is_open_;
			//
			u_short port_;
		};
	}
}