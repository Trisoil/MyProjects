#pragma once

namespace framework
{
	namespace mswin
	{

		class ShareMemory
		{
		public:
			ShareMemory() : m_Handle(NULL), m_View(NULL)
			{
			}
			~ShareMemory()
			{
				Close();
			}

			void Create(LPCSTR name, DWORD size);
			bool Open(LPCSTR name, DWORD access = FILE_MAP_READ);
			bool MapFile(HANDLE hFile, DWORD size, DWORD flProtect = PAGE_READONLY, DWORD access = FILE_MAP_READ);
			bool DoCreate(HANDLE hFile, DWORD protectFlags, DWORD size, LPCSTR name, DWORD access);
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
	}
}