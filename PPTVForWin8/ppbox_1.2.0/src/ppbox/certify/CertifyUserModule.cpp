// CertifyUserModule.cpp

#include "ppbox/certify/Common.h"
#include "ppbox/certify/CertifyUserModule.h"
#include "ppbox/certify/Certifier.h"

namespace ppbox
{
    namespace certify
    {

        CertifyUserModule::CertifyUserModule(
            util::daemon::Daemon & daemon)
            : cert_(util::daemon::use_module<Certifier>(daemon))
        {
        }

        boost::system::error_code CertifyUserModule::start_certify()
        {
            return cert_.certify_add_module(*this);
        }

        void CertifyUserModule::stop_certify()
        {
            cert_.certify_del_module(*this);
        }

        bool CertifyUserModule::is_certified(
            boost::system::error_code & ec)
        {
            return cert_.is_certified(ec);
        }

        bool CertifyUserModule::is_alive()
        {
            return cert_.is_alive();
        }

        void CertifyUserModule::on_certify(
            AuthStatus::Enum type, 
            boost::system::error_code const & ec)
        {
            switch(type)
            {
            case AuthStatus::failed:
                certify_failed(ec);
                break;
            case AuthStatus::authed:
                certify_startup();
                break;
            case AuthStatus::time_out:
                certify_shutdown(ec);
                break;
            default:
                assert(0);
                break;
            }
        }

    } // namespace certify
} // namespace ppbox
