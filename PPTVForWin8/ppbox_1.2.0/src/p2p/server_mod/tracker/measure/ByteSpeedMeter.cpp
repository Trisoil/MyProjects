#include "Common.h"
//#include "Common.h"
#include "measure/ByteSpeedMeter.h"

namespace measure
{

    ByteSpeedMeter::ByteSpeedMeter()
        : is_running_(false)
    {
        Clear();
    }

    ByteSpeedMeter::p ByteSpeedMeter::Create()
    {
        return p(new ByteSpeedMeter());
    }

    void ByteSpeedMeter::Clear()
    {
        start_time_ = 0;
        total_bytes_ = 0;
        last_sec_ = 0;
        memset(history_bytes_, 0, sizeof(history_bytes_));
    }

    void ByteSpeedMeter::Start()
    {
        if (is_running_)
            return;

        Clear();

        CheckTickCount();
        start_time_ = framework::timer::TickCounter::tick_count();

        is_running_ = true;
    }

    void ByteSpeedMeter::Stop()
    {
        if (!is_running_)
            return;

        Clear();

        is_running_ = false;
    }

    bool ByteSpeedMeter::IsRunning() const
    {
        return is_running_;
    }

    boost::uint32_t ByteSpeedMeter::TotalBytes() const
    {
        return total_bytes_;
    }

    boost::uint32_t ByteSpeedMeter::GetPositionFromSeconds(boost::uint32_t seconds)
    {
        return seconds % HISTORY_INTERVAL_IN_SEC;
    }

    boost::uint32_t ByteSpeedMeter::GetElapsedTimeInMilliSeconds() const
    {
        if (!is_running_)
            return 0;

        boost::uint32_t ms = framework::timer::TickCounter::tick_count() - start_time_;
        return ms <= 0 ? 1 : ms;
    }

    double ByteSpeedMeter::AverageByteSpeed() const
    {
        if (!is_running_)
            return 0.0;

        return 1000.0 * total_bytes_ / GetElapsedTimeInMilliSeconds();
    }

    double ByteSpeedMeter::SecondByteSpeed() // 2 second
    {
        if (!is_running_)
            return 0.0;

        CheckTickCount();

        double bytes_in_recent = 0.0;
        for (boost::uint32_t i = last_sec_; i > last_sec_ - SECONDS_IN_SECOND; i--)
            bytes_in_recent += history_bytes_[GetPositionFromSeconds(i)];

        boost::uint32_t elapsed_time = GetElapsedTimeInMilliSeconds();
        if (elapsed_time > SECONDS_IN_SECOND * 1000)
            return 1.0 * bytes_in_recent / SECONDS_IN_SECOND;
        else
            return 1000.0 * bytes_in_recent / elapsed_time;
    }

    double ByteSpeedMeter::CurrentByteSpeed() // 5 seconds
    {
        if (!is_running_)
            return 0.0;

        CheckTickCount();

        double bytes_in_recent = 0.0;
        for (boost::uint32_t i = last_sec_; i > last_sec_ - SECONDS_IN_RECENT; i--)
            bytes_in_recent += history_bytes_[GetPositionFromSeconds(i)];

        boost::uint32_t elapsed_time = GetElapsedTimeInMilliSeconds();
        if (elapsed_time > SECONDS_IN_RECENT * 1000)
            return 1.0 * bytes_in_recent / SECONDS_IN_RECENT;
        else
            return 1000.0 * bytes_in_recent / elapsed_time;
    }

    double ByteSpeedMeter::RecentByteSpeed() // 20 seconds
    {
        if (!is_running_)
            return 0.0;

        CheckTickCount();

        double bytes_in_recent_20sec = 0.0;
        for (boost::uint32_t i = last_sec_; i > last_sec_ - SECONDS_IN_RECENT_20SEC; i--)
            bytes_in_recent_20sec += history_bytes_[GetPositionFromSeconds(i)];

        boost::uint32_t elapsed_time = GetElapsedTimeInMilliSeconds();
        if (elapsed_time > SECONDS_IN_RECENT_20SEC * 1000)
            return 1.0 * bytes_in_recent_20sec / elapsed_time;
        else
            return 1000.0 * bytes_in_recent_20sec / elapsed_time;
    }

    double ByteSpeedMeter::RecentMinuteByteSpeed() // 1 minute
    {
        if (!is_running_)
            return 0.0;

        CheckTickCount();

        double bytes_in_minute = 0.0;
        for (boost::uint32_t i = 0; i < HISTORY_INTERVAL_IN_SEC; i++)
            bytes_in_minute += history_bytes_[i];

        boost::uint32_t elapsed_time = GetElapsedTimeInMilliSeconds();
        if (elapsed_time > HISTORY_INTERVAL_IN_SEC * 1000)
            return 1.0 * bytes_in_minute / HISTORY_INTERVAL_IN_SEC;
        else
            return 1000.0 * bytes_in_minute / elapsed_time;
    }

    void ByteSpeedMeter::UpdateTickCount(boost::uint32_t & curr_sec)
    {
        if (curr_sec - last_sec_ >= HISTORY_INTERVAL_IN_SEC)
        {
            memset(history_bytes_, 0, sizeof(history_bytes_));
        }
        else
        {
            for (boost::uint32_t i = curr_sec; i > last_sec_; i--)
                history_bytes_[GetPositionFromSeconds(i)] = 0;
        }

        last_sec_ = curr_sec;
    }
}
