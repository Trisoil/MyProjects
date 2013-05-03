// DescriptorSink.cpp

#include "tools/transfer/Common.h"
#include "tools/transfer/DescriptorSink.h"

#include <ppbox/demux/DemuxerBase.h>

#include <framework/string/Parse.h>
using namespace framework::string;

#include <boost/asio/write.hpp>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("DescriptorSink", 0)

namespace tools
{
    namespace transfer
    {

        DescriptorSink::DescriptorSink(
            boost::asio::io_service & io_svc, 
            std::string const & name, 
            boost::system::error_code & ec)
            : descriptor_(io_svc)
        {
            descriptor::native_type nd;
            ec = parse2(name, nd);
            if (!ec)
                descriptor_.assign(nd, ec);
        }

        DescriptorSink::~DescriptorSink()
        {
        }

        //工作线程调用
        size_t DescriptorSink::write(
            boost::posix_time::ptime const & time_send,
			ppbox::demux::Sample& sample,
			boost::system::error_code & ec)
        {
			ec.clear();
            return boost::asio::write(descriptor_, sample.data, boost::asio::transfer_all(), ec);
        }

    } // namespace transfer
} // namespace tools
