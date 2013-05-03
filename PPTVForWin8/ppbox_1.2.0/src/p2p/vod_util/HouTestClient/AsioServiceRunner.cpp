//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "AsioServiceRunner.h"



AsioServiceRunner::AsioServiceRunner(const std::string& service_name)
    : service_name_(service_name)
{
}

boost::shared_ptr<boost::asio::io_service> AsioServiceRunner::Start()
{
    
    io_service_.reset(new boost::asio::io_service());
    thread_.reset(new boost::thread(boost::bind(&AsioServiceRunner::ServiceThreadProc, shared_from_this())));
    return io_service_;
}

void AsioServiceRunner::Stop()
{
    if (io_service_)
    {
        io_service_->stop();
    }

    if (thread_)
    {
        thread_->join();
        thread_.reset();
    }

    io_service_.reset();
}

void AsioServiceRunner::ServiceThreadProc()
{
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
            io_service_->reset();
        }
    }
}
