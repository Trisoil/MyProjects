#pragma once

#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include "ByteSpeedMeter.h"

namespace statistic
{
	class SpeedInfoStatistic 
		: public boost::noncopyable
		, public boost::enable_shared_from_this<SpeedInfoStatistic>
	{
	public:

		typedef boost::shared_ptr<SpeedInfoStatistic> p;

		static p Create();

		void Start();

		void Stop();

		void Clear();

		bool IsRunning() const;

	public:

		//////////////////////////////////////////////////////////////////////////
		// Speed Info

		void SubmitDownloadedBytes(u_int downloaded_bytes);

		void SubmitUploadedBytes(u_int uploaded_bytes);

		SPEED_INFO GetSpeedInfo();

        //////////////////////////////////////////////////////////////////////////
        // Online Time Info

        u_int GetElapsedTimeInMilliSeconds() const;

	private:

		SpeedInfoStatistic();

		bool is_running_;

	private:

		ByteSpeedMeter::p download_speed_meter_;

		ByteSpeedMeter::p upload_speed_meter_;

		u_int start_time_;
	};
}