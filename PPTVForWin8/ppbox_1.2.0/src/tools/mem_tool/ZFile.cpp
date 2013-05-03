#include "ZFile.h"
#include <stdio.h>
#include <string.h>

ZFile::ZFile(void)
{
    m_pFile = NULL;
}

ZFile::~ZFile(void)
{
    clear();
}
ZFile::ZFile(const char* pFileName)
{
    m_pFile = NULL;
    m_strFileName = pFileName;
    open();	


}
void ZFile::SetFileName(const char* pFileName)
{
    m_strFileName = pFileName;	
}

void ZFile::AddHead(const char* pBuf,int iBufSize)
{
 
    if (NULL == m_pFile)
    {
        return;
    }

   long iLen = ftell(m_pFile); 
   if (iLen < 1)
   {
       return;
   }

   try
   {
       int iError = 0;
       char* szPbuf = new char[iLen+1];
       memset(szPbuf,0,iLen+1);

       iError = fseek(m_pFile,0,SEEK_SET);

       iError = fread(szPbuf,1,iLen,m_pFile);
       iError = fseek(m_pFile,0,SEEK_SET);

       iError = fwrite(pBuf,1,iBufSize,m_pFile);
       iError = fwrite(szPbuf,1,iLen,m_pFile);

       delete [] szPbuf;
   }
   catch (...)
   {
   	    printf("new or opr file exection\n");
   }
 
   

}


bool ZFile::GetLineInfo(char* pBuf, int iBufSize)
{
    if(NULL == m_pFile)
    {
        LOGS("file not open");
        return false;
    }

    return (NULL != fgets(pBuf,iBufSize,m_pFile))?true:false;
}

int ZFile::write(const char* pBuf,int iBufSize)
{
    if(NULL == m_pFile)
    {
        LOGS("file not open");
        return (-1);
    }

    return fwrite(pBuf,1,iBufSize,m_pFile);
}

int ZFile::fprintf( const char* format,...)
{
    if(NULL == m_pFile)
    {
        LOGS("file not open");
        return (-1);
    }

    return ::fprintf(m_pFile,format);
}


void ZFile::clear()
{
    if (NULL != m_pFile)
    {
        fclose(m_pFile);
        m_pFile = NULL;
    }
}

bool ZFile::open(const char* pMode)
{
	
    //LOGS("Begin Open File:");
    //LOGS(m_strFileName.c_str());
    try
    {
        clear();

        m_pFile = fopen(m_strFileName.c_str(),pMode);
        if (NULL == m_pFile)
        {
            LOGS("fopen file failed");
            return false;
        }

        //LOGS("open file success");
        return true;

    }
    catch (...)
    {
        LOGS("fopen file failed");
        m_pFile = NULL;
        return false;
    }


    return false;
}

