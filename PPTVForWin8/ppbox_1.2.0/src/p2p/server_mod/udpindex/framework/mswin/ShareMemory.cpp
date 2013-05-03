#include "stdafx.h"
#include "framework/mswin/ShareMemory.h"

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>

#define APP_DEBUG(s) LOG(__DEBUG, "app", s);
#define APP_INFO(s) LOG(__INFO, "app", s);
#define APP_EVENT(s) LOG(__EVENT, "app", s);
#define APP_WARN(s) LOG(__WARN, "app", s);
#define APP_ERROR(s) LOG(__ERROR, "app", s);

namespace framework
{
	namespace mswin
	{
		void ShareMemory::Create(LPCSTR name, DWORD size)
		{
			assert(!IsValid());
			APP_DEBUG("Create File Mapping: size=" << size << ", name=" << name);
			m_Handle = ::CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, name);
			if (!IsValid())
			{
				DWORD errcode = ::GetLastError();
				APP_DEBUG("CreateFileMapping failed " << boost::make_tuple(errcode, size) << " " << name);
				return;
			}
			Map(FILE_MAP_ALL_ACCESS);
		}

		void ShareMemory::Close()
		{
			Unmap();
			if (IsValid())
			{
				::CloseHandle(m_Handle);
				m_Handle = NULL;
			}
		}

		void ShareMemory::Map(DWORD access, DWORD pos, DWORD size)
		{
			assert(IsValid() && !IsMapped());
			m_View = ::MapViewOfFile(GetHandle(), access, 0, pos, size);
			if (IsMapped())
				return;
			DWORD errcode = ::GetLastError();
			APP_ERROR("MapViewOfFile failed " << GetHandle() << " " << errcode << " " << boost::make_tuple(pos, size));
		}

		void ShareMemory::Unmap()
		{
			if (IsMapped())
			{
				::UnmapViewOfFile(m_View);
				m_View = NULL;
			}
		}

		bool ShareMemory::DoCreate(HANDLE hFile, DWORD protectFlags, DWORD size, LPCSTR name, DWORD access)
		{
			assert(!IsValid());
			APP_DEBUG("Create File Mapping: size=" << size << " file=" << hFile);
			m_Handle = ::CreateFileMappingA(hFile, NULL, protectFlags, 0, size, name);
			if (!IsValid())
			{
				DWORD errcode = ::GetLastError();
				APP_DEBUG("CreateFileMapping failed " << boost::make_tuple(errcode, size) << " " << name);
				return false;
			}
			Map(access);
			return IsMapped();
		}

		bool ShareMemory::MapFile(HANDLE hFile, DWORD size, DWORD flProtect, DWORD access)
		{
			return this->DoCreate(hFile, flProtect, size, NULL, access);
		}

		bool ShareMemory::Open(LPCSTR name, DWORD access)
		{
			assert(!IsValid());
			APP_DEBUG("Create File Mapping: name=" << name);
			m_Handle = ::OpenFileMappingA(access, FALSE, name);
			if (!IsValid())
			{
				DWORD errcode = ::GetLastError();
				APP_DEBUG("OpenFileMapping failed " << boost::make_tuple(errcode, access) << " " << name);
				return false;
			}
			Map(access);
			return IsMapped();
		}
	}
}