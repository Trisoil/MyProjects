// FileSource.h

#ifndef _PPBOX_DATA_SOURCE_FILE_SOURCE_H_
#define _PPBOX_DATA_SOURCE_FILE_SOURCE_H_

#include "ppbox/data/base/SourceBase.h"

#include <fstream>

namespace ppbox
{
    namespace data
    {

        class FileSource
            : public SourceBase
        {
        public:
            FileSource(
                boost::asio::io_service & io_svc);

            virtual ~FileSource();

        public:
            virtual boost::system::error_code open(
                framework::string::Url const & url,
                boost::uint64_t beg, 
                boost::uint64_t end, 
                boost::system::error_code & ec);

            virtual bool is_open(
                boost::system::error_code & ec);

            virtual boost::system::error_code close(
                boost::system::error_code & ec);

            virtual boost::uint64_t total(
                boost::system::error_code & ec);

        private:
            // implement util::stream::Source
            virtual std::size_t private_read_some(
                boost::asio::mutable_buffer const & buffer,
                boost::system::error_code & ec);

        private:
            std::ifstream file_;
            bool is_open_;
        };

        PPBOX_REGISTER_SOURCE("file", FileSource);

    } // namespace demux
} // namespace ppbox

#endif // _PPBOX_DATA_SOURCE_FILE_SOURCE_H_
