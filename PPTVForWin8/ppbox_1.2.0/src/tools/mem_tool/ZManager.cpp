#include <stdio.h>
#include "ZManager.h"
#include "ZCodeSession.h"
#include "ZDataSession.h"




ZManager::ZManager(void)
{
    m_pCode = NULL;
    m_pData = NULL;

    init();
}

void ZManager::init()
{
    CleanMem();
    m_pCode = new ZCodeSession;
    m_pData = new ZDataSession;
}

void ZManager::CleanMem()
{
    if (NULL != m_pCode)
    {
        delete m_pCode;
        m_pCode = NULL;
    }

    if (NULL != m_pData)
    {
        delete m_pData;
        m_pData = NULL;
    }
}


ZManager::~ZManager(void)
{
    CleanMem();  
}


const char* ZManager::GetType(const int iType)
{
    switch (iType)
    {
    case 1:
        return "Data";
        break;
    case 2:
        return "Code";
        break;
    default:
        return "Wrong Type";
        break;
    }

    return NULL;
}



void ZManager::insert(std::string strName,MEM_DATA& _data)
{

    if (_data.iType < 1 || 3 == _data.iType)
    {
        return;
    }

    switch (_data.iType)
    {
    case 1:
        m_pData->insert(strName,_data);
    	break;
    case 2:
        m_pCode->insert(strName,_data);
        break;
    default:
        break;
    }
}


void ZManager::AddTitle()
{
    m_pCode->AddTitle();
    m_pData->AddTitle();
}
void ZManager::ZPrint()
{
    // ZPrintCode();
    m_pCode->ZPrint();
    m_pData->ZPrint();
    //ZPrintData();
}


void ZManager::Clean()
{
    m_pCode->Clean();
    m_pData->Clean();
}




