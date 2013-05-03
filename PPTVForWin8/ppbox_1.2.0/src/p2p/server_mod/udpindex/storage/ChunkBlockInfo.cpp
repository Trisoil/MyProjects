#include "stdafx.h"
#include <map>
#include "ChunkBlockInfo.h"


namespace storage
{
	ChunkBlockInfo::ChunkBlockInfo(u_int subpiece_num,bool b_null_block) 
	{
		assert(subpiece_num>0);
		max_subpiece_index_ = subpiece_num-1;
		curr_subpiece_count_ = 0;

		if(!b_null_block)
			return;

		u_int piece_index = max_subpiece_index_/subpiece_num_per_piece_g_;

		for(u_int i = 0; i<=piece_index;i++)
		{
			piece_info_set_.insert(make_pair(i,ChunkPieceInfo::p()));
		}
	} ;

	ChunkBlockInfo::p ChunkBlockInfo::Create(u_int subpiece_num)
	{
		p pointer;
		pointer = p( new ChunkBlockInfo(subpiece_num,true));
		assert(pointer);
		return pointer;
	}

	bool ChunkBlockInfo::Parse(Buffer blockinfo_buf,ChunkBlockInfo::p &chunk_block_info)
	{
		int max_num_subpiece = 0;
		int pieces_buffer_size = 0;
		if(!CheckBuf(blockinfo_buf,max_num_subpiece,pieces_buffer_size))
		{
			return false;
		}

		if(pieces_buffer_size==0)
		{
			chunk_block_info = ChunkBlockInfo::p();
			return true;
		}

		byte* buf = blockinfo_buf.data_.get()+4;
		int buflen = pieces_buffer_size;

		u_int piecebuflen;
		u_short piece_index;

		ChunkBlockInfo::p pointer = ChunkBlockInfo::p(new ChunkBlockInfo(max_num_subpiece,false));
		assert(pointer->piece_info_set_.size()==0);

		while(buflen>0)
		{
			piece_index = *(u_short*)buf;
			buf += 2 ; buflen -= 2;
			piecebuflen = *(u_int*)buf;
			buf += 4; buflen -= 4;

			if(buflen < piecebuflen )
			{
				return false;
			}

			if(piecebuflen==0)
			{
				pointer->piece_info_set_.insert(make_pair(piece_index,ChunkPieceInfo::p()));
				continue;
			}

			Buffer piecebuf(buf,piecebuflen);
			buf += piecebuflen; buflen -= piecebuflen;
			ChunkPieceInfo::p piece_pointer;

			if(!ChunkPieceInfo::Parse(piecebuf,piece_pointer))
			{
				return false;
			}
			pointer->piece_info_set_.insert(make_pair(piece_index,piece_pointer));
		}
		size_t curr_null_subpiece_count = 0;

		for(std::map<u_short,ChunkPieceInfo::p>::const_iterator pieceit = pointer->piece_info_set_.begin();pieceit!=pointer->piece_info_set_.end();pieceit++)
		{
			ChunkPieceInfo::p chunk_piece_info = pieceit->second;
			if(chunk_piece_info)
			{
				curr_null_subpiece_count += chunk_piece_info->GetCurrNullSubPieceCount();
			}
			else
			{
				if(pieceit->first==pointer->max_subpiece_index_/subpiece_num_per_piece_g_)
				{
					curr_null_subpiece_count += pointer->max_subpiece_index_%subpiece_num_per_piece_g_+1;
				}
				else
				{
					curr_null_subpiece_count += subpiece_num_per_piece_g_;
				}
			}
		}
		pointer->curr_subpiece_count_ = pointer->max_subpiece_index_+1-curr_null_subpiece_count;
		chunk_block_info = pointer;
		return true;
	}

	Buffer ChunkBlockInfo::NullBlockToBuffer()
	{
		Buffer null_block_buf(4);
		*(int*)null_block_buf.data_.get() = 0;
		return null_block_buf;
	}

	Buffer ChunkBlockInfo::ToBuffer()
	{
		int tmp_buffer_size = 2*1024*32;
		int buf_len = 0;

		byte *tmp = new byte[tmp_buffer_size]; 
		*(u_int*)(tmp+buf_len) = max_subpiece_index_+1;
		buf_len += 4;

		
		for(std::map<u_short,ChunkPieceInfo::p>::const_iterator pieceit = piece_info_set_.begin();pieceit!=piece_info_set_.end();pieceit++)
		{
			//piece index
			size_t piece_index = pieceit->first;
			*(u_short*)(tmp+buf_len) = piece_index;
			buf_len += 2;
			//piece len
			ChunkPieceInfo::p chunk_piece_info = pieceit->second;
			Buffer piecebuf;
			if(!chunk_piece_info)
			{
				piecebuf = ChunkPieceInfo::NullPieceToBuffer();
			}
			else
			{
				piecebuf = pieceit->second->ToBuffer();
			}

			if(buf_len+piecebuf.length_+1024>tmp_buffer_size)
			{
				tmp_buffer_size = tmp_buffer_size*2;
				byte *tmp2 = tmp;
				tmp = new byte[tmp_buffer_size];
				memcpy(tmp,tmp2,buf_len);
				delete[] tmp2;
			}
			//piecebuffer 长度、内容
			*(int*)(tmp+buf_len) = piecebuf.length_;
			buf_len += 4;
			memcpy((tmp+buf_len),piecebuf.data_.get(),piecebuf.length_);
			buf_len += piecebuf.length_;
		}
		Buffer outbuf(tmp,buf_len);

		delete[] tmp;
		return outbuf;

	}


	bool ChunkBlockInfo::AddSubPieceInfo(u_int subpiece_index)
	{
		assert(subpiece_index<=max_subpiece_index_);

		u_short piece_index = subpiece_index/subpiece_num_per_piece_g_;

		std::map<u_short,ChunkPieceInfo::p>::const_iterator it = piece_info_set_.find(piece_index);

		if(it==piece_info_set_.end())
		{
			STORAGE_ERR_LOG("ChunkBlockInfo::AddSubPieceInfo Subpiece is existed!"<<subpiece_index);
			return false;
		}
		ChunkPieceInfo::p piece_info_p;
		piece_info_p = it->second; 

		if(!piece_info_p)
		{
			u_int subpiece_size = subpiece_num_per_piece_g_;
			//是否最后一个Piece
			if(subpiece_index/subpiece_num_per_piece_g_ == max_subpiece_index_/subpiece_num_per_piece_g_)
			{
				subpiece_size = max_subpiece_index_%subpiece_num_per_piece_g_+1;
			}
			piece_info_set_.erase(piece_index);
			piece_info_p = ChunkPieceInfo::Create(subpiece_size);

			piece_info_set_.insert(make_pair(piece_index,piece_info_p));
		}

		if(piece_info_p->AddSubPieceInfo(subpiece_index%subpiece_num_per_piece_g_))
		{
			if(piece_info_p->IsFullPieceInfo())
			{
				piece_info_set_.erase(piece_index);
			}
			curr_subpiece_count_++;
			return true;
		}
		return false;
	}

	bool ChunkBlockInfo::HasSubPiece(const u_int subpiece_index)
	{
		assert(subpiece_index<=max_subpiece_index_);

		u_int piece_index = subpiece_index/subpiece_num_per_piece_g_;

		std::map<u_short,ChunkPieceInfo::p>::const_iterator it = piece_info_set_.find(piece_index);
		if(it==piece_info_set_.end())
		{
			return true;
		}
		if(!it->second)
		{
			return false;
		}
		return it->second->HasSubPiece(subpiece_index%subpiece_num_per_piece_g_);
	}

	bool ChunkBlockInfo::HasPiece(const PieceInfo& piece_info)
	{
		std::map<u_short,ChunkPieceInfo::p>::const_iterator it = piece_info_set_.find(piece_info.piece_index_);
		if(it==piece_info_set_.end())
		{
			return true;
		}
		return false;
	}


	bool ChunkBlockInfo::GetNextNullSubPiece(const u_int start_sub_subpiece_index, u_int& subpiece_for_download)
	{
		u_int start_piece_index = start_sub_subpiece_index/subpiece_num_per_piece_g_;
		u_int piece_index = start_piece_index;

		std::map<u_short,ChunkPieceInfo::p>::const_iterator it = piece_info_set_.lower_bound(piece_index);

		for(;it!=piece_info_set_.end();it = piece_info_set_.lower_bound(piece_index))
		{
			if(!it->second) 
			{
				//chunk_piece_info为空
				if(it->first==start_piece_index) 
				{
					//第一个piece
					subpiece_for_download = start_sub_subpiece_index;
				}
				else
				{
					subpiece_for_download = it->first*subpiece_num_per_piece_g_;
				}
				return true;
			}

			//chunk_piece_info不为空

			u_int startindex;
			//确定起始index
			if(start_piece_index==it->first)
			{
				startindex = start_sub_subpiece_index%subpiece_num_per_piece_g_;
			}
			else
			{
				piece_index = it->first;
				startindex  = 0;
			}
			//piece内搜索
			if(!it->second->GetNextNullSubPiece(startindex,subpiece_for_download))
			{
				piece_index++;
				continue;
			}
			//piece内偏移，必须重新计算
			subpiece_for_download = piece_index*subpiece_num_per_piece_g_+subpiece_for_download;
			return true;
		}
		return false;
	}

	bool  ChunkBlockInfo::CheckBuf(Buffer blockinfo_buf,int &max_num_subpiece,int &pieces_buffer_size)
	{
		if(blockinfo_buf.length_<4)
		{
			return false;
		}
		max_num_subpiece = *(int*)blockinfo_buf.data_.get();

		//if(max_num_subpiece==0)
		//{
		//	return false;
		//}

		pieces_buffer_size = blockinfo_buf.length_-4;
		return true;
	}


	//bool ChunkBlockInfo::Check(u_int subpiece_index)
	//{
	//	if(subpiece_index<=max_subpiece_index_)
	//		return true;
	//	return false;
	//}


}
