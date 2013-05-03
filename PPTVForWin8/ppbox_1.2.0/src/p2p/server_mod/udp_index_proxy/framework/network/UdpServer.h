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

		// UDP�հ� �� ������
		struct UDPRecvBuffer
		{
			// ÿ��UDP�հ�������ֽ���,��ΪUDPû�о�ȷ�հ���С
			static const size_t UDP_RECV_BUFFER_SIZE = 5*1024;
			// ����UDP���������ݵĻ�����
			boost::shared_array<byte> data_;
			// �Է���endpoint
			boost::shared_ptr<boost::asio::ip::udp::endpoint> remote_endpoint_;
			// ���캯��
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
			// UDP�հ� �� ���ݽṹ
			vector<UDPRecvBuffer> recv_buffers_;
			// �Լ��ļ��� endpoint
			boost::asio::ip::udp::socket socket_;
			// ������Ϣ��ȥ�ľ��
			IUdpServerListener::p handler_;
			//
			bool is_open_;
			//
			u_short port_;
		};
	}
}