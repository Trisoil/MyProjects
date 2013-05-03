// stdafx.cpp : 只包括标准包含文件的源文件
// p2pvideo.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用

namespace framework
{
	string w2b(const wstring& _src)
	{
		int nBufSize = ::WideCharToMultiByte(GetACP(), 0, _src.c_str(),-1, NULL, 0, 0, FALSE);

		char *szBuf = new char[nBufSize + 1];

		::WideCharToMultiByte(GetACP(), 0, _src.c_str(),-1, szBuf, nBufSize, 0, FALSE);

		string strRet(szBuf);

		delete []szBuf;
		szBuf = NULL;

		return strRet;
	}

	wstring b2w(const string& _src)
	{
		//计算字符串 string 转成 wchar_t 之后占用的内存字节数
        int nBufSize = ::MultiByteToWideChar(GetACP(),0,_src.c_str(),-1,NULL,0); 

		//为 wsbuf 分配内存 BufSize 个字节
		wchar_t *wsBuf = new wchar_t[nBufSize + 1];

		//转化为 unicode 的 WideString
        ::MultiByteToWideChar(GetACP(),0,_src.c_str(),-1,wsBuf,nBufSize); 

		wstring wstrRet(wsBuf);

		delete []wsBuf;
		wsBuf = NULL;

		return wstrRet;
	}
}