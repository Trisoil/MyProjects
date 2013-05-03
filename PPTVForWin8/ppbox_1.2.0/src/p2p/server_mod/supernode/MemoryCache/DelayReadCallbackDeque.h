//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_DELAY_READ_CALLBACK_DEQUE_H
#define SUPER_NODE_DELAY_READ_CALLBACK_DEQUE_H

#include <deque>

namespace super_node
{
    class ErrorCode;
    class BlockData;

    typedef boost::function<
        void (ErrorCode, boost::shared_ptr<BlockData> data)
    > DelayReadCallback;

    class DelayReadCallbackDeque
        : public count_object_allocate<DelayReadCallbackDeque>
    {
    public:
        void PushBack(DelayReadCallback delay_read_callback);
        bool IsEmpty() const;
        size_t Size() const { return delay_read_callback_deque_.size(); }
        void Callback(ErrorCode error_code, boost::shared_ptr<BlockData> data);

    private:
        std::deque<DelayReadCallback> delay_read_callback_deque_;
    };
}

#endif  // SUPER_NODE_DELAY_READ_CALLBACK_DEQUE_H