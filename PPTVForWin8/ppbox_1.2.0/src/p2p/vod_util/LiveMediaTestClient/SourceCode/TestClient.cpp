//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "TestClient.h"
#include "RequestGenerator.h"
#include "RequestSender.h"
#include "RequestsQueue.h"
#include "Request.h"
#include "ResourceSelector.h"

namespace live_media_test
{
    TestClient::TestClient(boost::shared_ptr<boost::asio::io_service> io_service)
        : io_service_(io_service)
    {
        resource_selector_.reset(new ResourceSelector(io_service_));
        request_queue_.reset(new RequestsQueue());
        request_generator_.reset(new RequestGenerator(io_service_, request_queue_, resource_selector_));
        request_sender_.reset(new RequestSender(io_service_, request_queue_));
    }

    void TestClient::Start()
    {
        resource_selector_->Start();
        request_generator_->Start();
        request_sender_->Start();
    }

    void TestClient::Stop()
    {
        resource_selector_->Stop();
        request_generator_->Stop();
        request_sender_->Stop();
    }
}

