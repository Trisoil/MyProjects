//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "AsioServiceRunner.h"
#include "ServiceStatusDetector.h"

namespace super_node
{
    AsioServiceRunner::AsioServiceRunner(const string& service_name)
        : service_name_(service_name)
    {
    }

    boost::shared_ptr<boost::asio::io_service> AsioServiceRunner::Start(size_t thread_cnt)
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::Service(), "AsioServiceRunner::Start()");

        io_service_.reset(new boost::asio::io_service());
        io_service_work_.reset(new boost::asio::io_service::work(*io_service_));

        assert(threads_.empty() && thread_cnt);
        threads_.resize(thread_cnt);

        for (size_t i = 0; i < thread_cnt; ++i)
        {
            threads_[i].reset(new boost::thread(boost::bind(&AsioServiceRunner::ServiceThreadProc, shared_from_this(), i)));
        }

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

        for (size_t i = 0; i < threads_.size(); ++i)
        {
            threads_[i]->join();
            LOG4CPLUS_INFO(Loggers::Service(), "Thread "<<service_name_<<"["<<i<<"] stopped.");
        }
        threads_.clear();

        io_service_work_.reset();
        io_service_.reset();
    }

    void AsioServiceRunner::ServiceThreadProc(size_t id)
    {
        LOG4CPLUS_TRACE_METHOD(Loggers::Service(), "AsioServiceRunner::ServiceThreadProc()");
        LOG4CPLUS_INFO(Loggers::Service(), "Thread " << service_name_ << "[" << id << "] started.");

        assert(io_service_);

        while(true)
        {
            try
            {
                io_service_->run();
                break;
            }
            catch(...)
            {
                LOG4CPLUS_ERROR(Loggers::Service(), "An error occurred in "<<service_name_<<"[" << id << "] thread. Will retry shortly.");
            }
        }
    }
}
