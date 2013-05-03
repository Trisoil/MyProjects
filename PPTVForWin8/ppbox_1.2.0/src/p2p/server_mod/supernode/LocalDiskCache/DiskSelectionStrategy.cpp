//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "DiskSelectionStrategy.h"
#include "LocalDisk.h"

namespace super_node
{
    DiskSelectionStrategy::DiskSelectionStrategy(const std::vector<boost::shared_ptr<ILocalDisk> >& disks)
    {
        disks_candidate_score_.reserve(disks.size());
        for(size_t i = 0; i < disks.size(); ++i)
        {
            disks_candidate_score_.push_back(disks[i]->GetFreeSpacePercentage());
        }

        score_sum_ = 0;
        for(size_t i = 0; i < disks_candidate_score_.size(); ++i)
        {
            score_sum_ += disks_candidate_score_[i];
        }
    }

    int DiskSelectionStrategy::SelectDisk() const
    {
        if (score_sum_ > 0)
        {
            size_t number = static_cast<size_t>(rand()%score_sum_);
            for(size_t i = 0; i < disks_candidate_score_.size(); ++i)
            {
                if (number < disks_candidate_score_[i])
                {
                    return i;
                }

                number -= disks_candidate_score_[i];
            }
        }

        assert(score_sum_ == 0);
        return -1;
    }
}