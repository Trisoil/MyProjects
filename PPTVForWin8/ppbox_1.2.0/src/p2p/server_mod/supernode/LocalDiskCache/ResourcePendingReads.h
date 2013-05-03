//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_RESOURCE_PENDING_READS_H
#define SUPER_NODE_RESOURCE_PENDING_READS_H

#include "PendingBlockReadRequest.h"
#include "LocalDiskCache/DiskResource.h"

namespace super_node
{
    class ResourcePendingReads
    {
    public:
        bool IsBlockRequested(size_t block_index) const
        {
            return blocks_pending_read_.size() > block_index && blocks_pending_read_[block_index];
        }

        void AddBlockRequest(size_t block_index)
        {
            if (blocks_pending_read_.size() <= block_index)
            {
                blocks_pending_read_.resize(block_index + 1);
            }

            blocks_pending_read_.set(block_index);
        }

        void AddBlockRequest(size_t block_index, ReadResourceBlockCallback callback)
        {
            AddBlockRequest(block_index);
            pending_reads_.push_back(PendingBlockReadRequest(block_index, callback));
        }

        bool HasPendingReadRequest() const
        {
            return !pending_reads_.empty();
        }

        void RemovePendingReadRequests(size_t block_index, std::vector<PendingBlockReadRequest>& pending_read_requests);
        void CancelAll();
    private:
        std::list<PendingBlockReadRequest> pending_reads_;
        BlocksMap blocks_pending_read_;
    };
}

#endif //SUPER_NODE_RESOURCE_PENDING_READS_H
