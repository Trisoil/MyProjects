#pragma once
#include "CommonZ.h"
#include "ZSession.h"

class ZDataSession :public ZSession
{
public:
    ZDataSession();
    virtual ~ZDataSession();

    virtual bool IsWriteTile();

protected:
    static bool m_bFirst;
};
