#include "stdafx.h"
#include "framework/io/Path.h"
#include "framework/mswin/module.h"
#include "udp_index_cleaner/UdpCleanerModule.h"
#include "udp_index_cleaner/LogThread.h"
#include "udp_index_cleaner/MergeThread.h"

using namespace udplog;

int _tmain(int argc, _TCHAR* argv[])
{
	tstring config_file_name = TEXT("udp_index_cleaner_config.txt");
    framework::mswin::Module module;
    framework::io::Path config_path;
    module.BuildLocalFilePath(config_path, config_file_name.c_str());
	if ( Config::Inst().LoadConfig(config_path.GetString()) == false )
	{
		CONSOLE_OUTPUT("Failed to Load Config file " << config_file_name);
		return 1;
	}


    Log::Inst().Start();
    Log::Inst().SetLogLevel(__DEBUG);

    MainThread::Inst().Start();
    LogThread::Inst().Start();
    MergeThread::Inst().Start();
	CONSOLE_LOG("已经启动主线程。。。");


	u_short udp_port = Config::Inst().GetInteger("cleaner.udp_port");

	MainThread::IOS().post(boost::bind(&UdpCleanerModule::Start,UdpCleanerModule::Inst(), udp_port));

	string help_text = 
        "============================ help ============================"
		"  help                 输出本帮助\n"
        "  load                 重新加载配置\n"
        "  print                打印内存中的内容\n"
		"  exit                 退出程序\n"
        "===============================================================";

	while (true)
	{
        const size_t bsize = 1024;
        char buff[bsize];
        cin.getline(buff,bsize);
        string command = buff;
		boost::algorithm::to_lower(command);
		if( command == "exit" )
		{
            break;
		}
        else if( command == "load" )
        {
            if ( Config::Inst().LoadConfig(config_path.GetString()) == false )
            {
                CONSOLE_OUTPUT("Failed to Load Config file " << config_file_name);
            }
        }
        else if( command == "print" )
        {
            MainThread::IOS().post(boost::bind(&UdpCleanerModule::OnPrintMem,UdpCleanerModule::Inst()));
        }
		else 
		{
			CONSOLE_OUTPUT(help_text);
		}
	}


    MainThread::IOS().post(boost::bind(&UdpCleanerModule::Stop,UdpCleanerModule::Inst()));
    //UdpCleanerModule::Inst()->Stop();

	MainThread::Inst().Stop();
    LogThread::Inst().Stop();
    MergeThread::Inst().Stop();
	CONSOLE_OUTPUT("主线程已经关闭");
    Log::Inst().Stop();
}
