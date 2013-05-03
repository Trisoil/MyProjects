//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_RESOURCE_CACHE_H
#define SUPER_NODE_RESOURCE_CACHE_H

#include "BlockSetInOrder.h"
#include "ResourceState.h"

namespace super_node
{
    class BlockCache;

    class ResourceCache
        : public count_object_allocate<ResourceCache>
    {
    public:
        ResourceCache(size_t resource_cost_delim);
        void Clear();
        void AllTick(boost::uint8_t resource_decay1, boost::uint8_t resource_decay2, boost::uint8_t block_decay1,
            boost::uint8_t block_decay2);
        void UpdateCost();
        void EraseBlockCacheFromSetInCostOrder(boost::uint32_t block_id);
        void InsertBlockCacheToSetInCostOrder(boost::shared_ptr<BlockCache> block);
        boost::shared_ptr<BlockCache> GetBlock(boost::uint32_t block_id) const;
        void SetBlock(boost::shared_ptr<BlockCache> block);
        boost::uint32_t GetLoadingBlockNum() const;
        void AddLoadingBlockNum(boost::uint32_t block_num);
        void SubtractLoadingBlockNum(size_t block_num);
        size_t DropBlockCacheInCostOrder(size_t need_drop_num, ReleaseBlockDataCallback callback);
        void ReArrangeTheBlockSet();
        size_t GetCost() const;
        bool IsResourceMetadataTrivial() const;
        void AddCacheBlockNum(size_t block_num);
        void SubtractCacheBlockNum(size_t block_num);
        void DropBlocks();
        size_t GetBlocksSize() const;
        size_t GetBlockSetSize() const;
        void SetCost(size_t cost);

        void Statistic(boost::uint8_t statistic_type, size_t num = 1);

    private:
        std::vector<boost::shared_ptr<BlockCache> > blocks_;
        ResourceState resource_state_;
        BlockSetInCostOrder block_set_in_cost_order_;
        size_t resource_cost_delim_;
    };
}

#endif  // SUPER_NODE_RESOURCE_CACHE_H