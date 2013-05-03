
#pragma once

/**
* @file
* @brief guid��ص���ͺ���
*/


#include <objbase.h>

#pragma comment(lib, "ole32.lib")

#include "framework/log.h"
#include <boost/algorithm/string.hpp>

#define UTIL_ERROR(s) LOG(__ERROR, "util", s)

namespace base
{
	/// ��ʽ��GUID
	bool FormatGUID(char* result, int result_buflen, const GUID& guid);

	/// ��GUID��ʽ��Ϊ�ַ���
	string FormatGUID(const GUID& guid);

	wstring wFormatGUID(const GUID& guid);

	/// ����guid�ַ���
	bool ParseGUID(GUID& guid, const string& guidstr);

	/// ����guid�ַ���
	bool ParseGUID(GUID& guid, const char* guidstr);

	/// ��GUID�������
	inline ostream& operator<<(ostream& os, const GUID& guid)
	{
		return os << FormatGUID(guid);
	}
	inline wostream& operator<<(wostream& os, const GUID& guid)
	{
		return os << wFormatGUID(guid);
	}

	/// ��װGUID��صĲ���
	struct Guid : public GUID
	{
	public:
		Guid()
		{
			Clear();
		}
		Guid(const GUID& guid) : GUID(guid)
		{
		}

		void operator=(const GUID& guid)
		{
			GUID& me = *this;
			me = guid;
		}

		/// ���
		void Clear()
		{
			FILL_ZERO(*this);
		}

		bool IsEmpty() const
		{
			return *this == GUID_NULL;
		}	

		/// �����ַ����õ�GUID
		bool Parse(const char* guidstr) { return Parse(string(guidstr)); }

		/// �����ַ����õ�GUID
		bool Parse(const string& guidstr) { return ParseGUID(*this, guidstr); }

		/// �������һ��(Ψһ��)GUID
		void Generate()
		{
			HRESULT hr = ::CoCreateGuid(this);
			assert(S_OK == hr);
		}

		/// ת��Ϊ�ַ���
		string ToString() const { return FormatGUID(*this); }
	};

	/// �Զ�������ɵ�guid
	struct RandomGuid : public Guid
	{
	public:
		RandomGuid()
		{
			Generate();
		}
	};

	inline bool FormatGUID(char* result, int result_buflen, const GUID& guid)
	{
		int len = sprintf_s(
			result, 
			result_buflen,
			"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X", 
			guid.Data1, guid.Data2, guid.Data3, 
			guid.Data4[0], guid.Data4[1], 
			guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]
		);
		if (len == sizeof(GUID) * 2)
			return true;
		UTIL_ERROR("FormatGUID failed. len=" << len);
		assert(!"FormatGUID failed.");
		return false;
	}
	inline bool FormatGUID(wchar_t* result,int result_buflen, const GUID& guid)
	{
		int len = swprintf_s(
			result, 
			result_buflen,
			L"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X", 
			guid.Data1, guid.Data2, guid.Data3, 
			guid.Data4[0], guid.Data4[1], 
			guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]
		);
		if (len == sizeof(GUID) * 2)
			return true;
		UTIL_ERROR("FormatGUID failed. len=" << len);
		assert(!"FormatGUID failed.");
		return false;
	}

	inline string FormatGUID(const GUID& guid)
	{
		char result[256];
		FormatGUID(result, 256, guid);
		return result;
	}

	inline wstring wFormatGUID(const GUID& guid)
	{
		wchar_t result[256];
		FormatGUID(result, 256,guid);
		return result;
	}

	inline bool DoParseGUID(GUID& guid, const char* guidstr)
	{
		//assert(guidstr != NULL);
		if(guidstr == NULL) return false;
		string guid_string = guidstr;
		boost::algorithm::replace_all(guid_string,"{","");
		boost::algorithm::replace_all(guid_string,"}","");
		boost::algorithm::replace_all(guid_string,"-","");
		//assert(guid_string.length()==32);
        if (guid_string.length() != 32)
        {
            return false;
        }
		const int guid_fields = 11;
		unsigned int fields[3];
		unsigned int fields2[8];
		int count = sscanf_s(
			guid_string.c_str(), 
			"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X", 
			&(fields[0]), &(fields[1]), &(fields[2]), 
			&(fields2[0]), &(fields2[1]), 
			&(fields2[2]), &(fields2[3]), 
			&(fields2[4]), &(fields2[5]), 
			&(fields2[6]), &(fields2[7])
			);
		assert(guid_fields == count);
		guid.Data1 = fields[0];
		guid.Data2 = fields[1];
		guid.Data3 = fields[2];
		for (int i = 0; i < 8; ++i)
		{
			guid.Data4[i] = fields2[i];
		}
		return guid_fields == count;
	}

	inline bool DoParseGUID(GUID& guid, const wchar_t* guidstr)
	{
		//assert(guidstr != NULL);
		if(guidstr == NULL) return false;
		wstring guid_string = guidstr;
		boost::algorithm::replace_all(guid_string,L"{",L"");
		boost::algorithm::replace_all(guid_string,L"}",L"");
		boost::algorithm::replace_all(guid_string,L"-",L"");
		//assert(guid_string.length()==32);
        if (guid_string.length() != 32)
        {
            return false;
        }

		const int guid_fields = 11;
		unsigned int fields[3];
		unsigned int fields2[8];
		int count = swscanf_s(
			guidstr, 
			L"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X", 
			&(fields[0]), &(fields[1]), &(fields[2]), 
			&(fields2[0]), &(fields2[1]), 
			&(fields2[2]), &(fields2[3]), 
			&(fields2[4]), &(fields2[5]), 
			&(fields2[6]), &(fields2[7])
			);
		assert(guid_fields == count);
		guid.Data1 = fields[0];
		guid.Data2 = fields[1];
		guid.Data3 = fields[2];
		for (int i = 0; i < 8; ++i)
		{
			guid.Data4[i] = fields2[i];
		}
		return guid_fields == count;
	}

	inline bool ParseGUID(GUID& guid, const char* guidstr)
	{
		return ParseGUID(guid, string(guidstr));
	}

	inline bool ParseGUID(GUID& guid, const string& guidstr)
	{
		string str = guidstr;
		if (str.empty())
			return false;
		if (str[0] == '{')
		{
			str.erase(0, 1);
		}
		if (str[str.size() - 1] == '}')
		{
			str.erase(str.size() - 1, 1);
		}
		bool res = DoParseGUID(guid, str.c_str());
		//assert(res);
		return res;
	}

	inline bool operator < (const Guid& guid1, const Guid& guid2)
	{
		return memcmp(&guid1, &guid2, sizeof(Guid)) < 0;
	}
}

typedef base::Guid MD5;
typedef base::Guid RID;

//////////////////////////////////////////////////////////////////////////
// Hash Function

namespace boost
{
	inline std::size_t hash_value(Guid const &g)
	{
		return g.Data1^((g.Data2<<0x10)|g.Data3)^((g.Data4[0]<<0x18)|(g.Data4[1]<<0x10)|(g.Data4[2]<<0x8)|g.Data4[3])^((g.Data4[4]<<0x18)|(g.Data4[5]<<0x10)|(g.Data4[6]<<0x8)|g.Data4[7]);
	}
	//inline std::size_t hash_value(Guid const& g)
	//{
	//	unsigned long a = g.Data1;
	//	unsigned short b = g.Data2, c = g.Data3;
	//	unsigned char f1 = g.Data4[0], k1 = g.Data4[4];
	//	unsigned char f2 = g.Data4[1], k2 = g.Data4[5];
	//	unsigned char f3 = g.Data4[2], k3 = g.Data4[6];
	//	unsigned char f4 = g.Data4[3], k4 = g.Data4[7];
	//	__asm
	//	{
	//		movzx eax, b
	//		shl eax, 10h
	//		or ax, c
	//		xor eax, a
	//		movzx edx, f1
	//		shl edx, 18h
	//		or cl, k1
	//		xor eax, edx
	//		movzx edx, f2
	//		shl edx, 18h
	//		or cl, k2
	//		xor eax, edx
	//		movzx edx, f3
	//		shl edx, 18h
	//		or cl, k3
	//		xor eax, edx
	//		movzx edx, f4
	//		shl edx, 18h
	//		or cl, k4
	//		xor eax, edx
	//	}	//Counting ((a ^ (b << 0x10) | c) ^ ((f << 0x18) | k))
	//	// return value is in eax now, so no return here
	//}
}
