#include "Common.h"
#include "Resource.h"
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include "MainThread.h"
#include "UdpTrackerAgModule.h"

using namespace framework;
using namespace udptrackerag;
using namespace std;

log4cplus::Logger g_logger = log4cplus::Logger::getInstance("g_tracker_ag");
log4cplus::Logger g_stat_logger = log4cplus::Logger::getInstance("g_trackerag_stat");

std::string g_config_file_name = "udp_trackerag.config.txt";

int main(int argc, char* argv[])
{
    
    if (argc == 2)
    {
        g_config_file_name = argv[1];
        printf("Load config %s\n.", g_config_file_name.c_str());
    }
    if ( Config::Instance().LoadConfig(g_config_file_name) == false )
    {
        printf("Failed to Load Config file %s\n.", g_config_file_name.c_str());
        return -1;
    }

    std::string log_conf = Config::Instance().GetTString("trackerag.logconf","trackerag_log.conf");

    log4cplus::PropertyConfigurator::doConfigure(log_conf); 


    LOG4CPLUS_DEBUG(g_logger, "load config file success " << g_config_file_name);


    CONSOLE_OUTPUT("================ PPVA Tracker Access Gateway (" << TRACKER_AG_VERSION_STR << ") ================");

    MainThread::Instance().Start();

    CONSOLE_LOG("已经启动主线程...");

    u_short udp_port = Config::Instance().GetInteger("trackerag.udp_port");

    MainThread::IOS().post(boost::bind(&UdpTrackerAgModule::Start, UdpTrackerAgModule::Instance(), udp_port));
    while(true)
    {
        std::string cmd;
        cin>>cmd;
        if(cmd == "exit")
        {
            break;
        }
    }
    MainThread::Instance().Stop();
    CONSOLE_LOG("主线程已经关闭...");   
    return 0;
}