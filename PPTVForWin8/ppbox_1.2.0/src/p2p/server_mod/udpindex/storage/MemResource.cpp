#include "stdafx.h"

#include "base\BlockMap.h"

#include "storage_base.h"
#include "storage/IStorage.h"
#include "storage/PieceInfo.h"
#include "storage/PendingSubpieceManager.h"
#include "storage/resource.h"

#include "storage\MemResource.h"
#include "storage\instance.h"
#include "framework\mainThread.h"

namespace storage
{
	MemResource::p MemResource::CreateResource(size_t file_length,boost::shared_ptr<Instance> inst_p)
	{
		MemResource::p pointer  =  p(new MemResource( file_length,inst_p));
		return pointer;
	}

	MemResource::MemResource(size_t file_length,boost::shared_ptr<Instance> inst_p)
		:Resource(file_length,false,inst_p)
	{
	}

	void MemResource::Close()
	{
		resource_stor_.clear();
		Resource::Close();
		MainThread::IOS().post(
			boost::bind( &Instance::OnResourceCloseFinish,instance_p_, shared_from_this())
			);
	}


	void MemResource::RemoveBlock(u_int block_index)
	{
		size_t offset = 0;
		u_int  length = 0;
		GetBlockPosition(block_index,offset,length);
		assert(length>0);
		u_int subpiece_num =(length-1)/bytes_num_per_subpiece_g_+1;
		SubPieceInfo subpiece_info;
		subpiece_info.block_index_ = block_index;
		map<SubPieceInfo,Buffer>::const_iterator it;
		for(int i = 0;i<subpiece_num;i++)
		{
			subpiece_info.subpiece_index_ = i;
			assert(resource_stor_.erase(subpiece_info)==1);
		}
	}


	void MemResource::ReadSubPiece(const SubPieceInfo& subpiece_info, const boost::asio::ip::udp::endpoint& end_point, IUploadListener::p listener)
	{

		map<SubPieceInfo,Buffer>::const_iterator it = resource_stor_.find(subpiece_info);
		assert(it!=resource_stor_.end());
		MainThread::IOS().post(
			boost::bind( &IUploadListener::OnAsyncGetSubPieceSucced, listener,file_resource_desc_p_->GetRidInfo().GetRID(),subpiece_info,end_point,it->second)
			);
		return;
	}

	void MemResource::WriteSubPiece(const SubPieceInfo& subpiece_info, const Buffer& buffer) 
	{
		assert(buffer.length_>0&&<buffer.length_<bytes_num_per_subpiece_g_);
		assert(resource_stor_.find(subpiece_info)==resource_stor_.end());
		resource_stor_.insert(make_pair(subpiece_info,buffer));
		return;
	}



	void MemResource::ReadSubPieceForPlay(const SubPieceInfo& subpiece_info, IPlayerListener::p listener) 
	{
		map<SubPieceInfo,Buffer>::const_iterator it = resource_stor_.find(subpiece_info);
		assert(it!=resource_stor_.end());
		assert(it->second.length_>0&&it->second.length_<bytes_num_per_subpiece_g_);

		size_t start_position = subpiece_info.block_index_*file_resource_desc_p_->GetRidInfo().block_size_+subpiece_info.subpiece_index_*bytes_num_per_subpiece_g_;
		MainThread::IOS().post(
			boost::bind( &IPlayerListener::OnAsyncGetSubPieceSucced, listener,start_position,it->second)
			);
	}

	MD5 MemResource::HashBlock(u_int block_index)
	{
		//Hash block
		size_t offset = 0;
		u_int  length = 0;
		GetBlockPosition(block_index,offset,length);
		assert(length>0);
		u_int subpiece_num =(length-1)/bytes_num_per_subpiece_g_+1;
		SubPieceInfo subpiece_info;
		subpiece_info.block_index_ = block_index;
		map<SubPieceInfo,Buffer>::const_iterator it;
		CMD5::p hash = CMD5::Create();
		for(int i = 0;i<subpiece_num;i++)
		{
			subpiece_info.subpiece_index_ =i;
			it= resource_stor_.find(subpiece_info);
			assert(it!=resource_stor_.end());
			hash->Add(it->second.data_.get(),it->second.length_);
		}
		hash->Finish();
		SMD5 hash_val;
		hash->GetHash(&hash_val);

		//POST
		MD5 hash_1 = *(MD5*)(&hash_val);
		STORAGE_DEBUG_LOG("MemResource::HashBlock!..........block_index:"<<block_index<<"block hash :"<<hash_1);

		return hash_1;
	}
}







