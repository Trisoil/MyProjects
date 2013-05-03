#pragma once

#include "framework/timer/Timer.h"


namespace framework
{
	namespace network
	{
		interface IResolverListener
		{
			typedef boost::shared_ptr<IResolverListener> p;

			virtual void OnResolverSucced(u_long ip, u_short port) = 0;
			virtual void OnResolverFailed(u_int error_code) = 0; // 1-Url������ 2-�����޷����� 3-������������ 4-����ʧ��

            virtual ~IResolverListener() {}
		};

		class Resolver
			: public boost::noncopyable
			, public boost::enable_shared_from_this<Resolver>
			, public framework::timer::ITimerListener
		{
		public:
			typedef boost::shared_ptr<Resolver> p;
			static p create(string url, u_short port, IResolverListener::p handler);
		public:
			// ����		
			Resolver(string url, u_short port, IResolverListener::p handler);
		public:
			// ����
			void Close();
			void DoResolver();
		public:
			// ����
			void SetResolverTimeout(u_int resolver_timeout) { resolver_timeout_ = resolver_timeout; }
		private:
			// ��Ϣ
			void HandleResolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
			// ��ʱ����Ϣ
			virtual void OnTimerElapsed(framework::timer::Timer::p pointer, u_int times);
			void HandleResolveTimeout();
		private:
			// ����
			boost::asio::ip::tcp::endpoint endpoint_;
			boost::asio::ip::tcp::resolver resolver_;
			string url_;
			u_short port_;
			// ��ʱ��
			framework::timer::OnceTimer::p resolver_timer_;	// �������� ��ʱ ��ʱ��
			u_int resolver_timeout_;	// �������� ��ʱ ��ʱʱ�䣬�Ժ���Ϊ��λ
			// ��Ϣ������
			IResolverListener::p handler_;
			// ״̬
			bool is_resolving_;
			u_int failed_times_;
		};
	}
	

}