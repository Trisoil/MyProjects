// RtpDispatcher.h

#ifndef _PPBOX_RTMPD_RTMP_DISPATCHER_H_
#define _PPBOX_RTMPD_RTMP_DISPATCHER_H_

#include <ppbox/dispatch/CustomDispatcher.h>

namespace util
{
    namespace protocol
    {
        class RtmpSocket;
    }
}

namespace ppbox
{
    namespace rtmpd
    {

        class RtmpSink;

        class RtmpDispatcher 
            : public ppbox::dispatch::CustomDispatcher
        {
        public:

            RtmpDispatcher(
                ppbox::dispatch::DispatcherBase & dispatcher);

            ~RtmpDispatcher();

        public:
            void async_open_play(
                util::protocol::RtmpSocket & socket, 
                framework::string::Url & url, 
                ppbox::dispatch::response_t const & seek_resp, 
                ppbox::dispatch::response_t  const & resp);

            bool teardown(
                boost::system::error_code & ec);

        public:
            ppbox::dispatch::DispatcherBase & get_dispatcher()
            {
                return dispatcher_;
            }

        private:
            void handle_open(
                util::protocol::RtmpSocket & socket, 
                ppbox::dispatch::response_t const & seek_resp, 
                ppbox::dispatch::response_t const & resp, 
                boost::system::error_code ec);

        private:
            ppbox::data::MediaInfo media_info_;
            RtmpSink * sink_;
        };

    } // namespace rtmpd
} // namespace ppbox

#endif // _PPBOX_RTMPD_RTP_DISPATCHER_H_