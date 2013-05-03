
#pragma once

/**
* @file
* @brief dll和模块相关的类
*/


#include "framework\io\path.h"

namespace framework
{
	namespace mswin
	{
		class Path;

		/// 封装win32模块操作的工具类
		class Module
		{
		public:
			explicit Module(HMODULE hModule = NULL) : handle_(hModule)
			{
			}
			~Module()
			{
			}

			/// 根据地址来查询此地址所属的模块句柄，参数为函数或静态变量的地址
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

			/// 关联到对应的地址所属的模块
			void AttachAddress(PVOID address)
			{
				handle_ = QueryModuleHandle(address);
				if (handle_ != NULL)
				{
					framework::io::Path path;
					assert(GetFilePath(path));
				}
			}

			/// 获得模块句柄
			HMODULE GetHandle() const { return handle_; }

			/// 模块是否有效
			bool IsValid() const { return handle_ != NULL; }

			/// 获取dll模块的导出项
			FARPROC GetExportItem(LPCSTR name)
			{
				if (!IsValid())
					return NULL;
				return ::GetProcAddress(GetHandle(), name);
			}


			/// 获取模块的文件名(包含完整路径)
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

			/// 获取模块的文件名(不包含路径)
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

			/// 获取模块的路径
			bool GetFileDirectory(framework::io::Path& result)
			{
				return GetFilePath(result) && result.RemoveFileSpec();
			}


			/// 构造跟模块同一目录下文件的完整路径(如: v:\\test.exe, 对应的结果为v:\\filename)
			bool BuildLocalFilePath(framework::io::Path& result, LPCTSTR filename)
			{
				return GetFileDirectory(result) && result.Append(filename);
			}

			/// 构造跟模块同一目录, 相同文件名, 不同扩展名的完整文件路径(如: v:\\test.exe, 对应的结果为v:\\test.ext)
			bool BuildLocalMainFilePath(framework::io::Path& result, LPCTSTR extName)
			{
				return GetFilePath(result) && result.RenameExtension(extName);
			}



		protected:
			/// 模块句柄
			HMODULE handle_;
		};


		/// 可装载的模块
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

			/// 关闭(释放)模块
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







