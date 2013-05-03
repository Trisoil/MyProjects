/*******************************************************************************
*
*  Filename: Storage.cpp
*
*******************************************************************************/

#include "stdafx.h"
#pragma  comment(lib, "shlwapi.lib")
#include <shlwapi.h>
#include "storage/Storage.h"
#include "storage/storagethread.h"

// ������У���"ResourceInfo.dat"��"ResourceInfo.dat.bak"���ᱣ����ppva��װĿ¼��
//#define _STORAGE_RESINFO_USE_PPVA_PATH_

namespace storage
{
	Storage::p Storage::inst_(new Storage());

	Storage::Storage():is_running_(false),need_save_info_to_disk(false)
	{
	}

	IStorage::p Storage::Inst()
	{
		return inst_;
		
	}
	Storage::p Storage::Inst_Storage()
	{
		return inst_;
	}
	
	void Storage::Start(
		BOOL		bUseDisk,			// �Ƿ�ʹ�ô���, �����TRUE,��ʹ�ô���; �����FALSE,���ڴ�
		ULONGLONG	ullDiskLimit,		// ʹ�ô�������
		wstring		DiskPathName		// ����ʹ��·��
		)
	{
		STORAGE_EVENT_LOG("storage system start.............");
		IniFile ini;
		ini.SetSection(TEXT("Storage"));

		if(DiskPathName.size()==0)
		{
			DiskPathName = ini.GetString(TEXT("DefalutStorePath"),TEXT("d:\\shareFLV\\"));
		}
		else
		{
			if(DiskPathName.rfind(TEXT('\\'))!=DiskPathName.size()-1)
			{
				DiskPathName.append(TEXT("\\"));
			}
		}
		int disk_limit = ullDiskLimit/1024/1024;
		ULONGLONG disk_limit_size = ini.GetInteger(TEXT("DiskLimitSize"),disk_limit);
		disk_limit_size = disk_limit_size*1024*1024;
        CreateDirectoryRecursive(DiskPathName);

#ifdef _STORAGE_RESINFO_USE_PPVA_PATH_	// -------------------------------------
		framework::mswin::Module _tmp_module;
		framework::io::Path _tmp_path;
		static short _tmp_use_for_module = 0;
		_tmp_module.AttachAddress(&_tmp_use_for_module);
		_tmp_module.GetFileDirectory(_tmp_path);
		resourceinfo_file_.assign(_tmp_path.GetString());
		resourceinfo_bak_file_.assign(_tmp_path.GetString());
#else	// ---------------------------------------------------------------------

		resourceinfo_file_.assign(DiskPathName);
		resourceinfo_bak_file_.assign(DiskPathName);
#endif	// ---------------------------------------------------------------------

		resourceinfo_file_.append(TEXT("ResourceInfo.dat"));
		resourceinfo_bak_file_.append(TEXT("ResourceInfo.dat.bak"));

		tstring store_path;
		store_path.assign(DiskPathName);
		space_manager_ = SpaceManager::Create(store_path,disk_limit_size,bUseDisk);//ullDiskLimit,bUseDisk);
		STORAGE_EVENT_LOG(" store_path:"<<framework::w2b(store_path)<<" disk_limit_size:"<<disk_limit_size<<" bUseDisk"<<bUseDisk);

		StorageThread::Inst().Start();
		space_manager_timer_ = framework::timer::PeriodicTimer::create(5000, shared_from_this() );
		space_manager_timer_->Start();
		res_info_timer_ = framework::timer::PeriodicTimer::create(60*1000, shared_from_this());
		res_info_timer_->Start();
		LoadResourceInfoFromDisk();
		is_running_ = true;
		SaveResourceInfoToDisk();
		STORAGE_EVENT_LOG("storage system start success!");
	}

	void Storage::Start()
	{
		tstring store_path(TEXT("d:\\shareFLV\\"));
		Start(true,400*1024*1024,store_path); // 400M
	}

	// storage��֪ͨinstance�رգ�storageȡ��instance����Դ���룬�ͷ���Դ�ռ�
	void Storage::OnInstanceCloseFinish(Instance::p instance_p, Resource::p resource_p)
	{
		space_manager_->CancelResourceRequest(instance_p);

		if(instance_p->GetStatus()==INSTANCE_REMOVING)
		{
			if(resource_p)
			{
				space_manager_->OnRemoveResourceFinish(resource_p);
			}
		}
		size_t dc = instance_set_.erase(instance_p);
		assert(dc==1);
		
		//! �ӳ��˳������������ӱ�־λ
		if(instance_set_.empty())
		{
			StorageThread::Inst().Stop();
		}
	}


	void Storage::Stop()
	{
		if(is_running_==false)
			return;

		STORAGE_EVENT_LOG("storage system stop!");

		if(space_manager_timer_) space_manager_timer_->Stop();
		if (res_info_timer_) res_info_timer_->Stop();
		SaveResourceInfoToDisk();
		is_running_ = false;
		set<Instance::p>::const_iterator it = instance_set_.begin();
		for(;it!=instance_set_.end();it++)
		{
			(*it)->Stop();
		}
		rid_instance_map_.clear();
		url_instance_map_.clear();
		StorageThread::Inst().Stop();
		STORAGE_TEST_DEBUG("Storage stop complate");
	}

	// ����url_info��map���ҵ��򴴽�һ��instance
	IInstance::p Storage::CreateInstance(const UrlInfo& url_info, bool is_force)
	{
		if(is_running_==false) return IInstance::p();
		STORAGE_EVENT_LOG("CreateInstance!url: "<<url_info<<(is_force?"force!":""));

		map<string,Instance::p>::const_iterator it = url_instance_map_.find(url_info.url_);
		if(it!=url_instance_map_.end())
		{
			//�ҵ�һ��Instance
			if(!is_force)
			{
				//��ǿ��
				STORAGE_EVENT_LOG("�Ѵ��� �������е�instance "<<url_info<<(is_force?"force!":""));
				it->second->AddUrlInfo(url_info); //?
				return it->second;
			}
			//ǿ����
			it->second->RemoveUrl(url_info.url_);
			url_instance_map_.erase(url_info.url_);
			STORAGE_EVENT_LOG("�Ѵ��� ���Ǵ���ģʽ ������instance "<<url_info<<(is_force?"force!":""));
		}
		Instance::p inst_p = Instance::Create(url_info);
		if(!inst_p)
		{
			return inst_p;
		}
		if(is_force)
		{
			inst_p->SetPureDownloadMode(true);
		}
		inst_p->Start();
		instance_set_.insert(inst_p);
		url_instance_map_.insert(make_pair(url_info.url_,inst_p));
		STORAGE_EVENT_LOG("CreateInstance success!");
		return inst_p;
	}

	// ��instance��ӵ�rid_map�������map�е�rid�ظ�����map�е�inst�ϲ���pointer
	void Storage::AddInstanceToRidMap(Instance::p pointer)
	{
		if(is_running_==false) return;
		if(!pointer)
			return;
		STORAGE_EVENT_LOG("AddInstanceToRidMap!instance: "<<pointer);

		RID rid = pointer->GetRID();
		assert(!rid.IsEmpty()); // rid����ǿ�
		map<RID,Instance::p>::iterator iter = rid_instance_map_.find(rid);
		// rid�ظ�
		if(iter!=rid_instance_map_.end())
		{
			// �ظ���ϲ����ϲ���pointer��
			MergeInstance(pointer,iter->second);
			return ;
		}
		rid_instance_map_.insert(make_pair(rid,pointer));
		return;
	}

	// ��instance2�ںϵ�instance1
	void Storage::MergeInstance(Instance::p instance_p1,Instance::p instance_p2)
	{
		if(is_running_==false) return;
		
		// ����Storage��url��rid�б���ԭ����inst2ƥ���url��ridɾ��������inst1ƥ��
		STORAGE_EVENT_LOG("MergeInstance! merge inst2: "<<instance_p2<<" To inst1: "<<instance_p1);
		assert(!instance_p1->GetRID().IsEmpty());

		vector<UrlInfo> in_url_info_s;
		instance_p2->GetUrls(in_url_info_s);
		//url
		assert(in_url_info_s.size()>=1);
		//instance_p1->AddUrlInfo(in_url_info_s);
		for(size_t i = 0; i<in_url_info_s.size();i++)
		{
			size_t dc = url_instance_map_.erase(in_url_info_s[i].url_);
			assert(dc==1);
			url_instance_map_.insert(make_pair(in_url_info_s[i].url_, instance_p1));
            STORAGE_DEBUG_LOG("�ϲ�URL: " << in_url_info_s[i].url_ << " �� instance: " << instance_p1);
		}
		instance_p1->AddUrlInfo(in_url_info_s);

		//rid
		if(!instance_p2->GetRID().IsEmpty())
		{
			size_t dc = rid_instance_map_.erase(instance_p2->GetRID());
            STORAGE_DEBUG_LOG("Erase " << instance_p2->GetRID() << " from rid_instance_map_");
			assert(dc==1);
			assert(!instance_p1->GetRID().IsEmpty());
			rid_instance_map_.insert(make_pair(instance_p1->GetRID(),instance_p1));
            STORAGE_DEBUG_LOG("Add " << instance_p2->GetRID() << " to rid_instance_map_");
		}

		//��instance_p2��Դ������instance_p1
		instance_p1->Merge(instance_p2); // ֻ�Ǵ��˸�log��ɶ��û��
		instance_p2->BeMergedTo(instance_p1); // ��ɺϲ�����֪ͨDownloadDriverǨ��
	}

	// instance��spacemanager������Դ
	void Storage::ApplyResource(Instance::p resource_inst)
	{
		if(is_running_==false) return;
		space_manager_->RequestResource(resource_inst);
	}

	// resource�����ɹ�����resource��instance, ����Ϣд��ResourceInfo�ļ���
	void Storage::OnCreateResourceSuccess(Resource::p resource_p,Instance::p instance_p)
	{
		assert(resource_p);
		assert(instance_p);
		instance_p->AttachResource(resource_p);
	}

	// ����instance_set_,��Instace��Ϣ���浽Resourceinfo��ͬʱ����ԭ��Դ��Ϣ��bak
	void Storage::SaveResourceInfoToDisk()
	{
		if(is_running_==false) return;
		int count = 0;
		ResourceInfoListFile r_file ;
		// �򿪳ɹ���ԭ��Դ��Ϣ����

		if(r_file.SecOpen(resourceinfo_file_.c_str()))
		{
			r_file.SecClose();
			::MoveFileEx(resourceinfo_file_.c_str(),resourceinfo_bak_file_.c_str(),MOVEFILE_REPLACE_EXISTING);
		}
		STORAGE_TEST_DEBUG("Save Resource Info###########################>");
		//RELEASE_OUTPUT("Save Resource Info###########################>");
		//���ǽ����ļ�
		if(r_file.SecCreate( resourceinfo_file_.c_str()))
		{
			//����instance_set_,��Instace��Ϣ���浽Resourceinfo�������ϰ�ȫ����
			set<Instance::p>::const_iterator it = instance_set_.begin();
			for(;it!=instance_set_.end();it++)
			{
				Instance::p inst = *it;
				FileResourceInfo r_info;
				if(!inst->GetFileResourceInfo(r_info))
				{
					continue;
				}
				ostringstream oss;
				for (list<size_t>::const_iterator it = r_info.traffic_list_.begin();
					it != r_info.traffic_list_.end(); ++it)
				{
					oss<<*it<<", ";
				}
				string tmp_traffic_s = oss.str();
				STORAGE_TEST_DEBUG(""
					<<"�ļ���: "<<framework::w2b(r_info.file_path_)
					<<"\n\tRID: "<<r_info.rid_info_.rid_
					<<"\tLastPushTime: "<<r_info.last_push_time_
					<<"\t����ͳ�ƣ�"<<tmp_traffic_s
					<<"Value: "<<inst->GetInstanceValue());
				STORAGE_TEST_DEBUG(""
					<<"�ļ���: "<<framework::w2b(r_info.file_path_)
					<<"\n\tRID: "<<r_info.rid_info_.rid_
					<<"\tLastPushTime: "<<r_info.last_push_time_
					<<"\t����ͳ�ƣ�"<<tmp_traffic_s
					<<"Value: "<<inst->GetInstanceValue());
				r_file.AddResourceInfo(r_info);
				count++;
			}
		}

		//�򲻿�����������
		r_file.SecClose();
		STORAGE_EVENT_LOG("SaveResourceInfoToDisk! resource count:"<<count);
	};

    bool Storage::CreateDirectoryRecursive(const tstring& directory)
    {
        STORAGE_EVENT_LOG("������ʼ");
        framework::io::Path parent_dir;
        parent_dir.Assign(directory.c_str());
        if (CreateDirectory(directory.c_str(), NULL) == FALSE)
        {
            if (GetLastError() == ERROR_ALREADY_EXISTS)
                return true;
            else
            {
                parent_dir.RemoveBackslash();
                if (false == parent_dir.RemoveFileSpec())
                    return false;

                if (CreateDirectoryRecursive(parent_dir.GetString()))
                {
                    return CreateDirectory(directory.c_str(), NULL);
                }
                return false;
            }
        }
        return true;
    }

	// ----------------------------------------------------------------------
	// װ�ش�����Դ��Ϣ�������Դ��Ϣ��ʧ����ɾ�������ļ�
	void Storage::LoadResourceInfoFromDisk()
	{
		int count = 0;
		std::set<tstring> filename_list; // ��������·��

		space_manager_->GetDirFileNameList(filename_list);
		filename_list.erase(resourceinfo_file_);
		filename_list.erase(resourceinfo_bak_file_);
//STORAGE_TEST_DEBUG("Load Resource Info-------------------");
		//��ȡ�ϴδ����Դ��Ϣ
		ResourceInfoListFile r_file;
		if(!r_file.SecOpen(resourceinfo_file_.c_str()))
		{
			r_file.SecClose();
			::DeleteFile(resourceinfo_file_.c_str());
			STORAGE_EVENT_LOG("LoadResourceInfoFromDisk!��ȡ��Դ��Ϣʧ�ܣ�");
            return ;
			::MoveFileEx(resourceinfo_bak_file_.c_str(),resourceinfo_file_.c_str(),MOVEFILE_REPLACE_EXISTING);
			//ʧ�ܣ���ȡ����
			if(!r_file.SecOpen(resourceinfo_file_.c_str()))
			{
				//�����ƻ���
				STORAGE_EVENT_LOG("LoadResourceInfoFromDisk!��ȡ��Դ��Ϣ����ʧ��");
				r_file.SecClose();
				::DeleteFile(resourceinfo_file_.c_str());
				for(std::set<tstring>::iterator it = filename_list.begin();it!=filename_list.end();it++)
				{
					::DeleteFile(it->c_str());
					STORAGE_EVENT_LOG("ɾ���洢Ŀ¼�����ļ�"<<framework::w2b(*it));
				}
				STORAGE_EVENT_LOG("SaveResourceInfoToDisk! resource count:"<<count);
				return;
			}
		}

		FileResourceInfo r_info;

		while(r_file.GetResourceInfo(r_info))
		{
			//STORAGE_TEST_DEBUG(""
			//	<<"last_push_time:"<<r_info.last_push_time_
			//	<<"\ttraffic_list.size():"<<r_info.traffic_list_.size());
			// ��ȡresourceinfo
			// ���RIDһ���ԣ�
			if(!r_info.rid_info_.GetRID().IsEmpty()) // rid�ǿ�
			{
				// ��rid, ɾ���ļ���cfg�ļ�
				if(rid_instance_map_.find(r_info.rid_info_.GetRID())!=rid_instance_map_.end())
				{
					tstring tmp_file_name;
					tmp_file_name = r_info.file_path_;
					::DeleteFile(tmp_file_name.c_str());
					tmp_file_name = r_info.file_path_ +cfg_extname;
					::DeleteFile(tmp_file_name.c_str());
					continue;
				}
			}
			//���urlһ���ԣ�
			vector<UrlInfo>::iterator it_url = r_info.url_info_.begin();
			int url_count = 0;
			for(; it_url!=r_info.url_info_.end();it_url++)
			{
				// �����Url��������
				if(url_instance_map_.find((*it_url).url_)!=url_instance_map_.end())
				{
					continue;
				}
				url_count++;
			}

			// ��Url��ɾ���ļ���cfg�ļ�
			if((url_count==0)&&(r_info.rid_info_.GetRID().IsEmpty()))
			{
				tstring tmp_file_name;
				tmp_file_name = r_info.file_path_;
				::DeleteFile(tmp_file_name.c_str());
				tmp_file_name = r_info.file_path_ +cfg_extname;
				::DeleteFile(tmp_file_name.c_str());
				continue;
			}
			tstring cfgfile = r_info.file_path_ + cfg_extname;
			filename_list.erase(cfgfile);

			//�����Դ�ļ��ͳ���
			Resource::p resource_p = space_manager_->OpenResource(r_info);
			if(!resource_p)
			{
				tstring tmp_file_name;
				tmp_file_name = r_info.file_path_;
				::DeleteFile(tmp_file_name.c_str());
				tmp_file_name = r_info.file_path_ +cfg_extname;
				::DeleteFile(tmp_file_name.c_str());
				continue;
			}
			Instance::p pointer = Instance::Open(r_info,resource_p);
			STORAGE_EVENT_LOG("OpenResource Success! local_file_name:"<<framework::w2b(r_info.file_path_)
				<<" �����أ�"<<pointer->GetDownloadBytes()<<"/"<<pointer->GetFileLength()<<" rid_info:"<<r_info.rid_info_);
			//
			filename_list.erase(r_info.file_path_);
			filename_list.erase(r_info.file_path_+cfg_extname);

			//����Instance
			assert(pointer);
			pointer->Start();
			//���Instance��Storage	
			AddInstanceToStorage(pointer);
			count++;
		}
		r_file.SecClose();

		for(std::set<tstring>::iterator it = filename_list.begin();it!=filename_list.end();it++)
		{
			::DeleteFile(it->c_str());
			STORAGE_EVENT_LOG("ɾ���洢Ŀ¼�����ļ�"<<framework::w2b(*it));
		}
		STORAGE_EVENT_LOG("SaveResourceInfoToDisk! resource count:"<<count);
	}

	// ����rid��Ӧ��instance
	IInstance::p Storage::GetInstanceByRID(const RID& rid)
	{
		if(is_running_==false) return IInstance::p();

		IInstance::p pointer;
		map<RID,Instance::p>::const_iterator it = rid_instance_map_.find(rid);
		if(it!=rid_instance_map_.end())
		{
			pointer = it->second;
		}
		return pointer;
	}

	// ���ĳ��instance��rid_map, url_map��inst_set��
	bool Storage::AddInstanceToStorage(Instance::p pointer)
	{
		if(!pointer)
			return false;

		RID rid = pointer->GetRID();
		if(!rid.IsEmpty()) // rid�ǿ�
		{
			assert(rid_instance_map_.find(rid)==rid_instance_map_.end());
			rid_instance_map_.insert(make_pair(rid,pointer));
		}
		vector<UrlInfo> url_s;
		pointer->GetUrls(url_s);
		for(size_t i = 0;i<url_s.size();i++)
		{
			if(url_instance_map_.find(url_s[i].url_)==url_instance_map_.end())
			{
				url_instance_map_.insert(make_pair(url_s[i].url_,pointer));
			}
		}
		instance_set_.insert(pointer);
		return true;
	}

	// �鿴url_info_s�е�url����Ӧ��instance�Ƿ���rid����Ӧ��instance
	// ���url����url_instance_map�У�����Ҫ��url��rid_instƥ�䣬������url_inst != rid_inst��
	// �Ҳ��Ǵ�����ģʽ����url_inst_rid != rid�����map�к�url_inst��ɾ����url
	void Storage::AttachHttpServerByRid(const RID& rid, const vector<UrlInfo>& url_info_s)
	{
		if(is_running_==false) return ;
		//�˴���URLһ�㲻����URL����Ҫ����ά��RID��URL��Ϣ��һ���ԣ�
		//������ֳ�ͻ���������ںϣ�����AttachRidByUrl��Resource������ɺ��RID�ȽϺ��ں�
		map<RID,Instance::p>::const_iterator iter = rid_instance_map_.find(rid);
		if(iter==rid_instance_map_.end())
			return;
		Instance::p inst = iter->second;
		vector<UrlInfo>::const_iterator v_url = url_info_s.begin(); 

		vector<UrlInfo> in_url_info_s; //���ڱ�����Ҫ��ӵ�inst���url�б�
		for(size_t i = 0; i<url_info_s.size();i++)
		{
			map<string,Instance::p>::iterator it = url_instance_map_.find(url_info_s[i].url_);
			if(it!=url_instance_map_.end())
			{
				if(it->second==inst) //��ȷ���Ѵ���
				{
					//in_url_info_s.push_back(url_info_s[i]);
					continue;
				}
				if(it->second->IsPureDownloadMode()) 
				{
					//��ͻ�������Ǵ�����ģʽʵ��,������URL
					continue;
				}

				if(it->second->GetRID().IsEmpty())  //�Ǵ��£�����û��RID
				{
					//���ں�
					//MergeInstance(iter->second,inst1);
					continue;
				}

				//RID����
				if(rid!=it->second->GetRID())
				{
					//����RID���и�url����������Ϣ�������������ɾ��url��DD�ģ���ô�죿
					it->second->RemoveUrl(url_info_s[i].url_);
					url_instance_map_.erase(it);
				}
			}
			// url����url_instance_map�У�����Ҫ��url��rid����Ӧ��instanceƥ��
			in_url_info_s.push_back(url_info_s[i]);
			url_instance_map_.insert(make_pair(url_info_s[i].url_,inst));
		}

		if(in_url_info_s.size())
		{
			inst->AddUrlInfo(in_url_info_s);
		}
	}

	// ���url��map�У�instance set is_need_to_add
	void Storage::AttachContentStatusByUrl(const string& url, bool is_need_to_add)
	{
		if(is_running_==false) return ;
		
		map<string,Instance::p>::const_iterator it = url_instance_map_.find(url);
		if(it==url_instance_map_.end())
		{
			STORAGE_ERR_LOG("Storage::AttachRidByUrl "<<"not find url:"<<url);
			return ;
		}
		Instance::p inst1 = it->second;

		inst1->SetContentNeedToAdd(is_need_to_add);

	}

	// ������µĹ��ڸ�url���ļ���
	void Storage::AttachFilenameByUrl(const string& url, const tstring& filename)
	{
		if (false == is_running_)
		{
			return;
		}
		tstring file_name = filename;
		if (file_name.find(TEXT(".flv")) == tstring::npos)
		{
			file_name.append(TEXT(".flv"));
		}

		map<string, Instance::p>::const_iterator it = url_instance_map_.find(url);
		// instance��δ���������浽map��
		if (it == url_instance_map_.end())
		{
			url_filename_map_.insert(make_pair(url,file_name));
			return;
		}
		// instance�Ѿ�������������
		Instance::p inst = it->second;
		tstring full_name = GetFullName(file_name);
		if (false == inst->IsComplete())
		{
			full_name = full_name + tpp_extname;
		}
        if (inst->resource_name_ != full_name)
        {
            DoVerifyName(file_name, full_name);
            if (false == inst->IsComplete())
            {
                full_name = full_name + tpp_extname;
            }
		    if (inst->Rename(full_name))
		    {
			    inst->resource_name_ = full_name;
			    SaveResourceInfoToDisk();
		    }
		    else
		    {
			    url_filename_map_.insert(make_pair(url, file_name));
		    }
        }
	}

    tstring Storage::GetFullName(tstring filename)
    {
        const static tstring sample = _T("CON|PRN|AUX|NUL|COM1|COM2|COM3|COM4|COM5|COM6|COM7|COM8|COM9|LPT1|LPT2|LPT3|LPT4|LPT5|LPT6|LPT7|LPT8|LPT9");

        tstring file_name = filename;
        tstring flvext = TEXT(".flv");
        size_t pos = file_name.find(flvext);
        file_name.erase(pos);

        // ����ļ����Ƿ�Ϸ�
        file_name.erase(
            std::remove_if(file_name.begin(), file_name.end(), boost::algorithm::is_any_of(_T("<>:\"/\\|?*"))),
            file_name.end()
            );
        file_name.erase(
            std::remove_if(file_name.begin(), file_name.end(), boost::algorithm::is_from_range((TCHAR)0, (TCHAR)31)),
            file_name.end()
            );

        boost::algorithm::trim(file_name);

        if (file_name.length() <= 4 && sample.find(file_name) != tstring::npos)
            file_name += _T("_");

        if (file_name.length() > 200)
            file_name = file_name.substr(0, 200);

        file_name = space_manager_->store_path_ + file_name;
        filename = file_name + TEXT(".flv");

        return filename;
    }

	// �����ļ������أ������������ļ���
	void Storage::DoVerifyName(tstring filename, tstring& lastname)
	{
		// filename��".flv"
		// file_name����".flv"
        const static tstring sample = _T("CON|PRN|AUX|NUL|COM1|COM2|COM3|COM4|COM5|COM6|COM7|COM8|COM9|LPT1|LPT2|LPT3|LPT4|LPT5|LPT6|LPT7|LPT8|LPT9");

        tstring file_name = filename;
		tstring flvext = TEXT(".flv");
		size_t pos = file_name.find(flvext);
		file_name.erase(pos);

        // ����ļ����Ƿ�Ϸ�
        file_name.erase(
            std::remove_if(file_name.begin(), file_name.end(), boost::algorithm::is_any_of(_T("<>:\"/\\|?*"))),
            file_name.end()
        );
        file_name.erase(
            std::remove_if(file_name.begin(), file_name.end(), boost::algorithm::is_from_range((TCHAR)0, (TCHAR)31)),
            file_name.end()
        );

        boost::algorithm::trim(file_name);

        if (file_name.length() <= 4 && sample.find(file_name) != tstring::npos)
            file_name += _T("_");

        if (file_name.length() > 200)
            file_name = file_name.substr(0, 200);

        file_name = space_manager_->store_path_ + file_name;
        filename = file_name + TEXT(".flv");

        // ����ļ��Ƿ����
		int i = 0;
		while(true)
		{
			// ������ͬ�����ļ�
			if(!::PathFileExists(filename.c_str()))
			{
				// תΪtpp�ļ���
				tstring tppfilename = filename + tpp_extname;
				if(!::PathFileExists(tppfilename.c_str()))
				{
					break;
				}
			}
			// �������ͬ���ļ����Զ�������
			char changestr[40];
			int changestr_size = sprintf_s(changestr,"(%d)",i++);
			string change_str(changestr,changestr_size);
			filename =file_name+framework::b2w(change_str)+flvext;
		}
		lastname  = filename;
	}

	tstring Storage::FindRealName(const string& url)
	{
		map<string, tstring>::const_iterator it = url_filename_map_.find(url);
		// urlû�ж�Ӧ����ʵ����
		if (it == url_filename_map_.end())
		{
			return TEXT("");
		}
		return it->second;
	}

	// Url�����url_map���Ѵ��ڲ�����RID��Ϣ��һ�£������MergeInstanceɾ�������½�����Instance
	void Storage::AttachRidByUrl(const string& url, const RidInfo& rid, MD5 ContentMD5, size_t ContentBytes, int flag) 
	{
		if(is_running_==false) return ;
        STORAGE_EVENT_LOG("Storage::AttachRidByUrl, URL: " << url << ", RID: " << rid);

        // ��֤rid
        CMD5::p hash = CMD5::Create();
        for(int i = 0;i<rid.block_count_;i++)
        {
            if(rid.block_md5_s_[i].IsEmpty())
            {
                STORAGE_DEBUG_LOG(" false: rid_info_.block_md5_s_[i].IsEmpty() "<<rid);
                return ;
            }
            hash->Add((LPCVOID)&(rid.block_md5_s_[i]),sizeof(RID));
        }

        hash->Finish();
        SMD5 valid_md5;
        hash->GetHash(&valid_md5);
        RID valid_rid = *(RID*)(&valid_md5);
        if (valid_rid != rid.rid_)
        {
            STORAGE_DEBUG_LOG(" false: valid_rid != rid.rid_ "<<rid);
            return ;
        }

		//�˴���URLһ�㶼����URL������Ѵ��ڲ�����RID��Ϣ��һ�£�ɾ�������½�����Instance
		map<string,Instance::p>::const_iterator it = url_instance_map_.find(url);
		if(it==url_instance_map_.end())
		{
			STORAGE_ERR_LOG("Storage::AttachRidByUrl, not such url:" << url);
			return ;
		}
		Instance::p inst1 = it->second;
		inst1->SetRidOriginFlag(flag);
		//�Ǵ���ģʽ
		if(inst1->IsPureDownloadMode())
        {
            STORAGE_DEBUG_LOG("Storage::AttachRidByUrl, IsPureDownloadMode");
			return;
        }

		//RID�Ѵ��ڣ�������RID
		//! ��μ�ʱ����RID
		if(!inst1->GetRID().IsEmpty())
		{
            STORAGE_DEBUG_LOG("Storage::AttachRidByUrl, inst rid is not empty");
			return ;
		}

//         STORAGE_DEBUG_LOG("rid_instance_map_: " << rid_instance_map_.size());
//         for (map<RID, Instance::p>::iterator it = rid_instance_map_.begin(); it != rid_instance_map_.end(); ++it)
//             STORAGE_DEBUG_LOG("LOAD: " << it->first);
		
        // url_inst(inst1)��RIDΪ��
		map<RID,Instance::p>::const_iterator iter = rid_instance_map_.find(rid.GetRID());

        //û���ظ��ģ�����Ҫ�ں�,ֱ������RidInfo
		if(iter == rid_instance_map_.end())
		{
            STORAGE_DEBUG_LOG("No such rid, no need Merge");

			if((inst1->GetFileLength()!=0)&&(inst1->GetFileLength()!=rid.file_length_))
			{
				//���ȼ�鲻�������أ�
				//inst1->SetPureDownloadMode(true);
				return;
			}

			//��ͷ�ڿ��ǿ��ǣ�������������������������������������������������������������������������������������������������������
			// ��rid��url_instƥ�䣬��Ϊurl_inst���rid��Ϣ
			rid_instance_map_.insert(make_pair(rid.GetRID(),inst1));
			inst1->SetRidInfo(rid, ContentMD5, ContentBytes);


		}
        else
        {
		    inst1->SetRidInfo(rid, ContentMD5, ContentBytes);

		    //��Ҫ�����ں�,�򵥲��ԣ��ϵ�merge�µ�
		    MergeInstance(iter->second,inst1); // ��inst1�ϲ���iter->second��
        }
	}

	// ��ȡrid_inst_map�е�����rid
	void Storage::GetLocalResources(set<RID>& rid_s)
	{
		if(is_running_==false) return ;
		for(map<RID,Instance::p>::const_iterator iter = rid_instance_map_.begin();iter!=rid_instance_map_.end();iter++)
		{
			rid_s.insert(iter->first);
		}
	}

	// ����mod_number��group_count��ȡrid_inst_map�е�rid
	void Storage::GetLocalResources(set<RID>& rid_s, u_int mod_number, u_int group_count)
	{
		if(is_running_==false) return ;
		map<RID,Instance::p>::const_iterator iter = rid_instance_map_.begin();
		for(map<RID,Instance::p>::const_iterator iter = rid_instance_map_.begin();iter!=rid_instance_map_.end();iter++)
		{
			if (base::GuidMod(iter->first, group_count) == mod_number)
				rid_s.insert(iter->first);
		}
	}

	// ��url��Ӧ��instance��url_map��ɾ��ĳ��Url
	void Storage::RemoveUrlInfo(const UrlInfo& url_info)
	{
		if(is_running_==false) return;
		map<string,Instance::p>::const_iterator it = url_instance_map_.find(url_info.url_);
		if(it==url_instance_map_.end())
		{
			return ;
		}
		it->second->RemoveUrl(url_info.url_);
		url_instance_map_.erase(url_info.url_);
	}

	void Storage::RemoveInstanceForMerge(Instance::p inst)
	{
		if(!inst)
			return;
		if(inst->resource_p_)
		{
			space_manager_->AddToRemoveResourceList(inst->resource_p_,inst->GetDiskFileSize());
		}

		inst->Remove(); // ɾ��instance
		return ;
	}

	// ��url��rid�б���ɾ��ĳ��instance������ӵ�spacemanager���ͷ���Դ�б���
	void Storage::RemoveInstance(Instance::p inst)
	{
		STORAGE_INFO_LOG(""<<inst->GetResourceName()<<" disk_file_size:"<<inst->GetDiskFileSize());
		assert((inst->GetDDNum()==0));

		if(!inst)
			return;
		//RELEASE_OUTPUT("");
		STORAGE_TEST_DEBUG("\n��Ҫ��ɾ�����ļ���"<<framework::w2b(inst->resource_name_)
			<<"\n\t�ļ���С��"<<float(inst->GetFileLength())/1024/1024<<"MB"
			<<"\tRID: "<<inst->GetRID()
			<<"\tValue: "<<inst->GetInstanceValue()
			<<"\n");
		STORAGE_TEST_DEBUG("\n��Ҫ��ɾ�����ļ���"<<framework::w2b(inst->resource_name_)
			<<"\n\t�ļ���С��"<<float(inst->GetFileLength())/1024/1024<<"MB"
			<<"\tRID: "<<inst->GetRID()
			<<"\tValue: "<<inst->GetInstanceValue()
			<<"\n");

		RID rid = inst->GetRID();
		if(!rid.IsEmpty())
		{
			rid_instance_map_.erase(rid);
		}
		vector<UrlInfo> url_s;
		inst->GetUrls(url_s);
		for(int i = 0;i<url_s.size();i++)
		{
			url_instance_map_.erase(url_s[i].url_);
		}

		if(inst->resource_p_)
		{
			space_manager_->AddToRemoveResourceList(inst->resource_p_,inst->GetDiskFileSize());
		}

		inst->Remove(); // ɾ��instance
		return ;
	}

	// ��instance_set����һ��instanceɾ��
	void Storage::RemoveOneInstance()
	{
		multimap<float, Instance::p> value_inst;
		float min_value = 10000000;
		Instance::p inst_rm, inst_minvalue;
        for(std::set<Instance::p>::iterator it = instance_set_.begin();it!=instance_set_.end();it++)
        {
            Instance::p inst_p = *it;
            if(inst_p->CanRemove())
            {
				// inst_ridΪ��
                if (inst_p->GetRID().IsEmpty())
                {
                    RemoveInstance(inst_p);
                    return;
                }
				if (inst_rm.get() == 0)
				{
					inst_rm = inst_p;
				}
				// inst_rid �ǿ�
				float tmp_value = inst_p->GetInstanceValue();
				if (tmp_value >=0 && tmp_value <min_value)
				{
					min_value = tmp_value;
				}
				value_inst.insert(make_pair(tmp_value, inst_p));
            }
        } // �����forѭ��
		if (value_inst.find(min_value) != value_inst.end() && inst_minvalue.get() == 0)
		{
			inst_minvalue = value_inst.find(min_value)->second;
		}
		if (inst_minvalue.get() == 0 && inst_rm.get() == 0)
		{
			STORAGE_DEBUG_LOG("�Ҳ���Ӧ��ɾ�����ļ�");
			return;
		}
		RemoveInstance(inst_minvalue.get()? inst_minvalue : inst_rm);
	}


	void Storage::OnTimerElapsed(framework::timer::Timer::p pointer, u_int times)
	{
		if(is_running_==false) return;

		if(pointer==space_manager_timer_)
		{
			space_manager_->DiskSpaceMaintain();
			//STORAGE_DEBUG_LOG("�ؼ�����size"<<endl
			//	<<" instance_set_.size()"<<instance_set_.size()<<endl 
			//	<<" rid_instance_map_.size()"<<rid_instance_map_.size()<<endl
			//	<<" url_instance_map_.size()"<<url_instance_map_.size()<<endl
			//	<<" space_manager_->removing_fileresource_set_.size()"<<space_manager_->removing_fileresource_set_.size()<<endl
			//	<<" space_manager_->pending_instance_need_resource_set.size()"<<space_manager_->pending_instance_need_resource_set.size());
			//set<Instance::p>::const_iterator it = instance_set_.begin();
			//for(;it!=instance_set_.end();it++)
			//{
			//	Instance::p inst = *it;
			//	if(inst->resource_p_)
			//	{
			//		float bytes_rate =  100*float(inst->GetDownloadBytes())/inst->GetFileLength();
			//		STORAGE_DEBUG_LOG("resource size:"<<inst->GetFileLength()<<"bytes! download rate:"<<bytes_rate<<"%");
			//	}
			//}
			return;
		}
		if (pointer == res_info_timer_)
		{
			SaveResourceInfoToDisk();
			return;
		}
		/*assert(0);*/
	}

} // namespace storage
