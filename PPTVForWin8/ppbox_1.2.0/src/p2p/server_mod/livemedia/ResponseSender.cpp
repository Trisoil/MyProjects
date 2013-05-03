//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "pre.h"
#include "ResponseSender.h"
#include "AsioServiceRunner.h"
#include "ResponseTask.h"
#include "channel_manager.h"

namespace live_media
{
    ResponseSender::ResponseSender(size_t id, boost::shared_ptr<ChannelManager> manager)
    {
        manager_ = manager;

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
        response_task->Execute(manager_->server_);
    }
}