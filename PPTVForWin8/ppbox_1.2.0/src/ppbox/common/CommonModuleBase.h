// CommonModuleBase.h

#ifndef _PPBOX_COMMON_COMMON_MODULE_BASE_H_
#define _PPBOX_COMMON_COMMON_MODULE_BASE_H_

#include "ppbox/common/CommonModule.h"

namespace ppbox
{
    namespace common
    {

        template <
            typename ModuleType
        >
        class CommonModuleBase
            : public util::daemon::ModuleBase<CommonModuleBase<ModuleType> >
        {
        public:
            CommonModuleBase(
                util::daemon::Daemon & daemon, 
                std::string const & name = "")
                : util::daemon::ModuleBase<CommonModuleBase<ModuleType> >(daemon, name)
                , common_(util::daemon::use_module<CommonModule>(daemon))
            {
            }

        public:
            ppbox::common::CommonModule & common()
            {
                return common_;
            }

            Environment const & environment() const
            {
                return common_.environment();
            }
            framework::memory::SharedMemory & shared_memory()
            {
                return common_.shared_memory();
            }

            framework::timer::TimerQueue & timer_queue()
            {
                return common_.timer_queue();
            }

            framework::process::MessageQueue & msg_queue()
            {
                return common_.msg_queue();
            }

        private:
            ppbox::common::CommonModule & common_;
        };

    } // namespace common
} // namespace ppbox

#endif // _PPBOX_COMMON_COMMON_MODULE_H_
