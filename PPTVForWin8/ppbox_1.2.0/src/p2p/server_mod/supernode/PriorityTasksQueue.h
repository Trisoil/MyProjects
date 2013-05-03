//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_PRIORITY_TASK_QUEUE_H
#define SUPER_NODE_PRIORITY_TASK_QUEUE_H

namespace super_node
{
    template <class Task>
    class PriorityTasksQueue
    {
    public:
        inline void Push(boost::shared_ptr<Task> task);

        inline boost::shared_ptr<Task> PopFront();
        inline boost::shared_ptr<Task> PopBack();
        inline boost::shared_ptr<Task> Back();
        
        size_t Size() const { return queue_.size(); }
        bool Empty() const { return queue_.empty(); }

    private:
        //priority as map key
        std::multimap<int, boost::shared_ptr<Task> > queue_;
    };

    template <class Task>
    void PriorityTasksQueue<Task>::Push(boost::shared_ptr<Task> task)
    {
        queue_.insert(std::make_pair(task->TaskPriority(), task));
    }

    template <class Task>
    boost::shared_ptr<Task> PriorityTasksQueue<Task>::PopFront()
    {
        boost::shared_ptr<Task> front;
        if (!Empty())
        {
            front = queue_.rbegin()->second;
            queue_.erase(--queue_.end());
        }

        return front;
    }

    template <class Task>
    boost::shared_ptr<Task> PriorityTasksQueue<Task>::PopBack()
    {
        boost::shared_ptr<Task> back;
        if (!Empty())
        {
            back = queue_.begin()->second;
            queue_.erase(queue_.begin());
        }

        return back;
    }

    template <class Task>
    boost::shared_ptr<Task> PriorityTasksQueue<Task>::Back()
    {
        boost::shared_ptr<Task> back;
        if (!Empty())
        {
            back = queue_.begin()->second;
        }

        return back;
    }
}

#endif //SUPER_NODE_PRIORITY_TASK_QUEUE_H