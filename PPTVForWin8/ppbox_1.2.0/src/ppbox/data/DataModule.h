// DataModule.h

#ifndef _PPBOX_DATA_DATA__MODULE_H_
#define _PPBOX_DATA_DATA__MODULE_H_

#include "ppbox/common/CommonModuleBase.h"

namespace ppbox
{
    namespace data
    {

        class DataModule
            : public ppbox::common::CommonModuleBase<DataModule>
        {
        public:
            DataModule(
                util::daemon::Daemon & daemon);

            ~DataModule();

        public:
            virtual boost::system::error_code startup();

            virtual void shutdown();
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_DATA__MODULE_H_
