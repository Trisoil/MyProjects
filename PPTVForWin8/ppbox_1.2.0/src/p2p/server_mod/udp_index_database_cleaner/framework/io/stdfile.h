
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
		/// ��׼C���ļ�������װ��
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

			/// ��ȡ�ļ����
			FILE* GetHandle() const { return m_handle; }

			/// �ļ�����Ƿ���Ч
			bool IsOpen() const { return m_handle != NULL; }

			/// �ر��ļ�
			void Close();

			/// �Ƿ����ļ�ĩβ
			bool IsEOF() const;

			/// ���ļ�
			bool Open(const TCHAR* path, const TCHAR* mode = TEXT("r"), int shareFlag = _SH_DENYNO);

			int ReadByte()
			{
				return fgetc(m_handle);
			}

			/// ��ȡ����
			size_t Read(void* buf, size_t size);

			/// ��ȡ�е�������
			bool ReadLine(char* buf, int size);

			/// ��ȡ�е��ַ���
			bool ReadLine(string& line);

			/// ��ȡ�е�������
			bool ReadLine(wchar_t* buf, int size);

			/// ��ȡ�е��ַ���
			bool ReadLine(wstring& line);

			/// д�뻺��������
			size_t Write(const void* data, size_t size);

			/// д���ַ���
			bool Write(const char* str);

			/// ���������ĸ�ʽ��д��
			int WriteV(const char* format, va_list argptr);

			/// ��ʽ����д��
			int WriteF(const char* format, ...);

			/// д��ṹ������
			template <typename StructT>
			bool WriteStruct(const StructT& buffer)
			{
				size_t size = Write(&buffer, sizeof(StructT));
				return size == sizeof(StructT);
			}

			/// ��ȡ�ṹ������
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

			/// ˢ�»�����
			bool Flush();

			/// �Ƿ�ʧ��
			bool IsFailed();
		protected:
			/// �ļ����
			FILE* m_handle;
		private:
		};


		/// ������ļ��ı�׼C�ļ�������װ��
		class StdFileReader : public StdFile
		{
		public:
			/// �Զ����Ʒ�ʽ��
			bool OpenBinary(const TCHAR* path, int shareFlag = _SH_DENYNO);

			/// ���ı���ʽ��
			bool OpenText(const TCHAR* path, int shareFlag = _SH_DENYNO);
		};


		/// ����д�ļ��ı�׼C�ļ�������װ��
		class StdFileWriter : public StdFile
		{
		public:
			StdFileWriter() { }
			~StdFileWriter();

			void Close();

			/// �Զ����Ʒ�ʽ��
			bool OpenBinary(const TCHAR* path, int shareFlag = _SH_DENYWR);

			/// ���ı���ʽ��
			bool OpenText(const TCHAR* path, int shareFlag = _SH_DENYWR);

			/// д���ļ�
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


