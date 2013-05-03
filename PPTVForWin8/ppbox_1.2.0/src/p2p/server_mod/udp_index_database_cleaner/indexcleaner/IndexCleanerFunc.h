#pragma once
#include "stdafx.h"
#include "Config.h"
#include "MySQLp.h"

#ifndef INDEX_CLEANER_FUNC
#define INDEX_CLEANER_FUNC

using namespace udpindex;
using namespace framework;

namespace indexcleaner
{
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
}

#endif