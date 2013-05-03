//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "ResourceState.h"

namespace super_node
{
    bool ResourceState::IsResourceMetadataTrivial() const
    {
        return cost_ < resource_cost_delim_ && cache_block_num_ == 0 && loading_block_num_ == 0;
    }

    void ResourceState::AllTick(boost::uint8_t decay1, boost::uint8_t decay2)
    {
        resource_state_.AllTick(decay1, decay2);
    }

    void ResourceState::UpdateCost()
    {
        cost_ = resource_state_.GetVisit().GetSmoothRate2();
    }

    void ResourceState::SubtractLoadingBlockNum(boost::uint32_t block_num)
    {
        assert(loading_block_num_ >= block_num);

        loading_block_num_ -= block_num;
    }

    void ResourceState::AddLoadingBlockNum(size_t block_num)
    {
        loading_block_num_ += block_num;
    }

    void ResourceState::SubtractCacheBlockNum(size_t block_num)
    {
        assert(cache_block_num_ >= block_num);

        cache_block_num_ -= block_num;
    }

    void ResourceState::AddCacheBlockNum(size_t block_num)
    {
        cache_block_num_ += block_num;
    }

    void ResourceState::Statistic(boost::uint8_t statistic_type, size_t num)
    {
        resource_state_.Add(statistic_type, num);
    }
}