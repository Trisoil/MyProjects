//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SERVER_INFO_H
#define SERVER_INFO_H

namespace bootstrap
{
    template <typename ServerName>
    class ServerInfo
    {
    public:
        const std::map<boost::uint32_t, std::vector<ServerName> > & GetServers();
        void SetServers(const std::vector<ServerName> & servers);
        boost::uint32_t GetNextIndex();
        void GetServerList(boost::uint32_t ip, std::vector<ServerName> & selected_servers, boost::uint32_t server_based_on_isp_count,
            boost::uint32_t random_server_count);
        void GetAllServerInOrder(boost::uint32_t ip, std::vector<ServerName> & selected_servers);
        void ChangeServersSequence();
        boost::uint32_t GetServerCount() const;

    private:
        void GetServerListBasedOnIsp(boost::uint32_t location, std::vector<ServerName> & selected_servers,
            boost::uint32_t server_based_on_isp_count) const;

        void GetServerListRandomly(boost::uint32_t location, std::vector<ServerName> & selected_servers,
            boost::uint32_t random_server_count);

        void AddServer(const std::vector<ServerName> & servers, std::vector<ServerName> & selected_servers,
            boost::uint32_t based_on_isp_server_count) const;

    private:
        std::map<boost::uint32_t, std::vector<ServerName> > servers_;
        std::vector<ServerName> servers_out_of_order_;
        boost::uint32_t next_index_to_select_;
    };
}

#include "ServerInfo.hpp"

#endif  // SERVER_INFO_H