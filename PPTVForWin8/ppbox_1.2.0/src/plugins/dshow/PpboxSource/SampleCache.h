// Ppbox.h

#ifndef PPBOX_DEFINE
#define PPBOX_EXTERN
#endif

#include "plugins/ppbox/ppbox.h"

#include <streams.h>

class SampleCache
{
public:
    SampleCache();

    ~SampleCache();

    HRESULT ReadSample(PPBOX_SampleEx2 & sample, BOOL * cancel);

    HRESULT Seek(REFERENCE_TIME time);

private:
    void Clear(int index);

private:
    CCritSec m_cSharedState;            // Protects our internal state
    CGenericList<PPBOX_SampleEx2> * m_listSamples[2];
    unsigned char const * m_pTempBuffer[2];
};
