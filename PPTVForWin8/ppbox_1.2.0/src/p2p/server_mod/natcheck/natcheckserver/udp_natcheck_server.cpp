#include "Common.h"
#include "MainThread.h"

#include <boost/asio/ip/udp.hpp>

#include "protocol/UdpServer.h"
#include "UdpNatCheckModule.h"

#include <boost/asio/ip/udp.hpp>

using namespace framework;
using namespace udpnatcheck;

log4cplus::Logger g_logger = log4cplus::Logger::getInstance("g_natcheck");
log4cplus::Logger g_statlog = log4cplus::Logger::getInstance("g_statlog");
const std::string NATCHECK_VERSION = "0.0.0.1";

int main(int argc, char* argv[])
{    
    std::string config_file_name = "udp_natcheck_server.config.txt";

    if (argc == 2)
    {
        config_file_name = argv[1];
        printf("Load config %s\n.", config_file_name.c_str());
    }

    if ( Config::Inst().LoadConfig(config_file_name) == false )
    {
        printf("Failed to Load Config file %s\n.", config_file_name.c_str());
        return 0;
    }
 
    std::string log_conf = Config::Inst().GetTString("natcheck.logconf","natcheck_log.conf");

    log4cplus::PropertyConfigurator::doConfigure(log_conf); 

    LOG4CPLUS_DEBUG(g_logger, "load config file success " << config_file_name);

    MainThread::Inst().Start(); 

    MainThread::IOS().post(boost::bind(&UdpNatCheckModule::Start, UdpNatCheckModule::Inst()));

    printf("start server,version:%s\n",NATCHECK_VERSION.c_str());
     
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
            MainThread::IOS().post(boost::bind(&UdpNatCheckModule::PrintNetInfo, UdpNatCheckModule::Inst(), false));
        }        
    }
 
    MainThread::IOS().post(boost::bind(&UdpNatCheckModule::Stop, UdpNatCheckModule::Inst()));

    MainThread::Inst().Stop();
    return 0;
}
