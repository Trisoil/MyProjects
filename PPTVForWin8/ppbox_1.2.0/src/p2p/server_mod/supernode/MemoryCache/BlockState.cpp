//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "BlockState.h"

namespace super_node
{
    BlockState::BlockState(size_t block_cost_delim) : expire_(0), cost_(0), block_cost_delim_(block_cost_delim)
    {

    }

    bool BlockState::IsBlockDataLoading() const
    {
        return expire_ == 0;
    }

    bool BlockState::IsBlockMetadataTrivial() const
    {
        return cost_ < block_cost_delim_ && !IsBlockDataLoading();
    }

    void BlockState::AllTick(boost::uint8_t decay1, boost::uint8_t decay2)
    {
        block_state_.AllTick(decay1,decay2);
    }

    void BlockState::UpdateCost()
    {
        cost_ = block_state_.GetVisit().GetSmoothRate2();
    }

    bool BlockState::IsInTheExpireQueue(time_t expire_delim) const
    {
        return expire_ >= expire_delim;
    }

    void BlockState::Statistic(boost::uint8_t statistic_type, size_t num)
    {
        block_state_.Add(statistic_type, num);
    }
}