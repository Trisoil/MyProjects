//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_RESOURCE_STATE_H
#define SUPER_NODE_RESOURCE_STATE_H

#include "State.h"

namespace super_node
{
    class ResourceState
        : public count_object_allocate<ResourceState>
    {
    public:
        ResourceState()
            : cost_(0)
            , cache_block_num_(0)
            , loading_block_num_(0)
        {

        }

        ResourceState(size_t resource_cost_delim)
            : cost_(0)
            , cache_block_num_(0)
            , loading_block_num_(0)
            , resource_cost_delim_(resource_cost_delim)
        {

        }

        bool IsResourceMetadataTrivial() const;

        void SetCost(size_t cost) { cost_ = cost; }
        size_t GetCost() const { return cost_; }

        boost::uint32_t GetCacheBlockNum() { return cache_block_num_; }
        void SubtractCacheBlockNum(size_t block_num);
        void AddCacheBlockNum(size_t block_num);
        boost::uint32_t GetLoadingBlockNum() const { return loading_block_num_; }
        void AddLoadingBlockNum(size_t block_num);
        void SubtractLoadingBlockNum(boost::uint32_t block_num);

        void SetResourceCostDelim(size_t resource_cost_delim) { resource_cost_delim_ = resource_cost_delim; }

        void AllTick(boost::uint8_t decay1, boost::uint8_t decay2);
        void UpdateCost();

        void Statistic(boost::uint8_t statistic_type, size_t num = 1);

    private:
        State resource_state_;                  // 状态统计信息
        size_t cost_;                           // 根据历史推断的价值
        boost::uint32_t cache_block_num_;       // 缓存的块数
        boost::uint32_t loading_block_num_;     // 正在等待被加载的块数
        size_t resource_cost_delim_;            // 元数据信息有意义的最小价值
    };
}

#endif  // SUPER_NODE_RESOURCE_STATE_H