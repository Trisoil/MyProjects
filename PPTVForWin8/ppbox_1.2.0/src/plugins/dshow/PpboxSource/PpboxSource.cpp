//------------------------------------------------------------------------------
// File: PushSourceBitmap.cpp
//
// Desc: DirectShow sample code - In-memory push mode source filter
//       Provides a static bitmap as the video output stream.
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#include "SampleCache.h"

#include "PpboxGuids.h"
#include "PpboxSource.h"
#include "PpboxPin.h"

/**********************************************
*
*  CPpboxSource Class
*
**********************************************/

CPpboxSource::CPpboxSource(IUnknown *pUnk, HRESULT *phr)
: CSource(NAME("PushSourceBitmap"), pUnk, CLSID_PpboxSource)
{
    *phr = S_OK;
    memset(m_pPin, 0, sizeof(m_pPin));
}

CPpboxSource::~CPpboxSource()
{
    DbgLog((LOG_TRACE, 5, _T("CPpboxSource::destructor()")));
    for (size_t i = 0; i < sizeof(m_pPin) / sizeof(m_pPin[0]); ++i) {
        if (m_pPin[i])
            delete m_pPin[i];
    }

    //PPBOX_StopP2PEngine();
}

STDMETHODIMP CPpboxSource::NonDelegatingQueryInterface(REFIID riid,
                                                       void **ppv)
{
    /* Do we have this interface */

    if (riid == IID_IFileSourceFilter) {
        return GetInterface((IFileSourceFilter *) this, ppv);
    } else {
        return CSource::NonDelegatingQueryInterface(riid, ppv);
    }
}

CPpboxSource * inst = NULL;

static int __cdecl init(int x, int y)
{
    return x + y;
}

STDMETHODIMP CPpboxSource::Load(LPCOLESTR pszFileName, 
                                const AM_MEDIA_TYPE *pmt)
{
    USES_CONVERSION;

    inst = this;

    m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_hMutex = CreateMutex(NULL, TRUE, NULL);

    DbgLog((LOG_TRACE, 5, _T("CPpboxSource::Load(%s)"), pszFileName));

    PPBOX_Close();

    LPCSTR pszPlaylink = W2A(pszFileName);
    PPBOX_AsyncOpen(pszPlaylink, &CPpboxSource::OpenCallback);

    IGraphBuilder * pGraphBuilder = NULL;
    m_pGraph->QueryInterface(IID_IGraphBuilder, (void **)&pGraphBuilder);
    
    HRESULT hr;

    while (TRUE) {
        DWORD dw = SignalObjectAndWait(m_hMutex, m_hEvent, 100, FALSE);
        WaitForSingleObject(m_hMutex, INFINITE);
        if (dw == WAIT_OBJECT_0) {
            hr = m_nOpenRet == ppbox_success ? S_OK : E_FAIL;
            break;
        } else if (pGraphBuilder && pGraphBuilder->ShouldOperationContinue() != S_OK) {
            hr = E_ABORT;
            break;
        }
    }

    pGraphBuilder->Release();

    if (hr == S_OK) {
        PP_uint32 n = PPBOX_GetStreamCount();
        for (PP_uint32 i = 0; i < n; ++i) {
            PPBOX_StreamInfoEx info;
            PPBOX_GetStreamInfoEx(i, &info);
            m_pPin[i] = new CPpboxPin(&hr, this, i, &info, &m_SampleCache);
        }
    }

    return hr;
}

void __cdecl CPpboxSource::OpenCallback(long err)
{
    WaitForSingleObject(inst->m_hMutex, INFINITE);
    inst->m_nOpenRet = err;
    SetEvent(inst->m_hEvent);
    ReleaseMutex(inst->m_hMutex);
}

STDMETHODIMP CPpboxSource::Stop()
{
    HRESULT hr = CBaseFilter::Stop();
    DbgLog((LOG_TRACE, 5, _T("CPpboxSource::Stop()")));
    //Ppbox::PPBOX_Close();
    return hr;
}

STDMETHODIMP CPpboxSource::GetCurFile(LPOLESTR *ppszFileName, AM_MEDIA_TYPE *pmt)
{
    return E_FAIL;
}

CUnknown * WINAPI CPpboxSource::CreateInstance(IUnknown *pUnk, HRESULT *phr)
{
    CPpboxSource *pNewFilter = new CPpboxSource(pUnk, phr);

    if (phr)
    {
        if (pNewFilter == NULL) 
            *phr = E_OUTOFMEMORY;
        else
            *phr = S_OK;
    }

    return pNewFilter;
}
