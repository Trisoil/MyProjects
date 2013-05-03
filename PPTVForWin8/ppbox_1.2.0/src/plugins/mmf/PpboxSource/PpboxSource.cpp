//////////////////////////////////////////////////////////////////////////
//
// PpboxSource.h
// Implements the Ppbox media source object.
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
#include <InitGuid.h>
#include <wmcodecdsp.h>

#define PPBOX_EXTERN
#include "ppbox/ppbox.h"
//-------------------------------------------------------------------
//
// Notes:
// This sample contains an Ppbox source.
//
// - The source parses Ppbox systems-layer streams and generates
//   samples that contain Ppbox payloads.
// - The source does not support files that contain a raw Ppbox
//   video or audio stream.
// - The source does not support seeking.
//
//-------------------------------------------------------------------

#pragma warning( push )
#pragma warning( disable : 4355 )  // 'this' used in base member initializer list


HRESULT CreateVideoMediaType(const PPBOX_StreamInfoEx& info, IMFMediaType **ppType);
HRESULT CreateAudioMediaType(const PPBOX_StreamInfoEx& info, IMFMediaType **ppType);
HRESULT GetStreamMajorType(IMFStreamDescriptor *pSD, GUID *pguidMajorType);
BOOL    SampleRequestMatch(SourceOp *pOp1, SourceOp *pOp2);


/* Public class methods */

//-------------------------------------------------------------------
// Name: CreateInstance
// Static method to create an instance of the source.
//
// ppSource:    Receives a ref-counted pointer to the source.
//-------------------------------------------------------------------

HRESULT PpboxSource::CreateInstance(PpboxSource **ppSource)
{
    if (ppSource == NULL)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;
    PpboxSource *pSource = new (std::nothrow) PpboxSource(hr);
    if (pSource == NULL)
    {
        return E_OUTOFMEMORY;
    }

    if (SUCCEEDED(hr))
    {
        *ppSource = pSource;
        (*ppSource)->AddRef();
    }

    SafeRelease(&pSource);
    return hr;
}


//-------------------------------------------------------------------
// IUnknown methods
//-------------------------------------------------------------------

HRESULT PpboxSource::QueryInterface(REFIID riid, void** ppv)
{
    static const QITAB qit[] =
    {
        QITABENT(PpboxSource, IMFMediaEventGenerator),
        QITABENT(PpboxSource, IMFMediaSource),
        { 0 }
    };
    return QISearch(this, qit, riid, ppv);
}

ULONG PpboxSource::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

ULONG PpboxSource::Release()
{
    LONG cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0)
    {
        delete this;
    }
    return cRef;
}

//-------------------------------------------------------------------
// IMFMediaEventGenerator methods
//
// All of the IMFMediaEventGenerator methods do the following:
// 1. Check for shutdown status.
// 2. Call the event queue helper object.
//-------------------------------------------------------------------

HRESULT PpboxSource::BeginGetEvent(IMFAsyncCallback* pCallback,IUnknown* punkState)
{
    HRESULT hr = S_OK;

    EnterCriticalSection(&m_critSec);

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        hr = m_pEventQueue->BeginGetEvent(pCallback, punkState);
    }

    LeaveCriticalSection(&m_critSec);
    return hr;
}

HRESULT PpboxSource::EndGetEvent(IMFAsyncResult* pResult, IMFMediaEvent** ppEvent)
{
    HRESULT hr = S_OK;

    EnterCriticalSection(&m_critSec);

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        hr = m_pEventQueue->EndGetEvent(pResult, ppEvent);
    }

    LeaveCriticalSection(&m_critSec);
    return hr;
}

HRESULT PpboxSource::GetEvent(DWORD dwFlags, IMFMediaEvent** ppEvent)
{
    // NOTE:
    // GetEvent can block indefinitely, so we don't hold the critical
    // section. Therefore we need to use a local copy of the event queue
    // pointer, to make sure the pointer remains valid.

    HRESULT hr = S_OK;

    IMFMediaEventQueue *pQueue = NULL;

    EnterCriticalSection(&m_critSec);

    // Check shutdown
    hr = CheckShutdown();

    // Cache a local pointer to the queue.
    if (SUCCEEDED(hr))
    {
        pQueue = m_pEventQueue;
        pQueue->AddRef();
    }

    LeaveCriticalSection(&m_critSec);

    // Use the local pointer to call GetEvent.
    if (SUCCEEDED(hr))
    {
        hr = pQueue->GetEvent(dwFlags, ppEvent);
    }

    SafeRelease(&pQueue);
    return hr;
}

HRESULT PpboxSource::QueueEvent(MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, const PROPVARIANT* pvValue)
{
    HRESULT hr = S_OK;

    EnterCriticalSection(&m_critSec);

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        hr = m_pEventQueue->QueueEventParamVar(met, guidExtendedType, hrStatus, pvValue);
    }

    LeaveCriticalSection(&m_critSec);

    return hr;
}

//-------------------------------------------------------------------
// IMFMediaSource methods
//-------------------------------------------------------------------


//-------------------------------------------------------------------
// CreatePresentationDescriptor
// Returns a shallow copy of the source's presentation descriptor.
//-------------------------------------------------------------------

HRESULT PpboxSource::CreatePresentationDescriptor(
    IMFPresentationDescriptor** ppPresentationDescriptor
    )
{
    if (ppPresentationDescriptor == NULL)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;

    EnterCriticalSection(&m_critSec);

    // Fail if the source is shut down.
    hr = CheckShutdown();

    // Fail if the source was not initialized yet.
    if (SUCCEEDED(hr))
    {
        hr = IsInitialized();
    }

    // Do we have a valid presentation descriptor?
    if (SUCCEEDED(hr))
    {
        if (m_pPresentationDescriptor == NULL)
        {
            hr = MF_E_NOT_INITIALIZED;
        }
    }

    // Clone our presentation descriptor.
    if (SUCCEEDED(hr))
    {
        hr = m_pPresentationDescriptor->Clone(ppPresentationDescriptor);
    }

    LeaveCriticalSection(&m_critSec);
    return hr;
}


//-------------------------------------------------------------------
// GetCharacteristics
// Returns capabilities flags.
//-------------------------------------------------------------------

HRESULT PpboxSource::GetCharacteristics(DWORD* pdwCharacteristics)
{
    if (pdwCharacteristics == NULL)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;

    EnterCriticalSection(&m_critSec);

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        *pdwCharacteristics =  MFMEDIASOURCE_CAN_PAUSE;
        if (m_uDuration > 0)
            *pdwCharacteristics |= MFMEDIASOURCE_CAN_SEEK;
    }

    // NOTE: This sample does not implement seeking, so we do not
    // include the MFMEDIASOURCE_CAN_SEEK flag.

    LeaveCriticalSection(&m_critSec);
    return hr;
}


//-------------------------------------------------------------------
// RequestSample
// 
//-------------------------------------------------------------------

HRESULT PpboxSource::RequestSample()
{
    EnterCriticalSection(&m_critSec);

    HRESULT hr = S_OK;

    // Fail if the source is shut down.
    hr = CheckShutdown();

    // Queue the operation.
    if (SUCCEEDED(hr))
    {
        hr = QueueAsyncOperation(SourceOp::OP_REQUEST_DATA);
    }

    LeaveCriticalSection(&m_critSec);
    return hr;
}


//-------------------------------------------------------------------
// Pause
// Pauses the source.
//-------------------------------------------------------------------

HRESULT PpboxSource::Pause()
{
    EnterCriticalSection(&m_critSec);

    HRESULT hr = S_OK;

    // Fail if the source is shut down.
    hr = CheckShutdown();

    // Queue the operation.
    if (SUCCEEDED(hr))
    {
        hr = QueueAsyncOperation(SourceOp::OP_PAUSE);
    }

    LeaveCriticalSection(&m_critSec);
    return hr;
}

//-------------------------------------------------------------------
// Shutdown
// Shuts down the source and releases all resources.
//-------------------------------------------------------------------

HRESULT PpboxSource::Shutdown()
{
    EnterCriticalSection(&m_critSec);

    HRESULT hr = S_OK;

    PpboxStream *pStream = NULL;

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        // Shut down the stream objects.

        for (DWORD i = 0; i < m_streams.GetCount(); i++)
        {
            (void)m_streams[i]->Shutdown();
        }

        // Shut down the event queue.
        if (m_pEventQueue)
        {
            (void)m_pEventQueue->Shutdown();
        }

        // Release objects.

        SafeRelease(&m_pEventQueue);
        SafeRelease(&m_pPresentationDescriptor);
        SafeRelease(&m_pCurrentOp);

        PPBOX_Close()();

        // Set the state.
        m_state = STATE_SHUTDOWN;
    }

    LeaveCriticalSection(&m_critSec);
    return hr;
}


//-------------------------------------------------------------------
// Start
// Starts or seeks the media source.
//-------------------------------------------------------------------

HRESULT PpboxSource::Start(
        IMFPresentationDescriptor* pPresentationDescriptor,
        const GUID* pguidTimeFormat,
        const PROPVARIANT* pvarStartPos
    )
{

    HRESULT hr = S_OK;
    SourceOp *pAsyncOp = NULL;

    // Check parameters.

    // Start position and presentation descriptor cannot be NULL.
    if (pvarStartPos == NULL || pPresentationDescriptor == NULL)
    {
        return E_INVALIDARG;
    }

    // Check the time format.
    if ((pguidTimeFormat != NULL) && (*pguidTimeFormat != GUID_NULL))
    {
        // Unrecognized time format GUID.
        return MF_E_UNSUPPORTED_TIME_FORMAT;
    }

    // Check the data type of the start position.
    if ((pvarStartPos->vt != VT_I8) && (pvarStartPos->vt != VT_EMPTY))
    {
        return MF_E_UNSUPPORTED_TIME_FORMAT;
    }

    EnterCriticalSection(&m_critSec);

    // Check if this is a seek request. This sample does not support seeking.

    if (pvarStartPos->vt == VT_I8)
    {
        // If the current state is STOPPED, then position 0 is valid.
        // Otherwise, the start position must be VT_EMPTY (current position).

        //if ((m_state != STATE_STOPPED) || (pvarStartPos->hVal.QuadPart != 0))
        //{
        //    hr = MF_E_INVALIDREQUEST;
        //    goto done;
        //}
    }

    // Fail if the source is shut down.
    hr = CheckShutdown();
    if (FAILED(hr))
    {
        goto done;
    }

    // Fail if the source was not initialized yet.
    hr = IsInitialized();
    if (FAILED(hr))
    {
        goto done;
    }

    // Perform a sanity check on the caller's presentation descriptor.
    hr = ValidatePresentationDescriptor(pPresentationDescriptor);
    if (FAILED(hr))
    {
        goto done;
    }

    // The operation looks OK. Complete the operation asynchronously.

    hr = SourceOp::CreateStartOp(pPresentationDescriptor, &pAsyncOp);
    if (FAILED(hr))
    {
        goto done;
    }

    hr = pAsyncOp->SetData(*pvarStartPos);
    if (FAILED(hr))
    {
        goto done;
    }

    hr = QueueOperation(pAsyncOp);

done:
    SafeRelease(&pAsyncOp);
    LeaveCriticalSection(&m_critSec);
    return hr;
}


//-------------------------------------------------------------------
// Stop
// Stops the media source.
//-------------------------------------------------------------------

HRESULT PpboxSource::Stop()
{
    EnterCriticalSection(&m_critSec);

    HRESULT hr = S_OK;

    // Fail if the source is shut down.
    hr = CheckShutdown();

    // Fail if the source was not initialized yet.
    if (SUCCEEDED(hr))
    {
        hr = IsInitialized();
    }

    // Queue the operation.
    if (SUCCEEDED(hr))
    {
        hr = QueueAsyncOperation(SourceOp::OP_STOP);
    }

    LeaveCriticalSection(&m_critSec);
    return hr;
}


//-------------------------------------------------------------------
// Public non-interface methods
//-------------------------------------------------------------------

/* Private methods */

PpboxSource::PpboxSource(HRESULT& hr) :
    OpQueue(m_critSec),
    m_cRef(1),
    m_pEventQueue(NULL),
    m_pPresentationDescriptor(NULL),
    m_state(STATE_INVALID),
    m_pCurrentOp(NULL),
    m_cRestartCounter(0),
    m_pSampleRequest(NULL),
    m_uDuration(0),
    m_OnScheduleDelayRequestSample(this, &PpboxSource::OnScheduleDelayRequestSample),
    m_keyScheduleDelayRequestSample(0)
{
    DllAddRef();

    InitializeCriticalSection(&m_critSec);

    // Create the media event queue.
    hr = MFCreateEventQueue(&m_pEventQueue);

    InitPresentationDescriptor();
}

PpboxSource::~PpboxSource()
{
    if (m_state != STATE_SHUTDOWN)
    {
        Shutdown();
    }

    DeleteCriticalSection(&m_critSec);

    DllRelease();
}


//-------------------------------------------------------------------
// IsInitialized:
// Returns S_OK if the source is correctly initialized with an
// Ppbox byte stream. Otherwise, returns MF_E_NOT_INITIALIZED.
//-------------------------------------------------------------------

HRESULT PpboxSource::IsInitialized() const
{
    if (m_state == STATE_INVALID)
    {
        return MF_E_NOT_INITIALIZED;
    }
    else
    {
        return S_OK;
    }
}


//-------------------------------------------------------------------
// IsStreamTypeSupported:
// Returns TRUE if the source supports the specified Ppbox stream
// type.
//-------------------------------------------------------------------

BOOL PpboxSource::IsStreamTypeSupported(StreamType type) const
{
    // We support audio and video streams.
    return (type == StreamType_Video || type == StreamType_Audio);
}

//-------------------------------------------------------------------
// IsStreamActive:
// Returns TRUE if the source should deliver a payload, whose type
// is indicated by the specified packet header.
//
// Note: This method does not test the started/paused/stopped state
//       of the source.
//-------------------------------------------------------------------

BOOL PpboxSource::IsStreamActive(unsigned long stream_id)
{
    // The source is already opened. Check if the stream is active.
    PpboxStream *pStream = m_streams.Find(stream_id);

    if (pStream == NULL)
    {
        return FALSE;
    }
    else
    {
        return pStream->IsActive();
    }
}


//-------------------------------------------------------------------
// InitPresentationDescriptor
//
// Creates the source's presentation descriptor, if possible.
//
// During the BeginOpen operation, the source reads packets looking
// for headers for each stream. This enables the source to create the
// presentation descriptor, which describes the stream formats.
//
// This method tests whether the source has seen enough packets
// to create the PD. If so, it invokes the callback to complete
// the BeginOpen operation.
//-------------------------------------------------------------------

HRESULT PpboxSource::InitPresentationDescriptor()
{
    HRESULT hr = S_OK;
    DWORD cStreams = 0;

    assert(m_pPresentationDescriptor == NULL);

    m_uDuration = PPBOX_GetDuration()();
    m_uDuration *= 10000;

    cStreams = PPBOX_GetStreamCount()();
    // Ready to create the presentation descriptor.

    // Create an array of IMFStreamDescriptor pointers.
    IMFStreamDescriptor **ppSD =
            new (std::nothrow) IMFStreamDescriptor*[cStreams];

    if (ppSD == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto done;
    }

    ZeroMemory(ppSD, cStreams * sizeof(IMFStreamDescriptor*));

    // Fill the array by getting the stream descriptors from the streams.
    for (DWORD i = 0; i < cStreams; i++)
    {
        CreateStream(i);
        hr = m_streams[i]->GetStreamDescriptor(&ppSD[i]);
        if (FAILED(hr))
        {
            goto done;
        }
    }

    // Create the presentation descriptor.
    hr = MFCreatePresentationDescriptor(cStreams, ppSD,
        &m_pPresentationDescriptor);

    if (FAILED(hr))
    {
        goto done;
    }

    if (m_uDuration > 0)
    {
        hr = m_pPresentationDescriptor->SetUINT64(MF_PD_DURATION, m_uDuration);
        if (FAILED(hr))
        {
            goto done;
        }
    }

    // Select the first video stream (if any).
    for (DWORD i = 0; i < cStreams; i++)
    {
        GUID majorType = GUID_NULL;

        hr = GetStreamMajorType(ppSD[i], &majorType);
        if (FAILED(hr))
        {
            goto done;
        }

        if (majorType == MFMediaType_Video)
        {
            hr = m_pPresentationDescriptor->SelectStream(i);
            if (FAILED(hr))
            {
                goto done;
            }
            break;
        }
    }

    // Switch state from "STATE_INVALID" to stopped.
    m_state = STATE_STOPPED;

done:
    // clean up:
    if (ppSD)
    {
        for (DWORD i = 0; i < cStreams; i++)
        {
            SafeRelease(&ppSD[i]);
        }
        delete [] ppSD;
    }
    return hr;
}


//-------------------------------------------------------------------
// QueueAsyncOperation
// Queue an asynchronous operation.
//
// OpType: Type of operation to queue.
//
// Note: If the SourceOp object requires additional information, call
// OpQueue<SourceOp>::QueueOperation, which takes a SourceOp pointer.
//-------------------------------------------------------------------

HRESULT PpboxSource::QueueAsyncOperation(SourceOp::Operation OpType)
{
    HRESULT hr = S_OK;
    SourceOp *pOp = NULL;

    hr = SourceOp::CreateOp(OpType, &pOp);

    if (SUCCEEDED(hr))
    {
        hr = QueueOperation(pOp);
    }

    SafeRelease(&pOp);
    return hr;
}

//-------------------------------------------------------------------
// BeginAsyncOp
//
// Starts an asynchronous operation. Called by the source at the
// begining of any asynchronous operation.
//-------------------------------------------------------------------

HRESULT PpboxSource::BeginAsyncOp(SourceOp *pOp)
{
    // At this point, the current operation should be NULL (the
    // previous operation is NULL) and the new operation (pOp)
    // must not be NULL.

    if (pOp == NULL || m_pCurrentOp != NULL)
    {
        assert(FALSE);
        return E_FAIL;
    }

    // Store the new operation as the current operation.

    m_pCurrentOp = pOp;
    m_pCurrentOp->AddRef();

    return S_OK;
}

//-------------------------------------------------------------------
// CompleteAsyncOp
//
// Completes an asynchronous operation. Called by the source at the
// end of any asynchronous operation.
//-------------------------------------------------------------------

HRESULT PpboxSource::CompleteAsyncOp(SourceOp *pOp)
{
    HRESULT hr = S_OK;

    // At this point, the current operation (m_pCurrentOp)
    // must match the operation that is ending (pOp).

    if (pOp == NULL || m_pCurrentOp == NULL)
    {
        assert(FALSE);
        return E_FAIL;
    }

    if (m_pCurrentOp != pOp)
    {
        assert(FALSE);
        return E_FAIL;
    }

    // Release the current operation.
    SafeRelease(&m_pCurrentOp);

    // Process the next operation on the queue.
    hr = ProcessQueue();

    return hr;
}

//-------------------------------------------------------------------
// DispatchOperation
//
// Performs the asynchronous operation indicated by pOp.
//
// NOTE:
// This method implements the pure-virtual OpQueue::DispatchOperation
// method. It is always called from a work-queue thread.
//-------------------------------------------------------------------

HRESULT PpboxSource::DispatchOperation(SourceOp *pOp)
{
    EnterCriticalSection(&m_critSec);

    HRESULT hr = S_OK;

    if (m_state == STATE_SHUTDOWN)
    {
        LeaveCriticalSection(&m_critSec);

        return S_OK; // Already shut down, ignore the request.
    }

    switch (pOp->Op())
    {

    // IMFMediaSource methods:

    case SourceOp::OP_START:
        hr = DoStart((StartOp*)pOp);
        break;

    case SourceOp::OP_STOP:
        hr = DoStop(pOp);
        break;

    case SourceOp::OP_PAUSE:
        hr = DoPause(pOp);
        break;

    // Operations requested by the streams:

    case SourceOp::OP_REQUEST_DATA:
        hr = OnStreamRequestSample(pOp);
        break;

    case SourceOp::OP_END_OF_STREAM:
        hr = OnEndOfStream(pOp);
        break;

    default:
        hr = E_UNEXPECTED;
    }

    if (FAILED(hr))
    {
        StreamingError(hr);
    }

    LeaveCriticalSection(&m_critSec);
    return hr;
}


//-------------------------------------------------------------------
// ValidateOperation
//
// Checks whether the source can perform the operation indicated
// by pOp at this time.
//
// If the source cannot perform the operation now, the method
// returns MF_E_NOTACCEPTING.
//
// NOTE:
// Implements the pure-virtual OpQueue::ValidateOperation method.
//-------------------------------------------------------------------

HRESULT PpboxSource::ValidateOperation(SourceOp *pOp)
{
    if (m_pCurrentOp != NULL)
    {
        return MF_E_NOTACCEPTING;
    }
    return S_OK;
}



//-------------------------------------------------------------------
// DoStart
// Perform an async start operation (IMFMediaSource::Start)
//
// pOp: Contains the start parameters.
//
// Note: This sample currently does not implement seeking, and the
// Start() method fails if the caller requests a seek.
//-------------------------------------------------------------------

HRESULT PpboxSource::DoStart(StartOp *pOp)
{
    assert(pOp->Op() == SourceOp::OP_START);

    IMFPresentationDescriptor *pPD = NULL;
    IMFMediaEvent  *pEvent = NULL;

    HRESULT     hr = S_OK;
    LONGLONG    llStartOffset = 0;
    BOOL        bRestartFromCurrentPosition = FALSE;
    BOOL        bSentEvents = FALSE;

    hr = BeginAsyncOp(pOp);

    // Get the presentation descriptor from the SourceOp object.
    // This is the PD that the caller passed into the Start() method.
    // The PD has already been validated.
    if (SUCCEEDED(hr))
    {
        hr = pOp->GetPresentationDescriptor(&pPD);
    }
    // Because this sample does not support seeking, the start
    // position must be 0 (from stopped) or "current position."

    // If the sample supported seeking, we would need to get the
    // start position from the PROPVARIANT data contained in pOp.

    if (SUCCEEDED(hr))
    {
        // Select/deselect streams, based on what the caller set in the PD.
        // This method also sends the MENewStream/MEUpdatedStream events.
        hr = SelectStreams(pPD, &pOp->Data());
    }

    if (SUCCEEDED(hr))
    {
        m_state = STATE_STARTED;

        // Queue the "started" event. The event data is the start position.
        hr = m_pEventQueue->QueueEventParamVar(
            MESourceStarted,
            GUID_NULL,
            S_OK,
            &pOp->Data()
            );
    }

    if (FAILED(hr))
    {
        // Failure. Send the error code to the application.

        // Note: It's possible that QueueEvent itself failed, in which case it
        // is likely to fail again. But there is no good way to recover in
        // that case.

        (void)m_pEventQueue->QueueEventParamVar(
            MESourceStarted, GUID_NULL, hr, NULL);
    }

    CompleteAsyncOp(pOp);

    SafeRelease(&pEvent);
    SafeRelease(&pPD);
    return hr;
}


//-------------------------------------------------------------------
// DoStop
// Perform an async stop operation (IMFMediaSource::Stop)
//-------------------------------------------------------------------

HRESULT PpboxSource::DoStop(SourceOp *pOp)
{
    HRESULT hr = S_OK;
    QWORD qwCurrentPosition = 0;

    hr = BeginAsyncOp(pOp);

    // Stop the active streams.
    if (SUCCEEDED(hr))
    {
        for (DWORD i = 0; i < m_streams.GetCount(); i++)
        {
            if (m_streams[i]->IsActive())
            {
                hr = m_streams[i]->Stop();
            }
            if (FAILED(hr))
            {
                break;
            }
        }
    }

    // Increment the counter that tracks "stale" read requests.
    if (SUCCEEDED(hr))
    {
        ++m_cRestartCounter; // This counter is allowed to overflow.
    }

    SafeRelease(&m_pSampleRequest);

    m_state = STATE_STOPPED;

    // Send the "stopped" event. This might include a failure code.
    (void)m_pEventQueue->QueueEventParamVar(MESourceStopped, GUID_NULL, hr, NULL);

    CompleteAsyncOp(pOp);

    return hr;
}


//-------------------------------------------------------------------
// DoPause
// Perform an async pause operation (IMFMediaSource::Pause)
//-------------------------------------------------------------------

HRESULT PpboxSource::DoPause(SourceOp *pOp)
{
    HRESULT hr = S_OK;

    hr = BeginAsyncOp(pOp);

    // Pause is only allowed while running.
    if (SUCCEEDED(hr))
    {
        if (m_state != STATE_STARTED)
        {
            hr = MF_E_INVALID_STATE_TRANSITION;
        }
    }

    // Pause the active streams.
    if (SUCCEEDED(hr))
    {
        for (DWORD i = 0; i < m_streams.GetCount(); i++)
        {
            if (m_streams[i]->IsActive())
            {
                hr = m_streams[i]->Pause();
            }
            if (FAILED(hr))
            {
                break;
            }
        }
    }

    m_state = STATE_PAUSED;


    // Send the "paused" event. This might include a failure code.
    (void)m_pEventQueue->QueueEventParamVar(MESourcePaused, GUID_NULL, hr, NULL);

    CompleteAsyncOp(pOp);

    return hr;
}


//-------------------------------------------------------------------
// StreamRequestSample
// Called by streams when they need more data.
//
// Note: This is an async operation. The stream requests more data
// by queueing an OP_REQUEST_DATA operation.
//-------------------------------------------------------------------

HRESULT PpboxSource::OnStreamRequestSample(SourceOp *pOp)
{
    HRESULT hr = S_OK;

    hr = BeginAsyncOp(pOp);

    // Ignore this request if we are already handling an earlier request.
    // (In that case m_pSampleRequest will be non-NULL.)

    if (SUCCEEDED(hr))
    {
        if (m_pSampleRequest == NULL)
        {
            // Add the request counter as data to the operation.
            // This counter tracks whether a read request becomes "stale."

            PROPVARIANT var;
            var.vt = VT_UI4;
            var.ulVal = m_cRestartCounter;

            hr = pOp->SetData(var);

            if (SUCCEEDED(hr))
            {
                // Store this while the request is pending.
                //m_pSampleRequest = pOp;
                //m_pSampleRequest->AddRef();

                // Try to parse data - this will invoke a read request if needed.
                DeliverPayload();
            }
        }

        CompleteAsyncOp(pOp);
    }

    return hr;
}


//-------------------------------------------------------------------
// OnEndOfStream
// Called by each stream when it sends the last sample in the stream.
//
// Note: When the media source reaches the end of the Ppbox stream,
// it calls EndOfStream on each stream object. The streams might have
// data still in their queues. As each stream empties its queue, it
// notifies the source through an async OP_END_OF_STREAM operation.
//
// When every stream notifies the source, the source can send the
// "end-of-presentation" event.
//-------------------------------------------------------------------

HRESULT PpboxSource::OnEndOfStream(SourceOp *pOp)
{
    HRESULT hr = S_OK;

    hr = BeginAsyncOp(pOp);

    // Decrement the count of end-of-stream notifications.
    if (SUCCEEDED(hr))
    {
        --m_cPendingEOS;
        if (m_cPendingEOS == 0)
        {
            // No more streams. Send the end-of-presentation event.
            hr = m_pEventQueue->QueueEventParamVar(MEEndOfPresentation, GUID_NULL, S_OK, NULL);
        }

    }

    if (SUCCEEDED(hr))
    {
        hr = CompleteAsyncOp(pOp);
    }

    return hr;
}



//-------------------------------------------------------------------
// SelectStreams
// Called during START operations to select and deselect streams.
//-------------------------------------------------------------------

HRESULT PpboxSource::SelectStreams(
    IMFPresentationDescriptor *pPD,   // Presentation descriptor.
    PROPVARIANT * varStart        // New start position.
    )
{
    HRESULT hr = S_OK;
    BOOL    fSelected = FALSE;
    BOOL    fWasSelected = FALSE;
    DWORD   stream_id = 0;

    IMFStreamDescriptor *pSD = NULL;

    PpboxStream *pStream = NULL; // Not add-ref'd

    // Reset the pending EOS count.
    m_cPendingEOS = 0;

    if (varStart->vt == VT_I8)
    {
        hr = PPBOX_Seek()(varStart->hVal.QuadPart / 10000);
        if (hr == ppbox_success || hr == ppbox_would_block) {
            hr = S_OK;
        } else {
            hr = E_FAIL;
        }
    }
    else
    {
        varStart->vt = VT_I8;
        varStart->hVal.QuadPart == 0;
    }

    if (FAILED(hr))
    {
        goto done;
    }

    // Loop throught the stream descriptors to find which streams are active.
    for (DWORD i = 0; i < m_streams.GetCount(); i++)
    {
        hr = pPD->GetStreamDescriptorByIndex(i, &fSelected, &pSD);
        if (FAILED(hr))
        {
            goto done;
        }

        hr = pSD->GetStreamIdentifier(&stream_id);
        if (FAILED(hr))
        {
            goto done;
        }

        pStream = m_streams.Find((BYTE)stream_id);
        if (pStream == NULL)
        {
            hr = E_INVALIDARG;
            goto done;
        }

        // Was the stream active already?
        fWasSelected = pStream->IsActive();

        // Activate or deactivate the stream.
        hr = pStream->Activate(fSelected);
        if (FAILED(hr))
        {
            goto done;
        }

        if (fSelected)
        {
            m_cPendingEOS++;

            // If the stream was previously selected, send an "updated stream"
            // event. Otherwise, send a "new stream" event.
            MediaEventType met = fWasSelected ? MEUpdatedStream : MENewStream;

            hr = m_pEventQueue->QueueEventParamUnk(met, GUID_NULL, hr, pStream);
            if (FAILED(hr))
            {
                goto done;
            }

            // Start the stream. The stream will send the appropriate event.
            hr = pStream->Start(*varStart);
            if (FAILED(hr))
            {
                goto done;
            }
        }
        SafeRelease(&pSD);
    }

done:
    SafeRelease(&pSD);
    return hr;
}


//-------------------------------------------------------------------
// EndOfMPEGStream:
// Called when the parser reaches the end of the Ppbox stream.
//-------------------------------------------------------------------

HRESULT PpboxSource::EndOfStream()
{
    // Notify the streams. The streams might have pending samples.
    // When each stream delivers the last sample, it will send the
    // end-of-stream event to the pipeline and then notify the
    // source.

    // When every stream is done, the source sends the end-of-
    // presentation event.

    HRESULT hr = S_OK;

    for (DWORD i = 0; i < m_streams.GetCount(); i++)
    {
        if (m_streams[i]->IsActive())
        {
            hr = m_streams[i]->EndOfStream();
        }
        if (FAILED(hr))
        {
            break;
        }
    }

    return hr;
}



//-------------------------------------------------------------------
// StreamsNeedData:
// Returns TRUE if any streams need more data.
//-------------------------------------------------------------------

BOOL PpboxSource::StreamsNeedData() const
{
    BOOL bNeedData = FALSE;

    switch (m_state)
    {
    case STATE_SHUTDOWN:
        // While shut down, we never need data.
        return FALSE;

    default:
        // If none of the above, ask the streams.
        for (DWORD i = 0; i < m_streams.GetCount(); i++)
        {
            if (m_streams[i]->NeedsData())
            {
                bNeedData = TRUE;
                break;
            }
        }
        return bNeedData;
    }
}

BYTE buf[256];
MPEG2VIDEOINFO * pMP2VI = (MPEG2VIDEOINFO *)buf;
DWORD video_stream_index = 0;

//-------------------------------------------------------------------
// DeliverPayload:
// Delivers an Ppbox payload.
//-------------------------------------------------------------------

HRESULT PpboxSource::DeliverPayload()
{
    // When this method is called, the read buffer contains a complete
    // payload, and the payload belongs to a stream whose type we support.

    HRESULT             hr = S_OK;
    PPBOX_SampleEx2     sample;
    PpboxStream         *pStream = NULL;    // not AddRef'd

    IMFMediaBuffer      *pBuffer = NULL;
    IMFSample           *pSample = NULL;
    BYTE                *pData = NULL;      // Pointer to the IMFMediaBuffer data.

    hr = PPBOX_ReadSampleEx2()(&sample);

    if (hr == ppbox_success)
    {
        hr = S_OK;
    }
    else if (hr == ppbox_would_block)
    {
        hr = MFScheduleWorkItem(&m_OnScheduleDelayRequestSample, NULL, -100, NULL);
        return hr;
    }
    else if (hr == ppbox_stream_end)
    {
        hr = EndOfStream();
        return hr;
    }
    else
    {
        hr = E_FAIL;
        return hr;
    }

    if (SUCCEEDED(hr))
    {
        //OutputDebugString(TEXT("DeliverPayload "));
        //OutputDebugString(sample.stream_index == 0 ? TEXT("video\n") : TEXT("audio\n"));
        pStream = m_streams.Find(sample.stream_index);
        assert(pStream != NULL);
    }

    // Create a media buffer for the payload.
    if (SUCCEEDED(hr))
    {
        if (sample.stream_index == video_stream_index && sample.is_sync)
        {
            hr = MFCreateMemoryBuffer(sample.buffer_length + pMP2VI->cbSequenceHeader, &pBuffer);
        }
        else
        {
            hr = MFCreateMemoryBuffer(sample.buffer_length, &pBuffer);
        }
    }

    if (SUCCEEDED(hr))
    {
        hr = pBuffer->Lock(&pData, NULL, NULL);
    }

    if (SUCCEEDED(hr))
    {
        if (sample.stream_index == video_stream_index)
        {
            BYTE * p = pData;
            if (sample.is_sync)
            {
                CopyMemory(p, pMP2VI->dwSequenceHeader, pMP2VI->cbSequenceHeader);
                p += pMP2VI->cbSequenceHeader;
            }
            CopyMemory(p, sample.buffer, sample.buffer_length);
            BYTE * e = p + sample.buffer_length;
            while (p < e)
            {
                DWORD l = p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3];
                p[0] = p[1] = p[2] = 0;
                p[3] = 1;
                p += 4;
                p += l;
            }
        }
        else
        {
            CopyMemory(pData, sample.buffer, sample.buffer_length);
        }
    }

    if (SUCCEEDED(hr))
    {
        hr = pBuffer->Unlock();
    }

    if (SUCCEEDED(hr))
    {
        hr = pBuffer->SetCurrentLength(sample.buffer_length);
    }

    // Create a sample to hold the buffer.
    if (SUCCEEDED(hr))
    {
        hr = MFCreateSample(&pSample);
    }
    if (SUCCEEDED(hr))
    {
        hr = pSample->AddBuffer(pBuffer);
    }

    // Time stamp the sample.
    if (SUCCEEDED(hr))
    {
        LONGLONG hnsStart = (sample.start_time + sample.composite_time_delta) * 10;

        hr = pSample->SetSampleTime(hnsStart);
    }

    // Deliver the payload to the stream.
    if (SUCCEEDED(hr))
    {
        hr = pStream->DeliverPayload(pSample);
    }

    if (SUCCEEDED(hr))
    {
        if (StreamsNeedData())
        {
            hr = RequestSample();
        }
    }

    if (FAILED(hr))
    {
        hr = EndOfStream();
    }

    SafeRelease(&pBuffer);
    SafeRelease(&pSample);
    return hr;
}



//-------------------------------------------------------------------
// CreateStream:
// Creates a media stream, based on a packet header.
//-------------------------------------------------------------------

HRESULT PpboxSource::CreateStream(long stream_id)
{
    // First see if the stream already exists.
    if ( m_streams.Find(stream_id) != NULL )
    {
        // The stream already exists. Nothing to do.
        return S_OK;
    }

    HRESULT hr = S_OK;

    PPBOX_StreamInfoEx info;

    IMFMediaType *pType = NULL;
    IMFStreamDescriptor *pSD = NULL;
    PpboxStream *pStream = NULL;
    IMFMediaTypeHandler *pHandler = NULL;

    PPBOX_GetStreamInfoEx()(stream_id, &info);

    switch (info.type)
    {
    case ppbox_video:
        video_stream_index = stream_id;
        hr = CreateVideoMediaType(info, &pType);
        break;

    case ppbox_audio:
        hr = CreateAudioMediaType(info, &pType);
        break;

    default:
        assert(false); // If this case occurs, then IsStreamTypeSupported() is wrong.
        hr = E_UNEXPECTED;
    }

    if (SUCCEEDED(hr))
    {
        // Create the stream descriptor from the media type.
        hr = MFCreateStreamDescriptor(stream_id, 1, &pType, &pSD);
    }

    // Set the default media type on the stream handler.
    if (SUCCEEDED(hr))
    {
        hr = pSD->GetMediaTypeHandler(&pHandler);
    }
    if (SUCCEEDED(hr))
    {
        hr = pHandler->SetCurrentMediaType(pType);
    }

    // Create the new stream.
    if (SUCCEEDED(hr))
    {
        pStream = new (std::nothrow) PpboxStream(this, pSD, hr);
        if (pStream == NULL)
        {
            hr = E_OUTOFMEMORY;
        }
    }

    // Add the stream to the array.
    if (SUCCEEDED(hr))
    {
        hr = m_streams.AddStream(stream_id, pStream);
    }

    SafeRelease(&pSD);
    SafeRelease(&pStream);
    return hr;
}


//-------------------------------------------------------------------
// ValidatePresentationDescriptor:
// Validates the presentation descriptor that the caller specifies
// in IMFMediaSource::Start().
//
// Note: This method performs a basic sanity check on the PD. It is
// not intended to be a thorough validation.
//-------------------------------------------------------------------

HRESULT PpboxSource::ValidatePresentationDescriptor(IMFPresentationDescriptor *pPD)
{
    HRESULT hr = S_OK;
    BOOL fSelected = FALSE;
    DWORD cStreams = 0;

    IMFStreamDescriptor *pSD = NULL;

    // The caller's PD must have the same number of streams as ours.
    hr = pPD->GetStreamDescriptorCount(&cStreams);

    if (SUCCEEDED(hr))
    {
        if (cStreams != m_streams.GetCount())
        {
            hr = E_INVALIDARG;
        }
    }

    // The caller must select at least one stream.
    if (SUCCEEDED(hr))
    {
        for (DWORD i = 0; i < cStreams; i++)
        {
            hr = pPD->GetStreamDescriptorByIndex(i, &fSelected, &pSD);
            if (FAILED(hr))
            {
                break;
            }
            if (fSelected)
            {
                break;
            }
            SafeRelease(&pSD);
        }
    }

    if (SUCCEEDED(hr))
    {
        if (!fSelected)
        {
            hr = E_INVALIDARG;
        }
    }

    SafeRelease(&pSD);
    return hr;
}


//-------------------------------------------------------------------
// StreamingError:
// Handles an error that occurs duing an asynchronous operation.
//
// hr: Error code of the operation that failed.
//-------------------------------------------------------------------

void PpboxSource::StreamingError(HRESULT hr)
{
    if (m_state != STATE_SHUTDOWN)
    {
        // An error occurred during streaming. Send the MEError event
        // to notify the pipeline.

        QueueEvent(MEError, GUID_NULL, hr, NULL);
    }
}

//-------------------------------------------------------------------
// OnScheduleDelayRequestSample
// Called when an asynchronous read completes.
//
// 
//-------------------------------------------------------------------

HRESULT PpboxSource::OnScheduleDelayRequestSample(IMFAsyncResult *pResult)
{
    EnterCriticalSection(&m_critSec);

    HRESULT hr = S_OK;
    DWORD cbRead = 0;

    IUnknown *pState = NULL;

    if (m_state == STATE_SHUTDOWN)
    {
        // If we are shut down, then we've already released the
        // byte stream. Nothing to do.
        LeaveCriticalSection(&m_critSec);
        return S_OK;
    }

    // Get the state object. This is either NULL or the most
    // recent OP_REQUEST_DATA operation.
    (void)pResult->GetState(&pState);

    // Complete the read opertation.
    hr = pResult->GetStatus();

    if (FAILED(hr))
    {
        StreamingError(hr);
    }

    hr = QueueAsyncOperation(SourceOp::OP_REQUEST_DATA);

    SafeRelease(&pState);
    LeaveCriticalSection(&m_critSec);
    return hr;
}



/* SourceOp class */


//-------------------------------------------------------------------
// CreateOp
// Static method to create a SourceOp instance.
//
// op: Specifies the async operation.
// ppOp: Receives a pointer to the SourceOp object.
//-------------------------------------------------------------------

HRESULT SourceOp::CreateOp(SourceOp::Operation op, SourceOp **ppOp)
{
    if (ppOp == NULL)
    {
        return E_POINTER;
    }

    SourceOp *pOp = new (std::nothrow) SourceOp(op);
    if (pOp  == NULL)
    {
        return E_OUTOFMEMORY;
    }
    *ppOp = pOp;

    return S_OK;
}

//-------------------------------------------------------------------
// CreateStartOp:
// Static method to create a SourceOp instance for the Start()
// operation.
//
// pPD: Presentation descriptor from the caller.
// ppOp: Receives a pointer to the SourceOp object.
//-------------------------------------------------------------------

HRESULT SourceOp::CreateStartOp(IMFPresentationDescriptor *pPD, SourceOp **ppOp)
{
    if (ppOp == NULL)
    {
        return E_POINTER;
    }

    SourceOp *pOp = new (std::nothrow) StartOp(pPD);
    if (pOp == NULL)
    {
        return E_OUTOFMEMORY;
    }

    *ppOp = pOp;
    return S_OK;
}


ULONG SourceOp::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

ULONG SourceOp::Release()
{
    LONG cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0)
    {
        delete this;
    }
    return cRef;
}

HRESULT SourceOp::QueryInterface(REFIID riid, void** ppv)
{
    static const QITAB qit[] =
    {
        QITABENT(SourceOp, IUnknown),
        { 0 }
    };
    return QISearch(this, qit, riid, ppv);
}

SourceOp::SourceOp(Operation op) : m_cRef(1), m_op(op)
{
    PropVariantInit(&m_data);
}

SourceOp::~SourceOp()
{
    PropVariantClear(&m_data);
}

HRESULT SourceOp::SetData(const PROPVARIANT& var)
{
    return PropVariantCopy(&m_data, &var);
}


StartOp::StartOp(IMFPresentationDescriptor *pPD) : SourceOp(SourceOp::OP_START), m_pPD(pPD)
{
    if (m_pPD)
    {
        m_pPD->AddRef();
    }
}

StartOp::~StartOp()
{
    SafeRelease(&m_pPD);
}


HRESULT StartOp::GetPresentationDescriptor(IMFPresentationDescriptor **ppPD)
{
    if (ppPD == NULL)
    {
        return E_POINTER;
    }
    if (m_pPD == NULL)
    {
        return MF_E_INVALIDREQUEST;
    }
    *ppPD = m_pPD;
    (*ppPD)->AddRef();
    return S_OK;
}


/*  Static functions */

struct AvcConfig
{
    BYTE Version;
    BYTE Profile;
    BYTE Profile_Compatibility;
    BYTE Level;
    BYTE Nalu_Length;
};

DWORD GetSequenceData(
                       const PPBOX_StreamInfoEx& info, 
                       BYTE * data, 
                       AvcConfig * config = NULL)
{
    BYTE * s = data;

    BYTE const * p = info.format_buffer;
    //BYTE const * e = p + info.format_size;
    BYTE Version = *p++;
    BYTE Profile = *p++;
    BYTE Profile_Compatibility = *p++;
    BYTE Level = *p++;
    BYTE Nalu_Length = 1 + ((*p++) & 3);
    size_t n = (*p++) & 31;
    for (size_t i = 0; i < n; ++i) {
        size_t l = (*p++);
        l = (l << 8) + (*p++);
        memcpy(s, "\000\000\000\001", 4);
        s += 4;
        memcpy(s, p, l);
        s += l;
        p += l;
    }
    n = (*p++) & 31;
    for (size_t i = 0; i < n; ++i) {
        size_t l = (*p++);
        l = (l << 8) + (*p++);
        memcpy(s, "\000\000\000\001", 4);
        s += 4;
        memcpy(s, p, l);
        s += l;
        p += l;
    }

    if (config)
    {
        config->Version = Version;
        config->Profile = Profile;
        config->Profile_Compatibility = Profile_Compatibility;
        config->Level = Level;
        config->Nalu_Length = Nalu_Length;
    };

    return s - data;
}

//-------------------------------------------------------------------
// CreateVideoMediaType:
// Create a media type from an Ppbox video sequence header.
//-------------------------------------------------------------------

/*
HRESULT CreateVideoMediaType(const PPBOX_StreamInfoEx& info, IMFMediaType **ppType)
{
    HRESULT hr = S_OK;

    IMFMediaType *pType = NULL;

    hr = MFCreateMediaType(&pType);

    if (SUCCEEDED(hr))
    {
        hr = pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    }

    // Subtype = Ppbox payload
    if (SUCCEEDED(hr))
    {
        hr = pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264);
    }

    // Format details.
    if (SUCCEEDED(hr))
    {
        // Frame size

        hr = MFSetAttributeSize(
            pType,
            MF_MT_FRAME_SIZE,
            info.video_format.width,
            info.video_format.height
            );
    }
    if (SUCCEEDED(hr))
    {
        // Frame rate

        hr = MFSetAttributeRatio(
            pType,
            MF_MT_FRAME_RATE,
            info.video_format.frame_rate,
            1
            );
    }
    if (SUCCEEDED(hr))
    {
        // foramt data
        BYTE sequence[256];
        DWORD nSize = GetSequenceData(info, sequence);
        //hr = pType->SetBlob(
        //    MF_MT_USER_DATA,
        //    sequence,
        //    nSize
        //    );
    }
    if (SUCCEEDED(hr))
    {
        *ppType = pType;
        (*ppType)->AddRef();
    }

    SafeRelease(&pType);
    return hr;
}
*/

DWORD GetMPEG2VideInfo(
                       const PPBOX_StreamInfoEx& info, 
                       MPEG2VIDEOINFO * pmvi)
{
    VIDEOINFOHEADER2 * pvi = &pmvi->hdr;
    SetRectEmpty(&(pvi->rcSource));
    SetRectEmpty(&(pvi->rcTarget));
    pvi->AvgTimePerFrame = 10 * 1000 * 1000 / info.video_format.frame_rate;

    BITMAPINFOHEADER * bmi = &pvi->bmiHeader;
    bmi->biSize = sizeof(BITMAPINFOHEADER);
    bmi->biWidth = info.video_format.width;
    bmi->biHeight = info.video_format.height;
    bmi->biBitCount = 0;
    bmi->biPlanes = 1;
    //bmi->biCompression = 0x31435641; // AVC1
    bmi->biCompression = 0x34363248; // H264

    BYTE * s = (BYTE *)pmvi->dwSequenceHeader;
    AvcConfig config;
    s += GetSequenceData(info, s, &config);

    pmvi->dwStartTimeCode = 0;
    pmvi->cbSequenceHeader = s - (BYTE *)pmvi->dwSequenceHeader;
    pmvi->dwProfile = config.Profile;
    pmvi->dwLevel = config.Level;
    pmvi->dwFlags = config.Nalu_Length;

    return s - (BYTE *)pmvi;
}

HRESULT CreateVideoMediaType(const PPBOX_StreamInfoEx& info, IMFMediaType **ppType)
{
    HRESULT hr = S_OK;

    IMFMediaType *pType = NULL;

    hr = MFCreateMediaType(&pType);

    if (SUCCEEDED(hr))
    {
        //BYTE buf[256];
        //MPEG2VIDEOINFO * pMP2VI = (MPEG2VIDEOINFO *)buf;
        ZeroMemory(buf, sizeof(buf));
        DWORD dwSize = GetMPEG2VideInfo(info, pMP2VI);
        hr = MFInitMediaTypeFromMPEG2VideoInfo(pType, pMP2VI, dwSize);
    }

    //if (SUCCEEDED(hr))
    //{
    //    hr = pType->SetGUID(MF_MT_AM_FORMAT_TYPE, FORMAT_MPEG2Video);
    //}

    //if (SUCCEEDED(hr))
    //{
    //    hr = pType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_MixedInterlaceOrProgressive);
    //}

    //if (SUCCEEDED(hr))
    //{
    //    hr = MFSetAtrributeRatio(pType, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
    //}

    //if (SUCCEEDED(hr))
    //{
    //    BYTE desc[] = {00, 0x00, 0x00, 0xbc, 0x73, 0x74, 0x73, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xac, 0x61, 0x76, 0x63, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x80, 0x01, 0x90, 0x00, 0x48, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0xff, 0xff, 0x00, 0x00, 0x00, 0x10, 0x70, 0x61, 0x73, 0x70, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x32, 0x61, 0x76, 0x63, 0x43, 0x01, 0x64, 0x00, 0x14, 0xff, 0xe1, 0x00, 0x1a, 0x67, 0x64, 0x00, 0x14, 0xac, 0x34, 0xec, 0x0a, 0x03, 0x3b, 0x01, 0x10, 0x00, 0x00, 0x03, 0x00, 0x10, 0x00, 0x00, 0x03, 0x03, 0x08, 0xf1, 0x42, 0x93, 0x80, 0x01, 0x00, 0x05, 0x68, 0xee, 0x32, 0xc8, 0xb0, 0x00, 0x00, 0x00, 0x14, 0x62, 0x74, 0x72, 0x74, 0x00, 0x00, 0x96, 0x6c, 0x00, 0x19, 0x11, 0x60, 0x00, 0x09, 0x89, 0x60};
    //    hr = pType->SetBlob(MF_MT_MPEG4_SAMPLE_DESCRIPTION, desc, sizeof(desc));
    //}

    //if (SUCCEEDED(hr))
    //{
    //    hr = pType->SetUINT32(MF_MT_MPEG4_CURRENT_SAMPLE_ENTRY, 0);
    //}

    if (SUCCEEDED(hr))
    {
        *ppType = pType;
        (*ppType)->AddRef();
    }

    SafeRelease(&pType);
    return hr;
}

//-------------------------------------------------------------------
// CreateAudioMediaType:
// Create a media type from an Ppbox audio frame header.
//
// Note: This function fills in an PpboxWAVEFORMAT structure and then
// converts the structure to a Media Foundation media type
// (IMFMediaType). This is somewhat roundabout but it guarantees
// that the type can be converted back to an PpboxWAVEFORMAT by the
// decoder if need be.
//
// The WAVEFORMATEX portion of the PpboxWAVEFORMAT structure is
// converted into attributes on the IMFMediaType object. The rest of
// the struct is stored in the MF_MT_USER_DATA attribute.
//-------------------------------------------------------------------
/*
HRESULT CreateAudioMediaType(const PPBOX_StreamInfoEx& info, IMFMediaType **ppType)
{
    HRESULT hr = S_OK;
    IMFMediaType *pType = NULL;
    DWORD dwSize = sizeof(WAVEFORMATEX) + info.format_size;

    WAVEFORMATEX  * wf = (WAVEFORMATEX  *)new BYTE(dwSize);
    if (wf == 0) 
        return(E_OUTOFMEMORY);
    ZeroMemory(wf, dwSize);

    wf->nChannels = info.audio_format.channel_count;
    wf->nSamplesPerSec = info.audio_format.sample_rate;
    wf->wBitsPerSample = info.audio_format.sample_size;
    wf->wFormatTag = 255;
    wf->cbSize = info.format_size;
    memcpy(wf + 1, info.format_buffer, info.format_size);

    // Use the structure to initialize the Media Foundation media type.
    hr = MFCreateMediaType(&pType);
    if (SUCCEEDED(hr))
    {
        hr = MFInitMediaTypeFromWaveFormatEx(pType, wf, dwSize);
    }

    if (SUCCEEDED(hr))
    {
        *ppType = pType;
        (*ppType)->AddRef();
    }

    SafeRelease(&pType);
    return hr;
}
*/
HRESULT CreateAudioMediaType(const PPBOX_StreamInfoEx& info, IMFMediaType **ppType)
{
    HRESULT hr = S_OK;
    IMFMediaType *pType = NULL;

    hr = MFCreateMediaType(&pType);

    if (SUCCEEDED(hr))
    {
        hr = pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    }

    // Subtype = Ppbox payload
    if (SUCCEEDED(hr))
    {
        hr = pType->SetGUID(MF_MT_SUBTYPE, MEDIASUBTYPE_RAW_AAC1);
    }

    // Format details.
    if (SUCCEEDED(hr))
    {
        // Sample size

        hr = pType->SetUINT32(
            MF_MT_AUDIO_BITS_PER_SAMPLE,
            info.audio_format.sample_size
            );
    }
    if (SUCCEEDED(hr))
    {
        // Channel count

        hr = pType->SetUINT32(
            MF_MT_AUDIO_NUM_CHANNELS,
            info.audio_format.channel_count
            );
    }
    if (SUCCEEDED(hr))
    {
        // Channel count

        hr = pType->SetUINT32(
            MF_MT_AUDIO_SAMPLES_PER_SECOND,
            info.audio_format.sample_rate
            );
    }
    if (SUCCEEDED(hr))
    {
        // foramt data

        hr = pType->SetBlob(
            MF_MT_USER_DATA,
            info.format_buffer,
            info.format_size
            );
    }
    if (SUCCEEDED(hr))
    {
        *ppType = pType;
        (*ppType)->AddRef();
    }

    SafeRelease(&pType);
    return hr;
}

// Get the major media type from a stream descriptor.
HRESULT GetStreamMajorType(IMFStreamDescriptor *pSD, GUID *pguidMajorType)
{
    if (pSD == NULL) { return E_POINTER; }
    if (pguidMajorType == NULL) { return E_POINTER; }

    HRESULT hr = S_OK;
    IMFMediaTypeHandler *pHandler = NULL;

    hr = pSD->GetMediaTypeHandler(&pHandler);
    if (SUCCEEDED(hr))
    {
        hr = pHandler->GetMajorType(pguidMajorType);
    }
    SafeRelease(&pHandler);
    return hr;
}


BOOL SampleRequestMatch(SourceOp *pOp1, SourceOp *pOp2)
{
    if ((pOp1 == NULL) && (pOp2 == NULL))
    {
        return TRUE;
    }
    else if ((pOp1 == NULL) || (pOp2 == NULL))
    {
        return FALSE;
    }
    else
    {
        return (pOp1->Data().ulVal == pOp2->Data().ulVal);
    }
}


#pragma warning( pop )