//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_RESOURCE_SET_IN_COST_ORDER_H
#define SUPER_NODE_RESOURCE_SET_IN_COST_ORDER_H

#include "ResourceCache.h"

namespace super_node
{
    struct ResourceCostCompare
        : public count_object_allocate<ResourceCostCompare>
    {
        bool operator () (
            boost::shared_ptr<ResourceCache> const & lhs,
            boost::shared_ptr<ResourceCache> const & rhs) const
        {
            return ((lhs->GetCost() < rhs->GetCost())
                || (lhs->GetCost() == rhs->GetCost()
                && lhs < rhs));
        }
    };

    class ResourceSetInCostOrder
        : public count_object_allocate<ResourceSetInCostOrder>
    {
    public:
        void Insert(boost::shared_ptr<ResourceCache> resource);
        void Erase(boost::shared_ptr<ResourceCache> resource);
        bool CheckOrder() const;
        size_t DropBlockCacheInOrder(size_t need_drop_num, ReleaseBlockDataCallback callback);
        void DropMetadataIfNeeded();
        void ReArrangeTheSet();
        size_t Size() const;

    private:
        std::set<boost::shared_ptr<ResourceCache>, ResourceCostCompare> resource_order_;
    };

}

#endif  // SUPER_NODE_RESOURCE_SET_IN_COST_ORDER_H