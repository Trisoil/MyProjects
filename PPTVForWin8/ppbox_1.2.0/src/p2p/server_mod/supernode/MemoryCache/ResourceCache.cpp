//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "ResourceCache.h"
#include "BlockCache.h"

namespace super_node
{
    ResourceCache::ResourceCache(size_t resource_cost_delim) : resource_cost_delim_(resource_cost_delim)
        , resource_state_(ResourceState(resource_cost_delim))
    {

    }
    void ResourceCache::Clear()
    {
        blocks_.clear();
        block_set_in_cost_order_.Clear();
    }

    void ResourceCache::DropBlocks()
    {
        for (size_t i = 0; i < blocks_.size(); ++i)
        {
            if (blocks_[i])
            {
                assert(!(blocks_[i]->GetBlockData()));
            }
        }

        blocks_.clear();
    }

    void ResourceCache::AllTick(boost::uint8_t resource_decay1, boost::uint8_t resource_decay2, boost::uint8_t block_decay1,
        boost::uint8_t block_decay2)
    {
        resource_state_.AllTick(resource_decay1, resource_decay2);
        for (size_t i = 0; i < blocks_.size(); ++i)
        {
            if (blocks_[i])
            {
                blocks_[i]->AllTick(block_decay1, block_decay2);
            }
        }
    }

    void ResourceCache::UpdateCost()
    {
        resource_state_.UpdateCost();
        for (size_t i = 0; i < blocks_.size(); ++i)
        {
            if (blocks_[i])
            {
                blocks_[i]->UpdateCost();
            }
        }
    }

    void ResourceCache::EraseBlockCacheFromSetInCostOrder(boost::uint32_t block_id)
    {
        assert(blocks_.size() > block_id);

        block_set_in_cost_order_.Erase(blocks_[block_id]);
    }

    void ResourceCache::InsertBlockCacheToSetInCostOrder(boost::shared_ptr<BlockCache> block)
    {
        block_set_in_cost_order_.Insert(block);
    }

    boost::shared_ptr<BlockCache> ResourceCache::GetBlock(boost::uint32_t block_id) const
    {
        boost::shared_ptr<BlockCache> block;

        if(blocks_.size() > block_id)
        {
            block = blocks_[block_id];
        }

        return block;
    }

    void ResourceCache::SetBlock(boost::shared_ptr<BlockCache> block)
    {
        assert(block);

        if(blocks_.size() <= block->GetBlockID())
        {
            blocks_.resize(block->GetBlockID() + 1);
        }

        blocks_[block->GetBlockID()] = block;
    }

    size_t ResourceCache::DropBlockCacheInCostOrder(size_t need_drop_num, ReleaseBlockDataCallback callback)
    {
        return block_set_in_cost_order_.DropBlockCacheInCostOrder(need_drop_num, callback);
    }

    void ResourceCache::ReArrangeTheBlockSet()
    {
        block_set_in_cost_order_.ReArrangeTheSet();
    }

    size_t ResourceCache::GetCost() const
    {
        return resource_state_.GetCost();
    }

    bool ResourceCache::IsResourceMetadataTrivial() const
    {
        return resource_state_.IsResourceMetadataTrivial();
    }

    void ResourceCache::SubtractCacheBlockNum(size_t block_num)
    {
        resource_state_.SubtractCacheBlockNum(block_num);
    }

    void ResourceCache::SubtractLoadingBlockNum(size_t block_num)
    {
        resource_state_.SubtractLoadingBlockNum(block_num);
    }

    void ResourceCache::AddCacheBlockNum(size_t block_num)
    {
        resource_state_.AddCacheBlockNum(block_num);
    }

    void ResourceCache::AddLoadingBlockNum(boost::uint32_t block_num)
    {
        resource_state_.AddLoadingBlockNum(block_num);
    }

    size_t ResourceCache::GetBlocksSize() const
    {
        return blocks_.size();
    }

    boost::uint32_t ResourceCache::GetLoadingBlockNum() const
    {
        return resource_state_.GetLoadingBlockNum();
    }

    size_t ResourceCache::GetBlockSetSize() const
    {
        return block_set_in_cost_order_.Size();
    }

    void ResourceCache::SetCost(size_t cost)
    {
        resource_state_.SetCost(cost);
    }

    void ResourceCache::Statistic(boost::uint8_t statistic_type, size_t num)
    {
        resource_state_.Statistic(statistic_type, num);
    }
}