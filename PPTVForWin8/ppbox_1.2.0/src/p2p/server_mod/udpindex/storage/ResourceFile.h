#pragma once
#include "base\blockmap.h"
#include "pieceinfo.h"

namespace storage
{
	class ResourceFile
		:public Resource
		,public boost::noncopyable
	{
	public:
		typedef boost::shared_ptr<FileResource> p ;

		boost::asio::io_service ios_; 

		//
		ResourceFile(string filename,int file_size,int resource_type){file_path_name_ = filename;file_size_ = file_size;resource_type = resource_type_;};

		virtual void FlushBlcok(int Blocksn) = NULL;

		virtual void FlushFile() = NULL;

		virtual void Close() = NULL;

		virtual bool Write(size_t offset,Buffer buf) = NULL;

		virtual Buffer  Read(size_t offset,u_int len) = NULL;

		string GetFileName(){ return file_path_name_; };

		size_t GetFileLen() {return file_size_;};

		int GetResourceType(){ return resource_type_;};

	private:
		static DWORD WINAPI Run(LPVOID	pParam);


	protected:
		size_t  file_size_;
		int    resource_type_;  //RID_GENERATING,RID_SET,RID_REPORTING  
		string  file_path_name_;
		RID     rid_;
		ResourceDescriptor::p file_resource_bitmap_p;
	private:
		boost::asio::io_service::work* work_;
		HANDLE t_;


	};

}