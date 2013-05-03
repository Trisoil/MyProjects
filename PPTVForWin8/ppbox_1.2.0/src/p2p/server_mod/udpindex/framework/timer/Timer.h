#pragma once

#include "framework/MainThread.h"

namespace framework
{
	namespace timer
	{
		class Timer;

		interface ITimerListener
		{
			typedef boost::shared_ptr<ITimerListener> p;
			virtual void OnTimerElapsed(boost::shared_ptr<Timer> pointer, u_int times) = 0;

            virtual ~ITimerListener() {}
		};

		class Timer : 
			public boost::noncopyable, 
			public boost::enable_shared_from_this<Timer>
		{
		public:
			typedef boost::shared_ptr<Timer> p;
		public:
			void SetInterval(u_int interval);
			void SetIntervalInSeconds(u_int interval_in_seconds);
            u_int GetInterval() const { return interval_; }
			void Start();
			void ReStart();
			void Stop();
		protected:
			virtual void DoOnceStart();
			virtual void DoOnTimer() = 0;
		protected:
			Timer(UINT interval_in_milliseconds, ITimerListener::p handler) 
				: interval_(interval_in_milliseconds), handler_(handler)
				, is_started(false), asio_timer_(framework::MainThread::IOS()) {};
			void OnTimer(const boost::system::error_code& err);
		protected:
			/// 是否已经启动
			bool is_started;
			/// 定时器的间隔时间（毫秒）
			u_int interval_;
			/// 响应定时器时间的 Handler
			ITimerListener::p handler_;
			/// asio 的定时起对象
			boost::asio::deadline_timer asio_timer_;
		};

		class OnceTimer : public Timer
		{
		public:
			typedef boost::shared_ptr<OnceTimer> p;
			static p create(UINT interval, ITimerListener::p handler) { return p(new OnceTimer(interval, handler)); }
		private:
			virtual void DoOnTimer();
			inline p shared_fromn_this_ex() { return boost::static_pointer_cast<OnceTimer>(shared_from_this()); }
			
		protected:
			OnceTimer(UINT interval, ITimerListener::p handler) : Timer(interval, handler) {}
		};

		class PeriodicTimer 
			: public Timer
		{
		public:
			typedef boost::shared_ptr<PeriodicTimer> p;
			static p create(UINT interval, ITimerListener::p handler) { return p(new PeriodicTimer(interval, handler)); }
		private:
			PeriodicTimer(UINT interval, ITimerListener::p handler) : Timer(interval, handler), times_(0) {}
			inline p shared_fromn_this_ex() { return boost::static_pointer_cast<PeriodicTimer>(shared_from_this()); }


		protected:
			virtual void DoOnTimer();
		protected:
			/// timer事件触发的次数
			u_int times_;
		};
	}
}