//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _CONSTRAINT_H_
#define _CONSTRAINT_H_

namespace live_media
{
    class Constraint
    {
    private:
        size_t hard_limit_;
        size_t current_limit_;
        size_t adjustment_ratio_;

    private:
        static bool Unlimited(size_t limit)
        {
            return limit == 0;
        }

    public:
        Constraint()
        {
            hard_limit_ = 0;
            current_limit_ = 0;
            adjustment_ratio_ = 5;
        }

        int GetCurrentLimit() const
        {
            return current_limit_;
        }

        void SetHardLimit(size_t hard_limit)
        {
            if (hard_limit == hard_limit_)
            {
                return;
            }

            hard_limit_ = hard_limit;

            if (Unlimited(hard_limit_))
            {
                current_limit_ = 0;
                return;
            }

            if (current_limit_ > hard_limit_ || Unlimited(current_limit_))
            {
                current_limit_ = hard_limit_;
            }
        }

        bool ReachesLimit(size_t current_value) const
        {
            if (Unlimited(current_limit_))
            {
                return false;
            }

            return current_value >= static_cast<size_t>(current_limit_);
        }

        void IncreaseLimit(size_t current_value)
        {
            if (Unlimited(current_limit_))
            {
                return;
            }

            if (static_cast<size_t>(current_limit_)*100 <= current_value*(100+adjustment_ratio_))
            {
                current_limit_ = current_limit_*(100+adjustment_ratio_)/100;
            }

            if (!Unlimited(hard_limit_) && current_limit_ > hard_limit_)
            {
                current_limit_ = hard_limit_;
            }
        }

        void DecreaseLimit(size_t current_value)
        {
            if (Unlimited(current_limit_))
            {
                current_limit_ = current_value;
                return;
            }

            if (current_limit_*(100-adjustment_ratio_) >= current_value*100)
            {
                current_limit_ = current_limit_*(100-adjustment_ratio_)/100;
            }
        }
    };
}

#endif //_CONSTRAINT_H_