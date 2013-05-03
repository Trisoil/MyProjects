
#pragma once

/**
* @file
* @brief ·����ز���
*/

namespace framework
{
	namespace io
	{
		const TCHAR PPL_PATH_SEPERATOR = TEXT('\\');

		/// win32·���Ĳ�����
		class Path : private boost::noncopyable
		{
		public:
			Path()
			{
			}
			Path(const Path& path) : path_(path.path_)
			{
			}

			void operator=(const Path& path)
			{
				path_ = path.path_;
			}


			enum { MAX_PATH_SIZE = MAX_PATH * 4 };
			enum { MAX_BUFFER_SIZE = MAX_PATH_SIZE + 4 };

			/// ��ȡ�ļ���
			LPCTSTR GetFileName() const
			{
				if (path_.empty())
				{
					assert(false);
					return NULL;
				}
				size_t pos = path_.rfind(PPL_PATH_SEPERATOR);
				if (pos == tstring::npos)
					return path_.c_str();
				return path_.c_str() + pos + 1;
			}
			/// ��ȡ�ļ���չ��
			LPCTSTR GetFileExtension() const
			{
				if (path_.empty())
				{
					assert(false);
					return NULL;
				}
				for (int i = (int) path_.size() - 1; i >= 0; --i)
				{
					TCHAR ch = path_[i];
					if (ch == PPL_PATH_SEPERATOR)
						return NULL;
					if (ch == '.')
					{
						return path_.c_str() + i + 1;
					}
				}
				return NULL;
			}

			tstring GetFileNameTstr() const
			{
				LPCTSTR file_name = GetFileName();
				if (NULL == file_name)
				{
					return tstring();
				}
				return tstring(file_name);
			}

			tstring GetFileExtensionTstr() const
			{
				LPCTSTR ext = GetFileExtension();
				if (NULL == ext)
				{
					return tstring();
				}
				return tstring(ext);
			}

			// ����·���̷�
			tstring GetDriveLetter() const
			{
				if (path_.empty())
				{
					assert(false);
					return tstring();
				}
				tstring::size_type pos = path_.find(PPL_PATH_SEPERATOR);
				if (tstring::npos == pos|| 2 != pos)
				{
					assert(false);
					return tstring();
				}
				return path_.substr(0, pos);
			}

			bool HasBackslash() const
			{
				if (path_.empty())
					return false;
				return path_[path_.size() - 1] == PPL_PATH_SEPERATOR;
			}

			/// ��ĩβ���ӷ�б��
			bool AddBackslash()
			{
				if (HasBackslash())
					return false;
				path_.append(1, PPL_PATH_SEPERATOR);
				return true;
			}
			/// ȥ��ĩβ�ķ�б��
			bool RemoveBackslash()
			{
				if (!HasBackslash())
					return false;
				path_.resize(path_.size() - 1);
				return true;
			}
			/// ȥ���ļ�����ֻ����Ŀ¼��·��
			bool RemoveFileSpec()
			{
				if (path_.empty())
				{
					assert(false);
					return false;
				}
				size_t pos = path_.rfind(PPL_PATH_SEPERATOR);
				if (pos == tstring::npos)
					return false;
				path_.resize(pos + 1);
				return true;
			}
			/// ȥ���ļ���չ����ֻ����Ŀ¼��·��
			bool RemoveFileExtension()
			{
				if (path_.empty())
				{
					assert(false);
					return false;
				}
				for (int i = (int) path_.size() - 1; i >= 0; --i)
				{
					TCHAR ch = path_[i];
					if (ch == PPL_PATH_SEPERATOR)
						return false;
					if (ch == '.')
					{
						path_.resize(i);
						return true;
					}
				}
				return false;
			}
			/// �޸��ļ���
			bool RenameFileSpec(LPCTSTR filename)
			{
				return RemoveFileSpec() && Append(filename);
			}
			/// �޸��ļ�����չ��
			bool RenameExtension(LPCTSTR ext)
			{
				RemoveFileExtension();
				path_.append(ext);
				return true;
			}
			/// ��·��dir���ļ���file�ϲ�����
			bool Combine(LPCTSTR dir, LPCTSTR file)
			{
				this->Assign(dir);
				this->Append(file);
				return true;
			}
			/// ���ļ���file��ӵ�ԭ�е�·����
			bool Append(LPCTSTR file)
			{
				assert(file != NULL);
				//assert(lstrlen(file) > 0);
				if (path_.empty())
				{
					path_.assign(file);
					return false;
				}
				if (path_[path_.size() - 1] == PPL_PATH_SEPERATOR)
				{
					if (file[0] == PPL_PATH_SEPERATOR)
					{
						path_.append(file + 1);
					}
					else
					{
						path_.append(file);
					}
				}
				else
				{
					if (file[0] == PPL_PATH_SEPERATOR)
					{
						path_.append(file);
					}
					else
					{
						path_.append(1, PPL_PATH_SEPERATOR);
						path_.append(file);
					}
				}
				return true;
			}
			/// ����·��
			void Assign(LPCTSTR path)
			{
				path_.assign(path);
			}
			/// ���
			void Clear()
			{
				path_.erase();
			}

			const tstring& GetString() const
			{
				return path_;
			}

			/// ��ȡֻ���Ļ�����
			LPCTSTR GetData() const
			{
				return path_.c_str();
			}

			operator LPCTSTR() const
			{
				return path_.c_str();
			}

		protected:
			void CheckAppendBlash(LPCTSTR file)
			{
				assert(file != NULL);
				if (path_.empty())
				{
				}
			}

		private:
			/// ���ڴ洢·�����ַ�������
			tstring path_;
		};

		#ifndef _DEBUG
		inline ostream& operator<<(ostream& os, const Path& path)
		{
			os << path.GetData();
			return os;
		}
		#endif
	}
}



