#include "stdafx.h"
#include "IndexFunc.h"

namespace udpindex
{
	string Url2Mini(string url, string refer)
	{
		string url_tmp;
		if (refer.find("youku") == string::npos)
		{
			url_tmp = network::Uri(url).getrequest();
		}
		else
		{
			url_tmp = network::Uri(url).getfilerequest();
		}
		return url_tmp;
	}

    MD5 Url2MD5(string mini_url)
    {
        CMD5::p md5_p = CMD5::Create();
        md5_p->Add((LPCVOID)mini_url.c_str(),mini_url.length());
        md5_p->Finish();
        MD5 md5;
        md5_p->GetHash((SMD5*)&md5);
        return md5;
    }
    MD5 Ctt2MD5(MD5 ctt, size_t ctt_bytes, size_t file_length)
    {
        CMD5::p md5_p = CMD5::Create();
        md5_p->Add((LPCVOID)&ctt,sizeof(MD5));
        md5_p->Add((LPCVOID)&ctt_bytes,sizeof(size_t));
        md5_p->Add((LPCVOID)&file_length,sizeof(size_t));
        md5_p->Finish();
        MD5 md5;
        md5_p->GetHash((SMD5*)&md5);
        return md5;
    }
}