//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "App.h"
#include "TestClient.h"
#include "RequestSender.h"
#include "RequestsQueue.h"

namespace live_media_test
{
    void App::HandleConsoleInput()
    {
        while ( true ) 
        {
            std::string command;
            std::cin >> command;
            if ( command == "exit" ) 
            {
                if (test_client_)
                {
                    test_client_->Stop();
                    test_client_.reset();
                }

                break;
            }

            if (command == "status") 
            {
                std::cout << "status - TBD" << std::endl;
                continue;
            }
            if (command == "help") 
            {
                std::cout << "help - TBD" << std::endl;
                continue;
            } 

            std::cout << "bad command: " << command << std::endl;
        }
    }

    void App::MainIoServiceThread(boost::shared_ptr<boost::asio::io_service> io_service)
    {
        boost::asio::io_service::work work(*io_service);
        io_service->run();
    }

    int App::Run()
    {
        boost::shared_ptr<boost::asio::io_service> io_service(new boost::asio::io_service());
        boost::thread ios_thread(boost::bind(&App::MainIoServiceThread, io_service));

        test_client_.reset(new TestClient(io_service));
        test_client_->Start();

        HandleConsoleInput();

        io_service->stop();
        ios_thread.join();

        return 0;
    }
}