//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_BLOCK_STATE_H
#define SUPER_NODE_BLOCK_STATE_H

#include "State.h"

namespace super_node
{
    class BlockState
        : public count_object_allocate<BlockState>
    {
    public:
        BlockState(size_t block_cost_delim);

        bool IsBlockDataLoading() const;
        bool IsBlockMetadataTrivial() const;

        void SetExpire(time_t expire) { expire_ = expire; }
        void SetCost(size_t cost) { cost_ = cost; }

        time_t GetExpire() const { return expire_; }
        size_t GetCost() const { return cost_; }

        bool IsBlockDead(time_t now) const { return expire_ < now; }

        void AllTick(boost::uint8_t decay1, boost::uint8_t decay2);
        void UpdateCost();

        bool IsInTheExpireQueue(time_t expire_delim) const;

        void Statistic(boost::uint8_t statistic_type, size_t num = 1);

        void SetCostDelim(size_t block_cost_delim) { block_cost_delim_ = block_cost_delim; }

    private:
        State block_state_;         // 状态统计信息
        time_t expire_;             // 生命期
        size_t cost_;               // 根据历史推断的价值
        size_t block_cost_delim_;   // 元数据信息有意义的最小价值
    };
}
#endif  // SUPER_NODE_BLOCK_STATE_H