// AliveProxy.cpp

#include "ppbox/alive/Common.h"
#include "ppbox/alive/AliveProxy.h"

#include <ppbox/certify/CertifyError.h>
#include <ppbox/certify/KeepaliveCommand.h>
#ifndef PPBOX_DISABLE_DAC
#include <ppbox/dac/DacModule.h>
#include <ppbox/dac/DacInfoAuth.h>
using namespace ppbox::dac;
#endif
using namespace ppbox::certify;
using namespace ppbox::certify::error;

#include <util/protocol/http/HttpProxy.h>
using namespace util::protocol;

#include <framework/network/NetName.h>
#include <framework/network/TcpSocket.hpp>
#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/logger/Section.h>

#include <boost/asio/write.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/bind.hpp>
using namespace boost::system;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.alive.AliveProxy", framework::logger::Debug);

namespace ppbox
{
    namespace alive
    {

        class AliveProxy::ProxyManager
            : public framework::network::ServerManager<AliveProxy::AliveSession, AliveProxy::ProxyManager>
        {
        public:
            ProxyManager(
                boost::asio::io_service & io_svc, 
                AliveProxy & status_proxy)
                : framework::network::ServerManager<AliveProxy::AliveSession, AliveProxy::ProxyManager>(io_svc)
                , status_proxy_(status_proxy)
                , is_started_(false)
            {
            }

        public:
            AliveProxy & module()
            {
                return status_proxy_;
            }

            void start_auth(
                KeepaliveRequest const & keepalive_request)
            {
                is_started_ = status_proxy_.start_auth(keepalive_request);
            }

            void add_session(
                AliveSession * session)
            {
                lst_session_.push_back(session);
            }

            void remove_session(
                AliveSession * session)
            {
                std::list<AliveProxy::AliveSession *>::iterator iter = 
                    std::find(lst_session_.begin(), lst_session_.end(), session);
                if (iter != lst_session_.end()) {
                    lst_session_.erase(iter);
                }
            }

            void send_auth_status(
                KeepaliveResponse const & keepalive_response);

            void stop()
            {
                framework::network::ServerManager<AliveProxy::AliveSession, AliveProxy::ProxyManager>::stop();

                stop_auth();
            }

            void stop_auth();

            boost::asio::streambuf & get_buf()
            {
                return head_buf_;
            }

            bool is_auth_started() const
            {
                return is_started_;
            }

        private:
            std::list<AliveProxy::AliveSession*> lst_session_;
            AliveProxy & status_proxy_;
            boost::asio::streambuf head_buf_;

            bool is_started_;
        };

        class AliveProxy::AliveSession
            : public util::protocol::HttpProxy
        {
        public:
            AliveSession(
                ProxyManager & mgr)
                : util::protocol::HttpProxy(mgr.io_svc())
                , mgr_(mgr)
            {
                mgr_.add_session(this);
            }

            ~AliveSession()
            {
                mgr_.remove_session(this);
            }

        public:
            void send_response(
                boost::asio::streambuf & buf)
            {
                if (!response_.empty()) {
                    error_code ec;
                    boost::asio::write(
                        get_client_data_stream(), 
                        buf.data(), 
                        boost::asio::transfer_all(), 
                        ec);

                    if (ec) {
                        response_type resp;
                        resp.swap(response_);
                        resp(ec, std::pair<size_t, size_t>(0, 0));
                    }
                }
            }

            void stop_auth()
            {
                error_code ec;
                if (!response_.empty()) {
                    response_type resp;
                    resp.swap(response_);
                    resp(ec, std::pair<size_t, size_t>(0, 0));
                }
            }

        public:
            virtual bool on_receive_request_head(
                HttpRequestHead & request_head)
            {
                return false;
            }

            virtual void on_receive_request_data(
                boost::asio::streambuf & request_data)
            {
                IHttpArchive ia(request_data);

                KeepaliveRequest keepalive_request;
                ia >> keepalive_request;

                if (!ia) {
                    //ec = error::request_error;
                } else {
                    mgr_.start_auth(keepalive_request);
                }
            }

            virtual void on_error(
                boost::system::error_code const & ec)
            {
                LOG_WARN("[on_error] ec:" << ec.message());
                if (ec == boost::asio::error::address_in_use)
                    mgr_.module().get_daemon().post_stop();
            }

            //收到请求报文体
            //virtual void local_process(
            //    local_process_response_type const & resp)
            //{
            //    error_code ec;

            //    IHttpArchive ia(request_data());

            //    KeepaliveRequest keepalive_request;
            //    ia >> keepalive_request;

            //    if (!ia) {
            //        ec = error::request_error;
            //    } else {
            //        mgr_.start_auth(keepalive_request.gid, keepalive_request.pid, keepalive_request.auth, keepalive_request.distance_time);
            //    }

            //    resp(ec);
            //}

            virtual void transfer_response_data(
                response_type const & resp)
            {
                error_code ec;
                boost::asio::write(
                    get_client_data_stream(), 
                    mgr_.get_buf().data(), 
                    boost::asio::transfer_all(), 
                    ec);

                if (ec) {
                    resp(ec, std::pair<size_t, size_t>(0, 0));
                } else {
                    response_ = resp;
                }

                if (!mgr_.is_auth_started()) {
                    stop_auth();
                }
            }

        private:
            ProxyManager & mgr_;

            response_type response_;
        };

        void AliveProxy::ProxyManager::stop_auth()
        {
            is_started_ = false;

            error_code ec;
            std::list<AliveProxy::AliveSession *>::iterator iter = lst_session_.begin();
            for ( ; iter != lst_session_.end(); ) {
                AliveProxy::AliveSession * session = *iter++;
                if (session) {
                    session->stop_auth();
                }
            }
        }

        void AliveProxy::ProxyManager::send_auth_status(
            KeepaliveResponse const & keepalive_response)
        {
            head_buf_.reset();
            OHttpArchive oa(head_buf_);
            oa << keepalive_response;

            oa << std::string("\r\n\r\n");

            std::list<AliveProxy::AliveSession *>::iterator iter = lst_session_.begin();
            for ( ; iter != lst_session_.end(); ) {
                AliveProxy::AliveSession * session = *iter++;
                if (session) {
                    session->send_response(head_buf_);
                }
            }
        }

        AliveProxy::AliveProxy(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<AliveProxy>(daemon, "AliveProxy")
            , portMgr_(util::daemon::use_module<ppbox::common::PortManager>(daemon))
            , addr_("(v4)127.0.0.1:9002+")
            , mgr_(NULL)
            , auth_(io_svc())
            , client_(io_svc())
            , distance_time_(0)
            , timer_(new clock_timer(io_svc()))
            , auth_status_(AuthStatus::stopped)
        {
            config().register_module("AliveProxy")
                << CONFIG_PARAM_NAME_RDWR("addr", addr_);
            mgr_ = new ProxyManager(io_svc(), *this);
        }

        AliveProxy::~AliveProxy()
        {
            delete mgr_;
            mgr_ = NULL;

            delete timer_;
            timer_ = NULL;
        }

        error_code AliveProxy::startup()
        {
            boost::system::error_code ec;
            mgr_->start(addr_, ec);

            if(!ec)
                portMgr_.set_port(ppbox::common::alive,addr_.port());
            return ec;
        }

        void AliveProxy::shutdown()
        {
            error_code ec;

            auth_.cancel(ec);

            timer_->cancel(ec);
            mgr_->stop();
            client_.stop();

            auth_status_ = AuthStatus::stopped;
        }

        bool AliveProxy::start_auth(
            KeepaliveRequest const & keepalive_request)
        {
            if (auth_status_ == AuthStatus::authing 
                || auth_status_ == AuthStatus::authed) {
                    return true;
            }

            ppbox::common::CommonModule & common = 
                util::daemon::use_module<ppbox::common::CommonModule>(get_daemon());
#ifndef PPBOX_DISABLE_DAC
            DacModule & dac = util::daemon::use_module<DacModule>(get_daemon());
            dac.set_auth_code(
                keepalive_request.gid.c_str(), 
                keepalive_request.pid.c_str(), 
                keepalive_request.auth.c_str());
#endif
            AuthRequest request;
            request.gid = keepalive_request.gid;
            request.pid = keepalive_request.pid;
            request.auth = keepalive_request.auth;
            request.hid = common.environment().hid;
            request.mid = common.environment().mid;
            request.aid = common.environment().aid;
            request.version = common.environment().version.to_string();
            auth_.async_certify(request, boost::bind(&AliveProxy::auth_callback, this, _1, _2));

            auth_status_ = AuthStatus::authing;

            distance_time_ = keepalive_request.distance_time;

            timer_->expires_from_now(Duration::seconds(distance_time_));
            timer_->async_wait(boost::bind(&AliveProxy::on_timer, this, _1));

            send_response(error_code());

            return true;
        }

        void AliveProxy::on_timer(
            error_code const & ec)
        {
            LOG_SECTION();

            if (ec)
                return;

            if (auth_status_ == AuthStatus::authing 
                || auth_status_ == AuthStatus::authed) {
                    send_response(error_code());

                    timer_->expires_from_now(Duration::seconds(distance_time_));
                    timer_->async_wait(boost::bind(&AliveProxy::on_timer, this, _1));
            }
        }

        void AliveProxy::auth_callback(
            AuthResponse const & response, 
            error_code const & ec)
        {
#ifndef PPBOX_DISABLE_DAC
            DacModule & dac = util::daemon::use_module<DacModule>(get_daemon());
            dac.submit(DacAuthInfo(auth_.get_stat()));
#endif
            if (!ec)
                auth_status_ = AuthStatus::authed;
            else if (ec == ppbox::certify::error::time_out)
                auth_status_ = AuthStatus::time_out;
            else
                auth_status_ = AuthStatus::failed;

            if (vod_playlink_key_.empty() || live_playlink_key_.empty()) {
                vod_playlink_key_ = response.vod_playlink_key;
                live_playlink_key_ = response.live_playlink_key;
            }

            send_response(ec);

            if (ec) {
                LOG_DEBUG("[auth_callback] ec: " << ec.message());
#ifndef PPBOX_DISABLE_DAC
                dac.submit(DacTerminateInfo(ec));
#endif
                error_code ec1;
                timer_->cancel(ec1);

                mgr_->stop_auth();
            } else {
                if (response.mutex_enable) {
                    ppbox::common::CommonModule & common = 
                        util::daemon::use_module<ppbox::common::CommonModule>(get_daemon());

                    client_.set_mutex_info(
                        common.environment().get_bid(), 
                        response.session_id, 
                        response.server_time, 
                        response.mutex_servers, 
                        response.mutex_encrypt, 
                        boost::bind(&AliveProxy::keepalive_callback, this, _1));

                    client_.start();
                }
            }
        }

        void AliveProxy::keepalive_callback(
            error_code const & ec)
        {
#ifndef PPBOX_DISABLE_DAC
            DacModule & dac = util::daemon::use_module<DacModule>(get_daemon());
#endif
            if (ec) {
                if (ec == mutex::client::error::time_out || 
                    ec == mutex::client::error::use_all_mutex_server) {
                        auth_status_ = AuthStatus::time_out;
                } else {
                    auth_status_ = AuthStatus::failed;
                }

                LOG_DEBUG("[keepalive_callback] ec: " << ec.message());

                send_response(ec);
#ifndef PPBOX_DISABLE_DAC
                dac.submit(DacTerminateInfo(ec));
#endif
                error_code ec1;
                timer_->cancel(ec1);

                mgr_->stop_auth();

                client_.stop();
            } else {
#ifndef PPBOX_DISABLE_DAC
                dac.submit(DacMutexInfo(client_.protocol_type()));
#endif
            }
        }

        void AliveProxy::send_response(
            error_code const & ec)
        {
            LOG_TRACE("[send_response] status: " << AuthStatus::status_str(auth_status_) 
                << " ec: " << ec.message());

            KeepaliveResponse keepalive_response;
            keepalive_response.vod_playlink_key = vod_playlink_key_;
            keepalive_response.live_playlink_key = live_playlink_key_;
            keepalive_response.status = auth_status_;
            keepalive_response.error_value = ec;
            mgr_->send_auth_status(keepalive_response);
        }

    } // namespace alive
} // namespace ppbox
