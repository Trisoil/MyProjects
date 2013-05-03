//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef TRACKER_H
#define TRACKER_H

#include "ServerInfo.h"

namespace bootstrap
{
    class Tracker
    {
    public:
        typedef std::map<boost::uint8_t, ServerInfo<protocol::TRACKER_INFO> > TrackerInSameIsp;
        typedef std::map<boost::uint32_t, TrackerInSameIsp> TrackerOfSameType;

    public:
        Tracker();
        boost::uint32_t GetBasedOnIspPercentage() const;
        boost::uint32_t GetServerCountInOneResponse() const;
        boost::uint32_t GetTotalServerCount(boost::uint32_t rigion_code) const;
        boost::uint32_t GetModNumber(boost::uint32_t rigion_code) const;
        void Visit(boost::uint32_t rigion_code);
        void Insert(boost::uint8_t mod, TrackerInSameIsp & tracker);
        void Clear();
        Tracker::TrackerInSameIsp & GetTrackers(boost::uint32_t rigion_code);
        void SetBasedOnIspPercentage(boost::uint32_t based_on_isp_percentage);
        void SetServerCountInOneResponse(boost::uint32_t server_count_in_one_response);
        boost::uint32_t GetStatistic() const;
        void ClearStatistic();
        bool DoesTrackerExist(boost::uint32_t rigion_code) const;

    private:
        void ChangeTrackerServersSequenceIfNeed(boost::uint32_t rigion_code);

    private:
        TrackerOfSameType trackers_;
        boost::uint32_t based_on_isp_percentage_;
        boost::uint32_t server_count_in_one_response_;
        boost::uint32_t request_count_for_statistic_;

        std::map<boost::uint32_t, boost::uint32_t> request_count_in_period_;
    };
}

#endif  // TRACKER_H
