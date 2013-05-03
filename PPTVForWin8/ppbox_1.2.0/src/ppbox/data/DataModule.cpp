// DataModule.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/DataModule.h"
#include "ppbox/data/base/SourceTypes.h"
#include "ppbox/data/base/SinkTypes.h"
#include "ppbox/data/base/MediaTypes.h"

namespace ppbox
{
    namespace data
    {

        DataModule::DataModule(
            util::daemon::Daemon & daemon)
            : ppbox::common::CommonModuleBase<DataModule>(daemon, "DataModule")
        {
        }

        DataModule::~DataModule()
        {
        }

        boost::system::error_code DataModule::startup()
        {
            boost::system::error_code ec;
            return ec;
        }

        void DataModule::shutdown()
        {
        }

    } // namespace data
} // namespace ppbox
