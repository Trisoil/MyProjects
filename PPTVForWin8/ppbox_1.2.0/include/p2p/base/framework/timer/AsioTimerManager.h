// AsioTimerManager.h

#ifndef _FRAMEWORK_TIMER_ASIO_TIMER_MANAGER_H_
#define _FRAMEWORK_TIMER_ASIO_TIMER_MANAGER_H_

#include "framework/timer/TimeTraits.h"
#include "framework/timer/TimerQueue.h"

#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>

namespace framework
{
    namespace timer
    {

        class AsioTimerManager
            : public TimerQueue
            , clock_timer
        {
        public:
            // herain(2011-12-9):need_skip详细解释见TimeQueue.h
            AsioTimerManager(
                boost::asio::io_service & io_svc, 
                boost::posix_time::time_duration const & interval,
                bool need_skip = true);

            AsioTimerManager(
                boost::asio::io_service & io_svc, 
                Duration const & interval,
                bool need_skip = true);

            ~AsioTimerManager();

            void start();

            void stop();

        private:
            void handle_timer(
                boost::system::error_code const & ec, 
                boost::weak_ptr<void> const & cancel_token);

        private:
            boost::shared_ptr<void> cancel_token_;
        };

    } // namespace timer
} // namespace framework

#endif // _FRAMEWORK_TIMER_ASIO_TIMER_MANAGER_H_
