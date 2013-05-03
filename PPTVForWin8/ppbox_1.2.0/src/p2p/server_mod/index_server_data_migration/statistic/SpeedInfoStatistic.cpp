#include "stdafx.h"
#include "SpeedInfoStatistic.h"

namespace statistic
{
	SpeedInfoStatistic::SpeedInfoStatistic()
		: is_running_(false)
		, download_speed_meter_(ByteSpeedMeter::Create())
		, upload_speed_meter_(ByteSpeedMeter::Create())
	{
	}

	SpeedInfoStatistic::p SpeedInfoStatistic::Create()
	{
		return p(new SpeedInfoStatistic());
	}

	void SpeedInfoStatistic::Start()
	{
		STAT_DEBUG("SpeedInfoStatistic::Start [IN]");
		if (is_running_ == true)
		{
			STAT_WARN("SpeedInfoStatistic is running, return.");
			return ;
		}

		Clear();

		start_time_ = GetTickCountInMilliSecond();
		download_speed_meter_->Start();
		upload_speed_meter_->Start();

		is_running_ = true;
		STAT_DEBUG("SpeedInfoStatistic::Start [OUT]");
	}

	void SpeedInfoStatistic::Stop()
	{
		STAT_DEBUG("SpeedInfoStatistic::Stop [IN]");
		if (is_running_ == false)
		{
			STAT_WARN("SpeedInfoStatistic is not running, return.");
			return ;
		}

		Clear();

		download_speed_meter_->Stop();
		upload_speed_meter_->Stop();
		start_time_ = 0;

		is_running_ = false;
		STAT_DEBUG("SpeedInfoStatistic::Stop [OUT]");
	}

	void SpeedInfoStatistic::Clear()
	{
		if (is_running_ == false)
			return ;

		start_time_ = 0;
		download_speed_meter_->Clear();
		upload_speed_meter_->Clear();
	}

	bool SpeedInfoStatistic::IsRunning() const
	{
		return is_running_;
	}

	void SpeedInfoStatistic::SubmitDownloadedBytes(u_int downloaded_bytes)
	{
		if (is_running_ == false)
			return ;
		download_speed_meter_->SubmitBytes(downloaded_bytes);
	}

	void SpeedInfoStatistic::SubmitUploadedBytes(u_int uploaded_bytes)
	{
		if (is_running_ == false)
			return ;
		upload_speed_meter_->SubmitBytes(uploaded_bytes);
	}

	SPEED_INFO SpeedInfoStatistic::GetSpeedInfo()
	{
		SPEED_INFO speed_info;

        if (is_running_ == false)
            return speed_info;

		speed_info.StartTime = start_time_;

		speed_info.AvgDownloadSpeed = download_speed_meter_->AverageByteSpeed();
		speed_info.AvgUploadSpeed = upload_speed_meter_->AverageByteSpeed();

		speed_info.TotalDownloadBytes = download_speed_meter_->TotalBytes();
		speed_info.TotalUploadBytes = upload_speed_meter_->TotalBytes();

		speed_info.NowDownloadSpeed = download_speed_meter_->CurrentByteSpeed();
		speed_info.NowUploadSpeed = upload_speed_meter_->CurrentByteSpeed();

		speed_info.MinuteDownloadSpeed = download_speed_meter_->RecentMinuteByteSpeed();
		speed_info.MinuteUploadSpeed = upload_speed_meter_->RecentMinuteByteSpeed();

		return speed_info;
	}

    //////////////////////////////////////////////////////////////////////////
    // Online Time Info

    u_int SpeedInfoStatistic::GetElapsedTimeInMilliSeconds() const
    {
        return download_speed_meter_->GetElapsedTimeInMilliSeconds();
    }
}
