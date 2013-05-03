#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Memconfig.h"

using namespace std;
namespace bi = boost::interprocess;

CMemConfigFile::CMemConfigFile()
{
    initflag_ = 0;
    pPPLiveMemConfig_ = NULL;
    configMap_.clear();
    constructMap();
}

//CCgiConfigFile:: CCgiConfigFile(const char* szFileName,key_t shmKey)
CMemConfigFile:: CMemConfigFile(const char* szFileName,const char* shmKey)
{
    pPPLiveMemConfig_ = NULL;
    initflag_ = 0;
    shmKey_ = shmKey;
    configMap_.clear();
    ParseFile(szFileName);
}

CMemConfigFile:: ~CMemConfigFile()
{
}

// delete pre- or -end white space
int CMemConfigFile:: StrimString(char* szLine)
{
    int i=0;
    int j=0;

    // ignor -end comment
    char *p;
    p= strstr(szLine, "//");
    if (p != NULL) {
        *p=0;
    }

    // delete -end white space
    j= strlen(szLine)-1;
    while ((szLine[j]==' ')||(szLine[j]=='\t')||(szLine[j]=='\n')||(szLine[j]=='\r'))
    {
        if (j == 0) return -1;
        szLine[j]=0;
        j--;
    }
    // delete pre- white space
    i=0; j=0;
    while ((szLine[j] == ' ')||(szLine[j] == '\t')) {
        if (szLine[j] == 0) return -1;
        j++;
    }
    // shift string
    while (szLine[j] != 0) {
        szLine[i] = szLine[j];
        i++;
        j++;
    }
    szLine[i]=0;

    // whole comment line
    if ((szLine[0] == '/')&&(szLine[1] == '/'))
        return 1;

    return 0;
}

int CMemConfigFile:: ParseFile(const char* szFileName)
{
    bool need_create = false;
    if (pPPLiveMemConfig_ == NULL)
    {
        //先尝试打开，如果打开失败，再创建		
        try
        {
#ifndef BOOST_WINDOWS_API
            shared_memory_ = SharedMemoryPtr(new bi::shared_memory_object(bi::open_only, shmKey_.c_str(), bi::read_write));
#else
            shared_memory_ = SharedMemoryPtr(new bi::windows_shared_memory(bi::open_only, shmKey_.c_str(), bi::read_only));
#endif			
            // region	
            mapped_region_ = boost::shared_ptr<boost::interprocess::mapped_region>(new bi::mapped_region(*shared_memory_, bi::read_write));
        }
        catch(const bi::interprocess_exception&)
        {
            need_create = true;
        }

        if (need_create)
        {
            try
            {
#ifndef BOOST_WINDOWS_API
                bi::shared_memory_object::remove(shmKey_.c_str());
                shared_memory_ = SharedMemoryPtr(new bi::shared_memory_object(bi::create_only, shmKey_.c_str(), bi::read_write));
                shared_memory_->truncate(sizeof(PPliveMemConfig));
#else
                shared_memory_ =SharedMemoryPtr(new bi::windows_shared_memory(bi::create_only, shmKey_.c_str(), bi::read_write, sizeof(PPliveMemConfig)));
#endif
                // region		
                mapped_region_ = boost::shared_ptr<boost::interprocess::mapped_region>(new bi::mapped_region(*shared_memory_, bi::read_write));
            }
            catch(const bi::interprocess_exception&)
            {				
                return -1;
            }
        }	

        if (mapped_region_->get_size() != sizeof(PPliveMemConfig))
        {
            return -2;
        }

        void*   pvShmAddr = mapped_region_->get_address();
        if ( !pvShmAddr || pvShmAddr == (void*)-1)  
        {
            return -3;
        }
        pPPLiveMemConfig_ = (PPliveMemConfig*)pvShmAddr; 
    }
    //	return ReadFile(szFileName);


    if (1 == pPPLiveMemConfig_->iInitFlag )
    {
        //已经加载过了,还需要构建map
    }
    else
    {
        //将文件内容读入到 pPPLiveMemConfig_ 里面
        ifstream configFile;
        configFile.open(szFileName);
        if (configFile.fail())
        {
            return -1;
        }

        char szLine[1024];
        char szSection[64], szParam[128];
        char *pColon;
        int  iLen;

        bool bFistIn = true;
        pPPLiveMemConfig_->iHeadCount = 0;

        while (configFile.getline(szLine, sizeof(szLine))) 
        {
            if (StrimString(szLine) != 0) continue;
            iLen = strlen(szLine);
            if (((szLine[0]=='[') && (szLine[iLen-1]==']')) ||
                ((szLine[0]=='<') && (szLine[iLen-1]=='>')) )
            {
                pColon = szLine+1;
                szLine[iLen-1] = 0;
                memset(szSection,0,sizeof(szSection));
                strncpy(szSection, pColon, sizeof(szSection));
                // section name
                pPPLiveMemConfig_->iHeadCount = pPPLiveMemConfig_->iHeadCount + 1;
                if (pPPLiveMemConfig_->iHeadCount >= MAX_NAME_COUNT)
                {
                    return -1;
                }
                sprintf(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 1], "%s", szSection);

                if (pPPLiveMemConfig_->iHeadCount == 1)
                {
                    //还没有其他section
                    sprintf(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 1] + MAX_NAME_COUNT,  "%d", 0);
                    sprintf(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 1] + MAX_NAME_COUNT + MAX_HEAD_VALUE_ONE_COUNT,"%d", 0);
                }
                else
                {
                    //已经有其他section
                    sprintf(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 1] + MAX_NAME_COUNT,  "%d",
                        atoi(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 2] + MAX_NAME_COUNT) + 
                        atoi(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 2] + MAX_NAME_COUNT + MAX_HEAD_VALUE_ONE_COUNT));
                    sprintf(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 1] + MAX_NAME_COUNT + MAX_HEAD_VALUE_ONE_COUNT,"%d", 0);				
                }
                continue;
            }
            if ((pColon= strstr(szLine, "=")) == NULL) {
                pColon= strstr(szLine, ":");
            }
            if (pColon == NULL) continue;
            *pColon=0;
            pColon++;
            if (StrimString(pColon) < 0) continue;
            if (StrimString(szLine) < 0) continue;
            if (szSection[0] != 0) 
            {
                sprintf(pPPLiveMemConfig_->chContentLine[atoi(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 1]  + MAX_NAME_COUNT) + atoi(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 1]  + MAX_NAME_COUNT + MAX_HEAD_VALUE_ONE_COUNT)], "%s", szLine);
                sprintf(pPPLiveMemConfig_->chContentLine[atoi(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 1]  + MAX_NAME_COUNT) + atoi(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 1]  + MAX_NAME_COUNT + MAX_HEAD_VALUE_ONE_COUNT)] + MAX_NAME_COUNT,  "%s", pColon);
                //length++
                sprintf(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 1] + MAX_NAME_COUNT + MAX_HEAD_VALUE_ONE_COUNT,
                    "%d", atoi(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 1]  + MAX_NAME_COUNT + MAX_HEAD_VALUE_ONE_COUNT) + 1);
                if (atoi(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 1] + MAX_NAME_COUNT + MAX_HEAD_VALUE_ONE_COUNT) >= MAX_CONTENT_COUNT)
                {
                    return -1;
                }
            }
            else 
            {			
                if (bFistIn)
                {
                    pPPLiveMemConfig_->iHeadCount = 1;
                    sprintf(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 1],  "%s", "NOSECTION");
                    sprintf(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 1] + MAX_NAME_COUNT,"%d", 0);
                    sprintf(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 1] + MAX_NAME_COUNT + MAX_HEAD_VALUE_ONE_COUNT,"%d", 0);
                    bFistIn = false;
                }
                sprintf(pPPLiveMemConfig_->chContentLine[atoi(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 1]  + MAX_NAME_COUNT) + atoi(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 1]  + MAX_NAME_COUNT + MAX_HEAD_VALUE_ONE_COUNT)], "%s", szLine);
                sprintf(pPPLiveMemConfig_->chContentLine[atoi(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 1]  + MAX_NAME_COUNT) + atoi(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 1]  + MAX_NAME_COUNT + MAX_HEAD_VALUE_ONE_COUNT)] + MAX_NAME_COUNT, "%s", pColon);
                //length++
                sprintf(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 1] + MAX_NAME_COUNT + MAX_HEAD_VALUE_ONE_COUNT,
                    "%d", atoi(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 1]  + MAX_NAME_COUNT + MAX_HEAD_VALUE_ONE_COUNT) + 1);
                if (atoi(pPPLiveMemConfig_->chHeadLine[pPPLiveMemConfig_->iHeadCount - 1] + MAX_NAME_COUNT + MAX_HEAD_VALUE_ONE_COUNT) >= MAX_CONTENT_COUNT)
                {					
                    return -1;
                }
            }
        }
        configFile.close();
        pPPLiveMemConfig_->iInitFlag = 1;
    }
    constructMap();

    return 0;
}

int CMemConfigFile::constructMap()
{
    if (initflag_ == 0)
    {
        if (pPPLiveMemConfig_ == NULL 
            || pPPLiveMemConfig_->iInitFlag != 1)
        {
            initflag_ = 0;
            return -1; 
        }

        // 遍历所有section-name-value配置，将配置缓存在m_configMap
        for(int i = 0; i < pPPLiveMemConfig_->iHeadCount; i++)
        {
            int offset = atoi(pPPLiveMemConfig_->chHeadLine[i] + MAX_NAME_COUNT);
            int count = atoi(pPPLiveMemConfig_->chHeadLine[i] 
            + MAX_NAME_COUNT 
                + MAX_HEAD_VALUE_ONE_COUNT);

            for(int j = offset;
                j < offset+count; 
                j++)
            {
                static char* fmt = "%s.%s";
                char  buff[1024] = {0};
                sprintf(buff, 
                    //sizeof(buff), 
                    fmt, 
                    pPPLiveMemConfig_->chHeadLine[i], 
                    pPPLiveMemConfig_->chContentLine[j]);

                configMap_[buff] = pPPLiveMemConfig_->chContentLine[j] + MAX_NAME_COUNT;
            }
        }
        initflag_ = 1;
    }
    return 0;
}

string CMemConfigFile::GetConfigFromMap(const char* szSection, const char* szName)
{
    if (constructMap() < 0)
    {
        return "";
    }

    static char* fmt = "%s.%s";
    char  buff[1024] = {0};
    //snprintf(buff, sizeof(buff), fmt, szSection, szName);
    sprintf(buff,fmt,szSection,szName);

    if (configMap_.find(buff) == configMap_.end())
    {
        return "";
    }

    return configMap_[buff];
}

string CMemConfigFile:: operator[](const char* szParam)
{
    if (szParam == NULL)
        return "";

    if (constructMap() < 0)
    {
        return "";
    }
    if (configMap_.find(szParam) == configMap_.end())
    {
        return "";
    }

    return configMap_[szParam];
}

string CMemConfigFile:: operator()(const char* szSection, const char* szName)
{
    if (szSection == NULL)
    {
        return GetConfigFromMap("NOSECTION",szName);
    }
    else
    {
        return GetConfigFromMap(szSection,szName);
    }
}

int CMemConfigFile::PrintfConfig()
{
    if (pPPLiveMemConfig_ == NULL || pPPLiveMemConfig_->iInitFlag != 1)
    {
        printf("error pPPLiveMemConfig_ == NULL || pPPLiveMemConfig_->iInitFlag != 1\n");
        return -1; 

    }

    for(int i = 0; i < pPPLiveMemConfig_->iHeadCount; i++)
    {
        int offset = atoi(pPPLiveMemConfig_->chHeadLine[i] + MAX_NAME_COUNT);
        int count = atoi(pPPLiveMemConfig_->chHeadLine[i] + MAX_NAME_COUNT + MAX_HEAD_VALUE_ONE_COUNT);
        printf("[%s] --from=%d len=%d\n",
            pPPLiveMemConfig_->chHeadLine[i],
            offset, count);

        for(int j = offset; 
            j < offset + count;
            j++)
        {
            printf("%s=%s\n",
                pPPLiveMemConfig_->chContentLine[j],
                pPPLiveMemConfig_->chContentLine[j] + MAX_NAME_COUNT);
        }
    }
    return 0;
}


