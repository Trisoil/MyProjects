//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "ResourcePendingReads.h"

namespace super_node
{
    void ResourcePendingReads::RemovePendingReadRequests(size_t block_index, std::vector<PendingBlockReadRequest>& pending_read_requests)
    {
        assert(blocks_pending_read_.size() > block_index);
        blocks_pending_read_.reset(block_index);

        for(std::list<PendingBlockReadRequest>::iterator iter = pending_reads_.begin();
            iter != pending_reads_.end();
            )
        {
            if (iter->GetTargetBlockIndex() == block_index)
            {
                pending_read_requests.push_back(*iter);
                pending_reads_.erase(iter++);
            }
            else
            {
                ++iter;
            }
        }
    }

    void ResourcePendingReads::CancelAll()
    {
    //    pending_reads_.clear();
        if (pending_reads_.empty())
        {
            return;
        }

        std::map<size_t, boost::shared_ptr<BlockData> > empty_blocks;
        for(std::list<PendingBlockReadRequest>::iterator pending_request = pending_reads_.begin();
            pending_request != pending_reads_.end();
            ++pending_request)
        {
            pending_request->ProcessRequest(ErrorCodes::ServiceStopped, empty_blocks);
        }
    }
}
