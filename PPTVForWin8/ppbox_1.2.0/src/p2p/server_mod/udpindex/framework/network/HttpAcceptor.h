#pragma once

#include "framework/MainThread.h"

namespace framework
{
	namespace network
	{
		class HttpServer;

		interface IHttpAcceptorListener
		{
			typedef boost::shared_ptr<IHttpAcceptorListener> p;
			virtual void OnHttpAccept( boost::shared_ptr<HttpServer> http_server_for_accept) = 0;
			virtual void OnHttpAcceptFailed() = 0;
            virtual ~IHttpAcceptorListener() {}
		};

		class HttpAcceptor : 
			public boost::noncopyable, 
			public boost::enable_shared_from_this<HttpAcceptor>
		{
		public:
			typedef boost::shared_ptr<HttpAcceptor> p;
			static p create(IHttpAcceptorListener::p handler) { return p(new HttpAcceptor(handler)); }
		private:
			HttpAcceptor(IHttpAcceptorListener::p handler) : handler_(handler), acceptor_(MainThread::IOS()), is_open_(false), port_(0) {}

		public:
			// ����
			bool Listen(u_short port);
			void TcpAccept();
			void Close();
		public:
			// ����
			u_short GetHttpPort() const { return port_; }
		protected:
			void HandleAccept(boost::shared_ptr<HttpServer> http_server_for_accept, const boost::system::error_code& err);

		private:
			// ������Ϣ��ȥ�ľ��
			IHttpAcceptorListener::p handler_;
			// �����ɹ��Ķ˿�
			u_short port_;
			// HTTP �����Ķ���
			boost::asio::ip::tcp::acceptor acceptor_;
			// �Ƿ���
			bool is_open_;
		};
	}
}