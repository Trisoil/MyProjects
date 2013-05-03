//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_RESOURCE_SELECTOR_H
#define SUPER_NODE_RESOURCE_SELECTOR_H

namespace super_node_test
{
    class Resource
    {
    public:
        Resource(const string& resource_name, size_t resource_size)
            : resource_name_(resource_name), resource_size_(resource_size)
        {
        }

        string GetResourceName() const { return resource_name_; }
        size_t GetResourceSize() const { return resource_size_; }

    private:
        string resource_name_;
        size_t resource_size_;
    };

    class ResourceSelector
        : public boost::enable_shared_from_this<ResourceSelector>
    {
    public:
        ResourceSelector(boost::shared_ptr<boost::asio::io_service> io_service) : hit_sum_(0)
        {
            started_ = false;
            io_service_ = io_service;
        }

        void Start();
        void Stop(){}
        Resource SelectResource();

        bool IsStarted() const 
        {
            return started_;
        }

    private:
        void DoStart();
        void ParseResourcesList();
        void ParseResourcesSize();
        void OnResourceListDownloaded(const string& resource_list_name);

    private:
        std::map<size_t, string> resources_on_server_;
        std::map<string, size_t> resources_size_;
        bool started_;

        size_t hit_sum_;
        std::string resource_list_;
        std::string resources_size_file_;
        boost::shared_ptr<boost::asio::io_service> io_service_;
    };
}

#endif //SUPER_NODE_RESOURCE_SELECTOR_H
