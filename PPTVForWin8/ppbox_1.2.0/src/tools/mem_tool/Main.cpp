#include <stdio.h>

#include <stdlib.h>
#include "ZFile.h"
#include "ZManager.h"
#include <string.h>


#include <sys/sysinfo.h>


#include "tools/mem_tool/Common.h" 

#include <framework/memory/PrivateMemory.h>

#include <iostream>

using namespace framework::memory;


unsigned int GetKB(const char* pData)
{
    //string like   Size:                 40 kB
    char szBuf[32] = {0};
    const char *pFind = strchr(pData,' ');
    char *pVaule = szBuf;
    if (NULL == pFind)
    {
        LOGS("Can not find KB lind");
        return 0;
    }

    while (' ' == *pFind)
    {
        pFind++;
    }

    while (' ' != *pFind)
    {
        *pVaule++ = *pFind++;
    }


    return atol(szBuf);
}

#define COMP(p,temp) strncmp(p,temp,strlen(temp))



//Êý¾ÝÌî³ä
void GetNumData(const char* pData,MEM_DATA& _data)
{

    unsigned int iSize = GetKB(pData);

    if (0 == COMP(pData,KB_SIZE))
    {
        _data.iSize = iSize;
    } 
    else if (0 == COMP(pData,KB_RSS))
    {
        _data.iRss = iSize;
    }
    else if (0 == COMP(pData,KB_SCLEAN))
    {
        _data.iShareClean = iSize;
    }
    else if (0 == COMP(pData,KB_SDIRTY))
    {
        _data.iShareDirty = iSize;
    }
    else if (0 == COMP(pData,KB_PCLEAN))
    {
        _data.iPriveClean = iSize;
    }
    else if (0 == COMP(pData,KB_PDIRTY))
    {
        _data.iPriveDirty = iSize;
    }
    else
    {
        // LOGS("Wrong KB line");
    }

}

void GetRW(const char* pData,char* pRw)
{
    const char* pBefore = pData;
	

    while ('\0' != *pBefore && ' ' != *pBefore)
    {
        pBefore++;
    };
    
    while ('\0' != *pBefore && ' ' == *pBefore)
    {
        pBefore++;
    };

    while ('\0' != *pBefore && ' ' != *pBefore)
    {
        *pRw = *pBefore;
        pRw++ ;
        pBefore++;
    };

}


void GetNameData(const char* pData,char* pName,MEM_DATA& _data)
{
    if(strlen(pData) < 5)
	{
	    _data.iType = 3;
	    return;
	}
    char szName[64] = "";
	char szWR[10] = {0};

    strncpy(pName,pData,8);
    _data.strKey = pName;
    if (NULL == strstr(pData,TYPE_KEY))
    {
        _data.iType = 1;
    }
    else
    {
        _data.iType = 2;
    }

	GetRW(pData,szWR);    
    _data.strName = szWR;
    _data.strName += "_";

    const char* pBefore = strchr(pData,':');
    if (NULL == pBefore)
    {
        _data.iType = 0;
        LOGS("Format is wrong");
        return;
    }

    while ('\0' != *pBefore && ' ' != *pBefore)
    {
        pBefore++;
    };

    while ('\0' != *pBefore && ' ' == *pBefore)
    {
        pBefore++;
    };

    while ('\0' != *pBefore && ' ' != *pBefore)
    {
        pBefore++;
    };

    while ('\0' != *pBefore && ' ' == *pBefore)
    {
        pBefore++;
    };

    if ( 0 == strcmp(pBefore,""))
    {
        //strcpy(pName, "Owner");
       // _data.strName = "";
    }
    else
    {
        const char* pFineName = NULL;
        const char* pNow = pBefore;
        while (NULL != (pFineName = strchr(pBefore,'/')) )
        {
            pBefore = pFineName+1;
            pNow = pBefore;

        }

        strcpy(szName,pNow);
        _data.strName += szName;
    }


}

#define FROMAT_STR "/proc/%s/smaps"

void  Exec(ZManager& zMan, const char* proID)
{
    MEM_DATA zData;
    //LOGS(proID);	
   // CLEAN_BUF(&zData,sizeof(MEM_DATA));

    char szName[BUF_SIZE] = {0};
    char szFileName[1024] = {0};
    sprintf(szFileName,FROMAT_STR,proID);	

    ZFile file(szFileName);

    char szBuf[BUF_SIZE] = "";

    char *pEnter = NULL;


    while ( file.GetLineInfo(szBuf,BUF_SIZE) )
    {

        pEnter = strchr(szBuf,'\r');
        if (NULL != pEnter)
        {
            *pEnter = '\0';
        }

        pEnter = strchr(szBuf,'\n');
        if (NULL != pEnter)
        {
            *pEnter = '\0';
        }

        if (NULL == strstr(szBuf,HEAD_KEY) )
        {
            //Head part
            //            ZPrintf(szName,zData);
            zMan.insert(szName,zData);

            CLEAN_BUF(szName,BUF_SIZE);

            GetNameData(szBuf,szName,zData);

            CLEAN_BUF(szBuf,BUF_SIZE);
            continue;
        }

        //input others param
        GetNumData(szBuf,zData);


        CLEAN_BUF(szBuf,BUF_SIZE);
    }  

    zMan.insert(szName,zData); 

}



struct sysinfo s_info;
//Get Free Memory Size
long GetFreeMemSize()
{
	long iMemSize = 0;

	memset(&s_info,0,sizeof(s_info));
	int error;
	error = ::sysinfo(&s_info);
	if(0 != error)
	{
		std::cout<<"Get Free Memory Failed"<<std::endl;
		return (-1);
	}	

	iMemSize = (size_t)s_info.freeram;
	

	return iMemSize;
}

//Alloc Mem
char* AllocMem(PrivateMemory& memMang, size_t iSize)
{
	size_t iFreeMem = GetFreeMemSize();
	std::cout<<"MemSize Befor(byte):"<<iFreeMem<<std::endl;

	return (char*)memMang.alloc_block(iSize);		
}

//Left Mem
char* LeaveMem(PrivateMemory& memMang,size_t& iSize)
{
	size_t iFreeMem = GetFreeMemSize();
	std::cout<<"MemSize Befor(byte):"<<iFreeMem<<std::endl;
	iSize = iFreeMem - iSize;	
	if(iSize < 1)
	{
		std::cout<<"Leave < Need Leave Vaule"<<std::endl;
		return NULL;
	}
	return (char*)memMang.alloc_block(iSize);	
}


int MemNew(int argc, char * argv[])
{
    size_t iType = atol(argv[1]);
    size_t iMemSize = atol(argv[2]);

    if(iType > 2 || iType < 1 ||  1 > iMemSize)
    {
        std::cout<<"Param is wrong"<<std::endl;
        return 0;
    }


    PrivateMemory memMang;

    char* pBuf = NULL;

    switch(iType)
    {
    case 1:
        pBuf = AllocMem(memMang,iMemSize);
        break;
    default:
        pBuf = LeaveMem(memMang,iMemSize);
        break;
    }


    if(NULL == pBuf)
    {
        std::cout<<"Alloc Mem Failed"<<std::endl;
        return 0;
    }	


    std::cout<<"MemSize Alloc(byte):"<<GetFreeMemSize()<<std::endl;

    //	char a;
    //	std::cin>>a;

    std::cin.get();	

    memMang.free_block((void*)pBuf,iMemSize);

    std::cout<<"MemSize Free (byte):"<<GetFreeMemSize()<<std::endl;
    return 0;
}

int MemCheck(int argc, char * argv[])
{
    int iTime = atoi(argv[2]);
    int iCount = atoi(argv[3]);

    printf("PID:%s,Time:%d,Count:%d\n",argv[1],iTime,iCount);


    ZManager zMan;

    while(iCount-- > 0)
    {

        printf("Exec left count: %d\n",iCount);
        Exec(zMan,argv[1]);

        zMan.Clean();

        sleep(iTime);
    }

    zMan.AddTitle();

    return 0;
}


int main(int argc, char * argv[])
{
	if(3 == argc)
	{
        return MemNew(argc,argv);
	}	
    else if(4 == argc)
    {
        return MemCheck(argc,argv);
    }

    
    printf("MemNew  *.exe a b\n  a: 1 new 2 left  b:size (BYTE)\n");

    printf("MemCheck  *.exe a b c\n  a: PID  b:Timer c: exec count\n");
	

	return 0;
}
	
