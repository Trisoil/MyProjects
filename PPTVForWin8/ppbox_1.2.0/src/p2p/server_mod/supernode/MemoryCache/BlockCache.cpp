//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "BlockCache.h"
#include "ResourceCache.h"
#include "BlockData.h"

namespace super_node
{
    BlockCache::BlockCache(boost::shared_ptr<ResourceCache> my_resource, boost::uint32_t block_id, size_t block_cost_delim)
        : my_resource_(my_resource), block_id_(block_id), block_state_(BlockState(block_cost_delim))
    {

    }

    void BlockCache::ReleaseBlockData(ReleaseBlockDataCallback callback)
    {
        assert(data_);

        my_resource_->SubtractCacheBlockNum(1);

        // 释放数据块内存
        data_.reset();
        callback(block_data_lifetime_.elapse());
    }

    void BlockCache::AllTick(boost::uint8_t decay1, boost::uint8_t decay2)
    {
        block_state_.AllTick(decay1, decay2);
    }

    void BlockCache::UpdateCost()
    {
        block_state_.UpdateCost();
    }

    bool BlockCache::IsInTheExpireQueue(time_t expire_delim) const
    {
        return block_state_.IsInTheExpireQueue(expire_delim);
    }

    void BlockCache::EraseFromTheSetInCostOrder()
    {
        my_resource_->EraseBlockCacheFromSetInCostOrder(block_id_);
    }

    void BlockCache::PushBackToTheDelayReadCallbackDeque(DelayReadCallback callback)
    {
        delay_read_callback_deque_.PushBack(callback);
    }

    void BlockCache::SetBlockData(boost::shared_ptr<BlockData> data)
    {
        data_ = data;
        block_data_lifetime_.reset();
    }

    boost::shared_ptr<BlockData> BlockCache::GetBlockData() const
    {
        return data_;
    }

    boost::shared_ptr<ResourceCache> BlockCache::GetMyResource() const
    {
        return my_resource_;
    }

    size_t BlockCache::GetDelayReadCallbackDequeSize() const
    {
        return delay_read_callback_deque_.Size();
    }

    boost::uint32_t BlockCache::GetBlockID() const
    {
        return block_id_;
    }

    boost::uint32_t BlockCache::GetLoadingBlockNum() const
    {
        return my_resource_->GetLoadingBlockNum();
    }

    void BlockCache::SubtractLoadingBlockNum(boost::uint32_t block_num)
    {
        my_resource_->SubtractLoadingBlockNum(block_num);
    }

    void BlockCache::SetExpire(time_t expire)
    {
        block_state_.SetExpire(expire);
    }

    time_t BlockCache::GetExpire() const
    {
        return block_state_.GetExpire();
    }

    size_t BlockCache::GetCost() const
    {
        return block_state_.GetCost();
    }

    void BlockCache::CallBackDelayRead(ErrorCode error_code, boost::shared_ptr<BlockData> data)
    {
        delay_read_callback_deque_.Callback(error_code, data);
    }

    bool BlockCache::IsBlockDead(time_t now) const
    {
        return block_state_.IsBlockDead(now);
    }

    void BlockCache::SetCost(size_t cost)
    {
        block_state_.SetCost(cost);
    }

    void BlockCache::Statistic(boost::uint8_t statistic_type, size_t num)
    {
        block_state_.Statistic(statistic_type, num);
    }
}