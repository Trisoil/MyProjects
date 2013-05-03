//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "ServerInfo.h"
#include "IpLocation.h"

namespace bootstrap
{
    template <typename ServerName>
    const std::map<boost::uint32_t, std::vector<ServerName> > & ServerInfo<ServerName>::GetServers()
    {
        return servers_;
    }

    template <typename ServerName>
    void ServerInfo<ServerName>::SetServers(const std::vector<ServerName> & servers)
    {
        servers_.clear();

        servers_out_of_order_ = servers;

        next_index_to_select_ = 0;

        assert(IpLocation::Inst());

        for (typename std::vector<ServerName>::const_iterator iter = servers.begin(); iter != servers.end(); ++iter)
        {
            boost::uint32_t location = IpLocation::Inst()->GetLocation(iter->GetIP());
            if (servers_.find(location) == servers_.end())
            {
                std::vector<ServerName> servers_in_same_location;
                servers_in_same_location.push_back(*iter);

                servers_.insert(std::make_pair(location, servers_in_same_location));
            }
            else
            {
                servers_[location].push_back(*iter);
            }
        }
    }

    template <typename ServerName>
    boost::uint32_t ServerInfo<ServerName>::GetNextIndex()
    {
        return next_index_to_select_;
    }

    template <typename ServerName>
    void ServerInfo<ServerName>::GetServerList(boost::uint32_t ip, std::vector<ServerName> & selected_servers, boost::uint32_t server_based_on_isp_count,
        boost::uint32_t random_server_count)
    {
        boost::uint32_t location = IpLocation::Inst()->GetLocation(ip);

        if (location != 0)
        {
            GetServerListBasedOnIsp(location, selected_servers, server_based_on_isp_count);
            GetServerListRandomly(location, selected_servers, random_server_count);
        }
        else
        {
            GetServerListRandomly(location, selected_servers, server_based_on_isp_count + random_server_count);
        }
    }

    template <typename ServerName>
    void ServerInfo<ServerName>::GetServerListBasedOnIsp(boost::uint32_t location, std::vector<ServerName> & selected_servers,
        boost::uint32_t based_on_isp_server_count) const
    {
        typename std::map<boost::uint32_t, std::vector<ServerName> >::const_iterator it_upper =  servers_.upper_bound(location);
        typename std::map<boost::uint32_t, std::vector<ServerName> >::const_reverse_iterator it_lower(it_upper);

        while(selected_servers.size() < based_on_isp_server_count && selected_servers.size() < servers_out_of_order_.size())
        {
            if(it_upper != servers_.end() && it_lower != servers_.rend())
            {
                if(abs((int)location - (int)it_upper->first) < abs((int)location - (int)it_lower->first))
                {
                    AddServer(it_upper->second, selected_servers, based_on_isp_server_count);
                    ++it_upper;
                }
                else
                {
                    AddServer(it_lower->second, selected_servers, based_on_isp_server_count);
                    ++it_lower;
                }
            }
            else if(it_upper!= servers_.end())
            {
                AddServer(it_upper->second, selected_servers, based_on_isp_server_count);
                ++it_upper;
            }
            else if(it_lower != servers_.rend())
            {
                AddServer(it_lower->second, selected_servers, based_on_isp_server_count);
                ++it_lower;
            }
            else
            {
                break;
            }
        }
    }

    template <typename ServerName>
    void ServerInfo<ServerName>::GetServerListRandomly(boost::uint32_t location, std::vector<ServerName> & selected_servers,
        boost::uint32_t random_server_count)
    {
        std::set<ServerName> unique_selected_servers;
        for (size_t i = 0; i < selected_servers.size(); ++i)
        {
            unique_selected_servers.insert(selected_servers[i]);
        }

        boost::uint32_t randomly_selected_server_count = 0;
        while(randomly_selected_server_count < random_server_count && selected_servers.size() < servers_out_of_order_.size())
        {
            if (next_index_to_select_ >= servers_out_of_order_.size())
            {
                next_index_to_select_ = 0;
            }

            if (unique_selected_servers.find(servers_out_of_order_[next_index_to_select_]) == unique_selected_servers.end())
            {
                unique_selected_servers.insert(servers_out_of_order_[next_index_to_select_]);
                selected_servers.push_back(servers_out_of_order_[next_index_to_select_]);
                ++randomly_selected_server_count;
            }

            ++next_index_to_select_;
        }
    }

    template <typename ServerName>
    void ServerInfo<ServerName>::AddServer(const std::vector<ServerName> & servers, std::vector<ServerName> & selected_servers,
        boost::uint32_t based_on_isp_server_count) const
    {
        for (size_t i = 0; i < servers.size() && selected_servers.size() < based_on_isp_server_count; ++i)
        {
            selected_servers.push_back(servers[i]);

            if (selected_servers.size() == based_on_isp_server_count)
            {
                break;
            }
        }
    }

    template <typename ServerName>
    void ServerInfo<ServerName>::ChangeServersSequence()
    {
        for (typename std::map<boost::uint32_t, std::vector<ServerName> >::iterator iter = servers_.begin();
            iter != servers_.end(); ++iter)
        {
            if (iter->second.size() > 1)
            {
                iter->second.push_back(iter->second[0]);
                iter->second.erase(iter->second.begin());
            }
        }

        random_shuffle(servers_out_of_order_.begin(), servers_out_of_order_.end());
    }

    template <typename ServerName>
    void ServerInfo<ServerName>::GetAllServerInOrder(boost::uint32_t ip, std::vector<ServerName> & selected_servers)
    {
        assert(IpLocation::Inst());

        boost::uint32_t location = IpLocation::Inst()->GetLocation(ip);
        if (location == 0)
        {
            GetServerListRandomly(location, selected_servers, servers_out_of_order_.size());
        }
        else
        {
            GetServerListBasedOnIsp(location, selected_servers, servers_out_of_order_.size());
        }
    }

    template <typename ServerName>
    boost::uint32_t ServerInfo<ServerName>::GetServerCount() const
    {
        return servers_out_of_order_.size();
    }
}
