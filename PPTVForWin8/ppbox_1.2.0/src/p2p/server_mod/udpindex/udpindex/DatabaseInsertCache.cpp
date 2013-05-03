#include "StdAfx.h"
#include "DatabaseInsertCache.h"
#include "Config.h"
#include "IndexFunc.h"

namespace udpindex
{
	DatabaseInsertCache::p DatabaseInsertCache::inst_ = DatabaseInsertCache::p(new DatabaseInsertCache());

	void DatabaseInsertCache::Start()
	{
		if (is_running_ == true) return;

		url_cache_size_ = Config::Inst().GetInteger("index.urlcachesize");
		rid_cache_size_ = Config::Inst().GetInteger("index.ridcachesize");
		content_cache_size_ = Config::Inst().GetInteger("index.contentcachesize");

		if (url_cache_size_ <= 0)
		{
			CONSOLE_OUTPUT("The URL Insert Cache Size is a Zero");
			url_cache_size_ = 10240;
		}
		if (rid_cache_size_ <= 0)
		{
			CONSOLE_OUTPUT("The RID Insert Cache Size is a Zero");
			rid_cache_size_ = 10240;
		}
		if (content_cache_size_ <= 0)
		{
			CONSOLE_OUTPUT("The Content Insert Cache Size is a Zero");
			content_cache_size_ = 10240;
		}

		is_running_ = true;
	}
	void DatabaseInsertCache::Stop()
	{
		if (is_running_ == false) return;
		is_running_ = false;
	}

	bool DatabaseInsertCache::ClearRids(size_t mod, vector<string> &sql_str_s)
	{
		if (is_running_ == false) return false;

		if (mod < max_mod_)
		{
			Lock lock(&rid_dbc_s_[mod].critical_section_);

			string str_tmp = "";
			while(rid_dbc_s_[mod].rid_info_s_.empty() == false)
			{
				string md5s_str = "";
				hash_map<RID, RidStruct>::iterator it = rid_dbc_s_[mod].rid_info_s_.begin();

				for(size_t j = 0; j < it->second.block_md5_s_.size(); j++)
				{
					md5s_str += it->second.block_md5_s_[j].ToString();
				}

				if (str_tmp.empty())
				{
					str_tmp += boost::str(boost::format("insert into Ppvideo_Index_Resource_%1% values ")%mod);
					str_tmp += boost::str(boost::format("(\'%1%\',%2%,%3%,%4%,\'%5%\',\'%6%\',%7%,%8%)")
						%it->second.rid_.ToString() %it->second.file_length_ %it->second.block_size_ 
						%it->second.block_num_	%md5s_str % it->second.content_md5_.ToString()
						%it->second.content_bytes_ %boost::hash_value(it->second.rid_));
				}
				else
				{
					str_tmp += boost::str(boost::format(",(\'%1%\',%2%,%3%,%4%,\'%5%\',\'%6%\',%7%,%8%)")
						%it->second.rid_.ToString() %it->second.file_length_ %it->second.block_size_ 
						%it->second.block_num_	%md5s_str % it->second.content_md5_.ToString()
						%it->second.content_bytes_ %boost::hash_value(it->second.rid_));
				}

				rid_dbc_s_[mod].rid_info_s_.erase(it);

				if (str_tmp.size() >= rid_cache_size_ || rid_dbc_s_[mod].rid_info_s_.empty())
				{
					sql_str_s.push_back(str_tmp);
					str_tmp = "";
					break;
				}
			}

			return true;
		}
		else
		{
			return false;
		}		
	}
	bool DatabaseInsertCache::ClearUrls(size_t mod, vector<string> &sql_str_s)
	{
		if (is_running_ == false) return false;

		if (mod < max_mod_)
		{
			Lock lock(&url_dbc_s_[mod].critical_section_);

			string str_tmp = "";
			while(url_dbc_s_[mod].url_info_s_.empty() == false)
			{
				hash_map<string, UrlStruct>::iterator it = url_dbc_s_[mod].url_info_s_.begin();

				if (str_tmp.empty())
				{
					str_tmp += boost::str(boost::format("insert into Ppvideo_Index_HttpServer_%1% values ")%mod);
					str_tmp += boost::str(boost::format("(\'%1%\',\'%2%\',%3%,\'%4%\',\'%5%\',%6%)")
						%it->first %it->second.refer_url_ %it->second.type_ 
						%it->second.rid_ %it->second.url_ %boost::hash_value(it->first));
				}
				else
				{
					str_tmp += boost::str(boost::format(",(\'%1%\',\'%2%\',%3%,\'%4%\',\'%5%\',%6%)")
						%it->first %it->second.refer_url_ %it->second.type_ 
						%it->second.rid_ %it->second.url_ %boost::hash_value(it->first));
				}

				url_dbc_s_[mod].url_info_s_.erase(it);

				if (str_tmp.size() >= url_cache_size_ || url_dbc_s_[mod].url_info_s_.empty())
				{
					sql_str_s.push_back(str_tmp);
					str_tmp = "";
					break;
				}
			}

			return true;
		}
		else
		{
			return false;
		}
	}
	bool DatabaseInsertCache::ClearCtts(size_t mod, vector<string> &sql_str_s)
	{
		if (is_running_ == false) return false;

		if (mod < max_mod_)
		{
			Lock lock(&ctt_dbc_s_[mod].critical_section_);

			string str_tmp = "";
			while(ctt_dbc_s_[mod].ctt_info_s_.empty() == false)
			{
				hash_map<pair_hash, ContentStruct>::iterator it = ctt_dbc_s_[mod].ctt_info_s_.begin();

				if (str_tmp.empty())
				{
					str_tmp += boost::str(boost::format("insert into Ppvideo_Index_Content_%1% values ")%mod);
					str_tmp += boost::str(boost::format("(\'%1%\', %2%, %6%, %4%,\'%3%\', %5%)")
						%it->second.content_md5_.ToString() %it->second.file_length_ %it->second.rid_.ToString()
						%it->second.content_bytes_ %boost::hash_value(it->second.content_md5_)
						%it->second.is_clash_);
				}
				else
				{
					str_tmp += boost::str(boost::format(",(\'%1%\', %2%, %6%, %4%,\'%3%\', %5%)")
						%it->second.content_md5_.ToString() %it->second.file_length_ %it->second.rid_.ToString()
						%it->second.content_bytes_ %boost::hash_value(it->second.content_md5_)
						%it->second.is_clash_);
				}

				ctt_dbc_s_[mod].ctt_info_s_.erase(it);

				if (str_tmp.size() >= content_cache_size_ || ctt_dbc_s_[mod].ctt_info_s_.empty())
				{
					sql_str_s.push_back(str_tmp);
					str_tmp = "";
					break;
				}
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	vector<size_t> DatabaseInsertCache::CacheSizes(size_t cache_num)
	{
		if (is_running_ == false) return vector<size_t>();

		vector<size_t> sizes;
		if (cache_num == 1)
		{
			for (size_t i = 0; i < max_mod_; i++)
			{
				Lock lock(&rid_dbc_s_[i].critical_section_);

				sizes.push_back(rid_dbc_s_[i].rid_info_s_.size());
			}
		}
		else if (cache_num == 2)
		{
			for (size_t i = 0; i < max_mod_; i++)
			{
				Lock lock(&url_dbc_s_[i].critical_section_);

				sizes.push_back(url_dbc_s_[i].url_info_s_.size());
			}
		}
		else if (cache_num == 3)
		{
			for (size_t i = 0; i < max_mod_; i++)
			{
				Lock lock(&ctt_dbc_s_[i].critical_section_);

				sizes.push_back(ctt_dbc_s_[i].ctt_info_s_.size());
			}
		}
		return sizes;
	}

	vector<RID> DatabaseInsertCache::RIDs(size_t mod)
	{
		if (is_running_ == false) return vector<RID>();

		vector<RID> rids;
		if (mod < max_mod_)
		{
			Lock lock(&rid_dbc_s_[mod].critical_section_);
			for (hash_map<RID,RidStruct>::iterator it = rid_dbc_s_[mod].rid_info_s_.begin(); it != rid_dbc_s_[mod].rid_info_s_.end(); it++)
			{
				rids.push_back(it->first);
			}
		}
		return rids;
	}
	vector<string> DatabaseInsertCache::URLs(size_t mod)
	{
		if (is_running_ == false) return vector<string>();

		vector<string> urls;
		if (mod < max_mod_)
		{
			Lock lock(&url_dbc_s_[mod].critical_section_);
			for (hash_map<string,UrlStruct>::iterator it = url_dbc_s_[mod].url_info_s_.begin(); it != url_dbc_s_[mod].url_info_s_.end(); it++)
			{
				urls.push_back(it->first);
			}		
		}
		return urls;
	}
	vector<pair<MD5,size_t>> DatabaseInsertCache::Contents(size_t mod)
	{
		if (is_running_ == false) return vector<pair<MD5,size_t>>();

		vector<pair<MD5,size_t>> contents;
		if (mod < max_mod_)
		{
			Lock lock(&ctt_dbc_s_[mod].critical_section_);
			for (hash_map<pair_hash,ContentStruct>::iterator it = ctt_dbc_s_[mod].ctt_info_s_.begin(); it != ctt_dbc_s_[mod].ctt_info_s_.end(); it++)
			{
				contents.push_back(it->first.pr_);
			}
		}
		return contents;
	}

	bool DatabaseInsertCache::Insert(size_t mod, const RidStruct &rid_info)
	{
		if (is_running_ == false) return false;

		if (mod < max_mod_)
		{
			Lock lock(&rid_dbc_s_[mod].critical_section_);
			rid_dbc_s_[mod].rid_info_s_.insert(make_pair(rid_info.rid_, rid_info));
			
			return true;
		}
		else
		{
			return false;
		}
	}
	bool DatabaseInsertCache::Insert(size_t mod, const UrlStruct &url_info)
	{
		if (is_running_ == false) return false;

		if (mod < max_mod_)
		{
			string mini = Url2Mini(url_info.url_, url_info.refer_url_);
				
			Lock lock(&url_dbc_s_[mod].critical_section_);
			url_dbc_s_[mod].url_info_s_.insert(make_pair(mini, url_info));
			
			return true;
		}
		else
		{
			return false;
		}
	}
	bool DatabaseInsertCache::Insert(size_t mod, const ContentStruct &content_info)
	{
		if (is_running_ == false) return false;

		if (mod < max_mod_)
		{
			Lock lock(&ctt_dbc_s_[mod].critical_section_);
			ctt_dbc_s_[mod].ctt_info_s_.insert(make_pair(make_pair(content_info.content_md5_,content_info.file_length_), content_info));
			
			return true;
		}
		else
		{
			return false;
		}
	}

	bool DatabaseInsertCache::Update(size_t mod, const UrlStruct &url_info)
	{
		if (is_running_ == false) return false;

		if (mod < max_mod_)
		{
			string mini = Url2Mini(url_info.url_,url_info.refer_url_);
			
			Lock lock(&url_dbc_s_[mod].critical_section_);

			hash_map<string, UrlStruct>::iterator it = url_dbc_s_[mod].url_info_s_.find(mini);
			if (it != url_dbc_s_[mod].url_info_s_.end())
			{
				it->second = url_info;
				return true;
			}

			return false;
		}
		else
		{
			return false;
		}
	}
	bool DatabaseInsertCache::Update(size_t mod, const RidStruct &rid_info)
	{
		if (is_running_ == false) return false;

		if (mod < max_mod_)
		{
			Lock lock(&rid_dbc_s_[mod].critical_section_);

			hash_map<RID,RidStruct>::iterator it = rid_dbc_s_[mod].rid_info_s_.find(rid_info.rid_);
			if (it != rid_dbc_s_[mod].rid_info_s_.end())
			{
				it->second = rid_info;
				return true;
			}
			return false;
		}
		else
		{
			return false;
		}
	}
	bool DatabaseInsertCache::Update(size_t mod, const ContentStruct &content_info)
	{
		if (is_running_ == false) return false;

		if (mod < max_mod_)
		{
			Lock lock(&ctt_dbc_s_[mod].critical_section_);

			hash_map<pair_hash, ContentStruct>::iterator it = ctt_dbc_s_[mod].ctt_info_s_.find(make_pair(content_info.content_md5_,content_info.file_length_));
			if (it != ctt_dbc_s_[mod].ctt_info_s_.end())
			{
				it->second.is_clash_ = 1;
				return true;
			}
			return false;
		}
		else
		{
			return false;
		}
	}

	bool DatabaseInsertCache::Get(size_t mod, const RID &rid, RidStruct &rid_struct)
	{
		if (is_running_ == false) return false;

		if (mod < max_mod_)
		{
			Lock lock(&rid_dbc_s_[mod].critical_section_);
			hash_map<RID, RidStruct>::iterator it = rid_dbc_s_[mod].rid_info_s_.find(rid);
			if (it != rid_dbc_s_[mod].rid_info_s_.end())
			{
				rid_struct = it->second;
				return true;
			}
			return false;
		}
		else
		{
			return false;
		}
	}
	bool DatabaseInsertCache::Get(size_t mod, const string &mini_url, UrlStruct &url_struct)
	{
		if (is_running_ == false) return false;

		if (mod < max_mod_)
		{			
			Lock lock(&url_dbc_s_[mod].critical_section_);
			hash_map<string, UrlStruct>::iterator it = url_dbc_s_[mod].url_info_s_.find(mini_url);
			if (it != url_dbc_s_[mod].url_info_s_.end())
			{
				url_struct = it->second;
				return true;
			}
			return false;
		}
		else
		{
			return false;
		}
	}
	bool DatabaseInsertCache::Get(size_t mod, const pair_hash &content_pair, ContentStruct &ctt_struct)
	{
		if (is_running_ == false) return false;

		if (mod < max_mod_)
		{
			Lock lock(&ctt_dbc_s_[mod].critical_section_);
			hash_map<pair_hash, ContentStruct>::iterator it = ctt_dbc_s_[mod].ctt_info_s_.find(content_pair);
			if (it != ctt_dbc_s_[mod].ctt_info_s_.end())
			{
				ctt_struct = it->second;
				return true;
			}
			return false;
		}
		else
		{
			return false;
		}
	}

	bool DatabaseInsertCache::IsExist(size_t mod, const RID &rid, MD5 &ctt)
	{
		if (is_running_ == false) return false;

		if (mod < max_mod_)
		{
			Lock lock(&rid_dbc_s_[mod].critical_section_);
			hash_map<RID, RidStruct>::iterator it = rid_dbc_s_[mod].rid_info_s_.find(rid);
			if (it != rid_dbc_s_[mod].rid_info_s_.end())
			{
				ctt = it->second.content_md5_;
				return true;
			}
			return false;
		}
		else
		{
			return false;
		}
	}
	bool DatabaseInsertCache::IsExist(size_t mod, const string &mini_url, RID &rid)
	{
		if (is_running_ == false) return false;

		if (mod < max_mod_)
		{	
			Lock lock(&url_dbc_s_[mod].critical_section_);
			hash_map<string, UrlStruct>::iterator it = url_dbc_s_[mod].url_info_s_.find(mini_url);
			if (it != url_dbc_s_[mod].url_info_s_.end())
			{
				rid = it->second.rid_;
				return true;
			}
			return false;
		}
		else
		{
			return false;
		}
	}
	bool DatabaseInsertCache::IsExist(size_t mod, const pair_hash &content_pair, int &is_clash, RID &rid)
	{
		if (is_running_ == false) return false;

		if (mod < max_mod_)
		{
			Lock lock(&ctt_dbc_s_[mod].critical_section_);
			hash_map<pair_hash, ContentStruct>::iterator it = ctt_dbc_s_[mod].ctt_info_s_.find(content_pair);
			if (it != ctt_dbc_s_[mod].ctt_info_s_.end())
			{
				is_clash = it->second.is_clash_;
				rid = it->second.rid_;
				return true;
			}
			return false;
		}
		else
		{
			return false;
		}
	}
}
