#pragma once
#include "CommonZ.h"
#include "ZSession.h"

class ZCodeSession :public ZSession
{
public:
    ZCodeSession();
    virtual ~ZCodeSession();

    virtual bool IsWriteTile();
protected:
    static bool m_bFirst;
};
