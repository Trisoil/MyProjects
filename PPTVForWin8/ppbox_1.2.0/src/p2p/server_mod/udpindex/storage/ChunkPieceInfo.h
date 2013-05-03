/*
ChunkPieceInfo.h
实现文件资源的分层描述 ResourceDescriptor.h
*/
#pragma once
#include <boost\dynamic_bitset.hpp>
#include "storage_base.h"

namespace storage
{
	class  ChunkPieceInfo
		:public boost::noncopyable
	{
	public:
		typedef boost::shared_ptr<ChunkPieceInfo> p;
		static ChunkPieceInfo::p null_piece_;//= p( new ChunkPieceInfo());//just one!

	private:
		ChunkPieceInfo(u_short subpiece_num = subpiece_num_per_piece_g_);  //1-128(subpiece_num_per_piece_g_)
		ChunkPieceInfo::ChunkPieceInfo( BYTE* bitbuf,int bitcount,int bitbuffersize ); 
	public:
		//static ChunkPieceInfo::p GetNullInfoObj();
		static ChunkPieceInfo::p Create(u_int subpiece_num);
		static bool Parse(Buffer piece_buf,ChunkPieceInfo::p &chunk_piece_info);
		static Buffer ChunkPieceInfo::NullPieceToBuffer();


		//操作
		Buffer ToBuffer();
		bool AddSubPieceInfo(u_int index);
		bool DeleteSubPieceInfo(u_int index);

		//属性
		bool HasSubPiece(u_int index); //0-127(subpiece_num_per_piece_g_-1);
		bool GetNextNullSubPiece(size_t start,size_t &next_index);

		size_t GetCurrNullSubPieceCount();
		size_t GetCurrSubPieceCount();
		size_t GetSize();
		bool IsFullPieceInfo();

	private:
		static bool CheckBuf(Buffer inbuf,int &bitcount,int &bitbuffersize);
		boost::dynamic_bitset<u_int> subpiece_set_;
	};
}

