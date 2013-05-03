// Dac.cpp

#include "ppbox/dac/Common.h"
#include "ppbox/dac/DacModule.h"
#include "ppbox/dac/DacInfo.h"
#include "ppbox/dac/LogSubmitter.h"
#include "ppbox/dac/DacInfoLog.h"
#include "ppbox/dac/DacInfoSystem.h"

#include <ppbox/common/DomainName.h>
#include <ppbox/common/Environment.h>
using namespace ppbox::common;

#include <framework/string/Format.h>
#include <framework/system/ErrorCode.h>
#include <framework/network/NetName.h>
#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/logger/StringRecord.h>
using namespace framework::network;
using namespace framework::string;

#include <util/protocol/pptv/Base64.h>
#include <util/protocol/http/HttpClient.h>
using namespace util::protocol;

#include <boost/bind.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/streambuf.hpp>
using namespace boost::system;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.dac.DacModule", framework::logger::Debug);

#ifndef PPBOX_DNS_DAC
#define PPBOX_DNS_DAC "(tcp)(v4)dac.150hi.com:80"
#endif

#ifndef PPBOX_DNS_LOG
#define PPBOX_DNS_LOG "(tcp)(v4)log.150hi.com:80"
#endif

namespace ppbox
{
    namespace dac
    {

        DEFINE_DOMAIN_NAME(dns_dac_server, PPBOX_DNS_DAC);
        DEFINE_DOMAIN_NAME(dns_log_server, PPBOX_DNS_LOG);

        static const char PPBOX_KEY[] = "kioe257ds";

        static inline std::string addr_host(
            NetName const & addr)
        {
            return addr.host() + ":" + addr.svc();
        }

        struct DacSubmitTask
        {
            InterfaceType::Enum type;
			LogReason::Enum reason;
            util::protocol::HttpRequest request;
        };

        DacModule::DacModule(
            util::daemon::Daemon & daemon)
            : ppbox::common::CommonModuleBase<DacModule>(daemon, "DacModule")
            , timer_(new clock_timer(io_svc()))
            , is_first_open_channel_(false)
            , status_(StatusType::stopped)
            , http_client_(new util::protocol::HttpClient(io_svc()))
            , log_submitter_(new LogSubmitter())
            , live_version_("0.0.0.0")
            , vod_version_("0.0.0.0")
        {
        }

        DacModule::~DacModule()
        {
            if (log_submitter_) {
                delete log_submitter_;
                log_submitter_ = NULL;
            }

            if (http_client_) {
                delete http_client_;
                http_client_ = NULL;
            }

            if (timer_) {
                delete timer_;
                timer_ = NULL;
            }
        }

        error_code DacModule::startup()
        {
            status_ = StatusType::running;

            return error_code();
        }

        void DacModule::shutdown()
        {
            status_ = StatusType::stopped;

            error_code ec;
            timer_->cancel(ec);

            if (senders_.size() > 0) {
                timer_->expires_from_now(Duration::seconds(2));
                timer_->async_wait(boost::bind(&DacModule::submit_timeout, this, _1));
            } else {
                //保证后续的任务请求不通过
                status_ = StatusType::timeout;
            }
        }

        void DacModule::submit_timeout(
            error_code const & ec)
        {
            status_ = StatusType::timeout;

            error_code ec1;
            http_client_->cancel(ec1);
        }

        void DacModule::set_auth_code(
            char const * gid, 
            char const * pid, 
            char const * auth)
        {
            gid_ = gid;
            pid_ = pid;
            auth_ = auth;
        }

        void DacModule::submit_log(
            char const * msg, 
            boost::int32_t size)
        {
            post_log(LogReason::ui_check, std::string(msg, size), error_code());
        }

        static char const * type_str[] = {
            "none", 
            "open", 
            "close", 
            "auth", 
            "mutex", 
            "run", 
            "terminate", 
            "sys",
            "peer",
        };

        static char const * log_reason_str[] = {
            "open too long",
            "open fail",
            "auth mutex fail",
            "proc fail",
            "ui check",
            "user define",
        };

        void DacModule::handle_fetch(
            error_code const & ec)
        {
            DacSubmitTask const & task = senders_.front();

            if (ec) {
				if (task.type == InterfaceType::log_info_type) {
					LOG_WARN("[handle_fetch] error, type: " << type_str[task.type] << " ec: " << ec.message());
				} else {
					LOG_WARN("[handle_fetch] error, log_reason: " << log_reason_str[task.reason] << " ec: " << ec.message());
				}

                //std::string path = decrypt(http_client->get_request_head().path.substr(8));

                //LOG_STR(Trace, path.c_str());
            }

            senders_.pop_front();

            if (status_ == StatusType::timeout) {
                senders_.clear();
            }

            if (senders_.size() > 0) {
                std::list<DacSubmitTask>::const_iterator iter = senders_.begin();
                http_client_->async_fetch((*iter).request, 
                    boost::bind(&DacModule::handle_fetch, this, _1));
            }

            if (status_ == StatusType::stopped
                && senders_.empty()) {
                    error_code ec;
                    timer_->cancel(ec);
            }
        }

        void DacModule::submit2(
            DacBaseInfo * sinfo)
        {
            if (status_ == StatusType::timeout)
                return;

            if (sinfo->ignore_this())
                return;

            {
                LOG_TRACE("[submit2] " << type_str[sinfo->interface_type]);
            }

            if (!is_first_open_channel_ && sinfo->interface_type == InterfaceType::play_open_info_type) {
                is_first_open_channel_ = true;
                first_play_info();
            }

            set_base_info(*sinfo);
            if (sinfo->has_core_info())
                set_core_info((DacCoreCommInfo &)*sinfo);
            DacArchive oa;
            sinfo->do_serialize(oa);

            LOG_STR(framework::logger::Trace, ("submit_str", oa.str()));

            DacSubmitTask info;
            util::protocol::HttpRequestHead & head = info.request.head();
            head.method = HttpRequestHead::get;
            head.path = "/1.html?" + encrypt(oa.str());
            head["Accept"] = "{*/*}";
            head.host.reset(addr_host(dns_dac_server));
            info.type = sinfo->interface_type;
            boost::system::error_code ec;
            info.reason = sinfo->need_log(ec);
            senders_.push_back(info);

            if (senders_.size() == 1) {
                http_client_->async_fetch(info.request, 
                    boost::bind(&DacModule::handle_fetch, this, _1));
            }

            if ( info.reason != LogReason::none) {
                submit_log2( info.reason, "", ec);
            }

            delete sinfo;
        }

        void DacModule::submit_log2(
            LogReason::Enum reason, 
            std::string const & msg, 
            error_code const & ec)
        {
            if (status_ == StatusType::timeout)
                return;

            {
                LOG_TRACE("[submit_log2] " << log_reason_str[reason]);
            }

            DacSubmitTask info;

            HttpRequestHead & head = info.request.head();
            head.method = HttpRequestHead::post;
            head.host.reset(addr_host(dns_log_server));
            head.path = "/upload/up.php?";

            DacLogInfo log_info(reason, ec);
            log_info.gid = gid_;
            log_info.pid = pid_;
            log_info.version = environment().version.to_string();
            log_info.mac = environment().get_bid();
            DacArchive oa;
            oa << log_info;

            head.path += encrypt(oa.str());

            if (!log_submitter_->get_request(info.request.data(), msg))
                return;

            info.type = InterfaceType::log_info_type;
            info.reason = reason;
            senders_.push_back(info);

            if (senders_.size() == 1) {
                http_client_->async_fetch(info.request, 
                    boost::bind(&DacModule::handle_fetch, this, _1));
            }
        }

        std::string DacModule::core_version()
        {
            std::string ret;
            ret = vod_version_;
            ret += "|";
            ret += live_version_;
            return ret;
        }

        void DacModule::set_base_info(
            DacBaseInfo & info)
        {
            info.interface_ver = 1;

            info.gid = gid_;
            info.pid = pid_;
            info.ppbox_ver = environment().version.to_string();
            info.bid = environment().get_bid();

            info.sub_time = (boost::uint32_t)time(NULL);
            info.stb_type = StbType::intermediate_layer;
        }

        void DacModule::set_core_info(
            DacCoreCommInfo & info)
        {
            info.core_ver = core_version();
        }

        void DacModule::on_second_timer(
            DacSysInfo * sys_info, 
            error_code const & ec)
        {
            if (ec 
                || status_ != StatusType::waiting) {
                    delete sys_info;
                    sys_info = NULL;

                    return;
            }

            status_ = StatusType::running;

            sys_info->calc();

            submit(*sys_info);

            delete sys_info;
            sys_info = NULL;
        }

        void DacModule::sys_info()
        {
            DacSysInfo * sys_info = new DacSysInfo(live_name_, vod_name_);
            timer_->expires_from_now(Duration::seconds(1));
            timer_->async_wait(boost::bind(&DacModule::on_second_timer, this, sys_info, _1));
        }

        void DacModule::post(
            DacBaseInfo * info)
        {
            io_svc().post(boost::bind(&DacModule::submit2, this, info));
        }

        void DacModule::post_log(
            LogReason::Enum reason, 
            std::string const & msg, 
            error_code const & ec)
        {
            io_svc().post(boost::bind(&DacModule::submit_log2, this, reason, msg, ec));
        }

        void DacModule::on_timer(
            error_code const & ec)
        {
            if (ec 
                || status_ != StatusType::waiting)
                return;

            sys_info();
        }

        void DacModule::first_play_info()
        {
            if (status_ == StatusType::running) {
                status_ = StatusType::waiting;
                timer_->expires_from_now(Duration::minutes(5));
                timer_->async_wait(boost::bind(&DacModule::on_timer, this, _1));
            }
        }

        std::string DacModule::encrypt(
            std::string const & str)
        {
            return pptv::base64_encode(str, PPBOX_KEY);
        }

        std::string DacModule::decrypt(
            std::string const & str)
        {
            return pptv::base64_decode(str, PPBOX_KEY);
        }

        void DacModule::set_live_version(
            std::string const &ver)
        {
            live_version_ = ver;
        }

        void DacModule::set_vod_version(
            std::string const &ver)
        {
            vod_version_ = ver;
        }

        void DacModule::set_live_name(
            std::string const &name)
        {
            live_name_ = name;
        }

        void DacModule::set_vod_name(
            std::string const &name)
        {
            vod_name_ = name;
        }
    }
}
