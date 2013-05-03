// VideoStreamWriter.h

#include <framework/system/BytesOrder.h>
#include <framework/string/Format.h>

#include <fstream>

struct VideoStreamWriter
{
    VideoStreamWriter(
        bool write)
    {
        last_sample_time_ = 0;
        write_ = write;
    }

    void open(
        size_t stream_index, 
        PPBOX_StreamInfoEx const & stream_info)
    {
        assert(stream_info.type == ppbox_video);
        if (write_) {
            opend_ = true;
            std::string name = framework::string::format(stream_index) + ".264";
            file_.open(name.c_str(), std::ios::binary | std::ios::out);
        }
        std::cout << "Sub Type: " << stream_info.sub_type << std::endl;
        std::cout << "  Width " << stream_info.video_format.width << std::endl;
        std::cout << "  Height " << stream_info.video_format.height << std::endl;
        std::cout << "  Frame Rate " << stream_info.video_format.frame_rate << std::endl;
        format_ = stream_info.format_type;
        if (stream_info.sub_type == ppbox_video_avc) {
            if (format_ == ppbox_video_avc_packet) {
                std::cout << "Avc Config: " << stream_info.format_size << " bytes data" << std::endl;
                PP_uchar const * p = stream_info.format_buffer;
                //PP_uchar const * e = p + stream_info.format_size;
                std::cout << "  Configuration Version: " << (int)*p++ << std::endl;
                std::cout << "  Profile: " << (int)*p++ << std::endl;
                std::cout << "  Profile Compatibility: " << (int)*p++ << std::endl;
                std::cout << "  Level: " << (int)*p++ << std::endl;
                nalu_len_ = 1 + ((*p++) & 3);
                std::cout << "  NALU Length Size: " << nalu_len_ << std::endl;
                size_t n = (*p++) & 31;
                for (size_t i = 0; i < n; ++i) {
                    size_t l = (*p++);
                    l = (l << 8) + (*p++);
                    std::cout << "  Sequence Parameter: " << l << " bytes data" << std::endl;
                    write_nalu(p, l, true);
                    p += l;
                }
                n = (*p++) & 31;
                for (size_t i = 0; i < n; ++i) {
                    size_t l = (*p++);
                    l = (l << 8) + (*p++);
                    std::cout << "  Picture Parameter: " << l << " bytes data" << std::endl;
                    write_nalu(p, l, true);
                    p += l;
                }
            }
        }
    }

    template <typename SampleType>
    bool handle(
        SampleType const & sample)
    {
        if (last_sample_time_ > sample.start_time)
            return false;
        last_sample_time_ = sample.start_time;
        return handle_sample_buffer(sample.buffer, sample.buffer_length);
    }

    void seek()
    {
        last_sample_time_ = 0;
    }

    void close()
    {
        if (opend_) {
            file_.close();
            opend_ = false;
        }
    }

private:
    bool handle_sample_buffer(
        PP_uchar const * buffer, 
        PP_uint32 buffer_length)
    {
        if (format_ == ppbox_video_avc_packet) {
            PP_uint32 len = buffer_length;
            PP_uchar const * buf = buffer;
            bool start_frame = true;
            while (len > nalu_len_) {
                PP_uint32 l = 0;
                memcpy(&l, buf, nalu_len_);
                l = framework::system::BytesOrder::net_to_host_long(l);
                buf += nalu_len_;
                len -= nalu_len_;
                if (l <= len) {
                    write_nalu(buf, l, start_frame);
                    start_frame = false;
                    buf += l;
                    len -= l;
                } else {
                    break;
                }
            }
            return (len == 0);
        } else {
            write((char const *)buffer, buffer_length);
            return buffer_length > 4
                && buffer[0] == '\0'
                && buffer[1] == '\0'
                && buffer[2] == '\0'
                && buffer[3] == '\1';
        }
    }

    void write_nalu(
        PP_uchar const * buf, 
        PP_uint32 len, 
        bool start_frame)
    {
        if (start_frame)
            write("\000\000\000\001", 4);
        else
            write("\000\000\001", 3);
        write((char const *)buf, len);
    }

    void write(
        char const * buf, 
        size_t len)
    {
        if (!opend_)
            return;
        file_.write(buf, len);
    }

private:
    PP_uint32 format_;
    PP_uint32 nalu_len_;
    boost::uint64_t last_sample_time_;
    bool write_;
    std::ofstream file_;
    bool opend_;
};
