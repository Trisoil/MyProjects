//////////////////////////////////////////////////////////////////////////
//
// PpboxByteStreamHandler.h
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


#pragma once


//-------------------------------------------------------------------
// PpboxByteStreamHandler  class
//
// Byte-stream handler for Ppbox streams.
//-------------------------------------------------------------------

class PpboxHandler
    : public IMFSchemeHandler
{
public:
    static HRESULT CreateInstance(REFIID iid, void **ppMEG);

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    virtual HRESULT STDMETHODCALLTYPE BeginCreateObject( 
        /* [in] */ LPCWSTR pwszURL,
        /* [in] */ DWORD dwFlags,
        /* [in] */ IPropertyStore *pProps,
        /* [out] */ IUnknown **ppIUnknownCancelCookie,
        /* [in] */ IMFAsyncCallback *pCallback,
        /* [in] */ IUnknown *punkState);

    virtual HRESULT STDMETHODCALLTYPE EndCreateObject( 
        /* [in] */ IMFAsyncResult *pResult,
        /* [out] */ MF_OBJECT_TYPE *pObjectType,
        /* [out] */ IUnknown **ppObject);

    virtual HRESULT STDMETHODCALLTYPE CancelObjectCreation( 
        /* [in] */ IUnknown *pIUnknownCancelCookie);

private:
    static void __cdecl StaticOpenCallback(long err);

    void OpenCallback(HRESULT hr);

    PpboxHandler(HRESULT& hr);
    ~PpboxHandler();

    long            m_cRef; // reference count
    HRESULT         m_OpenResult;
    PpboxSource     *m_pSource;
    IMFAsyncResult  *m_pResult;
};

inline HRESULT PpboxSchemeHandler_CreateInstance(REFIID riid, void **ppv)
{
    return PpboxHandler::CreateInstance(riid, ppv);
}
