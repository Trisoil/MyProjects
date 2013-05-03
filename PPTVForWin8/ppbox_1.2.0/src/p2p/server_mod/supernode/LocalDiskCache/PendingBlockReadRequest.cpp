//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "PendingBlockReadRequest.h"

namespace super_node
{
    void PendingBlockReadRequest::ProcessRequest(ErrorCode error_code, const std::map<size_t, boost::shared_ptr<BlockData> >& blocks)
    {
        boost::shared_ptr<BlockData> block_data;
        if (error_code == ErrorCodes::Success)
        {
            std::map<size_t, boost::shared_ptr<BlockData> >::const_iterator iter = blocks.find(block_index_);
            if (iter != blocks.end())
            {
                block_data = iter->second;
            }

            if (!block_data)
            {
                error_code = ErrorCodes::ResourceNotFound;
            }
        }

        assert(callback_);
        if (callback_)
        {
            callback_(error_code, block_data);
        }
    }
}