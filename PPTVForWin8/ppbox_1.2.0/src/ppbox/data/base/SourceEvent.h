// SourceEvent.h

#ifndef _PPBOX_DATA_BASE_SOURCE_EVENT_H_
#define _PPBOX_DATA_BASE_SOURCE_EVENT_H_

#include <util/event/Event.h>

namespace ppbox
{
    namespace data
    {

        struct SegmentPosition;

        class SegmentStartEvent
            : public util::event::EventBase<SegmentStartEvent>
        {
        public:
            SegmentPosition const & segment;

            SegmentStartEvent(
                SegmentPosition const & segment)
                : segment(segment)
            {
            }
        };

        // 定期发出的缓存状态

        class SegmentStopEvent
            : public util::event::EventBase<SegmentStopEvent>
        {
        public:
            SegmentPosition const & segment;

            SegmentStopEvent(
                SegmentPosition const & segment)
                : segment(segment)
            {
            }
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_BASE_SOURCE_EVENT_H_
