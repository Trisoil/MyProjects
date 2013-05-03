//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_TEST_APP_H
#define SUPER_NODE_TEST_APP_H

namespace super_node_test
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

#endif //SUPER_NODE_TEST_APP_H