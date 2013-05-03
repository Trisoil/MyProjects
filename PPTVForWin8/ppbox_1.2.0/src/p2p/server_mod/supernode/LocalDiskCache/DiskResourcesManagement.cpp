//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "DiskResourcesManagement.h"
#include "DiskResource.h"

namespace super_node
{
    boost::shared_ptr<DiskResource> DiskResourcesManagement::FindResource(const ResourceIdentifier& resource_identifier) const
    {
        DiskResourceMap::const_iterator iter = resources_on_disk_.find(resource_identifier);
        if (iter != resources_on_disk_.end())
        {
            return iter->second;
        }

        return boost::shared_ptr<DiskResource>();
    }

    void DiskResourcesManagement::AddResource(boost::uint8_t disk_index, boost::shared_ptr<DiskResource> resource)
    {
        resource->SetDiskIndex(disk_index);
        resources_on_disk_[resource->GetResourceIdentifier()] = resource;
    }

    void DiskResourcesManagement::RemoveResource(const ResourceIdentifier& resource_identifier)
    {
        resources_on_disk_.erase(resource_identifier);
    }

    void DiskResourcesManagement::RemoveAllResource()
    {
        resources_on_disk_.clear();
    }

    void DiskResourcesManagement::GetLeastSignificantResources(boost::uint8_t disk_index, size_t blocks_count, std::vector<ResourceIdentifier>& least_significant_resources) const
    {
        std::multimap<size_t, boost::shared_ptr<DiskResource> > importance_scores;
        for(DiskResourceMap::const_iterator iter = resources_on_disk_.begin();
            iter != resources_on_disk_.end();
            ++iter)
        {
            if (iter->second->GetDiskIndex() == disk_index)
            {
                importance_scores.insert(std::make_pair(iter->second->GetImportanceScore(), iter->second));
            }
        }

        least_significant_resources.clear();
        size_t blocks_sum(0);
        for (std::multimap<size_t, boost::shared_ptr<DiskResource> >::const_iterator iter = importance_scores.begin();
            iter != importance_scores.end();
            ++iter)
        {
            least_significant_resources.push_back(iter->second->GetResourceIdentifier());

            blocks_sum += iter->second->GetBlocksCount();
            if (blocks_sum >= blocks_count)
            {
                break;
            }
        }
    }

    size_t DiskResourcesManagement::GetResourcesCount() const
    {
        return resources_on_disk_.size();
    }

    void DiskResourcesManagement::OnMinuteElapsed(size_t minutes_elapsed)
    {
        for(DiskResourceMap::iterator iter = resources_on_disk_.begin();
            iter != resources_on_disk_.end();
            ++iter)
        {
            iter->second->OnMinuteElapsed(minutes_elapsed);
        }
    }
}