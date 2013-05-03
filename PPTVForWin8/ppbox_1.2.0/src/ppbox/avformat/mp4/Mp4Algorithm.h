// Mp4Algorithm.h

#ifndef _PPBOX_AVFORMAT_MP4_MP4_ALGORITHM_H_
#define _PPBOX_AVFORMAT_MP4_MP4_ALGORITHM_H_

#include <istream>

namespace ppbox
{
    namespace avformat
    {

        boost::uint64_t mp4_head_size(
            std::basic_istream<boost::uint8_t> & is, 
            boost::system::error_code & ec);

    } // namespace avformat
} // namespace ppbox

#endif // End _PPBOX_AVFORMAT_MP4_MP4_ALGORITHM_H_
