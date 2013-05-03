// CertifyUserModule.h

#ifndef _PPBOX_CERTIFY_CERTIFY_USER_MODULE_H_
#define _PPBOX_CERTIFY_CERTIFY_USER_MODULE_H_

#include <ppbox/common/CommonModuleBase.h>

#include "ppbox/certify/AuthStatus.h"

namespace ppbox
{
    namespace certify
    {
        class Certifier;

        class CertifyUserModule
        {
        public:
            CertifyUserModule(
                util::daemon::Daemon & daemon);

            virtual ~CertifyUserModule() {}

        protected:
            // �����������
            boost::system::error_code start_certify();

            // �����������
            void stop_certify();

        protected:
            // �����֤״̬��ͬʱҲ�ᴥ����֤����
            bool is_certified(
                boost::system::error_code & ec);

            bool is_alive();

        private:
            // ������֤�ɹ�״̬
            virtual void certify_startup() = 0;

            // �˳���֤�ɹ�״̬
            virtual void certify_shutdown(
                boost::system::error_code const & ec) = 0;

            // �˳���֤�ɹ�״̬
            virtual void certify_failed(
                boost::system::error_code const & ec) = 0;

        private:
            friend class Certifier;
            // ��֤״̬ͨ��
            void on_certify(
                AuthStatus::Enum type, 
                boost::system::error_code const & ec);

        protected:
            ppbox::certify::Certifier & cert_;
        };

        template <
            typename ModuleType
        >
        class CertifyUserModuleBase
            : public ppbox::common::CommonModuleBase<CertifyUserModuleBase<ModuleType> >
            , public CertifyUserModule
        {
        public:
            CertifyUserModuleBase(
                util::daemon::Daemon & daemon, 
                std::string const & name = "")
                : ppbox::common::CommonModuleBase<CertifyUserModuleBase<ModuleType> >(daemon, name)
                , CertifyUserModule(daemon)
            {
            }
        };

    } // namespace certify
} // namespace ppbox

#endif // _PPBOX_CERTIFY_CERTIFY_USER_MODULE_H_
