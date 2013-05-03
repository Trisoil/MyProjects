//------------------------------------------------------------------------------
// File: PpboxSource.H
//
// Desc: DirectShow sample code - In-memory push mode source filter
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

// Filter name strings
#define g_wszPpbox     L"PpboxSource Filter"


/**********************************************
 *
 *  Class declarations
 *
 **********************************************/

class CPpboxPin;

class CPpboxSource : public CSource, public IFileSourceFilter
{

private:
    // Constructor is private because you have to use CreateInstance
    CPpboxSource(IUnknown *pUnk, HRESULT *phr);
    ~CPpboxSource();

    CPpboxPin * m_pPin[4];

    static void __cdecl OpenCallback(long err);

    HANDLE m_hEvent;
    HANDLE m_hMutex;
    int m_nOpenRet;
    SampleCache m_SampleCache;

public:
    static CUnknown * WINAPI CreateInstance(IUnknown *pUnk, HRESULT *phr);  

    // override this to say what interfaces we support where
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

    STDMETHODIMP Stop();

    DECLARE_IUNKNOWN

    STDMETHODIMP Load(LPCOLESTR pszFileName, const AM_MEDIA_TYPE *pmt);

    STDMETHODIMP GetCurFile(LPOLESTR *ppszFileName, AM_MEDIA_TYPE *pmt);
};
