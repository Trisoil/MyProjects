// FullStrategy.h

#ifndef _PPBOX_DATA_STRATEGY_FULL_STRATEGY_H_
#define _PPBOX_DATA_STRATEGY_FULL_STRATEGY_H_

#include "ppbox/data/segment/SegmentStrategy.h"

namespace ppbox
{
    namespace data
    {

        class FullStrategy
            : public SegmentStrategy
        {
        public:
            FullStrategy(
                SegmentMedia & media);

            virtual ~FullStrategy();
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_STRATEGY_FULL_STRATEGY_H_
