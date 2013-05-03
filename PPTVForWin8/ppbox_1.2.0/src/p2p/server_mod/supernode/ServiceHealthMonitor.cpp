//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "ServiceHealthMonitor.h"

namespace super_node
{
    boost::shared_ptr<ServiceHealthMonitor> ServiceHealthMonitor::Instance()
    {
        static boost::shared_ptr<ServiceHealthMonitor> instance(new ServiceHealthMonitor());
        return instance;
    }

    ServiceHealthMonitor::ServiceHealthMonitor()
    {
        running_ = false;
    }

    void ServiceHealthMonitor::Start(boost::shared_ptr<boost::asio::io_service> io_service)
    {
        timer_.reset(new boost::asio::deadline_timer(*io_service));
        timer_->expires_from_now(boost::posix_time::seconds(5));
        timer_->async_wait(bind(&ServiceHealthMonitor::OnReportTimerElapsed, shared_from_this()));
        running_ = true;
    }

    void ServiceHealthMonitor::Stop()
    {
        running_ = false;

        if (timer_)
        {
            boost::system::error_code cancel_error;
            timer_->cancel(cancel_error);
            timer_.reset();
        }
    }

    boost::shared_ptr<ServiceComponent> ServiceHealthMonitor::RegisterComponent(const string& component_name)
    {
        LOG4CPLUS_TRACE(Loggers::Service(), "Registering component:"<<component_name);

        boost::mutex::scoped_lock lock(mutex_);
        if(service_components_.find(component_name) == service_components_.end())
        {
            boost::shared_ptr<ServiceComponent> service_component(new ServiceComponent(component_name));
            service_components_[component_name] = service_component;    
        }

        return service_components_[component_name];
    }

    void ServiceHealthMonitor::UnregisterComponent(const string& component_name)
    {
        LOG4CPLUS_TRACE(Loggers::Service(), "Unregistering component:"<<component_name);

        boost::mutex::scoped_lock lock(mutex_);
        std::map<string, boost::shared_ptr<ServiceComponent> >::iterator iter = service_components_.find(component_name);
        if (iter != service_components_.end())
        {
            iter->second->Unregister();
            service_components_.erase(iter);
        }
    }

    void ServiceHealthMonitor::ReportServiceHealth()
    {
        std::ofstream status_file("ServiceHealth.txt");
        if (!status_file)
        {
            return;
        }
            
        boost::mutex::scoped_lock lock(mutex_);
        boost::dynamic_bitset<boost::uint8_t> components_in_bad_state(service_components_.size());
        size_t component_index = 0;
        for(std::map<string, boost::shared_ptr<ServiceComponent> >::const_iterator iter = service_components_.begin();
            iter != service_components_.end();
            ++iter)
        {
            if (!iter->second->IsHealthy())
            {
                //dynamic_bitset输出到stream是反序的，所以这里反着设其对应的bit
                components_in_bad_state.set(components_in_bad_state.size() - component_index - 1);
            }

            ++component_index;
        }
        
        status_file<<((components_in_bad_state.count() > 0) ? '1' : '0')<<std::endl;
        status_file<<components_in_bad_state<<std::endl;

        for(std::map<string, boost::shared_ptr<ServiceComponent> >::const_iterator iter = service_components_.begin();
            iter != service_components_.end();
            ++iter)
        {
            boost::shared_ptr<ServiceComponent> component = iter->second;
            status_file << "[" << component->GetComponentName() << "]" <<std::endl;
            std::map<string, boost::shared_ptr<ComponentStatusItem> > status_items;
            component->GetStatusItems(status_items);
            for(std::map<string, boost::shared_ptr<ComponentStatusItem> >::const_iterator status_iter = status_items.begin();
                status_iter != status_items.end();
                ++status_iter)
            {
                boost::shared_ptr<ComponentStatusItem> status_item = status_iter->second;
                if (!status_item->IsHealthy())
                {
                    status_file<<"*";
                }

                status_file<<"\t"<<status_item->GetStatusDescription()<<std::endl;
            }
        }
    }

    void ServiceHealthMonitor::OnReportTimerElapsed()
    {
        if (!running_)
        {
            return;    
        }

        ReportServiceHealth();

        if (timer_)
        {
            timer_->expires_from_now(boost::posix_time::seconds(5));
            timer_->async_wait(bind(&ServiceHealthMonitor::OnReportTimerElapsed, shared_from_this()));
        }
    }
}
