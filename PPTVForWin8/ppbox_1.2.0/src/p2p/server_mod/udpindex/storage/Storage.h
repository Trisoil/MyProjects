#pragma once

/*******************************************************************************
*
* Filename: Storage.h
* Comment:  class Storage, 负责管理instance，该类是单例的
*
********************************************************************************/

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
#include "storage/SpaceManager.h"
#include "storage/MemResource.h"

namespace storage
{

	class Storage 
		: public boost::noncopyable
		, public boost::enable_shared_from_this<Storage>
		, public framework::timer::ITimerListener
		, public IStorage
	{
	public:
		typedef boost::shared_ptr<Storage> p;

	public:
		static IStorage::p Inst();
		static Storage::p Inst_Storage();
		virtual void Start(	
			BOOL			bUseDisk,		// 是否使用磁盘, 如果是TRUE,则使用磁盘; 如果是FALSE,则纯内存
			ULONGLONG		ullDiskLimit,	// 使用磁盘上限
			wstring			DiskPathName	// 磁盘使用路径
			);
		virtual void Start();

		virtual void Stop();

	private:
		Storage();
		static Storage::p inst_;

	public:
		// 根据url_info从map中找到或创建一个instance
		virtual IInstance::p CreateInstance(const UrlInfo& url_info, bool is_force = false);

		// 返回rid对应的instance
		virtual IInstance::p GetInstanceByRID(const RID& rid);

		// Url如果在url_map中已存在并且与RID信息不一致，则调用MergeInstance删除并重新建立新Instance
		virtual void AttachRidByUrl(const string& url, const RidInfo& rid, 
			MD5 content_md5, size_t content_bytes, int flag);

		// 查看url_info_s中的url所对应的instance是否是rid所对应的instance
		// 如果url不在url_instance_map中，则需要将url与rid_inst匹配，否则，若url_inst != rid_inst，
		// 且不是纯下载模式，且url_inst_rid != rid，则从map中和url_inst中删除该url
		virtual void AttachHttpServerByRid(const RID& rid, const vector<UrlInfo>& url_info_s);

		// 获取rid_inst_map中的所有rid
		virtual void GetLocalResources(set<RID>& rid_s);

		// 根据mod_number和group_count获取rid_inst_map中的rid
		virtual void GetLocalResources(set<RID>& rid_s, u_int mod_number, u_int group_count);

		// 从url对应的instance和url_map中删除某个Url
		virtual void RemoveUrlInfo(const UrlInfo& url_info);

		// 如果url在map中，instance set is_need_to_add
		virtual void AttachContentStatusByUrl(const string& url, bool is_need_to_add);

		// 获得了新的关于该url的文件名
		virtual void AttachFilenameByUrl(const string& url, const tstring& filename);

        //获得总共使用了的磁盘空间
        virtual __int64 GetUsedDiskSpace() { return space_manager_->curr_resource_files_total_size_; }

	private:
		// 装载磁盘资源信息，如果资源信息丢失，则删除所有文件
		virtual void LoadResourceInfoFromDisk();

        virtual bool CreateDirectoryRecursive(const tstring& directory);

    public:
		// 每0.5s调用DiskSpaceMaintain，遍历需要获取资源的instance列表，尝试为它们创建文件资源
		virtual void OnTimerElapsed(framework::timer::Timer::p pointer, u_int times);

		// 从url和rid列表中删除某个instance，并添加到spacemanager的释放资源列表中
		virtual void RemoveInstance(Instance::p inst);

		// 删除某个Instance
        virtual void RemoveInstanceForMerge(Instance::p inst);

		// 从instance_set中找一个instance删除，会调用RemoveInstance
		virtual void RemoveOneInstance();

		// storage被通知instance关闭，storage取消instance的资源申请，释放资源空间
		virtual void OnInstanceCloseFinish(Instance::p instance_p, Resource::p resource_p);

		// instance向spacemanager申请资源
		void ApplyResource(Instance::p resource_inst);

		// resource创建成功，绑定resource和instance, 将信息写入ResourceInfo文件中
		void OnCreateResourceSuccess(Resource::p resource_p,Instance::p instance_p);

		// 将instance添加到rid_map，如果和map中的rid重复，则将map中的inst合并到pointer
		virtual void AddInstanceToRidMap(Instance::p pointer);

		// 遍历instance_set_,将Instace信息保存到Resourceinfo，同时保存原资源信息到bak
		virtual void SaveResourceInfoToDisk();

		virtual SpaceManager::p GetSpaceManager() { return space_manager_;};

		// 通过url-filename map中查找获取到的文件名
		tstring FindRealName(const string& url);

		// 根据文件名判重，并返回最终文件名
		void DoVerifyName(tstring filename, tstring& lastname);

        tstring GetFullName(tstring filename);

		// ######未定义函数######
		void CheckAndDeleteInstance();

		bool InstancePendingHaveSubPiece(Instance::p inst, SubPieceInfo subpiece_info)
		{
			return inst->pending_subpiece_manager_p_->Find(subpiece_info);
		}

		bool InstancePendingEmpty(Instance::p inst)
		{
			return inst->pending_subpiece_manager_p_->Empty();
		}

	protected:
		// 将instance_p2融合到instance_p1
		virtual void MergeInstance(Instance::p instance_p1,Instance::p instance_p2);

		// 添加某个instance到rid_map, url_map和inst_set中
		virtual bool AddInstanceToStorage(Instance::p pointer);

	private:
		bool is_running_;
		bool need_save_info_to_disk;	// 没用, 默认一直都是false
		tstring resourceinfo_file_;		// "ResourceInfo.dat"
		tstring resourceinfo_bak_file_; // "ResourceInfo.dat.bak"

		framework::timer::PeriodicTimer::p space_manager_timer_; // 0.5s
		framework::timer::PeriodicTimer::p res_info_timer_;
		std::set<Instance::p>  instance_set_;			// inst_set
		std::map<RID,Instance::p>  rid_instance_map_;	// rid_map
		std::map<string,Instance::p> url_instance_map_;	// url_map
		std::map<string, tstring> url_filename_map_;	// url-filename
		SpaceManager::p space_manager_;

	};
}