// ConfigMgr.cpp

#include "ppbox/common/Common.h"
#include "ppbox/common/ConfigMgr.h"

#include <framework/timer/Timer.h>
#include <framework/string/Slice.h>
#include <framework/process/MessageQueue.h>
using namespace framework::string;
using namespace framework::process;

#include <boost/bind.hpp>
using namespace boost::system;

#define MSGTYPECONFIG   1
#define MSGLEVEL        0

namespace ppbox
{
    namespace common
    {

        ConfigMgr::ConfigMgr(
            util::daemon::Daemon & daemon)
            : ppbox::common::CommonModuleBase<ConfigMgr>(daemon, "ConfigMgr")
            , timer_(NULL)
        {
        }

        error_code ConfigMgr::startup()
        {
            error_code ec;
            timer_ = new framework::timer::PeriodicTimer(
                timer_queue(), 1000, boost::bind(&ConfigMgr::handle_timer, this));
            timer_->start();
            return ec;
        }

        // Í£Ö¹ÐÄÌø(keepalive)
        void ConfigMgr::shutdown()
        {
            timer_->stop();
            delete timer_;
            timer_ = NULL;
        }

        void ConfigMgr::set_config(
            char const * module, 
            char const * section, 
            char const * key, 
            char const * value)
        {
            Message msg;
            if (module)
                msg.receiver = module;
            msg.level = MSGLEVEL;
            msg.type = MSGTYPECONFIG;
            msg.data = section + std::string("|") + key + std::string("|") + value;
            msg_queue().push(msg);
        }

        void ConfigMgr::handle_timer()
        {
            Message msg;
            msg.level = MSGLEVEL;
            msg.type = MSGTYPECONFIG;
            if (msg_queue().pop(msg)) {
                std::vector<std::string> datas;
                slice<std::string>(msg.data, std::inserter(datas, datas.end()), "|");
                if (datas.size() != 3) {
                    return;
                }

                config().set(datas[0], datas[1], datas[2]);
            }
        }

    } // namespace common
} // namespace ppbox
