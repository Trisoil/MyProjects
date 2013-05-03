//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_STATISTICS_H
#define SUPER_NODE_STATISTICS_H

namespace super_node
{
    template <typename Type>
    class Statistics
        : public count_object_allocate<Statistics<Type> >
    {
    public:
        Statistics() : current_(0), last_(0), rate_(0), smooth_rate1_(0), smooth_rate2_(0){}
        void operator += (Type n);
        void Tick(boost::uint8_t decay1, boost::uint8_t decay2);
        void Tick();

        Type GetCurrent() const;
        Type GetLast() const;
        Type GetRate() const;
        Type GetSmoothRate1() const;
        Type GetSmoothRate2() const;

    private:
        Type current_;
        Type last_;
        Type rate_;
        Type smooth_rate1_;
        Type smooth_rate2_;
    };
}

#endif  // SUPER_NODE_STATISTICS_H