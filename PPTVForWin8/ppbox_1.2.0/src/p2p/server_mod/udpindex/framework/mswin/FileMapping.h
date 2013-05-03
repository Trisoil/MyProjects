
#pragma once

/**
* @file
* @brief 内存文件映射相关的类
*/
#define APP_INFO(message)
#define APP_EVENT(message)
#define APP_WARN(message)
#define APP_ERROR(message)
#define APP_DEBUG(message)

namespace framework
{
	namespace mswin
	{
		// 封装文件映射操作的工具类
		class FileMapping
		{
		public:
			FileMapping() : m_Handle(NULL), m_View(NULL)
			{
			}
			~FileMapping()
			{
				Close();
			}

			void Create(LPCTSTR name, DWORD size);

#if !defined(_WIN32_WCE)
			bool Open(LPCTSTR name, DWORD access = FILE_MAP_READ)
			{
				assert(!IsValid());
				APP_DEBUG("Create File Mapping: name=" << name);
				m_Handle = ::OpenFileMapping(access, FALSE, name);
				if (!IsValid())
				{
					DWORD errcode = ::GetLastError();
					APP_DEBUG("OpenFileMapping failed " << make_tuple(errcode, access) << " " << name);
					return false;
				}
				Map(access);
				return IsMapped();
			}
#endif

			bool MapFile(HANDLE hFile, DWORD size, DWORD flProtect = PAGE_READONLY, DWORD access = FILE_MAP_READ)
			{
				return this->DoCreate(hFile, flProtect, size, NULL, access);
			};

			bool DoCreate(HANDLE hFile, DWORD protectFlags, DWORD size, LPCTSTR name, DWORD access)
			{
				assert(!IsValid());
				APP_DEBUG("Create File Mapping: size=" << size << " file=" << hFile);
				m_Handle = ::CreateFileMapping(hFile, NULL, protectFlags, 0, size, name);
				if (!IsValid())
				{
					DWORD errcode = ::GetLastError();
					APP_DEBUG("CreateFileMapping failed " << make_tuple(errcode, size) << " " << name);
					return false;
				}
				Map(access);
				return IsMapped();
			}

			void Close();

			void Map(DWORD access = FILE_MAP_READ, DWORD pos = 0, DWORD size = 0);
			void Unmap();

			HANDLE GetHandle() const	{ return m_Handle; }
			LPVOID GetView() const		{ return m_View; }
			bool IsValid() const		{ return GetHandle() != NULL; }
			bool IsMapped() const		{ return m_View != NULL; }

		private:
			HANDLE	m_Handle;
			LPVOID	m_View;
		};




		inline void FileMapping::Create(LPCTSTR name, DWORD size)
		{
			assert(!IsValid());
			APP_DEBUG("Create File Mapping: size=" << size << ", name=" << name);
			m_Handle = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, name);
			if (!IsValid())
			{
				DWORD errcode = ::GetLastError();
				APP_DEBUG("CreateFileMapping failed " << make_tuple(errcode, size) << " " << name);
				return;
			}
			Map(FILE_MAP_ALL_ACCESS);
		}



		inline void FileMapping::Close()
		{
			Unmap();
			if (IsValid())
			{
				::CloseHandle(m_Handle);
				m_Handle = NULL;
			}
		}

		inline void FileMapping::Map(DWORD access, DWORD pos, DWORD size)
		{
			assert(IsValid() && !IsMapped());
			m_View = ::MapViewOfFile(GetHandle(), access, 0, pos, size);
			if (IsMapped())
				return;
			DWORD errcode = ::GetLastError();
			APP_ERROR("MapViewOfFile failed " << GetHandle() << " " << errcode << " " << make_tuple(pos, size));
		}
		inline void FileMapping::Unmap()
		{
			if (IsMapped())
			{
				::UnmapViewOfFile(m_View);
				m_View = NULL;
			}
		}
	}
}



