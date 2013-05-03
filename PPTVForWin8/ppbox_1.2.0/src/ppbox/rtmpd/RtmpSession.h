// RtmpSession.h

#ifndef _PPBOX_RTMPD_RTMP_SESSION_H_
#define _PPBOX_RTMPD_RTMP_SESSION_H_

#include <util/protocol/rtmp/RtmpServer.h>

#include <boost/shared_ptr.hpp>

namespace ppbox
{
    namespace rtmpd
    {

        class RtmpDispatcher;
        class RtmpdModule;

        class RtmpSession
            : public util::protocol::RtmpServer
             //,public AsyncCallback
        {
        public:
            RtmpSession(
                RtmpdModule & mgr);

            ~RtmpSession();

        public:
            virtual void local_process(
                response_type const & resp);

            virtual void on_error(
                boost::system::error_code const & ec);

            virtual void on_finish();

            virtual void post_process(
                response_type const & resp);

            void on_play(
                boost::system::error_code const & ec);

        private:
            RtmpdModule & mgr_;
            std::string cmd_;
            std::string path_;
            boost::uint32_t session_id_;
            RtmpDispatcher * dispatcher_;
            boost::uint32_t file_id_;
            boost::uint32_t play_count_;
            boost::uint32_t play_incarnation_;
            response_type post_resp_;
        };

    } // namespace rtmpd
} // namespace ppbox

#endif // _PPBOX_RTMPD_RTMP_SESSION_H_