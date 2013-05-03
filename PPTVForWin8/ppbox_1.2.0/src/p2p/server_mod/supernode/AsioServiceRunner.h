//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_ASIO_SERVICE_RUNNER_H
#define SUPER_NODE_ASIO_SERVICE_RUNNER_H

namespace super_node
{
    class AsioServiceRunner
        : public boost::enable_shared_from_this<AsioServiceRunner>
    {
    public:
        AsioServiceRunner(const string& service_name);
        
        boost::shared_ptr<boost::asio::io_service> Start(size_t thread_cnt = 1);
        void Stop();

    private:
        void ServiceThreadProc(size_t id);

    private:
        boost::shared_ptr<boost::asio::io_service> io_service_;
        boost::shared_ptr<boost::asio::io_service::work> io_service_work_;
        std::vector< boost::shared_ptr<boost::thread> > threads_;
        const string service_name_;
    };
}

#endif //SUPER_NODE_ASIO_SERVICE_RUNNER_H
