#include "mutex/server/Common.h"

namespace mutexserver
{
    boost::uint64_t current_delay_time_;
    boost::uint16_t interval_;

    void update_time()
    {
        time_t t_tick_;
        time(&t_tick_);
        current_delay_time_ = (boost::uint64_t)t_tick_;
    }

    boost::uint64_t get_last_time()
    {
        return current_delay_time_;
    }

    void set_interval(
        boost::uint16_t interval)
    {
        interval_ = interval;
    }

    boost::uint16_t get_config_interval()
    {
        return interval_;
    }

    std::string byteArray2Str(
        const boost::uint8_t * arr, size_t len)
    {
        char const * hex_chr = "0123456789abcdef";
        std::string hex;
        for (size_t i = 0; i < len; i++) {
            boost::uint8_t d = arr[i];
            hex.append(1, hex_chr[d >> 4]);
            hex.append(1, hex_chr[d & 0x0F]);
        }
        return hex;
    }
}