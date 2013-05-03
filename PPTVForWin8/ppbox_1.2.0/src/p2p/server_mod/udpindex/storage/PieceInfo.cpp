/*
文件说明：

日期：
作者：
*/
#include "stdafx.h"
#include "PieceInfo.h"

namespace storage
{
	//class ChunkPieceInfo impl
	ChunkPieceInfo::p ChunkPieceInfo::null_piece_ = ChunkPieceInfo::p(new ChunkPieceInfo(0));

	ChunkPieceInfo::ChunkPieceInfo(u_short subpiece_num)  //1-128(subpiece_num_per_piece_g_)
		:subpiece_set_(subpiece_num)
	{
		subpiece_set_.set();
	}

	ChunkPieceInfo::ChunkPieceInfo( char *bitbuf,int bitcount,int bitbuffersize )  //1-128(subpiece_num_per_piece_g_)
	{
		for(int i = 0;i<bitbuffersize/sizeof(u_int);i++)
		{
			subpiece_set_.append(*(u_int*)(bitbuf+i*sizeof(u_int)));
		}
		subpiece_set_.resize(bitcount,true);
	}

	ChunkPieceInfo::p ChunkPieceInfo::GetNullPieceInfoInfoObj()
	{
		assert(!null_piece_);
		return null_piece_;
	}

	ChunkPieceInfo::p ChunkPieceInfo::CreateChunkPieceInfo(u_int subpiece_num)
	{
		return p( new ChunkPieceInfo( subpiece_num));
	}

	ChunkPieceInfo::p ChunkPieceInfo::Parse(Buffer inbuf)
	{
		ChunkPieceInfo::p pointer ;
		int bitcount = 0;
		int bitbuffersize = 0;
		if(!CheckBuf(inbuf,bitcount,bitbuffersize))
			return pointer;
		pointer	= ChunkPieceInfo::p(new ChunkPieceInfo(inbuf.data_.get()+4,bitcount,bitbuffersize));
		return pointer;
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

		assert(bitcount<=subpiece_num_per_piece_g_);

		if(inbuf.length_%sizeof(u_int)!=0)
			return false;
		bitbuffersize = inbuf.length_-4;
		if(bitbuffersize>(bitcount-1)/8+1)
			return false;
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

	bool ChunkPieceInfo::IsFull()
	{
		return subpiece_set_.none();
	}

};

namespace storage
{
	ChunkBlockInfo::p ChunkBlockInfo::null_block_ = ChunkBlockInfo::p(new ChunkBlockInfo(0,true));
	ChunkBlockInfo::p ChunkBlockInfo::GetNullBlockInfoObj()
	{
		assert(null_block_!= NULL);
		//	null_block_ = p( new ChunkBlockInfo());
		return null_block_;
	}

	ChunkBlockInfo::p ChunkBlockInfo::Parse(Buffer blockinfo_buf)
	{
		ChunkBlockInfo::p pointer;

		byte* buf = blockinfo_buf.data_.get();
		int buflen = blockinfo_buf.length_;
		int &max_num_subpiece = *(int*)buf;

		buf += 4;buflen -= 4;

		if(max_num_subpiece==0)
		{
			if(buflen==0)
			{
				return ChunkBlockInfo::GetNullBlockInfoObj();
			}
			else
			{
				return pointer;
			}
		}
		int piecebuflen;
		u_short piece_index;

		pointer = ChunkBlockInfo::p(new ChunkBlockInfo(max_num_subpiece,true));

		while(buflen>=8)
		{
			piece_index = *(u_short*)buf;
			buf += 2 ; buflen -= 2;
			piecebuflen = *(int*)buf;
			buf += 4; buflen -= 4;
			if(buflen - piecebuflen <0)
			{
				assert(false);
				return ChunkBlockInfo::GetNullBlockInfoObj();
			}

			Buffer piecebuf(buf,piecebuflen);
			buf += piecebuflen; buflen -= piecebuflen;

			ChunkPieceInfo::p piece_pointer = ChunkPieceInfo::Parse(piecebuf);
			if(piece_pointer)
			{
				pointer->piece_info_set_[piece_index] = piece_pointer;
			}
		}
		return pointer;
	}

	size_t ChunkBlockInfo::GetCurrNullSubPieceCount(bool b_last)
	{
		size_t curr_null_subpiece_count = 0;
		for(std::map<u_short,ChunkPieceInfo::p>::iterator it = piece_info_set_.begin();
			it!=piece_info_set_.end();it++)
		{
			if(it->second!=ChunkPieceInfo::GetNullInfoObj())
			{
				curr_null_subpiece_count += it->second->GetCurrNullSubPieceCount();
			}
			else
			{
				if(max_subpiece_index_/subpiece_num_per_piece_g_==it->first))
				{
					curr_null_subpiece_count += max_subpiece_index_%subpiece_num_per_piece_g_+1;
				}
				else
				{
					curr_null_subpiece_count += subpiece_num_per_piece_g_ ;
				}
			}
		}
		return curr_null_subpiece_count;
	}

	Buffer ChunkBlockInfo::ToBuffer()
	{
		//if(this==ChunkBlockInfo::GetNullBlockInfoObj())
		//{
		//	Buffer outbuf(4);
		//	*(int*)(outbuf.data_.get()) = 0;  //max_num_subpiece
		//	return outbuf;
		//}
		//else
		{
			Buffer tmp(2*1024*8); 
			byte *buf = tmp.data_.get();
			u_int &max_num_subpiece = *(u_int*)buf;
			buf += 4;
			max_num_subpiece = 	max_subpiece_index_+1;
			if(max_num_subpiece!=0)
			{
				std::map<u_short,ChunkPieceInfo::p>::const_iterator pieceit = piece_info_set_.begin();
				while(pieceit!=piece_info_set_.end())
				{
					//piece index
					*(u_short*)buf = pieceit->first;
					buf += 2;
					//piecebuffer 长度、内容
					Buffer piecebuf = pieceit->second->ToBuffer();
					*(int*)buf = piecebuf.length_;
					buf += 4;
					memcpy(buf,piecebuf.data_.get(),piecebuf.length_);
					buf += piecebuf.length_;
					assert(buf<tmp.data_.get()+2*1024*8);
					pieceit ++;
				}
			}
			int len =buf - tmp.data_.get();
			Buffer outbuf(tmp.data_.get(),len);
			return outbuf;
		}

	}

	ChunkBlockInfo::p ChunkBlockInfo::CreateChunkBlockInfoObj(u_int subpiece_num,bool b_init_null)
	{
		p pointer;
		pointer = p( new ChunkBlockInfo(subpiece_num,b_init_null));
		assert(pointer);
		return pointer;
	}


	bool ChunkBlockInfo::AddSubPieceInfo(u_int subpiece_index)
	{
		assert(subpiece_index<=max_subpiece_index_);

		u_short piece_index = subpiece_index/subpiece_num_per_piece_g_;

		std::map<u_short,ChunkPieceInfo::p>::const_iterator it = piece_info_set_.find(piece_index);

		if(it==piece_info_set_.end())
		{
			STORAGE_LOG("ChunkBlockInfo::AddSubPieceInfo Subpiece is existed!"<<subpiece_index);
			return false;
		}

		if(it->second==ChunkPieceInfo::GetNullInfoObj())
		{
			u_int subpiece_size = subpiece_num_per_piece_g_;
			//是否最后一个Piece
			if(subpiece_index/subpiece_num_per_piece_g_ == max_subpiece_index_/subpiece_num_per_piece_g_)
			{
				subpiece_size = max_subpiece_index_%subpiece_num_per_piece_g_+1;
			}
			piece_info_set_[piece_index] = ChunkPieceInfo::CreateChunkPieceInfo(subpiece_size);
		}

		if(piece_info_set_[piece_index]->AddSubPieceInfo(subpiece_index%subpiece_num_per_piece_g_))
		{
			if(piece_info_set_[piece_index]->IsFull())
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
		u_int piece_index = subpiece_index/subpiece_num_per_piece_g_;
		std::map<u_short,ChunkPieceInfo::p>::const_iterator it = piece_info_set_.find(piece_index);
		if(it==piece_info_set_.end())
		{
			return true;
		}
		if(it->second==ChunkPieceInfo::GetNullInfoObj())
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
		u_int piece_index = start_sub_subpiece_index/subpiece_num_per_piece_g_;

		std::map<u_short,ChunkPieceInfo::p>::const_iterator it = piece_info_set_.lower_bound(piece_index);
		for(;it!=piece_info_set_.end();it = piece_info_set_.lower_bound(piece_index))
		{
			if(it->second==ChunkPieceInfo::GetNullInfoObj())
			{
				if(it->first==start_sub_subpiece_index/subpiece_num_per_piece_g_)
				{
					subpiece_for_download = start_sub_subpiece_index;
				}
				else
				{
					subpiece_for_download = it->first*subpiece_num_per_piece_g_;
				}
				return true;
			}
			u_int startindex;

			if(start_sub_subpiece_index/subpiece_num_per_piece_g_==it->first)
			{
				startindex = start_sub_subpiece_index%subpiece_num_per_piece_g_;
			}
			else
			{
				piece_index = it->first;
				startindex  = 0;
			}

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


	ChunkBlockInfo::ChunkBlockInfo(u_int subpiece_num,bool b_null_block) 
	{
		assert(subpiece_num>0);
		max_subpiece_index_ = subpiece_num-1;
		curr_subpiece_count_ = 0;

		if(!b_null_block)
			return;


		ChunkPieceInfo::p pointer = ChunkPieceInfo::GetNullInfoObj();

		u_int piece_index = max_subpiece_index_/subpiece_num_per_piece_g_;

		for(u_int i = 0; i<=piece_index;i++)
		{
			piece_info_set_[i] = pointer;
		}
	} ;
	bool ChunkBlockInfo::Check(u_int subpiece_index)
	{
		if(subpiece_index<=max_subpiece_index_)
			return true;
		return false;
	}


}


namespace storage
{
	//class ResourceDescriptor impl
	ResourceDescriptor::ResourceDescriptor(const RidInfo &rid_info,bool b_full_file)
	{
		assert(rid_info.file_length_>0);
		rid_info_ = rid_info;

		for(int i= rid_info_.block_md5_s_.size();i<rid_info_.block_count_;i++)
		{
			rid_info_.block_md5_s_.push_back( MD5());
		}
		max_subpiece_info_.subpiece_index_ =( (rid_info_.file_length_-1)%rid_info_.block_size_)/bytes_num_per_subpiece_g_;
		max_subpiece_info_.block_index_ = rid_info_.block_count_-1;
		block_bit_map_ = BlockMap::Create(rid_info_.block_count_);

		if((b_full_file))
		{
			//文件已全部下载完毕，MAP为空
			download_bytes_ = rid_info_.file_length_;
			return;
		}
		//将BlockMap用空BLOCK对象填充
		for(u_int block_index = 0;block_index<=max_subpiece_info_.block_index_;block_index++)
		{
			block_info_set_[block_index]=ChunkBlockInfo::GetNullBlockInfoObj();
		}
		download_bytes_ = 0;
	}

	ResourceDescriptor::p ResourceDescriptor::Create(const RidInfo &rid_info,bool b_full_file)
	{
		p pointer;
		pointer = p(new ResourceDescriptor(rid_info,b_full_file));
		return pointer;
	}

	ResourceDescriptor::p ResourceDescriptor::Create(const size_t &resource_size,bool b_full_file)
	{
		p pointer;
		RidInfo rid_info;
		assert(resource_size>0);
		rid_info.InitByFileLength(resource_size);
		pointer = p(new ResourceDescriptor(rid_info,b_full_file));
		return pointer;
	}
	ResourceDescriptor::p ResourceDescriptor::Parse(Buffer cfgfile_buf,const RidInfo &rid_info)
	{
		size_t curr_null_subpiece_count = 0;
		ResourceDescriptor::p pointer;
		if(!CheckCfgBuf(cfgfile_buf))
			return pointer;

		pointer = ResourceDescriptor::p(new ResourceDescriptor(rid_info,true));
		byte * buf = cfgfile_buf.data_.get();
		int buflen = cfgfile_buf.length_;

		while(buflen>=8)
		{
			u_int block_index = *(u_int*)buf;
			buf += 4 ; buflen -= 4;
			int blockbuflen = *(int*)buf;
			buf += 4; buflen -= 4;

			if(buflen - blockbuflen <0)
			{
				STORAGE_ERR_LOG("parse cfg file err!");
				break;
			}

			Buffer blockbuf(buf,blockbuflen);
			buf += blockbuflen; buflen -= blockbuflen;

			ChunkBlockInfo::p block_pointer = ChunkBlockInfo::Parse(blockbuf);
			assert(block_pointer);
			pointer->block_info_set_[block_index] = block_pointer;
			if(block_pointer!=ChunkBlockInfo::GetNullBlockInfoObj())
			{
				curr_null_subpiece_count+=block_pointer->GetCurrNullSubPieceCount(block_index==pointer->max_subpiece_info_.block_index_);
			}
			else
			{
				if(block_index==pointer->max_subpiece_info_.block_index_)
				{
					curr_null_subpiece_count+=(pointer->max_subpiece_info_.subpiece_index_+1);
				}
				else
				{
					curr_null_subpiece_count+=pointer->rid_info_.block_size_/bytes_num_per_subpiece_g_;
				}
			}
		}
		if(!pointer->HasSubPiece(pointer->max_subpiece_info_))
		{
			pointer->download_bytes_ = pointer->rid_info_.file_length_ - ((curr_null_subpiece_count-1)*bytes_num_per_subpiece_g_+pointer->rid_info_.file_length_%bytes_num_per_subpiece_g_);
		}
		else
		{
			pointer->download_bytes_ = pointer->rid_info_.file_length_ - curr_null_subpiece_count*bytes_num_per_subpiece_g_;
		}
		return pointer;
	};

	bool  ResourceDescriptor::CheckCfgBuf(Buffer cfg_buf)
	{
		return true;
	}

	Buffer ResourceDescriptor::ToBuffer()
	{
		Buffer tmpbuf(max_size_cfg_file_g_);
		byte *pos = tmpbuf.data_.get();

		//只记录空block、空或半空的piece
		std::map<int,ChunkBlockInfo::p>::const_iterator it = block_info_set_.begin();
		while(it!=block_info_set_.end())
		{
			if(max_size_cfg_file_g_+tmpbuf.data_.get()-pos < 8)
			{
				Buffer p;
				return p;
			}
			//index
			*(int*)pos = it->first;
			pos +=4;
			//blockbuf
			Buffer block_buf = it->second->ToBuffer();
			*(u_int*)pos = block_buf.length_;
			pos +=4;

			if(block_buf.length_!=0)
			{
				if(tmpbuf.data_.get() + tmpbuf.length_ < pos + block_buf.length_ )
				{
					Buffer p;
					return p;
				}
				memcpy(pos,block_buf.data_.get(),block_buf.length_);
				pos += block_buf.length_;
			}
			it++;
		}
		int len = pos - tmpbuf.data_.get() ;
		Buffer outbuf(tmpbuf.data_.get(),len);
		return outbuf;
	}




	bool ResourceDescriptor::AddSubPieceInfo(SubPieceInfo in, bool &b_block_full)
	{
		//测试检查
		if(!Check(in))
		{
			STORAGE_ERR_LOG("subpiece out of range! index:"<<in.block_index_<<":"<<in.subpiece_index_);
			return false;
		}
		std::map<int,ChunkBlockInfo::p>::const_iterator it = block_info_set_.find(in.block_index_);
		if(it==block_info_set_.end())
		{
			STORAGE_DEBUG_LOG("SubPiece is existed!");
			return false;
		}
		if(it->second==ChunkBlockInfo::GetNullBlockInfoObj())
		{
			//计算最后一块大小
			int block_subpiece_num = rid_info_.block_size_/bytes_num_per_subpiece_g_;
			assert(rid_info_.block_size_%bytes_num_per_subpiece_g_==0);
			if(in.block_index_==max_subpiece_info_.block_index_)
			{
				block_subpiece_num = max_subpiece_info_.subpiece_index_+1;
			}
			block_info_set_[in.block_index_] = ChunkBlockInfo::CreateChunkBlockInfoObj(block_subpiece_num);
		}

		if(block_info_set_[in.block_index_]->AddSubPieceInfo(in.subpiece_index_))
		{
			download_bytes_ += (in==max_subpiece_info_?(rid_info_.file_length_-1)%bytes_num_per_subpiece_g_+1:bytes_num_per_subpiece_g_);
			if(block_info_set_[in.block_index_]->IsFull())
			{
				block_info_set_.erase(in.block_index_);
				b_block_full = true;
			}
			else
			{
				b_block_full = false;
			}
			return true;
		}
		return false;
	}

	//查询是否已有指定的subpiece(1K大小，由bytes_num_per_subpiece_g_确定)
	bool ResourceDescriptor::HasSubPiece(SubPieceInfo in)
	{
		////if(pending_subpiece_manager_->Find(in))
		//	return true;

		if(!Check(in))
		{
			STORAGE_ERR_LOG("subpiece out of range! index:"<<in.block_index_<<":"<<in.subpiece_index_);
			return false;
		}

		//定位block_index;
		std::map<int,ChunkBlockInfo::p>::const_iterator it = block_info_set_.find(in.block_index_);
		if(it==block_info_set_.end())
		{
			//如果没有找到，说明本地已经拥有完整的block！当然也就有该subpiece
			return true;
		}
		if(it->second==ChunkBlockInfo::GetNullBlockInfoObj())
		{
			return false;
		}
		return it->second->HasSubPiece(in.subpiece_index_);
	}

	bool  ResourceDescriptor::HasPiece(const PieceInfo& piece_info)
	{
		std::map<int,ChunkBlockInfo::p>::const_iterator it = block_info_set_.find(piece_info.block_index_);
		if(it==block_info_set_.end())
		{
			//如果没有找到，说明本地已经拥有完整的block！当然也就有该piece
			return true;
		}
		if(it->second==ChunkBlockInfo::GetNullBlockInfoObj())
		{
			return false;
		}
		return it->second->HasPiece(piece_info);
	}
	bool ResourceDescriptor::IsPublishBlock(u_int blockIndex)
	{
		std::map<int,ChunkBlockInfo::p>::const_iterator it = block_info_set_.find(blockIndex);
		if(it==block_info_set_.end())
		{
			return true;
		}
		return false;
	}

	BlockMap::p ResourceDescriptor::GetBlockMap()
	{
		BlockMap::p pointer = BlockMap::Create(max_subpiece_info_.block_index_+1);
		for(int i = 0;i<pointer->GetSize();i++)
		{
			if(IsPublishBlock(i))
				pointer->Set(i);
		}
		return pointer;
	}

	void ResourceDescriptor::RemoveBlockInfo(u_int block_index) 
	{
		std::map<int,ChunkBlockInfo::p>::const_iterator it = block_info_set_.find(block_index);
		if(it!=block_info_set_.end())
		{
			if(it->second==ChunkBlockInfo::GetNullBlockInfoObj())
			{
				return;
			}
			download_bytes_ -= it->second->GetCurrNullSubPieceCount();
		}
		else
		{
			download_bytes_ -= (block_index==max_subpiece_info_.block_index_?(rid_info_.file_length_-1)%(rid_info_.block_size_)+1: rid_info_.block_size_);
		}
		block_info_set_.insert(make_pair(block_index,ChunkBlockInfo::GetNullBlockInfoObj()));
	}


	bool ResourceDescriptor::GetNextNullSubPiece(const SubPieceInfo in, SubPieceInfo& subpiece_for_download)
	{
		u_int block_sn = in.block_index_;
		u_int piece_sn = in.subpiece_index_;
		std::map<int,ChunkBlockInfo::p>::const_iterator it = block_info_set_.lower_bound(block_sn);

		for(;it!=block_info_set_.end();it = block_info_set_.lower_bound(block_sn))
		{

			if(it->second==ChunkBlockInfo::GetNullBlockInfoObj())
			{
				if(it->first==in.block_index_)
				{
					subpiece_for_download = in;
				}
				else
				{
					subpiece_for_download.block_index_ = it->first;
					subpiece_for_download.subpiece_index_ = 0;
				}
//				STORAGE_DEBUG_LOG("true;!"<<" SubPieceInfo:"<<in<<" subpiece_for_download"<<subpiece_for_download);
				return true;
			}

			int start_index ;
			if(it->first==in.block_index_)
			{
				start_index = in.subpiece_index_;
			}
			else
			{
				block_sn = it->first;
				start_index = 0;
			}
			if(!it->second->GetNextNullSubPiece(start_index,subpiece_for_download.subpiece_index_))
			{
				block_sn++;
				continue;
			}
			subpiece_for_download.block_index_ = block_sn;
//			STORAGE_DEBUG_LOG("true!"<<" SubPieceInfo:"<<in<<" subpiece_for_download"<<subpiece_for_download);
			return true;
		}
		STORAGE_DEBUG_LOG("false!"<<" SubPieceInfo:"<<in<<" subpiece_for_download"<<subpiece_for_download);
		return false;
	}

	u_int ResourceDescriptor::GetContinueSubPieceInfo(SubPieceInfo start_subpiece,SubPieceInfo &end_subpiece)
	{
		assert(start_subpiece<=max_subpiece_info_);
		if(GetNextNullSubPiece(start_subpiece,end_subpiece))
		{
			return (end_subpiece.block_index_ - start_subpiece.block_index_)*(rid_info_.block_size_/bytes_num_per_subpiece_g_)
				+ end_subpiece.subpiece_index_ - start_subpiece.subpiece_index_+1;
		}
		end_subpiece = max_subpiece_info_;
		return (end_subpiece.block_index_ - start_subpiece.block_index_)*(rid_info_.block_size_/bytes_num_per_subpiece_g_)
			+ end_subpiece.subpiece_index_ - start_subpiece.subpiece_index_+1;

	}

	bool ResourceDescriptor::Check(SubPieceInfo in)
	{
		//检查subpiece大小是否超过最大值，边界块（block）的subpiece大小是否合法
		return in<=max_subpiece_info_;
	}

	bool ResourceDescriptor::GenerateRid() 
	{
		if(!rid_info_.rid_.IsEmpty())
		{
			STORAGE_DEBUG_LOG("false: !rid_info_.rid_.IsEmpty()"<<rid_info_);
			return false;
		}
		if(!IsFullFile())
		{
			return false;
		}

		CMD5::p hash = CMD5::Create();
		for(int i = 0;i<rid_info_.block_count_;i++)
		{
			if(rid_info_.block_md5_s_[i].IsEmpty())
			{
				STORAGE_DEBUG_LOG(" false: rid_info_.block_md5_s_[i].IsEmpty() "<<rid_info_);
				return false ;
			}
			hash->Add((LPCVOID)&(rid_info_.block_md5_s_[i]),sizeof(RID));
		}

		hash->Finish();
		SMD5 rid;
		hash->GetHash(&rid);
		rid_info_.rid_ = *(MD5*)(&rid);
		STORAGE_DEBUG_LOG("Resource download finish! RidInfo:"<<rid_info_);
		return true;
	};

	bool  ResourceDescriptor::SetRidInfo( RidInfo rid_info) 
	{
		assert(rid_info.file_length_==rid_info_.file_length_); 
		assert(rid_info_.rid_.IsEmpty());
		rid_info_ = rid_info; 
		return true;
	};

	bool  ResourceDescriptor::SetBlockHash( int block_index,MD5 hash_md5) 
	{
		assert(rid_info_.rid_.IsEmpty());
		assert(block_index<rid_info_.block_count_);
		assert(rid_info_.block_md5_s_[block_index].IsEmpty());
		rid_info_.block_md5_s_[block_index] = hash_md5;
		return true;
	};



}