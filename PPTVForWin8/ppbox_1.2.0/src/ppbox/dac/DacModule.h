// DacModule.h

#ifndef _PPBOX_DAC_DAC_MODULE_H_
#define _PPBOX_DAC_DAC_MODULE_H_

#include "ppbox/dac/DacType.h"

#include <framework/timer/TimeTraits.h>

#include <boost/asio/io_service.hpp>

namespace util
{
    namespace protocol
    {
        class HttpClient;
    }
}

namespace ppbox
{
    namespace dac
    {

        class LogSubmitter;
        struct DacBaseInfo;
        struct DacCoreCommInfo;
        struct DacSysInfo;
        struct DacSubmitTask;

        class DacModule
            : public ppbox::common::CommonModuleBase<DacModule>
        {
        public:
            DacModule(
                util::daemon::Daemon & daemon);

            ~DacModule();

        public:
            virtual boost::system::error_code startup();

            virtual void shutdown();

        public:
            void set_auth_code(
                char const * gid, 
                char const * pid, 
                char const * auth);

            void set_live_version(
                std::string const &);

            void set_vod_version(
                std::string const &);

            void set_live_name(
                std::string const &);

            void set_vod_name(
                std::string const &);

        public:
            template <typename Info>
            void submit(
                Info const & info)
            {
                post(new Info(info));
            }

            // UI 调用的上传故障检测日志接口
            void submit_log(
                char const * msg, 
                boost::int32_t size);
            
        private:
            struct StatusType
            {
                enum Enum
                {
                    stopped,
                    running,
                    waiting,
                    timeout,
                };
            };

        private:
            // 第一次播放开始时
            void first_play_info();

            void sys_info();

            void submit2(
                DacBaseInfo * info);

            void submit_log2(
                LogReason::Enum reason, 
                std::string const & msg, 
                boost::system::error_code const & ec);

            void submit_timeout(
                boost::system::error_code const & ec);

            void on_timer(
                boost::system::error_code const & ec);

            std::string encrypt(
                std::string const & str);

            std::string decrypt(
                std::string const & str);

            void handle_fetch(
                boost::system::error_code const & ec);

            void post(
                DacBaseInfo * info);

            void post_log(
                LogReason::Enum reason, 
                std::string const & msg, 
                boost::system::error_code const & ec);

            void on_second_timer(
                DacSysInfo * sys_info, 
                boost::system::error_code const & ec);

            void set_base_info(
                DacBaseInfo & info);

            void set_core_info(
                DacCoreCommInfo & info);

            std::string core_version();

        private:
            clock_timer * timer_;

            bool is_first_open_channel_;

            StatusType::Enum status_;

            std::string gid_;
            std::string pid_;
            std::string auth_;

            util::protocol::HttpClient * http_client_;

            std::list<DacSubmitTask> senders_;

            LogSubmitter * log_submitter_;
            
            std::string live_version_;
            std::string vod_version_;
            std::string live_name_;
            std::string vod_name_;
        };

    } // namespace dac
} // namespace ppbox

#endif // _PPBOX_DAC_DAC_MODULE_H_
