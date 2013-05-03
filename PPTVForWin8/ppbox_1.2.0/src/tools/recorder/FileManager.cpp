#include "tools/recorder/Common.h"
#include "tools/recorder/Define.h"

#include "tools/recorder/FileManager.h"

#include <boost/filesystem.hpp>
#include <framework/string/Format.h>
#include <framework/logger/LoggerStreamRecord.h>

#include <stack>

using namespace boost::filesystem;
using namespace framework::logger;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("FileManager", 0)

BEGIN_NAME_SPACE

FileManager::FileManager(std::string& dir_,std::string& cName_,long& size_):m_iFileIndex(1),
                                                    m_iM3uFileSize(size_),
                                                    m_strRootDir(dir_),
                                                    m_channelName(cName_)
{

    CreateDir();
    m_strTempFileName = m_strRootDir;
    m_strTempFileName += "/";
    m_strTempFileName += m_channelName;
    m_strTempFileName += "/";
    m_strTempFileName += m_channelName;
    m_strTempFileName += "_temp.ts";
    m_iWriteSize = 0;
}

void FileManager::CreateDir()
{
    std::string dir(m_strRootDir);
    dir += "/";
    dir += m_channelName;
    try
	{
	if(! create_directories(dir.c_str()))
    {
        getIndex();
    }
	}
	catch(...)
	{
        std::cout<<"Create directory Failed"<<std::endl;
		LOG_S(Logger::kLevelDebug, "Error create dir");
		throw(1);
	}
}

long FileManager::getFileIndex(const char* pFullName)
{
    if(NULL == pFullName) return 0;
    if (NULL != strstr(pFullName,"temp"))
    {
        return 0;
    }
    char szBuf[64] = {0};
    char* pBuf = szBuf;
    while(*pFullName != '\0' && *pFullName != '.')
    {
        *pBuf++ = *pFullName++;
    }
    long iReturn = 0;
    framework::string::parse2(szBuf,iReturn);
    return iReturn;
}

#ifdef WIN32

#include <Windows.h> 
#define TS_FINE_FORMAT "%s/%s/*.ts"

#else
#include <stddef.h>
#include <dirent.h>
#include <sys/types.h>
#define TS_FINE_FORMAT "%s%s/"

#endif



void FileManager::getIndex()
{ 
    m_iFileIndex = 0; //初始化为0，后面要做++

    long iTemp = 0;
    char szBuf[256] = {0};
    sprintf(szBuf,TS_FINE_FORMAT,m_strRootDir.c_str(),m_channelName.c_str());

#ifdef WIN32

    WIN32_FIND_DATA FindFileData;

    HANDLE hFind = FindFirstFile(szBuf, &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE) 
    {
        ++m_iFileIndex;
        return;
    } 

    iTemp = getFileIndex(FindFileData.cFileName);
    m_iFileIndex = (iTemp > m_iFileIndex)?iTemp:m_iFileIndex;
    
    do
    {
        //strcpy(result[i++], FindFileData.cFileName);
        iTemp = getFileIndex(FindFileData.cFileName);
        m_iFileIndex = (iTemp > m_iFileIndex)?iTemp:m_iFileIndex;
    }
    while (FindNextFile(hFind, &FindFileData) != 0);

#else

    DIR*   dp = NULL; 
    struct   dirent*   ep = NULL;   
    dp=opendir(szBuf);
    if(NULL != dp)
    {
        while(NULL != (ep = readdir(dp)))
        {
            if(ep->d_name[0] == '.' || NULL == strstr(ep->d_name,".ts") )
                continue;
            iTemp =  getFileIndex(ep->d_name); 
            m_iFileIndex = (iTemp > m_iFileIndex)?iTemp:m_iFileIndex;
        }   
        closedir(dp);
    }

#endif

    ++m_iFileIndex;

    LOG_S(Logger::kLevelDebug, "[Curr Index] " << m_iFileIndex);
}

#define TS_FORMAT "%s/%s/%ld.ts"

std::string FileManager::next_file_name(bool isAdd)
{
    char szBuf[256] = {0};
    if(!isAdd) m_iFileIndex--;  //重新写之前失败的文件
    sprintf(szBuf,TS_FORMAT,m_strRootDir.c_str(),m_channelName.c_str(),m_iFileIndex);
    m_iFileIndex++;
    return szBuf;
}

std::string FileManager::get_writing_name()
{
    char szBuf[256] = {0};
    sprintf(szBuf,TS_FORMAT,m_strRootDir.c_str(),m_channelName.c_str(),m_iFileIndex);
    ++m_iFileIndex;
    return szBuf;
}


#define M3U_FORMAT "%s/%s.m3u8"

#define M3U_BEGIN "#EXTM3U\r\n" \
                  "#EXT-X-TARGETDURATION:10\r\n" 


#define M3U_BEGIN_SEQ "#EXT-X-MEDIA-SEQUENCE:%ld\r\n"

#define M3U_MID "#EXTINF:10,\r\n" 

#define M3U_END "#EXT-X-ENDLIST"                   

#define NEW_LINE "\r\n"

void FileManager::WriteM3uBegin()
{
    if(m_m3uFile.is_open())
    {
        m_m3uFile.close();
    }
    try
    {
        char szBuf[256] = {0};
        sprintf(szBuf,M3U_FORMAT,m_strRootDir.c_str(),m_channelName.c_str());
        m_m3uFile.open(szBuf,std::ios::out| std::ios::binary );
        m_m3uFile.write(M3U_BEGIN,strlen(M3U_BEGIN));
    }
    catch(...)
    {
        if(m_m3uFile.is_open())
        {
            m_m3uFile.close();
        }
    }
}
    
void FileManager::WriteM3uEnd()
{
    //m_m3uFile.write(M3U_END,strlen(M3U_END));
    m_m3uFile.close();
}
#define M3U_MID_FORMAT "./%s/%ld.ts"
void FileManager::updateM3u()
{
    //写m3u文件

    WriteM3uBegin();

    char szBuf[256] = {0};

    std::stack<std::string> strFileList;
    std::string str;
    long jj = 0;// = m_iFileIndex - 1;    

    for(long ii = m_iFileIndex - 1 ; (ii > 0 && ii > (m_iFileIndex - (m_iM3uFileSize+1))) ; --ii)
    {
        sprintf(szBuf,TS_FORMAT,m_strRootDir.c_str(),m_channelName.c_str(),ii);
        if(!exists(szBuf))
        {
            break;
        }
        sprintf(szBuf,M3U_MID_FORMAT,m_channelName.c_str(),ii);

        
        strFileList.push(szBuf);
        jj = ii;
    }
    
    //写 m3u文件的SEQUENCE 值
    memset(szBuf,0,sizeof(szBuf));
    sprintf(szBuf,M3U_BEGIN_SEQ,jj);
    m_m3uFile.write(szBuf,strlen(szBuf));

    
    while(strFileList.size() > 0)
    {
        str = strFileList.top(); 
        m_m3uFile.write(M3U_MID,strlen(M3U_MID));
        m_m3uFile.write(str.c_str(),str.size());
        m_m3uFile.write(NEW_LINE,strlen(NEW_LINE));
        strFileList.pop();
    }

    WriteM3uEnd();
}


FileManager::~FileManager()
{

}

bool FileManager::Destory()
{
    try{
        m_iWriteSize = 0;
        if(m_tsFile.is_open())
        {
            m_tsFile.close();
        }
        m_tsFile.open(m_strTempFileName.c_str(),std::ios::out| std::ios::binary );
        return true;
    }
    catch(...)
    {
        return false;
    }
}

bool FileManager::changeFile()
{
    
    try
    {

        if(m_tsFile.is_open())
        {
            m_tsFile.close();
        }
        
        if (m_iWriteSize > 0)
        {
            boost::filesystem::rename(m_strTempFileName.c_str(),this->get_writing_name().c_str());
            updateM3u();
        }
        else
        {
            LOG_S(Logger::kLevelError, "Size is Not enough for changefile");
        }
        
        //打开临时文件继续写
        return Destory();
    }
    catch(...)
    {
        return false;
    }
}
bool FileManager::write(const char* strBuf,int iSize)
{
    try{
        if(!m_tsFile.is_open())
        {
            Destory();
        }
        //统计已写入大小
        m_iWriteSize += iSize;

        m_tsFile.write(strBuf,iSize);
        return true;
    }
    catch(...)
    {
        return false;
    }
}


END_NAME_SPACE
