// Mp4Merge.cpp

#include "ppbox/avformat/Common.h"
#include "ppbox/avformat/mp4/Mp4Merge.h"
#include "ppbox/avformat/mp4/Ap4Merge.h"
#include "ppbox/avformat/mp4/Ap4Stream.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.avformat.Mp4Merge", framework::logger::Debug);

namespace ppbox
{
    namespace avformat
    {

        bool mp4_merge_head(
            std::istream & is,
            std::ostream & os, 
            std::vector<ppbox::data::SegmentInfo> & segment_infos,
            boost::system::error_code & ec)
        {
            if (segment_infos.empty())
                return true;

            AP4_File * file = NULL;
            boost::uint64_t offset = 0;
            // 第一段数据需要拷贝出来，最后写的时候会使用这些数据
            std::string str(segment_infos[0].head_size, 0);
            is.read(&str[0], str.size());
            std::istringstream iss(str);
            AP4_Stream first_stream(iss, segment_infos[0]);
            for (boost::uint32_t i = 0; i < segment_infos.size(); ++i) {
                is.seekg(offset, std::ios::beg);
                AP4_Stream stream(is, segment_infos[i]);
                if (!ap4_merge(file, i == 0 ? first_stream : stream, segment_infos[i], ec))
                    break;
                offset += segment_infos[i].head_size;
            }

            if (!ec) {
                AP4_Stream stream(os);
                ap4_write(file, stream, ec);
            }

            return !ec;
        }
        /*
        bool vod_valid_segment_info(
        std::istream & is,
        std::vector<ppbox::data::SegmentInfo> & segment_infos,
        boost::system::error_code & ec)
        {
        is.seekg(0, std::ios::end);
        boost::uint32_t big_head_size = is.tellg();
        is.seekg(0, std::ios::beg);
        AP4_ByteStream* obj_stream = NULL;
        AP4_StreamByteStream::Create(is, 
        0, 
        big_head_size,
        obj_stream);

        Ap4HeadMerge mp4_head;
        mp4_head.stream_ = obj_stream;
        mp4_head.CheckMp4Head(ec);
        boost::uint64_t duration_sum = 0;
        if (!ec) {
        for (boost::uint32_t i = 0; i < segment_infos.size()-1; ++i) {
        duration_sum += segment_infos[i].duration;
        boost::uint32_t diff_offset;
        mp4_head.FindMinOffset(duration_sum, diff_offset, i, segment_infos, ec);
        if (ec) {
        break;
        }
        segment_infos[i].size -= diff_offset;
        //segment_infos[i].size -= 34259;
        //diff_offset = diff_offset;
        }
        }
        return !ec;
        }
        */
    } // namespace avformat
} // namespace ppbox
