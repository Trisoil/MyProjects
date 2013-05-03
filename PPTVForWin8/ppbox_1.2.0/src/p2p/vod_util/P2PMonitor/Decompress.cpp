#include "stdafx.h"
#include <iosfwd>
#include <istream>
#include "zlib/zlib.h"
#include "zlib/zutil.h"
//#include "statistic/StatisticsRequest.h"
//#include "statistic/StatisticStructs.h"
//#include "statistic/StatisticsData.h"
#include "statistic/GZipCompressor.h"
#include <boost/interprocess/streams/vectorstream.hpp>
#include <framework/string/compress.h>
#include "Decompress.h"

bool Decompressor::Decompress(const string path)
{
    std::ifstream compress_file(path.c_str(), std::ios_base::binary);

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