#pragma once
#include "CommonZ.h"
#include <string>

class ZFile
{
public:
	ZFile(void);
	ZFile(const char* pFileName);
	~ZFile(void);
	
	void SetFileName(const char* pFileName);
	bool GetLineInfo(char* pBuf, int iBufSize);

	bool open(const char* pMode = RW_MODULE);

    int write(const char* pBuf,int iBufSize);

    int fprintf( const char* format,...);

    void AddHead(const char* pBuf,int iBufSize);

protected:

	void clear();

private:

	FILE* m_pFile;
	std::string m_strFileName;
};
