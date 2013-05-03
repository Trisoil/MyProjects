//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef LIVE_MEDIA_TEST_APP_H
#define LIVE_MEDIA_TEST_APP_H

namespace live_media_test
{
    class TestClient;

    class App
    {
    public:
        int Run();

    private:
        void HandleConsoleInput();
        static void MainIoServiceThread(boost::shared_ptr<boost::asio::io_service> io_service);

    private:
        boost::shared_ptr<TestClient> test_client_;
    };
}

#endif //LIVE_MEDIA_TEST_APP_H