#ifndef _TOOLS_TRANSFER_FILE_SINK_H_
#define _TOOLS_TRANSFER_FILE_SINK_H_

#include <ppbox/mux/tool/Sink.h>

#include <util/stream/StlStream.h>

#ifndef BOOST_WINDOWS_API
#  include <boost/asio/posix/stream_descriptor.hpp>
typedef boost::asio::posix::stream_descriptor descriptor;
#else
#  include <boost/asio/windows/stream_handle.hpp>
typedef boost::asio::windows::stream_handle descriptor;
#endif

#include <fstream>

namespace tools
{

    namespace transfer
    {

        class FileSink
            : public ppbox::mux::Sink
        {
        public:
            FileSink(
                boost::asio::io_service & io_svc, 
                std::string const & name, 
                boost::system::error_code & ec);

            virtual ~FileSink();

        private:
                size_t write(
                boost::posix_time::ptime const & time_send,
				ppbox::demux::Sample & sample,
			    boost::system::error_code& ec);

        private:
            std::ofstream file_;
            util::stream::StlOStream os_;
        };

    } // namespace transfer
} // namespace tools

#endif 
