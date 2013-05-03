// MmspSession.h

#ifndef _PPBOX_MMSPD_MMSP_SESSION_H_
#define _PPBOX_MMSPD_MMSP_SESSION_H_

#include <util/protocol/mmsp/MmspServer.h>

#include <boost/shared_ptr.hpp>

namespace ppbox
{
    namespace mmspd
    {

        class MmspDispatcher;
        class MmspdModule;

        class MmspSession
            : public util::protocol::MmspServer
             //,public AsyncCallback
        {
        public:
            MmspSession(
                MmspdModule & mgr);

            ~MmspSession();

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
            MmspdModule & mgr_;
            std::string path_;
            std::string transport_;
            boost::uint32_t session_id_;
            MmspDispatcher * dispatcher_;
            boost::uint32_t file_id_;
            boost::uint32_t play_count_;
            boost::uint32_t play_incarnation_;
            response_type post_resp_;
        };

    } // namespace mmspd
} // namespace ppbox

#endif // _PPBOX_MMSPD_MMSP_SESSION_H_