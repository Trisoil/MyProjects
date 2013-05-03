#pragma once
#include "Common.h"

#include <string.h>


#include "ZSession.h"

//#include <vector>
//using namespace std;



class ZManager
{
public:
	ZManager(void);
	~ZManager(void);
	
    void insert(std::string strName,MEM_DATA& _data);

    void ZPrint();
    
    void Clean();

    void AddTitle();

    


protected:

    const char* GetType(const int iType);

    void init();
    void CleanMem();

private:

    ZSession* m_pCode;
    ZSession* m_pData;
};
