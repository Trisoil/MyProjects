// WriteFile.h

#include <fstream>
#include <iostream>

struct FileWriter
{
    FileWriter()
        : last_offset_(PP_uint64(-1))
        , opend_(false)
    {
    }

    void open(
        char const * name)
    {
    }

    void handle(PPBOX_SampleEx const & sample)
    {
        if (sample.offset_in_file < last_offset_) {
            PPBOX_SegmentInfo segment;
            PPBOX_GetSegmentInfo(&segment);
            if (opend_) {
                file_.close();
                opend_ = false;
            }
            char name[] = "xxx.mp4";
            name[0] = segment.index / 100 + '0';
            name[1] = (segment.index % 100) / 10 + '0';
            name[2] = segment.index % 10 + '0';
            file_.open(name, std::ios::binary | std::ios::out);
            opend_ = true;
            file_.write((char const *)segment.head_buffer, segment.head_length);
            last_offset_ = segment.head_length;
            file_.seekp(last_offset_, std::ios::beg);
            file_.flush();
        }
        assert(last_offset_ <= sample.offset_in_file);
        if (last_offset_ < sample.offset_in_file) {
            std::cout << "writer skip " << last_offset_ << "(" << sample.offset_in_file - last_offset_ << ")" << std::endl;
            last_offset_ = sample.offset_in_file;
        }
        file_.write((char const *)sample.buffer, sample.buffer_length);
        last_offset_ += sample.buffer_length;
        file_.seekp(last_offset_, std::ios::beg);
    }

    void seek()
    {
        last_offset_ = PP_uint64(-1);
    }

    void close()
    {
        if (opend_) {
            file_.close();
            opend_ = false;
        }
    }

private:
    PP_uint64 last_offset_;
    std::ofstream file_;
    bool opend_;
};
