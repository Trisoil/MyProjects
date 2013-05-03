
/*
ChunkBlockInfo.h
ʵ���ļ���Դ�ķֲ����� 
*/
#pragma once
#include "storage_base.h"
#include "ChunkPieceInfo.h"
namespace storage
{
	class	ChunkBlockInfo 
		:boost::noncopyable
	{
	public:
		typedef boost::shared_ptr<ChunkBlockInfo> p;
	private:
		ChunkBlockInfo(u_int piece_num=piece_num_per_block_g_,bool b_full_block=false);
	public:
		static ChunkBlockInfo::p Create(u_int subpiece_num);
		static bool Parse(Buffer blockinfo_buf,ChunkBlockInfo::p &chunk_block_info);
		static Buffer ChunkBlockInfo::NullBlockToBuffer();
		Buffer ToBuffer();
		//����
		bool HasSubPiece(const u_int subpiece_index);

		bool HasPiece(const PieceInfo& piece_info);

		size_t GetCurrNullSubPieceCount() 
		{ 
			return max_subpiece_index_+1- curr_subpiece_count_;
		};

		size_t GetCurrSubPieceCount()const { return curr_subpiece_count_;};

		size_t GetSize()const  { return max_subpiece_index_+1;};

		bool IsFullBlock(){ return piece_info_set_.size()==0;};


		//����
		bool AddSubPieceInfo(u_int subpiece_index);
		bool GetNextNullSubPiece(const u_int sub_subpiece_index, u_int& subpiece_for_download);

	protected:
		std::map<u_short,ChunkPieceInfo::p> piece_info_set_;
	private:
		static bool CheckBuf(Buffer inbuf,int &max_num_subpiece,int &pieces_buffer_size);

	private:
		//block���߽�
		u_int  max_subpiece_index_;
		u_int  curr_subpiece_count_;
	};
};
