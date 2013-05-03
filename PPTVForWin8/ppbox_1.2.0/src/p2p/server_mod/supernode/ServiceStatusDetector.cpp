//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "ServiceStatusDetector.h"
#include "ServiceStatus.h"

namespace super_node
{
    ServiceStatusDetector& ServiceStatusDetector::Instance()
    {
        static ServiceStatusDetector instance;
        return instance;
    }

    void ServiceStatusDetector::AddService(const std::string& service_name, boost::shared_ptr<boost::asio::io_service> io_service)
    {
        boost::shared_ptr<ServiceStatus> status(new ServiceStatus(service_name, io_service));

        boost::mutex::scoped_lock lock(mutex_);
        services_status_.push_back(status);
    }

    const std::map<std::string, size_t> ServiceStatusDetector::GetServicesStatus()
    {
        std::map<std::string, size_t> services_queue_wait_time;
        boost::mutex::scoped_lock lock(mutex_);

        for(size_t i = 0; i < services_status_.size(); ++i)
        {
            boost::posix_time::ptime last_status_update;
            boost::posix_time::time_duration last_wait_time = services_status_[i]->GetQueueLoadStatus(last_status_update);
            services_queue_wait_time[services_status_[i]->ServiceName()] = last_wait_time.total_milliseconds();
        }

        return services_queue_wait_time;
    }

    void ServiceStatusDetector::TouchServices()
    {
        boost::mutex::scoped_lock lock(mutex_);
        for(size_t i = 0; i < services_status_.size(); ++i)
        {
            services_status_[i]->Touch();
        }
    }
}