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


#pragma once

#include <new.h>
#include <windows.h>
#include <assert.h>

#ifndef _ASSERTE
#define _ASSERTE assert
#endif

#include <mftransform.h>

#include <mfapi.h>
#include <mfobjects.h>
#include <mfidl.h>
#include <mferror.h>
#include <uuids.h>      // Ppbox subtypes

#include <amvideo.h>    // VIDEOINFOHEADER definition
#include <dvdmedia.h>   // VIDEOINFOHEADER2
#include <mmreg.h>      // PpboxWAVEFORMAT
#include <shlwapi.h>

#pragma comment(lib, "mfplat")
#pragma comment(lib, "mfuuid")      // Media Foundation GUIDs
#pragma comment(lib, "strmiids")    // DirectShow GUIDs
#pragma comment(lib, "Ws2_32")      // htonl, etc
#pragma comment(lib, "shlwapi")

// Common sample files.
#include "linklist.h"

#include "asynccb.h"
#include "OpQueue.h"


template <class T> void SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

void DllAddRef();
void DllRelease();


// Forward declares
class PpboxHandler;
class PpboxSource;
class PpboxStream;
class SourceOp;

typedef ComPtrList<IMFSample>       SampleList;
typedef ComPtrList<IUnknown, true>  TokenList;    // List of tokens for IMFMediaStream::RequestSample

enum SourceState
{
    STATE_INVALID,      // Initial state. Have not started opening the stream.
    STATE_STOPPED,
    STATE_PAUSED,
    STATE_STARTED,
    STATE_SHUTDOWN
};

#include "Parse.h"          // Ppbox parser
#include "PpboxStream.h"    // Ppbox stream



const UINT32 MAX_STREAMS = 32;

class StreamList
{
    PpboxStream*    m_streams[MAX_STREAMS];
    BYTE            m_id[MAX_STREAMS];
    UINT32          m_count;

public:
    StreamList() : m_count(0)
    {
        ZeroMemory(m_streams, sizeof(m_streams));
    }
    ~StreamList()
    {
        Clear();
    }

    UINT32 GetCount() const { return m_count; }

    void Clear()
    {
        for (UINT32 i = 0; i < MAX_STREAMS; i++)
        {
            SafeRelease(&m_streams[i]);
        }
        m_count = 0;
    }

    HRESULT AddStream(BYTE id, PpboxStream *pStream)
    {
        if (GetCount() >= MAX_STREAMS)
        {
            return E_FAIL;
        }

        m_streams[m_count] = pStream;
        pStream->AddRef();

        m_id[m_count] = id;

        m_count++;

        return S_OK;
    }

    PpboxStream* Find(BYTE id)
    {

        // This method can return NULL if the source did not create a
        // stream for this ID. In particular, this can happen if:
        //
        // 1) The stream type is not supported. See IsStreamTypeSupported().
        // 2) The source is still opening.
        //
        // Note: This method does not AddRef the stream object. The source
        // uses this method to access the streams. If the source hands out
        // a stream pointer (e.g. in the MENewStream event), the source
        // must AddRef the stream object.

        PpboxStream* pStream = NULL;
        for (UINT32 i = 0; i < m_count; i++)
        {
            if (m_id[i] == id)
            {
                pStream = m_streams[i];
                break;
            }
        }
        return pStream;
    }

    // Accessor.
    PpboxStream* operator[](DWORD index)
    {
        assert(index < m_count);
        return m_streams[index];
    }

    // Const accessor.
    PpboxStream* const operator[](DWORD index) const
    {
        assert(index < m_count);
        return m_streams[index];
    }
};


// Constants

const DWORD INITIAL_BUFFER_SIZE = 4 * 1024; // Initial size of the read buffer. (The buffer expands dynamically.)
const DWORD READ_SIZE = 4 * 1024;           // Size of each read request.
const DWORD SAMPLE_QUEUE = 2;               // How many samples does each stream try to hold in its queue?

// Represents a request for an asynchronous operation.

class SourceOp : public IUnknown
{
public:

    enum Operation
    {
        OP_START,
        OP_PAUSE,
        OP_STOP,
        OP_REQUEST_DATA,
        OP_END_OF_STREAM
    };

    static HRESULT CreateOp(Operation op, SourceOp **ppOp);
    static HRESULT CreateStartOp(IMFPresentationDescriptor *pPD, SourceOp **ppOp);

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    SourceOp(Operation op);
    virtual ~SourceOp();

    HRESULT SetData(const PROPVARIANT& var);

    Operation Op() const { return m_op; }
    PROPVARIANT& Data() { return m_data;}

protected:
    long        m_cRef;     // Reference count.
    Operation   m_op;
    PROPVARIANT m_data;     // Data for the operation.
};

class StartOp : public SourceOp
{
public:
    StartOp(IMFPresentationDescriptor *pPD);
    ~StartOp();

    HRESULT GetPresentationDescriptor(IMFPresentationDescriptor **ppPD);

protected:
    IMFPresentationDescriptor   *m_pPD; // Presentation descriptor for Start operations.
};


// PpboxSource: The media source object.
class PpboxSource : public OpQueue<SourceOp>, public IMFMediaSource
{
public:
    static HRESULT CreateInstance(PpboxSource **ppSource);

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IMFMediaEventGenerator
    STDMETHODIMP BeginGetEvent(IMFAsyncCallback* pCallback,IUnknown* punkState);
    STDMETHODIMP EndGetEvent(IMFAsyncResult* pResult, IMFMediaEvent** ppEvent);
    STDMETHODIMP GetEvent(DWORD dwFlags, IMFMediaEvent** ppEvent);
    STDMETHODIMP QueueEvent(MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, const PROPVARIANT* pvValue);

    // IMFMediaSource
    STDMETHODIMP CreatePresentationDescriptor(IMFPresentationDescriptor** ppPresentationDescriptor);
    STDMETHODIMP GetCharacteristics(DWORD* pdwCharacteristics);
    STDMETHODIMP Pause();
    STDMETHODIMP Shutdown();
    STDMETHODIMP Start(
        IMFPresentationDescriptor* pPresentationDescriptor,
        const GUID* pguidTimeFormat,
        const PROPVARIANT* pvarStartPosition
    );
    STDMETHODIMP Stop();

    // Queues an asynchronous operation, specify by op-type.
    // (This method is public because the streams call it.)
    HRESULT RequestSample();

    // Lock/Unlock:
    // Holds and releases the source's critical section. Called by the streams.
    void    Lock() { EnterCriticalSection(&m_critSec); }
    void    Unlock() { LeaveCriticalSection(&m_critSec); }

private:

    HRESULT QueueAsyncOperation(SourceOp::Operation OpType);

    PpboxSource(HRESULT& hr);
    ~PpboxSource();

    // CheckShutdown: Returns MF_E_SHUTDOWN if the source was shut down.
    HRESULT CheckShutdown() const
    {
        return ( m_state == STATE_SHUTDOWN ? MF_E_SHUTDOWN : S_OK );
    }

    HRESULT     CompleteOpen(HRESULT hrStatus);

    HRESULT     IsInitialized() const;
    BOOL        IsStreamTypeSupported(StreamType type) const;
    BOOL        IsStreamActive(unsigned long stream_id);
    BOOL        StreamsNeedData() const;

    HRESULT     DoStart(StartOp *pOp);
    HRESULT     DoStop(SourceOp *pOp);
    HRESULT     DoPause(SourceOp *pOp);
    HRESULT     OnStreamRequestSample(SourceOp *pOp);
    HRESULT     OnEndOfStream(SourceOp *pOp);

    HRESULT     InitPresentationDescriptor();
    HRESULT     SelectStreams(IMFPresentationDescriptor *pPD, PROPVARIANT * varStart);

    HRESULT     DeliverPayload();
    HRESULT     EndOfStream();

    HRESULT     CreateStream(long stream_id);

    HRESULT     ValidatePresentationDescriptor(IMFPresentationDescriptor *pPD);

    // Handler for async errors.
    void        StreamingError(HRESULT hr);

    HRESULT     BeginAsyncOp(SourceOp *pOp);
    HRESULT     CompleteAsyncOp(SourceOp *pOp);
    HRESULT     DispatchOperation(SourceOp *pOp);
    HRESULT     ValidateOperation(SourceOp *pOp);

    HRESULT     OnScheduleDelayRequestSample(IMFAsyncResult *pResult);

private:
    long                        m_cRef;                     // reference count

    CRITICAL_SECTION            m_critSec;                  // critical section for thread safety
    SourceState                 m_state;                    // Current state (running, stopped, paused)

    IMFMediaEventQueue          *m_pEventQueue;             // Event generator helper
    IMFPresentationDescriptor   *m_pPresentationDescriptor; // Presentation descriptor.

    StreamList                  m_streams;                  // Array of streams.

    DWORD                       m_cPendingEOS;              // Pending EOS notifications.
    ULONG                       m_cRestartCounter;          // Counter for sample requests.

    SourceOp                    *m_pCurrentOp;
    SourceOp                    *m_pSampleRequest;

    UINT64                      m_uDuration;

    // Async callback helper.
    AsyncCallback<PpboxSource>  m_OnScheduleDelayRequestSample;
    MFWORKITEM_KEY              m_keyScheduleDelayRequestSample;
};


