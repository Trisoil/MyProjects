//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_RECENT_RESOURCE_WRITE_ATTEMPTS_H
#define SUPER_NODE_RECENT_RESOURCE_WRITE_ATTEMPTS_H

namespace super_node
{
    enum ResourcePopularity
    {
        Undetermined,
        Popular,
        Unpopular
    };

    class ResourceWriteAttemptRecord
    {
        static const size_t TicksPerHour;
        static const double DiminishingRate;

    public:
        static const size_t DefaultPopularResourceRequestsCount;

        ResourceWriteAttemptRecord();

        void RecordWriteAttempt();

        void DiminishOverTime(size_t popular_resource_requests_count);

        ResourcePopularity GetPopularity(size_t popular_resource_requests_count) const;
        
        bool IsTrivialRecord(size_t popular_resource_requests_count) const;

    private:
        framework::timer::TimeCounter ticks_since_last_write_attempt_;
        size_t attempts_count_;
    };

    class RecentResourceWriteAttempts
    {
    public:
        RecentResourceWriteAttempts()
        {
            popular_resource_requests_count_ = ResourceWriteAttemptRecord::DefaultPopularResourceRequestsCount;
        }

        void ExpireTrivialRecords();

        //output parameter is_popular_resource returns true if the resource is popular
        void RecordWriteAttempt(const ResourceIdentifier& resource_identifier, bool& is_popular_resource);

        void SetPopularResourceRequestsCount(size_t popular_resource_requests_count);

        void Clear()
        {
            resources_.clear();
        }

    private:
        std::map<ResourceIdentifier, ResourceWriteAttemptRecord> resources_;
        size_t popular_resource_requests_count_;
    };
}

#endif //SUPER_NODE_RECENT_RESOURCE_WRITE_ATTEMPTS_H
