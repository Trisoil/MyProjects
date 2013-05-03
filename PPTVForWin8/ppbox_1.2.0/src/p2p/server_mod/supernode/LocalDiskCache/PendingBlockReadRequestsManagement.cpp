//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "PendingBlockReadRequestsManagement.h"

namespace super_node
{
    void PendingBlockReadRequestsManagement::CancelAll()
    {
        for(PendingBlockReadRequestsMap::iterator iter = pending_block_read_requests_.begin();
            iter != pending_block_read_requests_.end();
            ++iter)
        {
            const ResourceIdentifier& resource_identifier = iter->first;
            ResourcePendingReads& resource_pending_reads = iter->second;
            resource_pending_reads.CancelAll();
        }

        pending_block_read_requests_.clear();
    }

    void PendingBlockReadRequestsManagement::ProcessPendingReadRequests(const ResourceIdentifier& resource_identifier, ErrorCode error_code, const std::map<size_t, boost::shared_ptr<BlockData> >& blocks, size_t start_block_index, size_t num_of_blocks)
    {
        PendingBlockReadRequestsMap::iterator iter = pending_block_read_requests_.find(resource_identifier);

        assert(iter != pending_block_read_requests_.end());

        if (iter != pending_block_read_requests_.end())
        {
            ResourcePendingReads& resource_pending_reads = iter->second;

            for(size_t block_index = start_block_index; block_index < start_block_index + num_of_blocks; ++block_index)
            {
                std::vector<PendingBlockReadRequest> pending_reads_on_block;
                resource_pending_reads.RemovePendingReadRequests(block_index, pending_reads_on_block);

                for(size_t i = 0; i < pending_reads_on_block.size(); ++i)
                {
                    pending_reads_on_block[i].ProcessRequest(error_code, blocks);
                }
            }

            if (!resource_pending_reads.HasPendingReadRequest())
            {
                pending_block_read_requests_.erase(iter);
            }
        }
    }

    void PendingBlockReadRequestsManagement::AddBlockReadRequest(const ResourceIdentifier& resource_identifier, ReadResourceBlockCallback callback, size_t block_index, size_t& num_of_consecutive_missing_blocks)
    {
        if (pending_block_read_requests_.find(resource_identifier) == pending_block_read_requests_.end())
        {
            pending_block_read_requests_[resource_identifier] = ResourcePendingReads();
        }

        ResourcePendingReads& resource_pending_reads = pending_block_read_requests_[resource_identifier];

        for(size_t i = block_index; i < block_index + num_of_consecutive_missing_blocks; ++i)
        {
            if (resource_pending_reads.IsBlockRequested(i))
            {
                num_of_consecutive_missing_blocks = i - block_index;
                break;
            }

            resource_pending_reads.AddBlockRequest(i);
        }

        resource_pending_reads.AddBlockRequest(block_index, callback);
    }
}
