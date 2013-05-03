//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_SERVICE_STATUS_DETECTOR_H
#define SUPER_NODE_SERVICE_STATUS_DETECTOR_H

namespace super_node
{
    class ServiceStatus;

    class ServiceStatusDetector
    {
    public:
        static ServiceStatusDetector& Instance();

        void AddService(const std::string& service_name, boost::shared_ptr<boost::asio::io_service> io_service);
        
        const std::map<std::string, size_t> GetServicesStatus();
        
        void TouchServices();

    private:
        ServiceStatusDetector(){}
        ServiceStatusDetector(const ServiceStatusDetector&);

        std::vector<boost::shared_ptr<ServiceStatus> > services_status_;
        boost::mutex mutex_;
    };
}

#endif //SUPER_NODE_SERVICE_STATUS_DETECTOR_H