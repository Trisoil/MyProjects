//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_DISK_RESOURCES_MANAGEMENT_H
#define SUPER_NODE_DISK_RESOURCES_MANAGEMENT_H

#include "DiskResource.h"

namespace super_node
{
    class DiskResourcesManagement
    {
        typedef std::map<ResourceIdentifier, boost::shared_ptr<DiskResource> > DiskResourceMap;
    public:
        boost::shared_ptr<DiskResource> FindResource(const ResourceIdentifier& resource_identifier) const;

        void AddResource(boost::uint8_t disk_index, boost::shared_ptr<DiskResource> resource);

        void RemoveResource(const ResourceIdentifier& resource_identifier);

        void GetLeastSignificantResources(boost::uint8_t disk_index, size_t blocks_count, std::vector<ResourceIdentifier>& least_significant_resources) const;

        size_t GetResourcesCount() const;

        void OnMinuteElapsed(size_t minutes_elapsed);

        void RemoveAllResource();

    private:
        DiskResourceMap resources_on_disk_;
    };
}

#endif //SUPER_NODE_DISK_RESOURCES_MANAGEMENT_H