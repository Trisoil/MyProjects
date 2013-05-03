#include "stdafx.h"
#include "udpindex/list_config.h"

namespace udpindex
{
    /*
	bool Str2DBHost(const string& host_str, DataBaseHost& host)
	{
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
    */

	bool Str2IndexInfo(const string& str, INDEX_SERVER_INFO& index_info)
	{
		if (str.empty())
		{
			return false;
		}
		u_long ip;
		u_short port;
		if (framework::network::EndpointToIpPort(framework::network::ParseUdpEndpoint(str), ip, port))
		{
			INDEX_SERVER_INFO tmp_info((UINT32)ip, (UINT16)port);
			index_info = tmp_info;
			return true;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// class ListConfig

	//////////////////////////////////////////////////////////////////////////
	bool ListConfig::GetIndexServerList(const tstring& filename, 
		vector<INDEX_SERVER_INFO>& index_list, vector<UINT8>& mode_list)
	{
		framework::io::StdFileReader file;

		file.OpenText(filename.c_str());

		if (false == file.IsOpen())
			return false;

		string line;
		size_t num = 0;
		while (file.ReadLine(line))
		{
			// empty line or comments
			size_t comment_index = line.find_first_of('#');
			if (comment_index != string::npos)
				line = line.substr(0, comment_index);

			line = boost::algorithm::trim_left_copy( line );
			if (line.length() == 0 || line[0] == '#')
				continue;
			// TODO
			istringstream iss(line);
			if (num <= 255)
			{
				string mode_s, index_s;
				size_t mode_num = 0;
				size_t index_num = 0;
				iss>>mode_s>>index_s;
				try
				{
					mode_num = boost::lexical_cast<size_t>(mode_s);
					index_num = boost::lexical_cast<size_t>(index_s);
				}
				catch(boost::bad_lexical_cast&)
				{
					return false;
				}
				mode_num = (UINT8)mode_num;
				index_num = (UINT8)index_num;
				if(mode_num != num) return false;
				mode_list.push_back(index_num);
				++num;
			}
			else
			{
				string index_s, str;
				size_t index_num = 0;
				iss>>index_s>>str;
				try
				{
					index_num = boost::lexical_cast<size_t>(index_s);
				}
				catch(boost::bad_lexical_cast&)
				{
					return false;
				}
				index_num = (UINT8)index_num;
				if(index_num != index_list.size()) return false;
				INDEX_SERVER_INFO index_info;
				if (Str2IndexInfo(str, index_info) == false)
				{
					return false;
				}
				index_list.push_back(index_info);
			}
		} // while
		if(256 != mode_list.size()) 
		{
			CONSOLE_LOG("ListConfig::GetIndexServerList Mod个数不对，目前个数："<<mode_list.size());
			return false;
		}
		return true;
	}

	bool ListConfig::GetDBList(const tstring& filename, vector<HostInfo>& db_list, vector<DBInfo>& db_info
		, vector<boost::asio::ip::udp::endpoint>& stat_list)
	{
        tstring moduleName = filename; 
        framework::mswin::Module module;
        framework::io::Path config_path;
        module.BuildLocalFilePath(config_path, moduleName.c_str());

		framework::io::StdFileReader file;

		file.OpenText(config_path.GetString().c_str());

		if (false == file.IsOpen())
			return false;

		string line;
		size_t num = 0;
		while (file.ReadLine(line))
		{
			// empty line or comments
			size_t comment_index = line.find('#');
			if (comment_index != string::npos)
				line = line.substr(0, comment_index);

			boost::algorithm::trim_left( line );
			if (line.length() == 0 || line[0] == '#')
				continue;
			// TODO
			istringstream iss(line);
			if (num <= 255)
			{
				string mode_s, old_s, if_s, new_s;
				size_t mode_num = 0;
				size_t old_mode = 0;
				size_t new_mode = 0;
				bool if_trans;
				iss>>mode_s>>old_s>>if_s>>new_s;
				try
				{
					mode_num = boost::lexical_cast<size_t>(mode_s);
					old_mode = boost::lexical_cast<size_t>(old_s);
					if_trans = boost::lexical_cast<bool>(if_s);
					new_mode = boost::lexical_cast<size_t>(new_s);
				}
				catch(boost::bad_lexical_cast&)
				{
					return false;
				}
				mode_num = (UINT8)mode_num;
				old_mode = (UINT8)old_mode;
				new_mode = (UINT8)new_mode;
				if(mode_num != num) return false;
				DBInfo di;
				di.old_mode = old_mode;
				di.if_need_trans = if_trans;
				di.new_mode = new_mode;
				db_info.push_back(di);
				++num;
			}
			else
			{
				string db_s, str, stat_s;
				size_t db_mode = 0;
				iss>>db_s>>str>>stat_s;
				try
				{
					db_mode = boost::lexical_cast<size_t>(db_s);
				}
				catch(boost::bad_lexical_cast&)
				{
					return false;
				}
				db_mode = (UINT8)db_mode;
				if(db_mode != db_list.size()) return false;
				HostInfo db_host;
				
                size_t ps = str.find(":");
                if (ps == string::npos)
                {
                    return false;
                }
                db_host.ip_ = str.substr(0,ps);
                db_host.port_ = atoi(str.substr(ps+1).c_str());
                if (db_host.port_ <= 0)
                {
                    return false;
                }

				db_list.push_back(db_host);
				boost::asio::ip::udp::endpoint stat = framework::network::ParseUdpEndpoint(stat_s);
				stat_list.push_back(stat);
			}
		} // while
		if(256 != db_info.size()) 
		{
			CONSOLE_LOG("ListConfig::GetDBList Mod个数不对，目前个数："<<db_info.size());
			return false;
		}
		return true;
	}
}