#include "stdafx.h"
#include "framework/Log.h"
#include "framework/network/Resolver.h"

#include <boost/lexical_cast.hpp>

namespace framework
{
	namespace network
	{
		Resolver::Resolver(string url, u_short port, IResolverListener::p handler)
			: resolver_timeout_(30*1000)
			, resolver_(MainThread::IOS())
			, is_resolving_(false)
			, handler_(handler)
			, failed_times_(0)
			, url_(url)
			, port_(port)
			
		{
		}

		Resolver::p Resolver::create(string url, u_short port, IResolverListener::p handler)
		{
			return Resolver::p(new Resolver(url,  port, handler));
		}

		void Resolver::Close()
		{
			resolver_.cancel();
			if( resolver_timer_ )
				resolver_timer_->Stop();
		}

		void Resolver::DoResolver()
		{	
			if( true == is_resolving_) return;
			is_resolving_ = true;

			string domain = url_;
			if( domain == "" )
			{	// 出错，向handler_发送 Url 错误消息
				if( handler_)
				{
					MainThread::IOS().post(
						boost::bind( &IResolverListener::OnResolverFailed, handler_, 1)
						);
				}
				Close();
			}

			boost::asio::ip::tcp::resolver::query query( domain, boost::lexical_cast<string>(80) );
			resolver_.async_resolve(
				query,
				boost::bind(
				&Resolver::HandleResolve, 
				shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::iterator
				)
				);
			resolver_timer_ = framework::timer::OnceTimer::create(resolver_timeout_, shared_from_this());
			resolver_timer_->Start();	
		}

		void Resolver::OnTimerElapsed(framework::timer::Timer::p pointer, u_int times)
		{
			if (false == is_resolving_) return;

			if( pointer == resolver_timer_ )
			{
				HandleResolveTimeout();
				resolver_timer_.reset();
			}
			else
				assert(0);
		}

		void Resolver::HandleResolveTimeout()
		{
			if (false == is_resolving_) return;

			LOG(__INFO, "test", "HandleResolveTimeout failed times:"<<failed_times_<<url_);

			is_resolving_ = false;

			if (failed_times_ >= 0)
			{
				if( handler_)
				{
					MainThread::IOS().post(
						boost::bind( &IResolverListener::OnResolverFailed, handler_, 4)
						);
				}			
			}
			else
			{
				failed_times_++;
				DoResolver();
			}
		}

		void Resolver::HandleResolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
		{
			if( false == is_resolving_ ) return;

			is_resolving_ = false;

			assert(resolver_timer_);
			resolver_timer_->Stop();
			resolver_timer_.reset();

			if (!err)
			{
				endpoint_ = *endpoint_iterator;
				LOG(__INFO, "test", "HandleResolveSucced "<< endpoint_<<url_);
				if( handler_)
				{
					MainThread::IOS().post(
						boost::bind( &IResolverListener::OnResolverSucced, handler_, endpoint_.address().to_v4().to_ulong(), port_)
						);
				}
				
			}
			else if( err == boost::asio::error::operation_aborted )
			{	// 操作被取消
			}
			else
			{	// 出错，向handler_发送 域名解析失败 错误消息
				if( handler_)
				{
					LOG(__INFO, "test", "HandleResolveFailed"<< url_);
					MainThread::IOS().post(
						boost::bind( &IResolverListener::OnResolverFailed, handler_, 2)
						);
				}
				Close();
			}
		}

	}
}