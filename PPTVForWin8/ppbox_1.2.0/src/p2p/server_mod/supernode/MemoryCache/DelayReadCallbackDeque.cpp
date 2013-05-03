//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "DelayReadCallbackDeque.h"

namespace super_node
{
    void DelayReadCallbackDeque::PushBack(DelayReadCallback delay_read_callback)
    {
        delay_read_callback_deque_.push_back(delay_read_callback);
    }

    bool DelayReadCallbackDeque::IsEmpty() const
    {
        return delay_read_callback_deque_.empty();
    }

    void DelayReadCallbackDeque::Callback(ErrorCode error_code, boost::shared_ptr<BlockData> data)
    {
        while(!IsEmpty())
        {
            DelayReadCallback callback = delay_read_callback_deque_.front();
            callback(error_code, data);
            delay_read_callback_deque_.pop_front();
        }
    }
}