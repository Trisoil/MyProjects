// FileSink.cpp

#include "tools/transfer/Common.h"
#include "tools/transfer/FileSink.h"

#include <ppbox/demux/DemuxerBase.h>

#include <framework/system/ErrorCode.h>

#include <boost/asio/write.hpp>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("FileSink", 0)

namespace tools
{
    namespace transfer
    {

        FileSink::FileSink(
            boost::asio::io_service & io_svc, 
            std::string const & name, 
            boost::system::error_code & ec)
            : file_(name.c_str(), std::ios::binary)
            , os_(io_svc, file_)
        {
            if (!file_) {
                ec = framework::system::last_system_error();
            } else {
                ec.clear();
            }
        }

        FileSink::~FileSink()
        {
        }
        //工作线程调用
        size_t FileSink::write(
			boost::posix_time::ptime const & time_send,
            ppbox::demux::Sample& sample,
			boost::system::error_code& ec)
        {
			ec.clear();
            return boost::asio::write(os_, sample.data, boost::asio::transfer_all(), ec);
        }

    } // namespace transfer
} // namespace tools
