#include "stdafx.h"
#include "base/guid.h"
#include "framework/MainThread.h"
#include "framework/Log.h"
#include "framework/io/Path.h"
#include "Config.h"
#include "DataMigration.h"
#include "MySQLp.h"

using namespace framework;
using namespace index_migration;
using namespace udpindex;

bool GetHostFromConfig(string key, DataBaseHost &host)
{
	string host_str = Config::Inst().GetValue(key, "");
	if (host_str.empty())
	{
		return false;
	}
	vector<string> db_str_s;
	char split_char_s[] = " \t\r\n:@;|";
	char ip_split_char_s[] = ".";
	boost::algorithm::split(db_str_s, host_str, boost::algorithm::is_any_of(split_char_s));
	if (db_str_s.size() != 5)
	{
		return false;
	}
	host.usr_ = db_str_s[0];
	host.pwd_ = db_str_s[1];		
	host.ip_ = db_str_s[2];
	host.dbname_ = db_str_s[4];
	try
	{
		host.port_ = boost::lexical_cast<u_short>(db_str_s[3]);
	}
	catch (boost::bad_lexical_cast e)
	{
		return false;
	}
	return true;
}
int _tmain(int argc, _TCHAR* argv[])
{

	framework::Log::Inst().Start();
	framework::Log::Inst().SetLogLevel(__DEBUG);
	framework::Log::Inst().SetConsoleLevel(__ERROR);
	framework::Log::Inst().SetLogTypeOn("Migration");
	framework::Log::Inst().SetConsoleTypeOn("Migration");

	tstring moduleName = TEXT("index_server_data_migration.txt"); 
	framework::mswin::Module module;
	framework::io::Path config_path;
	module.BuildLocalFilePath(config_path, moduleName.c_str());

	if(Config::Inst().LoadConfig(config_path.GetString()) == false)
	{
		CONSOLE_OUTPUT("Failed to open the config file " << moduleName.c_str());
		return 0;
	}
	// 启动 主线程 读取TrackerList配置线程 数据库线程池
	MainThread::Inst().Start();

	DataBaseHost host_out, host_statistic;

	if (!GetHostFromConfig("database_out", host_out) || !GetHostFromConfig("database_statistic", host_statistic))
	{
		CONSOLE_LOG("Failed to Get Database Info from Config File");
		return 0;
	}

	MainThread::IOS().post(boost::bind(&DataMigration::Start, DataMigration::Inst(), host_out, host_statistic));

	while(true)
	{
		string command;
		cin >> command;
		boost::algorithm::to_lower(command);
		if( command == "exit" )
		{
			break;
		}
		else if (command == "migrateurl")
		{
			MainThread::IOS().post(boost::bind(&DataMigration::MigrateURL, DataMigration::Inst()));
		}
		else if (command == "migraterid")
		{
			MainThread::IOS().post(boost::bind(&DataMigration::MigrateRID, DataMigration::Inst()));
		}
// 		else if (command == "counturlbytes")
// 		{
// 			MainThread::IOS().post(boost::bind(&DataMigration::CountMiniUrlBytes, DataMigration::Inst()));
// 		}
		else
		{
			cout << "======================= help =================" << endl;
			cout << "MigrateURL ------------- 迁移URL数据" << endl;
			cout << "MigrateRID ------------- 迁移RID数据" << endl;
			cout << "==============================================" << endl;
		}
	}

	MainThread::IOS().post(boost::bind(&DataMigration::Stop, DataMigration::Inst()));

	MainThread::Inst().Stop();

	framework::Log::Inst().Stop();

	return 0;
}
