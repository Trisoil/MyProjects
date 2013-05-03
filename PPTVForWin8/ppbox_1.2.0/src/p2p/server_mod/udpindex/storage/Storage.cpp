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

// 定义此行，则"ResourceInfo.dat"和"ResourceInfo.dat.bak"将会保存在ppva安装目录下
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
		BOOL		bUseDisk,			// 是否使用磁盘, 如果是TRUE,则使用磁盘; 如果是FALSE,则纯内存
		ULONGLONG	ullDiskLimit,		// 使用磁盘上限
		wstring		DiskPathName		// 磁盘使用路径
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

	// storage被通知instance关闭，storage取消instance的资源申请，释放资源空间
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
		
		//! 延迟退出？？条件增加标志位
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

	// 根据url_info从map中找到或创建一个instance
	IInstance::p Storage::CreateInstance(const UrlInfo& url_info, bool is_force)
	{
		if(is_running_==false) return IInstance::p();
		STORAGE_EVENT_LOG("CreateInstance!url: "<<url_info<<(is_force?"force!":""));

		map<string,Instance::p>::const_iterator it = url_instance_map_.find(url_info.url_);
		if(it!=url_instance_map_.end())
		{
			//找到一个Instance
			if(!is_force)
			{
				//非强迫
				STORAGE_EVENT_LOG("已存在 返回已有的instance "<<url_info<<(is_force?"force!":""));
				it->second->AddUrlInfo(url_info); //?
				return it->second;
			}
			//强迫性
			it->second->RemoveUrl(url_info.url_);
			url_instance_map_.erase(url_info.url_);
			STORAGE_EVENT_LOG("已存在 但是存下模式 创建新instance "<<url_info<<(is_force?"force!":""));
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

	// 将instance添加到rid_map，如果和map中的rid重复，则将map中的inst合并到pointer
	void Storage::AddInstanceToRidMap(Instance::p pointer)
	{
		if(is_running_==false) return;
		if(!pointer)
			return;
		STORAGE_EVENT_LOG("AddInstanceToRidMap!instance: "<<pointer);

		RID rid = pointer->GetRID();
		assert(!rid.IsEmpty()); // rid必须非空
		map<RID,Instance::p>::iterator iter = rid_instance_map_.find(rid);
		// rid重复
		if(iter!=rid_instance_map_.end())
		{
			// 重复则合并，合并到pointer中
			MergeInstance(pointer,iter->second);
			return ;
		}
		rid_instance_map_.insert(make_pair(rid,pointer));
		return;
	}

	// 将instance2融合到instance1
	void Storage::MergeInstance(Instance::p instance_p1,Instance::p instance_p2)
	{
		if(is_running_==false) return;
		
		// 更新Storage的url和rid列表，将原来与inst2匹配的url和rid删除，并与inst1匹配
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
            STORAGE_DEBUG_LOG("合并URL: " << in_url_info_s[i].url_ << " 到 instance: " << instance_p1);
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

		//将instance_p2资源拷贝到instance_p1
		instance_p1->Merge(instance_p2); // 只是打了个log，啥都没做
		instance_p2->BeMergedTo(instance_p1); // 完成合并，并通知DownloadDriver迁移
	}

	// instance向spacemanager申请资源
	void Storage::ApplyResource(Instance::p resource_inst)
	{
		if(is_running_==false) return;
		space_manager_->RequestResource(resource_inst);
	}

	// resource创建成功，绑定resource和instance, 将信息写入ResourceInfo文件中
	void Storage::OnCreateResourceSuccess(Resource::p resource_p,Instance::p instance_p)
	{
		assert(resource_p);
		assert(instance_p);
		instance_p->AttachResource(resource_p);
	}

	// 遍历instance_set_,将Instace信息保存到Resourceinfo，同时保存原资源信息到bak
	void Storage::SaveResourceInfoToDisk()
	{
		if(is_running_==false) return;
		int count = 0;
		ResourceInfoListFile r_file ;
		// 打开成功，原资源信息存盘

		if(r_file.SecOpen(resourceinfo_file_.c_str()))
		{
			r_file.SecClose();
			::MoveFileEx(resourceinfo_file_.c_str(),resourceinfo_bak_file_.c_str(),MOVEFILE_REPLACE_EXISTING);
		}
		STORAGE_TEST_DEBUG("Save Resource Info###########################>");
		//RELEASE_OUTPUT("Save Resource Info###########################>");
		//覆盖建立文件
		if(r_file.SecCreate( resourceinfo_file_.c_str()))
		{
			//遍历instance_set_,将Instace信息保存到Resourceinfo，并加上安全防护
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
					<<"文件名: "<<framework::w2b(r_info.file_path_)
					<<"\n\tRID: "<<r_info.rid_info_.rid_
					<<"\tLastPushTime: "<<r_info.last_push_time_
					<<"\t流量统计："<<tmp_traffic_s
					<<"Value: "<<inst->GetInstanceValue());
				STORAGE_TEST_DEBUG(""
					<<"文件名: "<<framework::w2b(r_info.file_path_)
					<<"\n\tRID: "<<r_info.rid_info_.rid_
					<<"\tLastPushTime: "<<r_info.last_push_time_
					<<"\t流量统计："<<tmp_traffic_s
					<<"Value: "<<inst->GetInstanceValue());
				r_file.AddResourceInfo(r_info);
				count++;
			}
		}

		//打不开，不尝试了
		r_file.SecClose();
		STORAGE_EVENT_LOG("SaveResourceInfoToDisk! resource count:"<<count);
	};

    bool Storage::CreateDirectoryRecursive(const tstring& directory)
    {
        STORAGE_EVENT_LOG("函数开始");
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
	// 装载磁盘资源信息，如果资源信息丢失，则删除所有文件
	void Storage::LoadResourceInfoFromDisk()
	{
		int count = 0;
		std::set<tstring> filename_list; // 包含完整路径

		space_manager_->GetDirFileNameList(filename_list);
		filename_list.erase(resourceinfo_file_);
		filename_list.erase(resourceinfo_bak_file_);
//STORAGE_TEST_DEBUG("Load Resource Info-------------------");
		//读取上次存放资源信息
		ResourceInfoListFile r_file;
		if(!r_file.SecOpen(resourceinfo_file_.c_str()))
		{
			r_file.SecClose();
			::DeleteFile(resourceinfo_file_.c_str());
			STORAGE_EVENT_LOG("LoadResourceInfoFromDisk!读取资源信息失败！");
            return ;
			::MoveFileEx(resourceinfo_bak_file_.c_str(),resourceinfo_file_.c_str(),MOVEFILE_REPLACE_EXISTING);
			//失败，读取备份
			if(!r_file.SecOpen(resourceinfo_file_.c_str()))
			{
				//都被破坏了
				STORAGE_EVENT_LOG("LoadResourceInfoFromDisk!读取资源信息备份失败");
				r_file.SecClose();
				::DeleteFile(resourceinfo_file_.c_str());
				for(std::set<tstring>::iterator it = filename_list.begin();it!=filename_list.end();it++)
				{
					::DeleteFile(it->c_str());
					STORAGE_EVENT_LOG("删除存储目录多余文件"<<framework::w2b(*it));
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
			// 获取resourceinfo
			// 检查RID一致性！
			if(!r_info.rid_info_.GetRID().IsEmpty()) // rid非空
			{
				// 有rid, 删除文件和cfg文件
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
			//检查url一致性！
			vector<UrlInfo>::iterator it_url = r_info.url_info_.begin();
			int url_count = 0;
			for(; it_url!=r_info.url_info_.end();it_url++)
			{
				// 如果有Url，不计数
				if(url_instance_map_.find((*it_url).url_)!=url_instance_map_.end())
				{
					continue;
				}
				url_count++;
			}

			// 有Url，删除文件和cfg文件
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

			//检查资源文件和长度
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
				<<" 已下载："<<pointer->GetDownloadBytes()<<"/"<<pointer->GetFileLength()<<" rid_info:"<<r_info.rid_info_);
			//
			filename_list.erase(r_info.file_path_);
			filename_list.erase(r_info.file_path_+cfg_extname);

			//创建Instance
			assert(pointer);
			pointer->Start();
			//添加Instance到Storage	
			AddInstanceToStorage(pointer);
			count++;
		}
		r_file.SecClose();

		for(std::set<tstring>::iterator it = filename_list.begin();it!=filename_list.end();it++)
		{
			::DeleteFile(it->c_str());
			STORAGE_EVENT_LOG("删除存储目录多余文件"<<framework::w2b(*it));
		}
		STORAGE_EVENT_LOG("SaveResourceInfoToDisk! resource count:"<<count);
	}

	// 返回rid对应的instance
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

	// 添加某个instance到rid_map, url_map和inst_set中
	bool Storage::AddInstanceToStorage(Instance::p pointer)
	{
		if(!pointer)
			return false;

		RID rid = pointer->GetRID();
		if(!rid.IsEmpty()) // rid非空
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

	// 查看url_info_s中的url所对应的instance是否是rid所对应的instance
	// 如果url不在url_instance_map中，则需要将url与rid_inst匹配，否则，若url_inst != rid_inst，
	// 且不是纯下载模式，且url_inst_rid != rid，则从map中和url_inst中删除该url
	void Storage::AttachHttpServerByRid(const RID& rid, const vector<UrlInfo>& url_info_s)
	{
		if(is_running_==false) return ;
		//此处的URL一般不是主URL，需要考虑维护RID，URL信息的一致性，
		//如果出现冲突，不考虑融合，留待AttachRidByUrl和Resource下载完成后的RID比较后融合
		map<RID,Instance::p>::const_iterator iter = rid_instance_map_.find(rid);
		if(iter==rid_instance_map_.end())
			return;
		Instance::p inst = iter->second;
		vector<UrlInfo>::const_iterator v_url = url_info_s.begin(); 

		vector<UrlInfo> in_url_info_s; //用于保存需要添加到inst里的url列表
		for(size_t i = 0; i<url_info_s.size();i++)
		{
			map<string,Instance::p>::iterator it = url_instance_map_.find(url_info_s[i].url_);
			if(it!=url_instance_map_.end())
			{
				if(it->second==inst) //正确、已存在
				{
					//in_url_info_s.push_back(url_info_s[i]);
					continue;
				}
				if(it->second->IsPureDownloadMode()) 
				{
					//冲突、但是是纯下载模式实例,丢弃该URL
					continue;
				}

				if(it->second->GetRID().IsEmpty())  //非纯下，但是没有RID
				{
					//不融合
					//MergeInstance(iter->second,inst1);
					continue;
				}

				//RID考虑
				if(rid!=it->second->GetRID())
				{
					//两个RID都有该url，更新新信息？？？？如果被删除url是DD的，怎么办？
					it->second->RemoveUrl(url_info_s[i].url_);
					url_instance_map_.erase(it);
				}
			}
			// url不在url_instance_map中，则需要将url与rid所对应的instance匹配
			in_url_info_s.push_back(url_info_s[i]);
			url_instance_map_.insert(make_pair(url_info_s[i].url_,inst));
		}

		if(in_url_info_s.size())
		{
			inst->AddUrlInfo(in_url_info_s);
		}
	}

	// 如果url在map中，instance set is_need_to_add
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

	// 获得了新的关于该url的文件名
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
		// instance尚未创建，保存到map中
		if (it == url_instance_map_.end())
		{
			url_filename_map_.insert(make_pair(url,file_name));
			return;
		}
		// instance已经创建，改名字
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

        // 检查文件名是否合法
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

	// 根据文件名判重，并返回最终文件名
	void Storage::DoVerifyName(tstring filename, tstring& lastname)
	{
		// filename带".flv"
		// file_name不带".flv"
        const static tstring sample = _T("CON|PRN|AUX|NUL|COM1|COM2|COM3|COM4|COM5|COM6|COM7|COM8|COM9|LPT1|LPT2|LPT3|LPT4|LPT5|LPT6|LPT7|LPT8|LPT9");

        tstring file_name = filename;
		tstring flvext = TEXT(".flv");
		size_t pos = file_name.find(flvext);
		file_name.erase(pos);

        // 检查文件名是否合法
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

        // 检测文件是否存在
		int i = 0;
		while(true)
		{
			// 不存在同名的文件
			if(!::PathFileExists(filename.c_str()))
			{
				// 转为tpp文件名
				tstring tppfilename = filename + tpp_extname;
				if(!::PathFileExists(tppfilename.c_str()))
				{
					break;
				}
			}
			// 如果存在同名文件，自动增量名
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
		// url没有对应的真实名字
		if (it == url_filename_map_.end())
		{
			return TEXT("");
		}
		return it->second;
	}

	// Url如果在url_map中已存在并且与RID信息不一致，则调用MergeInstance删除并重新建立新Instance
	void Storage::AttachRidByUrl(const string& url, const RidInfo& rid, MD5 ContentMD5, size_t ContentBytes, int flag) 
	{
		if(is_running_==false) return ;
        STORAGE_EVENT_LOG("Storage::AttachRidByUrl, URL: " << url << ", RID: " << rid);

        // 验证rid
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

		//此处的URL一般都是主URL，如果已存在并且与RID信息不一致，删除并重新建立新Instance
		map<string,Instance::p>::const_iterator it = url_instance_map_.find(url);
		if(it==url_instance_map_.end())
		{
			STORAGE_ERR_LOG("Storage::AttachRidByUrl, not such url:" << url);
			return ;
		}
		Instance::p inst1 = it->second;
		inst1->SetRidOriginFlag(flag);
		//是纯下模式
		if(inst1->IsPureDownloadMode())
        {
            STORAGE_DEBUG_LOG("Storage::AttachRidByUrl, IsPureDownloadMode");
			return;
        }

		//RID已存在，不更新RID
		//! 如何即时更新RID
		if(!inst1->GetRID().IsEmpty())
		{
            STORAGE_DEBUG_LOG("Storage::AttachRidByUrl, inst rid is not empty");
			return ;
		}

//         STORAGE_DEBUG_LOG("rid_instance_map_: " << rid_instance_map_.size());
//         for (map<RID, Instance::p>::iterator it = rid_instance_map_.begin(); it != rid_instance_map_.end(); ++it)
//             STORAGE_DEBUG_LOG("LOAD: " << it->first);
		
        // url_inst(inst1)的RID为空
		map<RID,Instance::p>::const_iterator iter = rid_instance_map_.find(rid.GetRID());

        //没有重复的，不需要融合,直接设置RidInfo
		if(iter == rid_instance_map_.end())
		{
            STORAGE_DEBUG_LOG("No such rid, no need Merge");

			if((inst1->GetFileLength()!=0)&&(inst1->GetFileLength()!=rid.file_length_))
			{
				//长度检查不符，返回！
				//inst1->SetPureDownloadMode(true);
				return;
			}

			//回头在考虑考虑！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
			// 将rid与url_inst匹配，并为url_inst添加rid信息
			rid_instance_map_.insert(make_pair(rid.GetRID(),inst1));
			inst1->SetRidInfo(rid, ContentMD5, ContentBytes);


		}
        else
        {
		    inst1->SetRidInfo(rid, ContentMD5, ContentBytes);

		    //需要考虑融合,简单策略，老的merge新的
		    MergeInstance(iter->second,inst1); // 将inst1合并到iter->second中
        }
	}

	// 获取rid_inst_map中的所有rid
	void Storage::GetLocalResources(set<RID>& rid_s)
	{
		if(is_running_==false) return ;
		for(map<RID,Instance::p>::const_iterator iter = rid_instance_map_.begin();iter!=rid_instance_map_.end();iter++)
		{
			rid_s.insert(iter->first);
		}
	}

	// 根据mod_number和group_count获取rid_inst_map中的rid
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

	// 从url对应的instance和url_map中删除某个Url
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

		inst->Remove(); // 删除instance
		return ;
	}

	// 从url和rid列表中删除某个instance，并添加到spacemanager的释放资源列表中
	void Storage::RemoveInstance(Instance::p inst)
	{
		STORAGE_INFO_LOG(""<<inst->GetResourceName()<<" disk_file_size:"<<inst->GetDiskFileSize());
		assert((inst->GetDDNum()==0));

		if(!inst)
			return;
		//RELEASE_OUTPUT("");
		STORAGE_TEST_DEBUG("\n将要被删除的文件："<<framework::w2b(inst->resource_name_)
			<<"\n\t文件大小："<<float(inst->GetFileLength())/1024/1024<<"MB"
			<<"\tRID: "<<inst->GetRID()
			<<"\tValue: "<<inst->GetInstanceValue()
			<<"\n");
		STORAGE_TEST_DEBUG("\n将要被删除的文件："<<framework::w2b(inst->resource_name_)
			<<"\n\t文件大小："<<float(inst->GetFileLength())/1024/1024<<"MB"
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

		inst->Remove(); // 删除instance
		return ;
	}

	// 从instance_set中找一个instance删除
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
				// inst_rid为空
                if (inst_p->GetRID().IsEmpty())
                {
                    RemoveInstance(inst_p);
                    return;
                }
				if (inst_rm.get() == 0)
				{
					inst_rm = inst_p;
				}
				// inst_rid 非空
				float tmp_value = inst_p->GetInstanceValue();
				if (tmp_value >=0 && tmp_value <min_value)
				{
					min_value = tmp_value;
				}
				value_inst.insert(make_pair(tmp_value, inst_p));
            }
        } // 最外层for循环
		if (value_inst.find(min_value) != value_inst.end() && inst_minvalue.get() == 0)
		{
			inst_minvalue = value_inst.find(min_value)->second;
		}
		if (inst_minvalue.get() == 0 && inst_rm.get() == 0)
		{
			STORAGE_DEBUG_LOG("找不到应该删除的文件");
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
			//STORAGE_DEBUG_LOG("关键容器size"<<endl
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
