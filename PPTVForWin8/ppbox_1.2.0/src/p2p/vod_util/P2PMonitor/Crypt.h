#ifndef _CRYPT_H
#define _CRYPT_H

#include "afxwin.h"
class CCrypt
{
public:

    CString Encrypt(CString S, WORD Key);
    CString Decrypt(CString S, WORD Key);
};

#endif