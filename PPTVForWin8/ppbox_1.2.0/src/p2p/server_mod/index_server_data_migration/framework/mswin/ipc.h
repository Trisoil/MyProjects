
#pragma once


#include <tlhelp32.h>
#include <psapi.h>
#if defined(_WIN32_WCE)
//#pragma comment(lib, "toolhelp.lib")
#else
//#pragma comment(lib, "psapi.lib")
#endif

namespace framework
{
	namespace mswin
	{
		class ProcessObject
		{
		public:
			ProcessObject() : m_handle(::GetCurrentProcess()), m_id(::GetCurrentProcessId())
			{
			}
			explicit ProcessObject(DWORD id) : m_handle(::GetCurrentProcess()), m_id(::GetCurrentProcessId())
			{
				Open(id);
			}
			~ProcessObject()
			{
				Close();
			}

			void Close()
			{
				if (IsValid())
				{
					::CloseHandle(m_handle);
					m_handle = NULL;
					m_id = -1;
				}
			}
			bool IsValid() const
			{
				return m_handle != NULL;
			}

			bool Open(DWORD id)
			{
				Close();
				m_handle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);
				if (IsValid())
				{
					m_id = id;
					return true;
				}
				//APP_ERROR("OpenProcess " << id << " failed, error code=" << ::GetLastError());
				return false;
			}

			DWORD GetExitCode()
			{
				DWORD exitCode = 0;
				BOOL success = ::GetExitCodeProcess(m_handle, &exitCode);
				assert(success);
				return exitCode;
			}

#if !defined(_WIN32_WCE)
			tstring GetPath() const
			{
				TCHAR path[MAX_PATH * 4 + 4] = { 0 };
				DWORD len = ::GetModuleFileNameEx(m_handle, NULL, path, MAX_PATH * 4);
				assert(len > 0);
				return tstring(path, len);
			}

			tstring GetBaseName() const
			{
				TCHAR path[MAX_PATH * 4 + 4] = { 0 };
				DWORD len = ::GetModuleBaseName(m_handle, NULL, path, MAX_PATH * 4);
				assert(len > 0);
				return tstring(path, len);
			}
#endif

			/// 获取句柄
			HANDLE GetHandle() const { return m_handle; }

		private:
			HANDLE	m_handle;
			DWORD	m_id;
		};


		/*
		class ProcessEnum
		{
		public:
		enum { MAX_PROCESS_COUNT = 1024 };
		DWORD		m_ids[MAX_PROCESS_COUNT];
		DWORD		m_count;

		ProcessEnum() : m_count(0)
		{
		DWORD bytes = 0;
		if (!::EnumProcesses(m_ids, MAX_PROCESS_COUNT * sizeof(DWORD), &bytes))
		{
		APP_ERROR("EnumProcesses failed " << ::GetLastError());
		return;
		}
		assert(bytes % 4 == 0);
		m_count = bytes / 4;
		}

		DWORD GetCount() const
		{
		return m_count;
		}

		DWORD FindProcess(LPCTSTR name)
		{
		assert(m_count > 0);
		DWORD id = 0;
		for (DWORD i = 0; i < m_count; ++i)
		{
		ProcessObject obj(m_ids[i]);
		if (!obj.IsValid())
		continue;
		tstring path = obj.GetBaseName();
		if (stricmp(path.c_str(), name) == 0)
		{
		id = m_ids[i];
		break;
		}
		}
		return id;
		}
		};
		*/


		struct KernelObjectHandleCloser
		{
			static bool Close(HANDLE handle)
			{
				return ::CloseHandle(handle) != FALSE;
			}
		};

		template <typename HandleT, typename CloserT>
		class HandleWrapper
		{
		public:
			HandleWrapper() : m_handle(NullHandle())
			{
			}
			~HandleWrapper()
			{
				Close();
			}

			HandleT GetHandle() const	{ return m_handle; }
			bool IsValid() const		{ return GetHandle() != NullHandle(); }
			void Close()
			{
				if (IsValid())
				{
					CloserT::Close(m_handle);
					m_handle = NullHandle();
				}
			}
			void Attach(HandleT handle)
			{
				Close();
				m_handle = handle;
			}

			static HandleT NullHandle()
			{
				return NULL;
			}

		private:
			HandleT	m_handle;
		};



		class ToolhelpSnapshot : public HandleWrapper<HANDLE, KernelObjectHandleCloser>
		{
		public:
			void Open(DWORD flags, DWORD pid = 0)
			{
				Attach(::CreateToolhelp32Snapshot(flags, pid));
			}
			void OpenProcesses()
			{
				Open(TH32CS_SNAPPROCESS);
			}
			void OpenHeaps(DWORD pid)
			{
				Open(TH32CS_SNAPHEAPLIST, pid);
			}
		};


		class ProcessWalker
		{
		public:
			ProcessWalker()
			{
				FILL_ZERO(m_item);
				m_item.dwSize = sizeof(m_item);
				m_snapshot.OpenProcesses();
				assert(m_snapshot.IsValid());
				m_good = (::Process32First(m_snapshot.GetHandle(), &m_item) != FALSE);
			}

			bool IsGood() const
			{
				return m_good;
			}
			const PROCESSENTRY32& Current() const
			{
				return m_item;
			}
			void MoveNext()
			{
				assert(IsGood());
				m_good = (::Process32Next(m_snapshot.GetHandle(), &m_item) != FALSE);
			}

		private:
			ToolhelpSnapshot	m_snapshot;
			PROCESSENTRY32		m_item;
			bool				m_good;
		};



		// 注意：此类使用Module32First系列API实现，不支持WinNT4
		class ModuleWalker
		{
		public:
			explicit ModuleWalker(DWORD pid = GetCurrentProcessId()) : m_isValid(false)
			{
				FILL_ZERO(m_item);
				m_item.dwSize = sizeof(MODULEENTRY32);
				m_snapshot.Open(TH32CS_SNAPMODULE, pid);
				assert(m_snapshot.IsValid());
				m_isValid = (FALSE != ::Module32First(m_snapshot.GetHandle(), &m_item));
			}
			~ModuleWalker()
			{
			}

			bool HasMore() const
			{
				return m_isValid;
			}
			void MoveNext()
			{
				assert(HasMore());
				m_isValid = (FALSE != ::Module32Next(m_snapshot.GetHandle(), &m_item));
			}
			const MODULEENTRY32& Current() const
			{
				return m_item;
			}

		private:
			ToolhelpSnapshot m_snapshot;
			MODULEENTRY32	m_item;
			bool	m_isValid;
		};

		// 注意：此类使用Module32First系列API实现，不支持WinNT4
		class ThreadWalker
		{
		public:
			explicit ThreadWalker(DWORD pid = GetCurrentProcessId()) : m_isValid(false)
			{
				m_snapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pid);
				memset(&m_item, 0, sizeof(THREADENTRY32));
				m_item.dwSize = sizeof(THREADENTRY32);
				if (!IsSnapShotValid())
					return;
				m_isValid = (FALSE != ::Thread32First(m_snapShot, &m_item));
			}
			~ThreadWalker()
			{
				::CloseHandle(m_snapShot);
			}

			bool HasMore() const
			{
				return m_isValid;
			}
			void MoveNext()
			{
				assert(HasMore());
				m_isValid = (FALSE != ::Thread32Next(m_snapShot, &m_item));
			}
			const THREADENTRY32& Current() const
			{
				return m_item;
			}

		protected:
			bool IsSnapShotValid() const
			{
				return m_snapShot != INVALID_HANDLE_VALUE;
			}

		private:
			HANDLE	m_snapShot;
			THREADENTRY32	m_item;
			bool	m_isValid;
		};

		/*
		class ThreadObject
		{
		public:
		explicit ThreadObject(DWORD threadID, DWORD access = THREAD_SUSPEND_RESUME) : m_handle(NULL)
		{
		m_handle = ::OpenThread(access, FALSE, threadID);
		}
		~ThreadObject()
		{
		if (m_handle != NULL)
		{
		::CloseHandle(m_handle);
		m_handle = NULL;
		}
		}
		HANDLE GetHandle()
		{
		return m_handle;
		}
		void Suspend()
		{
		::SuspendThread(m_handle);
		}
		private:
		HANDLE m_handle;
		};
		*/

		class FileFinder
		{
		public:
			explicit FileFinder(LPCTSTR filename = NULL) : m_handle(INVALID_HANDLE_VALUE), m_hasMore(false)
			{
				if (filename != NULL)
				{
					Find(filename);
				}
			}
			~FileFinder()
			{
				Close();
			}

			void Close()
			{
				if (IsValid())
				{
					::CloseHandle(m_handle);
					m_handle = INVALID_HANDLE_VALUE;
					m_hasMore = false;
				}
			}
			bool IsValid() const
			{
				return m_handle != INVALID_HANDLE_VALUE;
			}

			void Find(LPCTSTR filename)
			{
				Close();
				m_handle = ::FindFirstFile(filename, &m_item);
				if (!IsValid())
					return;
				m_hasMore = true;
			}

			bool HasMore() const
			{
				return m_hasMore;
			}
			void MoveNext()
			{
				assert(HasMore());
				m_hasMore = (FALSE != ::FindNextFile(m_handle, &m_item));
			}
			const WIN32_FIND_DATA& Current() const
			{
				return m_item;
			}

		private:
			HANDLE	m_handle;
			WIN32_FIND_DATA	m_item;
			bool	m_hasMore;
		};
	}
}
