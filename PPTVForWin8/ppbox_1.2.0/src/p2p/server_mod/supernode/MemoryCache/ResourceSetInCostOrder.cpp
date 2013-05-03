//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "ResourceSetInCostOrder.h"

namespace super_node
{
    void ResourceSetInCostOrder::Insert(boost::shared_ptr<ResourceCache> resource)
    {
        resource_order_.insert(resource);
    }

    void ResourceSetInCostOrder::Erase(boost::shared_ptr<ResourceCache> resource)
    {
        resource_order_.erase(resource);
    }

    bool ResourceSetInCostOrder::CheckOrder() const
    {
        if (resource_order_.empty())
            return true;

        std::set<boost::shared_ptr<ResourceCache>, ResourceCostCompare>::const_iterator iter = resource_order_.begin();
        std::set<boost::shared_ptr<ResourceCache>, ResourceCostCompare>::const_iterator next_iter = resource_order_.begin();
        ++next_iter;

        ResourceCostCompare resource_cost_compare;
        for (; next_iter != resource_order_.end(); ++iter, ++next_iter)
        {
            if (!resource_cost_compare(*iter, *next_iter))
            {
                return false;
            }
        }
        return true;
    }

    size_t ResourceSetInCostOrder::DropBlockCacheInOrder(size_t need_drop_num, ReleaseBlockDataCallback callback)
    {
        size_t droped_num = 0;
        for (std::set<boost::shared_ptr<ResourceCache>, ResourceCostCompare>::iterator iter = resource_order_.begin();
            iter != resource_order_.end() && droped_num < need_drop_num; ++iter)
        {
            droped_num += (*iter)->DropBlockCacheInCostOrder(need_drop_num - droped_num, callback);
        }
        return droped_num;
    }

    void ResourceSetInCostOrder::DropMetadataIfNeeded()
    {
        for (std::set<boost::shared_ptr<ResourceCache>, ResourceCostCompare>::iterator iter = resource_order_.begin();
            iter != resource_order_.end();)
        {
            if ((*iter)->IsResourceMetadataTrivial())
            {
                (*iter)->DropBlocks();
                resource_order_.erase(iter++);
            }
            else
            {
                ++iter;
            }
        }
    }

    void ResourceSetInCostOrder::ReArrangeTheSet()
    {
        std::set<boost::shared_ptr<ResourceCache> > tmp_resource_set;

        for (std::set<boost::shared_ptr<ResourceCache>, ResourceCostCompare>::iterator iter = resource_order_.begin();
            iter != resource_order_.end(); ++iter)
        {
            tmp_resource_set.insert(*iter);
            (*iter)->ReArrangeTheBlockSet();
        }

        resource_order_.clear();

        for (std::set<boost::shared_ptr<ResourceCache> >::const_iterator iter = tmp_resource_set.begin();
            iter != tmp_resource_set.end(); ++iter)
        {
            Insert(*iter);
        }
    }

    size_t ResourceSetInCostOrder::Size() const
    {
        return resource_order_.size();
    }
}