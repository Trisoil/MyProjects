#include "stdafx.h"
#include "Logger.h"
#include "NotifyServerModule.h"
#include "MainThread.h"
#include "framework/configure/Config.h"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include <log4cplus/configurator.h>

// using namespace framework;
using namespace NotifyServer;

int main(int argc, char* argv[])
{
    const std::string Version = "1.0.0.7";

    boost::filesystem::path path = boost::filesystem::current_path() / NotifySvrModule::ConfigFileName;

    if (!boost::filesystem::exists(path))
    {
        std::cout << "Failed to Load Config file " << NotifySvrModule::ConfigFileName << std::endl;
        return 0;
    }

    std::cout << "================ NotifyServer 服务器 (" << Version << ") ================" << std::endl;

    MainThread::Inst().Start();
    std::cout << " 已经启动主线程 " << std::endl;

    log4cplus::PropertyConfigurator::doConfigure("NotifyServer-logging.conf");

    framework::configure::Config conf(NotifySvrModule::ConfigFileName);

    u_short udp_port, http_svr_port;
    conf.register_module("NotifyServer")
        << CONFIG_PARAM_NAME_RDONLY("udp_port", udp_port)
        << CONFIG_PARAM_NAME_RDONLY("http_svr_port", http_svr_port);

    MainThread::IOS().post(boost::bind(&NotifySvrModule::Start, NotifySvrModule::Inst(), udp_port,http_svr_port));

    string help_text = 
        "  help              输出本帮助\n"
        "  exit              退出程序\n"
        "  PeerMgrCount      输出当前服务器管理的Peer个数（第一层节点个数)\n"
        "  PeerOnline        输出当前用户汇报的在线Peer的个数\n"
        "  PeerComplete      输出某个任务用户汇报的当前的完成个数\n"
        "  TaskCount         输出当前正在运行的任务个数\n"
        "  TaskInfo          输出所有任务的详细信息\n"
        "  LoadConfig        重新加载配置文件\n"
        "  Version           显示版本信息\n"
        "  TestTask          测试一个任务\n"
        "  ReleaseTask       删除一个已经存在的任务\n"
        "  1                 输入1就查看在线人数\n"
        "  2                 输入2就查看任务，包括任务ID、任务类型、到期时间、完成数\n"
        "  3                 输入3就查看节点数量，包括内网节点数量、公网节点数量\n"
        "  4                 输入4就查看内网节点队列\n"
        "  5                 输入5就查看公网节点队列\n"
        ;
    std::cout << help_text << std::endl;
    while(true)
    {
        string command;
        cin >> command;
        std::cout << std::endl;
        if (command == "1")
        {
            MainThread::IOS().post(boost::bind(&NotifySvrModule::ConsoleLogListPeerReportCount, NotifySvrModule::Inst()));
        }
        else if (command == "2")
        {
            MainThread::IOS().post(boost::bind(&NotifySvrModule::ConsoleLogListAllTask, NotifySvrModule::Inst()));
        }
        else if (command == "3")
        {
            MainThread::IOS().post(boost::bind(&NotifySvrModule::ConsoleLogListAllPeerNum, NotifySvrModule::Inst()));
        }
        else if (command == "4")
        {
            MainThread::IOS().post(boost::bind(&NotifySvrModule::ConsoleLogListNatPeerInfo, NotifySvrModule::Inst()));
        }
        else if (command == "5")
        {
            MainThread::IOS().post(boost::bind(&NotifySvrModule::ConsoleLogListPublicPeerInfo, NotifySvrModule::Inst()));
        }
        else if( command == "exit" )
        {
            std::cout << "确定要退出Notify服务器吗？[y/n]" << std::endl;
            cin >> command;
            boost::algorithm::to_lower(command);
            if (command == "y" || command == "yes")
            {
                MainThread::IOS().post(boost::bind(&NotifySvrModule::Stop, NotifySvrModule::Inst()));
                break;
            }
            else
            {
                std::cout << " 服务器没有退出 " << std::endl;
            }
        }
        else if( command == "help")
        {
            std::cout << help_text << std::endl;
        }
        else if (command == "PeerMgrCount")
        {
            MainThread::IOS().post(boost::bind(&NotifySvrModule::ConsoleLogListPeerGuidCount, NotifySvrModule::Inst()));
        }
        else if(command == "PeerOnline")
        {
            MainThread::IOS().post(boost::bind(&NotifySvrModule::ConsoleLogListPeerReportCount, NotifySvrModule::Inst()));
        }
        else if(command == "PeerComplete")
        {
            std::cout << "输入要查看的TaskID" << std::endl;
            cin>>command;
            if(!command.empty())
            {
                boost::uint32_t tid = strtoul(command.c_str(), NULL ,10);
                MainThread::IOS().post(boost::bind(&NotifySvrModule::ConsoleLogListPeerReportCompleteCount, NotifySvrModule::Inst(), tid));
            }
        }
        else if (command == "TaskCount")
        {
            MainThread::IOS().post(boost::bind(&NotifySvrModule::ConsoleLogListTaskCount, NotifySvrModule::Inst()));
        }
        else if (command == "TaskInfo")
        {
            std::cout << "输入要查看的TaskID" << std::endl;
            cin>>command;
            if(!command.empty())
            {
                boost::uint32_t tid = strtoul(command.c_str(), NULL ,10);
                MainThread::IOS().post(boost::bind(&NotifySvrModule::ConsoleLogListTaskInfo, NotifySvrModule::Inst(), tid));
            }
        }
        else if (command == "Version")
        {
            std::cout << "Version: " << Version << std::endl;
        }
        else if (command == "TestTask")
        {
            boost::uint32_t tid;
            string content;
            boost::uint32_t duration;
            boost::uint32_t resttime;
            boost::uint32_t type;
            vector<char> data_vec;
            std::cout << "输入要测试的TaskID" << std::endl;
            cin>>command;
            if(!command.empty())
            {
                tid = strtoul(command.c_str(), NULL ,10);
            }
            std::cout << " 输入Task的任务内容 " << std::endl;
            cin >> command;
            if(!command.empty())
            {
                content = command;
                bool is_read_ok = false;
                if(content == "data.dat")
                {
                    do 
                    {
                        string the_dat = "data.dat";
                        FILE* fp = fopen(the_dat.c_str(), "rb");
                        if(!fp)
                            break;
                        is_read_ok = true;
                        fseek(fp, 0, SEEK_END);
                        long len = ftell(fp);
                        data_vec.resize(len);
                        fseek(fp, 0, SEEK_SET);
                        fread((void*)(&data_vec[0]), 1, len, fp);
                        fclose(fp);
                    } while (false);
                }
                if(is_read_ok == false)
                {
                    data_vec.resize(content.length());
                    memcpy((void*)(&data_vec[0]), content.c_str(), content.length());
                }

            }
            std::cout << " 输入Task的duration时间，单位是秒 " << std::endl;
            cin >> command;
            if(!command.empty())
            {
                duration = strtoul(command.c_str(), NULL ,10);
            }
            std::cout << "输入Task的持续时间，单位是秒" << std::endl;
            cin >> command;
            if(!command.empty())
            {
                resttime = strtoul(command.c_str(), NULL ,10);
            }
            std::cout << " 输入任务的种类，0表示文本，其他为非文本 " << std::endl;
            cin >> command;
            if(!command.empty())
            {
                type = strtoul(command.c_str(), NULL ,10);
            }
            MainThread::IOS().post(boost::bind(&NotifySvrModule::TestATask, NotifySvrModule::Inst(),
                tid, data_vec, resttime,duration, type));
            std::cout << "测试任务已经发出" << std::endl;
        }
        else if(command == "ReleaseTask")
        {
            std::cout << "输入要取消的TaskID" << std::endl;
            boost::uint32_t tid;
            cin>>command;
            if(!command.empty())
            {
                tid = strtoul(command.c_str(), NULL ,10);
            }
            MainThread::IOS().post(boost::bind(&NotifySvrModule::ConsoleReleaseTask, NotifySvrModule::Inst(), tid));
            std::cout << "已经取消对应任务" << std::endl;
        }
        else if (command == "LoadConfig")
        {
            MainThread::IOS().post(boost::bind(&NotifySvrModule::LoadConfig, NotifySvrModule::Inst()));
            std::cout << "Config Reloaded." << std::endl;
        }
    }

    MainThread::Inst().Stop();
    std::cout << " 主线程已经关闭 " << std::endl;

    std::cout << " 正在关闭日志线程 " << std::endl;

    return 0;
}
