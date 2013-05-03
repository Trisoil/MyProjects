#include "ZDataSession.h"
#include <string.h>

extern const char* g_cStr;

bool ZDataSession::m_bFirst = false;

ZDataSession::ZDataSession()
{

    m_bFirst = false;

    /*m_fTotal.SetFileName(DATA_TOTAL);
    m_fTotal.open("wb");

    m_fTotal.write(g_cStr,strlen(g_cStr));*/

    m_fShared.SetFileName(DATA_SHARED);
    m_fShared.open("w+b");

    m_fPhymem.SetFileName(DATA_PHYMEM);
    m_fPhymem.open("w+b");

    m_fSClean.SetFileName(DATA_SCLEAN);
    m_fSClean.open("w+b");

    m_fSDirty.SetFileName(DATA_SDIRTY);
    m_fSDirty.open("w+b");

    m_fPClean.SetFileName(DATA_PCLEAN);
    m_fPClean.open("w+b");

    m_fPDirty.SetFileName(DATA_PDIRTY);
    m_fPDirty.open("w+b");
}

ZDataSession::~ZDataSession()
{

}

bool ZDataSession::IsWriteTile()
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
