//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "Tracker.h"

namespace bootstrap
{
    Tracker::Tracker() : request_count_for_statistic_(0)
    {

    }

    boost::uint32_t Tracker::GetBasedOnIspPercentage() const
    {
        return based_on_isp_percentage_;
    }

    boost::uint32_t Tracker::GetServerCountInOneResponse() const
    {
        return server_count_in_one_response_;
    }

    boost::uint32_t Tracker::GetTotalServerCount(boost::uint32_t rigion_code) const
    {
        boost::uint32_t total_server_count = 0;

        TrackerOfSameType::const_iterator outter_iter = trackers_.find(rigion_code);

        if (outter_iter != trackers_.end())
        {
            for (TrackerInSameIsp::const_iterator inner_iter = outter_iter->second.begin();
                inner_iter != outter_iter->second.end();
                ++inner_iter)
            {
                total_server_count += inner_iter->second.GetServerCount();
            }
        }

        return total_server_count;
    }

    void Tracker::ChangeTrackerServersSequenceIfNeed(boost::uint32_t rigion_code)
    {
        assert(trackers_.find(rigion_code) != trackers_.end());
        assert(request_count_in_period_.find(rigion_code) != request_count_in_period_.end());

        if (request_count_in_period_.find(rigion_code) == request_count_in_period_.end())
        {
            request_count_in_period_.insert(std::make_pair(rigion_code, 1));
        }
        else if (++request_count_in_period_[rigion_code] > 100)
        {
            TrackerInSameIsp & trackers = trackers_[rigion_code];

            for (TrackerInSameIsp::iterator iter = trackers.begin(); iter != trackers.end(); ++iter)
            {
                iter->second.ChangeServersSequence();
            }

            request_count_in_period_[rigion_code] = 0;
        }
    }

    void Tracker::Visit(boost::uint32_t rigion_code)
    {
        ++request_count_for_statistic_;
        ChangeTrackerServersSequenceIfNeed(rigion_code);
    }

    void Tracker::Insert(boost::uint8_t mod, TrackerInSameIsp & tracker)
    {
        trackers_.insert(std::make_pair(mod, tracker));
    }

    void Tracker::Clear()
    {
        trackers_.clear();
    }

    Tracker::TrackerInSameIsp & Tracker::GetTrackers(boost::uint32_t rigion_code)
    {
        if (trackers_.find(rigion_code) != trackers_.end())
        {
            return trackers_[rigion_code];
        }

        TrackerInSameIsp empty_tracker;
        return empty_tracker;
    }

    boost::uint32_t Tracker::GetModNumber(boost::uint32_t rigion_code) const
    {
        TrackerOfSameType::const_iterator iter = trackers_.find(rigion_code);

        if (iter == trackers_.end())
        {
            return 0;
        }

        return iter->second.size();
    }

    void Tracker::SetBasedOnIspPercentage(boost::uint32_t based_on_isp_percentage)
    {
        if (based_on_isp_percentage > 100)
        {
            based_on_isp_percentage_ = 50;
        }
        else
        {
            based_on_isp_percentage_ = based_on_isp_percentage;
        }
    }

    void Tracker::SetServerCountInOneResponse(boost::uint32_t server_count_in_one_response)
    {
        server_count_in_one_response_ = server_count_in_one_response;
    }

    boost::uint32_t Tracker::GetStatistic() const
    {
        return request_count_for_statistic_;
    }

    void Tracker::ClearStatistic()
    {
        request_count_for_statistic_ = 0;
    }

    bool Tracker::DoesTrackerExist(boost::uint32_t rigion_code) const
    {
        return trackers_.find(rigion_code) != trackers_.end();
    }
}
