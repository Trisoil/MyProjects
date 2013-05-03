//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _RESOURCE_CHANGE_LISTENER_H_
#define _RESOURCE_CHANGE_LISTENER_H_
namespace super_node
{
    class ContentServer;

    class IResourceChangeListener
    {
    public:
        virtual ~IResourceChangeListener(){}
        virtual void OnResourceChanged(const std::set<std::string> & add_resources, const std::set<std::string> & remove_resources, boost::shared_ptr<ContentServer> content_server) = 0;
    };
}
#endif