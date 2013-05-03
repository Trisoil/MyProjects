// BodyStrategy.h

#ifndef _PPBOX_DATA_STRATEGY_BODY_STRATEGY_H_
#define _PPBOX_DATA_STRATEGY_BODY_STRATEGY_H_

#include <ppbox/data/segment/SegmentStrategy.h>

namespace ppbox
{
    namespace data
    {

        class BodyStrategy
            : public SegmentStrategy
        {
        public:
            BodyStrategy(
                SegmentMedia & media);

            virtual ~BodyStrategy();

        private:
            virtual void byte_range(
                SegmentPosition const & info, 
                SegmentRange & range);

            virtual void time_range(
                SegmentPosition const & info, 
                SegmentRange & range);
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_STRATEGY_BODY_STRATEGY_H_
