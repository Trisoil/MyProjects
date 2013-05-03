//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_IO_STATISTICS_H
#define SUPER_NODE_IO_STATISTICS_H

namespace super_node
{
    class IOStatistics
        : public count_object_allocate<IOStatistics>
    {
    public:
        IOStatistics()
        {
            consecutive_failures_count_ = 0;
            Reset();
        }

        void Add(boost::uint32_t bytes, boost::uint32_t ticks_elapsed)
        {
            total_bytes_ += bytes;
            total_ticks_ += ticks_elapsed;
            ++success_count_;

            if (consecutive_failures_count_ > recent_max_consecutive_failures_count_)
            {
                recent_max_consecutive_failures_count_ = consecutive_failures_count_;
            }

            consecutive_failures_count_ = 0;
        }

        void AddFailure()
        {
            ++failure_count_;
            ++consecutive_failures_count_;
        }

        boost::uint32_t SuccessCount() const
        {
            return success_count_;
        }

        boost::uint32_t FailureRate() const
        {
            if (failure_count_ == 0)
            {
                return 0;
            }

            return 100*failure_count_/(failure_count_ + success_count_);
        }

        boost::uint32_t ConsecutiveFailureCount() const
        {
            if (recent_max_consecutive_failures_count_ > consecutive_failures_count_)
            {
                return recent_max_consecutive_failures_count_;
            }

            return consecutive_failures_count_;
        }

        //average speed in MB/s
        boost::uint32_t AverageSpeed() const
        {
            if (total_bytes_ == 0 || total_ticks_ == 0)
            {
                return 0;
            }

            return (total_bytes_/1024)*1000/1024/total_ticks_;
        }

        void Reset()
        {
            failure_count_ = 0;
            success_count_ = 0;
            total_bytes_ = 0;
            total_ticks_ = 0;
            recent_max_consecutive_failures_count_ = 0;
            //purposedly NOT to reset consecutive_failures_count_, 
            //which is only reset once a success disk I/O is committed.
        }

    private:
        boost::uint32_t success_count_;
        boost::uint32_t failure_count_;
        boost::uint32_t total_bytes_;
        boost::uint32_t total_ticks_;
        boost::uint32_t consecutive_failures_count_;
        boost::uint32_t recent_max_consecutive_failures_count_;
    };
}

#endif //SUPER_NODE_IO_STATISTICS_H