// PortManager.h

#ifndef _PPBOX_COMMON_PORTMANAGER_H_
#define _PPBOX_COMMON_PORTMANAGER_H_

#include <ppbox/common/CommonModuleBase.h>
#include <boost/asio/io_service.hpp>



namespace ppbox
{
    namespace common
    {
        enum ModuleType
        {
            live,
            vod,
            alive
        };

        class PortManager
            : public ppbox::common::CommonModuleBase<PortManager>
        {
        public:
            PortManager(
                util::daemon::Daemon & daemon);
            ~PortManager();

            boost::system::error_code get_port(ModuleType type,boost::uint16_t& port);
            boost::system::error_code set_port(ModuleType type,boost::uint16_t port);
        private:
            struct PortContainer
            {
                PortContainer()
                    : liveport(0)
                    , vodport(0)
                    , aliveport(0)
                {
                }
                boost::uint16_t liveport;
                boost::uint16_t vodport;
                boost::uint16_t aliveport;
            };
        private:
            PortContainer* ports_;
        public:
            virtual boost::system::error_code startup();
            virtual void shutdown();

        };

    } // namespace PortManager
} // namespace ppbox

#endif    // _PPBOX_COMMON_PORTMANAGER_H_
