#include "stdafx.h"
#include <windows.h>
#include <shlwapi.h>
#include "storage_base.h"
#include "framework\io\Path.h"
#include "MappingFile.h"


namespace p2sp
{

	//MappingFile::p MappingFile::Create(tstring filename,size_t file_size)
	//{
	//	p pointer;
	//	if(!Check())
	//		return pointer;
	//	HANDLE hf = TryCreateFile(filename,file_size);
	//	if(hf==INVALID_HANDLE_VALUE)
	//		return pointer;
	//	pointer  = p(new MappingFile(hf,file_size));
	//	return pointer;
	//}
	//MappingFile::p MappingFile::Open(tstring filename,size_t file_size)
	//{
	//	p pointer;
	//	if(!Check())
	//		return pointer;
	//	HANDLE hf = TryOpenFile(filename,file_size);
	//	if(hf==INVALID_HANDLE_VALUE)
	//		return pointer;
	//	pointer  = p(new MappingFile(hf,file_size));
	//	return pointer;

	//}
	void MappingFile::Close()
	{
		if(mapping_buf_!=NULL)
		{
			UnmapViewOfFile(mapping_buf_);
			mapping_buf_ = NULL;
		}
		if(mappingfile_h_!=INVALID_HANDLE_VALUE)
		{
			CloseHandle(mappingfile_h_);
			mappingfile_h_ = INVALID_HANDLE_VALUE;
		}
		if(file_h_!=INVALID_HANDLE_VALUE)
		{
			CloseHandle(file_h_);
			file_h_ = INVALID_HANDLE_VALUE;
		}
	};


	bool MappingFile::Check(){return true;};

	HANDLE MappingFile::TryOpenFile(IN OUT tstring filename,size_t file_size)
	{
		DWORD dwcreation = OPEN_EXISTING;
		DWORD dwdesiredaccess = GENERIC_READ|GENERIC_WRITE;
		DWORD dwshare =NULL;// FILE_SHARE_READ|FILE_SHARE_WRITE

		HANDLE hf = CreateFile(filename.c_str(),dwdesiredaccess,dwshare,NULL,dwcreation,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hf==INVALID_HANDLE_VALUE)
		{
			STORAGE_EVENT_LOG("can not Open file! filename:");//<<filename<<" error code:"<< GetLastError());
			return hf;
		}
		size_t fsize = 0;
		if(GetFileSizeEx(hf,(PLARGE_INTEGER)&fsize))
		{
			if(fsize==file_size)
			{
				return hf;
			}
			STORAGE_ERR_LOG("file size unmatch! filename:"<<filename);
		}
		CloseHandle(hf);

		return INVALID_HANDLE_VALUE;
	}

	HANDLE MappingFile::TryCreateFile(IN OUT tstring &filename,size_t file_size)
	{
		HANDLE hf = INVALID_HANDLE_VALUE;
		DWORD dwcreation = CREATE_NEW;
		DWORD dwdesiredaccess = GENERIC_READ|GENERIC_WRITE;
		DWORD dwshare =NULL;// FILE_SHARE_READ|FILE_SHARE_WRITE

		if(PathFileExists(filename.c_str()))
		{
			Path source_file_name;
			source_file_name.Assign(filename.c_str());//t_name);
			assert(source_file_name.GetString().length()>0);

			tstring ext(source_file_name.GetFileExtension());
			source_file_name.RemoveFileExtension();
			tstring file_name(source_file_name.GetString());
			tstring change_str(TEXT("(0)"));
			int i = 0;
			while (i<10)
			{
				filename =file_name+change_str+ext;
				if(!PathFileExists(filename.c_str()))
					break;
				i++;
				change_str[1] = 30+i;
			}
			if(i>=10)
				return hf;
		}

		hf = CreateFile(filename.c_str(),dwdesiredaccess,dwshare,NULL,dwcreation,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hf==INVALID_HANDLE_VALUE)
		{
			STORAGE_EVENT_LOG("can not create file! filename:"<<filename<<" error code:"<< GetLastError());
			return hf;
		}
		size_t freesize = 0;
		if(GetDiskFreeSpace(filename,freesize))
		{
			if(freesize>file_size)
			{
				//增加文件长度
				SetFilePointerEx(hf,)

				return hf;
			}
		}
		CloseHandle(hf);
		return  INVALID_HANDLE_VALUE;
	}

	bool MappingFile::GetDiskFreeSpace(tstring path,size_t &free_space_size)
	{
		typedef BOOL (WINAPI *PGETDISKFREESPACEEX)(LPCSTR,
			PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);

		Path disk;
		disk.Assign(path.c_str());
		disk.RemoveFileSpec();

		PGETDISKFREESPACEEX pGetDiskFreeSpaceEx;
		__int64 i64FreeBytesToCaller, i64TotalBytes, i64FreeBytes;

		DWORD dwSectPerClust, 
			dwBytesPerSect, 
			dwFreeClusters, 
			dwTotalClusters;

		BOOL fResult = false;

		pGetDiskFreeSpaceEx = (PGETDISKFREESPACEEX) GetProcAddress( 
			GetModuleHandle(TEXT("kernel32.dll")),
			"GetDiskFreeSpaceExA");

		if (pGetDiskFreeSpaceEx)
		{
			fResult = GetDiskFreeSpaceEx (disk.GetData(),
				(PULARGE_INTEGER)&i64FreeBytesToCaller,
				(PULARGE_INTEGER)&i64TotalBytes,
				(PULARGE_INTEGER)&i64FreeBytes);
			if(fResult)
			{
				free_space_size = i64FreeBytes;
				return fResult;
			}
		}

		//fResult = GetDiskFreeSpaceA (disk.GetData(), 
		//	&dwSectPerClust, 
		//	&dwBytesPerSect,
		//	&dwFreeClusters, 
		//	&dwTotalClusters);

		//// Process GetDiskFreeSpace results.
		//if(fResult) 
		//{
		//	free_space_size = dwFreeClusters*dwSectPerClust*dwBytesPerSect;
		//}
		return fResult;
	}


	MappingFile::MappingFile(string filename,size_t file_size,int resource_type)	
		:ResourceFile(filename,file_size,resource_type)
	{
		file_h_ = TryCreateFile(filename,file_size);
		if(file_h_==INVALID_HANDLE_VALUE)
		//		return pointer;
		//	pointer  = p(new MappingFile(hf,file_size));

		file_h_ = file_handle;
		mappingfile_h_= CreateFileMapping(file_handle,NULL,PAGE_READWRITE,0,file_size,NULL);
		if(mappingfile_h_==INVALID_HANDLE_VALUE)
		{
			STORAGE_EVENT_LOG("Create file mapping error! errorcode:"<<GetLastError());
			assert( 0);
		}
		LPVOID mapping_buf_ = MapViewOfFile(mappingfile_h_,FILE_MAP_READ|FILE_MAP_WRITE,0,0,file_size);
	}

};

