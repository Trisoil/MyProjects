
#pragma once


#include <share.h>

#define UTIL_INFO(message)
#define UTIL_EVENT(message)
#define UTIL_WARN(message)
#define UTIL_ERROR(message)
#define UTIL_DEBUG(message)

namespace framework
{
	namespace io
	{
		/// 标准C的文件操作封装类
		class StdFile : private boost::noncopyable
		{
		public:
			StdFile() : m_handle(NULL)
			{
			}
			~StdFile()
			{
				Close();
			}

			/// 获取文件句柄
			FILE* GetHandle() const { return m_handle; }

			/// 文件句柄是否有效
			bool IsOpen() const { return m_handle != NULL; }

			/// 关闭文件
			void Close();

			/// 是否到了文件末尾
			bool IsEOF() const;

			/// 打开文件
			bool Open(const TCHAR* path, const TCHAR* mode = TEXT("r"), int shareFlag = _SH_DENYNO);

			int ReadByte()
			{
				return fgetc(m_handle);
			}

			/// 读取数据
			size_t Read(void* buf, size_t size);

			/// 读取行到缓冲区
			bool ReadLine(char* buf, int size);

			/// 读取行到字符串
			bool ReadLine(string& line);

			/// 读取行到缓冲区
			bool ReadLine(wchar_t* buf, int size);

			/// 读取行到字符串
			bool ReadLine(wstring& line);

			/// 写入缓冲区数据
			size_t Write(const void* data, size_t size);

			/// 写入字符串
			bool Write(const char* str);

			/// 不定参数的格式化写入
			int WriteV(const char* format, va_list argptr);

			/// 格式化的写入
			int WriteF(const char* format, ...);

			/// 写入结构体数据
			template <typename StructT>
			bool WriteStruct(const StructT& buffer)
			{
				size_t size = Write(&buffer, sizeof(StructT));
				return size == sizeof(StructT);
			}

			/// 读取结构体数据
			template <typename StructT>
			bool ReadStruct(StructT& buffer)
			{
				size_t size = Read(&buffer, sizeof(StructT));
				return size == sizeof(StructT);
			}

			bool Seek(long offset, int origin = SEEK_SET)
			{
				return 0 == fseek(m_handle, offset, origin);
			}

			/// 刷新缓冲区
			bool Flush();

			/// 是否失败
			bool IsFailed();
		protected:
			/// 文件句柄
			FILE* m_handle;
		private:
		};


		/// 负责读文件的标准C文件操作封装类
		class StdFileReader : public StdFile
		{
		public:
			/// 以二进制方式打开
			bool OpenBinary(const TCHAR* path, int shareFlag = _SH_DENYNO);

			/// 以文本方式打开
			bool OpenText(const TCHAR* path, int shareFlag = _SH_DENYNO);
		};


		/// 负责写文件的标准C文件操作封装类
		class StdFileWriter : public StdFile
		{
		public:
			StdFileWriter() { }
			~StdFileWriter();

			void Close();

			/// 以二进制方式打开
			bool OpenBinary(const TCHAR* path, int shareFlag = _SH_DENYWR);

			/// 以文本方式打开
			bool OpenText(const TCHAR* path, int shareFlag = _SH_DENYWR);

			/// 写入文件
			static bool WriteBinary(const TCHAR* path, const void* data, size_t size);
		};






		inline bool StdFile::Flush()
		{
			assert(IsOpen());
			if (!IsOpen())
			{
				return false;
			}
			return EOF != fflush(m_handle);
		}

		inline bool StdFile::IsFailed()
		{
			assert(IsOpen());
			return ferror(m_handle) != 0;
		}
		inline int StdFile::WriteV(const char* format, va_list argptr)
		{
			assert(format != NULL);
			assert(IsOpen());
			if (!IsOpen())
			{
				return 0;
			}
			return vfprintf(m_handle, format, argptr);
		}
		inline bool StdFile::Write(const char* str)
		{
			assert(str != NULL);
			assert(IsOpen());
			if (!IsOpen())
			{
				return false;
			}
			return EOF != fputs(str, m_handle);
		}
		inline size_t StdFile::Write(const void* data, size_t size)
		{
			assert(data != NULL && size > 0);
			assert(IsOpen());
			if (!IsOpen())
			{
				return 0;
			}
			return fwrite(data, size, 1, m_handle) * size;
		}
		inline size_t StdFile::Read(void* buf, size_t size)
		{
			assert(buf != NULL && size > 0);
			assert(IsOpen());
			if (!IsOpen())
			{
				return 0;
			}
			return fread(buf, 1, size, m_handle);
		}
		inline bool StdFile::IsEOF() const
		{
			return 0 != feof(m_handle);
		}

		inline void StdFile::Close()
		{
			if (IsOpen())
			{
				fclose(m_handle);
				m_handle = NULL;
			}
		}

		inline bool StdFile::Open(const TCHAR* path, const TCHAR* mode, int shareFlag)
		{
			assert(!IsOpen());
			m_handle = _tfsopen(path, mode, shareFlag);
			if (IsOpen())
				return true;
			UTIL_DEBUG("fopen failed " << " with " << path << " for mode " << mode);
			return false;
		}

		inline bool StdFile::ReadLine(char* buf, int size)
		{
			assert(buf != NULL && size > 0);
			assert(IsOpen());
			if (!IsOpen())
				return false;
			if (fgets(buf, size, m_handle) != NULL)
				return true;
			if (IsFailed())
			{
				UTIL_DEBUG("fgets failed ");
			}
			return false;
		}

		inline bool StdFile::ReadLine(string& line)
		{
			assert(IsOpen());
			if (!IsOpen())
				return false;
			const size_t max_size = 1025;
			line.clear();
			while (true)
			{
				char str[max_size] = { 0 };
				if (fgets(str, max_size, m_handle) == NULL)
				{
					return false;
				}
				string tmp_line = str;
				line += tmp_line;
				if (tmp_line.size() <1024 || tmp_line.size()==1024 && tmp_line[tmp_line.size()-1]=='\n')
				{
					break;
				}
			}
			return true;
		}

		inline bool StdFile::ReadLine(wchar_t* buf, int size)
		{
			assert(buf != NULL && size > 0);
			assert(IsOpen());
			if (!IsOpen())
				return false;
			if (fgetws(buf, size, m_handle) != NULL)
				return true;
			if (IsFailed())
			{
				UTIL_DEBUG("fgets failed ");
			}
			return false;
		}

		inline bool StdFile::ReadLine(wstring& line)
		{
			assert(IsOpen());
			if (!IsOpen())
				return false;
			const size_t max_size = 1024;
			wchar_t str[max_size + 1] = { 0 };
			if (fgetws(str, max_size, m_handle) == NULL)
			{
				if (IsFailed())
				{
					UTIL_DEBUG("fgets failed");
				}
				return false;
			}
			line = str;
			return true;
		}

		inline int StdFile::WriteF(const char* format, ...)
		{
			assert(format != NULL);
			va_list(args);
			va_start(args, format);
			int count = WriteV(format, args);
			va_end(args);
			return count;
		}





		inline bool StdFileReader::OpenBinary(const TCHAR* path, int shareFlag)
		{
			return this->Open(path, TEXT("rb"), shareFlag);
		}

		inline bool StdFileReader::OpenText(const TCHAR* path, int shareFlag)
		{
			return this->Open(path, TEXT("r"), shareFlag);
		}




		inline bool StdFileWriter::OpenBinary(const TCHAR* path, int shareFlag)
		{
			return this->Open(path, TEXT("wb"), shareFlag);
		}

		inline bool StdFileWriter::OpenText(const TCHAR* path, int shareFlag)
		{
			return this->Open(path, TEXT("w"), shareFlag);
		}

		inline StdFileWriter::~StdFileWriter()
		{
			Close();
		}

		inline void StdFileWriter::Close()
		{
			if (IsOpen())
			{
				Flush();
				StdFile::Close();
			}
		}


		inline bool StdFileWriter::WriteBinary(const TCHAR* path, const void* data, size_t size)
		{
			StdFileWriter file;
			if (!file.OpenBinary(path, _SH_DENYRW))
				return false;
			if (file.Write(data, size) != size)
				return false;
			file.Flush();
			return true;
		}

	}
}


