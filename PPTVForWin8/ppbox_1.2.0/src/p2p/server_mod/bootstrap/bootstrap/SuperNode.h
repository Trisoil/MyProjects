//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPERNODE_H
#define SUPERNODE_H

#include "ServerInfo.h"

namespace bootstrap
{
    class SuperNodes
    {
    public:
        SuperNodes();

        void SetReturnProbability(boost::uint32_t return_probability);
        void SetReturnCountBasedOnIsp(boost::uint32_t return_count_based_on_isp);
        void SetReturnCountInTotal(boost::uint32_t return_count_in_total);
        void GetServerList(boost::uint32_t ip, std::vector<protocol::SuperNodeInfo> & selected_servers);
        void SetServers(const std::vector<protocol::SuperNodeInfo> & servers);

    private:
        bool ShouldReturnServerList() const;
        void ChangeServersSequenceIfNeeded();

    private:
        boost::uint32_t list_request_count_in_period_;
        boost::uint32_t return_probability_;
        boost::uint32_t return_count_based_on_isp_;
        boost::uint32_t return_count_in_total_;

        ServerInfo<protocol::SuperNodeInfo> servers_;
    };
}

#endif  // SUPERNODE_H
