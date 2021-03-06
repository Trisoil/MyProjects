// Task.h

#include "ppbox/dispatch/Common.h"
#include "ppbox/dispatch/Task.h"
#include "ppbox/dispatch/Session.h"

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>

namespace ppbox
{
    namespace dispatch
    {

        TaskBase::TaskBase(
            TaskInfo & task_info, 
            response_t const & resp)
            : task_info_(task_info)
            , sinks_(*(SinkGroup *)NULL)
            , resp_(resp)
            , buffer_finish_(false)
        {
        }

        TaskBase::TaskBase(
            TaskInfo & task_info, 
            SinkGroup & sinks, 
            SeekRange const & range, 
            response_t const & seek_resp, 
            response_t const & resp)
            : task_info_(task_info)
            , sinks_(sinks)
            , range_(range)
            , seek_resp_(seek_resp)
            , resp_(resp)
            , buffer_finish_(false)
        {
        }

        bool TaskBase::write_sample(
            ppbox::avformat::Sample & sample, 
            boost::system::error_code & ec) const
        {
            return sinks_.write(sample, ec);
        }

        void TaskBase::check_speed(
            ppbox::avformat::Sample const & sample) const
        {
            framework::timer::Time send_time = start_time_ 
                + framework::timer::Duration::milliseconds(sample.time);
            if (send_time > framework::timer::Time::now()) {
                sleep();
            }
        }

        void TaskBase::reset_time(
            boost::uint64_t time)
        {
            start_time_ = framework::timer::Time::now() - framework::timer::Duration::milliseconds(time);
        }

        void TaskBase::sleep() const
        {
            boost::this_thread::sleep(boost::posix_time::milliseconds(100));
        }

        void TaskBase::response(
            response_t const & resp, 
            boost::system::error_code const & ec) const
        {
            task_info_.io_svc.post(boost::bind(resp, ec));
        }

    } // namespace dispatch
} // namespace ppbox
