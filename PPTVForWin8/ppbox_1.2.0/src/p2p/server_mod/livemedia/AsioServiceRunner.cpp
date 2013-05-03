//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "pre.h"
#include "AsioServiceRunner.h"

namespace live_media
{
    ServiceStatusDetector& ServiceStatusDetector::Instance()
    {
        static ServiceStatusDetector instance;
        return instance;
    }

    AsioServiceRunner::AsioServiceRunner(const string& service_name)
        : service_name_(service_name)
    {
    }

    boost::shared_ptr<boost::asio::io_service> AsioServiceRunner::Start()
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::Service(), "AsioServiceRunner::Start()");
        io_service_.reset(new boost::asio::io_service());
        thread_.reset(new boost::thread(boost::bind(&AsioServiceRunner::ServiceThreadProc, shared_from_this())));

        ServiceStatusDetector::Instance().AddService(service_name_, io_service_);

        return io_service_;
    }

    void AsioServiceRunner::Stop()
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::Service(), "AsioServiceRunner::Stop()");
        if (io_service_)
        {
            LOG4CPLUS_INFO(Loggers::Service(), "Stopping "<<service_name_);
            io_service_->stop();
        }

        if (thread_)
        {
            thread_->join();
            thread_.reset();

            LOG4CPLUS_INFO(Loggers::Service(), "Thread "<<service_name_<<" stopped.");
        }

        io_service_.reset();
    }

    void AsioServiceRunner::ServiceThreadProc()
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::Service(), "AsioServiceRunner::ServiceThreadProc()");
        LOG4CPLUS_INFO(Loggers::Service(), "Thread "<<service_name_<<" started.");

        assert(io_service_);

        while(true)
        {
            try
            {
                boost::asio::io_service::work io_service_work(*io_service_);
                io_service_->run();
                break;
            }
            catch(...)
            {
                LOG4CPLUS_ERROR(Loggers::Service(), "An error occurred in "<<service_name_<<" thread. Will retry shortly.");
                io_service_->reset();
            }
        }
    }
}