//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "push_server.h"
#include "task/asio_thread.h"
#include "task/content_matcher.h"
#include "task/content_quota_scheduler.h"

#include "type_compatible.h"

#include <iostream>

#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

using namespace push_server;
using namespace std;

const string g_pushtype_str[] = {"\"Generic\"", "\"OpenService\""};
const string push_version("2.0.7");

FRAMEWORK_LOGGER_DECLARE_MODULE("main");

void StartServer()
{
	printf("*******************************************************************************\n");
	printf("                          PPVA Push Server %s\n", push_version.c_str());
    printf("*******************************************************************************\n");

    main_thread()->Start();
	main_thread()->Ios().post(boost::bind(&PushServer::Start, PushServer::Inst(), "push_server.conf"));

	const string cmd_list_str = 
		"+-------------------------+------------------------------------------+\n"
		"|  Command                |             Info.                        |\n"
		"+-------------------------+------------------------------------------+\n"
		"|  exit/quit              |  Quit                                    |\n"
		"|  help/?                 |  List command list                       |\n"
		"|  loadconfig             |  Load Config File                        |\n"
		"|  config                 |  Show ServerConfig                       |\n"
		"|  packet/pakt            |  Show Packet Statistics                  |\n"
		"+-------------------------+------------------------------------------+\n"
		;

	std::string cmd_line;
	while (true)
	{
		getline(cin, cmd_line);
		std::istringstream iss(cmd_line);
		std::string cmd, param;
		iss>>cmd>>param;

		boost::algorithm::to_lower(cmd);
		if ("exit" == cmd || "quit" == cmd)
		{
			DWORD wait_sec = 1;
			printf("Closing, wait for %u sec..\n", wait_sec);
			main_thread()->Ios().post(boost::bind(&PushServer::Stop,
				PushServer::Inst()));

            boost::this_thread::sleep(boost::posix_time::milliseconds(wait_sec * 1000));
		}
		else if ("?" == cmd || "help" == cmd)
		{
			printf("%s", cmd_list_str.c_str());
		}
		else if ("load_config" == cmd || "loadconfig" == cmd)
		{
			main_thread()->Ios().post(boost::bind(&PushServer::LoadConfig, 
				PushServer::Inst()));
		}
		else if ("config" == cmd)
		{
			main_thread()->Ios().post(boost::bind(&PushServer::ShowConfig, 
				PushServer::Inst()));
		}
		else if ("packet" == cmd || "pakt" == cmd)
		{
			main_thread()->Ios().post(boost::bind(&PushServer::ShowPaktStat,
				PushServer::Inst()));
		}
		else
		{
   //         printf("Invalid command, here is the list of valid commands,cmd is:%s\n",cmd.c_str());
			//printf("%s", cmd_list_str.c_str());
		}
	}

	CONSOLE_LOG("Closing PullServer");

    reset_global_second_timer();
	//Log::Inst().Stop();
	main_thread()->Stop();
}

log4cplus::Logger g_logger = log4cplus::Logger::getInstance("g_pushserver");
log4cplus::Logger g_statlog = log4cplus::Logger::getInstance("g_stat");
log4cplus::Logger g_statistics = log4cplus::Logger::getInstance("g_statistics");
log4cplus::Logger g_vippush = log4cplus::Logger::getInstance("g_vippush");


int main()
{
	//TestTaskReader();
#ifndef TEST_CONTENT_MANAGER
    StartServer();
#else
    ContentManager cm;
    cm.TestContentManager();
#endif
	return 0;
}