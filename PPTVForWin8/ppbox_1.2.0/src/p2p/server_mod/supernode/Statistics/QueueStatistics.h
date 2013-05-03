//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_QUEUE_STATISTICS_H
#define SUPER_NODE_QUEUE_STATISTICS_H

#include "NumericStatistics.h"

namespace super_node
{
    class QueueStatistics
        : public count_object_allocate<QueueStatistics>
    {
    public:
        QueueStatistics()
        {
            Reset();
        }

        void Add(boost::uint32_t queue_length)
        {
            queue_length_.Add(queue_length);
        }

        size_t AverageQueueSize() const
        {
            return queue_length_.Average();
        }

        size_t MaxQueueLength() const
        {
            return queue_length_.Maximum();
        }

        boost::uint32_t DropRate() const
        {
            if (dropped_tasks_ == 0)
            {
                return 0;
            }

            return dropped_tasks_ < enqueued_tasks_ ? (100*dropped_tasks_/enqueued_tasks_) : 100;
        }

        void Enqueue()
        {
            ++enqueued_tasks_;
        }

        void Drop()
        {
            ++dropped_tasks_;
        }

        void Reset()
        {
            queue_length_.Reset();
            dropped_tasks_ = 0;
            enqueued_tasks_ = 0;
        }
    private:
        NumericStatistics queue_length_;
        boost::uint32_t dropped_tasks_;
        boost::uint32_t enqueued_tasks_;
    };
}

#endif //SUPER_NODE_QUEUE_STATISTICS_H