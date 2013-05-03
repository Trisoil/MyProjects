#include "stdafx.h"
#include "base/guid.h"
#include "framework/MainThread.h"
#include "framework/Log.h"
#include "framework/io/Path.h"

#include "udpindex/Config.h"
#include "udpindex/UdpIndexModule.h"
#include "udpindex/TrackerListReadThread.h"
#include "udpindex/DataBaseThreadPoolAdd.h"
#include "udpindex/DataBaseThreadPoolQuery.h"
#include "udpindex/DataBaseConnectionPool.h"
#include "protocal/IndexPacket.h"
#include "udpindex/UrlVote.h"

#include "IndexFunc.h"

using namespace framework;
using namespace udpindex;

int _tmain(int argc, _TCHAR* argv[])
{
	framework::Log::Inst().Start();
	framework::Log::Inst().SetLogLevel(__DEBUG);
	framework::Log::Inst().SetConsoleLevel(__ERROR);
	framework::Log::Inst().SetLogTypeOn("Index");
	framework::Log::Inst().SetConsoleTypeOn("Index");
	
	tstring moduleName = TEXT("udp_index_server.config.txt"); 
	framework::mswin::Module module;
	framework::io::Path config_path;
	module.BuildLocalFilePath(config_path, moduleName.c_str());
	if(Config::Inst().LoadConfig(config_path.GetString()) == false)
	{
		CONSOLE_OUTPUT("Failed to open the config file " << moduleName.c_str());
		return 0;
	}

	LOG(__INFO, "Index", "config loaded");

	// 启动 主线程 读取TrackerList配置线程 数据库线程池
	MainThread::Inst().Start();
	TrackerListReadThread::Inst().Start();
	DataBaseThreadPoolAdd::Inst().Start();
	DataBaseThreadPoolQuery::Inst().Start();

	DataBaseConnectionPool::Inst().Start();

	LOG(__INFO, "Index", "threads started");

	u_short udp_port = Config::Inst().GetInteger("index.udp_port");
	if (udp_port == 0)
	{
		RELEASE_LOG("The UDP Port is a Zero and It is Set as 4000");
		udp_port = 4000;
	}

	MainThread::IOS().post(boost::bind(&UdpIndexModule::Start, UdpIndexModule::Inst(), udp_port));

	while(true)
	{
		char cmd_c_str[1024];
		cin.getline(cmd_c_str, 1024);
		string cmd_str(cmd_c_str);
		boost::algorithm::trim(cmd_str);
		boost::algorithm::to_lower(cmd_str);
		boost::regex reg("\\s+", boost::regex::icase|boost::regex::perl);
		cmd_str = boost::regex_replace(cmd_str, reg, " ");

		vector<string> cmds;
		char split_char_s[] = " ";
		boost::algorithm::split(cmds, cmd_str, boost::algorithm::is_any_of(split_char_s));

		if (cmds.empty()) continue;

		if( cmds[0] == "exit" )
		{
			break;
		}
		else if (cmds[0] == "clearlog")
		{
			Log::Inst().GetDefaultLogWriter()->ClearLog();
		}
        else if (cmds[0] == "clearvote")
        {
            UrlVote::Inst().ClearUp();
        }
		else if (cmds[0] == "content")
		{
			if(cmds.size() != 3) continue;

			string ctt_str = cmds[1];
			u_int file_length;
			try
			{
				file_length = boost::lexical_cast<u_int>(cmds[2]);
			}
			catch(boost::bad_lexical_cast)
			{
				continue;
			}
			MainThread::IOS().post(
				boost::bind(&UdpIndexModule::PrintCache, UdpIndexModule::Inst() , 1, ctt_str, file_length)
				);
		}
		else if (cmds[0] == "url")
		{
			if(cmds.size() != 2) continue;

			string url_str = cmds[1];
			MainThread::IOS().post(
				boost::bind(&UdpIndexModule::PrintCache, UdpIndexModule::Inst(), 2, url_str, 0)
				);
		}
		else if (cmds[0] == "list")
		{
			if(cmds.size() < 2) continue;

			string tmp_str = cmds[1];

            if (tmp_str == "vote")
            {
                CONSOLE_OUTPUT("---------------------------- Vote --------------------------");
                CONSOLE_OUTPUT(UrlVote::Inst().GetVoteInfo());
                CONSOLE_OUTPUT("------------------------------------------------------------");
            }

            if(cmds.size() < 3) continue;

			if (tmp_str == "content")
			{
				if (cmds[2] == "qpc")
				{
					MainThread::IOS().post(
						boost::bind(&UdpIndexModule::PrintCache, UdpIndexModule::Inst(), 3, "", 0)
						);
				}
				else if (cmds[2] == "total")
				{
					MainThread::IOS().post(
						boost::bind(&UdpIndexModule::PrintCache, UdpIndexModule::Inst(), 4, "", 0)
						);
				}
			}
			else if (tmp_str == "url")
			{
				if (cmds[2] == "qpc")
				{
					MainThread::IOS().post(
						boost::bind(&UdpIndexModule::PrintCache, UdpIndexModule::Inst(), 5, "", 0)
						);
				}
				else if (cmds[2] == "total")
				{
					MainThread::IOS().post(
						boost::bind(&UdpIndexModule::PrintCache, UdpIndexModule::Inst(), 6, "", 0)
						);
				}
			}
		}			
		else if (cmds[0] == "status")
		{
			if(cmds.size() != 2) continue;

			MainThread::IOS().post(
				boost::bind(&UdpIndexModule::PrintCache, UdpIndexModule::Inst(), 7, cmds[1], 0)
				);
		}
		else
		{
			CONSOLE_OUTPUT("============================= help ===================================");
			CONSOLE_OUTPUT("  Status Cache ------------------------- Cache命中率");
			CONSOLE_OUTPUT("  Status Net --------------------------- 网络状况");
			CONSOLE_OUTPUT("  Status Database ---------------------- 数据库状况");
			CONSOLE_OUTPUT("  List Content Total ------------------- 列出Cache中所有的Content");
			CONSOLE_OUTPUT("  List Url Total ----------------------- 列出Cache中所有的URL");
			CONSOLE_OUTPUT("  Content [ContentMD5] [FileLength] ---- 查看该Content对应的URL和时间戳");
			CONSOLE_OUTPUT("  URL [FlvUrl] ------------------------- 查看该URL对应的RID和时间戳");
			CONSOLE_OUTPUT("  List Content QPC --------------------- 列出时间戳最小和最大的五个ContentMD5");
			CONSOLE_OUTPUT("  List URL QPC ------------------------- 列出时间戳最小和最大的五个URL");
			CONSOLE_OUTPUT("  ClearLog ----------------------------- 清除日志");
            CONSOLE_OUTPUT("  List Vote ---------------------------- 显示Vote表信息");
			CONSOLE_OUTPUT("  Exit --------------------------------- 退出");
			CONSOLE_OUTPUT("=======================================================================");
		}

	}

	MainThread::IOS().post(boost::bind(&UdpIndexModule::Stop, UdpIndexModule::Inst()));

	DataBaseConnectionPool::Inst().Stop();

	DataBaseThreadPoolAdd::Inst().Stop();
	DataBaseThreadPoolQuery::Inst().Stop();

	TrackerListReadThread::Inst().Stop();

	MainThread::Inst().Stop();

	LOG(__INFO, "Index", "it\'s the end");
	framework::Log::Inst().Stop();

	return 0;
}
