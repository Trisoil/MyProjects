#include "stdafx.h"
#include "framework/timer/Timer.h"


namespace framework
{
	namespace timer
	{
		void Timer::SetInterval(u_int interval)
		{
			interval_ = interval;
		}

		void Timer::SetIntervalInSeconds(u_int interval_in_seconds)
		{
			interval_ = 1000 * interval_in_seconds;
		}

		void Timer::Start()
		{
			if( ! is_started )
			{
				DoOnceStart();
                is_started = true;
			}
		}

		void Timer::Stop()
		{
			if( is_started )
			{
				asio_timer_.cancel(boost::system::error_code());
				is_started = false;
			}
		}

		void Timer::ReStart()
		{
			Stop();
			Start();
		}

		void Timer::DoOnceStart()
		{
		    asio_timer_.expires_from_now(boost::posix_time::milliseconds(interval_));
		    asio_timer_.async_wait(
			    boost::bind(&Timer::OnTimer, shared_from_this(), boost::asio::placeholders::error)
		    );
		}

		void Timer::OnTimer(const boost::system::error_code& err)
		{	
            if (err || !is_started)
                return ;

            DoOnTimer();
		}

		void OnceTimer::DoOnTimer()
		{
			MainThread::IOS().post(
				boost::bind(&ITimerListener::OnTimerElapsed, handler_, shared_from_this(), 1 )
			);
			is_started = false;
		}

		void PeriodicTimer::DoOnTimer()
		{
		    times_ ++;
		    MainThread::IOS().post(
			    boost::bind(&ITimerListener::OnTimerElapsed, handler_, shared_from_this(), times_ )
		    );
		    DoOnceStart();
		}
	}
}