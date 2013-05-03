//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_SERVICE_HEALTH_MONITOR_H
#define SUPER_NODE_SERVICE_HEALTH_MONITOR_H

#include "ServiceHealthMonitor/ServiceComponent.h"
#include "ServiceHealthMonitor/ServiceComponents.h"
#include "ServiceHealthMonitor/ComponentStatusItem.h"

namespace super_node
{
    class ServiceHealthMonitor
        : public boost::enable_shared_from_this<ServiceHealthMonitor>
    {
    public:
        static boost::shared_ptr<ServiceHealthMonitor> Instance();

        void Start(boost::shared_ptr<boost::asio::io_service> io_service);
        void Stop();

        boost::shared_ptr<ServiceComponent> RegisterComponent(const string& component_name);
        void UnregisterComponent(const string& component_name);

    private:
        ServiceHealthMonitor();
        void ReportServiceHealth();
        void OnReportTimerElapsed();

    private:
        std::map<string, boost::shared_ptr<ServiceComponent> > service_components_;
        boost::shared_ptr<boost::asio::deadline_timer> timer_;
        bool running_;
        boost::mutex mutex_;
    };
}

#endif //SUPER_NODE_SERVICE_HEALTH_MONITOR_H