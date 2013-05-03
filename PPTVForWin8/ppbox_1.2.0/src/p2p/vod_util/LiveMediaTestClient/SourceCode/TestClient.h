//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef LIVE_MEDIA_TEST_TEST_CLIENT_H
#define LIVE_MEDIA_TEST_TEST_CLIENT_H

namespace live_media_test
{
    class ResourceSelector;
    class RequestGenerator;
    class RequestSender;
    class RequestsQueue;

    class TestClient
    {
    public:
        TestClient(boost::shared_ptr<boost::asio::io_service> io_service);

        void Start();
        void Stop();
        
    private:
        boost::shared_ptr<boost::asio::io_service> io_service_;
        boost::shared_ptr<RequestGenerator> request_generator_;
        boost::shared_ptr<RequestSender> request_sender_;
        boost::shared_ptr<RequestsQueue> request_queue_;
        boost::shared_ptr<ResourceSelector> resource_selector_;
    };
}
#endif //LIVE_MEDIA_TEST_TEST_CLIENT_H