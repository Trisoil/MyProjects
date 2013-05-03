// MmspdModule.h

#ifndef _PPBOX_MMSPD_MMSPD_MODULE_H_
#define _PPBOX_MMSPD_MMSPD_MODULE_H_

#include <framework/network/ServerManager.h>
#include <framework/string/Url.h>

namespace ppbox
{
    namespace dispatch
    {
        class DispatchModule;
    }

    namespace mmspd
    {

        class MmspSession;
        class MmspDispatcher;

        class MmspdModule 
            : public ppbox::common::CommonModuleBase<MmspdModule>
            , public framework::network::ServerManager<MmspSession, MmspdModule>
        {
        public:
            MmspdModule(
                util::daemon::Daemon & daemon);

            virtual ~MmspdModule();

        public:
            virtual boost::system::error_code startup();

            virtual void shutdown();

        public:
            using ppbox::common::CommonModuleBase<MmspdModule>::io_svc;

            MmspDispatcher * alloc_dispatcher(
                framework::string::Url & url, 
                boost::system::error_code & ec);

            void free_dispatcher(
                MmspDispatcher * dispatcher);

        private:
            framework::network::NetName addr_;
            ppbox::dispatch::DispatchModule & dispatch_module_;
        };

    } // namespace mmspd
} // namespace ppbox

#endif // _PPBOX_MMSPD_MMSPD_MODULE_H_