// Certifier.h

#ifndef _PPBOX_CERTIFY_CERTIFIER_H_
#define _PPBOX_CERTIFY_CERTIFIER_H_

#include "ppbox/certify/AliveClient.h"
#include "ppbox/certify/AuthStatus.h"

#include <ppbox/common/PortManager.h>

#ifndef PPBOX_CONTAIN_PPBOX_ALIVE
#include <framework/process/Process.h>
#endif
#include <framework/string/Url.h>

#include <boost/thread/mutex.hpp>

namespace ppbox
{
#ifndef PPBOX_DISABLE_DAC
    namespace dac
    {
        class DacModule;
    }
#endif
    namespace certify
    {

        struct CertifyType
        {
            enum Enum
            {
                local, 
                vod, 
                live, 
            };
        };

        class CertifyUserModule;

        class Certifier
            : public ppbox::common::CommonModuleBase<Certifier>
        {
        public:
            Certifier(
                util::daemon::Daemon & daemon);

        public:
            // ��������(keepalive)
            virtual boost::system::error_code startup();

            // ֹͣ����(keepalive)
            virtual void shutdown();

        public:
            // ȷ������startup()֮ǰ��Ҫ���øú���
            void set_auth_code(
                char const * gid, 
                char const * pid, 
                char const * auth);

        public:
            // ��֤
            boost::system::error_code certify_add_module(
                CertifyUserModule & module);

            boost::system::error_code certify_del_module(
                CertifyUserModule & module);

            // �����֤״̬��ͬʱҲ�ᴥ����֤����
            bool is_certified(
                boost::system::error_code & ec);

        public:
            boost::system::error_code certify_url(
                framework::string::Url const & url, 
                std::string & out_key, 
                boost::system::error_code & ec);

            bool is_alive();

        private:
            void send_keepalive();

            void keepalive_callback(
                KeepaliveResponse const & response);

            void restart();

        private:
            std::string worker_name_;

        private:
            ppbox::common::PortManager &portMgr_;
#ifndef PPBOX_CONTAIN_PPBOX_ALIVE
            framework::process::Process process_;
#endif
            AuthStatus::Enum auth_status_;
            boost::system::error_code last_error_;
            std::string gid_;
            std::string pid_;
            std::string auth_;

            boost::mutex mutex_;
            std::vector<CertifyUserModule *> modules_;

            AliveClient client_;

            std::string vod_playlink_key_;
            std::string live_playlink_key_;
        };

    } // namespace Certifier
} // namespace ppbox

#endif // _PPBOX_CERTIFY_CERTIFIER_H_

