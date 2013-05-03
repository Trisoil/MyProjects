// IPpbox.cpp

#include "ppbox/ppbox/Common.h"
#include "ppbox/ppbox/Version.h"
#define PPBOX_SOURCE
#include "ppbox/ppbox/IPpbox.h"
using namespace ppbox::error;

#include <ppbox/common/CommonModule.h>
#include <ppbox/common/Version.h>
//#include <ppbox/common/ConfigMgr.h>
#include <ppbox/common/ScheduleManager.h>
#include <ppbox/common/Debuger.h>
#include <ppbox/common/PortManager.h>
#ifndef PPBOX_DISABLE_DEBUGPROXY
#include <ppbox/common/DebugProxy.h>
#endif
using namespace ppbox::common;

#include <ppbox/data/DataModule.h>
#include <ppbox/avformat/AvformatModule.h>
#include <ppbox/demux/DemuxModule.h>
#include <ppbox/merge/MergeModule.h>

#ifndef PPBOX_DISABLE_CERTIFY 
#  include <ppbox/certify/Certifier.h>
#endif

#include <ppbox/cdn/CdnModule.h>

#ifndef PPBOX_DISABLE_PEER
#  include <ppbox/peer/PeerModule.h>
#endif

#ifndef PPBOX_DISABLE_DAC 
#  include <ppbox/dac/DacModule.h>
#endif
#ifndef PPBOX_DISABLE_LIVE 
#  include <ppbox/live/LiveModule.h>
#endif
#ifndef PPBOX_DISABLE_MUX
#  include <ppbox/mux/MuxModule.h>
#endif

#ifndef PPBOX_DISABLE_HTTPD
#  include <ppbox/httpd/HttpdModule.h>
#endif
#ifndef PPBOX_DISABLE_RTSPD
#  include <ppbox/rtspd/RtspdModule.h>
#endif
#ifndef PPBOX_DISABLE_MMSPD
#  include <ppbox/mmspd/MmspdModule.h>
#endif
#ifndef PPBOX_DISABLE_RTMPD
#  include <ppbox/rtmpd/RtmpdModule.h>
#endif

#include <framework/logger/StreamRecord.h>
#include <framework/logger/Section.h>
#include <framework/process/MessageQueue.h>
#include <framework/process/Process.h>
#include <framework/filesystem/Path.h>
using namespace framework::logger;
using namespace framework::process;

#include <boost/asio/error.hpp>
#include <boost/thread/thread.hpp>
using namespace boost::system;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.IPpbox", framework::logger::Debug);

namespace ppbox
{

    class IPpbox
        : public util::daemon::Daemon
    {
    public:
        IPpbox()
            : util::daemon::Daemon("ppbox.conf")
        {

            char const * default_argv[] = {
                "++framework.logger.Stream.0.file=$LOG/ppbox.log", 
                "++framework.logger.Stream.0.append=true", 
                "++framework.logger.Stream.0.roll=true", 
                "++framework.logger.Stream.0.level=5", 
                "++framework.logger.Stream.0.size=102400", 
                "++RtspManager.addr=0.0.0.0:5054+", 
                "++HttpManager.addr=0.0.0.0:9006+", 
            };
            parse_cmdline(sizeof(default_argv) / sizeof(default_argv[0]), default_argv);

            framework::logger::load_config(global_logger(), config());

            ppbox::common::log_versions();

            std::string ppbox_name = "Ppbox_" + framework::filesystem::bin_file().stem();
            ppbox::common::CommonModule & common = 
                util::daemon::use_module<ppbox::common::CommonModule>(*this, ppbox_name);
            common.set_version(ppbox::version());

            //util::daemon::use_module<ppbox::common::ConfigMgr>(*this);
            util::daemon::use_module<ppbox::common::Debuger>(*this);
            util::daemon::use_module<ppbox::common::PortManager>(*this);
#ifndef PPBOX_DISABLE_DEBUGPROXY
            util::daemon::use_module<ppbox::common::DebugProxy>(*this);
#endif
#ifndef PPBOX_DISABLE_CERTIFY
            util::daemon::use_module<ppbox::certify::Certifier>(*this);
#endif
            util::daemon::use_module<ppbox::cdn::CdnModule>(*this);
#ifndef PPBOX_DISABLE_DAC
            util::daemon::use_module<ppbox::dac::DacModule>(*this);
#endif
#ifndef PPBOX_DISABLE_PEER
            util::daemon::use_module<ppbox::peer::PeerModule>(*this);
#endif
#ifndef PPBOX_DISABLE_LIVE
            util::daemon::use_module<ppbox::live::LiveModule>(*this);
#endif
            util::daemon::use_module<ppbox::data::DataModule>(*this);
            util::daemon::use_module<ppbox::avformat::AvformatModule>(*this);
            util::daemon::use_module<ppbox::demux::DemuxModule>(*this);
            util::daemon::use_module<ppbox::merge::MergeModule>(*this);
#ifndef PPBOX_DISABLE_MUX
            util::daemon::use_module<ppbox::mux::MuxModule>(*this);
#endif
#ifndef PPBOX_DISABLE_HTTPD
            util::daemon::use_module<ppbox::httpd::HttpdModule>(*this);
#endif
#ifndef PPBOX_DISABLE_RTSPD
            util::daemon::use_module<ppbox::rtspd::RtspdModule>(*this);
#endif
#ifndef PPBOX_DISABLE_MMSPD
            util::daemon::use_module<ppbox::mmspd::MmspdModule>(*this);
#endif
#ifndef PPBOX_DISABLE_RTMPD
            util::daemon::use_module<ppbox::rtmpd::RtmpdModule>(*this);
#endif
            LOG_INFO("Ppbox ready.");
        }

        boost::uint16_t get_port(char const* module)
        {
            boost::uint16_t port = 0;
            std::string strModule(module);
            if(strModule == "rtsp")
            {
                std::string netName("0.0.0.0:0");
                config().get("RtspManager","addr",netName);
                framework::network::NetName tmpNet(netName);
                port = tmpNet.port();
            }
            else if(strModule == "http")
            {
                std::string netName("0.0.0.0:0");
                config().get("HttpManager","addr",netName);
                framework::network::NetName tmpNet(netName);
                port = tmpNet.port();
            }
            else
            {
            }
            LOG_WARN("[get_port] Module:" << strModule<<" port:"<<port);
            return port;
        }

        error::errors start_p2p_engine(
            char const * gid, 
            char const * pid, 
            char const * auth)
        {
            LOG_SECTION();

            error_code ec;
            if (is_started()) {
                ec = already_start;
            } else {
#ifndef PPBOX_DISABLE_CERTIFY           
                ppbox::certify::Certifier & cert = 
                    util::daemon::use_module<ppbox::certify::Certifier>(*this);
                cert.set_auth_code(gid, pid, auth);
#endif				

#ifndef PPBOX_DISABLE_DAC
                ppbox::dac::DacModule & dac = 
                    util::daemon::use_module<ppbox::dac::DacModule>(*this);
                dac.set_auth_code(gid, pid, auth);
#endif
                ec = start(1);
            }

            return last_error(__FUNCTION__, ec);
        }

        error::errors stop_p2p_engine()
        {
            LOG_SECTION();

            error_code ec;
            if (!is_started()) {
                ec = not_start;
            } else {
                stop();
            }

            return last_error(__FUNCTION__, ec);
        }

        error::errors last_error(
            char const * log_title, 
            error_code const & ec) const
        {
            if (ec && ec != boost::asio::error::would_block) {
                LOG_WARN(log_title << ": " << ec.message());
            }
            ppbox::error::last_error(ec);
            return ppbox::error::last_error_enum(ec);
        }

        error::errors last_error() const
        {
            return ppbox::error::last_error_enum();
        }

        char const * last_error_msg() const
        {
            static std::string err_msg;
            err_msg = ppbox::error::last_error().message();
            return err_msg.c_str();
        }

        char const * version() const
        {
            return ppbox::version_string();
        }


        void set_config(
            char const * module, 
            char const * section, 
            char const * key, 
            char const * value)
        {
            if(NULL == section ||
                NULL == key ||
                NULL == value)
            {
                return;
            }
            config().set(section,key,value);
        }

        void debug_mode(
            bool mode)
        {
            ppbox::common::Debuger & debuger = 
                util::daemon::use_module<ppbox::common::Debuger>(*this);
            debuger.change_debug_mode(mode);
        }

        boost::uint32_t get_debug_msg(
            DialogMessage * vector, 
            boost::int32_t size, 
            char const * module, 
            boost::int32_t level)
        {
            ppbox::common::Debuger & debuger = 
                util::daemon::use_module<ppbox::common::Debuger>(*this);
            static std::vector<Message> msgs;
            msgs.clear();
            debuger.get_debug_msg(msgs, size, module, level);

            for (size_t i = 0; i < msgs.size(); ++i)
            {
                vector[i].time = msgs[i].time;
                vector[i].module = msgs[i].sender.c_str();
                vector[i].level = msgs[i].level;
                vector[i].size = msgs[i].data.length();
                vector[i].msg = msgs[i].data.c_str();
            }

            return msgs.size();
        }

        void submit_msg(
            char const * msg, 
            boost::int32_t size)
        {
#ifndef PPBOX_DISABLE_DAC
            ppbox::dac::DacModule & dac = 
                util::daemon::use_module<ppbox::dac::DacModule>(*this);
            dac.submit_log(msg, size);
#endif
        }

        void log_dump(
            PPBOX_OnLogDump callback,
            boost::uint32_t level)
        {
            ppbox::common::Debuger & debuger = 
                util::daemon::use_module<ppbox::common::Debuger>(*this);
            debuger.set_log_hook(
                ( ppbox::common::Debuger::on_logdump_type )(callback), level);
        }

        PPBOX_HANDLE schedule_callback(
            PP_uint32 delay, 
            void * user_data, 
            PPBOX_Callback callback)
        {
            ppbox::common::ScheduleManager & scheduler = 
                util::daemon::use_module<ppbox::common::ScheduleManager>(*this);
            return scheduler.schedule_callback(delay, user_data, 
                boost::bind(callback, _1, boost::bind(last_error_enum, _2)));
        }

        error::errors schedule_callback(
            PPBOX_HANDLE handle)
        {
            ppbox::common::ScheduleManager & scheduler = 
                util::daemon::use_module<ppbox::common::ScheduleManager>(*this);
            boost::system::error_code ec;
            scheduler.cancel_callback(handle, ec);
            return last_error(__FUNCTION__, ec);
        }
    };

    util::daemon::Daemon & global_daemon()
    {
        static IPpbox the_ppbox;
        return the_ppbox;
    }

}

ppbox::IPpbox & the_ppbox()
{
    return static_cast<ppbox::IPpbox &>(ppbox::global_daemon());
}

using namespace ppbox;

#if __cplusplus
extern "C" {
#endif // __cplusplus

    PPBOX_DECL boost::int32_t PPBOX_StartP2PEngine(
        char const * gid, 
        char const * pid, 
        char const * auth)
    {
        return the_ppbox().start_p2p_engine(gid, pid, auth);
    }

    PPBOX_DECL PP_uint16 PPBOX_GetPort(
        PP_char const * moduleName)
    {
        return the_ppbox().get_port(moduleName);
    }

    PPBOX_DECL void PPBOX_StopP2PEngine()
    {
        the_ppbox().stop_p2p_engine();
    }

    PPBOX_DECL boost::int32_t PPBOX_GetLastError()
    {
        return the_ppbox().last_error();
    }

    PPBOX_DECL char const * PPBOX_GetLastErrorMsg()
    {
        return the_ppbox().last_error_msg();
    }

    PPBOX_DECL char const * PPBOX_GetVersion()
    {
        return the_ppbox().version();
    }

    PPBOX_DECL void PPBOX_SetConfig(
        char const * module, 
        char const * section, 
        char const * key, 
        char const * value)
    {
        return the_ppbox().set_config(module, section, key, value);
    }

    PPBOX_DECL void PPBOX_DebugMode(
        bool mode)
    {
        the_ppbox().debug_mode(mode);
    }

    PPBOX_DECL PP_uint32 PPBOX_DialogMessage(
        DialogMessage * vector, 
        boost::int32_t size, 
        char const * module, 
        boost::int32_t level)
    {
        return the_ppbox().get_debug_msg(vector, size, module, level);
    }

    PPBOX_DECL void PPBOX_SubmitMessage(
        PP_char const * msg, 
        PP_int32 size)
    {
        the_ppbox().submit_msg(msg, size);
    }

    PPBOX_DECL void PPBOX_LogDump(
        PPBOX_OnLogDump callback,
        PP_int32 level)
    {
        the_ppbox().log_dump(callback, level);
    }

    PPBOX_DECL PPBOX_HANDLE PPBOX_ScheduleCallback(
        PP_uint32 delay, 
        void * user_data, 
        PPBOX_Callback callback)
    {
        return the_ppbox().schedule_callback(delay, user_data, callback);
    }

    PPBOX_DECL PP_err PPBOX_CancelCallback(
        PPBOX_HANDLE timer)
    {
        return the_ppbox().schedule_callback(timer);
    }


#if __cplusplus
}
#endif // __cplusplus
