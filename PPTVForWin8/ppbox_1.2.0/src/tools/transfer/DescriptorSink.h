#ifndef _TOOLS_TRANSFER_DESCRIPTOR_SINK_H_
#define _TOOLS_TRANSFER_DESCRIPTOR_SINK_H_

#include <ppbox/mux/tool/Sink.h>

#ifndef BOOST_WINDOWS_API
#  include <boost/asio/posix/stream_descriptor.hpp>
typedef boost::asio::posix::stream_descriptor descriptor;
#else
#  include <boost/asio/windows/stream_handle.hpp>
typedef boost::asio::windows::stream_handle descriptor;
#endif

namespace tools
{

    namespace transfer
    {

        class DescriptorSink
            : public ppbox::mux::Sink
        {
        public:
            DescriptorSink(
                boost::asio::io_service & io_svc, 
                std::string const & name, 
                 boost::system::error_code & ec);

            virtual ~DescriptorSink();

        private:
            size_t write(
                    boost::posix_time::ptime const & time_send,
					ppbox::demux::Sample & sample,
					boost::system::error_code & ec);

        private:
            descriptor descriptor_;
        };

    } // namespace transfer
} // namespace tools

#endif 
