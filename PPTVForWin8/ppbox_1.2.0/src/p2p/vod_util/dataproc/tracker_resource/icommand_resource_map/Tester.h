#pragma once

#include "stdafx.h"
#include <framework/timer/AsioTimerManager.h>

#define MAX_MSG_PER_SECOND 1

class Tester : protocol::IUdpServerListener
{
public:
	Tester( std::vector <string> * ips , std::vector <int> *ports );
    ~Tester(void);

    bool start();
    bool stop();


private:

    void OnUdpRecv(protocol::Packet const & packet);
    void on_timer();

private:
    protocol::UdpServer * udp_server_;
    boost::asio::io_service io_service_;

	std::vector <string> * ips_;
	std::vector <int> *ports_;

    unsigned short local_udp_port_;
	int msg_index_;
	int end_test_;

    //this is for timeout detection.
	int test_start_time_;
    int retry_count_;

    framework::timer::AsioTimerManager * asio_timer_manager_;
    framework::timer::PeriodicTimer::pointer tests_timer_;

};
