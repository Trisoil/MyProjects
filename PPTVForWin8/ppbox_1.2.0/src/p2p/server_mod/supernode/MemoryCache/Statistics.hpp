//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_STATISTICS_HPP
#define SUPER_NODE_STATISTICS_HPP

#include "Statistics.h"

namespace super_node
{
    template <typename Type>
    void Statistics<Type>::operator += (Type n)
    {
        current_ += n;
    }

    template <typename Type>
    void Statistics<Type>::Tick(boost::uint8_t decay1, boost::uint8_t decay2)
    {
        rate_ = current_ - last_;
        last_ = current_;
        smooth_rate1_ = (smooth_rate1_ * (100 - decay1) + rate_ * decay1) / 100;
        smooth_rate2_ = smooth_rate2_ * decay2 / 100 + rate_;
    }

    template <typename Type>
    void Statistics<Type>::Tick()
    {
        rate_ = current_ - last_;
        last_ = current_;
    }

    template <typename Type>
    Type Statistics<Type>::GetCurrent() const
    {
        return current_;
    }

    template <typename Type>
    Type Statistics<Type>::GetLast() const
    {
        return last_;
    }

    template <typename Type>
    Type Statistics<Type>::GetRate() const
    {
        return rate_;
    }

    template <typename Type>
    Type Statistics<Type>::GetSmoothRate1() const
    {
        return smooth_rate1_;
    }

    template <typename Type>
    Type Statistics<Type>::GetSmoothRate2() const
    {
        return smooth_rate2_;
    }
}

#endif  // SUPER_NODE_STATISTICS_HPP