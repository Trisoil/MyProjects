
#pragma once


#pragma comment(lib, "version.lib")

#define UTIL_INFO(message)
#define UTIL_EVENT(message)
#define UTIL_WARN(message)
#define UTIL_ERROR(message)
#define UTIL_DEBUG(message)

namespace framework
{
	namespace mswin
	{
		/// �ļ��İ汾��Ϣ
		class FileVersionInfo
		{
		public:
			FileVersionInfo();
			explicit FileVersionInfo(LPCTSTR filename);

			/// ��ȡָ���ļ��İ汾��Ϣ
			bool Retrieve(LPCTSTR filename);

			/// ��ȡ������Ϣ
			const VS_FIXEDFILEINFO& GetFixedInfo() const
			{
				assert(!m_data.empty());
				return m_fixedInfo;
			}

		private:
			/// �汾��Ϣ��ԭʼ����
			string m_data;

			/// �����İ汾��Ϣ
			VS_FIXEDFILEINFO m_fixedInfo;
		};



		inline FileVersionInfo::FileVersionInfo()
		{
			FILL_ZERO(m_fixedInfo);
		}

		inline FileVersionInfo::FileVersionInfo(LPCTSTR filename)
		{
			Retrieve(filename);
		}

		inline bool FileVersionInfo::Retrieve(LPCTSTR filename)
		{
			FILL_ZERO(m_fixedInfo);
			LPTSTR filepath = const_cast<LPTSTR>(filename);
			m_data.reserve(8 * 1024);
			DWORD dummyHandle = 0;
			DWORD size = ::GetFileVersionInfoSize(filepath, &dummyHandle);
			if (size <= 0)
			{
				UTIL_ERROR("FileVersionInfo::Retrieve: GetFileVersionInfoSize failed " << make_tuple(::GetLastError(), dummyHandle));
				return false;
			}
			m_data.resize(size);
			if (!::GetFileVersionInfo(filepath, 0, size, &m_data[0]))
			{
				UTIL_ERROR("FileVersionInfo::Retrieve: GetFileVersionInfo failed " << make_tuple(::GetLastError(), dummyHandle));
				return false;
			}
			void* buffer = NULL;
			UINT bufferLength = 0;
			const char* data = m_data.data();
			if (!::VerQueryValueA(const_cast<char*>(data), "\\", &buffer, &bufferLength))
			{
				UTIL_ERROR("FileVersionInfo::Retrieve: VerQueryValue failed " << make_tuple(::GetLastError(), dummyHandle));
				return false;
			}
			if (bufferLength < sizeof(VS_FIXEDFILEINFO))
			{
				UTIL_ERROR("FileVersionInfo::Retrieve: VerQueryValue failed buffer length error " << bufferLength);
				return false;
			}
			const VS_FIXEDFILEINFO* fixedInfo = static_cast<const VS_FIXEDFILEINFO*>(buffer);
			m_fixedInfo = *fixedInfo;
			return true;
		}
	}
}
