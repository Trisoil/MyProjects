//////////////////////////////////////////////////////////////////////////
//
// PpboxStream.cpp
// Implements the stream object (IMFMediaStream) for the Ppbox source.
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

#pragma warning( push )
#pragma warning( disable : 4355 )  // 'this' used in base member initializer list


/* PpboxStream::SourceLock class methods */

//-------------------------------------------------------------------
// PpboxStream::SourceLock constructor - locks the source
//-------------------------------------------------------------------

PpboxStream::SourceLock::SourceLock(PpboxSource *pSource)
    : m_pSource(NULL)
{
    if (pSource)
    {
        m_pSource = pSource;
        m_pSource->AddRef();
        m_pSource->Lock();
    }
}

//-------------------------------------------------------------------
// PpboxStream::SourceLock destructor - unlocks the source
//-------------------------------------------------------------------

PpboxStream::SourceLock::~SourceLock()
{
    if (m_pSource)
    {
        m_pSource->Unlock();
        m_pSource->Release();
    }
}



/* Public class methods */

//-------------------------------------------------------------------
// IUnknown methods
//-------------------------------------------------------------------

ULONG PpboxStream::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

ULONG PpboxStream::Release()
{
    LONG cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0)
    {
        delete this;
    }
    return cRef;
}

HRESULT PpboxStream::QueryInterface(REFIID riid, void** ppv)
{
    static const QITAB qit[] =
    {
        QITABENT(PpboxStream, IMFMediaEventGenerator),
        QITABENT(PpboxStream, IMFMediaStream),
        { 0 }
    };
    return QISearch(this, qit, riid, ppv);
}


//-------------------------------------------------------------------
// IMFMediaEventGenerator methods
//
// For remarks, see PpboxSource.cpp
//-------------------------------------------------------------------

HRESULT PpboxStream::BeginGetEvent(IMFAsyncCallback* pCallback,IUnknown* punkState)
{
    HRESULT hr = S_OK;

    SourceLock lock(m_pSource);

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        hr = m_pEventQueue->BeginGetEvent(pCallback, punkState);
    }

    return hr;
}

HRESULT PpboxStream::EndGetEvent(IMFAsyncResult* pResult, IMFMediaEvent** ppEvent)
{
    HRESULT hr = S_OK;

    SourceLock lock(m_pSource);

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        hr = m_pEventQueue->EndGetEvent(pResult, ppEvent);
    }

    return hr;
}

HRESULT PpboxStream::GetEvent(DWORD dwFlags, IMFMediaEvent** ppEvent)
{
    HRESULT hr = S_OK;

    IMFMediaEventQueue *pQueue = NULL;

    { // scope for lock

        SourceLock lock(m_pSource);

        // Check shutdown
        hr = CheckShutdown();

        // Cache a local pointer to the queue.
        if (SUCCEEDED(hr))
        {
            pQueue = m_pEventQueue;
            pQueue->AddRef();
        }
    }   // release lock

    // Use the local pointer to call GetEvent.
    if (SUCCEEDED(hr))
    {
        hr = pQueue->GetEvent(dwFlags, ppEvent);
    }

    SafeRelease(&pQueue);
    return hr;
}

HRESULT PpboxStream::QueueEvent(MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, const PROPVARIANT* pvValue)
{
    HRESULT hr = S_OK;

    SourceLock lock(m_pSource);

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        hr = m_pEventQueue->QueueEventParamVar(met, guidExtendedType, hrStatus, pvValue);
    }

    return hr;
}

//-------------------------------------------------------------------
// IMFMediaStream methods
//-------------------------------------------------------------------


//-------------------------------------------------------------------
// GetMediaSource:
// Returns a pointer to the media source.
//-------------------------------------------------------------------

HRESULT PpboxStream::GetMediaSource(IMFMediaSource** ppMediaSource)
{
    SourceLock lock(m_pSource);

    if (ppMediaSource == NULL)
    {
        return E_POINTER;
    }

    if (m_pSource == NULL)
    {
        return E_UNEXPECTED;
    }

    HRESULT hr = S_OK;

    hr = CheckShutdown();

    // QI the source for IMFMediaSource.
    // (Does not hold the source's critical section.)
    if (SUCCEEDED(hr))
    {
        hr = m_pSource->QueryInterface(IID_PPV_ARGS(ppMediaSource));
    }
    return hr;
}


//-------------------------------------------------------------------
// GetStreamDescriptor:
// Returns a pointer to the stream descriptor for this stream.
//-------------------------------------------------------------------

HRESULT PpboxStream::GetStreamDescriptor(IMFStreamDescriptor** ppStreamDescriptor)
{
    SourceLock lock(m_pSource);

    if (ppStreamDescriptor == NULL)
    {
        return E_POINTER;
    }

    if (m_pStreamDescriptor == NULL)
    {
        return E_UNEXPECTED;
    }

    HRESULT hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        *ppStreamDescriptor = m_pStreamDescriptor;
        (*ppStreamDescriptor)->AddRef();

    };
    return hr;
}


//-------------------------------------------------------------------
// RequestSample:
// Requests data from the stream.
//
// pToken: Token used to track the request. Can be NULL.
//-------------------------------------------------------------------

HRESULT PpboxStream::RequestSample(IUnknown* pToken)
{
    HRESULT hr = S_OK;
    IMFMediaSource *pSource = NULL;

    // Hold the media source object's critical section.
    SourceLock lock(m_pSource);

    hr = CheckShutdown();
    if (FAILED(hr))
    {
        goto done;
    }

    if (m_state == STATE_STOPPED)
    {
        hr = MF_E_INVALIDREQUEST;
        goto done;
    }

    if (!m_bActive)
    {
        // If the stream is not active, it should not get sample requests.
        hr = MF_E_INVALIDREQUEST;
        goto done;
    }

    if (m_bEOS && m_Samples.IsEmpty())
    {
        // This stream has already reached the end of the stream, and the
        // sample queue is empty.
        hr = MF_E_END_OF_STREAM;
        goto done;
    }

    hr = m_Requests.InsertBack(pToken);
    if (FAILED(hr))
    {
        goto done;
    }

    // Dispatch the request.
    hr = DispatchSamples();
    if (FAILED(hr))
    {
        goto done;
    }

done:
    if (FAILED(hr) && (m_state != STATE_SHUTDOWN))
    {
        // An error occurred. Send an MEError even from the source,
        // unless the source is already shut down.
        hr = m_pSource->QueueEvent(MEError, GUID_NULL, hr, NULL);
    }
    return hr;
}



//-------------------------------------------------------------------
// Public non-interface methods
//-------------------------------------------------------------------


PpboxStream::PpboxStream(PpboxSource *pSource, IMFStreamDescriptor *pSD, HRESULT& hr) :
    m_cRef(1),
    m_pEventQueue(NULL),
    m_state(STATE_STOPPED),
    m_bActive(FALSE),
    m_bEOS(FALSE)
{
    DllAddRef();

    assert(pSource != NULL);
    assert(pSD != NULL);

    m_pSource = pSource;
    m_pSource->AddRef();

    m_pStreamDescriptor = pSD;
    m_pStreamDescriptor->AddRef();

    // Create the media event queue.
    hr = MFCreateEventQueue(&m_pEventQueue);
}

PpboxStream::~PpboxStream()
{
    assert(m_state == STATE_SHUTDOWN);
    SafeRelease(&m_pSource);

    DllRelease();
}



//-------------------------------------------------------------------
// Activate
// Activates or deactivates the stream. Called by the media source.
//-------------------------------------------------------------------

HRESULT PpboxStream::Activate(BOOL bActive)
{
    SourceLock lock(m_pSource);

    if (bActive == m_bActive)
    {
        return S_OK; // No op
    }

    m_bActive = bActive;

    if (!bActive)
    {
        m_Samples.Clear();
        m_Requests.Clear();
    }
    return S_OK;
}


//-------------------------------------------------------------------
// Start
// Starts the stream. Called by the media source.
//
// varStart: Starting position.
//-------------------------------------------------------------------

HRESULT PpboxStream::Start(const PROPVARIANT& varStart)
{
    SourceLock lock(m_pSource);

    HRESULT hr = S_OK;

    hr = CheckShutdown();

    // Queue the stream-started event.
    if (SUCCEEDED(hr))
    {
        hr = QueueEvent(MEStreamStarted, GUID_NULL, S_OK, &varStart);
    }

    if (SUCCEEDED(hr))
    {
        m_state = STATE_STARTED;
    }

    // If we are restarting from paused, there may be
    // queue sample requests. Dispatch them now.
    if (SUCCEEDED(hr))
    {
        hr = DispatchSamples();
    }
    return hr;
}


//-------------------------------------------------------------------
// Pause
// Pauses the stream. Called by the media source.
//-------------------------------------------------------------------

HRESULT PpboxStream::Pause()
{
    SourceLock lock(m_pSource);

    HRESULT hr = S_OK;

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        m_state = STATE_PAUSED;

        hr = QueueEvent(MEStreamPaused, GUID_NULL, S_OK, NULL);
    }

    return hr;
}


//-------------------------------------------------------------------
// Stop
// Stops the stream. Called by the media source.
//-------------------------------------------------------------------

HRESULT PpboxStream::Stop()
{
    SourceLock lock(m_pSource);

    HRESULT hr = S_OK;

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        m_Requests.Clear();
        m_Samples.Clear();

        m_state = STATE_STOPPED;

        hr = QueueEvent(MEStreamStopped, GUID_NULL, S_OK, NULL);
    }

    return hr;
}


//-------------------------------------------------------------------
// EndOfStream
// Notifies the stream that the source reached the end of the Ppbox
// stream. For more information, see PpboxSource::EndOfMPEGStream().
//-------------------------------------------------------------------

HRESULT PpboxStream::EndOfStream()
{
    SourceLock lock(m_pSource);

    m_bEOS = TRUE;

    return DispatchSamples();
}


//-------------------------------------------------------------------
// Shutdown
// Shuts down the stream and releases all resources.
//-------------------------------------------------------------------

HRESULT PpboxStream::Shutdown()
{
    SourceLock lock(m_pSource);

    HRESULT hr = S_OK;

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        m_state = STATE_SHUTDOWN;

        // Shut down the event queue.
        if (m_pEventQueue)
        {
            m_pEventQueue->Shutdown();
        }

        // Release objects.
        m_Samples.Clear();
        m_Requests.Clear();

        SafeRelease(&m_pStreamDescriptor);
        SafeRelease(&m_pEventQueue);

        // NOTE:
        // Do NOT release the source pointer here, because the stream uses
        // it to hold the critical section. In particular, the stream must
        // hold the critical section when checking the shutdown status,
        // which obviously can occur after the stream is shut down.

        // It is OK to hold a ref count on the source after shutdown,
        // because the source releases its ref count(s) on the streams,
        // which breaks the circular ref count.
    }

    return hr;
}


//-------------------------------------------------------------------
// NeedsData
// Returns TRUE if the stream needs more data.
//-------------------------------------------------------------------

BOOL PpboxStream::NeedsData()
{
    SourceLock lock(m_pSource);

    // Note: The stream tries to keep a minimum number of samples
    // queued ahead.

    return (m_bActive && !m_bEOS && (m_Samples.GetCount() < SAMPLE_QUEUE));
}


//-------------------------------------------------------------------
// DeliverPayload
// Delivers a sample to the stream.
//-------------------------------------------------------------------

HRESULT PpboxStream::DeliverPayload(IMFSample *pSample)
{
    SourceLock lock(m_pSource);

    HRESULT hr = S_OK;

    // Queue the sample.
    hr = m_Samples.InsertBack(pSample);

    // Deliver the sample if there is an outstanding request.
    if (SUCCEEDED(hr))
    {
        hr = DispatchSamples();
    }

    return hr;
}

/* Private methods */

//-------------------------------------------------------------------
// DispatchSamples
// Dispatches as many pending sample requests as possible.
//-------------------------------------------------------------------

HRESULT PpboxStream::DispatchSamples()
{
    HRESULT hr = S_OK;
    BOOL bNeedData = FALSE;
    BOOL bEOS = FALSE;

    SourceLock lock(m_pSource);

    // An I/O request can complete after the source is paused, stopped, or
    // shut down. Do not deliver samples unless the source is running.
    if (m_state != STATE_STARTED)
    {
        return S_OK;
    }

    IMFSample *pSample = NULL;
    IUnknown  *pToken = NULL;

    // Deliver as many samples as we can.
    while (!m_Samples.IsEmpty() && !m_Requests.IsEmpty())
    {
        // Pull the next sample from the queue.
        hr = m_Samples.RemoveFront(&pSample);
        if (FAILED(hr))
        {
            goto done;
        }

        // Pull the next request token from the queue. Tokens can be NULL.
        hr = m_Requests.RemoveFront(&pToken);
        if (FAILED(hr))
        {
            goto done;
        }

        if (pToken)
        {
            // Set the token on the sample.
            hr = pSample->SetUnknown(MFSampleExtension_Token, pToken);
            if (FAILED(hr))
            {
                goto done;
            }
        }

        // Send an MEMediaSample event with the sample.
        hr = m_pEventQueue->QueueEventParamUnk(
            MEMediaSample, GUID_NULL, S_OK, pSample);

        if (FAILED(hr))
        {
            goto done;
        }

        SafeRelease(&pSample);
        SafeRelease(&pToken);
    }

    if (m_Samples.IsEmpty() && m_bEOS)
    {
        // The sample queue is empty AND we have reached the end of the source
        // stream. Notify the pipeline by sending the end-of-stream event.

        hr = m_pEventQueue->QueueEventParamVar(
            MEEndOfStream, GUID_NULL, S_OK, NULL);

        if (FAILED(hr))
        {
            goto done;
        }

        // Notify the source. It will send the end-of-presentation event.
        hr = m_pSource->RequestSample();
        if (FAILED(hr))
        {
            goto done;
        }
    }
    else if (NeedsData())
    {
        // The sample queue is empty; the request queue is not empty; and we
        // have not reached the end of the stream. Ask for more data.
        hr = m_pSource->RequestSample();
        if (FAILED(hr))
        {
            goto done;
        }
    } else {
        hr = S_OK;
        goto done;
    }

done:
    if (FAILED(hr) && (m_state != STATE_SHUTDOWN))
    {
        // An error occurred. Send an MEError even from the source,
        // unless the source is already shut down.
        m_pSource->QueueEvent(MEError, GUID_NULL, hr, NULL);
    }

    SafeRelease(&pSample);
    SafeRelease(&pToken);
    return S_OK;
}

#pragma warning( pop )