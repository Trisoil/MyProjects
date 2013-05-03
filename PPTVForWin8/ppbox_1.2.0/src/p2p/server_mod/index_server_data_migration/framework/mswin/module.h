
#pragma once

/**
* @file
* @brief dll��ģ����ص���
*/


#include "framework\io\path.h"

namespace framework
{
	namespace mswin
	{
		class Path;

		/// ��װwin32ģ������Ĺ�����
		class Module
		{
		public:
			explicit Module(HMODULE hModule = NULL) : handle_(hModule)
			{
			}
			~Module()
			{
			}

			/// ���ݵ�ַ����ѯ�˵�ַ������ģ����������Ϊ������̬�����ĵ�ַ
			static HMODULE QueryModuleHandle(PVOID address)
			{
				MEMORY_BASIC_INFORMATION meminfo = { 0 };
				const size_t sizeof_meminfo = sizeof(MEMORY_BASIC_INFORMATION);
				if (sizeof_meminfo != ::VirtualQuery(address, &meminfo, sizeof_meminfo))
				{
					return NULL;
				}
				return static_cast<HMODULE>(meminfo.AllocationBase);
			}

			/// ��������Ӧ�ĵ�ַ������ģ��
			void AttachAddress(PVOID address)
			{
				handle_ = QueryModuleHandle(address);
				if (handle_ != NULL)
				{
					framework::io::Path path;
					assert(GetFilePath(path));
				}
			}

			/// ���ģ����
			HMODULE GetHandle() const { return handle_; }

			/// ģ���Ƿ���Ч
			bool IsValid() const { return handle_ != NULL; }

			/// ��ȡdllģ��ĵ�����
			FARPROC GetExportItem(LPCSTR name)
			{
				if (!IsValid())
					return NULL;
				return ::GetProcAddress(GetHandle(), name);
			}


			/// ��ȡģ����ļ���(��������·��)
			bool GetFilePath(framework::io::Path& result)
			{
				TCHAR path[framework::io::Path::MAX_PATH_SIZE + 1] = { 0 };
				DWORD len = ::GetModuleFileName( handle_, path, framework::io::Path::MAX_PATH_SIZE );
				if (0 != len)
				{
					result.Assign(path);
					return true;
				}
				//TRACE(TEXT("GetModuleFileName failed %p %d\n"), handle_, ::GetLastError());
				result.Clear();
				assert(!"GetModuleFileName failed");
				return false;
			}

			/// ��ȡģ����ļ���(������·��)
			tstring GetFileName()
			{
				tstring result;
				framework::io::Path path;
				if (GetFilePath(path))
				{
					result = path.GetFileName();
				}
				return result;
			}
			//tstring GetBaseName();

			/// ��ȡģ���·��
			bool GetFileDirectory(framework::io::Path& result)
			{
				return GetFilePath(result) && result.RemoveFileSpec();
			}


			/// �����ģ��ͬһĿ¼���ļ�������·��(��: v:\\test.exe, ��Ӧ�Ľ��Ϊv:\\filename)
			bool BuildLocalFilePath(framework::io::Path& result, LPCTSTR filename)
			{
				return GetFileDirectory(result) && result.Append(filename);
			}

			/// �����ģ��ͬһĿ¼, ��ͬ�ļ���, ��ͬ��չ���������ļ�·��(��: v:\\test.exe, ��Ӧ�Ľ��Ϊv:\\test.ext)
			bool BuildLocalMainFilePath(framework::io::Path& result, LPCTSTR extName)
			{
				return GetFilePath(result) && result.RenameExtension(extName);
			}



		protected:
			/// ģ����
			HMODULE handle_;
		};


		/// ��װ�ص�ģ��
		class LoadableModule : public Module, private boost::noncopyable
		{
		public:
			LoadableModule()
			{
			}

			explicit LoadableModule(LPCTSTR filename)
			{
				handle_ = ::LoadLibrary(filename);
			}

			~LoadableModule()
			{
				Close();
			}

			bool Load(LPCTSTR filename)
			{
				assert(!IsValid());
				Close();
				handle_ = ::LoadLibrary(filename);
				return IsValid();
			}

			/// �ر�(�ͷ�)ģ��
			void Close()
			{
				if (IsValid())
				{
					::FreeLibrary(handle_);
					handle_ = NULL;
				}
			}
		};
	}
}







