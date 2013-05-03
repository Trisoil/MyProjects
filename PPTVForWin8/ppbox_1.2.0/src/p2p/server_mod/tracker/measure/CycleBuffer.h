#ifndef CYCLE_BUFFER_H
#define CYCLE_BUFFER_H

namespace measure 
{
    enum CycleMode
    {
        CYCLE_NONE      = 0x0,
        CYCLE_MIN_VAL   = 0x1,
        CYCLE_MAX_VAL   = 0x2,
    };

    class CycleBuffer
        : public boost::noncopyable
    {
    public:
        typedef boost::shared_ptr< CycleBuffer > p;

        static p Create(boost::uint32_t capacity, boost::uint32_t mode = 0);

        void Clear();

        void Push(boost::uint32_t val);

        double Average() const;

        boost::uint32_t MaxValue() const;

        boost::uint32_t MinValue() const;

        boost::uint32_t Count() const;

    private:
        CycleBuffer(boost::uint32_t capacity, boost::uint32_t mode);

    private:
        typedef std::multiset< boost::uint32_t, std::less<boost::uint32_t> > MinHeap;
        typedef std::multiset< boost::uint32_t, std::greater<boost::uint32_t> > MaxHeap;
        // 
        std::vector<boost::uint32_t> data_;
        MinHeap data_min_;
        MaxHeap data_max_;
        boost::uint32_t count_;
        boost::uint32_t index_;
        double sum_;
        boost::uint32_t mode_;
    };
}

#endif//CYCLE_BUFFER_H
