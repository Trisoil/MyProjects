// MkvFormat.h

#ifndef _PPBOX_AVFORMAT_MKV_MKV_FORMAT_H_
#define _PPBOX_AVFORMAT_MKV_MKV_FORMAT_H_

#include "ppbox/avformat/mkv/ebml/EBML_Type.h"
#include "ppbox/avformat/stream/FormatBuffer.h"

namespace ppbox
{
    namespace avformat
    {

        typedef EBML_OArchive MkvOArchive;

        typedef EBML_IArchive MkvIArchive;

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_MKV_MKV_FORMAT_H_
