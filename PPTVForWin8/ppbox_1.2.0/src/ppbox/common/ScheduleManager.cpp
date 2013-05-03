// ScheduleManager.cpp

#include "ppbox/common/Common.h"
#include "ppbox/common/ScheduleManager.h"

#include <boost/asio/deadline_timer.hpp>
#include <boost/bind.hpp>

namespace ppbox
{
    namespace common
    {

        ScheduleManager::ScheduleManager(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<ScheduleManager>(daemon, "ScheduleManager")
        {
        }

        boost::system::error_code ScheduleManager::startup()
        {
            boost::system::error_code ec;
            return ec;
        }

        // Í£Ö¹ÐÄÌø(keepalive)
        void ScheduleManager::shutdown()
        {
        }

        void * ScheduleManager::schedule_callback(
            boost::uint32_t delay, // ms
            void * user_data, 
            callback_t const & callback)
        {
            boost::asio::deadline_timer * timer2 = new boost::asio::deadline_timer(io_svc());
            timer2->expires_from_now(boost::posix_time::milliseconds(delay));
            timer2->async_wait(boost::bind(callback, user_data, _1));
            return timer2;
        }

        void ScheduleManager::cancel_callback(
            void * handle, 
            boost::system::error_code & ec)
        {
            boost::asio::deadline_timer * timer = (boost::asio::deadline_timer *)handle;
            timer->cancel(ec);
            delete timer;
        }

    } // namespace common
} // namespace ppbox
