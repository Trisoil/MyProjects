// FormatBuffer.h

#ifndef _PPBOX_AVFORMAT_STREAM_FORMAT_BUFFER_H_
#define _PPBOX_AVFORMAT_STREAM_FORMAT_BUFFER_H_

#include <util/archive/ArchiveBuffer.h>

namespace ppbox
{
    namespace avformat
    {

        typedef util::archive::ArchiveBuffer<boost::uint8_t> FormatBuffer;

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_STREAM_FORMAT_BUFFER_H_
