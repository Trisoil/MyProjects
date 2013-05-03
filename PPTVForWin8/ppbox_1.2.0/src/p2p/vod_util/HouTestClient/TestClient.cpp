//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "TestClient.h"
#include "AsioServiceRunner.h"
#include <framework/configure/Config.h>
#include <framework/timer/TickCounter.h>
#include "boost/date_time/posix_time/posix_time.hpp" //include all types plus i/oor
#include "boost/date_time/posix_time/posix_time_types.hpp" //no i/o just types
#include "string"

void TestClient::Start()
{
    framework::configure::Config conf("HouService.conf");
    conf.register_module("Hou")
        << CONFIG_PARAM_NAME_RDONLY("server_port", server_port_)
        << CONFIG_PARAM_NAME_RDONLY("server_address", server_address_)
        << CONFIG_PARAM_NAME_RDONLY("requst_url", request_url_);
        
        service_runner_.reset(new AsioServiceRunner("HouTestClient"));
        io_service_ = service_runner_->Start();
        socket_.reset(new boost::asio::ip::udp::socket(*io_service_));
        socket_->open(boost::asio::ip::udp::v4());
        timer_.reset(new boost::asio::deadline_timer(*io_service_));
        timer_->expires_from_now(boost::posix_time::seconds(1));
        timer_->async_wait(boost::bind(&TestClient::SendTo, shared_from_this()));
}

void TestClient::Stop()
{
    service_runner_->Stop();
}

static int count(0);
void TestClient::SendTo()
{
    boost::asio::ip::udp::endpoint end_point;
    end_point.port(server_port_);
    end_point.address(boost::asio::ip::address::from_string(server_address_));
    std::string request = request_url_ + " HTTP/1.1\r\nSID:1234567890FEDCBA\r\n\r\n";
    

#ifdef BOOST_WINDOWS_API
    int interval = 1000000 / send_num_in_1s_;
#else
    int interval = 1000000000 / send_num_in_1s_;
#endif
    int every_interval_send = 1;
    
    while (1)
    {
        int send_num = 0;
        
#ifdef BOOST_WINDOWS_API

        LARGE_INTEGER m_liPerfFreq={0};
        QueryPerformanceFrequency(&m_liPerfFreq);
        LARGE_INTEGER last_time={0};
        QueryPerformanceCounter(&last_time);
        LARGE_INTEGER current_time={0};

#else 
        timespec last_time;
        timespec current_time;
        clock_gettime(CLOCK_REALTIME, &last_time);

#endif
        while (send_num < send_num_in_1s_)
        {

#ifdef BOOST_WINDOWS_API
        
            QueryPerformanceCounter(&current_time);
            int time=( ((current_time.QuadPart - last_time.QuadPart) * 1000000)/m_liPerfFreq.QuadPart);
            if (time < interval)
                continue;

             last_time = current_time;
#else

            clock_gettime(CLOCK_REALTIME, &current_time);
            int duration = (current_time.tv_sec - last_time.tv_sec) * 1000000000 + (current_time.tv_nsec - last_time.tv_nsec);
            if (duration < interval)
            {
                continue;
            }
            
            last_time = current_time;
#endif
            

            for (int i = 0; i < every_interval_send; i++)
            {
                socket_->send_to(boost::asio::buffer(request.c_str(),request.length()), end_point);
                send_num++;
            }
        }

#ifdef BOOST_WINDOWS_API
        std::cout << current_time.QuadPart*1000 / m_liPerfFreq.QuadPart << " " << send_num << std::endl;
#else
        std::cout << current_time.tv_sec * 1000 + current_time.tv_nsec / 1000000 << " " << send_num << std::endl;
#endif
    }
}

int main()
{
    boost::uint32_t send_num_in_1s;
    std::cout << "Input request number send in 1 second: " << std::endl;
    std::cin >> send_num_in_1s;
    boost::shared_ptr<TestClient> test_client(new TestClient(send_num_in_1s));
    test_client->Start();
    
    while(1)
    {
        std::string command;
        std::cin >> command;
        if (command == "stop")
        {
            test_client->Stop();
            exit(0);
        }
        else if(command == "count")
        {
            std::cout << count << std::endl;
        }
    }

    return 0;
}