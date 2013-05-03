//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _UPLOAD_MANAGER_H
#define _UPLOAD_MANAGER_H

#include "UploadSpeedConstraint.h"
#include <framework/timer/TickCounter.h>

namespace live_media
{
    class SpeedStatistics;
    class BandwidthAdjustmentHandler;

    class UploadManager
    {
    public:
        UploadManager(boost::shared_ptr<SpeedStatistics> speed_statistics, boost::shared_ptr<BandwidthAdjustmentHandler> handler);
        void OnTimer();
        void SetMaxUploadSpeed(size_t max_upload_speed);
    private:
        bool CheckUploadLimit();
    private:
        UploadSpeedConstraint bandwidth_constraint_;
        framework::timer::TickCounter ticks_since_last_adjustment_;
        boost::shared_ptr<SpeedStatistics> speed_statistics_;
        boost::shared_ptr<BandwidthAdjustmentHandler> handler_;
    };
}

#endif //_UPLOAD_MANAGER_H