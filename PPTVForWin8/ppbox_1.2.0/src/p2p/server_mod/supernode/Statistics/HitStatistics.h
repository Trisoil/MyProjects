//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_HIT_STATISTICS_H
#define SUPER_NODE_HIT_STATISTICS_H

namespace super_node
{
    class HitStatistics
        : public count_object_allocate<HitStatistics>
    {
    public:
        HitStatistics()
        {
            Reset();
        }

        void Hit()
        {
            ++hit_count_;
        }

        void Missed()
        {
            ++missed_count_;
        }

        boost::uint32_t HitRate() const
        {
            if (hit_count_ == 0)
            {
                return 0;
            }

            return hit_count_*100/(hit_count_ + missed_count_);
        }

        boost::uint32_t HitCount() const
        {
            return hit_count_;
        }

        void Reset()
        {
            hit_count_ = 0;
            missed_count_ = 0;
        }

    private:
        boost::uint32_t hit_count_;
        boost::uint32_t missed_count_;
    };
}

#endif //SUPER_NODE_HIT_STATISTICS_H