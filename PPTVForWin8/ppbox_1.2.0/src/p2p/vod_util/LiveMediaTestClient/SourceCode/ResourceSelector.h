//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef LIVE_MEDIA_TEST_RESOURCESELECTOR_H
#define LIVE_MEDIA_TEST_RESOURCESELECTOR_H

namespace live_media_test
{
    class Resource
    {
    public:
        Resource(const string& channel_id)
            : channel_id_(channel_id)
        {
        }

        string GetChannelId() const { return channel_id_; }

    private:
        string channel_id_;
    };

    class ResourceSelector
        : public boost::enable_shared_from_this<ResourceSelector>
    {
    public:
        ResourceSelector(boost::shared_ptr<boost::asio::io_service> io_service)
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

    private:
        bool started_;

        std::vector<std::string> channels_;
        std::vector<std::size_t> channels_visit_frequency_;

        boost::shared_ptr<boost::asio::io_service> io_service_;
    };
}

#endif //LIVE_MEDIA_TEST_RESOURCESELECTOR_H