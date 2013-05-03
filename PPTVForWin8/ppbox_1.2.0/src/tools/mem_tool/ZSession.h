#pragma once
#include "CommonZ.h"
#include <vector>
#include "ZFile.h"


struct MEM_DATA
{
    MEM_DATA()
    {
        iType = 0;
		iSize = 0;
		iShareClean = 0;
		iShareDirty = 0;
		iPriveClean = 0;
		iPriveDirty = 0;
    }
    int iType; 
    unsigned int iSize;  
    unsigned int iRss;  
    unsigned int iShareClean;
    unsigned int iShareDirty;
    unsigned int iPriveClean;
    unsigned int iPriveDirty;
    std::string strName;  
    std::string strKey;    

};

typedef  std::vector<MEM_DATA> ZVecData;
typedef  std::vector<MEM_DATA>::iterator ZIter;




class ZSession
{
public:
    ZSession();
    virtual ~ZSession();  
    virtual void insert(std::string& strName,MEM_DATA& _data);
    virtual void ZPrint();

    virtual void Clean();

    virtual void AddTitle();

protected:
    void AddTotalFile();
    void AddSharedFile();
    void AddPhymemFile();

    void AddScleanFile();
    void AddSdiryFile();
    void AddPcleanFile();
    void AddpDirtyFile();
 
    
    void AddFile(ZFile& file,const char* pStr, const int iSize);

    //是否已经写入标题
    virtual bool IsWriteTile() = 0;
       

protected:

    

    ZVecData m_VecData;

    //ZFile m_fTotal;  

    ZFile m_fShared; //共享

    ZFile m_fPhymem;

    ZFile m_fSClean;  //共享
    ZFile m_fSDirty;

    ZFile m_fPClean; //私有
    ZFile m_fPDirty;



};
