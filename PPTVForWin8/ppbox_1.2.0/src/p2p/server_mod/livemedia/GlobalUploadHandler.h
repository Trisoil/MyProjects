//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _GLOBAL_UPLOAD_HANDLER_H
#define _GLOBAL_UPLOAD_HANDLER_H

#include "BandwidthAdjustmentHandler.h"

namespace live_media
{
    class UploadManager;
    class ChannelServer;

    class GlobalUploadHandler
        : public BandwidthAdjustmentHandler,
        public boost::enable_shared_from_this<GlobalUploadHandler>
    {
    public:
        GlobalUploadHandler(boost::shared_ptr<ChannelServer> channel_server);
        void Start();
        void OnTimer();
        void SetMaxUploadSpeed(int max_upload_speed);
        void OnIncreaseBandwidth();
        void OnDecreaseBandwidth();
    private:
        boost::shared_ptr<ChannelServer> channel_server_;
        boost::shared_ptr<UploadManager> upload_manager_;
    };
}

#endif //_GLOBAL_UPLOAD_HANDLER_H