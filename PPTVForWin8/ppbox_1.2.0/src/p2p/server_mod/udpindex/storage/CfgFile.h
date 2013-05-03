/*
CfgFile.h
*/


#pragma once

#include "framework\mswin\module.h"
#define UTIL_INFO(message)
#define UTIL_EVENT(message)
#define UTIL_WARN(message)

#define UTIL_DEBUG(message)

#include "framework\io\stdfile.h"
#include "storage_base.h"
#include "md5.h"

using framework::io::StdFile;
namespace storage
{
// ----------------------------------------------------------------------------
// cfg和dat文件的文件头，严格按字节对齐
#pragma pack(push,1)
	struct SecFileHead
	{
		int verison_;
		u_int64 time_stamp_;
		MD5 sec_md5_;
		SecFileHead():verison_(0),sec_md5_(),time_stamp_(0) {};
		inline static u_int md5_offset() {return sizeof(int)+sizeof(u_int64);};
	};
#pragma pack(pop)
// ----------------------------------------------------------------------------

	class SecFile
		:private StdFile
		,private CHmacMD5
	{
	public:
		SecFile():crash_b_(true),sec_content_count_(0),only_read(false){};
		SecFile(u_char *key,size_t key_len):CHmacMD5(key,key_len),crash_b_(true),sec_content_count_(0),only_read(false){};
		// 操作
		virtual bool SecOpen(const TCHAR* path);
		virtual bool SecCreate(const TCHAR* path);
		virtual bool SecWrite(const u_char *buf, size_t buflen);
		virtual size_t  SecRead(u_char *buf, size_t buflen);
		virtual void SecClose();

		//属性

	protected:
		virtual bool SetCrashFlag() { return crash_b_;};
		virtual size_t GetContentSize();
		virtual bool IsCrash() { return crash_b_;};
		virtual u_int HeadVersion() { return head_.verison_; }
		static u_int Version2() { return version2; }

	private:
		bool DoSign() ;
		bool DoVerify() ;

	private:
//		static const int sec_head_len_ = sizeof(SecFileHead); //(bytes)
		static const u_int version_ = 0x00000001;
		static const u_int version2 = 0x00000002;
		static const size_t max_sec_file_size = 32*1024*1024; //32M
		bool crash_b_;
		size_t sec_content_count_;
		SecFileHead head_;
		bool only_read;
	};

	class CfgFile
		:protected SecFile
	{
	public:
		CfgFile():SecFile((u_char*)default_cfg_key_g_.c_str(),default_cfg_key_g_.size()),resource_file_size_(0),cfg_head_len_(0){};
		CfgFile(u_char *key,u_short key_len):SecFile(key,key_len),resource_file_size_(0),cfg_head_len_(0){};
		// 属性
		virtual bool SecOpen(const tstring resource_file_path);
		virtual bool SecCreate(tstring respurce_file_name, size_t respurce_file_size);
		virtual void SecClose();
		bool AddContent(Buffer buf);
		Buffer GetContent();
	public:
		size_t resource_file_size_;
		tstring resource_file_name_;
	private:
		int cfg_head_len_;
	};

	//文件操作
	class ResourceInfoListFile
		:public SecFile
	{
	public:
		ResourceInfoListFile():SecFile(){};
		ResourceInfoListFile(u_char *key,u_short key_len):SecFile(key,key_len){};
		bool AddResourceInfo(const FileResourceInfo &r_info); 
		bool GetResourceInfo(FileResourceInfo &r_info);
	private:
	};
}