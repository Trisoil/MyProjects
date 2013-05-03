// StreamStatus.h

#ifndef _PPBOX_DATA_BASE_STREAM_STATUS_H_
#define _PPBOX_DATA_BASE_STREAM_STATUS_H_

namespace ppbox 
{
    namespace data 
    {

        struct StreamRange
        {
            StreamRange()
                : beg(0)
                , end(0)
                , pos(0)
                , buf(0)
            {
            }

            boost::uint64_t beg;
            boost::uint64_t end;
            boost::uint64_t pos;
            boost::uint64_t buf;
        };

        struct StreamStatus
        {
            StreamStatus()
                : video_index(size_t(-1))
                , audio_index(size_t(-1))
            {
            }

            size_t video_index;
            size_t audio_index;
            StreamRange byte_range;
            StreamRange time_range;
            boost::system::error_code buf_ec;
            std::string desc;
        }; 

    } // namespace data
} // namespace ppbox

#endif //_PPBOX_DATA_BASE_STREAM_STATUS_H_
