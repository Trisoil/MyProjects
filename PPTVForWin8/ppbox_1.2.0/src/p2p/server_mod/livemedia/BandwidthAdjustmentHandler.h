//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _BANDWIDTH_ADJUSTMENT_HANDLER_H
#define _BANDWIDTH_ADJUSTMENT_HANDLER_H

namespace live_media
{
    class BandwidthAdjustmentHandler
    {
    public:
        virtual void OnIncreaseBandwidth() = 0;
        virtual void OnDecreaseBandwidth() = 0;
        virtual ~BandwidthAdjustmentHandler(){}
    };
}

#endif //_BANDWIDTH_ADJUSTMENT_HANDLER_H