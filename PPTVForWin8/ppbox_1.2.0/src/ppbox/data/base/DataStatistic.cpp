// BufferStatistic.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/base/DataStatistic.h"

#include <framework/timer/Ticker.h>

namespace ppbox
{
    namespace data
    {

        DataStatistic::DataStatistic()
            : start_time(time(NULL))
            , total_bytes(0)
            , zero_time(0)
        {
            speeds[0].interval = ONE_REG;
            speeds[1].interval = FIVE_REG;
            speeds[2].interval = TWENTY_REG;
            speeds[3].interval = SIXTY_REG;
            for (boost::uint32_t i = 0; i < sizeof(speeds) / sizeof(SpeedStatistics); i++) {
                speeds[i].time_left = speeds[i].interval;
            }
        }

        DataObserver::DataObserver()
        {
            ticker_ = new framework::timer::Ticker(1000);
        }

        DataObserver::~DataObserver()
        {
            delete ticker_;
            ticker_ = NULL;
        }

        boost::uint32_t DataObserver::get_zero_interval()
        {
            if (zero_time == 0) {
                return 0;
            } else {
                return (boost::uint32_t)((ticker_->elapse() - zero_time) / 1000);
            }
        }

        void DataObserver::reset_zero_interval()
        {
            zero_time = 0;
        }

        void DataObserver::increase_bytes(
            boost::uint32_t byte_size)
        {
            total_bytes += byte_size; //记录总下载字节数
            boost::uint64_t milli_sec = 0;
            if (ticker_->check(milli_sec)) {
                if ((boost::uint32_t)total_bytes == speeds[0].last_bytes) {
                    if (zero_time == 0)
                        zero_time = milli_sec;
                } else {
                    zero_time = 0;
                }
                for (size_t i = 0;i< sizeof(speeds)/sizeof(SpeedStatistics);i++) {
                    if (--speeds[i].time_left == 0) {
                        speeds[i].time_left = speeds[i].interval;
                        if (milli_sec != speeds[i].last_milli_sec) {
                            speeds[i].cur_speed = 
                                (boost::uint32_t)((total_bytes - speeds[i].last_bytes) * 1000 / (milli_sec - speeds[i].last_milli_sec));
                            if (speeds[i].cur_speed > speeds[i].peak_speed) {
                                speeds[i].peak_speed = speeds[i].cur_speed;
                            }
                        }
                        speeds[i].last_milli_sec = milli_sec;
                        speeds[i].last_bytes = total_bytes;
                    }
                }
            }
        }
     
    } // namespace data
} // namespace ppbox
