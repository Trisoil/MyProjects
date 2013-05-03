//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "ResponseSender.h"
#include "AsioServiceRunner.h"
#include "ResponseTask.h"
#include "UdpServerManager.h"

namespace super_node
{
    ResponseSender::ResponseSender(size_t id, boost::shared_ptr<UdpServerManager> udp_server_manager)
    {
        udp_server_manager_ = udp_server_manager;

        std::ostringstream service_name;
        service_name << "sender_"<<id;
        runner_.reset(new AsioServiceRunner(service_name.str()));
    }

    void ResponseSender::Start()
    {
        io_service_ = runner_->Start();
    }

    void ResponseSender::Stop()
    {
        runner_->Stop();
    }

    void ResponseSender::SendResponse(boost::shared_ptr<ResponseTask> response_task)
    {
        io_service_->post(boost::bind(&ResponseSender::DoSendTask, shared_from_this(), response_task));
    }

    void ResponseSender::DoSendTask(boost::shared_ptr<ResponseTask> response_task)
    {
        assert(response_task);
        response_task->Execute(udp_server_manager_);
    }
}