// VideoStreamWriter.h

#include <framework/system/BytesOrder.h>
#include <framework/string/Format.h>

#include <fstream>

#include <ppbox/PptvInterface.h>


class FlvWriter
{
public:
    FlvWriter()
    {
        opend_ = false;
    }

    void open(
        size_t stream_index, 
        PPBOX_StreamInfoEx const & stream_info)
    {
       
       if(opend_ == false)
       {
            char *name_temp = "flv_test.flv";
            file_.open(name_temp, std::ios::binary | std::ios::out);
            char head[13];
            memset(head,0,sizeof(head));
            head[0] = 'F';
            head[1] = 'L';
            head[2] = 'V';
            head[3] = 0x1;
            head[4] = 0x5;
            head[8] = 0x9;
            file_.write(head,sizeof(head));
            opend_ = true;
       }
        unsigned char const *data = stream_info.format_buffer;
        boost::uint32_t data_len = stream_info.format_size;
        if (stream_info.sub_type == pptv_video_avc) {
            unsigned char tag_head[16];
            boost::uint32_t tag_len = 0;
            memset(tag_head,0,sizeof(tag_head));
            tag_head[0] = 0x9;
            boost::uint32_t size_data = 5 + stream_info.format_size;
            tag_head[1] = (size_data&0xff0000) >> 16;
            tag_head[2] = (size_data&0x00ff00) >> 8;
            tag_head[3] = (size_data&0x0000ff);
            tag_head[11] = 0x17;
            tag_head[12] = 0x0;
            write_tag(tag_head,16,data,data_len);
        }
        else if (stream_info.sub_type == pptv_audio_aac) {
            unsigned char tag_head[13];
            boost::uint32_t tag_len = 0;
            memset(tag_head,0,sizeof(tag_head));
            tag_head[0] = 0x8;
            boost::uint32_t size_data = 2 + stream_info.format_size;
            tag_head[1] = (size_data&0xff0000) >> 16;
            tag_head[2] = (size_data&0x00ff00) >> 8;
            tag_head[3] = (size_data&0x0000ff);
            tag_head[11] = 0xAF;
            tag_head[12] = 0x0;
            write_tag(tag_head,13,data,data_len);
        }
    }
    void handle_audio(
        PPBOX_SampleEx const & sample)
    {
        
        unsigned char tag_head[13];
        boost::uint32_t tag_len = 0;
        memset(tag_head,0,sizeof(tag_head));
        unsigned char const *data = sample.buffer;
        boost::uint32_t data_len = sample.buffer_length;

        tag_head[0] = 0x8;
        boost::uint32_t size_data = 2 + sample.buffer_length;//比音频少2个字节
        tag_head[1] = (size_data&0xff0000) >> 16;
        tag_head[2] = (size_data&0x00ff00) >> 8;
        tag_head[3] = (size_data&0x0000ff);
        boost::uint32_t start_time  = sample.start_time;
        tag_head[4] = (start_time&0xff0000) >> 16;
        tag_head[5] = (start_time&0x00ff00) >> 8;
        tag_head[6] = (start_time&0x0000ff);
        tag_head[11] = 0xAF;//1001 1111---- AAC
        tag_head[12] = 0x1;
        write_tag(tag_head,13,data,data_len);

    }
    void handle_video(
        PPBOX_SampleEx const & sample)
    {
        
        unsigned char tag_head[16];//纯数据前的16个字节描述（包括TAG信息，和视频信息）
        boost::uint32_t tag_len = 0;
        memset(tag_head,0,sizeof(tag_head));
        unsigned char const *data = sample.buffer;
        boost::uint32_t data_len = sample.buffer_length;
     
        tag_head[0] = 0x9;
        boost::uint32_t size_data = 5 + sample.buffer_length;//视频tag_data中的5个字节的关于音频的描述信息
        tag_head[1] = (size_data&0xff0000) >> 16;
        tag_head[2] = (size_data&0x00ff00) >> 8;
        tag_head[3] = (size_data&0x0000ff);
        boost::uint32_t start_time  = sample.start_time;
        tag_head[4] = (start_time&0xff0000) >> 16;
        tag_head[5] = (start_time&0x00ff00) >> 8;
        tag_head[6] = (start_time&0x0000ff);
        if(sample.is_sync)
            tag_head[11] = 0x17; //0001 -----keyframe (AVC)AVCVIDEOPACKET
        else
            tag_head[11] = 0x27;
        tag_head[12] = 0x1;
         write_tag(tag_head,16,data,data_len);

      
    }
    void close()
    {
        if (opend_) {
            file_.close();
            opend_ = false;
        }
    }
    void write_tag(unsigned char const * head_buf,
        boost::uint32_t tag_head_len,
        unsigned char const * data_buf,
        boost::uint32_t len)
    {

        file_.write((char const *)head_buf,tag_head_len);
        file_.write((char const *)data_buf,len);
        boost::uint32_t tag_total_len = len + 16;
        char  temp_buf[4];
        memset(temp_buf,0,sizeof(temp_buf));
        temp_buf[0] = (tag_total_len&0x00ff0000) >> 24;
        temp_buf[1] = (tag_total_len&0x00ff0000) >> 16;
        temp_buf[2] = (tag_total_len&0x0000ff00) >> 8;
        temp_buf[3] = (tag_total_len&0x000000ff);
        file_.write((char const*)temp_buf,sizeof(temp_buf));
    }
private:
    bool opend_;
    std::ofstream file_;
};
