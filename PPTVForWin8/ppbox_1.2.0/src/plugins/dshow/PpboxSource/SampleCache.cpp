// Ppbox.cpp

#define PPBOX_DEFINE
#include "SampleCache.h"

SampleCache::SampleCache()
{
    m_listSamples[0] = new CGenericList<PPBOX_SampleEx2>(_T("SampleCache 0"), 10, FALSE);
    m_listSamples[1] = new CGenericList<PPBOX_SampleEx2>(_T("SampleCache 1"), 10, FALSE);
    m_pTempBuffer[0] = m_pTempBuffer[1] = NULL;
}

SampleCache::~SampleCache()
{
    Clear(0);
    Clear(1);
    delete m_listSamples[0];
    delete m_listSamples[1];
}

HRESULT SampleCache::ReadSample(PPBOX_SampleEx2 & sample, BOOL * cancel)
{
    unsigned long index = sample.stream_index;

    while (!*cancel) {
        {
            CAutoLock cAutoLockShared(&m_cSharedState);
            if (m_listSamples[index]->GetCount() > 0) {
                sample = *m_listSamples[index]->GetHead();
                delete m_listSamples[index]->GetHead();
                m_listSamples[index]->RemoveHead();
                if (m_pTempBuffer[index])
                    delete [] m_pTempBuffer[index];
                m_pTempBuffer[index] = sample.buffer;
                return S_OK;
            }
            long ret = PPBOX_ReadSampleEx2(&sample);
            if (ret == ppbox_success) {
                if (sample.stream_index != index) {
                    PPBOX_SampleEx2 * sample2 = new PPBOX_SampleEx2(sample);
                    sample2->buffer = new unsigned char[sample.buffer_length];
                    memcpy((void *)sample2->buffer, sample.buffer, sample.buffer_length);
                    m_listSamples[sample.stream_index]->AddTail(sample2);
                    continue;
                }
                //OutputDebugString(_T("PPBOX_ReadSampleEx2 OK\r\n"));
                return S_OK;
            } else if (ret == ppbox_stream_end) {
                DbgLog((LOG_TRACE, 5, _T("PPBOX_ReadSampleEx2 ppbox_stream_end\r\n")));
                return S_FALSE;
            } else if (ret == ppbox_would_block) {
                DbgLog((LOG_TRACE, 5, _T("PPBOX_ReadSampleEx2 ppbox_would_block\r\n")));
            } else {
                USES_CONVERSION;
                DbgLog((LOG_TRACE, 5, _T("PPBOX_ReadSampleEx2 %s\r\n"), A2W(PPBOX_GetLastErrorMsg())));
                return S_FALSE;
                //return E_FAIL;
            }
        }
        Sleep(100);
    }
    OutputDebugString(_T("PPBOX_ReadSampleEx2 Cancel\r\n"));
    return S_FALSE;
}

HRESULT SampleCache::Seek(REFERENCE_TIME time)
{
    CAutoLock cAutoLockShared(&m_cSharedState);
    Clear(0);
    Clear(1);
    long ret = PPBOX_Seek(time * 1000 / UNITS);
    if (ret == ppbox_success) {
        return S_OK;
    } else if (ret == ppbox_would_block) {
        return S_OK;
    } else {
        return E_FAIL;
    }
}

void SampleCache::Clear(int index)
{
    if (m_pTempBuffer[index])
        delete [] m_pTempBuffer[index];
    m_pTempBuffer[index] = NULL;
    while (m_listSamples[index]->GetCount() > 0) {
        delete [] m_listSamples[index]->GetHead()->buffer;
        delete m_listSamples[index]->GetHead();
        m_listSamples[index]->RemoveHead();
    }
}