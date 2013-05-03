//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_DISK_SELECTION_STRATEGY_H
#define SUPER_NODE_DISK_SELECTION_STRATEGY_H

namespace super_node
{
    class ILocalDisk;

    class DiskSelectionStrategy
        : public count_object_allocate<DiskSelectionStrategy>
    {
    public:
        DiskSelectionStrategy(const std::vector<boost::shared_ptr<ILocalDisk> >& disks);

        int SelectDisk() const;

    private:
        //每个磁盘有自己的候选得分，得分越高者越有可能被挑选上
        std::vector<size_t> disks_candidate_score_;
        size_t score_sum_;
    };

}

#endif //SUPER_NODE_DISK_SELECTION_STRATEGY_H