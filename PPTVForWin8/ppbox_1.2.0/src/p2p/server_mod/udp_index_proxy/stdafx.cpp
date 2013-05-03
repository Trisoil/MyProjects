// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// p2pvideo.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� STDAFX.H ��
// �����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������

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
		//�����ַ��� string ת�� wchar_t ֮��ռ�õ��ڴ��ֽ���
        int nBufSize = ::MultiByteToWideChar(GetACP(),0,_src.c_str(),-1,NULL,0); 

		//Ϊ wsbuf �����ڴ� BufSize ���ֽ�
		wchar_t *wsBuf = new wchar_t[nBufSize + 1];

		//ת��Ϊ unicode �� WideString
        ::MultiByteToWideChar(GetACP(),0,_src.c_str(),-1,wsBuf,nBufSize); 

		wstring wstrRet(wsBuf);

		delete []wsBuf;
		wsBuf = NULL;

		return wstrRet;
	}
}