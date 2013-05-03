// FileSink.h

#ifndef _PPBOX_DATA_SINK_FILE_SINK_H_
#define _PPBOX_DATA_SINK_FILE_SINK_H_

#include "ppbox/data/base/SinkBase.h"

#include <fstream>

namespace ppbox
{
    namespace data
    {

        class FileSink
            : public SinkBase
        {
        public:
            FileSink(
                boost::asio::io_service & io_svc);

            virtual ~FileSink();

        public:
            virtual boost::system::error_code open(
                framework::string::Url const & url,
                boost::system::error_code & ec);

            virtual bool is_open(
                boost::system::error_code & ec);

            virtual boost::system::error_code close(
                boost::system::error_code & ec);

        public:
            std::ofstream & file_stream()
            {
                return file_;
            }

        private:
            // implement util::stream::Sink
            virtual std::size_t private_write_some(
                boost::asio::const_buffer const & buffer,
                boost::system::error_code & ec);

        private:
            std::ofstream file_;
            bool is_open_;
        };

        PPBOX_REGISTER_SINK("file", FileSink);

    } // namespace demux
} // namespace ppbox

#endif // _PPBOX_DATA_SINK_FILE_SINK_H_
