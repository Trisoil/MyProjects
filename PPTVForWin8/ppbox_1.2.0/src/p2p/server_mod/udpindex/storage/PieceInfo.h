/*
实现文件资源的分层描述 ResourceDescriptor.h
*/
#pragma once
#include <map>
#include <boost\dynamic_bitset.hpp>
#include "storage_base.h"
#include "base\blockmap.h"
#include "storage\CfgFile.h"


namespace storage
{
	static const int max_size_cfg_file_g_ = 2*1024*1024;
	static const int version_g_ = 0x03050700;
	static const int hash_len_g_ = 16;

	#define LAST_   0x08 //#define NORMAL_ 0x08
	#define PIECE_  0x04 //#define BLOCK_  0x04
	#define NULL_   0x01

		static const BYTE NULL_NORMAL_PIECE = NULL_|PIECE_ ;
		static const BYTE NULL_NORMAL_BLOCK = NULL_;
		static const BYTE NONULL_NORMAL_PIECE  = PIECE_ ;
		static const BYTE NONULL_LAST_PIECE = LAST_|PIECE_;

	class  ChunkPieceInfo
		:public boost::noncopyable
	{
	public:
		typedef boost::shared_ptr<ChunkPieceInfo> p;
		static ChunkPieceInfo::p null_piece_;//= p( new ChunkPieceInfo());//just one!

	private:
		ChunkPieceInfo(u_short subpiece_num = subpiece_num_per_piece_g_);  //1-128(subpiece_num_per_piece_g_)
		ChunkPieceInfo( Buffer inbuf);
	public:
		static ChunkPieceInfo::p GetNullInfoObj();
		static ChunkPieceInfo::p CreateChunkPieceInfo(u_int subpiece_num);

		static ChunkPieceInfo::p Parse(Buffer piece_set);
		Buffer ToBuffer();

		bool AddSubPieceInfo(u_int index);
		bool GetNextNullSubPiece(size_t start,size_t &next_index);
		bool HasSubPiece(u_int index); //0-127(subpiece_num_per_piece_g_-1);
		inline size_t GetCurrNullSubPieceCount() { return subpiece_set_.count();}; //0表示有，1表示没有
		inline size_t GetSubPieceCount() { return subpiece_set_.size();}; 

		bool IsFull();
		size_t GetSize();
	private:
		static bool CheckBuf(Buffer inbuf,int &bitcount,int &bitbuffersize);

		bool Check(string in_stream,u_int bits_size) {return true;};
		typedef boost::dynamic_bitset<u_int> _block_bitset;
		_block_bitset subpiece_set_;
	};



	class	ChunkBlockInfo 
		:boost::noncopyable
	{
	public:
		typedef boost::shared_ptr<ChunkBlockInfo> p;
		static ChunkBlockInfo::p null_block_ ;//= p( new ChunkBlockInfo()); //just one!

	public:
		static ChunkBlockInfo::p GetNullBlockInfoObj();
		static ChunkBlockInfo::p CreateChunkBlockInfoObj(u_int subpiece_num=default_subpiece_num_per_block_g_,bool b_init_null = true);
		static ChunkBlockInfo::p Parse(Buffer blockinfo_buf);
		Buffer ToBuffer();
		//属性
		bool HasSubPiece(const u_int subpiece_index);
		bool HasPiece(const PieceInfo& piece_info);
		bool IsFull()  { return piece_info_set_.size()==0;};
		size_t GetCurrSubPieceCount()const { return curr_subpiece_count_;};
		size_t GetSubPieceCount() const  { return max_subpiece_index_+1;};


		//操作
//		void AddChunkPieceInfo(PieceInfo piece_info, ChunkPieceInfo::p chunk_piece_info,bool& b_full);
		bool AddSubPieceInfo(u_int subpiece_index);
		bool GetNextNullSubPiece(const u_int sub_subpiece_index, u_int& subpiece_for_download);

	protected:
		std::map<u_short,ChunkPieceInfo::p> piece_info_set_;
	private:
		ChunkBlockInfo(u_int piece_num=piece_num_per_block_g_,bool b_full_block=false);
		bool Check(u_int max_num_subpiece);

	private:
		//static p full_block_;//never assign!

		//block最大边界
		u_int  max_subpiece_index_;
		u_int  curr_subpiece_count_;
	};


//实现文件资源的分层描述
	using boost::detail::lightweight_mutex;

	class ResourceDescriptor
		:boost::noncopyable
	{
	public:
		friend class Resource ;

		typedef boost::shared_ptr<ResourceDescriptor> p;
	public:
		static p Create(const size_t &resource_size,bool b_full); //创建
		static p Create(const RidInfo &rid_info,bool b_full); //创建
		static p Parse(Buffer cfg_buf,const RidInfo &rid_info);//解析创建资源描述

	private:
		ResourceDescriptor(const RidInfo &rid_info,bool b_full_file);
	public:
		Buffer ToBuffer();

		//属性
		BlockMap::p GetBlockMap();
		bool ResourceDescriptor::GenerateRid(); 
		bool  SetRidInfo( RidInfo rid_info);
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

		virtual void RemoveBlockInfo(u_int block_index) ;

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
		inline void SubPieceInfoToPieceInfo(SubPieceInfo &subpiece_info,PieceInfoEx &piece_info_ex)
		{
			piece_info_ex.block_index_ = subpiece_info.block_index_;
			piece_info_ex.piece_index_ = subpiece_info.subpiece_index_/subpiece_num_per_piece_g_;
			piece_info_ex.subpiece_index_ = subpiece_info.subpiece_index_%subpiece_num_per_piece_g_;

		}
		inline size_t SubpieceInfoToPosition(const SubPieceInfo &subpiec_info)
		{
			return subpiec_info.block_index_ *rid_info_.block_size_ + subpiec_info.subpiece_index_ *bytes_num_per_subpiece_g_;
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
	public:
		RidInfo rid_info_;
	protected:
		//文件资源描述管理-block层
		std::map<int,ChunkBlockInfo::p> block_info_set_;
		BlockMap::p block_bit_map_;

	private:
		//最大边界subpiece
		SubPieceInfo max_subpiece_info_;
		size_t download_bytes_;
	};
}
