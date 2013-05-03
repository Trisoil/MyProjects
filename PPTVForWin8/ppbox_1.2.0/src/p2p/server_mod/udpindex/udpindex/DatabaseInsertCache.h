#pragma once
//用于缓存数据库插入的Cache
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include "base/guid.h"
#include "UdpIndexModule.h"
#include "Lock.h"

namespace udpindex
{
	struct ContentStruct //Content信息结构
	{
		MD5 content_md5_;
		size_t file_length_;
		RID rid_;
		size_t content_bytes_;
		u_short is_clash_;

		ContentStruct():content_md5_(),file_length_(0),rid_(),content_bytes_(0),is_clash_(0){}
	};
	struct RidStruct //Resource信息结构
	{
		RID rid_;
		MD5 content_md5_;
		vector<MD5> block_md5_s_;
		size_t block_num_;
		size_t block_size_;
		size_t file_length_;
		size_t content_bytes_;

		RidStruct():rid_(),content_md5_(),block_md5_s_(),block_num_(0),block_size_(0),file_length_(0),content_bytes_(0){}
	};
	struct UrlStruct //URL信息结构
	{
		string url_;
		string refer_url_;
		u_short type_;
		RID rid_;

		UrlStruct():url_(),refer_url_(),type_(0),rid_(){}
	};

	struct UrlDBCache
	{
		hash_map<string, UrlStruct> url_info_s_;
		CriticalSection critical_section_;
	};
	struct RidDBCache
	{
		hash_map<RID, RidStruct> rid_info_s_;
		CriticalSection critical_section_;
	};
	struct CttDBCache
	{
		hash_map<pair_hash, ContentStruct> ctt_info_s_;
		CriticalSection critical_section_;
	};

	class DatabaseInsertCache
		:public boost::enable_shared_from_this<DatabaseInsertCache>
		,public boost::noncopyable
	{
	public:
		typedef boost::shared_ptr<DatabaseInsertCache> p;
		static DatabaseInsertCache &Inst(){return *inst_;};

	public:

		bool Insert(size_t mod, const RidStruct &rid_info);
		bool Insert(size_t mod, const UrlStruct &url_info);
		bool Insert(size_t mod, const ContentStruct &content_info);

		bool Update(size_t mod, const UrlStruct &url_info);
		bool Update(size_t mod, const RidStruct &rid_info);
		bool Update(size_t mod, const ContentStruct &content_info);		

		bool Get(size_t mod, const RID &rid, RidStruct &rid_struct);
		bool Get(size_t mod, const string &mini_url, UrlStruct &url_struct);
		bool Get(size_t mod, const pair_hash &content_pair, ContentStruct &ctt_struct);

		bool IsExist(size_t mod, const RID &rid, MD5 &ctt);
		bool IsExist(size_t mod, const string &mini_url, RID &rid);
		bool IsExist(size_t mod, const pair_hash &content_pair, int &is_clash, RID &rid);

		bool ClearRids(size_t mod, vector<string> &sql_str_s);
		bool ClearUrls(size_t mod, vector<string> &sql_str_s);
		bool ClearCtts(size_t mod, vector<string> &sql_str_s);

		vector<size_t> CacheSizes(size_t cache_num);
		vector<RID> RIDs(size_t mod);
		vector<string> URLs(size_t mod);
		vector<pair<MD5,size_t>> Contents(size_t mod);

	public:
		~DatabaseInsertCache(){}

	private:
		DatabaseInsertCache():is_running_(false){}

	public:
		void Start();
		void Stop();

	private:
		size_t url_cache_size_;
		size_t rid_cache_size_;
		size_t content_cache_size_;

		static p inst_;

	private:
		const static size_t max_mod_ = 256;

		RidDBCache rid_dbc_s_[max_mod_];
		UrlDBCache url_dbc_s_[max_mod_];
		CttDBCache ctt_dbc_s_[max_mod_];

		bool is_running_;
	};

}