//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_LOCAL_DISK_CACHE_STATISTICS_H
#define SUPER_NODE_LOCAL_DISK_CACHE_STATISTICS_H

namespace super_node
{
    class ComponentStatusItem;
    class DiskResource;
    class HitStatistics;
    class DiskResourcesManagement;

    class LocalDiskCacheStatistics
        : public boost::enable_shared_from_this<LocalDiskCacheStatistics>
        ,public count_object_allocate<LocalDiskCacheStatistics>
    {
    public:
        static const size_t SecondsPerMinute = 60;

        LocalDiskCacheStatistics();

        void Start();
        void Stop();

        void ReportStatus(const DiskResourcesManagement& disk_resources);

        void OnHit();
        void OnMissed();

    private:
        boost::shared_ptr<ComponentStatusItem> hits_status_;
        boost::shared_ptr<ComponentStatusItem> hits_count_;
        boost::shared_ptr<ComponentStatusItem> cached_resources_count_;

        boost::shared_ptr<HitStatistics> hits_statistics_;
    };

}

#endif //SUPER_NODE_LOCAL_DISK_CACHE_STATISTICS_H