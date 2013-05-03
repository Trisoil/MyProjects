// Statistics.h

#ifndef _STATISTICS_H_
#define _STATISTICS_H_

struct Statistics
{
    size_t cur;
    size_t last;
    size_t rate;
    size_t smooth_rate;

    Statistics()
        : cur(0)
        , last(0)
        , rate(0)
    {
    }

    void operator ++ ()
    {
        cur++;
    }

    void operator ++ (int)
    {
        cur++;
    }

    void operator += (size_t n)
    {
        cur += n;
    }

    void tick(
        size_t decay)
    {
        rate = cur - last;
        last = cur;
        smooth_rate = (smooth_rate * (100 - decay) + rate * decay) / 100;
    }

    void tick()
    {
        rate = cur - last;
        last = cur;
    }
};

#endif // #ifndef _STATISTICS_H_

