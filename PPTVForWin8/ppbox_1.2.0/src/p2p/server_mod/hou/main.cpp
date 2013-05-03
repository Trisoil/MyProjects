//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------
#include "Common.h"
#include "HouServer.h"
#include <iostream>

const string help_str("command list: \n" \
                      "help: show this help\n" \
                      "packet: show packet statistic\n"\
                      "status: show the status of udpserver\n"\
                      "version: show version\n"\
                      "exit: stop the udpserver\n\n");

int main()
{ 
    log4cplus::PropertyConfigurator::doConfigure("hou-logging.conf");
    if (!hou::HouServer::Inst().Start())
    {
        return -1;
    }

    std::cout << help_str << std::endl;

    do  
    {
        string command;
        std::cin >> command;
        if (command == "help")
        {
            std::cout << help_str;
        }
        else if (command == "packet")
        {
            std::cout << hou::HouServer::Inst().GetPacketString() << std::endl;
        }
        else if (command == "status")
        {
            std::cout << "Port: " << hou::HouServer::Inst().Port() << std::endl;
            std::cout << "Concurrent UDP Receiving Count: " << hou::HouServer::Inst().ConcurrentAsyncReceiveNum() << std::endl; 
            std::cout << "The Number Of Current Processing Udp Request: " << hou::HouServer::Inst().request_set_.size() << std::endl;
            std::cout << "The Length Of Current Udp Request Buffer: " << hou::HouServer::Inst().request_waiting_queue_.size() << std::endl;
        }
        else if (command == "version")
        {
            std::cout << "Http over udp server Version: " << hou::HouServer::Inst().GetVersion() << std::endl;
        }
        else if (command == "exit")
        {
            hou::HouServer::Inst().Stop();
            break;
        }
        else
        {
            std::cout << "invalid command" << std::endl;
            std::cout << help_str << std::endl;
        }
    } while (true);

    hou::HouServer::TearDown();

    return 0;
}