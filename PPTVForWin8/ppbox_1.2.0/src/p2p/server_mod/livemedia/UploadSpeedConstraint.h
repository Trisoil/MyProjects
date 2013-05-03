//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _UPLOAD_SPEED_CONSTRAINT_H
#define _UPLOAD_SPEED_CONSTRAINT_H

#include "Constraint.h"

namespace live_media
{
    class UploadSpeedConstraint
    {
    public:
        bool ReachesUploadLimit(size_t current_upload_speed)
        {
            return constraint_.ReachesLimit(current_upload_speed);
        }

        void SetLimit(size_t max_upload_speed)
        {
            constraint_.SetHardLimit(max_upload_speed);
        }
    private:
        Constraint constraint_;
    };
}

#endif //_UPLOAD_SPEED_CONSTRAINT_H