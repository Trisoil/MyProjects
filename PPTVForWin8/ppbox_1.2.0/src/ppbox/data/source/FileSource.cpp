// FileSource.h

#include <ppbox/data/Common.h>
#include <ppbox/data/source/FileSource.h>

#include <framework/system/ErrorCode.h>

#include <boost/filesystem/path.hpp>

namespace ppbox
{
    namespace data
    {

        FileSource::FileSource(
            boost::asio::io_service & io_svc)
            : SourceBase(io_svc)
            , is_open_(false)
        {
        }

        FileSource::~FileSource()
        {
        }

        boost::system::error_code FileSource::open(
            framework::string::Url const & url,
            boost::uint64_t beg, 
            boost::uint64_t end, 
            boost::system::error_code & ec)
        {
            if (is_open_)
                file_.close();
            boost::filesystem::path ph = url.path();
            file_.open(ph.file_string().c_str(), std::ios::binary | std::ios::in);
            is_open_ = file_.is_open();
            if (!is_open_) {
                ec = framework::system::last_system_error();
                if (!ec) {
                    ec = framework::system::logic_error::unknown_error;
                }
            } else {
                if (beg > 0)
                    file_.seekg(beg, std::ios_base::beg);
                assert(file_);
                assert((boost::uint64_t)file_.tellg() == beg);
            }
            return ec;
        }

        bool FileSource::is_open(
            boost::system::error_code & ec)
        {
            return is_open_;
        }

        boost::system::error_code FileSource::close(
            boost::system::error_code & ec)
        {
            file_.clear();
            file_.close();
            is_open_ = false;
            return ec = boost::system::error_code();
        }

        boost::uint64_t FileSource::total(
            boost::system::error_code & ec)
        {
            size_t cur;
            boost::uint64_t file_length;
            cur = file_.tellg();
            file_.seekg(0, std::ios_base::end);

            file_length = file_.tellg();
            file_.seekg(cur, std::ios_base::beg);

            return file_length;
        }

        std::size_t FileSource::private_read_some(
            boost::asio::mutable_buffer const & buffer,
            boost::system::error_code & ec)
        {
            char * buf_ptr = boost::asio::buffer_cast<char *>(buffer);
            size_t buf_size = boost::asio::buffer_size(buffer);
            file_.read(buf_ptr, buf_size);
            std::size_t n = file_.gcount();
            if (n > 0) {
                ec.clear();
            } else if (file_.eof()) {
                ec = boost::asio::error::eof;
            } else {
                ec = framework::system::last_system_error();
                if (!ec) {
                    ec = framework::system::logic_error::no_data;
                }
            }
            return n;
        }

    } // namespace data
} // namespace ppbox
