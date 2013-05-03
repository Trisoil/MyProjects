// SegmentInfo.h

#ifndef _PPBOX_DATA_SEGMENT_INFO_H_
#define _PPBOX_DATA_SEGMENT_INFO_H_

#include "ppbox/data/base/DataBase.h"

namespace ppbox 
{
    namespace data 
    {

        struct SegmentInfo
        {
            SegmentInfo()
                : head_size(invalid_size)
                , size(invalid_size)
                , offset(invalid_size)
                , duration(invalid_size)
            {
            }

            boost::uint64_t head_size;
            boost::uint64_t size;
            boost::uint64_t offset;
            boost::uint64_t duration;
        };

    } // namespace data
} // namespace ppbox

#endif//_PPBOX_DATA_SEGMENT_INFO_H_
