#include "stdafx.h"
#include "base/guid.h"
#include "framework/MainThread.h"
#include "framework/Log.h"
#include "framework/io/Path.h"

#include "Config.h"
#include "IndexCleanerModule.h"
#include "DelThread.h"

using namespace framework;
using namespace indexcleaner;

int _tmain(int argc, _TCHAR* argv[])
{

	framework::Log::Inst().Start();
	framework::Log::Inst().SetLogLevel(__DEBUG);
	framework::Log::Inst().SetConsoleLevel(__ERROR);
	framework::Log::Inst().SetLogTypeOn("Index");
	framework::Log::Inst().SetConsoleTypeOn("Index");

	tstring moduleName = TEXT("index_database_cleaner.ini"); 
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
	DelThread::Inst().Start();

	LOG(__INFO, "Index", "threads started");

	MainThread::IOS().post(boost::bind(&IndexCleanerModule::Start, IndexCleanerModule::Inst()));

	CONSOLE_OUTPUT("IndexServer Database Cleaner Start to Work");
	CONSOLE_OUTPUT("Input \'Help\' to Get an Introduction");

	while(true)
	{
		char cmd_buf[1024];
		cin.getline(cmd_buf,1024);
		string command(cmd_buf);
		boost::algorithm::to_lower(command);
		if( command == "exit" )
		{
			DelThread::IOS().post(boost::bind(&IndexCleanerModule::Stop, IndexCleanerModule::Inst()));
			break;
		}
		else if (command == "clearlog")
		{
			Log::Inst().GetDefaultLogWriter()->ClearLog();
		}
		else if (command.find("clearrids") == 0)
		{
			size_t days;
			try
			{
				days = boost::lexical_cast<size_t>(command.substr(command.find(' ')+1));
			}
			catch (boost::bad_lexical_cast)
			{
				continue;
			}
			CONSOLE_TEXT("Are You Sure to Clear the RIDs " << days << " Days Befor?(Y/N) ");
			char buf_yes_or_no[1024];
			cin.getline(buf_yes_or_no, 1024);
			string str_yes_or_no(buf_yes_or_no);
			if (str_yes_or_no == "Y" || str_yes_or_no == "y")
			{
				MainThread::IOS().post(boost::bind(&IndexCleanerModule::ClearRidTable, IndexCleanerModule::Inst(),days));
			}			
		}
		else if (command.find("clearurls") == 0)
		{
			size_t days;
			try
			{
				days = boost::lexical_cast<size_t>(command.substr(command.find(' ')+1));
			}
			catch (boost::bad_lexical_cast)
			{
				continue;
			}
			CONSOLE_TEXT("Are You Sure to Clear the URLs " << days << " Days Befor?(Y/N) ");
			char buf_yes_or_no[1024];
			cin.getline(buf_yes_or_no, 1024);
			string str_yes_or_no(buf_yes_or_no);
			if (str_yes_or_no == "Y" || str_yes_or_no == "y")
			{
				MainThread::IOS().post(boost::bind(&IndexCleanerModule::ClearUrlTable, IndexCleanerModule::Inst(),days));
			}	
		}
		else if (command.find("cleardatabase") == 0)
		{
			size_t days;
			try
			{
				days = boost::lexical_cast<size_t>(command.substr(command.find(' ')+1));
			}
			catch (boost::bad_lexical_cast)
			{
				continue;
			}
			CONSOLE_TEXT("Are You Sure to Clear the Datas " << days << " Days Befor?(Y/N) ");
			char buf_yes_or_no[1024];
			cin.getline(buf_yes_or_no, 1024);
			string str_yes_or_no(buf_yes_or_no);
			if (str_yes_or_no == "Y" || str_yes_or_no == "y")
			{
				MainThread::IOS().post(boost::bind(&IndexCleanerModule::ClearDatabase, IndexCleanerModule::Inst(),days));
			}	
		}
		else if (command == "counturl")
		{
			MainThread::IOS().post(boost::bind(&IndexCleanerModule::CountUrlHaveMark, IndexCleanerModule::Inst()));
		}
		else
		{
			CONSOLE_OUTPUT("============================= help ===================================");
			CONSOLE_OUTPUT("  ClearRids [Days] --------------------- 清除距今[Days]天前的RID");
			CONSOLE_OUTPUT("  ClearUrls [Days] --------------------- 清除距今[Days]天前的URL");
			CONSOLE_OUTPUT("  ClearDatabase [Days] ----------------- 清除距今[Days]天前的RID和URL");
			CONSOLE_OUTPUT("  ClearLog ----------------------------- 清除日志");
			CONSOLE_OUTPUT("  Exit --------------------------------- 退出");
			CONSOLE_OUTPUT("=======================================================================");
		}

	}

	do
	{
		::Sleep(100);
	}while(IndexCleanerModule::is_finished_url == false || IndexCleanerModule::is_finished_rid == false);

	MainThread::Inst().Stop();
	DelThread::Inst().Stop();

	LOG(__INFO, "Index", "it\'s the end");
	framework::Log::Inst().Stop();

	return 0;
}
