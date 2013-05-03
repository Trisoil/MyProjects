// MkvAlgorithm.cpp

#include "ppbox/avformat/Common.h"
#include "ppbox/avformat/mkv/MkvAlgorithm.h"
#include <ppbox/avformat/mkv/MkvObjectType.h>

#include "ppbox/demux/base/DemuxError.h"


namespace ppbox
{
    namespace avformat
    {

        boost::uint64_t mkv_head_size(
            std::basic_istream<boost::uint8_t> & is, 
            boost::system::error_code & ec)
        {
            boost::uint32_t const ids[] = {
                MkvSegmentInfo::StaticId, 
                MkvTracks::StaticId, 
                MkvCluster::StaticId, 
            };

            MkvIArchive ia(is);
            std::ios::pos_type old_pos = ia.tellg();
            assert(ia);
            boost::uint64_t head_len = 0;
            boost::uint64_t seg_end = 0;
            boost::uint32_t find_flag = 0;

            while (true) {
                ia.seekg(head_len, std::ios_base::beg);
                EBML_ElementHeader header;
                ia >> header;
                if (!ia) {
                    ia.clear();
                    assert(ia);
                    head_len += 2;
                    ec = ppbox::demux::error::file_stream_error;
                    break;
                }
                if (seg_end == 0) {
                    if (header.Id == MkvSegment::StaticId) {
                        seg_end = (boost::uint64_t)ia.tellg() + header.data_size();
                        head_len += header.head_size();
                    } else {
                        head_len += header.byte_size();
                    }
                } else {
                    for (size_t i = 0; i < sizeof(ids) / sizeof(ids[0]); ++i) {
                        if(header.Id == ids[i]) {
                            find_flag |= 1 << i;
                            break;
                        }
                    }
                    if (find_flag == (1 << sizeof(ids) / sizeof(ids[0])) - 1) {
                        ec.clear();
                        if(header.Id != MkvCluster::StaticId) {
                            head_len += header.byte_size();
                            ia.seekg(head_len, std::ios_base::beg);
                            if (!ia) {
                                ia.clear();
                                assert(ia);
                                ec = ppbox::demux::error::file_stream_error;
                            }
                        }
                        break;
                    }
                    head_len += header.byte_size();
                    if (head_len >= seg_end) {
                        ec = ppbox::demux::error::bad_file_format;
                        break;
                    }
                }
            }
            is.seekg(old_pos, std::ios::beg);
            assert(is);
            return head_len;
        }

    } // namespace avformat
} // namespace ppbox
