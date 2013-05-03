#include "stdafx.h"
#include "DataMigration.h"
#include <stdlib.h>

using namespace boost;

namespace index_migration
{

	DataMigration::p DataMigration::inst_(new DataMigration());

	DataMigration::p DataMigration::Create()
	{
		return p(new DataMigration());
	}

	void DataMigration::Start(DataBaseHost database_out, DataBaseHost database_statistic)
	{
		if (is_running_ == true) return;
		filter_ = boost::shared_ptr<BloomFilter>(new BloomFilter(27));

		database_index_ = MySQLp::Create(database_out);
		database_statistic_ = MySQLp::Create(database_statistic);
		if (database_index_->IsGoodConnection() && database_statistic_->IsGoodConnection())
		{
			CONSOLE_OUTPUT("Connect to IP:\n" << database_out.ip_ << " && " << database_statistic.ip_);
			CONSOLE_OUTPUT("Connect to Database:\n" << database_out.dbname_ << " && " << database_statistic.dbname_);
		}
		else
		{
			CONSOLE_OUTPUT("Failed to Connect to Database");
		}
		print_sample_timer_ = framework::timer::PeriodicTimer::create(1000, shared_from_this());

		wstring httpserver_table_file_name = Config::Inst().GetTString("httpserver_table_file");
		wstring resource_table_file_name = Config::Inst().GetTString("resource_table_file");

		if (httpserver_table_file_name.empty() || resource_table_file_name.empty())
		{
			CONSOLE_OUTPUT("Failed to Get the FileName Data from");
			return;
		}

		httpserver_table_file_ = boost::shared_ptr<StdFile>(new StdFile());
		resource_table_file_ = boost::shared_ptr<StdFile>(new StdFile());
		httpserver_table_file_->Open(httpserver_table_file_name.c_str());
		resource_table_file_->Open(resource_table_file_name.c_str());

		if (httpserver_table_file_->IsOpen() == false || resource_table_file_->IsOpen() == false)
		{
			CONSOLE_OUTPUT("Failed to Open File");
			return;
		}
		if (httpserver_table_file_->IsEOF()==false)
		{
			string str;
			httpserver_table_file_->ReadLine(str);
		}
		if (resource_table_file_->IsEOF()==false)
		{
			string str;
			resource_table_file_->ReadLine(str);
		}

		//////////////////////////////////////////////////////////////////////////
		/* 
#ifdef _WIN32
		
		int rc = WSAStartup(MAKEWORD(2,0), &wsaData_);
		if (rc != 0) {
			CONSOLE_OUTPUT("Failed to start WinSock");
			return;
		}
#endif
		

		mem_cache_server_ = Config::Inst().GetValue("mem_cache_server");
		if (mem_cache_server_.empty())
		{
			CONSOLE_OUTPUT("Failed to Get MemCache Info");
			return;
		}
		mem_cache_client_ = boost::shared_ptr<MemCacheClient>(new MemCacheClient());
		mem_cache_client_->AddServer(mem_cache_server_.c_str());
		vector<MemCacheClient::string_t> rgServers;
		mem_cache_client_->GetServers(rgServers);
		for (size_t n = 0; n < rgServers.size(); ++n) 
		{
			CONSOLE_OUTPUT("Using server: " << rgServers[n].data());
		}
		mem_cache_client_->FlushAll(mem_cache_server_.c_str(), 1);
		CONSOLE_OUTPUT("Init MemCache");
		::Sleep(1000);*/
		is_running_ = true;
		is_finished_ = false;
	}
	void DataMigration::Stop()
	{
		if (is_running_ == false) return;
		is_running_ = false;

		database_index_->Stop();
		print_sample_timer_->Stop();
		httpserver_table_file_->Close();
		resource_table_file_->Close();
		filter_->Clear();
// 		mem_cache_client_->FlushAll(mem_cache_server_.c_str(), 1);
// 		mem_cache_client_->ClearServers();
// 		mem_cache_client_.reset();
// #ifdef _WIN32
// 		WSACleanup();
// #endif
// 		
	}

	/*
	void DataMigration::Migrate(size_t begin_index, size_t end_index, size_t sample_num)
	{
		if (is_running_ == false) return;

		print_sample_timer_->Start();
		time_start_ = time(NULL);

		size_t url_num = GetUrlNum();

		if (end_index > url_num)
		{
			end_index = url_num;
		}

		if (sample_num > end_index-begin_index)
		{
			sample_num = end_index-begin_index;
		}


		if (begin_index >= end_index) 
		{
			is_finished_ = true;
			return;
		}

		CONSOLE_OUTPUT("    From " << begin_index << " to " << end_index << "; sample " << sample_num);

		sample_.clear();

		size_t per_time_num = 1000;

		size_t total_num = 0;

		set<size_t> rand_indexs;
        
		//while (rand_indexs.size() < sample_num)
		//{
		//	rand_indexs.insert((((size_t(0)+rand())<<16)|rand())%(end_index-begin_index));
		//}
        

		if (sample_num > 0)
		{
			size_t range = end_index - begin_index;
			size_t block_size = range / sample_num;
			for (size_t i = 0; i < range; i += block_size)
			{
				size_t e = (i + block_size >= range ? range : i + block_size);
				rand_indexs.insert(i + size_t((rand() << 16) | rand()) % (e - i));
			}
		}

		for (size_t i = 0; i < (end_index-begin_index)/per_time_num; i++)
		{
			vector<vector<string>> urls = GetURLs(begin_index+i*per_time_num, per_time_num);
			if (urls.empty())
			{
				continue;
			}

			total_num += urls.size();

			for (size_t j = 0; j < urls.size(); j++)
			{
				InsertURL(urls[j]);
				string rid_str = urls[j][3];
				vector<string> rid_line = GetRID(rid_str);
				InsertRID(rid_line);

				if (rand_indexs.find(i*per_time_num+j) != rand_indexs.end())
				{
					sample_.push_back(make_pair(urls[j][0],rid_str));
				}
			}
		}
		vector<vector<string>> urls = GetURLs(end_index-(end_index-begin_index)%per_time_num, (end_index-begin_index)%per_time_num);
		if (urls.empty())
		{
			is_finished_ = true;
			return;
		}

		total_num += urls.size();

		for (size_t k = 0; k < urls.size(); k++)
		{
			InsertURL(urls[k]);
			string rid_str = urls[k][3];
			vector<string> rid_line = GetRID(rid_str);
			InsertRID(rid_line);

			if (rand_indexs.find(k+end_index-(end_index-begin_index)%per_time_num) != rand_indexs.end())
			{
				sample_.push_back(make_pair(urls[k][0],rid_str));
			}
		}
		is_finished_ = true;
	}
	*/
// 
// 	void DataMigration::OutputSample()
// 	{
// 		if (is_running_ == false) return;
// 
// 		size_t i = 0;
// 		for (list<pair<string/*URL*/, string/*RID*/>>::iterator it = sample_.begin(); 
// 			it != sample_.end(); it++, i++)
// 		{
// 			RELEASE_OUTPUT(i << "\t" << it->first << "\t" << it->second);
// 			LOG(__INFO, "Migration", i << "\t" << it->first << "\t" << it->second);
// 		}
// 
// 		sample_.clear();
// 	}

	vector<string> DataMigration::GetURL()
	{
		while (httpserver_table_file_->IsEOF() == false)
		{
			memset(readline_buf_,0,buf_size_);
			httpserver_table_file_->ReadLine(readline_buf_,buf_size_);
			string line_str(readline_buf_);
			if (line_str.empty()==false)
			{
				vector<string> url_str_s;
				char split_char_s[] = "\r\n,";
				boost::algorithm::split(url_str_s, line_str, boost::algorithm::is_any_of(split_char_s));
// 
// 				for (size_t i = 0; i < url_str_s.size(); i++)
//  				{
//  					CONSOLE_OUTPUT(url_str_s[i]);
// 				}
//  				cin.get();

				if (url_str_s.empty())
				{
					continue;
				}

				return url_str_s;
			}
		}
		return vector<string>();
	}
	vector<string> DataMigration::GetRID()
	{
		while (resource_table_file_->IsEOF() == false)
		{
			memset(readline_buf_,0,buf_size_);
			resource_table_file_->ReadLine(readline_buf_,buf_size_);
			string line_str(readline_buf_);
			if (line_str.empty()==false)
			{
				vector<string> rid_str_s;
				char split_char_s[] = "\r\n,";
				boost::algorithm::split(rid_str_s, line_str, boost::algorithm::is_any_of(split_char_s));
 
//  			for (size_t i = 0; i < rid_str_s.size(); i++)
// 				{
//  				CONSOLE_OUTPUT(rid_str_s[i]);
//  			}
//  			cin.get();
				if (rid_str_s.empty())
				{
					continue;
				}

				return rid_str_s;
			}
		}
		return vector<string>();
	}

	/*
	bool DataMigration::InsertURL(vector<string> url_line)
	{
		if (url_line.size() != 5)
		{
			return false;
		}
		string url = url_line[0];
		string refer = url_line[1];
		string type = url_line[2];
		string rid = url_line[3];
		string mini = url_line[4];

		mini = Url2Mini(url, refer);

		size_t url_key = boost::hash_value(mini);
		size_t url_mod = url_key%256;

		string s5 = "select count(0) from Ppvideo_Index_HttpServer_%2% where UrlKey = %3% and MiniFlvUrl = \'%1%\'";
		string sql5 = boost::str(boost::format(s5)%mini%url_mod%url_key );
		if (!(database_index_->Query(sql5)))
		{
			CONSOLE_LOG("Failed to Select URL "<<url);
			return false;
		}
		else
		{
			if (database_index_->GetResults().empty() == false)
			{
				size_t count = 0;

				try
				{
					count = lexical_cast<size_t>(database_index_->GetResults()[0][0]);
				}
				catch (bad_lexical_cast &e)
				{
					CONSOLE_LOG("Failed to do a lexical_cast");
					return false;
				}
				if (count == 0)
				{

					string s2 = "insert into Ppvideo_Index_HttpServer_%5% values (\'%1%\',\'%2%\',%3%,\'%4%\',\'%6%\',%7%)";
					string sql2 = boost::str(boost::format(s2) %mini %refer %type %rid %url_mod %url%url_key);

					if(!(database_index_->Query(sql2)))
					{
						CONSOLE_LOG("Failed to Insert URL "<<url);
						return false;
					}
//					RELEASE_LOG("Insert URL " << url << "; MOD = " << url_mod);
				}
				else
				{
//					RELEASE_LOG("Clash URL " << url << "; MOD = " << url_mod);
				}
			}
		}

		string s4 = "select count(0) from Ppvideo_Index_Url_Visit_Date_%2% where UrlKey = %3% and FlvUrl = \'%1%\'";
		string sql4 = boost::str(boost::format(s4)%mini%url_mod%url_key );
		if (!(database_statistic_->Query(sql4)))
		{
			CONSOLE_LOG("Failed to Select URL "<<url);
			return false;
		}
		else
		{
			if (database_statistic_->GetResults().empty() == false)
			{
				size_t count = 0;

				try
				{
					count = lexical_cast<size_t>(database_statistic_->GetResults()[0][0]);
				}
				catch (bad_lexical_cast &e)
				{
					CONSOLE_LOG("Failed to do a lexical_cast");
					return false;
				}
				if (count == 0)
				{
					string sql3 = boost::str(boost::format("insert into Ppvideo_Index_Url_Visit_Date_%1% values(\'%2%\',%3%, %4%)"
						) %url_mod %mini %time(NULL)%url_key);
					if(!(database_statistic_->Query(sql3)))
					{
						CONSOLE_LOG("Failed to Insert URL Visited Date, URL = "<<url);
						return false;
					}
				}
			}
		}

		return true;
	}
	bool DataMigration::InsertRID(vector<string> rid_line)
	{
		if (rid_line.size() != 7)
		{
			return false;
		}
		RID rid;
		rid.Parse(rid_line[0]);
		if (rid.IsEmpty())
		{
			return false;
		}
		string file_length = rid_line[1];
		string block_size = rid_line[2];
		string block_num = rid_line[3];
		string block_md5s = rid_line[4];

		size_t rid_key = boost::hash_value(rid);
		size_t rid_mod = rid_key%256;

		MD5 content_md5 = Guid();

		string s5 = "select count(0) from Ppvideo_Index_Resource_%2% where RidKey = %3% and ResourceID = \'%1%\'";
		string sql5 = boost::str(boost::format(s5)%rid.ToString()%rid_mod%rid_key);
		if (!(database_index_->Query(sql5)))
		{
			CONSOLE_LOG("Failed to Select RID "<<rid.ToString());
			return false;
		}
		else
		{
			if (database_index_->GetResults().empty() == false)
			{
				size_t count = 0;

				try
				{
					count = lexical_cast<size_t>(database_index_->GetResults()[0][0]);
				}
				catch (bad_lexical_cast &e)
				{
					CONSOLE_LOG("Failed to do a lexical_cast");
					return false;
				}
				if (count == 0)
				{
					string s4 = "%6% insert into Ppvideo_Index_Resource_%8% values(\'%1%\',%2%,%3%,%4%,\'%5%\',\'%7%\',%9%,%10%)";
					string sql4 = boost::str(boost::format(s4)
						%rid.ToString()%file_length%block_size%block_num%block_md5s%" "%content_md5.ToString()%rid_mod%0%rid_key );

					if (!(database_index_->Query(sql4)))
					{
						CONSOLE_LOG("Failed to Insert RID "<<rid.ToString());
						return false;
					}
					//RELEASE_LOG("Insert RID " << rid.ToString() << "; MOD = " << rid_mod);
				}
				else
				{
					//RELEASE_LOG("Clash RID " << rid.ToString() << "; MOD = " << rid_mod);
				}
			}
		}

		string s6 = "select count(0) from Ppvideo_Index_RID_Visit_Date_%2% where RidKey = %3% and ResourceID = \'%1%\'";
		string sql6 = boost::str(boost::format(s6)%rid.ToString()%rid_mod%rid_key );
		if (!(database_statistic_->Query(sql6)))
		{
			CONSOLE_LOG("Failed to Select RID "<<rid.ToString());
			return false;
		}
		else
		{
			if (database_statistic_->GetResults().empty() == false)
			{
				size_t count = 0;

				try
				{
					count = lexical_cast<size_t>(database_statistic_->GetResults()[0][0]);
				}
				catch (bad_lexical_cast &e)
				{
					CONSOLE_LOG("Failed to do a lexical_cast");
					return false;
				}
				if (count == 0)
				{
					string sql3 = boost::str(boost::format("insert into Ppvideo_Index_RID_Visit_Date_%1% values(\'%2%\',%3%,%4%)"
														   ) %rid_mod %rid.ToString() %time(NULL)%rid_key);
					if(!(database_statistic_->Query(sql3)))
					{
						CONSOLE_LOG("Failed to Insert RID Visited Date, RID = "<<rid.ToString());
						return false;
					}
				}
			}
		}

		return true;
	}
	*/

	string DataMigration::Url2Mini(string url, string refer)
	{
		string url_tmp;
		if (refer.find("youku") == string::npos)
		{
			url_tmp = network::Uri(url).getrequest();
		}
		else
		{
			url_tmp = network::Uri(url).getfilerequest();
		}

		return url_tmp;
	}
/*
	size_t DataMigration::GetUrlNum()
	{
		string sql = "select count(0) from ppvideo_index_httpserver";
		if (!database_from_->Query(sql))
		{
			CONSOLE_LOG("Failed to Get URL Num");
			return 0;
		}
		size_t num;
		try
		{
			num = boost::lexical_cast<size_t>(database_from_->GetResults()[0][0]);
		}
		catch (bad_lexical_cast &e)
		{
			CONSOLE_LOG("Failed to Get URL Num");
			num = 0;
		}

		return num;
	}
*/
	void DataMigration::OnTimerElapsed(framework::timer::Timer::p pointer, u_int times)
	{
		if( is_running_ == false ) return;
		if (is_finished_ && pointer == print_sample_timer_)
		{
			CONSOLE_OUTPUT("Finished and Cost Time " << time(NULL)-time_start_ << "s");
			print_sample_timer_->Stop();
		}
	}

/*	void DataMigration::CountMiniUrlBytes()
	{
		if (is_running_ == false)
		{
			return;
		}
		CONSOLE_OUTPUT("please wait...");

		size_t count = 0, num = 0, failed_num = 0;
		while (true)
		{
			vector<string> url_line = GetURL();

			if (url_line.empty())
			{
				break;
			}

			if (url_line.size() != 5)
			{
				failed_num++;
				continue;
			}


			num++;

			string url = url_line[0];
			string refer = url_line[1];

			string mini = Url2Mini(url, refer);

			count += mini.size();
		}

		CONSOLE_OUTPUT("Total MiniUrl Length: " << count);
		CONSOLE_OUTPUT("Total MiniUrl Num: " << num-failed_num << "/" << num);
	}
*/
	void DataMigration::MigrateURL()
	{
		if (is_running_ == false) return;

		print_sample_timer_->Start();
		time_start_ = time(NULL);

		size_t time_start_per = time(NULL);

		CONSOLE_OUTPUT("Please wait...");

		UINT64 total_num = 0, failed_num = 0, clash_count = 0;

		string url_temp_string[256], visit_date_tmp_str[256];
		size_t url_temp_string_length[256], max_l = 0;
		const size_t max_length = 1024*100;

		for (int i = 0; i < 256; i ++)
		{
			url_temp_string[i] = "";
			visit_date_tmp_str[i] = "";
			url_temp_string_length[i] = 0;
		}

		while (true)
		{
			vector<string> url_line = GetURL();

			if (url_line.empty())
			{
				break;
			}

			total_num ++;

			if (url_line.size() != 5+1)
			{
// 				string tem_output;
// 				for (int j = 0; j < url_line.size(); j ++)
// 					tem_output += url_line[j]+";";
// 				CONSOLE_LOG("Bad Line: " << tem_output);

				failed_num++;

				continue;
			}

			
			string url = url_line[0];
			string refer = url_line[1];
			string type = url_line[2];
			string rid = url_line[3];
			string mini = url_line[4];

			mini = Url2Mini(url, refer);

			size_t url_key = boost::hash_value(mini);
			size_t url_mod = url_key%256;

// 
// 			MemCacheClient::MemRequest oItem;
// 			oItem.mKey = mini;
			if (filter_->HasString(mini.c_str()) == false)
			{
				filter_->AddString(mini.c_str());
				if (url_temp_string[url_mod] == "")
				{
					string s4 = "insert into Ppvideo_Index_HttpServer_%5% values (\'%1%\',\'%2%\',%3%,\'%4%\',\'%6%\',%7%)";
					string sql4 = boost::str(boost::format(s4) %mini %refer %type %rid %url_mod %url%url_key);
					url_temp_string[url_mod] = sql4;
					url_temp_string_length[url_mod] = 1;
				}
				else
				{
					string s4 = "%5%,(\'%1%\',\'%2%\',%3%,\'%4%\',\'%6%\',%7%)";
					string sql4 = boost::str(boost::format(s4) %mini %refer %type %rid %" " %url%url_key);
					url_temp_string[url_mod] += sql4;
					url_temp_string_length[url_mod] ++;
					if (url_temp_string_length[url_mod] > max_l)
						max_l = url_temp_string_length[url_mod];
				}

				if (visit_date_tmp_str[url_mod] == "")
				{
					string sql3 = boost::str(boost::format("insert into Ppvideo_Index_Url_Visit_Date_%1% values(\'%2%\',%3%, %4%)"
						) %url_mod %mini %time(NULL)%url_key);
					visit_date_tmp_str[url_mod] = sql3;
				}
				else
				{
					string sql3 = boost::str(boost::format("%1%,(\'%2%\',%3%, %4%)"
						) %" " %mini %time(NULL)%url_key);
					visit_date_tmp_str[url_mod] += sql3;
				}

			}
			else
			{
				RELEASE_LOG("Clash MiniURL = :" << mini);
				clash_count++;
			}

			if (url_temp_string[url_mod].size() > max_length)
			{
				if (!(database_index_->Query(url_temp_string[url_mod])))
				{
					CONSOLE_LOG("Failed to Insert URL ");
				}
				url_temp_string[url_mod] = "";
				url_temp_string_length[url_mod] = 0;
			}
			if (visit_date_tmp_str[url_mod].size() > max_length)
			{
				if(!(database_statistic_->Query(visit_date_tmp_str[url_mod])))
				{
					CONSOLE_LOG("Failed to Insert URL Visited Date");
				}
				visit_date_tmp_str[url_mod] = "";
			}
		}

		for (int i = 0; i < 256; i ++)
		{
			if (url_temp_string[i] != "")
			{
				if (!(database_index_->Query(url_temp_string[i])))
				{
					CONSOLE_LOG("Failed to Insert URL ");
				}
			}
			if (visit_date_tmp_str[i] != "")
			{
				if(!(database_statistic_->Query(visit_date_tmp_str[i])))
				{
					CONSOLE_LOG("Failed to Insert URL Visited Date");
				}
			}
		}

		CONSOLE_OUTPUT("Succeed: " << total_num-failed_num << "/" << total_num);
		CONSOLE_OUTPUT("Clashed: " << clash_count << "/" << total_num);

		filter_->Clear();

		is_finished_ = true;
	}

	void DataMigration::MigrateRID()
	{
		if (is_running_ == false) return;

		print_sample_timer_->Start();
		time_start_ = time(NULL);

		size_t time_start_per = time(NULL);

		CONSOLE_OUTPUT("Please wait...");

		UINT64 total_num = 0, failed_num = 0;

		string rid_temp_string[256], visit_date_tmp_str[256];
		size_t rid_temp_string_length[256], max_l = 0;
		const size_t max_length = 1024*100;

		for (int i = 0; i < 256; i ++)
		{
			rid_temp_string[i] = "";
			visit_date_tmp_str[i] = "";
			rid_temp_string_length[i] = 0;
		}

		while(true)
		{
			vector<string> rid_line = GetRID();

			if (rid_line.empty())
			{
				break;
			}

			total_num ++;

			if (rid_line.size() != 7+1)
			{
// 				string tem_output;
// 				for (int j = 0; j < rid_line.size(); j ++)
// 					tem_output += rid_line[j]+";";
// 				CONSOLE_LOG("Bad Line: " << tem_output);
				failed_num++;
				continue;
			}

			RID rid;
			rid.Parse(rid_line[0]);
			if (rid.IsEmpty())
			{
				continue;
			}
			string file_length = rid_line[1];
			string block_size = rid_line[2];
			string block_num = rid_line[3];
			string block_md5s = rid_line[4];

			size_t rid_key = boost::hash_value(rid);
			size_t rid_mod = rid_key%256;

			MD5 content_md5 = Guid();

			//MemCacheClient::MemRequest oItem;
			//oItem.mKey = rid_line[0];
			//CONSOLE_LOG(rid_line[0]);
			//if (mem_cache_client_->Get(oItem) == 0)
			{
				//mem_cache_client_->Set(oItem);
				if (rid_temp_string[rid_mod] == "")
				{
					string s4 = "%6% insert into Ppvideo_Index_Resource_%8% values(\'%1%\',%2%,%3%,%4%,\'%5%\',\'%7%\',%9%,%10%)";
					string sql4 = boost::str(boost::format(s4)
						%rid.ToString()%file_length%block_size%block_num%block_md5s%" "%content_md5.ToString()%rid_mod%0%rid_key );

					rid_temp_string[rid_mod] = sql4;
					rid_temp_string_length[rid_mod] = 1;
				}
				else
				{
					string s4 = "%6% ,(\'%1%\',%2%,%3%,%4%,\'%5%\',\'%7%\',%8%,%9%)";
					string sql4 = boost::str(boost::format(s4)
						%rid.ToString()%file_length%block_size%block_num%block_md5s%" "%content_md5.ToString()%0%rid_key );
					rid_temp_string[rid_mod] += sql4;
					rid_temp_string_length[rid_mod] ++;
					if (rid_temp_string_length[rid_mod] > max_l)
						max_l = rid_temp_string_length[rid_mod];
				}

				if (visit_date_tmp_str[rid_mod] == "")
				{
					string sql3 = boost::str(boost::format("insert into Ppvideo_Index_RID_Visit_Date_%1% values(\'%2%\',%3%,%4%)"
						) %rid_mod %rid.ToString() %time(NULL)%rid_key);
					visit_date_tmp_str[rid_mod] = sql3;
				}
				else
				{
					string sql3 = boost::str(boost::format("%1%,(\'%2%\',%3%,%4%)"
						) %" " %rid.ToString() %time(NULL)%rid_key);
					visit_date_tmp_str[rid_mod] += sql3;
				}

			}

			if (rid_temp_string[rid_mod].size() > max_length)
			{
				if (!(database_index_->Query(rid_temp_string[rid_mod])))
				{
					CONSOLE_LOG("Failed to Insert RID ");
				}
				rid_temp_string[rid_mod] = "";
				rid_temp_string_length[rid_mod] = 0;
			}
			if (visit_date_tmp_str[rid_mod].size() > max_length)
			{
				if(!(database_statistic_->Query(visit_date_tmp_str[rid_mod])))
				{
					CONSOLE_LOG("Failed to Insert RID Visited Date");
				}
				visit_date_tmp_str[rid_mod] = "";
			}
		}

		for (int i = 0; i < 256; i ++)
		{
			if (rid_temp_string[i] != "")
			{
				if (!(database_index_->Query(rid_temp_string[i])))
				{
					CONSOLE_LOG("Failed to Insert RID ");
				}
			}
			if (visit_date_tmp_str[i] != "")
			{
				if(!(database_statistic_->Query(visit_date_tmp_str[i])))
				{
					CONSOLE_LOG("Failed to Insert RID Visited Date");
				}
			}
		}

		CONSOLE_LOG("Succeed: "<<total_num-failed_num << "/" << total_num);
		is_finished_ = true;
	}
/*
	size_t DataMigration::GetRIDNum()
	{
		string sql = "select count(0) from ppvideo_index_resource";
		if (!database_from_->Query(sql))
		{
			CONSOLE_LOG("Failed to Get RID Num");
			return 0;
		}
		size_t num;
		try
		{
			num = boost::lexical_cast<size_t>(database_from_->GetResults()[0][0]);
		}
		catch (bad_lexical_cast &e)
		{
			CONSOLE_LOG("Failed to Get RID Num");
			num = 0;
		}

		return num;
	}
	*/
}