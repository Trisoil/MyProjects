#include "Common.h"
//#include "Common.h"
#include "measure/CycleBuffer.h"

namespace measure
{
    CycleBuffer::p CycleBuffer::Create(boost::uint32_t capacity, boost::uint32_t mode)
    {
        return CycleBuffer::p(new CycleBuffer(capacity, mode));
    }

    void CycleBuffer::Clear()
    {
        index_ = 0;
        count_ = 0;
        sum_ = 0;
        data_min_.clear();
        data_max_.clear();
    }

    void CycleBuffer::Push(boost::uint32_t val)
    {

        if (count_ == data_.size()) {
            boost::uint32_t v = data_[index_];
            sum_ -= v;
            // remove
            if (mode_ & CYCLE_MIN_VAL) {
                data_min_.erase(data_min_.find(v));
            }
            if (mode_ & CYCLE_MAX_VAL) {
                data_max_.erase(data_max_.find(v));
            }
        }

        data_[index_++] = val;
        sum_ += val;
        if (index_ == data_.size()) {
            index_ = 0;
        }
        if (count_ < data_.size()) {
            ++count_;
        }

        // 
        if (mode_ & CYCLE_MIN_VAL) {
            data_min_.insert(val);
        }
        if (mode_ & CYCLE_MAX_VAL) {
            data_max_.insert(val);
        }

    }

    double CycleBuffer::Average() const
    {
        if (0 == count_) {
            return 0.0;
        }
        return sum_ / count_;
    }

    boost::uint32_t CycleBuffer::MaxValue() const
    {
        if (!data_max_.empty()) {
            return *data_max_.begin();
        }
        return (boost::uint32_t)-1;
    }

    boost::uint32_t CycleBuffer::MinValue() const
    {
        if (!data_min_.empty()) {
            return *data_min_.begin();
        }
        return (boost::uint32_t)-1;
    }

    boost::uint32_t CycleBuffer::Count() const
    {
        return count_;
    }

    CycleBuffer::CycleBuffer(boost::uint32_t capacity, boost::uint32_t mode)
    {
        index_ = 0;
        count_ = 0;
        sum_ = 0;
        mode_ = mode;
        data_.resize(capacity);
    }
}
