//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "ContentServerResourceManager.h"
#include "RemoteCache/ContentServer.h"

namespace super_node
{
    void ContentServerResourceManager::Start()
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::RemoteCache(), "ContentServerResourceManager::Start()");
        server_list_asker_->Start();
        LOG4CPLUS_INFO(Loggers::RemoteCache(), "ContentServerResourceManager started.");
    }

    void ContentServerResourceManager::Stop()
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::RemoteCache(), "ContentServerResourceManager::Stop()");
        for(std::vector<boost::shared_ptr<ContentServer> >::iterator iter = content_servers_.begin(); iter != content_servers_.end(); iter ++)
        {
            (*iter)->Stop();
        }
        LOG4CPLUS_INFO(Loggers::RemoteCache(), "ContentServerResourceManager stopped.");
    }

    boost::shared_ptr<IContentServer> ContentServerResourceManager::FindContentServer(const std::string & resource_name)const
    {
        std::map<std::string, boost::uint16_t>::const_iterator iter = resource_map_.find(resource_name);
        if (iter != resource_map_.end())
        {
            return content_servers_[iter->second];
        }
        return boost::shared_ptr<ContentServer>();
    }

    void ContentServerResourceManager::OnResourceChanged(const std::set<std::string> & add_resources, const std::set<std::string> & remove_resources, boost::shared_ptr<ContentServer> content_server)
    {
        assert(add_resources.size() > 0 || remove_resources.size() > 0);

        boost::uint16_t content_server_index;
        if (content_server)
        {
            for (content_server_index = 0; content_server_index < content_servers_.size(); ++content_server_index)
            {
                if (*content_servers_[content_server_index] == *content_server)
                {
                    break;
                }
            }
        }

        assert(content_server_index < content_servers_.size());

        for (std::set<string>::const_iterator add_iter = add_resources.begin(); add_iter != add_resources.end(); ++add_iter)
        {
            resource_map_[*add_iter] = content_server_index;
        }

        for(std::set<string>::const_iterator remove_iter = remove_resources.begin(); remove_iter != remove_resources.end(); ++remove_iter)
        {
            std::map<std::string, boost::uint16_t>::iterator resource_map_iter = resource_map_.find(*remove_iter);
            if (resource_map_iter != resource_map_.end() && resource_map_iter->second == content_server_index)
            {
                resource_map_.erase(*remove_iter);
            }
        }

        LOG4CPLUS_INFO(Loggers::RemoteCache(), "Server["<<content_server_index<<"] Resources Update: added: "<<add_resources.size()<<", removed: "<<remove_resources.size());
    }

    void ContentServerResourceManager::DoUpdateContentServers(const std::vector<std::string> & server_addresses)
    {
        std::vector<boost::shared_ptr<ContentServer> > new_content_servers;
        for (std::vector<std::string>::const_iterator iter = server_addresses.begin(); 
            iter != server_addresses.end(); ++iter)
        {
            boost::shared_ptr<ContentServer> content_server;
            content_server.reset(new ContentServer(*iter, config_, shared_from_this(), io_service_));
            new_content_servers.push_back(content_server);
        }

        if (new_content_servers.size() == 0)
        {
            LOG4CPLUS_WARN(Loggers::RemoteCache(), "No content server is found to serve this SuperNode instance. Please make sure ServerList.txt and place_identifier setting is configured properly.");
            OperationsLogging::Log(OperationsLogEvents::WarningNoContentServer, Warning);
        }

        if (!TwoContentServerListEqual(new_content_servers, content_servers_))
        {
            for (std::vector<boost::shared_ptr<ContentServer> >::iterator iter = content_servers_.begin();
                iter != content_servers_.end(); ++iter)
            {
                (*iter)->Stop();
            }

            content_servers_.clear();
            content_servers_ = new_content_servers;
            resource_map_.clear();

            for (std::vector<boost::shared_ptr<ContentServer> >::iterator it = content_servers_.begin();
                it != content_servers_.end(); ++it)
            {
                (*it)->Start();
            }

            LOG4CPLUS_INFO(Loggers::RemoteCache(), "ContentServers updated. now have " << content_servers_.size() << "CDNs");
        }
    }

    bool ContentServerResourceManager::TwoContentServerListEqual(const std::vector<boost::shared_ptr<ContentServer> > & lhs, const std::vector<boost::shared_ptr<ContentServer> > & rhs)
    {
        if (lhs.size() != rhs.size())
            return false;

        for ( boost::uint32_t index = 0; index < lhs.size(); ++index)
        {
            if (*lhs[index] != *rhs[index])
            {
                return false;
            }
        }

        return true;
    }

    void ContentServerResourceManager::UpdateConfig(const RemoteCacheConfig & config)
    {
        config_ = config;

        for (std::vector<boost::shared_ptr<ContentServer> >::iterator iter = content_servers_.begin();
            iter != content_servers_.end(); 
            ++iter)
        {
            (*iter)->UpdateConfig(config);
        }

        server_list_asker_->UpdateConfig(config.ask_for_server_list_interval_, config.place_identifier_);
    }
}
