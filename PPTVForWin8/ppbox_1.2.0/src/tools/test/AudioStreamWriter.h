// VideoStreamWriter.h

#include <framework/system/BytesOrder.h>
#include <framework/string/Format.h>

#include <fstream>

class BitsReader
{
public:
    BitsReader(
        boost::uint8_t const * buf, 
        boost::uint32_t size)
        : buf_(buf)
        , size_(size - 1)
        , size_this_byte_(8)
        , failed_(false)
    {
        assert(size > 0);
    }

    boost::uint32_t read_bits(
        boost::uint32_t len)
    {
        if (size_this_byte_ + size_ * 8 < len || len > 32) {
            failed_ = true;
        }
        if (failed_) {
            return 0;
        }
        boost::uint32_t v = 0;
        while (len > size_this_byte_) {
            v = (v << size_this_byte_) | (((boost::uint32_t)(*buf_)) & ((1 << size_this_byte_) - 1));
            len -= size_this_byte_;
            ++buf_;
            --size_;
            size_this_byte_ = 8;
        }
        if (len) {
            v = (v << len) | ((((boost::uint32_t)(*buf_)) >> (size_this_byte_ - len)) & ((1 << len) - 1));
            size_this_byte_ -= len;
        }
        return v;
    }

private:
    boost::uint8_t const * buf_;
    boost::uint32_t size_;
    boost::uint32_t size_this_byte_;
    bool failed_;
};

class BitsWriter
{
public:
    BitsWriter(
        boost::uint8_t * buf, 
        boost::uint32_t size)
        : buf_(buf)
        , size_(size - 1)
        , size_this_byte_(8)
        , failed_(false)
    {
        assert(size > 0);
        memset(buf, 0, size);
    }

    boost::uint32_t write_bits(
        boost::uint32_t v, 
        boost::uint32_t len)
    {
        if (size_this_byte_ + size_ * 8 < len || len > 32) {
            failed_ = true;
        }
        if (failed_) {
            return 0;
        }
        while (len > size_this_byte_) {
            *buf_ = (boost::uint8_t )(((boost::uint32_t)(*buf_)) | (((v >> (len - size_this_byte_)) & ((1 << size_this_byte_) - 1))));
            len -= size_this_byte_;
            ++buf_;
            --size_;
            size_this_byte_ = 8;
        }
        if (len) {
            *buf_ = (boost::uint8_t )(((boost::uint32_t)(*buf_)) | ((v & ((1 << len) - 1)) << (size_this_byte_ - len)));
            size_this_byte_ -= len;
        }
        return v;
    }

private:
    boost::uint8_t * buf_;
    boost::uint32_t size_;
    boost::uint32_t size_this_byte_;
    bool failed_;
};

struct AudioStreamWriter
{
    AudioStreamWriter(
        bool write)
    {
        last_sample_time_ = 0;
        write_ = write;
    }

    void open(
        size_t stream_index, 
        PPBOX_StreamInfoEx const & stream_info)
    {
        assert (stream_info.type == ppbox_audio);
        std::cout << "Sub Type: " << stream_info.sub_type << std::endl;
        assert (stream_info.type == ppbox_audio);
        std::cout << "  Channel Count " << stream_info.audio_format.channel_count << std::endl;
        std::cout << "  Sample Rate " << stream_info.audio_format.sample_rate << std::endl;
        std::cout << "  Sample Size " << stream_info.audio_format.sample_size << std::endl;
        if (write_) {
            opend_ = true;
            std::string name = framework::string::format(stream_index) + ".aac";
            file_.open(name.c_str(), std::ios::binary | std::ios::out);
        }
        if (stream_info.sub_type == ppbox_audio_aac) {
            BitsReader reader(stream_info.format_buffer, stream_info.format_size);
            object_type_ = (PP_uchar)reader.read_bits(5);
            if (object_type_ == 31) {
                object_type_ = (PP_uchar)(32 + reader.read_bits(6));
            }
            frequency_index_ = (PP_uchar)reader.read_bits(4);
            PP_uint32 frequency = 0;
            if (frequency_index_ == 15) {
                frequency = (PP_uint32)reader.read_bits(24);
            }
            channel_config_ = (PP_uchar)reader.read_bits(4);
            std::cout << "  Object Type: " << (int)object_type_ << std::endl;
            std::cout << "  Frequency: " << frequency << std::endl;
            std::cout << "  Channel Config: " << (int)channel_config_ << std::endl;
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
    void write_adts(PP_uint32 frame_size)
    {
        unsigned char bits[7];
        frame_size += sizeof(bits);
        BitsWriter writer(bits, sizeof(bits));
        writer.write_bits(0x00000fff, 12);//0:  syncword 12 always: '111111111111' 
        writer.write_bits(0x00000000, 01);//12: ID 1 0: MPEG-4, 1: MPEG-2 
        writer.write_bits(0x00000000, 02);//13: layer 2 always: '00' 
        writer.write_bits(0x00000001, 01);//15: protection_absent 1  
        writer.write_bits(object_type_ - 1, 02);//16: profile 2  
        writer.write_bits(frequency_index_, 04);//18: sampling_frequency_index 4  
        writer.write_bits(0x00000000, 01);//22: private_bit 1  
        writer.write_bits(channel_config_, 03);///23: channel_configuration 3  
        writer.write_bits(0x00000000, 01);//26: original/copy 1  
        writer.write_bits(0x00000000, 01);//27: home 1  
        //writer.write_bits(0x00000001, 01);//28: emphasis 2 only if ID == 0 
        writer.write_bits(0x00000000, 01);//28: copyright_identification_bit 1  
        writer.write_bits(0x00000000, 01);//29: copyright_identification_start 1  
        writer.write_bits(frame_size, 13);//30: aac_frame_length 13 length of the frame including header (in bytes) 
        writer.write_bits(0x000007ff, 11);//43: adts_buffer_fullness 11 0x7FF indicates VBR 
        writer.write_bits(0x00000000, 02);//54: no_raw_data_blocks_in_frame 2  

        write((char const *)bits, sizeof(bits));
    }

    bool handle_sample_buffer(
        PP_uchar const * buffer, 
        PP_uint32 buffer_length)
    {
        write_adts(buffer_length);
        write((char const *)buffer, buffer_length);
        return true;
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
    PP_uchar object_type_;
    PP_uchar frequency_index_;
    PP_uchar channel_config_;
    boost::uint64_t last_sample_time_;
    bool write_;
    std::ofstream file_;
    bool opend_;
};
