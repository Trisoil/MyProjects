// MP4Merge.h

#ifndef _PPBOX_MERGE_MP4_MP4_MERGE_H_
#define _PPBOX_MERGE_MP4_MP4_MERGE_H_

#include <ppbox/data/segment/SegmentInfo.h>

namespace ppbox
{
    namespace avformat
    {

        bool mp4_merge_head(
                std::istream & is,
                std::ostream & os, 
                std::vector<ppbox::data::SegmentInfo> & segment_infos, 
                boost::system::error_code & ec);

        bool vod_valid_segment_info(
            std::istream & is,
            std::vector<ppbox::data::SegmentInfo> & segment_infos,
            boost::system::error_code & ec);

    } // namespace merge
} // namespace ppbox

#endif // _PPBOX_MERGE_MP4_MP4_MERGE_H_
