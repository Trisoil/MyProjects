//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_NUMERIC_STATISTICS_H
#define SUPER_NODE_NUMERIC_STATISTICS_H

namespace super_node
{
    class NumericStatistics
        : public count_object_allocate<NumericStatistics>
    {
    public:
        NumericStatistics()
        {
            Reset();
        }

        void Reset()
        {
            sum_ = 0;
            count_ = 0;
            minimum_ = 0;
            maximum_ = 0;
        }

        void Add(size_t num)
        {
            sum_ += num;
            ++count_;

            if (count_ == 1)
            {
                minimum_ = num;
                maximum_ = num;
            }
            else
            {
                if (num < minimum_)
                {
                    minimum_ = num;
                }

                if (num > maximum_)
                {
                    maximum_ = num;
                }
            }
        }

        size_t Sum() const { return sum_; }

        size_t Average() const
        {
            if (count_ == 0)
            {
                return 0;
            }

            return sum_/count_;
        }

        size_t Minimum() const
        {
            return minimum_;
        }

        size_t Maximum() const
        {
            return maximum_;
        }

    private:
        size_t sum_;
        size_t count_;
        size_t minimum_;
        size_t maximum_;
    };
}

#endif //SUPER_NODE_NUMERIC_STATISTICS_H
