//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef LIVE_MEDIA_ASIO_SERVICE_RUNNER_H
#define LIVE_MEDIA_ASIO_SERVICE_RUNNER_H

namespace live_media
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

    class ServiceStatusDetector
    {
    public:
        static ServiceStatusDetector& Instance();

        void AddService(const std::string& service_name, boost::shared_ptr<boost::asio::io_service> io_service)
        {
            boost::shared_ptr<ServiceStatus> status(new ServiceStatus(service_name, io_service));

            boost::mutex::scoped_lock lock(mutex_);
            services_status_.push_back(status);
        }
        
        std::string GetServicesStatus()
        {
            boost::mutex::scoped_lock lock(mutex_);

            std::ostringstream status_stream;
            for(size_t i = 0; i < services_status_.size(); ++i)
            {
                boost::posix_time::ptime last_status_update;
                boost::posix_time::time_duration last_wait_time = services_status_[i]->GetQueueLoadStatus(last_status_update);
                boost::posix_time::time_duration time_since_last_update = boost::posix_time::microsec_clock::local_time() - last_status_update;
                status_stream<<services_status_[i]->ServiceName()<<" "<<last_wait_time.total_milliseconds()<<"ms,"<<time_since_last_update.total_seconds()<<"s"<<";";
            }

            return status_stream.str();
        }
        
        void TouchServices()
        {
            boost::mutex::scoped_lock lock(mutex_);
            for(size_t i = 0; i < services_status_.size(); ++i)
            {
                services_status_[i]->Touch();
            }
        }

    private:
        ServiceStatusDetector(){}
        ServiceStatusDetector(const ServiceStatusDetector&);

        std::vector<boost::shared_ptr<ServiceStatus> > services_status_;
        boost::mutex mutex_;
    };

    class AsioServiceRunner
        : public boost::enable_shared_from_this<AsioServiceRunner>
    {
    public:
        AsioServiceRunner(const string& service_name);
        
        boost::shared_ptr<boost::asio::io_service> Start();
        void Stop();

    private:
        void ServiceThreadProc();

    private:
        boost::shared_ptr<boost::asio::io_service> io_service_;
        boost::shared_ptr<boost::thread> thread_;
        const string service_name_;
    };
}

#endif //LIVE_MEDIA_ASIO_SERVICE_RUNNER_H