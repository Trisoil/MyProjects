//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_ASIO_SERVICE_RUNNER_H
#define SUPER_NODE_ASIO_SERVICE_RUNNER_H

namespace super_node
{
    class ServiceStatus
        : public boost::enable_shared_from_this<ServiceStatus>
    {
    public:
        ServiceStatus(const std::string& service_name, boost::shared_ptr<boost::asio::io_service> io_service)
        {
            io_service_ = io_service;
            service_name_ = service_name;
            last_queue_status_update_ = boost::posix_time::microsec_clock::local_time();
        }

        void Touch()
        {
            boost::posix_time::ptime start = boost::posix_time::microsec_clock::local_time();
            io_service_->post(
                boost::bind(
                    &ServiceStatus::LoadHandler,
                    shared_from_this(),
                    start));
        }

        const std::string& ServiceName() const { return service_name_; }

        boost::posix_time::time_duration GetQueueLoadStatus(boost::posix_time::ptime& last_update_time)
        {
            boost::mutex::scoped_lock lock(mutex_);
            last_update_time = last_queue_status_update_;
            return last_queue_wait_time_;
        }

    private:
        void LoadHandler(const boost::posix_time::ptime& start)
        {
            boost::mutex::scoped_lock lock(mutex_);
            last_queue_status_update_ = boost::posix_time::microsec_clock::local_time();
            last_queue_wait_time_ = last_queue_status_update_ - start;
        }

    private:
        boost::shared_ptr<boost::asio::io_service> io_service_;
        std::string service_name_;

        boost::posix_time::ptime last_queue_status_update_;
        boost::posix_time::time_duration last_queue_wait_time_;
        boost::mutex mutex_;
    };
}

#endif //SUPER_NODE_ASIO_SERVICE_RUNNER_H