/*******************************************************************************
*   Instance.cpp
*******************************************************************************/

#include "stdafx.h"
#include "base/base.h"
#include "p2sp/MetaFLVParser.h"
#include "p2sp/MetaMP4Parser.h"
#include "storage_base.h"
#include "base\BlockMap.h"

#include "storage/IStorage.h"
#include "storage/ResourceDescriptor.h"
#include "storage/PendingSubpieceManager.h"
#include "PendingGetSubPieceManager.h"
#include "storage/resource.h"
#include "storage/Instance.h"
#include "storage/Storage.h"
#include "framework\network\uri.h"
#include "storage/MemResource.h"
#include "storage/FileResource.h"
#include "storage/StorageThread.h"
#include "p2sp/AppModule.h"
#include <algorithm>


namespace storage
{
	Instance::Instance()
		:is_running_(true),b_pure_download_mode_(false),instance_state_(INSTANCE_NEED_RESOURCE),
		add_repeat_subiece_count_(0),disk_file_size_(0), dd_num_(0), flag_rid_origin_(base::Rid_From::RID_BY_URL)
	{
		delete_tc_.Sync();
		STORAGE_DEBUG_LOG("sucess!");
		pending_subpiece_manager_p_ = PendingSubPieceManager::Create();
		pending_get_subpiece_req_manager_p_ = PendingGetSubPieceManager::Create();
		last_push_time_ = 0;
		traffic_tc_.Sync();
	}

	// 创建一个拥有该url的instance
	Instance::p Instance::Create(UrlInfo url_info)
	{
		STORAGE_DEBUG_LOG("Instance::Create "<<url_info);
        if (url_info.url_.empty()) STORAGE_DEBUG_LOG("********************************************");

		Instance::p pointer;

		//url检查！怎么检查！
//		assert(url_info.url_.rfind(TEXT(".flv"))!=string::npos);

		pointer = p(new Instance());
		assert(pointer);
		pointer->origanel_url_info_ = url_info;
		pointer->url_info_s_.insert(url_info);
		return pointer;
	}

	// 根据FileResourceInfo和Resource_p创建一个新的instance
	Instance::p Instance::Open(FileResourceInfo r_f,Resource::p resource_p)
	{
		assert(resource_p);
		STORAGE_EVENT_LOG("函数开始");
		Instance::p pointer ;
		STORAGE_DEBUG_LOG("Instance::Create"<<r_f.url_info_<<r_f.rid_info_);
		//解析ResourceInfo
		pointer = Instance::p(new Instance());
		assert(pointer);
		pointer->url_info_s_.insert(r_f.url_info_.begin(),r_f.url_info_.end());
		pointer->last_push_time_ = r_f.last_push_time_;
		pointer->traffic_list_.assign(r_f.traffic_list_.begin(),r_f.traffic_list_.end());

		pointer->memory_resource_desc_p_ = resource_p->CloneResourceDescriptor();
		assert(pointer->memory_resource_desc_p_);

		pointer->memory_resource_desc_p_->InitRidInfo(r_f.rid_info_);
		// 重新分配block_verify_map的大小，并以0(NO_VERIFIED)填充
		pointer->block_verify_map_.resize(r_f.rid_info_.block_count_,NO_VERIFIED);

		pointer->instance_state_ = INSTANCE_APPLY_RESOURCE;
		pointer->AttachResource(resource_p);
		return pointer;
	}

	// 根据file_length创建文件资源
	void Instance::SetFileLength(size_t file_length)
	{
		if( is_running_ == false ) return;
		STORAGE_DEBUG_LOG("Instance::SetFileLength"<<file_length);

		if(file_length==0)
		{
			return;
		}
		RidInfo rid_info;
		rid_info.InitByFileLength(file_length);

		// 如果文件描述符为空，则创建文件资源
		bool result = SetRidInfo(rid_info, MD5(), 0);
		assert(result);
	}

	void Instance::Start()
	{
		cfg_timer_ = framework::timer::PeriodicTimer::create(1000*10, shared_from_this());
		cfg_timer_->Start();
		traffic_timer_ = framework::timer::PeriodicTimer::create(1*1000, shared_from_this());
		traffic_timer_->Start();
		is_running_ = true;

		//content_kb_ = 10;
		content_bytes_ = 0;
		if (IsComplete())
			content_need_to_add_ = content_need_to_query_ = false;
		else
			content_need_to_add_ = content_need_to_query_ = true;
	}

	bool Instance::IsRunning()
	{
		return is_running_;
	}

	void Instance::Stop()
	{
		if( is_running_ == false ) return;
		is_running_ = false;

		assert(download_driver_s_.empty());
		STORAGE_DEBUG_LOG("Instance::Stop! ");
		instance_state_ = INSTANCE_CLOSING;

		if(cfg_timer_)
		{
			cfg_timer_->Stop();
		}

		if(merge_timer_)
		{
			merge_timer_->Stop();
		}
		if(resource_p_)
		{
			// 保存信息到cfg文件，关闭资源文件
			StorageThread::IOS().post(
				boost::bind( &Resource::CloseResource,resource_p_)
				);
			return;
		}
		OnResourceCloseFinish(resource_p_);
	}

	// 关闭本instance并通知storage，释放空间
	void Instance::Remove()
	{
		if( is_running_ == false ) return;
		is_running_ = false;
		STORAGE_DEBUG_LOG("Instance::Remove! ");

		instance_state_ = INSTANCE_REMOVING;
		// download_driver应该已经迁移过
		assert(download_driver_s_.empty());

		cfg_timer_->Stop();
		if(merge_timer_)
		{
			merge_timer_->Stop();
		}

		if(resource_p_)
		{
			// 先关闭resource，然后调用OnResourceCloseFinish，但是如果此时instance已经销毁，则
			// 通知spacemanager释放资源空间
			StorageThread::IOS().post(
				boost::bind( &Resource::CloseResource,resource_p_)
				);
			return;
			//StorageThread::IOS().post(
			//	boost::bind( &Resource::RemoveResource,resource_p_)
			//	);
			//return;
		}
		// resource已被关闭，则直接调用OnResourceCloseFinish，通知storage取消instance的资源申请
		OnResourceCloseFinish(resource_p_);
	}

	// 通知storage关闭instance，释放资源空间
	void Instance::OnResourceCloseFinish(Resource::p resource_p)
	{
		STORAGE_DEBUG_LOG("Instance::OnResourceCloseFinish! ");
		assert(resource_p_==resource_p);
		pending_subpiece_manager_p_=PendingSubPieceManager::p();

		resource_p_ = Resource::p();
		url_info_s_.clear();

		MainThread::IOS().post(
			boost::bind( &Storage::OnInstanceCloseFinish,Storage::Inst_Storage(),shared_from_this(),resource_p)
			);
	}

	// 文件是否完整(是否下载完毕)
	bool Instance::IsComplete()
	{
		if( is_running_ == false ) return false;

		if(!memory_resource_desc_p_) // 文件描述为空
		{
			STORAGE_DEBUG_LOG("no");
			return false;
		}
		//if(memory_resource_desc_p_->rid_info_.rid_.IsEmpty()) // rid为空
		//{
		//	STORAGE_DEBUG_LOG(" no");
		//	return false;
		//}
		if(memory_resource_desc_p_->IsFullFile()) // 文件完整(下载完毕)
		{
			STORAGE_DEBUG_LOG("yes");
			return true;
		}
		STORAGE_DEBUG_LOG("no");
		return false;
	}

	// ms什么都没做
	void  Instance::Merge(Instance::p inst2)
	{
		if( is_running_ == false ) return;
		assert(inst2);
		STORAGE_DEBUG_LOG("Instance::Merge!inst1:"<<memory_resource_desc_p_->rid_info_.rid_<<" inst2:"<<inst2->GetRID());
	}

	// 将本instance合并到new_instance中，并通知download_driver，然后删除本instance
	void  Instance::BeMergedTo(Instance::p new_instance)
	{
		if( is_running_ == false ) return;
		merge_to_instance_p_ = new_instance;
		merge_timer_ = framework::timer::OnceTimer::create(250,shared_from_this());
		set<IDownloadDriver::p>::const_iterator iter = download_driver_s_.begin();
		//如何缓慢发送已下载资源！
		assert(!new_instance->GetRID().IsEmpty());

		while(iter!=download_driver_s_.end())
		{
            STORAGE_DEBUG_LOG("Instance::BeMergedTo Notify!! IDownloadDriver::OnNoticeChangeResource");
			MainThread::IOS().post(
				boost::bind( &IDownloadDriver::OnNoticeChangeResource, *iter,shared_from_this(),new_instance)
			);
			iter++;
		}
		instance_state_ = INSTANCE_BEING_MERGED;
		merging_pos_subpiece.block_index_ = 0;
		merging_pos_subpiece.subpiece_index_ = 0;

		if(!memory_resource_desc_p_)
		{
			//还未曾获取过长度，memory_resource_desc_p_未被初始化！
			// 直接删除本instance，merge完成
			MainThread::IOS().post(
				boost::bind( &Storage::RemoveInstanceForMerge,Storage::Inst_Storage(),shared_from_this())
				);
			STORAGE_DEBUG_LOG(" merging finish!");
			return;
		}

		MergeResourceTo();
	}

	// 合并，然后通知Storage删除本instance
	void  Instance::MergeResourceTo()
	{
		//merge处理！
		assert(instance_state_==INSTANCE_BEING_MERGED);
		bool merge_finish = false;

		size_t merging_count = 0;
		while(merging_count<2048)
		{
			// 如果当前资源中有该subpiece而merge目的资源中没有，则mergesubpiece
			if(HasSubPiece(merging_pos_subpiece)&&(!merge_to_instance_p_->HasSubPiece(merging_pos_subpiece)))
			{
				MergeSubPiece(merging_pos_subpiece,merge_to_instance_p_);
				merging_count++;
			}
			// subpiece++
			if(memory_resource_desc_p_->IncSubPieceInfo(merging_pos_subpiece))
			{
				merge_finish = true;
				break;
			}
		}

		STORAGE_ERR_LOG(" merging_count"<<merging_count);
		if((!merge_finish)||(!download_driver_s_.empty()))
		{
			merge_timer_->Start();
			return;
		}

		//merge完成
		MainThread::IOS().post(
			boost::bind( &Storage::RemoveInstanceForMerge,Storage::Inst_Storage(),shared_from_this())
			);
		STORAGE_ERR_LOG(" merging finish!"<<merging_count);
		return;
	}


	// ---------------------------------------------------------------
	//  一系列获取信息的Get函数
	RID Instance::GetRID()
	{
		if( is_running_ == false ) return RID();
		if(!memory_resource_desc_p_)
		{
			 return RID();
		}
		return memory_resource_desc_p_->rid_info_.rid_;
	}

	void Instance::GetRidInfo(RidInfo &rid_info)
	{
		if( is_running_ == false ) return;

		if(!memory_resource_desc_p_)
		{
			rid_info = RidInfo();
			return;
		}
		rid_info = memory_resource_desc_p_->rid_info_;
	}

	size_t Instance::GetFileLength()
	{
		if(is_running_==false)	return 0;
		return GetResourceLength();
	}

	size_t Instance::GetResourceLength()
	{
		if(!memory_resource_desc_p_)
		{
			return 0;
		}
		return memory_resource_desc_p_->rid_info_.file_length_;
	}
	size_t Instance::GetDiskFileSize()
	{
		return disk_file_size_;
	}
	size_t Instance::GetDownloadBytes()
	{
		if(!memory_resource_desc_p_)
		{
			return 0;
		}
		return memory_resource_desc_p_->GetDownloadBytes();
	}

	size_t Instance::GetBlockSize()
	{
		if( is_running_ == false ) return 0;
//		STORAGE_DEBUG_LOG(" ");
		if(!memory_resource_desc_p_)
		{
			return 0; //怪异！
		}
		return memory_resource_desc_p_->rid_info_.block_size_;
	}

	size_t Instance::GetBlockCount()
	{
		if( is_running_ == false ) return 0;
		STORAGE_DEBUG_LOG(" ");
		assert(memory_resource_desc_p_);
		return memory_resource_desc_p_->rid_info_.block_count_;
	}

	void Instance::GetUrls(set<UrlInfo>& url_s)
	{
		if( is_running_ == false ) return;
		url_s.clear();
		url_s.insert(url_info_s_.begin(),url_info_s_.end());
		if (!url_s.empty()) STORAGE_DEBUG_LOG(" "<<*(url_s.begin())<<" url_s.size"<<url_s.size());
	}

	void Instance::GetUrls(vector<UrlInfo>& url_s)
	{
		if( is_running_ == false ) return;
		url_s.assign(url_info_s_.begin(),url_info_s_.end());

		STORAGE_DEBUG_LOG(" "<<" url_s.size"<<url_s.size());
	}

    UrlInfo Instance::GetOriginalUrl()
    {
        if (is_running_ == false) return UrlInfo();
        return origanel_url_info_;
    }

	// 如果资源描述为空(正常情况)，则根据rid_info创建资源描述符，进而创建文件资源
	bool Instance::SetRidInfo(const RidInfo& rid_info, MD5 content_md5, size_t content_bytes)
	{
		if( is_running_ == false ) return false;
		while(true)
		{
			if(!memory_resource_desc_p_) // 资源描述符为空
			{
				memory_resource_desc_p_ = ResourceDescriptor::Create(rid_info,false);
				block_verify_map_.resize(rid_info.block_count_,NO_VERIFIED);
				assert(memory_resource_desc_p_);
				break;
			}
			// ------------------------------------------------------------------------------
			// 已经有资源描述符
			if(b_pure_download_mode_)
			{
				STORAGE_EVENT_LOG("Instance::SetRidInfo block hash 存下模式，返回!"<<rid_info<<" memory_resource_desc_p_->rid_info_"<<memory_resource_desc_p_->rid_info_);
				return false;
			}

			if(!memory_resource_desc_p_->rid_info_.rid_.IsEmpty())
			{
				STORAGE_EVENT_LOG("Instance::SetRidInfo RID已存在，返回!"<<rid_info<<" memory_resource_desc_p_->rid_info_"<<memory_resource_desc_p_->rid_info_);
				return false;
			}

			if(memory_resource_desc_p_->rid_info_.file_length_!=rid_info.file_length_)
			{
				STORAGE_EVENT_LOG("Instance::SetRidInfo file_length_不同，返回！"<<rid_info<<" memory_resource_desc_p_->rid_info_"<<memory_resource_desc_p_->rid_info_);
				return false;
			}

			for(u_int i = 0;i<memory_resource_desc_p_->rid_info_.block_count_;i++)
			{
				if(!memory_resource_desc_p_->rid_info_.block_md5_s_[i].IsEmpty())
				{
					if(memory_resource_desc_p_->rid_info_.block_md5_s_[i]!=rid_info.block_md5_s_[i])
					{
						STORAGE_EVENT_LOG("Instance::SetRidInfo block hash 不同，返回！"<<rid_info<<" memory_resource_desc_p_->GetRidInfo()"<<memory_resource_desc_p_->rid_info_);
						//已下载block hash 不符，返回
						return false;
					}
				}
			}
			memory_resource_desc_p_->InitRidInfo( rid_info);
			break;
		}

		assert(rid_info.block_count_ == memory_resource_desc_p_->rid_info_.block_count_);
		assert(rid_info.block_size_ == memory_resource_desc_p_->rid_info_.block_size_);

		// 如果content_md5为空，则赋值
		if (content_bytes != 0)
		{
			assert(content_md5.IsEmpty()==false);
			content_need_to_add_ = false;
			if (true == content_md5_.IsEmpty())
			{
				content_md5_ = content_md5;
				content_bytes_ = content_bytes;
			}
		}
		// 告诉download_driver, rid改变
		set<IDownloadDriver::p>::const_iterator iter = download_driver_s_.begin();
		if(!memory_resource_desc_p_->rid_info_.rid_.IsEmpty())
		{
			content_need_to_query_ = false;

			while(iter!=download_driver_s_.end())
			{
				MainThread::IOS().post(
					boost::bind( &IDownloadDriver::OnNoticeRIDChange, *iter)
					);
				iter++;
			}
		}
		STORAGE_EVENT_LOG(" success!"<<rid_info<<" memory_resource_desc_p_->GetRidInfo()"<<memory_resource_desc_p_->rid_info_);
		//注意

		// 创建文件资源
		if(instance_state_==INSTANCE_NEED_RESOURCE)
		{
			assert(!resource_p_);
			TryCreateResource();
		}
		return true;
	}

	// 根据url_info_s的第一个Url生成资源文件的文件名，然后向Storage申请资源
	// instance状态：NEED_RESOURCE --> APPLY_RESOURCE
	void Instance::TryCreateResource()
	{
		assert(url_info_s_.size()>0);
		STORAGE_EVENT_LOG("函数开始");
		if(instance_state_!=INSTANCE_NEED_RESOURCE)
		{
			return ;
		}
		instance_state_ = INSTANCE_APPLY_RESOURCE;
		tstring resource_name;
		// 根据Url获取文件名
		ParseFileNameFromUrl(*(url_info_s_.begin()),resource_name);

		STORAGE_DEBUG_LOG("try create resource file :"<<framework::w2b(resource_name_)<<" length:"<<memory_resource_desc_p_->rid_info_.file_length_);
		Storage::Inst_Storage()->ApplyResource(shared_from_this());

		return ;
	}

	// 添加url_info(如果已存在不做操作)，并通知download_driver
	void Instance::AddUrlInfo(const vector<UrlInfo>& url_infos)
	{
		if( is_running_ == false ) return;

		bool need_notify = false;
		for(vector<UrlInfo>::const_iterator it =  url_infos.begin(); it!=url_infos.end(); ++it)
		{
            STORAGE_DEBUG_LOG("Instance::AddUrlInfo, 添加URL: " << *it);

			bool need_insert = true;

			for(set<UrlInfo>::const_iterator list_it = url_info_s_.begin();list_it!=url_info_s_.end(); ++list_it)
			{
				//成批更新，是从index_server获取的信息，如果url相同，本地refer保留
				if((*list_it).url_==(*it).url_)
				{
					need_insert = false;
					break;
				}
			}

            /*
            if (url_info_s_.count(*it) != 0)
                need_insert = false;
            */

			if(need_insert)
			{
				need_notify = true;
				url_info_s_.insert(*it);
			}
		}

        STORAGE_DEBUG_LOG("Instance::AddUrlInfo, 添加URL之后：" << url_info_s_.size());

		if(!need_notify)
		{
			return;
		}


		for(set<IDownloadDriver::p>::const_iterator iter = download_driver_s_.begin();iter!=download_driver_s_.end();iter++)
		{
            STORAGE_DEBUG_LOG("Instance::AddUrlInfo Notify -> IDownloadDriver::OnNoticeUrlChange");
			MainThread::IOS().post(
				boost::bind( &IDownloadDriver::OnNoticeUrlChange, *iter)
				);
		}
	}

	// 添加url(如果已存在，替换本地refer)，并通知download_driver
	void Instance::AddUrlInfo(const UrlInfo& url_info)
	{
		if( is_running_ == false ) return;

		bool need_insert = true;

		for(set<UrlInfo>::iterator list_it = url_info_s_.begin();list_it!=url_info_s_.end();list_it++)
		{
			//单个增加，是从CreateInstance获取的原生url信息，如果url相同，refer将被替换！
			if((*list_it).url_==url_info.url_)
			{
				if((*list_it).refer_url_==url_info.refer_url_)
				{
					return;
				}
				(*list_it).refer_url_ = url_info.refer_url_;
				need_insert = false;
				break;
			}
		}

		if(need_insert)
		{
			url_info_s_.insert(url_info);
		}


		for(set<IDownloadDriver::p>::const_iterator iter = download_driver_s_.begin();iter!=download_driver_s_.end();iter++)
		{
			MainThread::IOS().post(
				boost::bind( &IDownloadDriver::OnNoticeUrlChange, *iter)
				);
		}
	}

	// 从url_info_s中删除某个url，并通知download_driver
	void Instance::RemoveUrl(const  string& url_str)
	{
		if( is_running_ == false ) return;
		int  count = 0; // 删除的个数
		if(url_info_s_.empty())
		{
			return;
		}
		for(set<UrlInfo>::iterator it = url_info_s_.begin();it!=url_info_s_.end();)
		{
			if(url_str==(*it).url_)
			{
				it = url_info_s_.erase(it);
				count++;
			} else
				it ++;
		}
		if(count==0) // 什么都没删
		{
			STORAGE_ERR_LOG("erase error!");
			return;
		}

		set<IDownloadDriver::p>::const_iterator iter = download_driver_s_.begin();
		while(iter!=download_driver_s_.end())
		{
			MainThread::IOS().post(
				boost::bind( &IDownloadDriver::OnNoticeUrlChange, *iter)
				);
			iter++;
		}
	}

	void Instance::SetPureDownloadMode(bool b_mode)
	{
		if( is_running_ == false ) return;
		b_pure_download_mode_ = b_mode;
		//assert(memory_resource_desc_p_->rid_info_.rid_.IsEmpty());
	}

	bool Instance::IsPureDownloadMode()
	{
		return b_pure_download_mode_;
	}

	BlockMap::p Instance::GetBlockMap()
	{
		BlockMap::p pointer = BlockMap::Create(0);
		if( is_running_ == false ) return BlockMap::p();

//         if(!memory_resource_desc_p_)
// 		{
// 			STORAGE_ERR_LOG("GetBlockMap memory_resource_desc_p_ is null");
// 			return pointer;
// 		}

		//assert(!memory_resource_desc_p_->rid_info_.rid_.IsEmpty());

        if (!resource_p_ || !(resource_p_->file_resource_desc_p_))
        {
            STORAGE_ERR_LOG("GetBlockMap memory_resource_desc_p_ is null");
            return pointer;
        }

		pointer = resource_p_->file_resource_desc_p_->GetBlockMap();

        STORAGE_DEBUG_LOG("(*block_bitmap)"<<(*pointer));
		return pointer;//memory_resource_desc_p_->GetBlockMap();
	}

	// 将subpiece添加到cache
	void Instance::AsyncAddSubPiece(const SubPieceInfo& subpiece_info, const Buffer& buffer)
	{
		if( is_running_ == false ) return;
		//STORAGE_DEBUG_LOG("AsyncAddSubPiece "<<subpiece_info);
		//assert(memory_resource_desc_p_);
		if(!memory_resource_desc_p_)
		{
			STORAGE_ERR_LOG("memory_resource_desc_p_ is null!");
			return;
		}

		//如果不是未验证状态，就是已验证、正在验证状态
		if(block_verify_map_[subpiece_info.block_index_]!=NO_VERIFIED)
		{
			STORAGE_ERR_LOG("该BLOCK处于已验证或正在验证状态"<<subpiece_info);
			add_repeat_subiece_count_++;
			return;
		}

		// 重复的直接丢弃
		if(memory_resource_desc_p_->HasSubPiece(subpiece_info))
		{
//			STORAGE_ERR_LOG("Resource::AsyncAddSubPiece:the subpiece is existing(pending!)!"<<subpiece_info);
			add_repeat_subiece_count_++;
			return ;
		}

		pending_subpiece_manager_p_->PushToInPool(subpiece_info,buffer);
		bool block_full =false;
		memory_resource_desc_p_->AddSubPieceInfo(subpiece_info,block_full);
		//STORAGE_TEST_DEBUG("Add subpiece:"<<subpiece_info<<"  buf len:"<<buffer.length_);
		//if (subpiece_info.block_index_ == 0 && subpiece_info.subpiece_index_ < content_kb_)
		//{
		//	content_buf_.insert(make_pair(subpiece_info,buffer));
		//}

		//// 下载了10K (注：2048 = rid_info.block_size/1024，下面的表达式以1K为单位)
		//if (subpiece_info.block_index_ * 2048 + subpiece_info.subpiece_index_ == content_kb_)
		//{
		//	STORAGE_TEST_DEBUG(subpiece_info);
		//	DoMakeContentMd5AndQuery();
		//}

        //if(block_full) // 一个block满了，启动写入
		//{
		//	block_verify_map_[subpiece_info.block_index_] = BEING_VERIFIED;
		//	if(resource_p_)
		//	{
		//		std::map<SubPieceInfo,Buffer> pending_buffer_set;
		//		pending_subpiece_manager_p_->MoveBlockBufferToOutPool(subpiece_info.block_index_,pending_buffer_set);
		//		STORAGE_DEBUG_LOG("will post to ThreadPendingHashBlock:"<<subpiece_info.block_index_<<" pending buffer count:"<<pending_buffer_set.size());
		//		// 将block写入文件，然后检查上传等操作
		//		StorageThread::IOS().post(
		//			boost::bind( &Resource::ThreadPendingHashBlock,resource_p_, subpiece_info.block_index_,pending_buffer_set)
		//			);
		//	}
		//	else
		//	{
		//		// 通过subpiece计算block的MD5值，然后检查上传等操作
		//		PendingHashBlock(subpiece_info.block_index_);
		//	}
		//	return;
		//}
		//// 如果block没有满，但是poolin的队列过长，启动写入，写入会将poolin所有subpiece搬到poolout中，
		//// 然后写入文件，最后清除poolout
		//if(pending_subpiece_manager_p_->NeedPost())
		//{
		//	WriteToResource();
		//}
	}

	// 将content写入文件，并通知download_driver
	bool Instance::DoMakeContentMd5AndQuery(Buffer content_buffer)
	{
		STORAGE_DEBUG_LOG("DoMakeContentMd5AndQuery");
		if (!memory_resource_desc_p_)
		{
			STORAGE_DEBUG_LOG("!memory_resource_desc_p_");
			return false;
		}

		//for (int i = 0; i < content_kb_; i ++)
		//	if (memory_resource_desc_p_->HasSubPiece(SubPieceInfo(0, i)) == false)
		//	{
		//		STORAGE_DEBUG_LOG("memory_resource_desc_p_->HasSubPiece(SubPieceInfo(0, i)) == false");
		//		return false;
		//	}

//		if (memory_resource_desc_p_->rid_info_.GetRID().IsEmpty())// && content_bytes_ == 0)
		//if (content_md5_.IsEmpty())// && content_bytes_ == 0)
        STORAGE_DEBUG_LOG(" content_md5_=" << content_md5_ << " content_buffer_=" << content_buffer_.length_ << " buf_=" << content_buffer.length_);
        //if (content_md5_.IsEmpty())
		{
            content_buffer_ = content_buffer;
			PendingHashContent();
			return true;
		}
	}

	// 将poolin中的所有subpiece移到poolout中，并由Storage线程写入文件
	// 待写入完成后交由MainThread从poolout中删除该subpiece
	void Instance::WriteToResource()
	{
		if(!resource_p_)
		{
			return;
		}
		while(true)
		{
			SubPieceInfo subpiece_info;
			Buffer buf;

			if(!pending_subpiece_manager_p_->MoveToOutPool(subpiece_info,buf))
			{
				break;
			}
			StorageThread::IOS().post(
				boost::bind( &Resource::ThreadSecWriteSubPiece, resource_p_,subpiece_info,buf)
				);
		}
	}

	// 将poolin中的某个block迁移到poolout中，然后由Storage线程写入文件
	// 待写入完成后交由MainThread从poolout中删除
	void Instance::WriteBlockToResource(size_t block_index)
	{
        if (!memory_resource_desc_p_)
        {
            return;
        }
        STORAGE_EVENT_LOG("函数开始");
        if (false == memory_resource_desc_p_->HasFullBlock(block_index))
		{
			std::map<SubPieceInfo,Buffer> pending_buffer_set;
			pending_subpiece_manager_p_->MoveBlockBufferToOutPool(block_index, pending_buffer_set);
			STORAGE_DEBUG_LOG("will post to ThreadWriteBlock:"<<block_index<<" pending buffer count:"<<pending_buffer_set.size());
			// 将block写入文件，不做校验
			StorageThread::IOS().post(
				boost::bind( &Resource::ThreadWriteBlock,resource_p_, block_index,pending_buffer_set)
				);
			// 保存资源信息到cfg文件中
			StorageThread::IOS().post(
				boost::bind(&Resource::SecSaveResourceFileInfo,resource_p_)
				);
			return;
		}
		block_verify_map_[block_index] = BEING_VERIFIED;
		if(resource_p_)
		{
			std::map<SubPieceInfo,Buffer> pending_buffer_set;
			pending_subpiece_manager_p_->MoveBlockBufferToOutPool(block_index, pending_buffer_set);
			STORAGE_DEBUG_LOG("will post to ThreadPendingHashBlock:"<<block_index<<" pending buffer count:"<<pending_buffer_set.size());

			// 将block写入文件，然后检查上传等操作
			StorageThread::IOS().post(
				boost::bind( &Resource::ThreadPendingHashBlock,resource_p_, block_index,pending_buffer_set)
				);
			// 保存资源信息到cfg文件中
			StorageThread::IOS().post(
				boost::bind(&Resource::SecSaveResourceFileInfo,resource_p_)
				);
		}
		else
		{
			// 通过subpiece计算block的MD5值，然后检查上传等操作
			PendingHashBlock(block_index);
		}
		return;
	}

	// 从poolout中删除某个subpiece
	void Instance::OnWriteSubPieceFinish(SubPieceInfo subpiece_info)
	{
//		STORAGE_DEBUG_LOG(""<<GetRID()<<subpiece_info);
		pending_subpiece_manager_p_->Remove(subpiece_info);
	}

	// 通过subpiece计算block的MD5值，然后检查上传等操作
	void Instance::PendingHashBlock(u_int block_index)
	{
		size_t offset = 0;
		u_int  length = 0;
		u_int subpiece_num = memory_resource_desc_p_->GetBlockSubPieceCount(block_index);
		SubPieceInfo subpiece_info;
		subpiece_info.block_index_ = block_index;
		map<SubPieceInfo,Buffer>::const_iterator it;
		CMD5::p hash = CMD5::Create();
		for(u_int i = 0;i<subpiece_num;i++)
		{
			subpiece_info.subpiece_index_ =i;
			Buffer buf = pending_subpiece_manager_p_->Get(subpiece_info);
			assert(buf.length_!=0);
			hash->Add(buf.data_.get(),buf.length_);
		}

		hash->Finish();
		SMD5 smd5_val;
		hash->GetHash(&smd5_val);
		MD5 hash_val = *(MD5*)(&smd5_val);
		// block写入完毕，赋予MD5值，检查文件是否写入完毕，完毕则通知其他模块下载完毕
		// 读取需要上传的block，上传
		OnPendingHashBlockFinish(block_index,hash_val);
	}

	// 通过subpiece计算content的MD5值
	void Instance::PendingHashContent()
	{
		STORAGE_DEBUG_LOG("PendingHashContent");
	
		CMD5::p hash = CMD5::Create();
        hash->Add(content_buffer_.data_.get(), content_buffer_.length_);
		hash->Finish();
		SMD5 smd5_val;
		hash->GetHash(&smd5_val);
		MD5 hash_val = *(MD5*)(&smd5_val);
		OnPendingHashContentFinish(hash_val, content_buffer_.length_);
	}

	void Instance::OnHashBlockFinish(size_t block_index,MD5 hash_val, Buffer& buf, IUploadListener::p listener)
	{
		assert(block_verify_map_[block_index]==BEING_VERIFIED);
		STORAGE_DEBUG_LOG(" block_index:"<<block_index<<"md5:"<<hash_val);

		if(memory_resource_desc_p_->rid_info_.block_md5_s_[block_index].IsEmpty())
		{
			memory_resource_desc_p_->rid_info_.block_md5_s_[block_index] = hash_val;
		}

		if(memory_resource_desc_p_->rid_info_.block_md5_s_[block_index]==hash_val)
		{
			block_verify_map_[block_index] = HAS_BEEN_VERIFIED;
			MainThread::IOS().post(
				boost::bind(&IUploadListener::OnAsyncGetBlockSucced,listener,GetRID(),block_index,buf)
				);
			STORAGE_TEST_DEBUG("Hash ok and Upload------>block index:"<<block_index);
			return;
		}
		// MD5校验失败--------------------------------
		block_verify_map_[block_index] = NO_VERIFIED;
		MainThread::IOS().post(
			boost::bind( &IUploadListener::OnAsyncGetBlockFailed, listener,GetRID(),
			block_index, ERROR_GET_SUBPIECE_BLOCK_VERIFY_FAILED)
			);
		STORAGE_TEST_DEBUG("Hash failed----->block index:"<<block_index);
	}

	// block写入完毕，赋予MD5值，检查文件是否写入完毕，完毕则通知其他模块下载完毕
	// 读取需要上传的block，上传
	void Instance::OnPendingHashBlockFinish(size_t block_index,MD5 hash_val)
	{
		assert(block_verify_map_[block_index]==BEING_VERIFIED);

		STORAGE_DEBUG_LOG(" block_index:"<<block_index<<"md5:"<<hash_val);

		if(memory_resource_desc_p_->rid_info_.block_md5_s_[block_index].IsEmpty())
		{
			memory_resource_desc_p_->rid_info_.block_md5_s_[block_index] = hash_val;
		}

		if(memory_resource_desc_p_->rid_info_.block_md5_s_[block_index]==hash_val)
		{
			block_verify_map_[block_index] = HAS_BEEN_VERIFIED;
			// 告诉download_driver，hash成功
			OnNotifyHashBlock(block_index,true);
			// 计算RID
			if(memory_resource_desc_p_->GenerateRid())
			{
				// 通知其他模块下载完毕
				OnHashResourceFinish();
				content_need_to_add_ = false;
			}
			else if (false == memory_resource_desc_p_->rid_info_.rid_.IsEmpty() && content_need_to_add_ && content_md5_.IsEmpty() == false)
			{
				if (url_info_s_.size() > 0)
                {
				    // 通知appmodule发AddRidUrlRequestPacket包
				    MainThread::IOS().post(
					    boost::bind( &p2sp::AppModule::DoAddUrlRid,p2sp::AppModule::Inst(),*(url_info_s_.begin()),
					    memory_resource_desc_p_->rid_info_, content_md5_, content_bytes_, GetRidOriginFlag())
					    );
                }
				content_need_to_add_ = false;
			}
			PendingGetSubPiece req;
			// 从pending_get_subpiece_manager中读取某个block的subpiece
			while(pending_get_subpiece_req_manager_p_->PopReq(req,block_index))
			{
				// 如果某个subpiece所在block已被鉴定，则将该subpiece交给upload_driver
				// 否则，计算该block的MD5值，然后再调用本函数
				AsyncGetSubPiece(req.subpiece_info_,req.end_point_,req.packet_,req.listener_);
			}
			return;
		}

		if(resource_p_)
		{
			//Hash冲突，先删除resource存储信息，
			StorageThread::IOS().post(
				boost::bind( &Resource::RemoveBlock,resource_p_,block_index)
				);
			return;
		}
		// 删除block，作用同上
		OnRemoveResourceBlockFinish(block_index);
	}

	// content写入完毕，通知download_driver
	void Instance::OnPendingHashContentFinish(MD5 hash_val, size_t content_bytes)
	{
		STORAGE_DEBUG_LOG(" OnPendingHashContentFinish:"<<"md5:"<<hash_val);
		set<IDownloadDriver::p>::const_iterator iter;

		content_md5_ = hash_val;
		content_bytes_ = content_bytes;
        content_buffer_ = Buffer();

        STORAGE_DEBUG_LOG(" IDownloadDriver::OnNoticeContentHashSucced need_query=" << content_need_to_query_);
		if (false == content_need_to_query_)
			return ;

		for(iter = download_driver_s_.begin();iter!=download_driver_s_.end();iter++)
		{
			MainThread::IOS().post(
				boost::bind( &IDownloadDriver::OnNoticeContentHashSucced, *iter, origanel_url_info_.url_, hash_val, content_bytes, GetResourceLength())
			);
		}

	}

	// 从资源描述, pending_subpiece_manager和pending_get_subpiece_manager中删除block
	// 通知upload_listener获取subpiece失败，通知download_driver，makeblock失败
	void Instance::OnRemoveResourceBlockFinish(size_t block_index)
	{
		STORAGE_DEBUG_LOG(" block_index:"<<block_index<<"rid_info: "<<memory_resource_desc_p_->rid_info_);
		memory_resource_desc_p_->RemoveBlockInfo(block_index);
		pending_subpiece_manager_p_->RemoveBlock(block_index);

		PendingGetSubPiece req;
		while(pending_get_subpiece_req_manager_p_->PopReq(req,block_index))
		{
			MainThread::IOS().post(
				boost::bind( &IUploadListener::OnAsyncGetSubPieceFailed, req.listener_,memory_resource_desc_p_->rid_info_.rid_,req.subpiece_info_,req.end_point_,ERROR_GET_SUBPIECE_BLOCK_VERIFY_FAILED,req.packet_)
				);
		}
		block_verify_map_[block_index] = NO_VERIFIED;
		OnNotifyHashBlock(block_index,false);
	}

	void Instance::WeUploadSubPiece(size_t num)
	{
//		STORAGE_TEST_DEBUG("\n\nwe upload subpiece");
		if (traffic_list_.size() == 0 && TRAFFIC_T0 >0)
		{
			traffic_list_.push_back(num);
			last_push_time_ = 0;
			return;
		}
		else
		{
			traffic_list_.back() = traffic_list_.back()+num;
		}
	}

	// 如果某个block已被鉴定，则从文件中读取，将该block交给upload_driver，否则...
	void Instance::AsyncGetBlock(size_t block_index, IUploadListener::p listener)
	{
		STORAGE_TEST_DEBUG("Upload Get Block");
		if( is_running_ == false ) return;
		if(!resource_p_->file_resource_desc_p_)
		{
			STORAGE_ERR_LOG("AsyncGetBlock: resource_p_->file_resource_desc_p_==null");
			return;
		}
		assert(!GetRID().IsEmpty());

		// block不是已被鉴定状态
		if(block_verify_map_[block_index]!=HAS_BEEN_VERIFIED)
		{
			// block不是满的
			if (false == resource_p_->file_resource_desc_p_->HasFullBlock(block_index))
			{
				MainThread::IOS().post(
					boost::bind( &IUploadListener::OnAsyncGetBlockFailed, listener,GetRID(),
					block_index,ERROR_GET_SUBPIECE_BLOCK_NOT_FULL)
					);
				return;
			}
			if(block_verify_map_[block_index]==BEING_VERIFIED)
			{
				return;
			}
			//NO_VERIFIED
			block_verify_map_[block_index] = BEING_VERIFIED;
			// block是满的，校验，然后交给UploadDriver
			StorageThread::IOS().post(
				boost::bind(&Resource::ThreadReadBlockForUpload, resource_p_, GetRID(),
				block_index, listener, true)
				);
			return;
		}
		// block已被鉴定，从文件中读取，交给UploadDriver
		StorageThread::IOS().post(
			boost::bind(&Resource::ThreadReadBlockForUpload, resource_p_, GetRID(),
			block_index, listener, false)
			);
	}

	// 如果某个subpiece所在block已被鉴定，则将该subpiece交给upload_driver
	// 否则，计算该block的MD5值，然后再调用本函数
	void Instance::AsyncGetSubPiece(const SubPieceInfo& subpiece_info, const boost::asio::ip::udp::endpoint& end_point, RequestSubPiecePacket::p packet, IUploadListener::p listener)
	{
		STORAGE_TEST_DEBUG("Upload Get Subpiece");
		if( is_running_ == false ) return;
		if(!memory_resource_desc_p_)
		{
			STORAGE_ERR_LOG("AsyncGetSubPiece:memory_resource_desc_p_==null");
			return;
		}
		assert(!GetRID().IsEmpty());
		// block不是已被鉴定状态
		if(block_verify_map_[subpiece_info.block_index_]!=HAS_BEEN_VERIFIED)
		{
			// block不是满的，通知upload_driver获取subpiece失败
			if(!memory_resource_desc_p_->HasFullBlock(subpiece_info.block_index_))
			{
				MainThread::IOS().post(
					boost::bind( &IUploadListener::OnAsyncGetSubPieceFailed, listener,memory_resource_desc_p_->rid_info_.rid_,subpiece_info,end_point,ERROR_GET_SUBPIECE_BLOCK_NOT_FULL, packet)
					);
				return;
			}

			// 先把subpiece存放到pending_get_subpiece_manager，待block鉴定完成后读取再给upload_driver
			PendingGetSubPiece req(subpiece_info,end_point,packet,listener);
			pending_get_subpiece_req_manager_p_->PushReq(req);

			if(block_verify_map_[subpiece_info.block_index_]==BEING_VERIFIED)
			{
				return;
			}

			//NO_VERIFIED
			block_verify_map_[subpiece_info.block_index_] = BEING_VERIFIED;
			if(resource_p_)
			{
				std::map<SubPieceInfo,Buffer> pending_buffer_set;
				pending_subpiece_manager_p_->MoveBlockBufferToOutPool(subpiece_info.block_index_,pending_buffer_set);
				// 将一个block的subpiece写入文件，并计算该block的MD5值，然后再尝试调用本函数
				StorageThread::IOS().post(
					boost::bind( &Resource::ThreadPendingHashBlock,resource_p_, subpiece_info.block_index_,pending_buffer_set)
					);
			}
			else
			{
				 //通过subpiece计算block的MD5值，然后通知download_driver
				PendingHashBlock(subpiece_info.block_index_);
			}
			return;
		}
		// ------------------------------------------------------------------------
		// block已被鉴定，从pending_subpiece_manager或文件中读取subpiece，交给upload_listener
		assert(memory_resource_desc_p_->HasFullBlock(subpiece_info.block_index_));

		STORAGE_TEST_DEBUG("Upload Hit Rate: "<<hit_rate_.GetHitRate()<<"%");
		Buffer buf = pending_subpiece_manager_p_->Get(subpiece_info);
		if(buf.length_==0)
		{
			++upload_hit_rate_.from_disk;
			if(resource_p_)
			{
				// 从文件中读取指定的subpiece，然后交给IUploadListener
				StorageThread::IOS().post(
					boost::bind( &Resource::ThreadSecReadSubPiece, resource_p_,GetRID(),subpiece_info,end_point,packet,listener)
					);
				STORAGE_TEST_DEBUG("Storage Upload: \n>FileName: "<<resource_p_->GetLocalFileName()<<"\n>"<<subpiece_info);
				return;
			}
			assert(0);
		}
		assert((buf.length_<=bytes_num_per_piece_g_)&&(buf.length_>0));
		++upload_hit_rate_.from_mem;
		MainThread::IOS().post(
			boost::bind( &IUploadListener::OnAsyncGetSubPieceSucced, listener,memory_resource_desc_p_->rid_info_.rid_,subpiece_info,end_point,buf, packet)
			);
		STORAGE_TEST_DEBUG("Storage Upload: \n>FileName: "<<resource_p_->GetLocalFileName()<<"\n>"<<subpiece_info);
		return;
	}

	// 从pending_subpiece_manager中或文件中找到某个subpiece，然后merge到merge_to_instance_p
	void Instance::MergeSubPiece(const SubPieceInfo& subpiece_info,Instance::p merge_to_instance_p)
	{
		if(!memory_resource_desc_p_)
		{
			return;
		}

		assert(memory_resource_desc_p_->HasSubPiece(subpiece_info));

		Buffer ret_buf = pending_subpiece_manager_p_->Get(subpiece_info);

		if(ret_buf.length_!=0)
		{
			//			STORAGE_DEBUG_LOG("success! pending return:"<<subpiece_info);
			MainThread::IOS().post(
				boost::bind( &Instance::OnMergeSubPieceSuccess,merge_to_instance_p,subpiece_info,ret_buf)
				);
			STORAGE_DEBUG_LOG(" merge from pending_subpiece_manager_p_"<<subpiece_info);
			return;
		}

		if(resource_p_)
		{
			// 从文件中读取指定的subpiece，然后merge
			StorageThread::IOS().post(
				boost::bind( &Resource::ThreadMergeSubPieceToInstance, resource_p_,subpiece_info,merge_to_instance_p)
				);
			return;
		}
		STORAGE_ERR_LOG(" merge from shit!"<<subpiece_info);

	}

	// 将subpiece添加到准备写入的队列，并检查是否该写入文件，并检查block, 上传等操作, 针对merge_to_instance_p
	void Instance::OnMergeSubPieceSuccess(const SubPieceInfo& subpiece_info,Buffer buffer)
	{
		if(is_running_==false)
			return;
		STORAGE_DEBUG_LOG(""<<subpiece_info);
		AsyncAddSubPiece(subpiece_info,buffer);
	}

	// 从pending_subpiece_manager中读取某个subpiece交给player_listener
	// 如果pending_subpiece_manager中没有，则从磁盘中读取小于1M的连续数据交给player_listener
	void Instance::AsyncGetSubPieceForPlay(const SubPieceInfo& subpiece_info, IPlayerListener::p listener)
	{
		if( is_running_ == false ) return;
		if(!memory_resource_desc_p_)
		{
			STORAGE_ERR_LOG("Don't have resource length!");
			return;
		}

		size_t start_position = memory_resource_desc_p_->SubpieceInfoToPosition(subpiece_info);

		if(!memory_resource_desc_p_->HasSubPiece(subpiece_info))
		{
//			STORAGE_DEBUG_LOG("fail! !memory_resource_desc_p_->HasSubPiece(subpiece_info):"<<subpiece_info);
			MainThread::IOS().post(
				boost::bind( &IPlayerListener::OnAsyncGetSubPieceFailed, listener,start_position,ERROR_GET_SUBPIECE_NOT_FIND_SUBPIECE)
				);
			return;
		}
//		STORAGE_TEST_DEBUG("Hit Rate: "<<hit_rate_.GetHitRate()<<"%");
		Buffer ret_buf = pending_subpiece_manager_p_->GetForPlay(subpiece_info);
		if(ret_buf.length_!=0)
		{
//			++hit_rate_.from_mem;
//			STORAGE_DEBUG_LOG("success! pending return:"<<subpiece_info);
			MainThread::IOS().post(
				boost::bind( &IPlayerListener::OnAsyncGetSubPieceSucced, listener,start_position,ret_buf)
			);
			set<size_t> block_need_write = pending_subpiece_manager_p_->GetNeedWriteBlock();
			if (pending_subpiece_manager_p_->GetPlaySubPieceInfo() ==
				memory_resource_desc_p_->GetMaxSubPieceInfo())
			{
				block_need_write.insert(pending_subpiece_manager_p_->GetPlaySubPieceInfo().block_index_);
			}
			MemNeedWriteToResource(block_need_write);

			return;
		}

		if(resource_p_)
		{
//			++hit_rate_.from_disk;
			//StorageThread::IOS().post(
			//	boost::bind( &Resource::ThreadGetSubPieceForPlay, resource_p_,subpiece_info,listener)
			//	);
			StorageThread::IOS().post(
				boost::bind(&Resource::ThreadReadBufferForPlay, resource_p_, subpiece_info, 1024, listener)
				);
			return;
		}
		STORAGE_DEBUG_LOG("shit! :"<<subpiece_info);

//		assert(0);
	}

	void Instance::MemNeedWriteToResource(const set<size_t>& block_need_write)
	{
		for (set<size_t>::const_iterator it = block_need_write.begin(); it != block_need_write.end(); ++it)
		{
			WriteBlockToResource(*it);
		}
	}

	// 从pending_subpiece_manager或文件中读取某个subpiece交给player_listener
	void Instance::AsyncGetSubPieceForPlay(size_t start_position, IPlayerListener::p listener)
	{
//		STORAGE_DEBUG_LOG(" start_position:"<<start_position);
		if( is_running_ == false ) return;
		if(!memory_resource_desc_p_)
		{
			MainThread::IOS().post(
				boost::bind( &IPlayerListener::OnAsyncGetSubPieceFailed, listener,start_position,ERROR_GET_SUBPIECE_NO_FILELENGTH)
				);
			return;
		}
		assert(memory_resource_desc_p_);

		SubPieceInfo start_s_info;
		if(!memory_resource_desc_p_->PosToSubPieceInfo(start_position,start_s_info))
		{
			STORAGE_ERR_LOG("ERROR_GET_SUBPIECE_OUTOFRANGE :"<<start_position);
			MainThread::IOS().post(
				boost::bind( &IPlayerListener::OnAsyncGetSubPieceFailed, listener,start_position,ERROR_GET_SUBPIECE_OUTOFRANGE)
				);
			return;
		}
		AsyncGetSubPieceForPlay(start_s_info,listener);
	}

	// 从pending_subpiece_manager中连续读取max_count个subpicec，并通知PlayerListener
	void Instance::AsyncGetSubPieceForPlay(size_t start_position, u_int max_count, IPlayerListener::p listener)
	{
		STORAGE_DEBUG_LOG(" start_position:"<<start_position<<" max_count:"<<max_count);
		if( is_running_ == false ) return;
		assert(0);
		assert(memory_resource_desc_p_);
		SubPieceInfo start_s_info;
		if(!memory_resource_desc_p_->PosToSubPieceInfo(start_position,start_s_info))
		{
			MainThread::IOS().post(
				boost::bind( &IPlayerListener::OnAsyncGetSubPieceFailed, listener,start_position,ERROR_GET_SUBPIECE_OUTOFRANGE)
				);
			return;
		}

		//保证顺序性
		if(resource_p_)
		{
			//StorageThread::IOS().post(
			//	boost::bind( &Resource::ThreadGetSubPieceForPlay, resource_p_,start_position,max_count,listener)
			//	);
			return ;
		}

		//保证顺序性
		Buffer ret_buf;
		for(u_int i = 0; i<max_count;i++)
		{
			if(!memory_resource_desc_p_->HasSubPiece(start_s_info))
			{
				break;
			}
			ret_buf = pending_subpiece_manager_p_->GetForPlay(start_s_info);
			assert((ret_buf.length_<=bytes_num_per_piece_g_)&&(ret_buf.length_>0));
			MainThread::IOS().post(
				boost::bind( &IPlayerListener::OnAsyncGetSubPieceSucced, listener,start_position,ret_buf)
				);
			// subpiece向后加1，内含越界检查
			if(!memory_resource_desc_p_->IncSubPieceInfo(start_s_info))
			{
				break;
			}
		}
	}

	bool Instance::HasPiece(const PieceInfo& piece_info)
	{
		if( is_running_ == false ) return false;
		if(!memory_resource_desc_p_)
			return false;
		return memory_resource_desc_p_->HasPiece(piece_info);
	}

	bool Instance::HasPiece(size_t start_postion)
	{
		if( is_running_ == false ) return false;
		if(!memory_resource_desc_p_)
		{
			return false;
		}
		PieceInfo piece_info;
		if(!memory_resource_desc_p_->PosToPieceInfo(start_postion,piece_info))
		{
			return false;
		}
		return memory_resource_desc_p_->HasPiece(piece_info);
	}

	bool Instance::HasSubPiece(const SubPieceInfo& subpiece_info)
	{
		if( is_running_ == false ) return false;
		if(!memory_resource_desc_p_)
		{
			return false;
		}
		return memory_resource_desc_p_->HasSubPiece(subpiece_info);
	}

	bool Instance::GetNextPieceForDownload(const PieceInfo &start_piece_index, PieceInfoEx& piece_for_download)
	{
		if( is_running_ == false ) return false;
		assert(memory_resource_desc_p_);
		SubPieceInfo sub_subpiece_index,subpiece_for_download;
		memory_resource_desc_p_->PieceInfoToSubPieceInfo(start_piece_index,sub_subpiece_index);
		if(!memory_resource_desc_p_->GetNextNullSubPiece(sub_subpiece_index,subpiece_for_download))
		{
			STORAGE_DEBUG_LOG("Instance::GetNextPieceForDownload false!"<<" start_piece_index:"<<start_piece_index<<" piece_for_download"<<piece_for_download);
			return false;
		}
		memory_resource_desc_p_->SubPieceInfoToPieceInfoEx(subpiece_for_download,piece_for_download);
		STORAGE_DEBUG_LOG("Instance::GetNextPieceForDownload false!"<<" start_piece_index:"<<start_piece_index<<" piece_for_download"<<piece_for_download);
		return true;
	}

	bool Instance::GetNextPieceForDownload(size_t start_position, PieceInfoEx& piece_for_download)
	{
		if( is_running_ == false ) return false;
		if(!memory_resource_desc_p_)
		{
			piece_for_download.block_index_ = 0;
			piece_for_download.piece_index_ = 0;
			piece_for_download.subpiece_index_ = 0;
//			STORAGE_DEBUG_LOG("Instance::GetNextPieceForDownload true!"<<" start_position:"<<start_position<<" piece_for_download"<<piece_for_download);
			return true;
		}
		SubPieceInfo sub_subpiece_index,subpiece_for_download;
		if(!memory_resource_desc_p_->PosToSubPieceInfo(start_position,sub_subpiece_index))
		{
			STORAGE_DEBUG_LOG("Instance::GetNextPieceForDownload false!"<<" start_position:"<<start_position<<" piece_for_download"<<piece_for_download);
			return false;
		}
		if(!GetNextSubPieceForDownload(sub_subpiece_index,subpiece_for_download))
		{
			STORAGE_DEBUG_LOG("Instance::GetNextPieceForDownload false!"<<" start_position:"<<start_position<<" piece_for_download"<<piece_for_download);
			return false;
		}
		memory_resource_desc_p_->SubPieceInfoToPieceInfoEx(subpiece_for_download,piece_for_download);
//		STORAGE_DEBUG_LOG("Instance::GetNextPieceForDownload true!"<<" start_position:"<<start_position<<" piece_for_download"<<piece_for_download);
		return true;
	}


	bool Instance::GetNextSubPieceForDownload(const SubPieceInfo &sub_subpiece_index, SubPieceInfo& subpiece_for_download)
	{
		if( is_running_ == false ) return false;
		assert(memory_resource_desc_p_);
		return memory_resource_desc_p_->GetNextNullSubPiece(sub_subpiece_index,subpiece_for_download);
	}



	bool Instance::GetNextSubPieceForDownload(size_t start_position, SubPieceInfo& subpiece_for_download)
	{
		if( is_running_ == false ) return false;
		SubPieceInfo sub_subpiece_index;
		if(!memory_resource_desc_p_)
		{
			subpiece_for_download.block_index_ = 0;
			subpiece_for_download.subpiece_index_ = 0;
			STORAGE_DEBUG_LOG("Instance::GetNextSubPieceForDownload true!"<<" start_position:"<<start_position<<" subpiece_for_download"<<subpiece_for_download);
			return true;
		}

		if(!memory_resource_desc_p_->PosToSubPieceInfo(start_position,sub_subpiece_index))
		{
			STORAGE_DEBUG_LOG("Instance::GetNextSubPieceForDownload false!"<<" start_position:"<<start_position<<" subpiece_for_download"<<subpiece_for_download);
			return false;
		}
		return GetNextSubPieceForDownload(sub_subpiece_index,subpiece_for_download);
	}


	// 向instance加入某个download_driver
	void  Instance::AttachDownloadDriver(IDownloadDriver::p download_driver)
	{
		STORAGE_EVENT_LOG(" download_driver"<<download_driver);
		if( is_running_ == false ) return;
		assert(download_driver!=0);

		if(download_driver_s_.find(download_driver)==download_driver_s_.end())
		{
			download_driver_s_.insert(download_driver);
			++dd_num_;
		}
	}

	// 从instance中删掉某个download_driver，并将内存中所有数据写入文件
	void  Instance::DettachDownloadDriver(IDownloadDriver::p download_driver)
	{
		if( is_running_ == false ) return;
		//detach
		size_t dc = download_driver_s_.erase(download_driver);
		--dd_num_;
		assert(dc==1);

		if(download_driver_s_.empty())
		{
			STORAGE_DEBUG_LOG("download_driver_s_.empty");
			delete_tc_.Sync();
		}
		if (resource_p_)
		{
			if (false == pending_subpiece_manager_p_->Empty())
			{
				set<size_t> block_need_write = pending_subpiece_manager_p_->GetAllBlock();
				MemNeedWriteToResource(block_need_write);
			}
		}
	}

	// 根据url生成文件名，如果获取失败，则生成一串随机数表示文件名
	void Instance::ParseFileNameFromUrl(const UrlInfo &url_info,tstring &resource_name)
	{
		//从URL解析并确定文件名
		size_t pos = 0;
		STORAGE_DEBUG_LOG("url "<<url_info);

		string tmp_url = url_info.url_;
		if (tmp_url != origanel_url_info_.url_ && origanel_url_info_.url_.size() !=0)
		{
			tmp_url = origanel_url_info_.url_;
		}

		tstring tmp_name = Storage::Inst_Storage()->FindRealName(tmp_url);
		if (tmp_name.size() != 0)
		{
			resource_name = tmp_name;
			resource_name_ = resource_name;
			return;
		}

		framework::network::Uri uri( url_info.url_);
		resource_name = framework::b2w(uri.getfile());
		if(resource_name.size()==0)
		{
			char changestr[40];
			int changestr_size = sprintf_s(changestr,"%8x%8x%s", framework::util::Random::GetGlobal().Next(), framework::util::Random::GetGlobal().Next(),".flv");
			string change_str(changestr,changestr_size);
			resource_name_ =framework::b2w(change_str);
		}
		else
		{
			resource_name_ = resource_name;
		}

	}

	// 通知Storage做相应操作，通知appmodule发AddRidUrlRequestPacket包，通知download_driver下载完毕
	void Instance::OnHashResourceFinish()
	{
		assert(!memory_resource_desc_p_->rid_info_.rid_.IsEmpty());
		assert(url_info_s_.size()==1);

		Storage::Inst_Storage()->AddInstanceToRidMap(shared_from_this());

        if (!url_info_s_.empty())
        {
            UrlInfo url_info = *(url_info_s_.begin());

            MainThread::IOS().post(
                boost::bind( &p2sp::AppModule::DoAddUrlRid,p2sp::AppModule::Inst(),url_info,memory_resource_desc_p_->rid_info_, content_md5_, content_bytes_, flag_rid_origin_)
                );
            STORAGE_DEBUG_LOG(" post to DoAddUrlRid!"<<memory_resource_desc_p_->rid_info_);
        }

		set<IDownloadDriver::p>::const_iterator iter;

		for(iter = download_driver_s_.begin();iter!=download_driver_s_.end();iter++)
		{
			MainThread::IOS().post(
				boost::bind( &IDownloadDriver::OnNoticeDownloadComplete, *iter)
			);
		}
	}

	// 通知DownloadDriver makeblock成功或失败
	void Instance::OnNotifyHashBlock(size_t block_index,bool b_success)
	{
		if( is_running_ == false ) return;
		STORAGE_DEBUG_LOG("block_index:"<<block_index<<(b_success?"hash success!":"hash fail! ")<<memory_resource_desc_p_->rid_info_);
		set<IDownloadDriver::p>::const_iterator iter;
		if(!b_success)
		{
			for(iter = download_driver_s_.begin();iter!=download_driver_s_.end();iter++)
			{
				MainThread::IOS().post(
					boost::bind( &IDownloadDriver::OnNoticeMakeBlockFailed, *iter,block_index)
					);
			}
			return;
		}

		for(iter = download_driver_s_.begin();iter!=download_driver_s_.end();iter++)
		{
			MainThread::IOS().post(
				boost::bind( &IDownloadDriver::OnNoticeMakeBlockSucced, *iter,block_index)
				);
		}
		return;
	}

	// 将resource_p_中的instance指向本对象，并将信息写入ResourceInfo文件中(ResourceInfo.dat)
	// instance状态: APPLY_RESOURCE --> HAVE_RESOURCE
	void Instance::AttachResource(Resource::p resource_p)
	{
		assert(is_running_==true);
		assert(instance_state_==INSTANCE_APPLY_RESOURCE);
		assert(resource_p);
		assert(!resource_p->GetInstance());
		assert(memory_resource_desc_p_);

		disk_file_size_ = resource_p->GetLocalFileSize();
		resource_name_ = resource_p->GetLocalFileName();

		resource_p->BindInstance(shared_from_this());
		resource_p_ = resource_p;
		instance_state_ = INSTANCE_HAVE_RESOURCE;

		Storage::Inst_Storage()->SaveResourceInfoToDisk();
		return ;
	}

	// 只有没有download_driver且计时器大于10s时，才能删除
	bool Instance::CanRemove()
	{
		if(!resource_p_)
			return false;

		if(instance_state_!=INSTANCE_HAVE_RESOURCE)
		{
			return false;
		}

		if(disk_file_size_==0)
		{
			return false;
		}

		if(download_driver_s_.empty())
		{
			if(delete_tc_.GetElapsed()>default_delay_tickcount_for_delete)
			{
				return true;
			}
		}
		return false;
	}


	void Instance::OnTimerElapsed(framework::timer::Timer::p pointer, u_int times)
	{
		if(is_running_==false) return;
		//STORAGE_DEBUG_LOG("关键容器size"
		//	<<"download_driver_s_.size()"<<download_driver_s_.size()<<endl
		//	<<"url_info_s_.size()"<<url_info_s_.size()<<endl
		//	<<"block_verify_map_.size()"<<block_verify_map_.size()<<endl
		//	<<"memory_resource_desc_p_"<<*memory_resource_desc_p_);
		if( pointer == cfg_timer_ )
		{
			//STORAGE_DEBUG_LOG("DownloadDriver: "<<download_driver_s_.size());
			if(!resource_p_)
			{
				if(download_driver_s_.empty() && pending_subpiece_manager_p_->Empty())
				{
					if(delete_tc_.GetElapsed()>default_delay_tickcount_for_delete)
					{
						STORAGE_EVENT_LOG("delete_tc_.GetElapsed()>default_delay_tickcount_for_delete kill myslef! notice storage to remove myself!");
						MainThread::IOS().post(
							boost::bind(&Storage::RemoveInstance,Storage::Inst_Storage(),shared_from_this())
								);
					}
				}
				return;
			}
			// else if (0==dd_num_ && !memory_resource_desc_p_->IsFullFile())
			//{
			//	WriteToResource();
			//	// 保存资源信息到cfg文件中
			//	StorageThread::IOS().post(
			//		boost::bind(&Resource::SecSaveResourceFileInfo,resource_p_)
			//		);
			//}
			// 将pending_subpiece_manager中的所有subpiece写入文件
//			WriteToResource();
////			STORAGE_DEBUG_LOG("instance:"<<GetResourceName()<<" GetRepeatDownloadSubPieceCount:"<<GetRepeatDownloadSubPieceCount());
//
//			// 保存资源信息到cfg文件中
//			StorageThread::IOS().post(
//				boost::bind(&Resource::SecSaveResourceFileInfo,resource_p_)
//				);
//			Storage::Inst_Storage()->SaveResourceInfoToDisk();
			return;
		} // pointer == cfg_timer_

		if(pointer==merge_timer_)
		{
			MergeResourceTo();
			return;
		}
		if (pointer == traffic_timer_)
		{
			++last_push_time_;
			if (last_push_time_ >= TRAFFIC_UNIT_TIME)
			{
				traffic_list_.push_back(0);
				while (traffic_list_.size() > TRAFFIC_T0)
				{
					traffic_list_.pop_front();
				}
				last_push_time_ = 0;
			}
		}
	}

	tstring Instance::GetResourceName()
	{
		return resource_name_;
	}


	bool Instance::GetFileResourceInfo(FileResourceInfo &r_info)
	{
		if(instance_state_!=INSTANCE_HAVE_RESOURCE)
		{
			return false;
		}
		assert(resource_p_);

		if(disk_file_size_==0)
		{
			return false;
		}

		r_info.url_info_.assign( url_info_s_.begin(),url_info_s_.end());

		r_info.rid_info_ = memory_resource_desc_p_->rid_info_;

		r_info.file_path_ = resource_name_;

		r_info.last_push_time_ = last_push_time_;

		r_info.traffic_list_.assign(traffic_list_.begin(), traffic_list_.end());

		assert(!r_info.file_path_.empty());
		return true;
	}

	// 资源文件改名，通知storage将文件信息写入资源信息文件
	void Instance::OnDiskFileNameChange(tstring file_name)
	{
		resource_name_ = file_name;
		MainThread::IOS().post(
			boost::bind( &Storage::SaveResourceInfoToDisk,Storage::Inst_Storage())
			);

	}

	// 用于AttatchFilenameByUrl，newname应为完整文件名
	bool Instance::Rename( const tstring& newname )
	{
		const tstring new_name = newname;
		if (new_name.size() == 0)
		{
			return false;
		}
		if (resource_p_)
		{
			StorageThread::IOS().post(boost::bind(&Resource::Rename, resource_p_, new_name));
			return true;
		}
		return false;
	}

	void Instance::GetBlockPosition( size_t block_index,size_t &offset,u_int &length )
	{
		resource_p_->file_resource_desc_p_->GetBlockPosition(block_index,offset,length);
	}

	void Instance::GetSubPiecePosition( const SubPieceInfo &subpiec_info,size_t &offset,u_int &length )
	{
		resource_p_->file_resource_desc_p_->GetSubPiecePosition(subpiec_info,offset,length);
	}

	float Instance::GetInstanceValue()
	{
		size_t total_time = traffic_list_.size();
		if (total_time <= TRAFFIC_PROTECT_TIME)
		{
			return -1;
		}
		if (total_time == 0)
		{
			STORAGE_DEBUG_LOG("Total Time == 0");
			return 0;
		}
		assert(total_time > 0);

		list<size_t>::iterator begin = traffic_list_.begin();
		list<size_t>::iterator end = traffic_list_.end();
		//std::advance(begin, traffic_list_.size()-total_time);
		size_t sum = 0;
		for (list<size_t>::iterator it = begin; it!=end; ++it)
		{
			sum += *it;
		}
		if (resource_p_->actual_size_ == 0)
		{
			STORAGE_DEBUG_LOG("Actual size == 0");
			return 0;
		}
		assert(resource_p_->actual_size_ > 0);
		return float(sum * base::SUB_PIECE_SIZE) / (total_time * (resource_p_->actual_size_));
	}



    bool Instance::ParseMetaData(Buffer buf)
    {
        UINT8 *buffer = buf.data_.get();
        int length = buf.length_;

        if (buffer == NULL || length == 0)
            return false;

        // format
        if (true == p2sp::MetaMP4Parser::IsMP4((const UINT8*)buffer, length))
            GetMetaData().FileFormat = "mp4";
        else if (true == p2sp::MetaFLVParser::IsFLV((const UINT8*)buffer, length))
            GetMetaData().FileFormat = "flv";
        else
            GetMetaData().FileFormat = "none";

        // meta
        if (GetMetaData().FileFormat == "flv")
        {
            p2sp::MetaFLVParser parser;
            parser.Parse((const UINT8*)buffer, length);

            // get
            boost::any duration = parser.GetProperty("duration");
            if (duration.type() == typeid(double)) GetMetaData().Duration = (UINT32)(boost::any_cast<double>(duration) + 0.5);
            boost::any width = parser.GetProperty("width");
            if (width.type() == typeid(double)) GetMetaData().Width = (UINT32)(boost::any_cast<double>(width) + 0.5);
            boost::any height = parser.GetProperty("height");
            if (height.type() == typeid(double)) GetMetaData().Height = (UINT32)(boost::any_cast<double>(height) + 0.5);
            boost::any video_data_rate = parser.GetProperty("videodatarate");
            if (video_data_rate.type() == typeid(double)) GetMetaData().VideoDataRate = (UINT32)(boost::any_cast<double>(video_data_rate) + 0.5);
        }
        else if (GetMetaData().FileFormat == "mp4")
        {
            p2sp::MetaMP4Parser parser;
            parser.Parse((const UINT8*)buffer, length);

            GetMetaData().Duration = parser.GetDurationInSeconds();
        }

        GetMetaData().FileLength = GetFileLength();
        GetMetaData().VideoDataRate = (UINT32)(GetMetaData().FileLength / (GetMetaData().Duration+0.0) + 0.5);

        return true;
    }

    //void Instance::AddContentBuffer(size_t start_position, Buffer buffer)
    //{
    //    if (is_content_buffer_full_)
    //        return ;
    //    size_t last_position = 0;
    //    if (content_buf_.size() > 0)
    //    {
    //        last_position = content_buf_.rbegin()->first + content_buf_.rbegin()->second.length_;
    //        if (last_position >= content_kb_*1024)
    //        {
    //            is_content_buffer_full_ = true;
    //            return ;
    //        }
    //    }
    //    if (start_position != last_position && start_position == 0)
    //    {
    //        content_buf_.clear();
    //        last_position = 0;
    //    }
    //    if (start_position == last_position)
    //    {
    //        // check
    //        if (start_position + buffer.length_ >= content_kb_ * 1024)
    //        {
    //            content_buf_[start_position] = Buffer(buffer.data_.get(), start_position + buffer.length_ - content_kb_ * 1024);
    //            is_content_buffer_full_ = true;
    //        }
    //        else
    //        {
    //            content_buf_[start_position] = buffer;
    //        }
    //    }
    //}
}
