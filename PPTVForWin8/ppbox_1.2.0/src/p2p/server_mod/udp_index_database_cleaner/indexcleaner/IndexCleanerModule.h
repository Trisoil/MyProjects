#pragma once
#include "stdafx.h"
#include "Config.h"
#include "MySQLp.h"
#include "BloomFilter.h"
#include "DirTraversing.h"
#include <boost/shared_ptr.hpp>

using namespace framework;
using namespace udpindex;
using namespace udpstatistic;

namespace indexcleaner
{
	class IndexCleanerModule
		: public boost::noncopyable
		, public boost::enable_shared_from_this<IndexCleanerModule>
	{
	public:
		typedef boost::shared_ptr<IndexCleanerModule> p;
		static p Create() { return p(new IndexCleanerModule); }

		static bool is_finished_url;
		static bool is_finished_rid;

	public:
		void Start();
		void Stop();
		void ClearRidTable(size_t days_byond_now);
		void ClearUrlTable(size_t days_byond_now);
		void ClearDatabase(size_t days_byond_now);
		void CountUrlHaveMark();
		bool IsRunning() const { return is_running_; }

	private:
		bool DelRid(RID rid);
		bool DelUrl(string url);

		bool ClearRID(size_t mod);
		bool ClearURL(size_t mod);
		bool ClearCTT(size_t mod);

		string FullPath(const string &path_str, const string &file_name);
		double FileName2Days(string file_name);

		inline string SysTime2Str(SYSTEMTIME sys_time);
	private:
		IndexCleanerModule() : is_running_(false),is_clear_db_(false){}
		static IndexCleanerModule::p inst_;
		bool is_running_;

	public:
		static IndexCleanerModule::p Inst() { return inst_; };

	private:
		MySQLp::p mysql_;

		string url_file_path_;
		string rid_file_path_;

		DirTraversing::p rid_dir_;
		DirTraversing::p url_dir_;
		DirTraversing::p rid_dir_bak_;
		DirTraversing::p url_dir_bak_;

	public:
		bool is_clear_db_;

	private:
		const static size_t max_mod_ = 256;

		size_t rid_cache_size_now_[max_mod_];
		size_t url_cache_size_now_[max_mod_];
		size_t ctt_cache_size_now_[max_mod_];

		set<pair<size_t/*RidKey*/, Guid/*RID*/>> rid_s_[max_mod_];
		set<pair<size_t/*UrlKey*/, string/*MiniURL*/>> url_s_[max_mod_];
		set<pair<size_t/*FileLength*/,pair<size_t/*ContentKey*/,Guid/*ContentMD5*/>>> ctt_s_[max_mod_];

		size_t max_cache_size_;
		size_t cycle_time_;
	};
	class ClearDB
	{
		IndexCleanerModule::p index_cleaner_;
	public:
		ClearDB(IndexCleanerModule::p index_cleaner):index_cleaner_(index_cleaner){index_cleaner_->is_clear_db_ = true;}
		~ClearDB(){index_cleaner_->is_clear_db_ = false;}
	};

}