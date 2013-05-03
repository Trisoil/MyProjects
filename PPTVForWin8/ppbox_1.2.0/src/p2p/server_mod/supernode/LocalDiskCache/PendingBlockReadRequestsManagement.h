//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_PENDING_BLOCK_READ_REQUESTS_MANAGEMENT_H
#define SUPER_NODE_PENDING_BLOCK_READ_REQUESTS_MANAGEMENT_H

#include "ResourcePendingReads.h"

namespace super_node
{
    class PendingBlockReadRequestsManagement
    {
        typedef std::map<ResourceIdentifier, ResourcePendingReads> PendingBlockReadRequestsMap;
    public:
        void CancelAll();
        void ProcessPendingReadRequests(const ResourceIdentifier& resource_identifier, ErrorCode error_code, const std::map<size_t, boost::shared_ptr<BlockData> >& blocks, size_t start_block_index, size_t num_of_blocks);
        void AddBlockReadRequest(const ResourceIdentifier& resource_identifier, ReadResourceBlockCallback callback, size_t block_index, size_t& num_of_consecutive_missing_blocks);
    private:
        PendingBlockReadRequestsMap pending_block_read_requests_;
    };
}

#endif //SUPER_NODE_PENDING_BLOCK_READ_REQUESTS_MANAGEMENT_H