#ifndef _BYTE_SPEED_METER_H_
#define _BYTE_SPEED_METER_H_

#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

// #include "StatisticUtil.h"
// #include "StatisticStructs.h"

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

        void SubmitBytes(boost::uint32_t bytes);

        double AverageByteSpeed() const; // bytes per second

        double CurrentByteSpeed(); // 5 seconds

        double RecentMinuteByteSpeed(); // 1 minute

        boost::uint32_t GetElapsedTimeInMilliSeconds() const;

        boost::uint64_t TotalBytes() const;

    private:

        static boost::uint32_t GetPositionFromSeconds(boost::uint32_t seconds);

        void UpdateTickCount();

    private:

        static const boost::uint32_t SECONDS_IN_MINUTE = 60;

        static const boost::uint32_t SECONDS_IN_RECENT = 5;

        static const boost::uint32_t HISTORY_INTERVAL_IN_SEC = SECONDS_IN_MINUTE;

    private:

        boost::uint32_t start_time_;

        boost::uint64_t total_bytes_;

        boost::uint32_t history_bytes_[ HISTORY_INTERVAL_IN_SEC ];

        boost::uint32_t last_sec_;

        boost::uint32_t last_tick_count_;

        bool is_running_;

    private:

        ByteSpeedMeter();
    };
}
#endif // _BYTE_SPEED_METER_H_