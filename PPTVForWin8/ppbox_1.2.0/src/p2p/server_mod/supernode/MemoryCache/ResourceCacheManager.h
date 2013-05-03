//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_RESOURCE_CACHE_MANAGER_H
#define SUPER_NODE_RESOURCE_CACHE_MANAGER_H

#include "ResourceIdentifier.h"

namespace super_node
{
    class ResourceCache;
    class BlockCache;

    class ResourceCacheManager
    {
    public:
        void Statistics(boost::uint8_t resource_decay1, boost::uint8_t resource_decay2, boost::uint8_t block_decay1,
            boost::uint8_t block_decay2);

        void Insert(const ResourceIdentifier & resource_identifier, boost::shared_ptr<ResourceCache> resource_cache);
        void Erase(const ResourceIdentifier & resource_identifier);

        // 如果没找到ResourceCache，返回false。如果找到ResourceCache，不管找没找到BlockCache，都返回true
        bool FindBlock(const ResourceIdentifier & resource_identifier, boost::uint32_t block_id,
            boost::shared_ptr<ResourceCache> & resource, boost::shared_ptr<BlockCache> & block,
            size_t resource_cost_delim, size_t block_cost_delim);
        void DropMetadataIfNeeded();

        size_t Size() const { return resource_map_.size(); }

        void Clear();

    private:
        std::map<ResourceIdentifier, boost::shared_ptr<ResourceCache> > resource_map_;
    };
}

#endif  // SUPER_NODE_RESOURCE_CACHE_MANAGER_H
