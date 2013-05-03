//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "State.h"

namespace super_node
{
    void State::Add(boost::uint8_t StatisticsType, size_t add_num)
    {
        switch(StatisticsType)
        {
        case VISIT:
            visit_ += add_num;
            break;
        case HIT:
            hit_ += add_num;
            break;
        case CONTINUOUS_HIT:
            continuous_hit_ += add_num;
            break;
        case LOAD:
            load_ += add_num;
            break;
        case LOADED:
            loaded_ += add_num;
            break;
        case REFUSED:
            refused_ += add_num;
            break;
        default:
            assert(false);
            break;
        }
    }

    void State::AllTick(boost::uint8_t decay1, boost::uint8_t decay2)
    {
        visit_.Tick(decay1, decay2);
        hit_.Tick();
        continuous_hit_.Tick();
        load_.Tick();
        loaded_.Tick();
        refused_.Tick();
    }
}