#include "stdafx.h"

#pragma  comment(lib, "shlwapi.lib")

#include <shlwapi.h>
#include "framework\io\Path.h"
#include "storage/Storage.h"
#include "storage/storagethread.h"

#include "base/base.h"
#include "storage_base.h"
#include "base\BlockMap.h"
#include "framework/timer/Timer.h"

#include "storage/IStorage.h"
#include "storage/ResourceDescriptor.h"
#include "storage/PendingSubpieceManager.h"
#include "storage/cfgfile.h"
#include "storage/resource.h"
#include "storage/FileResource.h"
#include "storage/Instance.h"

#include "storage/MemResource.h"	
namespace storage
{
	SpaceManager::p SpaceManager::Create(tstring stor_path,__int64 space_size,bool b_use_disk)
	{
		return SpaceManager::p(new SpaceManager(stor_path,space_size,b_use_disk));
	}

	SpaceManager::SpaceManager(tstring store_path,__int64 space_size,bool b_use_disk):store_size_(space_size),b_use_disk_(b_use_disk)
	{
		//need_space_size_ = 0;
		will_be_free_space_size_ = 0;
		curr_resource_files_total_size_ = 0;
		free_size_ = space_size;

		store_path_ = store_path;
		TCHAR old_dir[MAX_PATH];
		DWORD pathname_len = GetCurrentDirectory(MAX_PATH,old_dir);
		assert(pathname_len<MAX_PATH);
		OpenAndCreateStoreDir();
		SetCurrentDirectory(old_dir);

		IniFile ini;
		ini.SetSection(TEXT("Storage"));
		file_size_max_ = ini.GetInteger(TEXT("ResourceFileSizeMax"),200*1024)*1024*1024;
		file_size_min_ = ini.GetInteger(TEXT("ResourceFileSizeMin"),100)*1024;
	
	}
	__int64 SpaceManager::GetDirectoryAvialbeSize(tstring DirPathName)
	{
		ULARGE_INTEGER ulDiskFreeSize;
		ulDiskFreeSize.QuadPart = 0;

		if(DirPathName.size() < 3)
		{
			return ulDiskFreeSize.QuadPart;
		}

		TCHAR DiskName[4];
		memcpy(DiskName,DirPathName.c_str(),sizeof(TCHAR)*3);
		DiskName[3] = 0;


		if(!::GetDiskFreeSpaceEx(DiskName, &ulDiskFreeSize, NULL,NULL))
		{
			ulDiskFreeSize.QuadPart = 0;
			return ulDiskFreeSize.QuadPart;
		}

		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = INVALID_HANDLE_VALUE;

		tstring tmp_find_str(DirPathName);
		tmp_find_str.append(TEXT("\\*"));

		hFind = ::FindFirstFile(tmp_find_str.c_str(), &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE) 
		{
			return ulDiskFreeSize.QuadPart;
		} 

		tstring cfilename(FindFileData.cFileName);
		assert((cfilename.size()==1)&&(cfilename.find(TEXT('.'))==0));

		while (::FindNextFile(hFind, &FindFileData) != 0) 
		{
			cfilename.assign(FindFileData.cFileName);
			if((cfilename.size()==2)&&(cfilename.find(TEXT(".."))==0))
				continue;
			ulDiskFreeSize.QuadPart += FindFileData.nFileSizeLow;
			ulDiskFreeSize.HighPart += FindFileData.nFileSizeHigh;
		}
		::FindClose(hFind);
		return ulDiskFreeSize.QuadPart;
	}

	void SpaceManager::OnRemoveResourceFinish(Resource::p resource_p)
	{
		STORAGE_DEBUG_LOG(" file_name:"<<resource_p->GetLocalFileName()<<" file_size:"<<resource_p->GetLocalFileSize());

		size_t filesize = 	resource_p->FreeDiskSpace();

		if(removing_fileresource_set_.find(resource_p)==removing_fileresource_set_.end())
		{
			STORAGE_ERR_LOG("removing_fileresource_set_中未发现已经被删除的Resouce！");
			return;
		}
		if(filesize!=0)
		{
			removing_fileresource_set_.erase(resource_p);
			free_size_ += filesize;
			curr_resource_files_total_size_ -= filesize;
			will_be_free_space_size_-=filesize;
		}
		else
		{
			STORAGE_ERR_LOG("Delete file error!");
		}
		DiskSpaceMaintain();
	}

	bool SpaceManager::CancelResourceRequest(Instance::p inst_p)
	{
		std::set<Instance::p>::iterator it = pending_instance_need_resource_set.find(inst_p);
		if(it!=pending_instance_need_resource_set.end())
		{
			pending_instance_need_resource_set.erase(inst_p);
			//need_space_size_-= inst_p->GetResourceLength();
			return true;
		}
		return false;
	}

	void  SpaceManager::GetDirFileNameList(std::set<tstring> &filename_list)
	{
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		DWORD dwError;

		tstring tmp_find_str = store_path_;
		if(tmp_find_str.rfind(_T("PPLiveVAShareFlv"))==tstring::npos)
		{
			return;
		}
		tmp_find_str.append(TEXT("*"));

		hFind = FindFirstFile(tmp_find_str.c_str(), &FindFileData);

		if (hFind == INVALID_HANDLE_VALUE) 
		{
			STORAGE_ERR_LOG("Invalid file handle. Error is "<<::GetLastError());
			return ;
		} 

		tstring cfilename(FindFileData.cFileName);
		assert((cfilename.size()==1)&&(cfilename.find(TEXT('.'))==0));

		while (FindNextFile(hFind, &FindFileData) != 0) 
		{
			cfilename.assign(FindFileData.cFileName);
			if((cfilename.size()==2)&&(cfilename.find(TEXT(".."))==0))
				continue;
			filename_list.insert(store_path_+cfilename); 
		}
		dwError = ::GetLastError();
		FindClose(hFind);
		if (dwError != ERROR_NO_MORE_FILES) 
		{
			STORAGE_ERR_LOG("FindNextFile error. Error is "<<dwError);
		}
		return;
	}


	void SpaceManager::AddToRemoveResourceList(Resource::p resource_p,size_t disk_file_size)
	{
		assert(resource_p);
		assert(removing_fileresource_set_.find(resource_p)==removing_fileresource_set_.end());
		removing_fileresource_set_.insert(resource_p);
		will_be_free_space_size_+=disk_file_size;
	}

	void SpaceManager::DiskSpaceMaintain()
	{
		__int64 curr_free_disk_size = 0;
		if(!GetDiskFreeSpace(store_path_,curr_free_disk_size))
		{
			STORAGE_ERR_LOG("DiskSpaceMaintain::GetDiskFreeSpace error!");
			return;
		}
		__int64 min_free_size = curr_free_disk_size<free_size_?curr_free_disk_size:free_size_;
		if (min_free_size <= 0)
		{
			Storage::Inst_Storage()->RemoveOneInstance();
		}

		Instance::p resource_inst;
		std::set<Instance::p>::iterator it = pending_instance_need_resource_set.begin();
		for(;it!=pending_instance_need_resource_set.end();)
		{
			Instance::p resource_inst = (*it);

			if(resource_inst->GetStatus()!=INSTANCE_APPLY_RESOURCE)
			{
                STORAGE_DEBUG_LOG("SpaceManager::DiskSpaceMaintain() resource_inst->GetStatus()!=INSTANCE_APPLY_RESOURCE ");
				//被停止、融合，不需要了
				it = pending_instance_need_resource_set.erase(it);
				continue;
			}

            size_t init_size = min(2 * 1024 * 1024, resource_inst->GetResourceLength());
            if(init_size >will_be_free_space_size_+min_free_size)
            {
                STORAGE_EVENT_LOG("need_space_size_("<<init_size<<") will_be_free_space_size_("<<will_be_free_space_size_<<") free_size_("<<free_size_<<")"
                    <<" curr_free_disk_size ("<<curr_free_disk_size<<")");
                Storage::Inst_Storage()->RemoveOneInstance();			
            }

            /* 较小的初始大小，自动扩大 */
            STORAGE_DEBUG_LOG("SpaceManager::DiskSpaceMaintain() curr_free_disk_size = " << curr_free_disk_size << ", init_size = " << init_size << ", free_size_ = " << free_size_ << ", reult: (curr_free_disk_size>=init_size)&&(free_size_>=init_size) " << (curr_free_disk_size>=init_size)&&(free_size_>=init_size));
			if((curr_free_disk_size>=init_size)&&(free_size_>=init_size))
			{
				if(OnCreateResource(resource_inst, init_size))
				{
					STORAGE_DEBUG_LOG(" OnCreateResource success free_size:"<<free_size_<<" instance-size:"<<init_size);
					free_size_ -= init_size;
					curr_resource_files_total_size_ += init_size;
					it = pending_instance_need_resource_set.erase(it);
					continue;
				}
			}
			//看看能不能满足其他instance的需求
			it++;
		} // for
	}

	Resource::p SpaceManager::OpenResource(FileResourceInfo resource_info)
	{
		HANDLE resource_file_handle = INVALID_HANDLE_VALUE;
		ResourceDescriptor::p resource_map;
        size_t actual_size;
		if(b_use_disk_)
		{
			if(OpenResourceFile(resource_info,resource_file_handle,resource_map, actual_size))
			{
				FileResource::p resource_p = FileResource::ParseResource(resource_map,resource_info.file_path_,resource_file_handle,Instance::p(), actual_size);
				if(resource_p)
				{
					free_size_ -= actual_size;
					curr_resource_files_total_size_ += actual_size;
					return resource_p;
				}
			}
		}
		tstring cfgfilename = resource_info.file_path_+cfg_extname;
		::DeleteFile(resource_info.file_path_.c_str());
		::DeleteFile(cfgfilename.c_str());
		return Resource::p();
	}

	void SpaceManager::RequestResource(Instance::p resource_inst)
	{
		size_t resource_length = resource_inst->GetResourceLength();
		STORAGE_DEBUG_LOG(" free_size:"<<free_size_<<" instance-size:"<<resource_length);
		assert(pending_instance_need_resource_set.find(resource_inst)==pending_instance_need_resource_set.end());
		pending_instance_need_resource_set.insert(resource_inst);
		//need_space_size_ += resource_length;
		DiskSpaceMaintain();
		return ; 
	}

	bool SpaceManager::OnCreateResource(Instance::p resource_inst, size_t init_size)
	{
		if(!b_use_disk_)
		{
            STORAGE_DEBUG_LOG("b_use_disk_ = " << b_use_disk_);
			return false;
		}
		assert(resource_inst->GetStatus()==INSTANCE_APPLY_RESOURCE);
		tstring filename = resource_inst->GetResourceName();
		assert(filename.size()>0);
		size_t file_size = resource_inst->GetResourceLength();
		assert(file_size>0);
		STORAGE_DEBUG_LOG(" try create resource_name_"<<framework::w2b(filename));
		tstring full_file_name = store_path_+filename;

		tstring last_filename;
		HANDLE file_handle  = TryCreateFile(full_file_name,last_filename,init_size);
		if(file_handle!=INVALID_HANDLE_VALUE)
		{
			STORAGE_DEBUG_LOG(" success! full_file_name"<<framework::w2b(full_file_name));
			Resource::p resource_p = FileResource::CreateResource(file_size,last_filename,file_handle,Instance::p(), init_size);
			assert(resource_p);
			Storage::Inst_Storage()->OnCreateResourceSuccess(resource_p,resource_inst);
			return true;
		}
		STORAGE_DEBUG_LOG(" fail! full_file_name"<<framework::w2b(full_file_name));
		return false;
	}

	bool SpaceManager::OpenResourceFile(FileResourceInfo resource_info,HANDLE &resource_file_handle,ResourceDescriptor::p &resource_map, size_t &actual_size)
	{
		HANDLE hf = INVALID_HANDLE_VALUE;
		DWORD dwcreation = OPEN_EXISTING;
		DWORD dwdesiredaccess = GENERIC_READ|GENERIC_WRITE;
		DWORD dwshare =FILE_SHARE_READ;//|FILE_SHARE_WRITE
		tstring cfgfilename = resource_info.file_path_+cfg_extname;

		if(!::PathFileExists(resource_info.file_path_.c_str()))
		{
			return false;
		}

		LARGE_INTEGER file_size ;
		file_size.QuadPart = 0;
		resource_file_handle = ::CreateFile(resource_info.file_path_.c_str(),dwdesiredaccess,dwshare,NULL,dwcreation,FILE_ATTRIBUTE_NORMAL,NULL);
		if(resource_file_handle==INVALID_HANDLE_VALUE)
		{
			return false;
		}

		if(GetFileSizeEx(resource_file_handle,&file_size)==false)
		{
			::CloseHandle(resource_file_handle);
			return false;
		}

        /* XXX: no more than 4GB, right? */
        actual_size = file_size.LowPart;

        if(resource_info.file_path_.rfind(tpp_extname)==tstring::npos)
        { 
            if(resource_info.rid_info_.file_length_ == actual_size)
            {
                resource_map = ResourceDescriptor::Create(actual_size, true);
                return true;
            }
        }
        else
        {
            /* XXX: now file size is not equal to rid_info_.file_length_ */
            CfgFile cfg_file;
            if (cfg_file.SecOpen(resource_info.file_path_))
            {
                if(cfg_file.resource_file_name_==resource_info.file_path_)
                {
                    Buffer r_mapbuf = cfg_file.GetContent();
                    if(r_mapbuf.length_!=0)
                    {
                        RidInfo rid_info;
                        rid_info.InitByFileLength(cfg_file.resource_file_size_);
                        resource_map = ResourceDescriptor::Parse(r_mapbuf,rid_info);
                        if(resource_map)
                        {
                            return true;
                        }
                    }
                }
            }
            cfg_file.SecClose();
        }

		::CloseHandle(resource_file_handle);
		return false;
	}


	HANDLE SpaceManager::TryCreateFile(tstring filename,tstring &last_filename,size_t file_size)
	{
		HANDLE file_handle = INVALID_HANDLE_VALUE;
		DWORD dwcreation = CREATE_NEW;
		DWORD dwdesiredaccess = GENERIC_READ|GENERIC_WRITE;
		DWORD dwshare =FILE_SHARE_READ;//|FILE_SHARE_WRITE

		//剩余磁盘空间检查
		__int64 curr_free_disk_size = 0;
		if(GetDiskFreeSpace(store_path_,curr_free_disk_size))
		{
			if(curr_free_disk_size<__int64(file_size))
			{
				return INVALID_HANDLE_VALUE;
			}
		}
		else
		{
			return INVALID_HANDLE_VALUE;
		}

		framework::io::Path source_file_name;
		source_file_name.Assign(filename.c_str());//t_name);
		assert(source_file_name.GetString().length()>0);
		//扩展名
		tstring ext;
		LPCTSTR ext_p = source_file_name.GetFileExtension();
		if(ext_p)
		{
			ext.assign(TEXT("."));
			ext.append(ext_p);
		}



		
		//文件名
		source_file_name.RemoveFileExtension();
		tstring file_name(source_file_name.GetString());
		
		int i = 0;
		while(true)
		{
			if(!::PathFileExists(filename.c_str()))
			{
				tstring tppfilename = filename + tpp_extname;
				if(!::PathFileExists(tppfilename.c_str()))
				{
					break;
				}
			}
			//自动增量名
			char changestr[40];
			int changestr_size = sprintf_s(changestr,"(%d)",i++);
			string change_str(changestr,changestr_size);
			filename =file_name+framework::b2w(change_str)+ext;
		}
		last_filename = filename + tpp_extname;
		file_handle = ::CreateFile(last_filename.c_str(),dwdesiredaccess,dwshare,NULL,dwcreation,FILE_ATTRIBUTE_NORMAL,NULL);
		if(file_handle==INVALID_HANDLE_VALUE)
		{
			if(::GetLastError()!=123) //文件名不合法
			{
				STORAGE_EVENT_LOG("can not create file! filename:"<<framework::w2b(last_filename)<<" error code:"<< ::GetLastError());
				return INVALID_HANDLE_VALUE;
			}
			char changestr[40];
			int changestr_size = sprintf_s(changestr,"%x%x%s",framework::util::Random::GetGlobal().Next(), framework::util::Random::GetGlobal().Next(),".flv");
			string change_str(changestr,changestr_size);
			last_filename = store_path_ + framework::b2w(change_str);
			file_handle = ::CreateFile(last_filename.c_str(),dwdesiredaccess,dwshare,NULL,dwcreation,FILE_ATTRIBUTE_NORMAL,NULL);
			if(file_handle==INVALID_HANDLE_VALUE)
			{
				STORAGE_EVENT_LOG("can not create file! filename:"<<framework::w2b(last_filename)<<" error code:"<< ::GetLastError());
				return INVALID_HANDLE_VALUE;
			}
		}
		LARGE_INTEGER il;
		il.QuadPart = file_size;
		//增加文件长度
		if(::SetFilePointerEx(file_handle,il,NULL,FILE_BEGIN))
		{
			if(::SetEndOfFile(file_handle))
			{
				return file_handle;
			}
		}
		::CloseHandle(file_handle);
		file_handle = INVALID_HANDLE_VALUE;
		tstring tppfilename = filename + tpp_extname;
		::DeleteFile(tppfilename.c_str());
		return  file_handle;
	}


	bool SpaceManager::GetDiskFreeSpace(tstring path,__int64 &free_space_size)
	{
		framework::io::Path disk;
		disk.Assign(path.c_str());
		disk.RemoveFileSpec();

		__int64 i64FreeBytesToCaller, i64TotalBytes, i64FreeBytes;

		BOOL fResult = false;

		fResult = ::GetDiskFreeSpaceEx (disk.GetData(),
				(PULARGE_INTEGER)&i64FreeBytesToCaller,
				(PULARGE_INTEGER)&i64TotalBytes,
				(PULARGE_INTEGER)&i64FreeBytes);
		if(fResult)
		{
			free_space_size = i64FreeBytes;
			return fResult;
		}
		return fResult;
	}

	bool SpaceManager::OpenAndCreateStoreDir()
	{
		tstring dirname = store_path_;
		dirname.resize(dirname.size()-1);
		if(!::CreateDirectory(dirname.c_str(),NULL))
		{
			if(::GetLastError()==ERROR_ALREADY_EXISTS)
			{
				return true;
			}
			return false;
		}
		return true;
	}

    void SpaceManager::OnAllocDiskSpace( size_t alloc_space )
    {
        free_size_ -= alloc_space;
        curr_resource_files_total_size_ += alloc_space;
    }

};


