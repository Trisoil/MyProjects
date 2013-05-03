#include "stdafx.h"


#include "base\BlockMap.h"
#include "framework/timer/Timer.h"

#include "storage_base.h"
#include "storage/IStorage.h"
#include "storage/ResourceDescriptor.h"
#include "storage/PendingSubpieceManager.h"

#include "storage/resource.h"
#include "storage/Instance.h"
#include "storage/Storage.h"
#include "storage/StorageThread.h"
#include "storage/SpaceManager.h"


namespace storage
{

	Resource::Resource(size_t file_length,tstring file_name,boost::shared_ptr<Instance> inst_p, size_t init_size)
		:instance_p_(inst_p),is_running_(false),need_saveinfo_to_disk_(false),actual_size_(init_size)
	{
		if(instance_p_)
		{
			is_running_ = true;
		}
		file_resource_desc_p_ = ResourceDescriptor::Create(file_length,false);
		assert(file_resource_desc_p_);
		file_name_ = file_name;
	}

	Resource::Resource(ResourceDescriptor::p resource_desc_p,tstring file_name,boost::shared_ptr<Instance> inst_p, size_t actual_size)
		:instance_p_(inst_p),is_running_(false),need_saveinfo_to_disk_(false),actual_size_(actual_size)
	{
		if(instance_p_)
		{
			is_running_ = true;
		}
		file_resource_desc_p_ = resource_desc_p;
		assert(file_resource_desc_p_);
		file_name_ = file_name;
	}

	void Resource::BindInstance(boost::shared_ptr<Instance> instance_p)
	{
		assert(is_running_==false);
		assert(!instance_p_);
		instance_p_ = instance_p;
		if(instance_p_)
			is_running_ = true;
		else
			is_running_ = false;
	}

	ResourceDescriptor::p Resource::CloneResourceDescriptor()
	{
		assert(is_running_==false);
		Buffer buf;
		if(!file_resource_desc_p_->ToBuffer(buf))
			return ResourceDescriptor::p();

		return ResourceDescriptor::Parse(buf,file_resource_desc_p_->rid_info_);
	}

	void Resource::SecSaveResourceFileInfo()
	{
		if(is_running_==false)
		{
			return;
		}

		if(!need_saveinfo_to_disk_)
		{
			return;
		}

		STORAGE_DEBUG_LOG(""<<framework::w2b(file_name_)<<" 长度:"<<file_resource_desc_p_->rid_info_.file_length_<<"bytes! 已下载"
			<<file_resource_desc_p_->GetDownloadBytes()<<"bytes!");

		FlushStore();

		CfgFile cfg_file;
		if(!cfg_file.SecCreate(file_name_,file_resource_desc_p_->GetFileSize()))
		{
			STORAGE_ERR_LOG("无法保留文件资源描述，无法建立cfg文件"<<file_resource_desc_p_->rid_info_.file_length_);
			return;
		}
		Buffer buf;
		if(!file_resource_desc_p_->ToBuffer(buf))
		{
			cfg_file.SecClose();
			return;
		}

		if(!cfg_file.AddContent(buf))
		{
			cfg_file.SecClose();
			return;
		}
		cfg_file.SecClose();
		need_saveinfo_to_disk_ = false;
	}

	tstring  Resource::GetLocalFileName()
	{
//		assert(is_running_==false);
		return file_name_;
	}

	// 将一个block的subpiece写入文件，不计算MD5
	void Resource::ThreadWriteBlock(size_t block_index, std::map<SubPieceInfo,Buffer> block_pending_buffer_set)
	{
		if(is_running_==false)
		{
			return;
		}

		STORAGE_DEBUG_LOG(":"<<block_index<<" pending buffer count:"<<block_pending_buffer_set.size());
		std::map<SubPieceInfo,Buffer>::iterator it = block_pending_buffer_set.begin();
		for(; it != block_pending_buffer_set.end(); ++it)
		{
			ThreadSecWriteSubPiece(it->first, it->second);
		}
	}

	// 将一个block的subpiece写入文件，并计算该block的MD5值，然后交给Instance,读取需要上传的block，上传
	void Resource::ThreadPendingHashBlock(size_t block_index,std::map<SubPieceInfo,Buffer> block_pending_buffer_set)
	{
		if(is_running_==false)
		{
			return;
		}

		STORAGE_DEBUG_LOG(":"<<block_index<<" pending buffer count:"<<block_pending_buffer_set.size());
		std::map<SubPieceInfo,Buffer>::iterator it = block_pending_buffer_set.begin();
		for(; it != block_pending_buffer_set.end(); ++it)
		{
			ThreadSecWriteSubPiece(it->first, it->second);
		}
		assert(file_resource_desc_p_->HasFullBlock(block_index));

		//Hash block
		size_t offset = 0;
		u_int  length = 0;
		CMD5::p hash = CMD5::Create();
		// 获取block_index在文件中的偏移地址和block长度
		file_resource_desc_p_->GetBlockPosition(block_index,offset,length);
		Buffer buf = ReadBuffer(offset,length); // 由派生类实现
		assert(buf.length_==length);

		hash->Add(buf.data_.get(),buf.length_);
		hash->Finish();
		SMD5 hash_val;
		hash->GetHash(&hash_val);
		STORAGE_DEBUG_LOG("Block:"<<block_index<<" Hash:"<<*(MD5*)(&hash_val));

		// block写入完毕，赋予MD5值，检查文件是否写入完毕，完毕则通知其他模块下载完毕
		// 读取需要上传的block，上传
		MainThread::IOS().post(
			boost::bind( &Instance::OnPendingHashBlockFinish, instance_p_,block_index,*(MD5*)(&hash_val))
			);
	}

	// 将subpiece写入文件，并计算content的MD5值，然后交给Instance
	void Resource::ThreadPendingHashContent(std::map<SubPieceInfo,Buffer> block_pending_buffer_set, size_t content_bytes)
	{
		if(is_running_==false)
		{
			return;
		}

		STORAGE_DEBUG_LOG(":"<<" pending buffer count:"<<block_pending_buffer_set.size());
		std::map<SubPieceInfo,Buffer>::iterator it = block_pending_buffer_set.begin();
		for(;it!=block_pending_buffer_set.end();it++)
		{
			ThreadSecWriteSubPiece(it->first,it->second);
		}

		//Hash block

		CMD5::p hash = CMD5::Create();
		Buffer buf = ReadBuffer(0,content_bytes);
		assert(buf.length_==content_bytes);

		hash->Add(buf.data_.get(),buf.length_);
		hash->Finish();
		SMD5 hash_val;
		hash->GetHash(&hash_val);

		MainThread::IOS().post(
			boost::bind( &Instance::OnPendingHashContentFinish, instance_p_,*(MD5*)(&hash_val), content_bytes)
			);
	}

	void Resource::ThreadGetSubPieceForPlay(const SubPieceInfo subpiece_info, IPlayerListener::p listener)
	{
		if(is_running_==false) 
		{
			return ;
		}

		Buffer ret_buf ;
		if(!file_resource_desc_p_->HasSubPiece(subpiece_info))
		{
			MainThread::IOS().post(
				boost::bind( 
				&IPlayerListener::OnAsyncGetSubPieceFailed, 
				listener,
				file_resource_desc_p_->SubpieceInfoToPosition(subpiece_info),
				ERROR_GET_SUBPIECE_NOT_FIND_SUBPIECE)
				);
			return;
		}
		size_t startoffset,length;
		file_resource_desc_p_->GetSubPiecePosition(subpiece_info,startoffset,length);
		ret_buf = ReadBuffer(startoffset, length);
		assert((ret_buf.length_<=bytes_num_per_piece_g_)&&(ret_buf.length_>0));
		MainThread::IOS().post(
			boost::bind( &IPlayerListener::OnAsyncGetSubPieceSucced,
			listener,
			file_resource_desc_p_->SubpieceInfoToPosition(subpiece_info),
			ret_buf)
			);
		return;
	}

	// 从某个subpiece处读取不超过n个的连续subpiece，然后交给player_listener
	void Resource::ThreadReadBufferForPlay(const SubPieceInfo subpiece_info, size_t n, IPlayerListener::p listener)
	{
		if (false == is_running_)
		{
			return;
		}
		Buffer ret_buf;
		if (false == file_resource_desc_p_->HasSubPiece(subpiece_info))
		{
			STORAGE_DEBUG_LOG("GetSubPieceFailed! Not Has SubPiece: "<<subpiece_info);
			MainThread::IOS().post(
				boost::bind(
                    &IPlayerListener::OnAsyncGetSubPieceFailed, 
				    listener,
				    file_resource_desc_p_->SubpieceInfoToPosition(subpiece_info),
				    ERROR_GET_SUBPIECE_NOT_FIND_SUBPIECE
                )
			);
		}

		size_t start_offset = file_resource_desc_p_->SubpieceInfoToPosition(subpiece_info);
		size_t continue_len = 1;
		SubPieceInfo tmp_subpiece_info = subpiece_info;
		bool tmp_is_max = false;
		while (true)
		{
			if (tmp_subpiece_info == file_resource_desc_p_->GetMaxSubPieceInfo())
			{
				tmp_is_max = true;
				break;
			}
			if (false == file_resource_desc_p_->IncSubPieceInfo(tmp_subpiece_info))
			{
				break;
			}
			// 有该subpiece并且不在内存中(说明在磁盘中)
			if (file_resource_desc_p_->HasSubPiece(tmp_subpiece_info) 
                /*&& !Storage::Inst_Storage()->InstancePendingHaveSubPiece(instance_p_,tmp_subpiece_info)*/)
			{
				++continue_len;
				if(continue_len >= n) break; // 不能超过n个subpiece
				continue;
			}
			break;
		}
		size_t length;
		if (tmp_is_max)
		{
			size_t tmp_off, tmp_len;
			file_resource_desc_p_->GetSubPiecePosition(tmp_subpiece_info, tmp_off, tmp_len);
			length = (continue_len-1) * bytes_num_per_subpiece_g_ + tmp_len;
		}
		else
		{
			length = continue_len * bytes_num_per_subpiece_g_;
		}

		ret_buf = ReadBuffer(start_offset, length);

		STORAGE_DEBUG_LOG("读磁盘：(start offset: "<<start_offset<<", Length: "<<length<<")");
		MainThread::IOS().post(
			boost::bind( 
			&IPlayerListener::OnAsyncGetBufferSucced,
			listener,
			file_resource_desc_p_->SubpieceInfoToPosition(subpiece_info),
			ret_buf)
			);
		return;
	}

	void   Resource::ThreadMergeSubPieceToInstance(const SubPieceInfo subpiece_info, boost::shared_ptr<Instance> merge_to_instance_p)
	{
		if(is_running_==false) 
		{
			return ;
		}

		if(!file_resource_desc_p_->HasSubPiece(subpiece_info))
		{
			STORAGE_ERR_LOG("merge subpiece fail! "<<subpiece_info);
			return;
		}
		Buffer ret_buf ;
		size_t startoffset,length;
		file_resource_desc_p_->GetSubPiecePosition(subpiece_info,startoffset,length);
		ret_buf = ReadBuffer(startoffset, length);
		assert((ret_buf.length_<=bytes_num_per_piece_g_)&&(ret_buf.length_>0));
		MainThread::IOS().post(
			boost::bind( &Instance::OnMergeSubPieceSuccess, merge_to_instance_p,subpiece_info,ret_buf)
			);
		STORAGE_ERR_LOG("merge subpiece from resource! "<<subpiece_info);
		return;

	}
	//void Resource::ThreadGetSubPieceForPlay(size_t start_postion, u_int max_count, IPlayerListener::p listener)
	//{
	//	if( is_running_ == false ) return;
	//	SubPieceInfo start_s_info;	
	//	Buffer ret_buf ;


	//	if(!file_resource_desc_p_->PosToSubPieceInfo(start_postion,start_s_info))
	//	{
	//		MainThread::IOS().post(
	//			boost::bind( &IPlayerListener::OnAsyncGetSubPieceFailed, listener,start_postion,ERROR_GET_SUBPIECE_OUTOFRANGE)
	//			);
	//		return;
	//	}

	//	for(int i = 0; i<max_count;i++)
	//	{
	//		{//为lightweight_mutex::scoped_lock lock使用，不要删
	//			lightweight_mutex::scoped_lock lock(instance_p_->criticalsection_);
	//			//pending库中查找
	//			ret_buf = instance_p_->pending_subpiece_manager_p_->Get(start_s_info);
	//		}
	//		if(ret_buf.length_==0)
	//		{
	//			if(!file_resource_desc_p_->HasSubPiece(start_s_info))
	//			{
	//				break;
	//			}
	//			size_t startoffset,length;
	//			file_resource_desc_p_->GetSubPiecePosition(start_s_info,startoffset,length);
	//			ret_buf = ReadBuffer(startoffset, length);
	//		}

	//		assert((ret_buf.length_<=bytes_num_per_piece_g_)&&(ret_buf.length_>0));
	//		MainThread::IOS().post(
	//			boost::bind( &IPlayerListener::OnAsyncGetSubPieceSucced, listener,file_resource_desc_p_->SubpieceInfoToPosition(start_s_info),ret_buf)
	//			);
	//		//超界检查
	//		if(!file_resource_desc_p_->IncSubPieceInfo(start_s_info))
	//		{
	//			break;
	//		}
	//	}
	//}

	// 从文件中读取指定的subpiece，然后交给IUploadListener
	void Resource::ThreadSecReadSubPiece(const RID& rid,const SubPieceInfo& subpiece_info,
		const REndpoint& end_point, RequestSubPiecePacket::p packet, IUploadListener::p listener)
	{
		if(is_running_==false)
		{
			return;
		}
		assert(!rid.IsEmpty());
		if(!file_resource_desc_p_->HasSubPiece(subpiece_info))
		{
			MainThread::IOS().post(
				boost::bind( &IUploadListener::OnAsyncGetSubPieceFailed, listener,rid,
				subpiece_info,end_point,ERROR_GET_SUBPIECE_NOT_FIND_SUBPIECE, packet)
				);
		}

		Buffer ret_buf;
		size_t startoffset,length;
		file_resource_desc_p_->GetSubPiecePosition(subpiece_info,startoffset,length);
		ret_buf = ReadBuffer(startoffset, length);
		assert((ret_buf.length_<=bytes_num_per_piece_g_)&&(ret_buf.length_>0));
		MainThread::IOS().post(
			boost::bind( &IUploadListener::OnAsyncGetSubPieceSucced, listener,rid,
			subpiece_info,end_point,ret_buf, packet)
			);
		return;
	}

	// 从文件中读取指定的block，然后交给IUploadListener
	void Resource::ThreadReadBlockForUpload(const RID& rid,const size_t block_index, 
		IUploadListener::p listener, bool need_hash)
	{
		if (false == is_running_)
		{
			return;
		}
		STORAGE_TEST_DEBUG("Read Block From Disk------->block index:"<<block_index);
		assert(!rid.IsEmpty());
		if (!file_resource_desc_p_->HasFullBlock(block_index))
		{
			STORAGE_TEST_DEBUG("Do not has full block---->block index:"<<block_index);
			MainThread::IOS().post(
				boost::bind(&IUploadListener::OnAsyncGetBlockFailed, listener,rid,block_index,ERROR_GET_SUBPIECE_NOT_FIND_SUBPIECE)
				);
		}

		Buffer ret_buf;
		size_t startoffset,length;
		file_resource_desc_p_->GetBlockPosition(block_index,startoffset,length);
		ret_buf = ReadBuffer(startoffset,length);

		if (need_hash)
		{
			CMD5::p hash = CMD5::Create();
			hash->Add(ret_buf.data_.get(),ret_buf.length_);
			hash->Finish();
			SMD5 hash_val;
			hash->GetHash(&hash_val);
			MainThread::IOS().post(
				boost::bind(&Instance::OnHashBlockFinish,instance_p_,block_index,*(MD5*)(&hash_val),
				ret_buf,listener)
				);
			return;
		}
		MainThread::IOS().post(
			boost::bind(&IUploadListener::OnAsyncGetBlockSucced,listener,rid,block_index,ret_buf)
			);
		STORAGE_TEST_DEBUG("OnAsyncGetBlockSucced from disk------>block index:"<<block_index);
		return;
	}

	void Resource::ThreadWriteBuffer(set<SubPieceInfo>& splist, Buffer buf)
	{
		if (false == is_running_)
		{
			return;
		}
		assert(!file_resource_desc_p_->HasSubPiece(*splist.begin()));
		size_t startoffset, length;
		file_resource_desc_p_->GetSubPiecePosition(*splist.begin(),startoffset,length);
		WriteBuffer(startoffset,buf);
		for (set<SubPieceInfo>::const_iterator it = splist.begin(); it!=splist.end(); ++it)
		{
			bool b_block_full = false;

			file_resource_desc_p_->AddSubPieceInfo(*it,b_block_full);

			// 从poolout中删除某个subpiece
			MainThread::IOS().post(
				boost::bind( &Instance::OnWriteSubPieceFinish,instance_p_,*it)
				);
			if(!b_block_full) 
				return;

			if(file_resource_desc_p_->IsFullFile() && Storage::Inst_Storage()->InstancePendingEmpty(instance_p_))
			{
				if(TryRenameToNormalFile())
				{
					MainThread::IOS().post(
						boost::bind( &Instance::OnDiskFileNameChange,instance_p_,file_name_)
						);	
				}
				need_saveinfo_to_disk_ = false;
				return;
			}
		} // for
	}

	void Resource::ThreadSecWriteSubPiece(SubPieceInfo subpiece_info,Buffer buf)
	{
		if(is_running_==false)
		{
			return;
		}
		//写入
		assert(!file_resource_desc_p_->HasSubPiece(subpiece_info));
		//{
		//	STORAGE_DEBUG_LOG("shit!!!!!"<<subpiece_info<<" buflength:"<<buf.length_);
		//	return;
		//}
		size_t startoffset,length;
		file_resource_desc_p_->GetSubPiecePosition(subpiece_info,startoffset,length);

		WriteBuffer(startoffset,buf);

		bool b_block_full = false;

		file_resource_desc_p_->AddSubPieceInfo(subpiece_info,b_block_full);

		// 从poolout中删除某个subpiece
		MainThread::IOS().post(
			boost::bind( &Instance::OnWriteSubPieceFinish,instance_p_,subpiece_info)
			);	

		if(!b_block_full) 
			return;

		if(file_resource_desc_p_->IsFullFile())
		{
			if(TryRenameToNormalFile())
			{
				MainThread::IOS().post(
					boost::bind( &Instance::OnDiskFileNameChange,instance_p_,file_name_)
					);	
			}
			need_saveinfo_to_disk_ = false;
		}
	}
	


	//void Resource::ThreadWriting() 
	//{
	//	if(is_running_==false)
	//	{
	//		return;
	//	}
	//	int count = 40;
	//	for(int i = 0; i<count; i++)
	//	{
	//		//先取一个Buffer
	//		Buffer buf; 
	//		SubPieceInfo subpiece_info;
	//		{
	//			lightweight_mutex::scoped_lock lock(instance_p_->criticalsection_);
	//			buf = instance_p_->pending_subpiece_manager_p_->MoveToOutPool(subpiece_info);
	//			if(buf.length_==0)
	//			{
	//				break;
	//			}
	//		}
	//		//写入
	//		assert(!file_resource_desc_p_->HasSubPiece(subpiece_info));
	//		size_t startoffset,length;
	//		file_resource_desc_p_->GetSubPiecePosition(subpiece_info,startoffset,length);

	//		WriteBuffer(startoffset,buf);

	//		bool b_block_full = false;

	//		{//"{}"为lightweight_mutex::scoped_lock lock使用，不要删
	//			lightweight_mutex::scoped_lock lock(instance_p_->criticalsection_);
	//			file_resource_desc_p_->AddSubPieceInfo(subpiece_info,b_block_full);
	//			instance_p_->pending_subpiece_manager_p_->Remove(subpiece_info);
	//		}

	//		if(!b_block_full) 	continue;

	//		if(file_resource_desc_p_->IsFullFile())
	//		{
	//			lightweight_mutex::scoped_lock lock(instance_p_->criticalsection_);
	//			TryRenameToNormalFile();
	//			need_saveinfo_to_disk_ = false;
	//			MainThread::IOS().post(
	//				boost::bind( &Storage::SaveResourceInfoToDisk,Storage::Inst_Storage())
	//				);	
	//		}
	//	}
	//	lightweight_mutex::scoped_lock lock(instance_p_->criticalsection_);
	//	instance_p_->pending_subpiece_manager_p_->SetPostFlag();
	//}

	// 改名并通知instance、删除某个block并通知instance、保存资源信息
	void Resource::RemoveBlock(size_t index)
	{
		if(is_running_==false)
		{
			return;
		}
		size_t offset = 0;
		u_int length = 0;
		if(TryRenameToTppFile())
		{
			// 通知instance改名成功，将资源信息写入资源信息文件
			MainThread::IOS().post(
				boost::bind( &Instance::OnDiskFileNameChange,instance_p_,file_name_)
				);	
		}

		file_resource_desc_p_->RemoveBlockInfo(index);
		file_resource_desc_p_->GetBlockPosition(index,offset,length);

		need_saveinfo_to_disk_ = true;

		Erase(offset,length);
		// 从资源描述, pending_subpiece_manager和pending_get_subpiece_manager中删除block
		// 通知upload_listener获取subpiece失败，通知download_driver，makeblock失败
		MainThread::IOS().post(
			boost::bind( &Instance::OnRemoveResourceBlockFinish,instance_p_,index)	
			);
		SecSaveResourceFileInfo();
	}

}


