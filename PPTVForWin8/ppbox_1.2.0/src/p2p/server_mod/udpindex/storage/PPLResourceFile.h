#pragma once
#include "blockmap.h"
#include "pieceinfo.h"
#include "ResourceFile.h"

namespace p2sp
{
	class PPLResourceFile
		:public ResourceFile
	{
	public:
		//typedef boost::shared_ptr<PPLResourceFile> p;
	public:
		//static p Create(string filename,size_t file_size);

		//static p Open(string filename,size_t file_size);

		PPLResourceFile(string filename,int file_size,int resource_type);

		static bool Check(string filename,size_t file_size);

		void FlushBlcok(int Blocksn);

		void Flush();

		bool Write(size_t offset,Buffer buf);

		Buffer Read(size_t offset,u_int len);

		string GetFileName(){ return file_path_name_; };

		size_t GetFileLen() {return file_size_;};

	private:
		void GenResourceID(HANDLE *hfile,size_t filelen){};

		inline bool Check(size_t offset,Buffer write_buf);

		////异步线程内文件访问
		//void OnThreadGetSubPiece(SubPieceInfo subpiece_info, udp::endpoint end_point, IUploadListener::p listener);
		//void OnThreadAddSubPiece(SubPieceInfo subpiece_info, Buffer buffer);

	private:
		int    status_flag_;  //RID_GENERATING,RID_SET,RID_REPORTING  

		string  file_path_name_;

		string  downloadinfo_file_path_;

		size_t  file_size_;

		HANDLE  h_thread_;
		HANDLE	hfile_;
		HANDLE  hmapfile_;
		LPVOID  view_map_buffer_;
	};

}