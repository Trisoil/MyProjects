#include "stdafx.h"

#include "base\BlockMap.h"

#include "storage_base.h"
#include "storage/IStorage.h"
#include "storage/ResourceDescriptor.h"
#include "storage/PendingSubpieceManager.h"
#include "storage/resource.h"

#include "storage\FileResource.h"
#include "storage\instance.h"
#include "framework\mainThread.h"
#include "storage\Storage.h"


namespace storage
{
	FileResource::p FileResource::CreateResource(size_t file_length,tstring file_name,HANDLE file_handle,boost::shared_ptr<Instance> inst_p, size_t init_size)
	{
		assert(file_handle!=INVALID_HANDLE_VALUE);
		FileResource::p pointer  =  p(new FileResource( file_length,file_name,file_handle,inst_p, init_size));
		return pointer;
	}

	FileResource::p FileResource::ParseResource(ResourceDescriptor::p resource_desc_p,tstring file_name,HANDLE file_handle,boost::shared_ptr<Instance> inst_p, size_t actual_size)
	{
		assert(file_handle!=INVALID_HANDLE_VALUE);
		FileResource::p pointer  =  p(new FileResource( resource_desc_p,file_name,file_handle,inst_p, actual_size));
		return pointer;
	}

	FileResource::FileResource(size_t file_length,tstring file_name,HANDLE file_handle,boost::shared_ptr<Instance> inst_p, size_t init_size)
		:Resource(file_length,file_name,inst_p, init_size),file_handle_(file_handle)
	{
	}

	FileResource::FileResource(ResourceDescriptor::p resource_desc_p,tstring file_name,HANDLE file_handle,boost::shared_ptr<Instance> inst_p, size_t actual_size)
		:Resource(resource_desc_p,file_name,inst_p, actual_size),file_handle_(file_handle)
	{
	}

	Buffer FileResource::ReadBuffer(const size_t startpos, const size_t length)
	{
		assert(file_handle_!=INVALID_HANDLE_VALUE);
		assert(startpos+length<=file_resource_desc_p_->rid_info_.file_length_);

		Buffer ret_buf(length);
		SetFilePointer(file_handle_,startpos,NULL,FILE_BEGIN);
		DWORD readlen = 0;
		ReadFile(file_handle_,ret_buf.data_.get(),ret_buf.length_,&readlen,NULL);
		assert(readlen==ret_buf.length_);
		return ret_buf;
	}

	void   FileResource::WriteBuffer(const size_t startpos, const Buffer & buffer)
	{
		assert(file_handle_!=INVALID_HANDLE_VALUE);
		assert(startpos+buffer.length_<=file_resource_desc_p_->rid_info_.file_length_);

        if (startpos + buffer.length_ > actual_size_)
        {
            /* extend file */
            size_t new_size = min(actual_size_ + 2 * 1024 * 1024, 
                file_resource_desc_p_->rid_info_.file_length_);
            SetFilePointer(file_handle_, new_size, NULL, FILE_BEGIN);
            if (SetEndOfFile(file_handle_) == 0)
            {
                /* no disk space ???? .... */
                return;
            }
            MainThread::IOS().post(boost::bind(&SpaceManager::OnAllocDiskSpace, 
                Storage::Inst_Storage()->GetSpaceManager(), new_size - actual_size_));
            actual_size_ = new_size;
        }

		SetFilePointer(file_handle_,startpos,NULL,FILE_BEGIN);
		DWORD write_len = 0;
		WriteFile(file_handle_,buffer.data_.get(),buffer.length_,&write_len,NULL);
		assert(write_len==buffer.length_);
		need_saveinfo_to_disk_ = true;
		return ;
	}

	void   FileResource::Erase(const size_t startpos, const size_t length)
	{
		return;
	}

	void FileResource::FlushStore()
	{
		//assert(file_handle_!=INVALID_HANDLE_VALUE);
		if (file_handle_ == INVALID_HANDLE_VALUE)
		{
			STORAGE_ERR_LOG("file handle invalid");
			return;
		}
		FlushFileBuffers(file_handle_);
	}

	void FileResource::CloseResource()
	{
		if(is_running_==false) return ;
		is_running_ = false;
		need_saveinfo_to_disk_ = true;
		SecSaveResourceFileInfo();
		::CloseHandle(file_handle_);
		file_handle_ = INVALID_HANDLE_VALUE;
		if(instance_p_)
		{
			MainThread::IOS().post(
				boost::bind( &Instance::OnResourceCloseFinish,instance_p_,shared_from_this())
				);
			instance_p_ = Instance::p();
			return;
		}
		MainThread::IOS().post(
			boost::bind( &SpaceManager::OnRemoveResourceFinish,Storage::Inst_Storage()->GetSpaceManager(),shared_from_this())
			);
	}

	size_t FileResource:: FreeDiskSpace()
	{
		assert(is_running_==false);
		assert(file_handle_==INVALID_HANDLE_VALUE);
		if(!::DeleteFile(file_name_.c_str()))
		{
			return 0;
		}
		tstring cfg_file_name = file_name_+cfg_extname;
		::DeleteFile(cfg_file_name.c_str());
		return actual_size_;
	}

	//void FileResource::RemoveResource()
	//{
	//	if(is_running_==false) return ;
	//	is_running_ = false;
	//	::CloseHandle(file_handle_);
	//	file_handle_ = INVALID_HANDLE_VALUE;
	//	::DeleteFile(file_name_.c_str());
	//	tstring cfg_file_name = file_name_+cfg_extname;
	//	::DeleteFile(cfg_file_name.c_str());
	//	if(instance_p_)
	//	{
	//		MainThread::IOS().post(
	//			boost::bind( &Instance::OnResourceCloseFinish,instance_p_,shared_from_this())
	//			);
	//		instance_p_ = Instance::p();
	//		return;
	//	}
	//	MainThread::IOS().post(
	//		boost::bind( &SpaceManager::OnRemoveResourceFinish,Storage::Inst_Storage()->GetSpaceManager(),shared_from_this())
	//		);
	//}

	size_t FileResource::GetLocalFileSize()
	{ 
		assert(is_running_==false);
		return actual_size_; 
	};

	bool   FileResource::TryRenameToNormalFile()
	{
		assert(file_handle_!=INVALID_HANDLE_VALUE);

		int pos = file_name_.rfind(tpp_extname);

		if(pos==tstring::npos)
		{
			return false;
		}

		assert(pos==file_name_.size()-4);
		tstring new_file_name(file_name_.c_str(),pos);
		FlushStore();
		::CloseHandle(file_handle_);
		file_handle_ = INVALID_HANDLE_VALUE;

		tstring cfg_file = file_name_+cfg_extname;
		::MoveFileEx(file_name_.c_str(),new_file_name.c_str(),MOVEFILE_REPLACE_EXISTING);
		STORAGE_DEBUG_LOG("old filename:"<<framework::w2b(file_name_)<<" new_file_name: "<<framework::w2b(new_file_name));
		file_name_ = new_file_name;
		::DeleteFile(cfg_file.c_str());

		DWORD dwcreation = OPEN_EXISTING;
		DWORD dwdesiredaccess = GENERIC_READ;
		DWORD dwshare =FILE_SHARE_READ;
		file_handle_ = ::CreateFile(file_name_.c_str(),dwdesiredaccess,dwshare,NULL,dwcreation,FILE_ATTRIBUTE_NORMAL,NULL);
		//万一.......如何处理？
		assert(file_handle_!=INVALID_HANDLE_VALUE);
		return true;
	}

	bool   FileResource::TryRenameToTppFile()
	{
		assert(file_handle_!=INVALID_HANDLE_VALUE);

		int pos = file_name_.rfind(tpp_extname);

		if(pos!=tstring::npos)
		{
			assert(pos==file_name_.size()-4);
			return false;
		}

		tstring new_file_name = file_name_;
		new_file_name.append(tpp_extname);
		::CloseHandle(file_handle_);
		::MoveFileEx(file_name_.c_str(),new_file_name.c_str(),MOVEFILE_REPLACE_EXISTING);
		STORAGE_DEBUG_LOG("old filename:"<<framework::w2b(file_name_)<<"  new_file_name: "<<framework::w2b(new_file_name));
		
		file_name_ = new_file_name;

		DWORD dwcreation = OPEN_EXISTING;
		DWORD dwdesiredaccess = GENERIC_READ|GENERIC_WRITE;
		DWORD dwshare =FILE_SHARE_READ;
		file_handle_ = ::CreateFile(file_name_.c_str(),dwdesiredaccess,dwshare,NULL,dwcreation,FILE_ATTRIBUTE_NORMAL,NULL);
		//万一.......如何处理？
		assert(file_handle_!=INVALID_HANDLE_VALUE);
		return true;
	}

	void FileResource::Rename( const tstring& newname)
	{
		if(file_handle_ == INVALID_HANDLE_VALUE)
		{
			return ;
		}
		FlushStore();
		if (FALSE == ::CloseHandle(file_handle_))
		{
			return;
		}

		do
		{
			if (FALSE == ::MoveFileEx(file_name_.c_str(), newname.c_str(), MOVEFILE_REPLACE_EXISTING))
			{
				STORAGE_WARN_LOG("非法文件名: ");
				break;
			}

			if (false == instance_p_->IsComplete())
			{
				tstring cfg_file = file_name_+cfg_extname;
				::DeleteFile(cfg_file.c_str());
				file_name_ = newname;
				SecSaveResourceFileInfo();
			}
			else
			{
				file_name_ = newname;
			}
		} while (false);

		// 打开新文件
		DWORD dwcreation = OPEN_EXISTING;
		DWORD dwdesiredaccess = GENERIC_READ|GENERIC_WRITE;
		DWORD dwshare =FILE_SHARE_READ;
		file_handle_ = ::CreateFile(file_name_.c_str(),dwdesiredaccess,dwshare,NULL,dwcreation,FILE_ATTRIBUTE_NORMAL,NULL);

		assert(file_handle_!=INVALID_HANDLE_VALUE);
		return;
	}
}








