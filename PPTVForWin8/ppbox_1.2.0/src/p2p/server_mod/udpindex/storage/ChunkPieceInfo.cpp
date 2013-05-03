/*
文件说明：ChunkPieceInfo.cpp

日期：
作者：
*/
#include "stdafx.h"
#include <boost\dynamic_bitset.hpp>
#include "ChunkPieceInfo.h"

namespace storage
{
	//class ChunkPieceInfo impl
	ChunkPieceInfo::ChunkPieceInfo(u_short subpiece_num) :subpiece_set_(subpiece_num)
	{
		subpiece_set_.set();
	}

	ChunkPieceInfo::ChunkPieceInfo( BYTE* bitbuf,int bitcount,int bitbufferlen )  //1-128(subpiece_num_per_piece_g_)
	{
		int buffer_block_count = bitbufferlen/sizeof(u_int);
		for(int i = 0;i<buffer_block_count;i++)
		{
			subpiece_set_.append(*(u_int*)(bitbuf+i*sizeof(u_int)));
		}
		subpiece_set_.resize(bitcount,true);
	}


	ChunkPieceInfo::p ChunkPieceInfo::Create(u_int subpiece_num)
	{
		return p( new ChunkPieceInfo( subpiece_num));
	}

	bool ChunkPieceInfo::Parse(Buffer piece_buf,ChunkPieceInfo::p &chunk_piece_info)
	{
		int bitcount = 0;
		int bitbuffersize = 0;
		if(!CheckBuf(piece_buf,bitcount,bitbuffersize))
			return false;

		if(bitbuffersize==0)
		{
			chunk_piece_info = ChunkPieceInfo::p();
			return true;
		}

		chunk_piece_info = ChunkPieceInfo::p(new ChunkPieceInfo(piece_buf.data_.get()+4,bitcount,bitbuffersize));
		return true;
	}

	bool ChunkPieceInfo::CheckBuf(Buffer inbuf,int &bitcount,int &bitbuffersize)
	{
		if(inbuf.length_<4)
			return false;
		bitcount = *(u_int*)inbuf.data_.get();
		if(bitcount==0)
		{
			assert(inbuf.length_==4);
			bitbuffersize = 0;
			return true;
		}

		if(bitcount>subpiece_num_per_piece_g_)
			return false;

		bitbuffersize = inbuf.length_-4;

		if(bitbuffersize%sizeof(u_int)!=0)
			return false;

		//if(bitbuffersize>(bitcount-1)/8+1)
		//	return false;

		return true;
	}

	Buffer ChunkPieceInfo::ToBuffer()
	{
		int bit_block_len = subpiece_set_.m_bits.size();
		int buf_len = bit_block_len*4 + 4;

		Buffer outbuf(buf_len);
		int *bitblock = (int *)outbuf.data_.get();

		*bitblock = subpiece_set_.size();

		bitblock++;

		for(int i = 0;i<bit_block_len;i++)
		{
			*bitblock = subpiece_set_.m_bits[i];
			bitblock++;
		}
		return outbuf;
	}

	Buffer ChunkPieceInfo::NullPieceToBuffer()
	{
		Buffer outbuf(4);
		*(int *)outbuf.data_.get() = 0;
		return outbuf;
	}

	bool ChunkPieceInfo::AddSubPieceInfo(u_int index)
	{
		assert(index<subpiece_set_.size());

		if(!subpiece_set_.test(index))
		{
			STORAGE_EVENT_LOG("AddSubPieceInfo Again! subpiece_index:"<<index);
			return false;
		}
		subpiece_set_.reset(index);
		return true;
	}

	bool ChunkPieceInfo::DeleteSubPieceInfo(u_int index)
	{
		assert(index<subpiece_set_.size());

		if(subpiece_set_.test(index))
		{
			return false;
		}
		subpiece_set_.set(index);
		return true;
	}

	bool ChunkPieceInfo::HasSubPiece(u_int index) //0-127(subpiece_num_per_piece_g_-1)
	{
		if(index>=subpiece_set_.size())
		{
			return false;
		}
		return !subpiece_set_.test(index);
	}


	bool ChunkPieceInfo::GetNextNullSubPiece(size_t start,size_t &next_index)
	{
		size_t pos = start;
		assert(start<subpiece_set_.size());
		if(subpiece_set_.test(start))
		{
			next_index = start;
			return true;
		}

		next_index = subpiece_set_.find_next(pos);
		if(next_index==boost::dynamic_bitset<u_int>::npos)
		{
			return false;
		}
		assert(next_index<subpiece_set_.size());
		return true;
	};


	size_t ChunkPieceInfo::GetSize()
	{
		return subpiece_set_.size();//subpiece_set_.GetSize(); 
	}

	bool ChunkPieceInfo::IsFullPieceInfo()
	{
		return subpiece_set_.none();
	}

	size_t ChunkPieceInfo::GetCurrNullSubPieceCount()
	{
		//0表示有，1表示没有
		return subpiece_set_.count();
	}; 
	size_t ChunkPieceInfo::GetCurrSubPieceCount()
	{ 
		//0表示有，1表示没有
		return subpiece_set_.size()-subpiece_set_.count();
	};

};
