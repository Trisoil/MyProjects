#pragma once

/*******************************************************************************
*
* Filename: Storage.h
* Comment:  class Storage, �������instance�������ǵ�����
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
			BOOL			bUseDisk,		// �Ƿ�ʹ�ô���, �����TRUE,��ʹ�ô���; �����FALSE,���ڴ�
			ULONGLONG		ullDiskLimit,	// ʹ�ô�������
			wstring			DiskPathName	// ����ʹ��·��
			);
		virtual void Start();

		virtual void Stop();

	private:
		Storage();
		static Storage::p inst_;

	public:
		// ����url_info��map���ҵ��򴴽�һ��instance
		virtual IInstance::p CreateInstance(const UrlInfo& url_info, bool is_force = false);

		// ����rid��Ӧ��instance
		virtual IInstance::p GetInstanceByRID(const RID& rid);

		// Url�����url_map���Ѵ��ڲ�����RID��Ϣ��һ�£������MergeInstanceɾ�������½�����Instance
		virtual void AttachRidByUrl(const string& url, const RidInfo& rid, 
			MD5 content_md5, size_t content_bytes, int flag);

		// �鿴url_info_s�е�url����Ӧ��instance�Ƿ���rid����Ӧ��instance
		// ���url����url_instance_map�У�����Ҫ��url��rid_instƥ�䣬������url_inst != rid_inst��
		// �Ҳ��Ǵ�����ģʽ����url_inst_rid != rid�����map�к�url_inst��ɾ����url
		virtual void AttachHttpServerByRid(const RID& rid, const vector<UrlInfo>& url_info_s);

		// ��ȡrid_inst_map�е�����rid
		virtual void GetLocalResources(set<RID>& rid_s);

		// ����mod_number��group_count��ȡrid_inst_map�е�rid
		virtual void GetLocalResources(set<RID>& rid_s, u_int mod_number, u_int group_count);

		// ��url��Ӧ��instance��url_map��ɾ��ĳ��Url
		virtual void RemoveUrlInfo(const UrlInfo& url_info);

		// ���url��map�У�instance set is_need_to_add
		virtual void AttachContentStatusByUrl(const string& url, bool is_need_to_add);

		// ������µĹ��ڸ�url���ļ���
		virtual void AttachFilenameByUrl(const string& url, const tstring& filename);

        //����ܹ�ʹ���˵Ĵ��̿ռ�
        virtual __int64 GetUsedDiskSpace() { return space_manager_->curr_resource_files_total_size_; }

	private:
		// װ�ش�����Դ��Ϣ�������Դ��Ϣ��ʧ����ɾ�������ļ�
		virtual void LoadResourceInfoFromDisk();

        virtual bool CreateDirectoryRecursive(const tstring& directory);

    public:
		// ÿ0.5s����DiskSpaceMaintain��������Ҫ��ȡ��Դ��instance�б�����Ϊ���Ǵ����ļ���Դ
		virtual void OnTimerElapsed(framework::timer::Timer::p pointer, u_int times);

		// ��url��rid�б���ɾ��ĳ��instance������ӵ�spacemanager���ͷ���Դ�б���
		virtual void RemoveInstance(Instance::p inst);

		// ɾ��ĳ��Instance
        virtual void RemoveInstanceForMerge(Instance::p inst);

		// ��instance_set����һ��instanceɾ���������RemoveInstance
		virtual void RemoveOneInstance();

		// storage��֪ͨinstance�رգ�storageȡ��instance����Դ���룬�ͷ���Դ�ռ�
		virtual void OnInstanceCloseFinish(Instance::p instance_p, Resource::p resource_p);

		// instance��spacemanager������Դ
		void ApplyResource(Instance::p resource_inst);

		// resource�����ɹ�����resource��instance, ����Ϣд��ResourceInfo�ļ���
		void OnCreateResourceSuccess(Resource::p resource_p,Instance::p instance_p);

		// ��instance��ӵ�rid_map�������map�е�rid�ظ�����map�е�inst�ϲ���pointer
		virtual void AddInstanceToRidMap(Instance::p pointer);

		// ����instance_set_,��Instace��Ϣ���浽Resourceinfo��ͬʱ����ԭ��Դ��Ϣ��bak
		virtual void SaveResourceInfoToDisk();

		virtual SpaceManager::p GetSpaceManager() { return space_manager_;};

		// ͨ��url-filename map�в��һ�ȡ�����ļ���
		tstring FindRealName(const string& url);

		// �����ļ������أ������������ļ���
		void DoVerifyName(tstring filename, tstring& lastname);

        tstring GetFullName(tstring filename);

		// ######δ���庯��######
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
		// ��instance_p2�ںϵ�instance_p1
		virtual void MergeInstance(Instance::p instance_p1,Instance::p instance_p2);

		// ���ĳ��instance��rid_map, url_map��inst_set��
		virtual bool AddInstanceToStorage(Instance::p pointer);

	private:
		bool is_running_;
		bool need_save_info_to_disk;	// û��, Ĭ��һֱ����false
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