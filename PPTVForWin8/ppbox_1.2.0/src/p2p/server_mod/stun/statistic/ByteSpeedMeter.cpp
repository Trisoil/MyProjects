#include "Common.h"
#include "ByteSpeedMeter.h"

#include <algorithm>
using namespace std;

namespace statistic
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
        last_tick_count_ = 0;
        last_sec_ = 0;
        memset(history_bytes_, 0, sizeof(history_bytes_));
    }

    void ByteSpeedMeter::Start()
    {
        if (is_running_ == true)
            return ;
    
        Clear();

        UpdateTickCount();
        start_time_ = last_tick_count_;

        is_running_ = true;
    }

    void ByteSpeedMeter::Stop()
    {
        if (is_running_ == false)
            return ;

        Clear();

        is_running_ = false;
    }

    bool ByteSpeedMeter::IsRunning() const
    {
        return is_running_;
    }

    boost::uint64_t ByteSpeedMeter::TotalBytes() const
    {
        return total_bytes_;
    }

    boost::uint32_t ByteSpeedMeter::GetPositionFromSeconds(boost::uint32_t seconds)
    {
        return seconds % HISTORY_INTERVAL_IN_SEC;
    }

    boost::uint32_t ByteSpeedMeter::GetElapsedTimeInMilliSeconds() const
    {
//         if (is_running_ == false)
//             return 0;
// 
//         u_int ms = GetTickCountInMilliSecond() - start_time_;
//         return ms <= 0 ? 1 : ms;
        if (is_running_ == false)
            return 0;

        boost::uint32_t ms = framework::timer::TickCounter::tick_count() - start_time_;
        return ms <= 0 ? 1 : ms;
    }

    void ByteSpeedMeter::SubmitBytes(boost::uint32_t bytes)
    {
        if (is_running_ == false)
            return ;

        UpdateTickCount();

        total_bytes_ += bytes;
        history_bytes_[ GetPositionFromSeconds(last_sec_) ] += bytes;
    }

    double ByteSpeedMeter::AverageByteSpeed() const
    {
        if (is_running_ == false)
            return 0.0;

        return 1000.0 * total_bytes_ / GetElapsedTimeInMilliSeconds();
    }

    double ByteSpeedMeter::CurrentByteSpeed() // 5 seconds
    {
        if (is_running_ == false)
            return 0.0;

        UpdateTickCount();

        double bytes_in_recent = 0.0;
        for (boost::uint32_t i = last_sec_; i > last_sec_ - SECONDS_IN_RECENT; i--)
            bytes_in_recent += history_bytes_[ GetPositionFromSeconds(i) ];

        boost::uint32_t elapsed_time = GetElapsedTimeInMilliSeconds();
        if (elapsed_time > SECONDS_IN_RECENT * 1000)
            return 1.0 * bytes_in_recent / SECONDS_IN_RECENT;
        else
            return 1000.0 * bytes_in_recent / elapsed_time;
    }

    double ByteSpeedMeter::RecentMinuteByteSpeed() // 1 minute
    {
        if (is_running_ == false)
            return 0.0;

        UpdateTickCount();

        double bytes_in_minute = 0.0;
        for (boost::uint32_t i = 0; i < HISTORY_INTERVAL_IN_SEC; i++)
            bytes_in_minute += history_bytes_[i];

        boost::uint32_t elapsed_time = GetElapsedTimeInMilliSeconds();
        if (elapsed_time > HISTORY_INTERVAL_IN_SEC * 1000)
            return 1.0 * bytes_in_minute / HISTORY_INTERVAL_IN_SEC;
        else
            return 1000.0 * bytes_in_minute / elapsed_time;
    }

    void ByteSpeedMeter::UpdateTickCount()
    {
        boost::uint32_t curr_tick_count = framework::timer::TickCounter::tick_count();
        boost::uint32_t curr_sec = curr_tick_count / 1000;
        
        assert(curr_sec >= last_sec_);

        if (curr_sec == last_sec_)
            return ;

        if (curr_sec - last_sec_ >= HISTORY_INTERVAL_IN_SEC)
        {
            memset(history_bytes_, 0, sizeof(history_bytes_));
        }
        else
        {
            for (boost::uint32_t i = curr_sec; i > last_sec_; i--)
                history_bytes_[ GetPositionFromSeconds(i) ] = 0;
        }

        last_tick_count_ = curr_tick_count;
        last_sec_ = curr_sec;
    }
}
