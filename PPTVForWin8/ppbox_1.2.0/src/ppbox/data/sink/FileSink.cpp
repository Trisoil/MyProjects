// FileSink.h

#include <ppbox/data/Common.h>
#include <ppbox/data/sink/FileSink.h>

#include <framework/system/ErrorCode.h>

#include <boost/filesystem/path.hpp>

namespace ppbox
{
    namespace data
    {

        FileSink::FileSink(
            boost::asio::io_service & io_svc)
            : SinkBase(io_svc)
            , is_open_(false)
        {
        }

        FileSink::~FileSink()
        {
        }

        boost::system::error_code FileSink::open(
            framework::string::Url const & url,
            boost::system::error_code & ec)
        {
            if (is_open_)
                file_.close();
            boost::filesystem::path ph = url.path();
            file_.open(ph.file_string().c_str(), std::ios::binary | std::ios::out);
            is_open_ = file_.is_open();
            if (!is_open_) {
                ec = framework::system::last_system_error();
                if (!ec) {
                    ec = framework::system::logic_error::unknown_error;
                }
            }
            return ec;
        }

        bool FileSink::is_open(
            boost::system::error_code & ec)
        {
            return is_open_;
        }

        boost::system::error_code FileSink::close(
            boost::system::error_code & ec)
        {
            file_.close();
            is_open_ = false;
            return ec = boost::system::error_code();
        }

        std::size_t FileSink::private_write_some(
            boost::asio::const_buffer const & buffer,
            boost::system::error_code & ec)
        {
            char const * buf_ptr = boost::asio::buffer_cast<char const *>(buffer);
            size_t buf_size = boost::asio::buffer_size(buffer);
            file_.write(buf_ptr, buf_size);
            if (file_) {
                ec.clear();
                return buf_size;
            } else {
                ec = framework::system::last_system_error();
                if (!ec) {
                    ec = framework::system::logic_error::unknown_error;
                }
                return 0;
            }
        }

    } // namespace data
} // namespace ppbox
