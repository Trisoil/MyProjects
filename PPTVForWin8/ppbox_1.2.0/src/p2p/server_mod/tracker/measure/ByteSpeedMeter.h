#ifndef FRAMEWORK_STATISTIC_BYTESPEEDMETER_H
#define FRAMEWORK_STATISTIC_BYTESPEEDMETER_H

#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <framework/timer/Timer.h>
#include <framework/timer/TickCounter.h>

namespace measure
{

    class ByteSpeedMeter: public boost::noncopyable, public boost::enable_shared_from_this<ByteSpeedMeter>
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

        void SubmitBytes(boost::uint32_t bytes);

        double AverageByteSpeed() const; // bytes per second

        double SecondByteSpeed(); // 1 second

        double CurrentByteSpeed(); // 5 seconds

        double RecentByteSpeed(); // 20 seconds

        double RecentMinuteByteSpeed(); // 1 minute

        boost::uint32_t GetElapsedTimeInMilliSeconds() const;

        boost::uint32_t TotalBytes() const;

    private:

        static boost::uint32_t GetPositionFromSeconds(boost::uint32_t seconds);

        void CheckTickCount();

        void UpdateTickCount(boost::uint32_t & curr_sec);

    private:

        static const boost::uint32_t SECONDS_IN_SECOND = 1;

        static const boost::uint32_t SECONDS_IN_RECENT = 5;

        static const boost::uint32_t SECONDS_IN_RECENT_20SEC = 20;

        static const boost::uint32_t SECONDS_IN_MINUTE = 60;

        static const boost::uint32_t HISTORY_INTERVAL_IN_SEC = SECONDS_IN_MINUTE;

    private:

        boost::uint32_t start_time_;

        boost::uint32_t total_bytes_;

        boost::uint32_t history_bytes_[HISTORY_INTERVAL_IN_SEC];

        boost::uint32_t last_sec_;

        bool is_running_;

    private:

        ByteSpeedMeter();
    };

    inline void ByteSpeedMeter::SubmitBytes(boost::uint32_t bytes)
    {
        CheckTickCount();
        total_bytes_ += bytes;
        history_bytes_[GetPositionFromSeconds(last_sec_)] += bytes;
    }

    inline void ByteSpeedMeter::CheckTickCount()
    {
        boost::uint32_t curr_sec = framework::timer::TickCounter::tick_count() / 1000;
        if (curr_sec == last_sec_)
            return;
        UpdateTickCount(curr_sec);
    }
}

#endif//FRAMEWORK_STATISTIC_BYTESPEEDMETER_H
