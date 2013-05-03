//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "GlobalResourceState.h"

namespace super_node
{
    void GlobalResourceState::SubtractCacheBlockNum(size_t block_num)
    {
        assert(cache_block_num_ >= block_num);
        cache_block_num_ -= block_num;
    }

    void GlobalResourceState::AllTick(boost::uint8_t decay1, boost::uint8_t decay2)
    {
        state_.AllTick(decay1, decay2);
    }

    void GlobalResourceState::Statistic(boost::uint8_t statistic_type, size_t num)
    {
        state_.Add(statistic_type, num);
    }
}