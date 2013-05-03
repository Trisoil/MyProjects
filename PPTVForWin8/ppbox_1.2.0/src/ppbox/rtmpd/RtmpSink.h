// RtmpSink.h

#ifndef _PPBOX_RTMPD_RTMP_SINK_H_
#define _PPBOX_RTMPD_RTMP_SINK_H_

#include <ppbox/dispatch/Sink.h>

#include <util/protocol/rtmp/RtmpSocket.hpp>

namespace ppbox
{
    namespace rtmpd
    {

        class RtmpSink
            : public ppbox::dispatch::Sink
        {
        public:
            RtmpSink(
                util::protocol::RtmpSocket & socket)
                : socket_(socket)
            {
            }

        public:
            virtual size_t write(
                ppbox::avformat::Sample const & sample, 
                boost::system::error_code & ec)
            {
                return socket_.write_raw_msg(sample.data, ec);
            }

        private:
            util::protocol::RtmpSocket & socket_;
        };

    } // namespace rtmpd
} // namespace ppbox

#endif // _PPBOX_RTMPD_RTMP_SINK_H_
