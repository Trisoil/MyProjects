//------------------------------------------------------------------------------
// File: PushSourceBitmap.cpp
//
// Desc: DirectShow sample code - In-memory push mode source filter
//       Provides a static bitmap as the video output stream.
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#define PPBOX_EXTERN
#include "SampleCache.h"

#include <streams.h>
#include <dvdmedia.h>
#include <wmcodecdsp.h>

#include "PpboxPin.h"
#include "PpboxGuids.h"

#define UINTS_MICROSECOND    (UNITS / 1000000)
#define UINTS_MILLISECOND    (UNITS / 1000)

const AMOVIESETUP_MEDIATYPE sudOpPinTypes =
{
    &MEDIATYPE_Video,       // Major type
    &MEDIASUBTYPE_NULL      // Minor type
};


/**********************************************
 *
 *  CPpboxPin Class
 *  
 *
 **********************************************/

CPpboxPin::CPpboxPin(HRESULT *phr, CSource *pFilter, 
                     unsigned long index, 
                     PPBOX_StreamInfoEx const *info, 
                     SampleCache * pSampleCache)
      : CSourceStream(NAME("Pppbox Source"), phr, pFilter, L"Out")
      , CSourceSeeking(NAME("Pppbox Source"), (IPin*)this, phr, &m_cSharedState)
      , m_nIndex(index)
      , m_nSampleDuration(0)
      , m_info(new PPBOX_StreamInfoEx(*info))
      , m_SampleCache(pSampleCache)
      , m_bCancel(FALSE)
      , m_bDiscontinuty(FALSE)
      , m_bSeekable(FALSE)
{
    unsigned long duration = PPBOX_GetDuration();
    m_bSeekable = duration != (unsigned long)-1;
    if (m_bSeekable)
        m_rtDuration = m_rtStop = duration * UINTS_MILLISECOND;
    m_info->format_buffer = new PP_uchar[info->format_size];
    memcpy((void *)m_info->format_buffer, info->format_buffer, info->format_size);
}

CPpboxPin::~CPpboxPin()
{
    delete [] m_info->format_buffer;
    delete m_info;
}

HRESULT CPpboxPin::GetMediaType(CMediaType *pMediaType)
{
    CAutoLock cAutoLock(m_pFilter->pStateLock());

    CheckPointer(pMediaType, E_POINTER);

    return FillMediaType(pMediaType);
}

HRESULT CPpboxPin::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pRequest)
{
    HRESULT hr;
    CAutoLock cAutoLock(m_pFilter->pStateLock());

    CheckPointer(pAlloc, E_POINTER);
    CheckPointer(pRequest, E_POINTER);

    // Ensure a minimum number of buffers
    if (pRequest->cBuffers == 0)
    {
        pRequest->cBuffers = 20;
    }
    if (m_info->type == ppbox_video)
        pRequest->cbBuffer = 2 * 1024 * 1024;
    else
        pRequest->cbBuffer = 2 * 1024;

    ALLOCATOR_PROPERTIES Actual;
    hr = pAlloc->SetProperties(pRequest, &Actual);
    if (FAILED(hr)) 
    {
        return hr;
    }

    // Is this allocator unsuitable?
    if (Actual.cbBuffer < pRequest->cbBuffer) 
    {
        return E_FAIL;
    }

    return S_OK;
}


// This is where we insert the DIB bits into the video stream.
// FillBuffer is called once for every sample in the stream.
HRESULT CPpboxPin::FillBuffer(IMediaSample *pSample)
{
    CheckPointer(pSample, E_POINTER);

    // If the bitmap file was not loaded, just fail here.
    CAutoLock cAutoLockShared(&m_cSharedState);

    PPBOX_SampleEx2 sample;
    sample.stream_index = m_nIndex;
    HRESULT ret = m_SampleCache->ReadSample(sample, &m_bCancel);
    if (ret == S_OK) {
        BYTE *pData;
        long cbData;
        pSample->GetPointer(&pData);
        cbData = pSample->GetSize();
        if (cbData > sample.buffer_length)
            cbData = sample.buffer_length;
        memcpy(pData, sample.buffer, cbData);
        pSample->SetActualDataLength(cbData);
        if (sample.start_time * UINTS_MICROSECOND < m_rtStart)
            m_rtStart = sample.start_time * UINTS_MICROSECOND;
        REFERENCE_TIME rtStart = sample.start_time * UINTS_MICROSECOND - m_rtStart;
        REFERENCE_TIME rtStop  = rtStart + m_nSampleDuration;
        pSample->SetTime(&rtStart, &rtStop);
        pSample->SetSyncPoint(sample.is_sync);
        pSample->SetDiscontinuity(m_bDiscontinuty);
        m_bDiscontinuty = FALSE;
    }
    return ret;
}

void CPpboxPin::UpdateFromSeek()
{
    if (ThreadExists()) 
    {
        OutputDebugString(_T("UpdateFromSeek Cancel\r\n"));
        m_bCancel = TRUE;
        DeliverBeginFlush();
        // Shut down the thread and stop pushing data.
        Stop();
        m_SampleCache->Seek(m_rtStart);
        m_bDiscontinuty = TRUE;
        OutputDebugString(_T("UpdateFromSeek Resume\r\n"));
        m_bCancel = FALSE;
        DeliverEndFlush();
        // Restart the thread and start pushing data again.
        Pause();
    }
}

HRESULT CPpboxPin::OnThreadStartPlay()
{
    return DeliverNewSegment(m_rtStart, m_rtStop, m_dRateSeeking);
}

HRESULT CPpboxPin::ChangeStart()
{
    UpdateFromSeek();
    return S_OK;
}

HRESULT CPpboxPin::ChangeStop()
{
    //UpdateFromSeek();
    return S_OK;
}

static HRESULT FillAVC1(CMediaType *pMediaType, PPBOX_StreamInfoEx const * m_info)
{
    MPEG2VIDEOINFO * pmvi =  // maybe  sizeof(MPEG2VIDEOINFO) + m_info->format_size - 7 - 4
        (MPEG2VIDEOINFO *)pMediaType->AllocFormatBuffer(sizeof(MPEG2VIDEOINFO) + m_info->format_size - 11);
    if (pmvi == 0) 
        return(E_OUTOFMEMORY);
    ZeroMemory(pmvi, pMediaType->cbFormat);   

    pMediaType->SetSubtype(&MEDIASUBTYPE_AVC1);
    pMediaType->SetFormatType(&FORMAT_MPEG2Video);
    pMediaType->SetTemporalCompression(TRUE);
    pMediaType->SetVariableSize();

    VIDEOINFOHEADER2 * pvi = &pmvi->hdr;
    SetRectEmpty(&(pvi->rcSource));
    SetRectEmpty(&(pvi->rcTarget));
    pvi->AvgTimePerFrame = UNITS / m_info->video_format.frame_rate;

    BITMAPINFOHEADER * bmi = &pvi->bmiHeader;
    bmi->biSize = sizeof(BITMAPINFOHEADER);
    bmi->biWidth = m_info->video_format.width;
    bmi->biHeight = m_info->video_format.height;
    bmi->biBitCount = 0;
    bmi->biPlanes = 1;
    bmi->biCompression = 0x31435641; // AVC1

    //pmvi->dwStartTimeCode = 0;
    pmvi->cbSequenceHeader = m_info->format_size - 7;
    BYTE * s = (BYTE *)pmvi->dwSequenceHeader;

    PP_uchar const * p = m_info->format_buffer;
    //PP_uchar const * e = p + stream_info.format_size;
    PP_uchar Version = *p++;
    PP_uchar Profile = *p++;
    PP_uchar Profile_Compatibility = *p++;
    PP_uchar Level = *p++;
    PP_uchar Nalu_Length = 1 + ((*p++) & 3);
    size_t n = (*p++) & 31;
    PP_uchar const * q = p;
    for (size_t i = 0; i < n; ++i) {
        size_t l = (*p++);
        l = (l << 8) + (*p++);
        p += l;
    }
    memcpy(s, q, p - q);
    s += p - q;
    n = (*p++) & 31;
    q = p;
    for (size_t i = 0; i < n; ++i) {
        size_t l = (*p++);
        l = (l << 8) + (*p++);
        p += l;
    }
    memcpy(s, q, p - q);
    s += p - q;

    pmvi->dwProfile = Profile;
    pmvi->dwLevel = Level;
    pmvi->dwFlags = Nalu_Length;

    return S_OK;
}

static HRESULT FillAAC1(CMediaType *pMediaType, PPBOX_StreamInfoEx const * m_info)
{
    WAVEFORMATEX  * wf = (WAVEFORMATEX  *)pMediaType->AllocFormatBuffer(sizeof(WAVEFORMATEX) + m_info->format_size);
    if (wf == 0) 
        return(E_OUTOFMEMORY);
    ZeroMemory(wf, pMediaType->cbFormat);

    pMediaType->SetSubtype(&MEDIASUBTYPE_RAW_AAC1);
    pMediaType->SetFormatType(&FORMAT_WaveFormatEx);
    pMediaType->SetTemporalCompression(TRUE);
    pMediaType->SetVariableSize();

    wf->cbSize = sizeof(WAVEFORMATEX);
    wf->nChannels = m_info->audio_format.channel_count;
    wf->nSamplesPerSec = m_info->audio_format.sample_rate;
    wf->wBitsPerSample = m_info->audio_format.sample_size;
    wf->wFormatTag = WAVE_FORMAT_RAW_AAC1;
    memcpy(wf + 1, m_info->format_buffer, m_info->format_size);

    return S_OK;
}

HRESULT CPpboxPin::FillMediaType(CMediaType *pMediaType)
{
    if (m_info->type == ppbox_video) {
        m_nSampleDuration = UNITS / m_info->video_format.frame_rate;
        pMediaType->SetType(&MEDIATYPE_Video);
        if (m_info->sub_type == ppbox_video_avc) {
            if (m_info->format_type == ppbox_video_avc_packet) {
                return FillAVC1(pMediaType, m_info);
            }
        }
    } else if (m_info->type == ppbox_audio) {
        pMediaType->SetType(&MEDIATYPE_Audio);
        if (m_info->sub_type == ppbox_audio_aac) {
            if (m_info->format_type == ppbox_audio_iso_mp4) {
                return FillAAC1(pMediaType, m_info);
            }
        }
    }

    return E_FAIL;
}

STDMETHODIMP CPpboxPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
    if( riid == IID_IMediaSeeking && m_bSeekable) 
    {
        return CSourceSeeking::NonDelegatingQueryInterface( riid, ppv );
    }
    return CSourceStream::NonDelegatingQueryInterface(riid, ppv);
}
