#include "Common.h"
#include "MainThread.h"

#include <boost/asio/ip/udp.hpp>

#include "protocol/UdpServer.h"
#include "UdpStunModule.h"

#include <boost/asio/ip/udp.hpp>

using namespace framework;
using namespace udpstun;
FRAMEWORK_LOGGER_DECLARE_MODULE("StunServer");
int main(int argc, char* argv[])
{    
    framework::configure::Config conf("log.conf");
    framework::logger::glog.load_config(conf);

    std::string config_file_name = "udp_stun_server.config.txt";

    if ( Config::Inst().LoadConfig(config_file_name) == false )
    {
        printf("Failed to Load Config file %s\n.", config_file_name.c_str());
        return 0;
    }
 
     LOG(__INFO, "Stun", "config loaded");

    // 启动 主线程 读取TrackerList配置线程 数据库线程池
     MainThread::Inst().Start();

    LOG(__INFO, "Stun", "threads started");

    u_short udp_port = Config::Inst().GetInteger("stun.udp_port", 7000);
    if (udp_port == 0)
    {
        CONSOLE_LOG("The UDP Port is a Zero!!!!!");
        udp_port = 7000;
    }

    MainThread::IOS().post(boost::bind(&UdpStunModule::Start, UdpStunModule::Inst(), udp_port));
     
    while (true)
    {
        std::string commond_line;
        cin >> commond_line;
        boost::algorithm::to_lower(commond_line);
        if (commond_line == "exit")
        {
            break;
        }
        else if (commond_line == "netinfo")
        {
            MainThread::IOS().post(boost::bind(&UdpStunModule::PrintNetInfo, UdpStunModule::Inst(), false));
        }
        else
        {
            CONSOLE_OUTPUT("=========================== help ==============================");
            CONSOLE_OUTPUT("    NetInfo ---------------------------Print Net Info");
            CONSOLE_OUTPUT("    Exit ------------------------------Exit");
            CONSOLE_OUTPUT("===============================================================");
        }
    }
 
    MainThread::IOS().post(boost::bind(&UdpStunModule::Stop, UdpStunModule::Inst()));

     MainThread::Inst().Stop();

    LOG(__INFO, "Stun", "it\'s the end");

    return 0;
}
