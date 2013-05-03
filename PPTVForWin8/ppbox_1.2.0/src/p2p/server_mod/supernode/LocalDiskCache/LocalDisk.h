//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_LOCAL_DISK_H
#define SUPER_NODE_LOCAL_DISK_H

#include "../LocalDiskCache.h"

namespace super_node
{

class DiskStatistics;
class ComponentStatusItem;

class ILocalDisk
{
public:
    virtual void Start() = 0;
    virtual void Stop() = 0;

    virtual bool AsyncRead(const ResourceIdentifier& resource_identifier, int block_index, int read_priority, ReadResourceBlockCallback callback) = 0;
    virtual bool AsyncWrite(const ResourceIdentifier& resource_identifier, const std::map<size_t, boost::shared_ptr<BlockData> >& blocks, WriteResourceTaskCallback callback) = 0;
    virtual bool AsyncLoadResources(LoadResourcesTaskCallback callback, bool delete_resource_on_error) = 0;
    virtual bool AsyncDeleteResources(const std::vector<ResourceIdentifier>& resources, DeleteResourcesTaskCallback callback) = 0;

    virtual boost::filesystem::path GetDiskPath() const = 0;

    virtual size_t GetFreeSpacePercentage() const = 0;
    virtual boost::uint64_t GetDiskSpaceInBytes() const = 0;

    virtual void UpdateConfig() = 0;

    virtual ~ILocalDisk(){}
};

boost::shared_ptr<ILocalDisk> CreateLocalDisk(const std::string& path);

}

#endif //SUPER_NODE_LOCAL_DISK_H
