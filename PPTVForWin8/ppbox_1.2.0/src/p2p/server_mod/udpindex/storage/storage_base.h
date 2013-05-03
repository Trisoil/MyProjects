#pragma once
#include "base\base.h"
#include <list>

using namespace base;
using std::list;

#pragma pack(push, 1)

#pragma  pack(pop)

#include "framework\util\random.h"
namespace storage
{

	enum{
		ERROR_UNKOWNED,
		ERROR_GET_SUBPIECE_RESOURCE_NOT_EXIST,
		ERROR_GET_SUBPIECE_OUTOFRANGE,
		ERROR_GET_SUBPIECE_NOT_FIND_SUBPIECE,
		ERROR_GET_SUBPIECE_BLOCK_NOT_BEEN_PUBLISHED,
		ERROR_GET_SUBPIECE_BLOCK_VERIFY_FAILED,
		ERROR_GET_SUBPIECE_BLOCK_NOT_FULL,
		ERROR_ADD_SUBPIECE_BLOCK_BEEN_PUBLISHED,
		ERROR_GET_SUBPIECE_VERIFY_WRONG,
		ERROR_GET_SUBPIECE_NO_FILELENGTH,
		ERROR_GET_BLOCK_NOT_FIND_BLOCK
	};
	enum
	{
		INSTANCE_NEED_RESOURCE,
		INSTANCE_APPLY_RESOURCE,
		INSTANCE_HAVE_RESOURCE,
		INSTANCE_BEING_MERGED,
		INSTANCE_REMOVING,
		INSTANCE_STOP,
		INSTANCE_CLOSING
	};
	enum
	{
		NO_VERIFIED,
		HAS_BEEN_VERIFIED,
		BEING_VERIFIED
	};


#define STORAGE_LOG(a)     LOG(__DEBUG, "storage",__FUNCTION__<<" line:"<<__LINE__<<" "<<a)
#define STORAGE_ERR_LOG(a)  LOG(__ERROR, "storage", __FUNCTION__<<" line:"<<__LINE__<<" "<<a)
#define STORAGE_WARN_LOG(a)  LOG(__WARN, "storage", __FUNCTION__<<" line:"<<__LINE__<<" "<<a)
#define STORAGE_EVENT_LOG(a)  LOG(__EVENT, "storage",__FUNCTION__<<" line:"<<__LINE__<<" "<<a)
#define STORAGE_INFO_LOG(a)  LOG(__INFO, "storage", __FUNCTION__<<" line:"<<__LINE__<<" "<<a)
#define STORAGE_DEBUG_LOG(a)  LOG(__DEBUG, "storage",__FUNCTION__<<" line:"<<__LINE__<<" "<<a)
#define STORAGE_TEST_DEBUG(a) LOG(__DEBUG, "storage_test", a)

	static const tstring tpp_extname(TEXT(".tpp"));
	static const tstring cfg_extname(TEXT(".cfg"));
	static const int max_size_cfg_file_g_ = 2*1024*1024;
	static const int hash_len_g_ = 16;

	struct StorageConfig
	{
		u_short bytes_num_per_subpiece_g_;
		u_short subpiece_num_per_piece_g_;
		u_short piece_num_per_block_g_;
		u_int  max_block_num_g_ ;
	};

#define  bytes_num_per_subpiece_g_ base::SUB_PIECE_SIZE


	static StorageConfig storage_config;
//	static const u_short bytes_num_per_subpiece_g_ = u_short(1)<<10;
	static const u_short subpiece_num_per_piece_g_ = u_short(1)<<7;
	static const u_short piece_num_per_block_g_=   u_short(1)<<4; 
	static const u_int bytes_num_per_piece_g_ =   bytes_num_per_subpiece_g_*subpiece_num_per_piece_g_; 
	static const u_short default_subpiece_num_per_block_g_ = subpiece_num_per_piece_g_*piece_num_per_block_g_;//u_short(1)<<11;
	static const u_int  default_block_size_g_ = piece_num_per_block_g_*subpiece_num_per_piece_g_*bytes_num_per_subpiece_g_; 
	static const u_int  max_block_num_g_ = 50;
	static const string  default_cfg_key_g_ = "123456"; 
	static const u_int   default_delay_tickcount_for_delete = 10*1000;  //连续50天工作会有问题！

	static const u_int TRAFFIC_UNIT_TIME = 8*60*60;	// 格子的时间单位，秒
	static const u_int TRAFFIC_T0 = 7;				// T0，格子数
	static const u_int TRAFFIC_PROTECT_TIME = 2;	// 保护时间，格子数

	struct FileResourceInfo
	{
		RidInfo rid_info_; 
		vector<UrlInfo> url_info_;
		tstring file_path_;  //增加".tpp"扩展名表示下载未完成文件，下载状态文件固定为".tpp.cfg"扩展名
		size_t last_push_time_;
		list<size_t> traffic_list_;
		explicit FileResourceInfo() : rid_info_(), url_info_(), file_path_(), last_push_time_(0)
		{
			for(size_t i=0 ;i<rid_info_.block_count_;i++)
			{
				rid_info_.block_md5_s_.push_back(MD5());
			}
		}

		Buffer ToBuffer() const
		{
			Buffer tmp(2*1024*1024);
			size_t total_len = 0;
			BYTE *p = tmp.data_.get()+sizeof(UINT32);
			size_t &len =*(size_t*)p;

			//file name!
			len = file_path_.size()*2;
			p+=sizeof(size_t);
			memcpy(p,(BYTE*)file_path_.c_str(),len);
			p+=len;

			//Rid_info
			RidInfo &rid_info = *(RidInfo*)p;
			rid_info.rid_ = rid_info_.rid_;
			rid_info.block_count_ = rid_info_.block_count_;
			rid_info.file_length_ = rid_info_.file_length_;
			rid_info.block_size_ = rid_info_.block_size_;
			p+=((char*)&(rid_info.block_md5_s_)-(char*)&rid_info);
			size_t i = 0;
			for(;i<rid_info_.block_md5_s_.size();i++)
			{
				*(MD5*)p = rid_info_.block_md5_s_[i];
				p+=sizeof(MD5);
			}
			for(;i<rid_info_.block_count_-rid_info_.block_md5_s_.size();i++)
			{
				*(MD5*)p = MD5();
				p+=sizeof(MD5);
			}
			//URL
			*(size_t*)p = url_info_.size();
			p+=sizeof(size_t);
			for(i = 0;i<url_info_.size();i++)
			{
				*(u_short*)p = url_info_[i].type_;
				p+=sizeof(u_short);
				*(size_t*)p = url_info_[i].url_.size();
				p+=sizeof(size_t);
				memcpy(p,url_info_[i].url_.c_str(),url_info_[i].url_.size());
				p+=url_info_[i].url_.size();
				*(size_t*)p = url_info_[i].refer_url_.size();
				p+=sizeof(size_t);
				memcpy(p,url_info_[i].refer_url_.c_str(),url_info_[i].refer_url_.size());
				p+=url_info_[i].refer_url_.size();
			}
		// ----------------------------------------------
		// version2
			*(size_t*)p = last_push_time_;
			p+=sizeof(size_t);
			*(size_t*)p = traffic_list_.size();
			p+=sizeof(size_t);
			for (list<size_t>::const_iterator it = traffic_list_.begin(); it!=traffic_list_.end(); ++it)
			{
				*(size_t*)p = *it;
				p+=sizeof(size_t);
			}
		// ----------------------------------------------
			total_len = p-tmp.data_.get();
			*(size_t*)tmp.data_.get() = total_len;
			Buffer r_buf(tmp.data_.get(),total_len);
			return r_buf;
		}

		bool Parse(const Buffer buf)
		{
			if((buf.length_>=2*1024*1024)||(buf.length_<4))
			{
				STORAGE_ERR_LOG("FileResourceInfo::Parse Buffer size error! "<<buf.length_);
				return false;
			}

			BYTE *p = buf.data_.get();
			size_t &total_len = *(size_t*)p;
			if(total_len!=buf.length_)
			{
				STORAGE_ERR_LOG("FileResourceInfo::Parse length error! buf.length_:"<<buf.length_<<" total_len:"<<total_len);
				return false;
			}
			p+=sizeof(size_t);

			//file name!
			size_t len =*(size_t*)p;
			p+=sizeof(size_t);
			file_path_.clear();
			file_path_.append((TCHAR*)p,len/2);
			p+=len;

			//Rid_info
			RidInfo &rid_info = *(RidInfo*)p;
			rid_info_.rid_ = rid_info.rid_ ;
			rid_info_.block_count_ = rid_info.block_count_;
			rid_info_.file_length_ = rid_info.file_length_;
			rid_info_.block_size_ = rid_info.block_size_;
			p+=sizeof(rid_info.rid_)+sizeof(rid_info.block_count_)+sizeof(rid_info.file_length_)+sizeof(rid_info.block_size_);
			rid_info_.block_md5_s_.clear();
			size_t i = 0;
			for(;i<rid_info_.block_md5_s_.size();i++)
			{
				rid_info_.block_md5_s_[i] = (*(MD5*)p);
				p+=sizeof(MD5);
			}
			for(;i<rid_info_.block_count_;i++)
			{
				rid_info_.block_md5_s_.push_back(*(MD5*)p);
				p+=sizeof(MD5);
			}
			//URL
			url_info_.clear();
			size_t url_count = *(size_t*)p ;
			p+=sizeof(size_t);
			for(i = 0;i<url_count;i++)
			{
				UrlInfo t_url_info; 
				t_url_info.type_ = *(u_short*)p ;
				p+=sizeof(u_short);
				int url_len = *(int*)p ;
				p+=sizeof(int);
				t_url_info.url_.assign((char*)p,url_len);
				p+=url_len;
				size_t ref_len = *(size_t*)p;
				p+=sizeof(size_t);
				t_url_info.refer_url_.assign((char*)p,ref_len);
				p+=ref_len;
				url_info_.push_back(t_url_info);
			}
			return true;
		}

		// ---------------------------------------------------
		// version2
		bool Parse(const Buffer buf, bool)
		{
			if((buf.length_>=2*1024*1024)||(buf.length_<4))
			{
				STORAGE_ERR_LOG("FileResourceInfo::Parse Buffer size error! "<<buf.length_);
				return false;
			}

			BYTE *p = buf.data_.get();
			size_t &total_len = *(size_t*)p;
			if(total_len!=buf.length_)
			{
				STORAGE_ERR_LOG("FileResourceInfo::Parse length error! buf.length_:"<<buf.length_<<" total_len:"<<total_len);
				return false;
			}
			p+=sizeof(size_t);

			//file name!
			size_t len =*(size_t*)p;
			p+=sizeof(size_t);
			file_path_.clear();
			file_path_.append((TCHAR*)p,len/2);
			p+=len;

			//Rid_info
			RidInfo &rid_info = *(RidInfo*)p;
			rid_info_.rid_ = rid_info.rid_ ;
			rid_info_.block_count_ = rid_info.block_count_;
			rid_info_.file_length_ = rid_info.file_length_;
			rid_info_.block_size_ = rid_info.block_size_;
			p+=sizeof(rid_info.rid_)+sizeof(rid_info.block_count_)+sizeof(rid_info.file_length_)+sizeof(rid_info.block_size_);
			rid_info_.block_md5_s_.clear();
			size_t i = 0;
			for(;i<rid_info_.block_md5_s_.size();i++)
			{
				rid_info_.block_md5_s_[i] = (*(MD5*)p);
				p+=sizeof(MD5);
			}
			for(;i<rid_info_.block_count_;i++)
			{
				rid_info_.block_md5_s_.push_back(*(MD5*)p);
				p+=sizeof(MD5);
			}
			//URL
			url_info_.clear();
			size_t url_count = *(size_t*)p ;
			p+=sizeof(size_t);
			for(i = 0;i<url_count;i++)
			{
				UrlInfo t_url_info; 
				t_url_info.type_ = *(u_short*)p ;
				p+=sizeof(u_short);
				int url_len = *(int*)p ;
				p+=sizeof(int);
				t_url_info.url_.assign((char*)p,url_len);
				p+=url_len;
				size_t ref_len = *(size_t*)p;
				p+=sizeof(size_t);
				t_url_info.refer_url_.assign((char*)p,ref_len);
				p+=ref_len;
				url_info_.push_back(t_url_info);
			}
		// ----------------------------------------------
		//  version2
			last_push_time_ = *(size_t*)p;
			p+=sizeof(size_t);
			size_t list_size = *(size_t*)p;
			p+=sizeof(size_t);
			traffic_list_.clear();
			for (i=0;i<list_size;i++)
			{
				size_t unit_traffic = *(size_t*)p;
				p+=sizeof(size_t);
				traffic_list_.push_back(unit_traffic);
			}
			return true;
		}
	};
}
