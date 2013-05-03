//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _CONTENT_SERVER_RESOURCE_MANAGER_H_
#define _CONTENT_SERVER_RESOURCE_MANAGER_H_

#include "RemoteCache/ResourceChangeListener.h"
#include "SuperNodeServiceStruct.h"
#include "RemoteCache/ServerListAsker.h"


namespace super_node
{
    class IContentServer;
    class IContentServerResourceManager
    {
    public:
        virtual void Start() = 0;
        virtual void Stop() = 0;
        virtual boost::shared_ptr<IContentServer> FindContentServer(const std::string & resource_name)const = 0;
        virtual void UpdateConfig(const RemoteCacheConfig & config) = 0;
        virtual ~IContentServerResourceManager(){};
    };

    class ContentServerResourceManager
        :public IResourceChangeListener,
        public boost::enable_shared_from_this<ContentServerResourceManager>,
        public IContentServerResourceManager
    {
    public:
        ContentServerResourceManager(boost::shared_ptr<boost::asio::io_service> io_service, const RemoteCacheConfig & config)
            :io_service_(io_service), config_(config)
        {

        }

        void Start();
        void Stop();
        boost::shared_ptr<IContentServer> FindContentServer(const std::string & resource_name)const;
        void OnResourceChanged(const std::set<std::string> & add_resources, const std::set<std::string> & remove_resources, boost::shared_ptr<ContentServer> content_server);
        void DoUpdateContentServers(const std::vector<std::string> & server_addresses);

        void SetServerListAsker(boost::shared_ptr<IServerListAsker> server_list_asker)
        {
            server_list_asker_ = server_list_asker;
        }

        size_t GetContentServerNumber()
        {
            return content_servers_.size();
        }

        void UpdateConfig(const RemoteCacheConfig & config);

        bool TwoContentServerListEqual(const std::vector<boost::shared_ptr<ContentServer> > & lhs, const std::vector<boost::shared_ptr<ContentServer> > & rhs);

    private:
        RemoteCacheConfig config_;
        std::map<std::string, boost::uint16_t> resource_map_;
        std::vector<boost::shared_ptr<ContentServer> > content_servers_;
        boost::shared_ptr<IServerListAsker> server_list_asker_;
        boost::shared_ptr<boost::asio::io_service> io_service_;
    };
}

#endif