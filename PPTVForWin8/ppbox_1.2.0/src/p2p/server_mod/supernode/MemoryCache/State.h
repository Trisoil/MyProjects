//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_STATE_H
#define SUPER_NODE_STATE_H

#include "Statistics.hpp"

namespace super_node
{
    class State
        : public count_object_allocate<State>
    {
    public:
        enum StatisticsEnum
        {
            VISIT,
            HIT,
            CONTINUOUS_HIT,
            LOAD,
            LOADED,
            REFUSED
        };

        const Statistics<size_t>& GetVisit() const { return visit_; }
        const Statistics<size_t>& GetHit() const { return hit_; }
        const Statistics<size_t>& GetContinuousHit() const { return continuous_hit_; }
        const Statistics<boost::uint32_t>& GetLoad() const { return load_; }
        const Statistics<boost::uint32_t>& GetLoaded() const { return loaded_; }
        const Statistics<boost::uint32_t>& GetRefused() const { return refused_; }

        void Add(boost::uint8_t StatisticsType, size_t add_num = 1);
        void AllTick(boost::uint8_t decay1, boost::uint8_t decay2);

    private:
        Statistics<size_t> visit_;              // 访问次数
        Statistics<size_t> hit_;                // Cache命中次数
        Statistics<size_t> continuous_hit_;     // Cache命中次数(连续访问)
        Statistics<boost::uint32_t> load_;               // 加载次数
        Statistics<boost::uint32_t> loaded_;             // 成功加载次数
        Statistics<boost::uint32_t> refused_;            // 拒绝提供数据次数
    };
}

#endif  // SUPER_NODE_STATE_H