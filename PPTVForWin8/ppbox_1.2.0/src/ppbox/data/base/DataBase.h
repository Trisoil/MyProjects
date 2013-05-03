// DataBase.h

#ifndef _PPBOX_DATA_BASE_DATA_BASE_H_
#define _PPBOX_DATA_BASE_DATA_BASE_H_

#include <framework/string/Url.h>

namespace ppbox 
{
    namespace data 
    {

        static boost::uint64_t const invalid_size = boost::uint64_t(-1);

        class MediaBase;

        class SourceBase;
        class SinkBase;

        class SegmentMedia;

    } // namespace data
} // namespace ppbox

#endif//_PPBOX_DATA_BASE_DATA_BASE_H_
