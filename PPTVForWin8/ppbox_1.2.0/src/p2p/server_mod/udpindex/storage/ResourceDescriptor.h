/*
实现文件资源的分层描述 ResourceDescriptor.h
*/
#pragma once
#include <map>
#include <boost\dynamic_bitset.hpp>
#include "storage_base.h"
#include "base\blockmap.h"
#include "storage\ChunkBlockInfo.h"

//实现文件资源的分层描述

namespace storage
{
	class ResourceDescriptor
		:boost::noncopyable
	{
	public:
		friend class Resource ;

		typedef boost::shared_ptr<ResourceDescriptor> p;
	public:
		static p Create(const size_t &resource_size,bool b_full); //创建
		static p Create(const RidInfo &rid_info,bool b_full); //创建
		static p Parse(Buffer cfg_buf,const RidInfo &rid_info);//解析资源描述

		friend   ostream& operator<<(ostream& os, const ResourceDescriptor& rd)
		{
			return os << "ResourceDescriptor: "<<rd.rid_info_<<" download_bytes:"<<rd.download_bytes_<<" block_info_set_.size:"<<rd.block_info_set_.size();
		}


	private:
		ResourceDescriptor(const RidInfo &rid_info,bool b_full_file);
	public:
		bool ToBuffer(Buffer &resource_desc_buffer) const;

		//属性
		BlockMap::p GetBlockMap();
		bool ResourceDescriptor::GenerateRid(); 
		bool  InitRidInfo( RidInfo rid_info);
		bool  SetBlockHash( int block_index,MD5 hash_md5); 

		//操作
		bool AddSubPieceInfo(SubPieceInfo in,bool &b_block_full);
		//查询 

		bool HasSubPiece(SubPieceInfo in); 
		bool HasPiece(const PieceInfo& piece_info);
		bool HasFullBlock(const int  block_index) {return block_info_set_.find(block_index)==block_info_set_.end();};
		bool IsPublishBlock(u_int blockIndex);
		bool GetNextNullSubPiece(const SubPieceInfo sub_subpiece_index, SubPieceInfo& subpiece_for_download);
		//		bool GetNextSubPieceInfo(const SubPieceInfo sub_subpiece_index, SubPieceInfo& subpiece_for_download);

		u_int GetBlockCount() {return rid_info_.block_count_;};
		u_int GetBlockSize() {return rid_info_.block_size_;};
		size_t GetFileSize() { return rid_info_.file_length_;};
		size_t GetDownloadBytes() { return download_bytes_;};



		u_int GetContinueSubPieceInfo(SubPieceInfo start_subpiece,SubPieceInfo &end_subpiece);

		bool IsFullFile() {return block_info_set_.size()==0; };

		virtual void RemoveBlockInfo(size_t block_index) ;

	protected:
		//		void AddChunkPieceInfo(PieceInfo piece_info, ChunkPieceInfo::p chunk_piece_info);

		bool Check(SubPieceInfo in);

		static bool CfgFileAuthenticationVerify(Buffer cfg_buf);

		static bool CfgFileAuthenticationSign(Buffer cfg_buf);

		static bool CheckCfgBuf(Buffer cfg_buf);


		//void RefreshPublishStatus();
	public:
		//辅助函数

		inline bool PosToSubPieceInfo(size_t position,SubPieceInfo&subpiec_info)
		{
			if(position>=rid_info_.file_length_)
			{
				STORAGE_WARN_LOG("PosToSubPieceInfo 越界 文件长度："<<rid_info_.file_length_<<"  pos:"<<position);
				return false;
			}
			subpiec_info.block_index_ = position/rid_info_.block_size_;
			subpiec_info.subpiece_index_ = position%rid_info_.block_size_/bytes_num_per_subpiece_g_;
			return true;
		}
		inline bool PosToPieceInfo(size_t position,PieceInfo &piec_info)
		{
			if(position>=rid_info_.file_length_)
			{
				STORAGE_WARN_LOG("PosToPieceInfo 越界 文件长度："<<rid_info_.file_length_<<"  pos:"<<position);
				return false;
			}
			piec_info.block_index_ = position/rid_info_.block_size_;
			piec_info.piece_index_ = position%rid_info_.block_size_/bytes_num_per_piece_g_;
			return true;
		}
		inline void SubPieceInfoToPieceInfoEx(const SubPieceInfo &subpiece_info,PieceInfoEx &piece_info_ex)
		{
			piece_info_ex.block_index_ = subpiece_info.block_index_;
			piece_info_ex.piece_index_ = subpiece_info.subpiece_index_/subpiece_num_per_piece_g_;
			piece_info_ex.subpiece_index_ = subpiece_info.subpiece_index_%subpiece_num_per_piece_g_;
		}
		inline void PieceInfoToSubPieceInfo(const PieceInfo &piece_info,SubPieceInfo &subpiece_info)
		{
			subpiece_info.block_index_ = piece_info.block_index_ ; 
			subpiece_info.subpiece_index_ = piece_info.piece_index_*subpiece_num_per_piece_g_;
		}
		inline size_t SubpieceInfoToPosition(const SubPieceInfo &subpiec_info)
		{
			return subpiec_info.block_index_ *rid_info_.block_size_ + subpiec_info.subpiece_index_ *bytes_num_per_subpiece_g_;
		}

		inline int GetBlockSubPieceCount(size_t block_index)
		{
			if(block_index==max_subpiece_info_.block_index_)
			{
				return max_subpiece_info_.subpiece_index_+1;
			}
			return rid_info_.block_size_/bytes_num_per_subpiece_g_;
		}

		inline void GetBlockPosition(size_t block_index,size_t &offset,u_int &length)
		{
			assert(block_index<=max_subpiece_info_.block_index_);
			offset = block_index*rid_info_.block_size_;
			if(block_index==rid_info_.file_length_/rid_info_.block_size_)
			{
				length = rid_info_.file_length_%rid_info_.block_size_;
			}
			else
			{
				length = rid_info_.block_size_;
			}
			return ;
		}

		inline void GetSubPiecePosition(const SubPieceInfo &subpiec_info,size_t &offset,u_int &length)
		{
			assert(subpiec_info<=max_subpiece_info_);
			if(subpiec_info==max_subpiece_info_)
			{
				length = (rid_info_.file_length_-1)%bytes_num_per_subpiece_g_+1;
			}
			else
			{
				length = bytes_num_per_subpiece_g_;
			}
			offset = subpiec_info.block_index_*rid_info_.block_size_+subpiec_info.subpiece_index_*bytes_num_per_subpiece_g_;
			assert(offset+length<=rid_info_.file_length_);
			return ;
		}

		inline bool IncSubPieceInfo(SubPieceInfo &subpiec_info) 
		{
			if(subpiec_info.subpiece_index_==rid_info_.block_size_/bytes_num_per_subpiece_g_-1)
			{
				subpiec_info.subpiece_index_ = 0;
				subpiec_info.block_index_++;
			}
			else
			{
				subpiec_info.subpiece_index_++;
			}
			if(SubpieceInfoToPosition(subpiec_info)>=rid_info_.file_length_)
			{
				return false;
			}
			return true;
		}

		inline SubPieceInfo GetMaxSubPieceInfo() { return max_subpiece_info_; }

	public:
		RidInfo rid_info_;
	protected:
		//文件资源描述管理-block层
		std::map<int,ChunkBlockInfo::p> block_info_set_;
		BlockMap::p block_bit_map_;

		//最大边界subpiece
		SubPieceInfo max_subpiece_info_;
		size_t download_bytes_;
	};
}