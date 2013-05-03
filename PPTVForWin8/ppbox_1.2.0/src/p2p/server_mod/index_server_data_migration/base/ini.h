
#pragma once

/**
 * @file
 * @brief ini�ļ�����
 */
/*
 *ʹ�÷�����
 *		1������ģ���е���SetLocalFileName��������ʼ��Ini�ļ��� ���磺ini.SetLocalFileName(TEXT("params.ini"), m_dllModule.GetHandle());
 *		2���ڸ�����ģ���е���SetSection(sectionName)��ȷ��Ini�ļ��е�section��;���磺ini.SetSection(TEXT("Storage");
 *		3������Ҫ�ĵط�����GetString������GetInteger()�Ȼ�ȡ���ñ�����
 */
#include "framework/mswin/module.h"
/// ini�����ļ�������
class IniFile
{
public:
	IniFile()
	{
		SetLocalFileName();
	}

	/// ����ini�ļ���
	void SetFileName(const tstring& filename)
	{
		m_filename = filename;
	}

	/// ���ý�����
	void SetSection(const tstring& section)
	{
		m_section = section;
	}

	/// ���������exeĿ¼�µ�ini�ļ�
	void SetLocalFileName(const TCHAR* filename, HMODULE hModule = NULL)
	{
		framework::io::Path path;
		framework::mswin::Module(hModule).BuildLocalFilePath(path, filename);
		SetFileName(path.GetData());
	}

	/// ���ø�exeͬ����ini�ļ�
	void SetLocalFileName(HMODULE hModule = NULL)
	{
		framework::io::Path path;
		framework::mswin::Module(hModule).GetFilePath(path);
		path.RenameExtension(TEXT(".ini"));
		SetFileName(path.GetData());
	}

	/************************************************************************/
	/* string                                                               */
	/************************************************************************/
	DWORD GetString(LPCTSTR section, LPCTSTR key, LPTSTR buffer, DWORD size, LPCTSTR defaultValue)
	{
		assert(section != NULL);
		CheckFileName();
		return ::GetPrivateProfileString(section, key, defaultValue, buffer, size, GetFileName());
	}
	tstring GetString(LPCTSTR section, LPCTSTR key, LPCTSTR defaultValue)
	{
		const int max_size = 1024;
		TCHAR buf[max_size + 1] = { 0 };
		DWORD length = GetString(section, key, buf, max_size, defaultValue);
		assert(length <= max_size);
		return tstring(buf, length);
	}
	bool SetString(LPCTSTR section, LPCTSTR key, LPCTSTR value)
	{
		assert(section != NULL);
		CheckFileName();
		return FALSE != ::WritePrivateProfileString(section, key, value, GetFileName());
	}
	bool SetString(LPCTSTR section, LPCTSTR key, const tstring& value)
	{
		assert(section != NULL);
		CheckFileName();
		return SetString(section, key, value.c_str());
	}

	DWORD GetString(LPCTSTR key, LPTSTR buffer, DWORD size, LPCTSTR defaultValue)
	{
		CheckSection();
		return GetString(GetSection(), key, buffer, size, defaultValue);
	}
	tstring GetString(LPCTSTR key, LPCTSTR defaultValue)
	{
		CheckSection();
		return GetString(GetSection(), key, defaultValue);
	}
	bool SetString(LPCTSTR key, LPCTSTR value)
	{
		CheckSection();
		return SetString(GetSection(), key, value);
	}
	bool SetString(LPCTSTR key, const tstring& value)
	{
		CheckSection();
		return SetString(GetSection(), key, value);
	}



	/************************************************************************/
	/* binary                                                               */
	/************************************************************************/
	bool GetBinaryData(LPCTSTR section, LPCTSTR key, void* buffer, UINT size)
	{
		assert(section != NULL);
		CheckFileName();
		return FALSE != ::GetPrivateProfileStruct(section, key, buffer, size, GetFileName());
	}
	bool SetBinaryData(LPCTSTR section, LPCTSTR key, const void* data, UINT size)
	{
		assert(section != NULL);
		CheckFileName();
		return FALSE != ::WritePrivateProfileStruct(section, key, const_cast<void*>(data), size, GetFileName());
	}
	template <typename StructT>
	bool GetStruct(LPCTSTR section, LPCTSTR key, StructT& val)
	{
		return GetBinaryData(section, key, &val, sizeof(StructT));
	}
	template <typename StructT>
	bool SetStruct(LPCTSTR section, LPCTSTR key, const StructT& val)
	{
		return SetBinaryData(section, key, &val, sizeof(StructT));
	}

	bool GetBinaryData(LPCTSTR key, void* buffer, UINT size)
	{
		CheckSection();
		return GetBinaryData(GetSection(), key, buffer, size);
	}
	bool SetBinaryData(LPCTSTR key, const void* data, UINT size)
	{
		CheckSection();
		return SetBinaryData(GetSection(), key, data, size);
	}
	template <typename StructT>
	bool GetStruct(LPCTSTR key, StructT& val)
	{
		CheckSection();
		return GetStruct(GetSection(), key, val);
	}
	template <typename StructT>
	bool SetStruct(LPCTSTR key, const StructT& val)
	{
		CheckSection();
		return SetStruct(GetSection(), key, val);
	}



	/************************************************************************/
	/* integer                                                              */
	/************************************************************************/
	UINT GetInteger(LPCTSTR section, LPCTSTR key, int defaultValue)
	{
		CheckFileName();
		return ::GetPrivateProfileInt(section, key, defaultValue, GetFileName());
	}
	bool SetInteger(LPCTSTR section, LPCTSTR key, int value)
	{
		TCHAR str[256] = { 0 };
		::wsprintf(str, TEXT("%d"), value);
		return SetString(section, key, str);
	}

	UINT GetInteger(LPCTSTR key, int defaultValue)
	{
		CheckSection();
		return GetInteger(GetSection(), key, defaultValue);
	}
	bool SetInteger(LPCTSTR key, int value)
	{
		CheckSection();
		return SetInteger(GetSection(), key, value);
	}


	/************************************************************************/
	/* bool                                                                 */
	/************************************************************************/
	bool GetBool(LPCTSTR section, LPCTSTR key, bool defaultValue)
	{
		return 0 != GetInteger(section, key, defaultValue);
	}
	bool SetBool(LPCTSTR section, LPCTSTR key, bool value)
	{
		return SetInteger(section, key, value);
	}
	BOOL GetBOOL(LPCTSTR section, LPCTSTR key, BOOL defaultValue)
	{
		UINT res = GetInteger(section, key, defaultValue);
		return (0 != res) ? TRUE : FALSE;
	}
	bool SetBOOL(LPCTSTR section, LPCTSTR key, BOOL value)
	{
		return SetInteger(section, key, value);
	}


	bool GetBool(LPCTSTR key, bool defaultValue)
	{
		CheckSection();
		return GetBool(GetSection(), key, defaultValue);
	}
	bool SetBool(LPCTSTR key, bool value)
	{
		CheckSection();
		return SetBool(GetSection(), key, value);
	}
	BOOL GetBOOL(LPCTSTR key, BOOL defaultValue)
	{
		CheckSection();
		return GetBOOL(GetSection(), key, defaultValue);
	}
	bool SetBOOL(LPCTSTR key, BOOL value)
	{
		CheckSection();
		return SetBOOL(GetSection(), key, value);
	}

protected:
	/// ���ini�ļ����ַ���
	LPCTSTR GetFileName() const
	{
		return m_filename.c_str();
	}
	LPCTSTR GetSection() const
	{
		return m_section.c_str();
	}
	void CheckSection()
	{
		assert(!m_section.empty());
	}
	void CheckFileName()
	{
		assert(!m_filename.empty());
	}

private:
	/// ini�ļ���(���ܰ���·��)
	tstring m_filename;
	/// ini����
	tstring m_section;
};



