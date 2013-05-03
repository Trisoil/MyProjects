#pragma once

#include <framework/timer/AsioTimerManager.h>

class Tester : protocol::IUdpServerListener
{
public:
    Tester( char * ip , unsigned short port );
    ~Tester(void);

    bool start();
    bool stop();

    // -1: 失败
    // 0 : 未知
    // 1 : 成功
    int test_result_;

private:

    void OnUdpRecv(protocol::Packet const & packet);
    void on_timer();

    enum
    {
        want_test_report ,
        testing_report ,
        want_test_list ,
        testing_list ,
        want_test_icommand,
        testing_icommand,
        ending_test
    } test_status_;

private:
    protocol::UdpServer * udp_server_;
    boost::asio::io_service io_service_;

    char * test_tracker_ip_;
    unsigned short test_tracker_port_;

    unsigned short local_udp_port_;

    // 用来检测是否超时
    DWORD test_start_time_;
    int retry_count_;
    Guid my_guid_;

    framework::timer::AsioTimerManager * asio_timer_manager_;
    framework::timer::PeriodicTimer::pointer tests_timer_;

    std::set<RID> local_resource_set_;
    std::vector<protocol::REPORT_RESOURCE_STRUCT> update_resource_set_;
};
