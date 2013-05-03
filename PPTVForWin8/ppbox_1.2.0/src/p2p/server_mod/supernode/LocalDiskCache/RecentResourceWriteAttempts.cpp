//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "RecentResourceWriteAttempts.h"

namespace super_node
{
    const size_t ResourceWriteAttemptRecord::TicksPerHour = 60*60*1000;
    const double ResourceWriteAttemptRecord::DiminishingRate = 0.9;
    const size_t ResourceWriteAttemptRecord::DefaultPopularResourceRequestsCount = 30;

    ResourceWriteAttemptRecord::ResourceWriteAttemptRecord()
    {
        attempts_count_ = 0;
    }

    void ResourceWriteAttemptRecord::RecordWriteAttempt()
    {
        ticks_since_last_write_attempt_.reset();
        ++attempts_count_;
    }

    void ResourceWriteAttemptRecord::DiminishOverTime(size_t popular_resource_requests_count)
    {
        if (attempts_count_ > popular_resource_requests_count)
        {
            return;
        }

        if (ticks_since_last_write_attempt_.elapse() > ResourceWriteAttemptRecord::TicksPerHour)
        {
            attempts_count_ = static_cast<size_t>(attempts_count_*ResourceWriteAttemptRecord::DiminishingRate);
        }
    }

    ResourcePopularity ResourceWriteAttemptRecord::GetPopularity(size_t popular_resource_requests_count) const
    {
        if (ticks_since_last_write_attempt_.elapse() > ResourceWriteAttemptRecord::TicksPerHour && 
            attempts_count_ < 3)
        {
            return Unpopular;
        }

        if (attempts_count_ > popular_resource_requests_count)
        {
            return Popular;
        }

        return Undetermined;
    }

    bool ResourceWriteAttemptRecord::IsTrivialRecord(size_t popular_resource_requests_count) const
    {
        ResourcePopularity popularity = GetPopularity(popular_resource_requests_count);
        if (popularity == Unpopular)
        {
            return true;
        }

        if (popularity == Popular && ticks_since_last_write_attempt_.elapse() > ResourceWriteAttemptRecord::TicksPerHour)
        {
            return true;
        }

        return false;
    }

    void RecentResourceWriteAttempts::ExpireTrivialRecords()
    {
        size_t erased_resources(0);
        for(std::map<ResourceIdentifier, ResourceWriteAttemptRecord>::iterator iter = resources_.begin();
            iter != resources_.end();
            )
        {
            ResourceWriteAttemptRecord& record = iter->second;
            record.DiminishOverTime(popular_resource_requests_count_);
            if (record.IsTrivialRecord(popular_resource_requests_count_))
            {
                ++erased_resources;
                resources_.erase(iter++);
            }
            else
            {
                ++iter;
            }
        }

        LOG4CPLUS_INFO(Loggers::LocalDiskCache(), "RecentResourceDownloads: expiring "<<erased_resources<<" unpopular resources out of the overall "<<resources_.size() + erased_resources<<" resources.");
    }

    void RecentResourceWriteAttempts::RecordWriteAttempt(const ResourceIdentifier& resource_identifier, bool& is_popular_resource)
    {
        std::map<ResourceIdentifier, ResourceWriteAttemptRecord>::iterator iter = resources_.find(resource_identifier);
        if (iter == resources_.end())
        {
            ResourceWriteAttemptRecord new_record;
            resources_.insert(std::make_pair(resource_identifier, new_record));
        }

        ResourceWriteAttemptRecord& attempt_record = resources_[resource_identifier];
        attempt_record.RecordWriteAttempt();
        is_popular_resource = attempt_record.GetPopularity(popular_resource_requests_count_) == Popular;
    }

    void RecentResourceWriteAttempts::SetPopularResourceRequestsCount(size_t popular_resource_requests_count)
    {
        if (popular_resource_requests_count_ == popular_resource_requests_count)
        {
            return;   
        }

        LOG4CPLUS_INFO(Loggers::LocalDiskCache(), "Updating popular_resource_requests_count from "<< popular_resource_requests_count_ << " to " << popular_resource_requests_count);
        popular_resource_requests_count_ = popular_resource_requests_count;
    }
}