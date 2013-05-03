#include "Common.h"

#include "udptracker/TrackerLogger.h"
#include "udptracker/UdpTrackerModule.h"
#include "udptracker/IpLocater.h"
#include "server_mod/api/utility/util.h"

#include "Resource.h"
#include <framework/logger/Logger.h>
#include <framework/configure/Config.h>


#ifdef WIN32
#include "MiniDump.h"
#endif
#include "MainThread.h"

using namespace framework;
using namespace udptracker;
using namespace std;

#ifdef DEFINE_OWN_UUID 

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>


    typedef unsigned char uuid_t[16];

    void
        uuid_generate(uuid_t out)
    {
        srand(time(NULL));
        for (size_t i = 0; i < sizeof(uuid_t); ++i)
        {
            out[i] = rand()%256;
        }
    }

#ifdef __cplusplus
}
#endif

#endif

log4cplus::Logger g_logger = log4cplus::Logger::getInstance("g_tracker");
log4cplus::Logger g_stat_logger = log4cplus::Logger::getInstance("g_stat"); 
log4cplus::Logger g_rid_peer_logger = log4cplus::Logger::getInstance("g_rid_peer");


int main(int argc, char* argv[])
{
#ifdef WIN32
    SetUnhandledExceptionFilter(CreateMiniDump);
#endif

    std::string config_file_name = "udp_tracker_server.config.txt";
    if (argc == 2)
    {
        config_file_name = argv[1];
        printf("Load config %s\n.", config_file_name.c_str());
    }
    if ( Config::Inst().LoadConfig(config_file_name) == false )
    {
        printf("Failed to Load Config file %s\n.", config_file_name.c_str());
        return -1;
    }

    std::string log_conf = Config::Inst().GetTString("tracker.logconf","tracker_log.conf");

    log4cplus::PropertyConfigurator::doConfigure(log_conf); 

    LOG4CPLUS_DEBUG(g_logger, "load config file success " << config_file_name);

    //framework::configure::Config conf("tracker.conf");
    //framework::logger::glog.load_config(conf);


    CONSOLE_OUTPUT("================ PPVA Tracker 服务器 (" << TRACKER_VERSION_STR << ") ================");

    MainThread::Inst().Start();

    CONSOLE_LOG("已经启动主线程...");

    unsigned dump_file_number = Config::Inst().GetInteger("tracker.dump_file_number");

    u_short udp_port = Config::Inst().GetInteger("tracker.udp_port");

	//绑定cpu的配置文件
	bool is_bind = Config::Inst().GetBoolean("tracker.is_bind", true);
	bool is_special = Config::Inst().GetBoolean("tracker.is_special", false);

	int cpu_id = Config::Inst().GetInteger("tracker.cpu_id", 0);

	//绑定到特定的CPU
	if (is_bind)
	{
		if (is_special)
		{
			MainThread::IOS().post(boost::bind(&ns_pplive::utility::BindCPU, cpu_id));
		}
		else
		{
			MainThread::IOS().post(boost::bind(&ns_pplive::utility::BindCPU, udp_port + 1));
		}
	}

    MainThread::IOS().post(boost::bind(&UdpTrackerModule::Start, UdpTrackerModule::Inst(), udp_port));

    std::string help_text = 
        "  help/?            输出本帮助\n"
        "  exit              退出程序\n"
        "  PeerCount         输出当前在线Peer个数\n"
        "  RIDCount          输出当前在线资源个数\n"
        "  list PeerGuid     输出所有Peer的PID\n"
        "  list RID          输出所有资源的RID\n"
        "  PeerGuid <Guid>   显示<Guid>对应Peer的资源拥有信息\n"
        "  RID <RID>         显示<RID>对应的资源信息\n"
        "  ShowEmptyPeer     显示拥有空资源的Peer\n"
        "  ShowConfigInfo    显示配置文件信息\n"
        "  Statistic         显示统计信息\n"
        "  DumpResources     输出所有资源信息\n"
        "  DumpPeers         输出所有Peer信息\n"
        "  Dump on/off/show  开启/关闭/查看Dump状态\n"
        "  DelayDump <Sec>   统计时间<Sec>秒后\n"
        "  LoadConfig        重新加载配置文件\n"
        "  Version           显示版本信息\n"
        ;

    while(true)
    {
        //      sleep( 10000 );
        std::string command;
        cin >> command;
        CONSOLE_LOG("");
        if( command == "exit" )
        {
            CONSOLE_OUTPUT("确定要退出Tracker服务器吗？[y/n]");
            cin >> command;
            boost::algorithm::to_lower(command);
            if (command == "y" || command == "yes")
            {
                MainThread::IOS().post(boost::bind(&UdpTrackerModule::Stop, UdpTrackerModule::Inst()));
                break;
            }
            else
            {
                CONSOLE_OUTPUT("服务器没有退出 ");
            }
        }
        else if (command == "PeerCount")
        {
            MainThread::IOS().post(boost::bind(&UdpTrackerModule::ConsoleLogListPeerGuidCount, UdpTrackerModule::Inst()));
        }
        else if (command == "RIDCount")
        {
            MainThread::IOS().post(boost::bind(&UdpTrackerModule::ConsoleLogListRIDCount, UdpTrackerModule::Inst()));
        }
        else if (command == "list")
        {
            cin>>command;
            if (command == "PeerGuid")
            {
                MainThread::IOS().post(boost::bind(&UdpTrackerModule::ConsoleLogListPeerGuid, UdpTrackerModule::Inst()));
            }
            else if (command == "RID")
            {
                MainThread::IOS().post(boost::bind(&UdpTrackerModule::ConsoleLogListRID, UdpTrackerModule::Inst()));
            }
        }
        else if (command == "PeerGuid")
        {
            cin>>command;
            Guid peer_guid;
            if (!peer_guid.from_string(command))
            {
                MainThread::IOS().post(boost::bind(&UdpTrackerModule::ConsoleLogPeerGuid, UdpTrackerModule::Inst(), peer_guid));
            }
        }
        else if (command == "RID")
        {
            cin>>command;
            RID rid;
            if (!rid.from_string(command))
            {
                MainThread::IOS().post(boost::bind(&UdpTrackerModule::ConsoleLogRID, UdpTrackerModule::Inst(), rid));
            }
        }
        else if (command == "ShowEmptyPeer")
        {
            MainThread::IOS().post(boost::bind(&UdpTrackerModule::ConsoleLogShowEmptyPeer, UdpTrackerModule::Inst()));
        }
        else if (command == "ShowConfigInfo")
        {
            MainThread::IOS().post(boost::bind(&UdpTrackerModule::ConsoleLogShowConfig, UdpTrackerModule::Inst()));
        }
        else if (command == "Statistic")
        {
            MainThread::IOS().post(boost::bind(&UdpTrackerModule::ConsoleLogShowStatistic, UdpTrackerModule::Inst()));
        }
        else if (command == "DumpResources")
        {
            MainThread::IOS().post(boost::bind(&UdpTrackerModule::DumpResources, UdpTrackerModule::Inst()));
        }
        else if (command == "DumpPeers")
        {
            MainThread::IOS().post(boost::bind(&UdpTrackerModule::DumpPeers, UdpTrackerModule::Inst()));
        }
        else if (command == "Version")
        {
            CONSOLE_OUTPUT("Version: " << TRACKER_VERSION_STR);
        }
        else if (command == "Dump")
        {
            cin >> command;
            if (command == "on")
                MainThread::IOS().post(boost::bind(&UdpTrackerModule::SetDumpOn, UdpTrackerModule::Inst(), true));
            else if (command == "off")
                MainThread::IOS().post(boost::bind(&UdpTrackerModule::SetDumpOn, UdpTrackerModule::Inst(), false));
            else if (command == "show")
                MainThread::IOS().post(boost::bind(&UdpTrackerModule::ConsoleLogDumpStatus, UdpTrackerModule::Inst()));
        }
        else if (command == "CheckResources")
        {
            cin >> command;
            if (command == "on")
                MainThread::IOS().post(boost::bind(&UdpTrackerModule::SetCheckResources, UdpTrackerModule::Inst(), true));
            else if (command == "off")
                MainThread::IOS().post(boost::bind(&UdpTrackerModule::SetCheckResources, UdpTrackerModule::Inst(), false));
            else if (command == "show")
                MainThread::IOS().post(boost::bind(&UdpTrackerModule::ConsoleLogCheckResourcesStatus, UdpTrackerModule::Inst()));
        }
        else if (command == "LoadConfig")
        {
            MainThread::IOS().post(boost::bind(&Config::LoadConfig, boost::ref(Config::Inst()), config_file_name));
            MainThread::IOS().post(boost::bind(&UdpTrackerModule::LoadSampleResources, UdpTrackerModule::Inst()));
            CONSOLE_OUTPUT("Config Reloaded.");
        }
        else if (command == "DelayDump")
        {
            cin >> command;
            try {
                unsigned seconds = boost::lexical_cast<unsigned>(command);
                MainThread::IOS().post(boost::bind(&UdpTrackerModule::DelayDump, UdpTrackerModule::Inst(), seconds));
            } catch (boost::bad_lexical_cast& e) {
                CONSOLE_OUTPUT("Error: " << e.what());
            }
        }
        else
        {
            CONSOLE_OUTPUT(help_text);
        }
    }

    MainThread::Inst().Stop();
    CONSOLE_LOG("主线程已经关闭...");

    CONSOLE_LOG("正在关闭日志线程···");

    return 0;
}


