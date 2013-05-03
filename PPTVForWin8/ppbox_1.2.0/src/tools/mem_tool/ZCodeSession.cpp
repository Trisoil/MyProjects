#include "ZCodeSession.h"
#include <string.h>


extern const char* g_cStr;

bool ZCodeSession::m_bFirst = false;

ZCodeSession::ZCodeSession()
{
    m_bFirst = false;

   /* m_fTotal.SetFileName(CODE_TOTAL);
    m_fTotal.open("wb");
    m_fTotal.write(g_cStr,strlen(g_cStr));*/


    m_fShared.SetFileName(CODE_SHARED);
    m_fShared.open("w+b");

    m_fPhymem.SetFileName(CODE_PHYMEM);
    m_fPhymem.open("w+b");

    m_fSClean.SetFileName(CODE_SCLEAN);
    m_fSClean.open("w+b");

    m_fSDirty.SetFileName(CODE_SDIRTY);
    m_fSDirty.open("w+b");

    m_fPClean.SetFileName(CODE_PCLEAN);
    m_fPClean.open("w+b");

    m_fPDirty.SetFileName(CODE_PDIRTY);
    m_fPDirty.open("w+b");   
}


bool ZCodeSession::IsWriteTile()
{
    if(m_bFirst)
    {
        return m_bFirst;

    }
    else
    {
        m_bFirst = true;
        return false;
    }

}

ZCodeSession::~ZCodeSession()
{

}
