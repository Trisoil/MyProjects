//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef _SESSIONS_COUNT_CONSTRAINT_H_
#define _SESSIONS_COUNT_CONSTRAINT_H_

#include "Constraint.h"

namespace live_media
{
    class SessionsCountConstraint
    {
    public:
        bool AcceptsNew(size_t current_sessions_count) const
        {
            return !constraint_.ReachesLimit(current_sessions_count);
        }

        void SetHardLimit(int max_sessions_count)
        {
            constraint_.SetHardLimit(max_sessions_count);
        }

        size_t GetCurrentLimit() const
        {
            return constraint_.GetCurrentLimit();
        }

        void IncreaseLimit(size_t current_sessions_count)
        {
            constraint_.IncreaseLimit(current_sessions_count);
        }

        void DecreaseLimit(size_t current_sessions_count)
        {
            constraint_.DecreaseLimit(current_sessions_count);
        }

    private:
        Constraint constraint_;
    };
}

#endif //_SESSIONS_COUNT_CONSTRAINT_H_