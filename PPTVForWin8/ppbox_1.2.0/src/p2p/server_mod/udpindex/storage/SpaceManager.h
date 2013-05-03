/**
* @file
* @brief SpaceManager 类的包含文件
*/
#pragma once

#include "base/base.h"
#include "storage_base.h"
#include "base\BlockMap.h"
#include "framework/timer/Timer.h"

#include "storage/IStorage.h"
#include "storage/ResourceDescriptor.h"
#include "storage/PendingSubpieceManager.h"
#include "storage/cfgfile.h"
#include "storage/resource.h"
#include "storage/Instance.h"

#include "storage/MemResource.h"
namespace storage
{

	class SpaceManager
		: public boost::noncopyable
		, public boost::enable_shared_from_this<SpaceManager>
	{
	public:
		typedef boost::shared_ptr<SpaceManager> p;
		static SpaceManager::p Create(tstring stor_path,__int64 space_size,bool b_use_disk);
		friend class Storage;
	protected:
		SpaceManager(tstring store_path,__int64 space_size,bool b_use_disk);
	public:
		//空间管理策略
		void RequestResource(Instance::p resource_inst);
		Resource::p OpenResource(FileResourceInfo resource_info);

		//磁盘空间管理策略
		void DiskSpaceMaintain();
		void AddToRemoveResourceList(Resource::p resource_p,size_t disk_file_size);
		bool CancelResourceRequest(Instance::p inst_p);
		void OnRemoveResourceFinish(Resource::p resource_p);
		void OnFreeDiskSpaceFinish(size_t filesize, Resource::p resource_p);
		__int64 GetDirectoryAvialbeSize(tstring DirPathName);
        void OnAllocDiskSpace(size_t alloc_space);


	private:
		bool OnCreateResource(Instance::p resource_inst, size_t init_size);
		void TryCreateResourceFile(tstring filename,size_t file_size,Instance::p resource_inst);
		bool OpenResourceFile(FileResourceInfo resource_info,HANDLE &resource_file_handle,ResourceDescriptor::p &resource_map, size_t &actual_size);
		void GetDirFileNameList(std::set<tstring> &filename_list);
		bool OpenAndCreateStoreDir();
		bool GetDiskFreeSpace(tstring path,__int64 &free_space_size);
		HANDLE TryCreateFile(tstring filename,tstring &last_filename,size_t file_size);
		std::set<Resource::p>  removing_fileresource_set_;
		std::set<Instance::p>  pending_instance_need_resource_set;

		bool b_use_disk_;

		tstring store_path_; 
		__int64 store_size_;
		__int64 curr_resource_files_total_size_;
		//__int64	need_space_size_ ;  /* 有用么 */
		__int64	will_be_free_space_size_;
		__int64  free_size_;

		//config
		size_t file_size_min_;
		size_t file_size_max_;

	};
}
