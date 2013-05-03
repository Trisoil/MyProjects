// PipeSource.h

#include <ppbox/data/Common.h>
#include <ppbox/data/source/PipeSource.h>

#include <framework/system/ErrorCode.h>

namespace ppbox
{
    namespace data
    {

        PipeSource::PipeSource(
            boost::asio::io_service & io_svc)
            : SourceBase(io_svc)
            , descriptor_(io_svc)
            , is_open_(false)
        {
        }

        PipeSource::~PipeSource()
        {
        }

        boost::system::error_code PipeSource::open(
            framework::string::Url const & url,
            boost::uint64_t beg, 
            boost::uint64_t end, 
            boost::system::error_code & ec)
        {
            if (is_open_)
                descriptor_.close(ec);
            if (beg != 0 && end != boost::uint64_t(-1)) {
                ec = framework::system::logic_error::unknown_error;
                return ec;
            }
            native_descriptor nd = framework::string::parse<native_descriptor>(url.path().substr(1));
            descriptor_.assign(nd);
            is_open_ = descriptor_.is_open();
            if (!is_open_) {
                ec = framework::system::last_system_error();
                if (!ec) {
                    ec = framework::system::logic_error::unknown_error;
                }
            }
            return ec;
        }

        bool PipeSource::is_open(
            boost::system::error_code & ec)
        {
            return is_open_;
        }

        boost::system::error_code PipeSource::close(
            boost::system::error_code & ec)
        {
            descriptor_.close();
            is_open_ = false;
            return ec = boost::system::error_code();
        }

        std::size_t PipeSource::private_read_some(
            util::stream::StreamMutableBuffers const & buffers,
            boost::system::error_code & ec)
        {
            return descriptor_.read_some(buffers, ec);
        }

        boost::system::error_code PipeSource::set_non_block(
            bool non_block, 
            boost::system::error_code & ec)
        {
#ifndef BOOST_WINDOWS_API
            boost::asio::posix::descriptor_base::non_blocking_io cmd(non_block);
            return descriptor_.io_control(cmd, ec);
#else
            return ec = boost::system::error_code();
#endif    
        }

        bool PipeSource::continuable(
            boost::system::error_code const & ec)
        {
            return ec == boost::asio::error::would_block;
        }

    } // namespace data
} // namespace ppbox
