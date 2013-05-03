// Certifier.cpp

#include "ppbox/certify/Common.h"
#include "ppbox/certify/Certifier.h"
#include "ppbox/certify/CertifyUserModule.h"
#include "ppbox/certify/KeepaliveCommand.h"
using namespace ppbox::certify::error;

#include <ppbox/alive/Name.h>

#ifdef PPBOX_CONTAIN_PPBOX_ALIVE
#include <ppbox/alive/AliveProxy.h>
using namespace ppbox::alive;
#else
using namespace framework::process;
#endif

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
using namespace framework::system;

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
using namespace boost::system;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.certify.Certifier", framework::logger::Debug);

namespace ppbox
{
    namespace certify
    {

        static const char PPBOX_VOD_KEY[] = "kioe257ds";
        static const char PPBOX_LIVE_KEY[] = "pplive";

        Certifier::Certifier(
            util::daemon::Daemon & daemon)
            : ppbox::common::CommonModuleBase<Certifier>(daemon, "Certifier")
            , portMgr_(util::daemon::use_module<ppbox::common::PortManager>(daemon))
            , auth_status_(AuthStatus::stopped)
            , last_error_(error::not_started)
            , client_(io_svc())
            , vod_playlink_key_(PPBOX_VOD_KEY)
            , live_playlink_key_(PPBOX_LIVE_KEY)
        {
#ifdef PPBOX_CONTAIN_PPBOX_ALIVE
            util::daemon::use_module<ppbox::alive::AliveProxy>(daemon);
#endif    
        }

        error_code Certifier::startup()
        {
            error_code ec;

            auth_status_ = AuthStatus::authing;
            last_error_.clear();

            LOG_INFO("[startup]");
            
#ifndef PPBOX_CONTAIN_PPBOX_ALIVE
            boost::filesystem::path cmd_file(ppbox::alive::name_string());
            Process::CreateParamter param;
            param.wait = true;
            process_.open(cmd_file, param, ec);
#endif
            if (!ec) 
            {
                boost::uint16_t port = 0;
                portMgr_.get_port(ppbox::common::alive,port);
                client_.set_port(port);
                LOG_INFO("[startup] ok,alive port:"<<port);
                send_keepalive();
            } else {
                LOG_WARN("[startup] ec = " << ec.message());
            }

            return ec;
        }

        void Certifier::shutdown()
        {
            if (auth_status_ == AuthStatus::authing
                || auth_status_ == AuthStatus::authed) {
                    client_.cancel();
            }

            auth_status_ = AuthStatus::stopped;
            last_error_ = error::not_started;
        }

        void Certifier::set_auth_code(
            char const * gid, 
            char const * pid, 
            char const * auth)
        {
            gid_ = gid;
            pid_ = pid;
            auth_ = auth;
        }

        error_code Certifier::certify_add_module(
            CertifyUserModule & module)
        {
            modules_.push_back(&module);

            if (auth_status_ == AuthStatus::authed) {
                module.on_certify(auth_status_, last_error_);
            }

            return error_code();
        }

        error_code Certifier::certify_del_module(
            CertifyUserModule & module)
        {
            std::vector<CertifyUserModule *>::iterator iter = modules_.begin();
            for( ; iter != modules_.end(); ++iter) {
                if (*iter == &module) {
                    modules_.erase(iter);
                    return error_code();
                }
            }

            return logic_error::item_not_exist;
        }

        void Certifier::send_keepalive()
        {
            LOG_INFO("certify: start");

            KeepaliveRequest request;
            request.gid = gid_;
            request.pid = pid_;
            request.auth = auth_;
            request.distance_time = TIMEOUT - 5;
            client_.async_keepalive(request, boost::bind(&Certifier::keepalive_callback, this, _1));
        }

        // 检查认证状态，同时也会触发认证重启
        bool Certifier::is_certified(
            error_code & ec)
        {
            //暂时不做
            //if (!is_alive()) {
            //    restart();
            //}

            AuthStatus::Enum status = auth_status_;
            ec = last_error_;

            if (auth_status_ == AuthStatus::time_out) {
                boost::mutex::scoped_lock lock(mutex_);
                if (auth_status_ == AuthStatus::time_out) {
                    status = auth_status_ = AuthStatus::authing;
                    last_error_.clear();
                    ec.clear();
                    send_keepalive();
                }
            }

            if (status == AuthStatus::stopped 
                || status == AuthStatus::failed
                || status == AuthStatus::authed) {
                    if (!ec) {
                        LOG_INFO("certify: success");
                    } else {
                        LOG_WARN("certify: failure");
                    }
                    return true;
            } else if (status == AuthStatus::authing) {
                return true;
            } else {
                assert(0);
                return false;
            }
        }

        void Certifier::keepalive_callback(
            KeepaliveResponse const & response)
        {
            if (auth_status_ == response.status ||
                auth_status_ == AuthStatus::stopped)
                return;

            AuthStatus::Enum notify_status = response.status;
            if (auth_status_ == AuthStatus::authing && 
                notify_status == AuthStatus::time_out) {
                    //可以恢复的
                    notify_status = AuthStatus::time_out;
            }

            auth_status_ = response.status;
            last_error_  = response.error_value;

            if (notify_status != AuthStatus::authing) {
                if (notify_status == AuthStatus::authed) {
                    LOG_INFO("certify: success");

                    vod_playlink_key_ = response.vod_playlink_key;
                    live_playlink_key_ = response.live_playlink_key;
                } else {
                    LOG_WARN("certify: failure");
                    LOG_WARN("[keepalive_callback] : status = " << 
                        AuthStatus::status_str(auth_status_) << ", ec = " << last_error_.message());
                }

                std::vector<CertifyUserModule *>::iterator iter = modules_.begin();
                for ( ; iter != modules_.end(); ++iter) {
                    (*iter)->on_certify(notify_status, response.error_value);
                }
            }

            if (response.error_value == error::alive_dead && !is_alive()) {
                restart();
            }
        }

        error_code Certifier::certify_url(
            framework::string::Url const & url, 
            std::string & out_key, 
            error_code & ec)
        {
            // TODO:
            //if (type == CertifyType::vod) {
            //    if (vod_playlink_key_.empty())
            //        ec = no_vod_key;
            //    else
            //        out_key = vod_playlink_key_;
            //} else if (type == CertifyType::live) {
            //    if (live_playlink_key_.empty())
            //        ec = no_live_key;
            //    else
            //        out_key = live_playlink_key_;
            //}

            return ec;
        }

        bool Certifier::is_alive()
        {
            error_code ec;
#ifdef PPBOX_CONTAIN_PPBOX_ALIVE
            return !ec;
#else
            return process_.is_alive(ec);
#endif
        }

        void Certifier::restart()
        {
#ifndef PPBOX_CONTAIN_PPBOX_ALIVE

            error_code ec;
            process_.close(ec);
            LOG_DEBUG("[restart]");
            boost::filesystem::path cmd_file(ppbox::alive::name_string());
            Process::CreateParamter param;
            param.wait = true;
            process_.open(cmd_file, param, ec);

            if (!ec) {
                boost::uint16_t port = 0;
                portMgr_.get_port(ppbox::common::alive,port);
                client_.set_port(port);
                LOG_INFO("[restart] ok,alive port:"<<port);
            } else {
                LOG_WARN("[restart] ec = " << ec.message());
            }
#endif            
        }

    } // namespace certify
} // namespace ppbox
