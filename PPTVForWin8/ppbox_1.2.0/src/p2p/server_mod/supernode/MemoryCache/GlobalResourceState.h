//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_GLOBAL_RESOURCE_STATE_H
#define SUPER_NODE_GLOBAL_RESOURCE_STATE_H

#include "State.h"

namespace super_node
{
    class GlobalResourceState
    {
    public:
        GlobalResourceState() : cache_block_num_(0) {}

        boost::uint32_t GetCacheBlockNum() { return cache_block_num_; }
        void SubtractCacheBlockNum(size_t block_num);
        void AddCacheBlockNum(size_t block_num) { cache_block_num_ += block_num; }

        void AllTick(boost::uint8_t decay1, boost::uint8_t decay2);
        void Statistic(boost::uint8_t statistic_type, size_t num = 1);

        const Statistics<size_t>& GetVisit() const
        {
            return state_.GetVisit();
        }

        const Statistics<size_t>& GetHit() const
        {
            return state_.GetHit();
        }

    private:
        State state_;                           // 状态统计信息
        boost::uint32_t cache_block_num_;       // 缓存的块数
    };
}

#endif  // SUPER_NODE_GLOBAL_RESOURCE_STATE_H