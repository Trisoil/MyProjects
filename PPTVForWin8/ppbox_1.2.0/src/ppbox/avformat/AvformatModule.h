// AvformatModule.h

#ifndef _PPBOX_AVFORMAT_AVFORMAT_MODULE_H_
#define _PPBOX_AVFORMAT_AVFORMAT_MODULE_H_

#include "ppbox/common/CommonModuleBase.h"

namespace ppbox
{
    namespace avformat
    {

        class AvformatModule
            : public ppbox::common::CommonModuleBase<AvformatModule>
        {
        public:
            AvformatModule(
                util::daemon::Daemon & daemon);

            ~AvformatModule();

        public:
            virtual boost::system::error_code startup();

            virtual void shutdown();
        };

    } // namespace avformat
} // namespace ppbox

#endif // _PPBOX_AVFORMAT_AVFORMAT_MODULE_H_
