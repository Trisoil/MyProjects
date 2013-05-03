// AvformatModule.cpp

#include "ppbox/avformat/Common.h"
#include "ppbox/avformat/AvformatModule.h"
#include "ppbox/avformat/codec/CodecTypes.h"

namespace ppbox
{
    namespace avformat
    {

        AvformatModule::AvformatModule(
            util::daemon::Daemon & daemon)
            : ppbox::common::CommonModuleBase<AvformatModule>(daemon, "AvformatModule")
        {
        }

        AvformatModule::~AvformatModule()
        {
        }

        boost::system::error_code AvformatModule::startup()
        {
            boost::system::error_code ec;
            return ec;
        }

        void AvformatModule::shutdown()
        {
        }

    } // namespace avformat
} // namespace ppbox
