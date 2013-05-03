//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "SuperNode.h"

namespace bootstrap
{
    SuperNodes::SuperNodes() : list_request_count_in_period_(0)
    {

    }

    void SuperNodes::SetReturnProbability(boost::uint32_t return_probability)
    {
        return_probability_ = return_probability;
    }

    void SuperNodes::SetReturnCountBasedOnIsp(boost::uint32_t return_count_based_on_isp)
    {
        return_count_based_on_isp_ = return_count_based_on_isp;
    }

    void SuperNodes::SetReturnCountInTotal(boost::uint32_t return_count_in_total)
    {
        return_count_in_total_ = return_count_in_total;
    }

    bool SuperNodes::ShouldReturnServerList() const
    {
        return rand() % 10000 < return_probability_;
    }

    void SuperNodes::ChangeServersSequenceIfNeeded()
    {
        if (list_request_count_in_period_ > 100)
        {
            servers_.ChangeServersSequence();
            list_request_count_in_period_ = 0;
        }
    }

    void SuperNodes::GetServerList(boost::uint32_t ip, std::vector<protocol::SuperNodeInfo> & selected_servers)
    {
        if (!ShouldReturnServerList())
        {
            return;
        }

        ++list_request_count_in_period_;

        ChangeServersSequenceIfNeeded();

        servers_.GetServerList(ip, selected_servers, return_count_based_on_isp_, return_count_in_total_ - return_count_based_on_isp_);
    }

    void SuperNodes::SetServers(const std::vector<protocol::SuperNodeInfo> & servers)
    {
        servers_.SetServers(servers);
    }
}
