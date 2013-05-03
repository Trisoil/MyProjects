//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "LocalDiskCacheStatistics.h"
#include "HitStatistics.h"
#include "../LocalDiskCache/DiskResourcesManagement.h"
#include "../ServiceHealthMonitor.h"

namespace super_node
{
    LocalDiskCacheStatistics::LocalDiskCacheStatistics()
        : hits_statistics_(new HitStatistics())
    {
    }

    void LocalDiskCacheStatistics::Start()
    {
        boost::shared_ptr<ServiceComponent> service_component = ServiceHealthMonitor::Instance()->RegisterComponent(ServiceComponents::LocalDiskCache);
        hits_status_ = service_component->RegisterStatusItem("Hits Rate", "%");
        hits_count_ = service_component->RegisterStatusItem("Hits Count", "hits/s");
        cached_resources_count_ = service_component->RegisterStatusItem("Resources on disks", "resources");
    }

    void LocalDiskCacheStatistics::Stop()
    {
        ServiceHealthMonitor::Instance()->UnregisterComponent(ServiceComponents::LocalDiskCache);
    }

    void LocalDiskCacheStatistics::ReportStatus(const DiskResourcesManagement& disk_resources)
    {
        cached_resources_count_->SetStatus(disk_resources.GetResourcesCount());
        hits_count_->SetStatus(hits_statistics_->HitCount()/SecondsPerMinute);
        hits_status_->SetStatus(hits_statistics_->HitRate());
        hits_statistics_->Reset();
    }

    void LocalDiskCacheStatistics::OnHit()
    {
        hits_statistics_->Hit();
    }

    void LocalDiskCacheStatistics::OnMissed()
    {
        hits_statistics_->Missed();
    }
}