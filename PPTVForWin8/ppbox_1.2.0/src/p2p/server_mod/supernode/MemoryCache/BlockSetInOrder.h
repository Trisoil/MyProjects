//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_BLOCK_SET_H
#define SUPER_NODE_BLOCK_SET_H

#include "BlockCache.h"

namespace super_node
{
    class BlockCache;

    template <typename BlockCompareStruct>
    class BlockSetInOrder
        : public count_object_allocate<BlockSetInOrder<BlockCompareStruct> >
    {
    public:
        void Insert(boost::shared_ptr<BlockCache> block);
        void Erase(boost::shared_ptr<BlockCache> block);
        void ReArrangeTheSet();
        bool CheckOrder() const;
        void Clear();
        size_t Size() const;

    protected:
        std::set<boost::shared_ptr<BlockCache>, BlockCompareStruct> block_order_;
    };

    class BlockSetInExpireOrder : public BlockSetInOrder<BlockExpireCompare>
        , public count_object_allocate<BlockSetInExpireOrder>
    {
    public:
        void MoveDeadBlockToBlockSetInCostOrder(size_t expire_delim);
        size_t DropDeadBlockCacheInExpireOrder(size_t need_drop_num, time_t now, ReleaseBlockDataCallback callback);
        size_t DropAliveBlockCacheInExpireOrder(size_t need_drop_num, time_t now, ReleaseBlockDataCallback callback);
    };

    class BlockSetInCostOrder : public BlockSetInOrder<BlockCostCompare>
        , public count_object_allocate<BlockSetInCostOrder>
    {
    public:
        size_t DropBlockCacheInCostOrder(size_t need_drop_num, ReleaseBlockDataCallback callback);
    };
}

#include "BlockSetInOrder.hpp"

#endif  // SUPER_NODE_BLOCK_SET_H
