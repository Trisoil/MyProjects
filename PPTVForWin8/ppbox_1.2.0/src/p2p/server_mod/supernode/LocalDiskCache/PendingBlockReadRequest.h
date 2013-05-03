//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_PENDING_BLOCK_READ_REQUEST_H
#define SUPER_NODE_PENDING_BLOCK_READ_REQUEST_H

namespace super_node
{
    class BlockData;

    typedef boost::function<
        void (ErrorCode, boost::shared_ptr<BlockData>)
    > ReadResourceBlockCallback;

    class PendingBlockReadRequest
        : public count_object_allocate<PendingBlockReadRequest>
    {
    public:
        PendingBlockReadRequest()
            : block_index_(0)
        {}

        PendingBlockReadRequest(size_t block_index, ReadResourceBlockCallback callback)
            : block_index_(block_index), callback_(callback)
        {
        }

        void ProcessRequest(ErrorCode error_code, const std::map<size_t, boost::shared_ptr<BlockData> >& blocks);

        size_t GetTargetBlockIndex() const
        {
            return block_index_;
        }

    private:
        size_t block_index_;
        ReadResourceBlockCallback callback_;
    };
}

#endif //SUPER_NODE_PENDING_BLOCK_READ_REQUEST_H