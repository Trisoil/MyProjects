// LogSubmitter.h

#ifndef _PPBOX_DAC_LOG_SUBMITTER_H_
#define _PPBOX_DAC_LOG_SUBMITTER_H_

#include <boost/asio/io_service.hpp>
#include <boost/asio/streambuf.hpp>

namespace ppbox
{
    namespace dac
    {
        struct Buffer;

        class LogSubmitter
        {
        public:
            LogSubmitter();

            ~LogSubmitter();

        public:
            bool get_request(
                boost::asio::streambuf & buf,
                std::string const & msg);

        private:
            boost::uint32_t file_size(
                std::ifstream & is);

            bool join_log(
                Buffer & log_buffer);

            bool join_ui_log(
                Buffer & log_buffer, 
                std::string const & msg);

            bool file_to_buffer(
                Buffer & log_buffer, 
                std::string const & filename);

        private:
            std::string ppbox_log_;
            std::string alive_log_;
            std::string vod_log_;
            std::string live_log_;
        };
    }
}

#endif // _PPBOX_DAC_LOG_SUBMITTER_H_
