//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "pre.h"
#include "GlobalUploadHandler.h"
#include "UploadManager.h"
#include "channel_server.h"
#include "SessionManagement/SessionManager.h"

namespace live_media
{
    GlobalUploadHandler::GlobalUploadHandler(boost::shared_ptr<ChannelServer> channel_server)
        : channel_server_(channel_server)
    {
    }

    void GlobalUploadHandler::SetMaxUploadSpeed(int max_upload_speed)
    {
        upload_manager_->SetMaxUploadSpeed(max_upload_speed);
    }

    void GlobalUploadHandler::Start()
    {
        upload_manager_.reset(new UploadManager(channel_server_->speed_statistics_, shared_from_this()));
    }

    void GlobalUploadHandler::OnTimer()
    {
        assert(upload_manager_);
        upload_manager_->OnTimer();
    }

    void GlobalUploadHandler::OnDecreaseBandwidth()
    {
        channel_server_->GetSessionManager()->DecreaseSessionsCount();
    }

    void GlobalUploadHandler::OnIncreaseBandwidth()
    {
        channel_server_->GetSessionManager()->IncreaseSessionsCount();
    }
}