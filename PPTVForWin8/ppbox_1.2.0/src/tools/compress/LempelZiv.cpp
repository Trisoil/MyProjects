#include "tools/compress/Common.h"
#include "tools/compress/LempelZiv.h"

#include <framework/memory/MemoryPage.h>
#include <framework/string/Compress.h>

#include <boost/cstdint.hpp>
#include <boost/filesystem/operations.hpp>
using namespace boost::system;

#include <iostream>
#include <fstream>

namespace ppbox
{
    namespace compress
    {
        struct Buffer
        {
            Buffer(boost::uint32_t len)
                : size(framework::memory::MemoryPage::align_page(len))
                , buf((char *)malloc(size))
                , used(0)
            {
            }

            ~Buffer()
            {
                if (buf) {
                    free(buf);
                    used = 0;
                }
                size = 0;
            }

            boost::uint32_t size;
            char * buf;
            boost::uint32_t used;
        };

        LempelZiv::LempelZiv(boost::filesystem::path const & in, 
                boost::filesystem::path const & out)
            : in_file_path_(in)
            , out_file_path_(out)
        {
        }

        LempelZiv::~LempelZiv()
        {
        }

        error_code LempelZiv::compress(error_code & ec)
        {
            Buffer buf(500*1024);
            file_to_buffer(in_file_path_.file_string(), buf, ec);
            if (!ec) {
                Buffer obj(500*1024);
                obj.used = framework::string::compress(buf.buf, buf.used, obj.buf);
                buffer_to_file(out_file_path_.file_string(), obj, ec);
            }
            return ec;
        }

        error_code LempelZiv::uncompress(error_code & ec)
        {
            boost::uint32_t size = boost::filesystem::file_size(in_file_path_);
            Buffer buf(size);
            file_to_buffer(in_file_path_.file_string(), buf, ec);
            if (!ec) {
                Buffer obj(size * 100);
                obj.used = framework::string::decompress(buf.buf, buf.used, obj.buf);
                buffer_to_file(out_file_path_.file_string(), obj, ec);
            }
            return ec;
        }

        error_code LempelZiv::file_to_buffer(
            std::string const & filename,
            Buffer & buf,
            error_code & ec)
        {
            if (boost::filesystem::exists(filename)) {
                std::ifstream is;
                is.open(filename.c_str(), std::ios::binary);
                if (is.is_open()) {
                    boost::uint32_t length = file_size(is);
                    if ((buf.size - buf.used) >= length) {
                        is.read(buf.buf+buf.used, length);
                        buf.used += length;
                    } else {
                        ec = error::low_buffer;
                    }
                } else {
                    ec = error::file_open_failed;
                }
                is.close();
            } else {
                ec = error::file_not_found;
            }
            return ec;
        }

        error_code LempelZiv::buffer_to_file(
            std::string const & filename,
            Buffer const & buf,
            error_code & ec)
        {
            std::ofstream os;
            os.open(filename.c_str(), std::ios::binary);
            if (os.is_open()) {
                os.write(buf.buf, buf.used);
            } else {
                ec = error::file_open_failed;
            }
            os.close();
            return ec;
        }

        boost::uint32_t LempelZiv::file_size(std::ifstream & is)
        {
            boost::uint32_t length = 0;
            assert(is.is_open());
            is.seekg (0, std::ios::end);
            length = is.tellg();
            is.seekg (0, std::ios::beg);
            return length;
        }

    }
}