// ConfigMgr.h

#ifndef _PPBOX_COMMON_CONFIG_MGR_H_
#define _PPBOX_COMMON_CONFIG_MGR_H_


namespace framework
{
    namespace timer
    {
        class Timer;
    }
}

using namespace boost::system;


namespace ppbox
{
    namespace common
    {

        class ConfigMgr
            : public ppbox::common::CommonModuleBase<ConfigMgr>
        {
        public:
            ConfigMgr(
                util::daemon::Daemon & daemon);

        public:
            // ¿ªÆôÐÄÌø(keepalive)
            virtual boost::system::error_code startup();

            // Í£Ö¹ÐÄÌø(keepalive)
            virtual void shutdown();

        public:
            void set_config(
                char const * module, 
                char const * section, 
                char const * key, 
                char const * value);

        private:
            void handle_timer();

        private:
            framework::timer::Timer * timer_;
        };

    } // namespace common
} // namespace ppbox

#endif // _PPBOX_COMMON_CONFIG_MGR_H_
