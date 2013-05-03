//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_BLOCK_CACHE_H
#define SUPER_NODE_BLOCK_CACHE_H

#include "DelayReadCallbackDeque.h"
#include "BlockState.h"

namespace super_node
{
    class BlockData;
    class ResourceCache;

    typedef boost::function<
        void (boost::uint32_t lifetime)
    > ReleaseBlockDataCallback;

    class BlockCache
        : public count_object_allocate<BlockCache>
    {
    public:
        BlockCache(boost::shared_ptr<ResourceCache> my_resource, boost::uint32_t block_id, size_t block_cost_delim);
        void SetBlockData(boost::shared_ptr<BlockData> data);
        boost::shared_ptr<BlockData> GetBlockData() const;
        void ReleaseBlockData(ReleaseBlockDataCallback callback);
        boost::shared_ptr<ResourceCache> GetMyResource() const;
        void AllTick(boost::uint8_t decay1, boost::uint8_t decay2);
        void UpdateCost();
        bool IsInTheExpireQueue(time_t expire_delim) const;
        void EraseFromTheSetInCostOrder();
        void CallBackDelayRead(ErrorCode error_code, boost::shared_ptr<BlockData> data);
        void PushBackToTheDelayReadCallbackDeque(DelayReadCallback callback);
        size_t GetDelayReadCallbackDequeSize() const;
        boost::uint32_t GetBlockID() const;
        boost::uint32_t GetLoadingBlockNum() const;
        void SubtractLoadingBlockNum(boost::uint32_t block_num);
        void SetExpire(time_t expire);
        time_t GetExpire() const;
        size_t GetCost() const;
        void SetCost(size_t cost);
        bool IsBlockDead(time_t now) const;
        void Statistic(boost::uint8_t statistic_type, size_t num = 1);

    private:
        boost::uint32_t block_id_;
        DelayReadCallbackDeque delay_read_callback_deque_;
        boost::shared_ptr<ResourceCache> my_resource_;
        boost::shared_ptr<BlockData> data_;
        BlockState block_state_;
        framework::timer::TimeCounter block_data_lifetime_;
    };

    struct BlockExpireCompare
        : public count_object_allocate<BlockExpireCompare>
    {
        bool operator () (
            boost::shared_ptr<BlockCache> const & lhs,
            boost::shared_ptr<BlockCache> const & rhs) const
        {
            return ((lhs->GetExpire() < rhs->GetExpire())
                || (lhs->GetExpire() == rhs->GetExpire()
                && lhs < rhs));
        }
    };

    struct BlockCostCompare
        : public count_object_allocate<BlockCostCompare>
    {
        bool operator () (
            boost::shared_ptr<BlockCache> const & lhs,
            boost::shared_ptr<BlockCache> const & rhs) const
        {
            return ((lhs->GetCost() < rhs->GetCost())
                || (lhs->GetCost() == rhs->GetCost()
                && lhs < rhs));
        }
    };
}

#endif  // SUPER_NODE_BLOCK_CACHE_H