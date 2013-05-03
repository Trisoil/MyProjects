#include "stdafx.h"
#include "IndexCleanerModule.h"
#include "IndexCleanerFunc.h"
#include "framework/io/stdfile.h"
#include "DelThread.h"
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace framework::io;
namespace indexcleaner
{
	bool IndexCleanerModule::is_finished_url = true;
	bool IndexCleanerModule::is_finished_rid = true;
	IndexCleanerModule::p IndexCleanerModule::inst_(new IndexCleanerModule());

	void IndexCleanerModule::Start()
	{
		if (is_running_ == true)return;
		
		DataBaseHost host;
		GetHostFromConfig("index_database", host);
		mysql_ = MySQLp::Create(host);

		if (mysql_->IsGoodConnection() == false)
		{
			CONSOLE_OUTPUT("Faild to Connect to the Database!");
			return;
		}

		rid_dir_ = DirTraversing::Create();
		url_dir_ = DirTraversing::Create();
		rid_dir_bak_ = DirTraversing::Create();
		url_dir_bak_ = DirTraversing::Create();

		url_file_path_ = Config::Inst().GetValue("url_path");
		rid_file_path_ = Config::Inst().GetValue("rid_path");

		max_cache_size_ = Config::Inst().GetInteger("max_cache_size", 819200);
		cycle_time_ = Config::Inst().GetInteger("del_db_cycle_time", 10);
		cycle_time_ *= 1000;

		if (url_file_path_.empty() || rid_file_path_.empty())
		{
			CONSOLE_OUTPUT("Failed to Get the Path of URLs or RIDs!");
			return;
		}

		rid_dir_->LoadDir(rid_file_path_, "*.log");
		url_dir_->LoadDir(url_file_path_, "*.log");
		rid_dir_bak_->LoadDir(rid_file_path_, "*.bak");
		url_dir_bak_->LoadDir(url_file_path_, "*.bak");

		if (rid_dir_->GetFiles().empty())
		{
			CONSOLE_OUTPUT("The RIDs Dir is Empty!");
		}
		if (url_dir_->GetFiles().empty())
		{
			CONSOLE_OUTPUT("The URLs Dir is Empty!");
		}

		is_running_ = true;
	}
	void IndexCleanerModule::Stop()
	{
		if (is_running_ == false)return;
		is_running_ = false;
		do
		{
			::Sleep(100);
		}while(is_finished_url == false || is_finished_rid == false);
		mysql_->Stop();
	}
	string IndexCleanerModule::FullPath(const string &path_str, const string &file_name)
	{
		string slash_tmp = "";
		if (path_str[path_str.size()-1] != '\\' && path_str[path_str.size()-1] != '/')
		{
			slash_tmp = '\\';
		}
		return path_str+slash_tmp+file_name;
	}
	double IndexCleanerModule::FileName2Days(string file_name)
	{
		string datatime_str = file_name.substr(0,file_name.find(".log")-0);
		vector<string> time_str_s;
		char split_char_s[] = "- .";
		boost::algorithm::split(time_str_s, datatime_str, boost::algorithm::is_any_of(split_char_s));
		SYSTEMTIME time_file;
		if (time_str_s.size() != 7)
		{
			return -1;
		}
		try
		{
			time_file.wYear = boost::lexical_cast<size_t>(time_str_s[0]);
			time_file.wMonth = boost::lexical_cast<size_t>(time_str_s[1]);
			time_file.wDay = boost::lexical_cast<size_t>(time_str_s[2]);
			time_file.wHour = boost::lexical_cast<size_t>(time_str_s[3]);
			time_file.wMinute = boost::lexical_cast<size_t>(time_str_s[4]);
			time_file.wSecond = boost::lexical_cast<size_t>(time_str_s[5]);
			time_file.wMilliseconds = boost::lexical_cast<size_t>(time_str_s[6]);
		}
		catch (boost::bad_lexical_cast)
		{
			return -2;				
		}
		double t_file;
		if (SystemTimeToVariantTime(&time_file, &t_file) == false)
		{
			return -3;
		}
		return t_file;
	}
	void IndexCleanerModule::ClearRidTable(size_t days_byond_now)
	{
		is_finished_rid = false;

		for (size_t i = 0; i < max_mod_; i++)
		{
			rid_cache_size_now_[i] = 0;
			ctt_cache_size_now_[i] = 0;
		}
		boost::shared_ptr<BloomFilter> rid_bloomfilter_ = boost::shared_ptr<BloomFilter>(new BloomFilter(27));

		vector<string> old_files, new_files;
		SYSTEMTIME time_now;
		GetSystemTime(&time_now);
		double t_now;
		if (SystemTimeToVariantTime(&time_now, &t_now) == false)
		{
			CONSOLE_OUTPUT("Failed to Get Time Now!");
			return;
		}
		for (size_t i = 0; i < rid_dir_->GetFiles().size(); i++)
		{
			double t_file = FileName2Days(rid_dir_->GetFiles()[i]);
			if (t_file < 0)
			{
				CONSOLE_OUTPUT("Failed to Get Time from a File Name");
				continue;
			}
			if (t_now - t_file > days_byond_now)
			{
				old_files.push_back(rid_dir_->GetFiles()[i]);
			}
			else
			{
				new_files.push_back(rid_dir_->GetFiles()[i]);
			}
		}

		for (size_t i = 0; i < new_files.size(); i++)
		{
			StdFile file_in;
			file_in.Open(b2w(FullPath(rid_file_path_, new_files[i])).c_str());
			while (file_in.IsOpen() && file_in.IsEOF() == false)
			{
				string rid_str;
				file_in.ReadLine(rid_str);
				if (rid_str.empty())
				{
					continue;
				}
				RID rid_tmp;
				rid_tmp.Parse(boost::algorithm::trim_copy(rid_str));
				if (rid_tmp.IsEmpty() == false)
				{
					rid_bloomfilter_->AddGuid(rid_tmp);
				}
			}
			file_in.Close();
		}
		StdFile file_bak, file_stl;
		if(!CreateDirectory(b2w(FullPath(rid_file_path_, "Log.bak")).c_str(),NULL))
		{
			if (GetLastError() != ERROR_ALREADY_EXISTS)
			{
				CONSOLE_OUTPUT("Failed to Create the Back Directory");
				return;
			}
		}
		file_bak.Open(b2w(FullPath(rid_file_path_, "Log.bak\\"+SysTime2Str(time_now)+".log.bak")).c_str(),L"w");
		file_stl.Open(b2w(FullPath(rid_file_path_, "Log.bak\\"+SysTime2Str(time_now)+".log.stl")).c_str(),L"w");
		for (size_t i = 0; i < old_files.size(); i++)
		{
			if(rid_dir_bak_->IsExist("rid_del.bak"))
			{
				StdFile file_bak_i;
				file_bak_i.Open(b2w(FullPath(rid_file_path_, "rid_del.bak")).c_str(),L"r");
				while (file_bak_i.IsOpen() && file_bak_i.IsEOF() == false)
				{
					string str_t;
					file_bak_i.ReadLine(str_t);
					RID rid_t;
					rid_t.Parse(boost::algorithm::trim_copy(str_t));
					rid_bloomfilter_->AddGuid(rid_t);
				}
				file_bak_i.Close();
			}
			StdFile file_in, file_out;
			file_out.Open(b2w(FullPath(rid_file_path_, old_files[i]+".bak")).c_str(),L"w");
			file_in.Open(b2w(FullPath(rid_file_path_, old_files[i])).c_str());
			while (file_in.IsOpen() && file_in.IsEOF() == false && is_running_)
			{
				while(is_clear_db_)
				{
					::Sleep(cycle_time_);
				}
				string rid_str;
				file_in.ReadLine(rid_str);
				file_out.Write(rid_str.c_str());
				if (rid_str.empty())
				{
					continue;
				}
				RID rid_tmp;
				rid_tmp.Parse(boost::algorithm::trim_copy(rid_str));
				if (rid_tmp.IsEmpty() == false && rid_bloomfilter_->HasGuid(rid_tmp) == false)
				{
					rid_bloomfilter_->AddGuid(rid_tmp);
					DelThread::IOS().post(boost::bind(&IndexCleanerModule::DelRid, IndexCleanerModule::Inst(),rid_tmp));
					file_bak.Write(rid_str.c_str());
				}
				else
				{
					file_stl.Write(rid_str.c_str());
				}
			}
			file_in.Close();
			file_out.Close();
			if(is_running_)DeleteFile(b2w(FullPath(rid_file_path_, old_files[i])).c_str());
		}
		file_bak.Close();
		file_stl.Close();

		rid_bloomfilter_->Clear();

		for(size_t i = 0; i < max_mod_; i++)
		{
			ClearRID(i);
			ClearCTT(i);
		}

		DeleteFile(b2w(FullPath(rid_file_path_, "rid_del.bak")).c_str());
		is_finished_rid = true;
		CONSOLE_OUTPUT("Finished to Clear RIDs " << days_byond_now << " Days Before");
	}
	void IndexCleanerModule::ClearUrlTable(size_t days_byond_now)
	{
		is_finished_url = false;

		for (size_t i = 0; i < max_mod_; i++)
		{
			url_cache_size_now_[i] = 0;
		}

		boost::shared_ptr<BloomFilter> url_bloomfilter_ = boost::shared_ptr<BloomFilter>(new BloomFilter(27));

		vector<string> old_files, new_files;
		SYSTEMTIME time_now;
		GetSystemTime(&time_now);
		double t_now;
		if (SystemTimeToVariantTime(&time_now, &t_now) == false)
		{
			CONSOLE_OUTPUT("Failed to Get Time Now!");
			return;
		}
		for (size_t i = 0; i < url_dir_->GetFiles().size(); i++)
		{
			double t_file = FileName2Days(url_dir_->GetFiles()[i]);
			if (t_file < 0)
			{
				CONSOLE_OUTPUT("Failed to Get Time from a File Name");
				continue;
			}
			if (t_now - t_file > days_byond_now)
			{
				old_files.push_back(url_dir_->GetFiles()[i]);
			}
			else
			{
				new_files.push_back(url_dir_->GetFiles()[i]);
			}
		}

		for (size_t i = 0; i < new_files.size(); i++)
		{
			StdFile file_in;
			file_in.Open(b2w(FullPath(url_file_path_, new_files[i])).c_str());
			while (file_in.IsOpen() && file_in.IsEOF() == false)
			{
				string url_tmp;
				file_in.ReadLine(url_tmp);
				boost::algorithm::trim(url_tmp);
				if (url_tmp.empty() == false)
				{
					url_bloomfilter_->AddString(url_tmp.c_str());
				}
			}
			file_in.Close();
		}
		StdFile file_bak, file_stl;
		if(!CreateDirectory(b2w(FullPath(url_file_path_, "Log.bak")).c_str(),NULL))
		{
			if (GetLastError() != ERROR_ALREADY_EXISTS)
			{
				CONSOLE_OUTPUT("Failed to Create the Back Directory");
				return;
			}
		}
		file_bak.Open(b2w(FullPath(url_file_path_, "Log.bak\\"+SysTime2Str(time_now)+".log.bak")).c_str(),L"w");
		file_stl.Open(b2w(FullPath(url_file_path_, "Log.bak\\"+SysTime2Str(time_now)+".log.stl")).c_str(),L"w");
		for (size_t i = 0; i < old_files.size(); i++)
		{
			if(url_dir_bak_->IsExist("url_del.bak"))
			{
				StdFile file_bak_i;
				file_bak_i.Open(b2w(FullPath(url_file_path_, "url_del.bak")).c_str(),L"r");
				while (file_bak_i.IsOpen() && file_bak_i.IsEOF() == false)
				{
					string str_t;
					file_bak_i.ReadLine(str_t);
					boost::algorithm::trim(str_t);
					url_bloomfilter_->AddString(str_t.c_str());
				}
				file_bak_i.Close();
			}
			StdFile file_in, file_out;
			file_out.Open(b2w(FullPath(url_file_path_, old_files[i]+".bak")).c_str(),L"w");
			file_in.Open(b2w(FullPath(url_file_path_, old_files[i])).c_str());
			while (file_in.IsOpen() && file_in.IsEOF() == false && is_running_)
			{
				while(is_clear_db_)
				{
					::Sleep(cycle_time_);
				}
				string url_tmp;
				file_in.ReadLine(url_tmp);
				file_out.Write(url_tmp.c_str());
				boost::algorithm::trim(url_tmp);
				if (url_tmp.empty() == false && url_bloomfilter_->HasString(url_tmp.c_str()) == false)
				{
					url_bloomfilter_->AddString(url_tmp.c_str());
					DelThread::IOS().post(boost::bind(&IndexCleanerModule::DelUrl, IndexCleanerModule::Inst(),url_tmp));
					file_bak.Write(string(url_tmp+"\n").c_str());
				}
				else
				{
					file_stl.Write(string(url_tmp+"\n").c_str());
				}
			}
			file_in.Close();
			file_out.Close();
			if(is_running_)DeleteFile(b2w(FullPath(url_file_path_, old_files[i])).c_str());
		}
		file_bak.Close();
		file_stl.Close();

		url_bloomfilter_->Clear();
		for(size_t i = 0; i < max_mod_; i++)
		{
			ClearURL(i);
		}

		DeleteFile(b2w(FullPath(url_file_path_, "url_del.bak")).c_str());

		is_finished_url = true;
		CONSOLE_OUTPUT("Finished to Clear URLs " << days_byond_now << " Days Before");
	}
	bool IndexCleanerModule::DelRid(RID rid)
	{
		if (rid.IsEmpty())return true;

		size_t rid_key = boost::hash_value(rid);
		size_t rid_mod = rid_key%256;

		string sql_str = "select ContentMD5, FileLength from Ppvideo_Index_Resource_%2% where  RidKey = %3% and ResourceID = \'%1%\'";
		string fmt_str = boost::str(boost::format(sql_str)%rid.ToString() %rid_mod %rid_key);
		if (!(mysql_->Query(fmt_str)))
		{
			return false;
		}
		if (mysql_->GetResults().empty() == false)
		{
			MD5 content_md5;
			content_md5.Parse(mysql_->GetResults()[0][0]);
			if (content_md5.IsEmpty() == false)
			{
				size_t content_md5_key = boost::hash_value(content_md5);
				size_t content_n = content_md5_key % 256;
				size_t file_length;
				try
				{
					file_length = boost::lexical_cast<size_t>(mysql_->GetResults()[0][1]);
				}
				catch (boost::bad_lexical_cast)
				{
					return false;
				}

				ctt_s_[content_n].insert(make_pair(file_length,make_pair(content_md5_key,content_md5)));
				ctt_cache_size_now_[content_n] += string(" or (ContentKey =  and FileLength =  and ContentMD5 = \'\')").size() + 10 + 10 + 32;

				if (ctt_cache_size_now_[content_n] + string("delete from Ppvideo_Index_Content_ where ").size() + 10 - 4 > max_cache_size_)
				{
					ClearCTT(content_n);
				}
			}

			rid_s_[rid_mod].insert(make_pair(rid_key,rid));

			rid_cache_size_now_[rid_mod] += string(" or (RidKey =  and ResourceID = \'\')").size() + 10 + 32;
			if (rid_cache_size_now_[rid_mod] + string("delete from Ppvideo_Index_Resource_ where ").size() + 10 - 4 > max_cache_size_)
			{
				ClearRID(rid_mod);
			}
		}
		return true;
	}
	bool IndexCleanerModule::DelUrl(string url)
	{
		if(url.empty())return true;

		size_t url_key = boost::hash_value(url);
		size_t url_mod = url_key%256;

		url_s_[url_mod].insert(make_pair(url_key,url));

		url_cache_size_now_[url_mod] += string(" or (UrlKey =  and MiniFlvUrl = \'\')").size() + 10 + url.size();
		if (url_cache_size_now_[url_mod] + string("delete from Ppvideo_Index_HttpServer_ where ").size() + 10 - 4 > max_cache_size_)
		{
			ClearURL(url_mod);
		}
		return true;
	}

	bool IndexCleanerModule::ClearRID(size_t mod)
	{
		if (rid_s_[mod].empty()) return true;
		
		ClearDB cleardb(shared_from_this());

		CONSOLE_OUTPUT("ClearRID");
		string str_tmp = "";
		StdFile file_del_bak;
		file_del_bak.Open(b2w(FullPath(rid_file_path_, "rid_del.bak")).c_str(), L"a");
		while(rid_s_[mod].empty() == false)
		{
			set<pair<size_t, Guid>>::iterator it = rid_s_[mod].begin();

			if (str_tmp.empty())
			{
				str_tmp += boost::str(boost::format("delete from Ppvideo_Index_Resource_%1% where ")%mod);
				str_tmp += boost::str(boost::format("(RidKey = %1% and ResourceID = \'%2%\')")
					%it->first %it->second);
			}
			else
			{
				str_tmp += boost::str(boost::format(" or (RidKey = %1% and ResourceID = \'%2%\')")
					%it->first %it->second.ToString());
			}

			file_del_bak.Write(string(it->second.ToString()+"\n").c_str());
			rid_s_[mod].erase(it);
		}
		rid_cache_size_now_[mod] = 0;
		file_del_bak.Close();
		if(!(mysql_->Query(str_tmp)))
		{
			return false;
		}
		return true;
	}
	bool IndexCleanerModule::ClearURL(size_t mod)
	{
		if (url_s_[mod].empty()) return true;

		ClearDB cleardb(shared_from_this());

		CONSOLE_OUTPUT("ClearURL");
		string str_tmp = "";
		StdFile file_del_bak;
		file_del_bak.Open(b2w(FullPath(url_file_path_, "url_del.bak")).c_str(), L"a");
		while(url_s_[mod].empty() == false)
		{
			set<pair<size_t, string>>::iterator it = url_s_[mod].begin();

			if (str_tmp.empty())
			{
				str_tmp += boost::str(boost::format("delete from Ppvideo_Index_HttpServer_%1% where ")%mod);
				str_tmp += boost::str(boost::format("(UrlKey = %1% and MiniFlvUrl = \'%2%\')")
					%it->first %it->second);
			}
			else
			{
				str_tmp += boost::str(boost::format(" or (UrlKey = %1% and MiniFlvUrl = \'%2%\')")
					%it->first %it->second);
			}

			file_del_bak.Write(string(it->second+"\n").c_str());
			url_s_[mod].erase(it);
		}
		url_cache_size_now_[mod] = 0;
		file_del_bak.Close();
		if(!(mysql_->Query(str_tmp)))
		{
			return false;
		}

		return true;
	}
	bool IndexCleanerModule::ClearCTT(size_t mod)
	{
		if (ctt_s_[mod].empty()) return true;

		ClearDB cleardb(shared_from_this());

		CONSOLE_OUTPUT("ClearCTT");
		string str_tmp = "";
		while(ctt_s_[mod].empty() == false)
		{
			set<pair<size_t, pair<size_t,Guid>>>::iterator it = ctt_s_[mod].begin();

			if (str_tmp.empty())
			{
				str_tmp += boost::str(boost::format("delete from Ppvideo_Index_Content_%1% where ")%mod);
				str_tmp += boost::str(boost::format("(ContentKey = %2% and FileLength = %1% and ContentMD5 = \'%3%\')")
					%it->first %it->second.first %it->second.second.ToString());
			}
			else
			{
				str_tmp += boost::str(boost::format(" or (ContentKey = %2% and FileLength = %1% and ContentMD5 = \'%3%\')")
					%it->first %it->second.first %it->second.second.ToString());
			}
			ctt_s_[mod].erase(it);
		}
		ctt_cache_size_now_[mod] = 0;
		if(!(mysql_->Query(str_tmp)))
		{
			return false;
		}
		return true;
	}

	void IndexCleanerModule::ClearDatabase(size_t days_byond_now)
	{
		ClearRidTable(days_byond_now);
		ClearUrlTable(days_byond_now);
	}
	string IndexCleanerModule::SysTime2Str(SYSTEMTIME sys_time)
	{
		return boost::str(boost::format("%04d-%02d-%02d %02d-%02d-%02d.%03d") 
			%sys_time.wYear %sys_time.wMonth %sys_time.wDay %sys_time.wHour 
			%sys_time.wMinute %sys_time.wSecond %sys_time.wMilliseconds);
	}

	void IndexCleanerModule::CountUrlHaveMark()
	{
		size_t count = 0, total = 0, ohmygod = 0;
		boost::shared_ptr<BloomFilter> url_bloomfilter_ = boost::shared_ptr<BloomFilter>(new BloomFilter(27));
		for (size_t i = 0; i < url_dir_->GetFiles().size(); i++)
		{
			string str = url_dir_->GetFiles()[i];
			if (str.find("00.000.log")!=string::npos)
			{
				continue;
			}

			StdFile file_in;
			file_in.Open(b2w(FullPath(url_file_path_, str)).c_str());
			while (file_in.IsOpen() && file_in.IsEOF() == false)
			{
				string url_tmp;
				file_in.ReadLine(url_tmp);
				boost::algorithm::trim(url_tmp);
				if (url_tmp.empty() == false && url_tmp.find('\?')!=string::npos && url_tmp.find("get_video") == string::npos)
				{
					RELEASE_OUTPUT(url_tmp);
					if (url_bloomfilter_->HasString(url_tmp.c_str()))
					{
						ohmygod++;
					}
					else
					{
						url_bloomfilter_->AddString(url_tmp.c_str());
					}
					count++;
				}
				total++;
			}
			file_in.Close();
		}
		CONSOLE_OUTPUT("The Number of URLs having \'\?\' is " << ohmygod << ":" << count << "/" << total);
	}
}