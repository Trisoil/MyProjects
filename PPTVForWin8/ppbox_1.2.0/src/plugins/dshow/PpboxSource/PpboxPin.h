//------------------------------------------------------------------------------
// File: PpboxPin.h
//
// Desc: DirectShow sample code - In-memory push mode source filter
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

/**********************************************
 *
 *  Class declarations
 *
 **********************************************/
//extern "C" {
//    struct PPBOX_StreamInfoEx;
//};

class CPpboxPin : public CSourceStream, public CSourceSeeking
{
protected:
    unsigned long m_nIndex;
    unsigned long m_nSampleDuration;
    PPBOX_StreamInfoEx * m_info;
    SampleCache * m_SampleCache;
    BOOL m_bCancel;
    BOOL m_bDiscontinuty;
    BOOL m_bSeekable;

    //int m_FramesWritten;                // To track where we are in the file
    //BOOL m_bZeroMemory;                 // Do we need to clear the buffer?
    //CRefTime m_rtSampleTime;            // The time stamp for each sample

    //BITMAPINFO *m_pBmi;                 // Pointer to the bitmap header
    //DWORD       m_cbBitmapInfo;         // Size of the bitmap header
    

    //int m_iFrameNumber;
    //const REFERENCE_TIME m_rtFrameLength;
    CCritSec m_cSharedState;            // Protects our internal state
    //CImageDisplay m_Display;            // Figures out our media type for us

public:

    CPpboxPin(
        HRESULT *phr, 
        CSource *pFilter, 
        unsigned long index, 
        PPBOX_StreamInfoEx const *info, 
        SampleCache * m_SampleCache);

    ~CPpboxPin();

    // Override the version that offers exactly one media type
    HRESULT GetMediaType(CMediaType *pMediaType);
    HRESULT DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pRequest);
    HRESULT FillBuffer(IMediaSample *pSample);
    
    // Quality control
    // Not implemented because we aren't going in real time.
    // If the file-writing filter slows the graph down, we just do nothing, which means
    // wait until we're unblocked. No frames are ever dropped.
    STDMETHODIMP Notify(IBaseFilter *pSelf, Quality q)
    {
        return E_FAIL;
    }

private:
    HRESULT FillMediaType(CMediaType *pMediaType);
    void UpdateFromSeek();

private:
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

private:
    virtual HRESULT OnThreadStartPlay(void);

    virtual HRESULT ChangeStart();
    virtual HRESULT ChangeStop();
    virtual HRESULT ChangeRate() {return E_FAIL;};
};
