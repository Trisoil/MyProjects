//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "ResourceCacheManager.h"
#include "ResourceCache.h"

namespace super_node
{
    void ResourceCacheManager::Statistics(boost::uint8_t resource_decay1, boost::uint8_t resource_decay2, boost::uint8_t block_decay1,
        boost::uint8_t block_decay2)
    {
        for (std::map<ResourceIdentifier, boost::shared_ptr<ResourceCache> >::iterator iter = resource_map_.begin();
            iter != resource_map_.end(); ++iter)
        {
            iter->second->AllTick(resource_decay1, resource_decay2, block_decay1, block_decay2);
            iter->second->UpdateCost();
        }
    }

    void ResourceCacheManager::DropMetadataIfNeeded()
    {
        for (std::map<ResourceIdentifier, boost::shared_ptr<ResourceCache> >::iterator iter = resource_map_.begin();
            iter != resource_map_.end();)
        {
            if (iter->second->IsResourceMetadataTrivial())
            {
                resource_map_.erase(iter++);
            }
            else
            {
                ++iter;
            }
        }
    }

    void ResourceCacheManager::Insert(const ResourceIdentifier & resource_identifier, boost::shared_ptr<ResourceCache> resource_cache)
    {
        resource_map_[resource_identifier] = resource_cache;
    }

    void ResourceCacheManager::Erase(const ResourceIdentifier & resource_identifier)
    {
        resource_map_.erase(resource_identifier);
    }

    bool ResourceCacheManager::FindBlock(const ResourceIdentifier & resource_identifier, boost::uint32_t block_id,
        boost::shared_ptr<ResourceCache> & resource, boost::shared_ptr<BlockCache> & block,
        size_t resource_cost_delim, size_t block_cost_delim)
    {
        bool is_resource_founded = true;

        if (resource_map_.find(resource_identifier) == resource_map_.end())
        {
            resource = boost::shared_ptr<ResourceCache>(new ResourceCache(resource_cost_delim));
            resource_map_.insert(std::make_pair(resource_identifier, resource));
            is_resource_founded = false;
        }
        else
        {
            resource = resource_map_[resource_identifier];
        }

        if (!resource->GetBlock(block_id))
        {
            block = boost::shared_ptr<BlockCache>(new BlockCache(resource, block_id, block_cost_delim));
            resource->SetBlock(block);
        }
        else
        {
            block = resource->GetBlock(block_id);
        }

        return is_resource_founded;
    }

    void ResourceCacheManager::Clear()
    {
        for (std::map<ResourceIdentifier, boost::shared_ptr<ResourceCache> >::iterator iter = resource_map_.begin();
            iter != resource_map_.end(); 
            ++iter)
        {
            iter->second->Clear();
        }

        resource_map_.clear();
    }
}