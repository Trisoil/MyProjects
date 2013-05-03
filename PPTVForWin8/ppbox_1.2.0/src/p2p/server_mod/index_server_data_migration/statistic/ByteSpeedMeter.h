#pragma once

#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include "StatisticUtil.h"
#include "StatisticStructs.h"

namespace statistic
{

	class ByteSpeedMeter
		: public boost::noncopyable
		, public boost::enable_shared_from_this<ByteSpeedMeter>
	{
	public:

		typedef boost::shared_ptr<ByteSpeedMeter> p;

		static p Create();

	public:

		void Start();

		void Stop();

		bool IsRunning() const;

		void Clear();

	public:

		void SubmitBytes(u_int bytes);

		double AverageByteSpeed() const; // bytes per second

		double CurrentByteSpeed(); // 5 seconds

		double RecentMinuteByteSpeed(); // 1 minute

		u_int GetElapsedTimeInMilliSeconds() const;

		boost::uint64_t TotalBytes() const;

	private:

		static u_int GetPositionFromSeconds(u_int seconds);

		void UpdateTickCount();

	private:

		static const u_int SECONDS_IN_MINUTE = 60;

		static const u_int SECONDS_IN_RECENT = 5;

		static const u_int HISTORY_INTERVAL_IN_SEC = SECONDS_IN_MINUTE;

	private:

		u_int start_time_;

		boost::uint64_t total_bytes_;

		u_int history_bytes_[ HISTORY_INTERVAL_IN_SEC ];

		u_int last_sec_;

		u_int last_tick_count_;

		bool is_running_;

	private:

		ByteSpeedMeter();
	};
}
