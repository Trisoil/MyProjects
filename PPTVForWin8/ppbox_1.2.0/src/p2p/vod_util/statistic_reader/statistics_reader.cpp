// statistics_reader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//#include "p2sp/AppModule.h"
#include <iosfwd>
#include <istream>
#include <fstream>
#include "zlib/zlib.h"
#include "zlib/zutil.h"
#include "statistic/StatisticsRequest.h"
#include "statistic/StatisticStructs.h"
#include "statistic/StatisticsData.h"
#include "statistic/GZipCompressor.h"
#include <boost/interprocess/streams/vectorstream.hpp>
#include "xml_creater.h"
#include "csv_creater.h"

class Decompressor
{
private:
    int max_raw_data_size_;

    int raw_data_size_;
    char* raw_data_buffer_;

public:
    Decompressor(int max_raw_data_size)
        :max_raw_data_size_(max_raw_data_size), raw_data_size_(0), raw_data_buffer_(0)
    {}

    ~Decompressor()
    {
        delete[] raw_data_buffer_;
    }

    bool Decompress(const char* path);

    int GetRawDataSize() const { return raw_data_size_; }

    const char* GetRawData() const { return raw_data_buffer_; }

private:

    bool DoDecompress(char* buffer, boost::uint32_t compress_data_size, boost::uint32_t expected_crc);

    static boost::uint8_t ReadByte(std::istream& input)
    {
        char character(0);
        input.read(&character, sizeof(character));

        return static_cast<boost::uint8_t>(character);
    }

    static boost::uint32_t ReadValue (std::istream& input)
    {
        boost::uint32_t x = ReadByte(input);

        x += ReadByte(input)<<8;
        x += ReadByte(input)<<16;
        x += ReadByte(input)<<24;
        return x;
    }
};

bool Decompressor::Decompress(const char* path)
{
    std::ifstream compress_file(path, std::ios_base::binary);

    if (!compress_file)
    {
        return false;
    }

    compress_file.seekg(0, std::ios_base::end);
    size_t file_size = compress_file.tellg();

    if (file_size <= 10+8)
    {
        return false;
    }

    compress_file.seekg(-8, std::ios_base::end);
    if (!compress_file)
    {
        return false;
    }

    //read CRC & raw data size from the last two dwords
    boost::uint32_t expected_crc = ReadValue(compress_file);
    raw_data_size_ = ReadValue(compress_file);

    if (raw_data_size_ <= 0) {
        return false;
    }

    if (max_raw_data_size_ > 0 && raw_data_size_ > max_raw_data_size_)
    {
        return false;
    }

    //skip the header
    compress_file.seekg(10, std::ios_base::beg);

    if (!compress_file)
    {
        return false;
    }

    size_t compress_data_size = file_size-10-8;
    char* buffer = new char[compress_data_size];
    compress_file.read(buffer, compress_data_size);

    delete[] raw_data_buffer_;
    raw_data_buffer_ = new char[raw_data_size_];

    bool uncompression_succeeded = DoDecompress(buffer, compress_data_size, expected_crc);

    delete[] buffer;

    return uncompression_succeeded;
}

bool Decompressor::DoDecompress(char* buffer, boost::uint32_t compress_data_size, boost::uint32_t expected_crc)
{
    z_stream stream;
    stream.next_in = (Bytef*)buffer;
    stream.avail_in = compress_data_size;

    stream.next_out = (Bytef*)raw_data_buffer_;
    stream.avail_out = raw_data_size_;

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;

    int err = inflateInit2(&stream, -MAX_WBITS);

    if (err != Z_OK) 
    {
        return false;
    }

    err = inflate(&stream, Z_FINISH);
    if (err != Z_STREAM_END) 
    {
        inflateEnd(&stream);
        return false;
    }

    bool validation_passed = false;
    if(stream.total_out == raw_data_size_)
    {
        boost::uint32_t crc = crc32(0L, Z_NULL, 0);
        crc = crc32(crc, reinterpret_cast<Bytef*>(raw_data_buffer_), raw_data_size_);
        if (crc == expected_crc)
        {
            validation_passed = true;
        }
    }

    err = inflateEnd(&stream);
    return err == Z_OK && validation_passed;
}

using namespace statistic;

bool Deserialize(const char* raw_data, int raw_data_size, std::vector<boost::shared_ptr<StatisticsData> >& statistics_datas)
{
    util::archive::ArchiveBuffer<> archive_buffer(const_cast<char*>(raw_data), raw_data_size, raw_data_size);
    util::archive::LittleEndianBinaryIArchive<> input(archive_buffer);

    PeerStatisticsInfo peer_statistics_info;
    input >> peer_statistics_info;

    int offset = raw_data_size - archive_buffer.size();

    for(size_t i = 0; i < peer_statistics_info.collected_statistics_size_in_seconds_; ++i)
    {
        boost::shared_ptr<StatisticsData> statistics_data(new StatisticsData());
        int incremental_bytes = statistics_data->Deserialize(
            peer_statistics_info.version_, 
            const_cast<boost::uint8_t*>(reinterpret_cast<const boost::uint8_t*>(raw_data + offset)), 
            raw_data_size - offset);
        
        if (incremental_bytes < 0)
        {
            return false;
        }

        statistics_datas.push_back(statistics_data);
        offset += incremental_bytes;
    }

    return offset == raw_data_size;
}

int main(int argc, char* argv[])
{
    char* posted_data_path;
    char* output_data_path;

#ifdef BOOST_WINDOWS_API
    if (argc == 3)
    {
        posted_data_path = argv[1];
        output_data_path = argv[2];
    }
    else
    {
        std::cout << "缺少卡顿日志的路径和(或)输出文件的路径" << std::endl;
        return -1;
    }
#else
    int oc;
    while ((oc = getopt(argc, argv, "i:o:")) != -1)
    {
        switch(oc)
        {
        case 'i':
            posted_data_path = optarg;
            break;
        case 'o':
            output_data_path = optarg;
            break;
        }
    }
#endif
    //const string posted_data_path = "C:\\posted_data.gz";

    Decompressor compressor(1000*1024);

    std::vector<boost::shared_ptr<StatisticsData> > statistics_datas;

    if (compressor.Decompress(posted_data_path))
    {
        std::cout<<"decompression completed successfully."<<std::endl;

        int raw_data_size = compressor.GetRawDataSize();
        const char* raw_data = compressor.GetRawData();

        if (Deserialize(raw_data, raw_data_size, statistics_datas))
        {
            std::cout<<"deserialization completed successfully."<<std::endl;
            XmlCreater xml_creater;
            xml_creater.CreateXmlFile(statistics_datas, output_data_path);

            //CsvCreater csv_creater;
            //csv_creater.SimpleWriteToCSVFile(statistics_datas, output_data_path);
            return 0;
        }
        else
        {
            std::cout<<"deserialization failed."<<std::endl;
            return -1;
        }

        //boost::interprocess::basic_vectorstream<vector<char> > data1;
        //boost::interprocess::basic_vectorstream<vector<char> > data2;
        //GZipCompressor gz_compressor;
        //bool succeeded1 = gz_compressor.Compress(reinterpret_cast<boost::uint8_t*>(const_cast<char*>(raw_data)), raw_data_size, data1);

        //LzCompressor lz_compressor;
        //bool succeeded2 = lz_compressor.Compress(reinterpret_cast<boost::uint8_t*>(const_cast<char*>(raw_data)), raw_data_size, data2);

        //if (succeeded1 && succeeded2)
        //{
        //    std::vector<char> v1 = data1.vector();
        //    std::vector<char> v2 = data2.vector();

        //    std::cout<<"GZip compressed size:"<<v1.size()<<std::endl;
        //    std::cout<<"LZ compressed size:"<<v2.size()<<std::endl;;
        //}
    }

    return -1;
}

//class LzCompressor
//    :public ICompressor
//{
//public:
//    LzCompressor(){}
//
//    ~LzCompressor()
//    {
//
//    }
//
//    bool Compress(uint8_t* raw_data, int raw_data_size, std::ostream& result)
//    {
//        char* compressed_data = new char[2*raw_data_size + 200];
//        int compressed_size = framework::string::compress(raw_data, raw_data_size, compressed_data);
//
//        bool succeeded = (compressed_size > 0 && compressed_size < 2*raw_data_size + 200);
//        if (succeeded)
//        {
//            std::copy(compressed_data, compressed_data + compressed_size, std::ostreambuf_iterator<char>(result));
//        }
//
//        delete[] compressed_data;
//
//        return succeeded;
//    }
//};
