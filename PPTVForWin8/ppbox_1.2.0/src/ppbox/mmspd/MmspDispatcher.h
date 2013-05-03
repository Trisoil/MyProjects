// RtpDispatcher.h

#ifndef _PPBOX_MMSPD_MMSP_DISPATCHER_H_
#define _PPBOX_MMSPD_MMSP_DISPATCHER_H_


#include <ppbox/dispatch/CustomDispatcher.h>
#include <ppbox/dispatch/Sink.h>

#include <boost/asio/ip/tcp.hpp>

#include <boost/asio/streambuf.hpp>

namespace util
{
    namespace protocol
    {
        struct MmspDataStartPlaying;
        struct MmspDataReportOpenFile;
    }
}

namespace ppbox
{
    namespace mmspd
    {  

        class MmspDispatcher 
            : public ppbox::dispatch::CustomDispatcher
        {
        public:

            MmspDispatcher(
                ppbox::dispatch::DispatcherBase & dispatcher);

            ~MmspDispatcher();

        public:
            void async_open(
                framework::string::Url & url, 
                util::protocol::MmspDataReportOpenFile & rsp, 
                ppbox::dispatch::response_t  const & resp);

            bool setup(
                boost::asio::ip::tcp::socket & mmsp_sock, 
                std::string const & transport, 
                boost::system::error_code & ec);

            bool write_header(
                boost::asio::ip::tcp::socket & mmsp_sock, 
                boost::system::error_code & ec);

            void async_play(
                util::protocol::MmspDataStartPlaying & req, 
                ppbox::dispatch::response_t const & seek_resp, 
                ppbox::dispatch::response_t const & resp);

            bool teardown(
                boost::system::error_code & ec);

        public:
            ppbox::dispatch::DispatcherBase & get_dispatcher()
            {
                return dispatcher_;
            }

        private:
            void handle_open(
                util::protocol::MmspDataReportOpenFile & rsp, 
                ppbox::dispatch::response_t  const & resp, 
                boost::system::error_code ec);

            void handle_seek(
                std::string & rtp_info, 
                ppbox::dispatch::SeekRange & range, 
                ppbox::dispatch::response_t const & resp, 
                boost::system::error_code ec);

        private:
            ppbox::data::MediaInfo media_info_;
            ppbox::dispatch::WrapSink * sink_;
        };

    } // namespace mmspd
} // namespace ppbox

#endif // _PPBOX_MMSPD_RTP_DISPATCHER_H_