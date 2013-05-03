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
			virtual void OnResolverFailed(u_int error_code) = 0; // 1-Url有问题 2-域名无法解析 3-域名解析出错 4-连接失败

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
			// 构造		
			Resolver(string url, u_short port, IResolverListener::p handler);
		public:
			// 操作
			void Close();
			void DoResolver();
		public:
			// 属性
			void SetResolverTimeout(u_int resolver_timeout) { resolver_timeout_ = resolver_timeout; }
		private:
			// 消息
			void HandleResolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
			// 定时器消息
			virtual void OnTimerElapsed(framework::timer::Timer::p pointer, u_int times);
			void HandleResolveTimeout();
		private:
			// 变量
			boost::asio::ip::tcp::endpoint endpoint_;
			boost::asio::ip::tcp::resolver resolver_;
			string url_;
			u_short port_;
			// 定时器
			framework::timer::OnceTimer::p resolver_timer_;	// 域名解析 超时 定时器
			u_int resolver_timeout_;	// 域名解析 超时 定时时间，以毫秒为单位
			// 消息接受者
			IResolverListener::p handler_;
			// 状态
			bool is_resolving_;
			u_int failed_times_;
		};
	}
	

}