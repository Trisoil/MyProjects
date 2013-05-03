/** /*/
#include "stdafx.h"
#include <map>
#include "ResourceDescriptor.h"
#include "CfgFile.h"

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
			block_bit_map_->SetAll(true);
			return;
		}
		//将BlockMap用空BLOCK对象填充
		for(u_int block_index = 0;block_index<=max_subpiece_info_.block_index_;block_index++)
		{
			block_info_set_.insert(make_pair(block_index,ChunkBlockInfo::p()));
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
		pointer->block_bit_map_->SetAll(true);
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
				return ResourceDescriptor::p();
			}

			Buffer blockbuf(buf,blockbuflen);
			buf += blockbuflen; buflen -= blockbuflen;

			ChunkBlockInfo::p block_pointer;
			if(!ChunkBlockInfo::Parse(blockbuf,block_pointer))
			{
				return ResourceDescriptor::p();
			}
//			assert(block_pointer);

			pointer->block_info_set_.erase(block_index);

			pointer->block_bit_map_->Reset(block_index);
			pointer->block_info_set_.insert(make_pair(block_index,block_pointer));
			if(block_pointer)
			{
				curr_null_subpiece_count+=block_pointer->GetCurrNullSubPieceCount();
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
		//计算下载字节数！
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

	bool ResourceDescriptor::ToBuffer(Buffer &resource_desc_buf ) const
	{
		int tmp_buffer_size = max_size_cfg_file_g_;
		byte *tmpbuf = new byte[tmp_buffer_size];
		size_t buf_len = 0;
		
		//只记录空block、空或半空的piece
		for(std::map<int,ChunkBlockInfo::p>::const_iterator it = block_info_set_.begin();it!=block_info_set_.end();it++)
		{
			//index
			*(int*)(tmpbuf+buf_len) = it->first;
			buf_len +=4;
			//blockbuf
			Buffer block_buf ;
			ChunkBlockInfo::p chunk_block_info = it->second;
			if(!chunk_block_info)
			{
				block_buf = ChunkBlockInfo::NullBlockToBuffer();
			}
			else
			{
				block_buf = chunk_block_info->ToBuffer();
			}
			*(u_int*)(tmpbuf+buf_len) = block_buf.length_;
			buf_len +=4;

			if(tmp_buffer_size < buf_len + block_buf.length_+ 256)
			{
				tmp_buffer_size = tmp_buffer_size*2;
				byte *tmp2 = tmpbuf;
				tmpbuf = new byte[tmp_buffer_size];
				memcpy(tmpbuf,tmp2,buf_len);
				delete[] tmp2;
			}
			if(block_buf.length_!=0)
			{
				memcpy((tmpbuf+buf_len),block_buf.data_.get(),block_buf.length_);
				buf_len += block_buf.length_;
			}
		}
		resource_desc_buf = Buffer(tmpbuf,buf_len);
		delete[] tmpbuf;
		return true;
	}

	bool ResourceDescriptor::AddSubPieceInfo(SubPieceInfo in, bool &b_block_full)
	{
		//测试检查
		if(!Check(in))
		{
			STORAGE_ERR_LOG("subpiece out of range! index:"<<in.block_index_<<":"<<in.subpiece_index_);
			return false;
		}
		ChunkBlockInfo::p block_info_p;
		std::map<int,ChunkBlockInfo::p>::iterator it = block_info_set_.find(in.block_index_);
		if(it==block_info_set_.end())
		{
			STORAGE_DEBUG_LOG("SubPiece is existed!");
			return false;
		}
		block_info_p = it->second;
		if(!block_info_p)
		{
			//计算最后一块大小
			int block_subpiece_num = rid_info_.block_size_/bytes_num_per_subpiece_g_;
			assert(rid_info_.block_size_%bytes_num_per_subpiece_g_==0);
			if(in.block_index_==max_subpiece_info_.block_index_)
			{
				block_subpiece_num = max_subpiece_info_.subpiece_index_+1;
			}
			block_info_set_.erase(it);
			block_info_p = ChunkBlockInfo::Create(block_subpiece_num);
			block_info_set_.insert(make_pair(in.block_index_, block_info_p));
		}

		if(block_info_p->AddSubPieceInfo(in.subpiece_index_))
		{
			download_bytes_ += (in==max_subpiece_info_?(rid_info_.file_length_-1)%bytes_num_per_subpiece_g_+1:bytes_num_per_subpiece_g_);
			if(block_info_p->IsFullBlock())
			{
				block_info_set_.erase(in.block_index_);
				block_bit_map_->Set(in.block_index_);
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
		if(!it->second)
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
		if(!it->second)
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
		return block_bit_map_->Clone();
		//BlockMap::p pointer = BlockMap::Create(max_subpiece_info_.block_index_+1);
		//for(int i = 0;i<pointer->GetSize();i++)
		//{
		//	if(IsPublishBlock(i))
		//		pointer->Set(i);
		//}
		//return pointer;
	}

	void ResourceDescriptor::RemoveBlockInfo(size_t block_index) 
	{
		std::map<int,ChunkBlockInfo::p>::const_iterator it = block_info_set_.find(block_index);
		if(it!=block_info_set_.end())
		{
			if(!it->second)
			{
				return;
			}
			download_bytes_ -= it->second->GetCurrNullSubPieceCount();
		}
		else
		{
			download_bytes_ -= (block_index==max_subpiece_info_.block_index_?(rid_info_.file_length_-1)%(rid_info_.block_size_)+1: rid_info_.block_size_);
		}
		block_info_set_.erase(block_index);

		block_bit_map_->Reset(block_index);
		block_info_set_.insert(make_pair(block_index,ChunkBlockInfo::p()));
	}


	bool ResourceDescriptor::GetNextNullSubPiece(const SubPieceInfo in, SubPieceInfo& subpiece_for_download)
	{
		u_int block_sn = in.block_index_;
		u_int piece_sn = in.subpiece_index_;
		std::map<int,ChunkBlockInfo::p>::const_iterator it = block_info_set_.lower_bound(block_sn);

		for(;it!=block_info_set_.end();it = block_info_set_.lower_bound(block_sn))
		{

			if(!it->second)
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

	bool  ResourceDescriptor::InitRidInfo( RidInfo rid_info) 
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