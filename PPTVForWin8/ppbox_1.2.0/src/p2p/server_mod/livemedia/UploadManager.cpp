//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "pre.h"
#include "UploadManager.h"
#include "BandwidthAdjustmentHandler.h"
#include "packet_statistics.h"

namespace live_media
{
    UploadManager::UploadManager(boost::shared_ptr<SpeedStatistics> speed_statistics, boost::shared_ptr<BandwidthAdjustmentHandler> handler)
        : speed_statistics_(speed_statistics), handler_(handler)
    {
    }

    void UploadManager::SetMaxUploadSpeed(size_t max_upload_speed)
    {
        bandwidth_constraint_.SetLimit(max_upload_speed);
    }

    void UploadManager::OnTimer()
    {
        bool reached_upload_limit = CheckUploadLimit();

        boost::uint32_t elapsed_ticks = ticks_since_last_adjustment_.elapsed();

        if (reached_upload_limit)
        {
            if (elapsed_ticks >= 2000)
            {
                ticks_since_last_adjustment_.reset();
                handler_->OnDecreaseBandwidth();
            }

            return;
        }

        if (elapsed_ticks >= 5000)
        {
            ticks_since_last_adjustment_.reset();
            handler_->OnIncreaseBandwidth();
        }
    }

    bool UploadManager::CheckUploadLimit()
    {
        size_t current_upload_speed = speed_statistics_->GetCurentCycleSpeedSend();
        return bandwidth_constraint_.ReachesUploadLimit(current_upload_speed);
    }
}