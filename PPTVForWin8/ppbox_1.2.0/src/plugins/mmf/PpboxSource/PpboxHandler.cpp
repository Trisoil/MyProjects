//////////////////////////////////////////////////////////////////////////
//
// PpboxByteStreamHandler.cpp
// Implements the byte-stream handler for the Ppbox source.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////


#include "PpboxSource.h"
#include "PpboxHandler.h"

#include <atlconv.h>

#include "ppbox/ppbox.h"

//-------------------------------------------------------------------
// PpboxByteStreamHandler  class
//-------------------------------------------------------------------


//-------------------------------------------------------------------
// CreateInstance
// Static method to create an instance of the oject.
//
// This method is used by the class factory.
//
//-------------------------------------------------------------------

HRESULT PpboxHandler::CreateInstance(REFIID iid, void **ppv)
{
    if (ppv == NULL)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;

    PpboxHandler *pHandler = new (std::nothrow) PpboxHandler(hr);
    if (pHandler == NULL)
    {
        return E_OUTOFMEMORY;
    }

    if (SUCCEEDED(hr))
    {
        hr = pHandler->QueryInterface(iid, ppv);
    }

    SafeRelease(&pHandler);
    return hr;
}


//-------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------

PpboxHandler::PpboxHandler(HRESULT& hr)
    : m_cRef(1), m_pSource(NULL), m_pResult(NULL)
{
    DllAddRef();
}

//-------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------

PpboxHandler::~PpboxHandler()
{
    SafeRelease(&m_pSource);
    SafeRelease(&m_pResult);

    DllRelease();
}


//-------------------------------------------------------------------
// IUnknown methods
//-------------------------------------------------------------------

ULONG PpboxHandler::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

ULONG PpboxHandler::Release()
{
    LONG cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0)
    {
        delete this;
    }
    return cRef;
}

HRESULT PpboxHandler::QueryInterface(REFIID riid, void** ppv)
{
    static const QITAB qit[] =
    {
        QITABENT(PpboxHandler, IMFSchemeHandler),
        { 0 }
    };
    return QISearch(this, qit, riid, ppv);
}


//-------------------------------------------------------------------
// IMFByteStreamHandler methods
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// BeginCreateObject
// Starts creating the media source.
//-------------------------------------------------------------------

static PpboxHandler * inst = NULL;

HRESULT PpboxHandler::BeginCreateObject(
    /* [in] */ LPCWSTR pwszURL,
    /* [in] */ DWORD dwFlags,
    /* [in] */ IPropertyStore *pProps,
    /* [out] */ IUnknown **ppIUnknownCancelCookie,
    /* [in] */ IMFAsyncCallback *pCallback,
    /* [in] */ IUnknown *punkState
    )
{
    USES_CONVERSION;

    if (pCallback == NULL)
    {
        return E_POINTER;
    }

    if ((dwFlags & MF_RESOLUTION_MEDIASOURCE) == 0)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    IMFAsyncResult * pResult = NULL;

    PpboxSource    *pSource = NULL;

    inst = this;

    hr = MFCreateAsyncResult(NULL, pCallback, punkState, &pResult);

    if (SUCCEEDED(hr))
    {
        m_pResult = pResult;
        m_pResult->AddRef();

        LPCSTR pszPlaylink = W2A(pwszURL);
        PPBOX_AsyncOpen()(pszPlaylink, &PpboxHandler::StaticOpenCallback);
    }

    return hr;
}

void __cdecl PpboxHandler::StaticOpenCallback(long err)
{
    if (err != ppbox_success)
    {
        PPBOX_Close()();
    }
    inst->OpenCallback(err == ppbox_success ? S_OK : E_FAIL);
}

void PpboxHandler::OpenCallback(HRESULT hr)
{
    PpboxSource    *pSource = NULL;

    if (SUCCEEDED(hr)) {
        // Create an instance of the media source.
        hr = PpboxSource::CreateInstance(&pSource);
    }

    if (SUCCEEDED(hr)) {
        m_pSource = pSource;
        m_pSource->AddRef();
    }

    m_pResult->SetStatus(hr);

    MFInvokeCallback(m_pResult);

    SafeRelease(&pSource);
}

//-------------------------------------------------------------------
// EndCreateObject
// Completes the BeginCreateObject operation.
//-------------------------------------------------------------------

HRESULT PpboxHandler::EndCreateObject(
        /* [in] */ IMFAsyncResult *pResult,
        /* [out] */ MF_OBJECT_TYPE *pObjectType,
        /* [out] */ IUnknown **ppObject)
{
    if (pResult == NULL || pObjectType == NULL || ppObject == NULL)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;

    *pObjectType = MF_OBJECT_INVALID;
    *ppObject = NULL;

    hr = pResult->GetStatus();

    if (SUCCEEDED(hr))
    {
        *pObjectType = MF_OBJECT_MEDIASOURCE;
        assert(m_pSource != NULL);
        hr = m_pSource->QueryInterface(IID_PPV_ARGS(ppObject));
    }

    SafeRelease(&m_pSource);
    SafeRelease(&m_pResult);

    return hr;
}

HRESULT PpboxHandler::CancelObjectCreation(
    IUnknown *pIUnknownCancelCookie)
{
    PPBOX_Close()();
    return S_OK;
}
