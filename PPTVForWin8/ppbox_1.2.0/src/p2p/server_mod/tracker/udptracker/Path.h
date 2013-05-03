
#pragma once

/**
* @file
* @brief 路径相关操作
*/

const TCHAR PPL_PATH_SEPERATOR = TEXT('\\');

#ifndef ppassert
#define ppassert assert 
#endif

/// win32路径的操作类
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

	/// 获取文件名
	LPCTSTR GetFileName() const
	{
		if (path_.empty())
		{
			ppassert(false);
			return NULL;
		}
		size_t pos = path_.rfind(PPL_PATH_SEPERATOR);
		if (pos == std::string::npos)
			return path_.c_str();
		return path_.c_str() + pos + 1;
	}
	/// 获取文件扩展名
	LPCTSTR GetFileExtension() const
	{
		if (path_.empty())
		{
			ppassert(false);
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

	std::string GetFileNameTstr() const
	{
		LPCTSTR file_name = GetFileName();
		if (NULL == file_name)
		{
			return std::string();
		}
		return std::string(file_name);
	}

	std::string GetFileExtensionTstr() const
	{
		LPCTSTR ext = GetFileExtension();
		if (NULL == ext)
		{
			return std::string();
		}
		return std::string(ext);
	}

	// 返回路径盘符
	std::string GetDriveLetter() const
	{
		if (path_.empty())
		{
			ppassert(false);
			return std::string();
		}
		std::string::size_type pos = path_.find(PPL_PATH_SEPERATOR);
		if (std::string::npos == pos|| 2 != pos)
		{
			ppassert(false);
			return std::string();
		}
		return path_.substr(0, pos);
	}

	bool HasBackslash() const
	{
		if (path_.empty())
			return false;
		return path_[path_.size() - 1] == PPL_PATH_SEPERATOR;
	}

	/// 在末尾增加反斜线
	bool AddBackslash()
	{
		if (HasBackslash())
			return false;
		path_.append(1, PPL_PATH_SEPERATOR);
		return true;
	}
	/// 去掉末尾的反斜线
	bool RemoveBackslash()
	{
		if (!HasBackslash())
			return false;
		path_.resize(path_.size() - 1);
		return true;
	}
	/// 去掉文件名，只保留目录的路径
	bool RemoveFileSpec()
	{
		if (path_.empty())
		{
			ppassert(false);
			return false;
		}
		size_t pos = path_.rfind(PPL_PATH_SEPERATOR);
		if (pos == std::string::npos)
			return false;
		path_.resize(pos + 1);
		return true;
	}
	/// 去掉文件扩展名，只保留目录的路径
	bool RemoveFileExtension()
	{
		if (path_.empty())
		{
			ppassert(false);
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
	/// 修改文件名
	bool RenameFileSpec(LPCTSTR filename)
	{
		return RemoveFileSpec() && Append(filename);
	}
	/// 修改文件的扩展名
	bool RenameExtension(LPCTSTR ext)
	{
		RemoveFileExtension();
		path_.append(ext);
		return true;
	}
	/// 将路径dir和文件名file合并起来
	bool Combine(LPCTSTR dir, LPCTSTR file)
	{
		this->Assign(dir);
		this->Append(file);
		return true;
	}
	/// 将文件名file添加到原有的路径后
	bool Append(LPCTSTR file)
	{
		ppassert(file != NULL);
		ppassert(lstrlen(file) > 0);
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
	/// 设置路径
	void Assign(LPCTSTR path)
	{
		path_.assign(path);
	}
	/// 清除
	void Clear()
	{
		path_.erase();
	}

	const std::string& GetString() const
	{
		return path_;
	}

	/// 获取只读的缓冲区
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
		ppassert(file != NULL);
		if (path_.empty())
		{
		}
	}

private:
	/// 用于存储路径的字符缓冲区
	std::string path_;
};

#ifndef _DEBUG
inline ostream& operator<<(ostream& os, const Path& path)
{
	os << path.GetData();
	return os;
}
#endif
