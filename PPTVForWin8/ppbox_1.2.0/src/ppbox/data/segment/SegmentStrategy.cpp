// SegmentStrategy.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/segment/SegmentStrategy.h"
#include "ppbox/data/base/SourceError.h"

namespace ppbox
{
    namespace data
    {

        SegmentStrategy::SegmentStrategy(
            SegmentMedia const & media)
            : media_(media)
        {
        }

        SegmentStrategy::~SegmentStrategy()
        {
        }

        bool SegmentStrategy::next_segment(
            SegmentPosition & pos, 
            boost::system::error_code & ec)
        {
            if (pos.item_context == NULL) {
                pos.index = boost::uint32_t(-1);
                pos.item_context = this;
            }
            if (++pos.index >= media_.segment_count()) {
                pos.item_context = NULL;
                ec = source_error::no_more_segment;
                return false;
            }
            
            if (!media_.segment_info(pos.index, pos, ec)) {
                --pos.index;
                return false;
            }

            pos.byte_range.before_next();
            byte_range(pos, pos.byte_range);
            pos.byte_range.after_next();

            pos.time_range.before_next();
            time_range(pos, pos.time_range);
            pos.time_range.after_next();

            ec.clear();
            return true;
        }

        bool SegmentStrategy::byte_seek(
            boost::uint64_t offset, 
            SegmentPosition & pos, 
            boost::system::error_code & ec)
        {
            if (offset < pos.byte_range.big_beg()) {
                pos = SegmentPosition();
                if (!next_segment(pos, ec)) {
                    assert(0);
                    return false;
                }
            }

            while (offset >= pos.byte_range.big_end()) {
                if (!next_segment(pos, ec)) {
                    return false;
                }
            }

            pos.byte_range.pos = offset - pos.byte_range.big_offset;

            return true;
        }

        bool SegmentStrategy::time_seek(
            boost::uint64_t offset, 
            SegmentPosition & pos, 
            boost::system::error_code & ec)
        {
            if (offset < pos.time_range.big_beg()) {
                pos = SegmentPosition();
                if (!next_segment(pos, ec)) {
                    assert(0);
                    return false;
                }
            }

            while (offset >= pos.time_range.big_end()) {
                if (!next_segment(pos, ec)) {
                    return false;
                }
            }

            pos.time_range.pos = offset - pos.time_range.big_offset;

            return true;
        }

        bool SegmentStrategy::get_url(
            SegmentPosition const & pos, 
            framework::string::Url & url, 
            boost::system::error_code & ec)
        {
            return media_.segment_url(pos.index, url, ec);
        }

        boost::uint64_t SegmentStrategy::byte_size()
        {
            SegmentPosition pos;
            boost::system::error_code ec;
            while (next_segment(pos, ec));
            return pos.byte_range.big_end();
        }

        boost::uint64_t SegmentStrategy::time_size()
        {
            SegmentPosition pos;
            boost::system::error_code ec;
            while (next_segment(pos, ec));
            return pos.time_range.big_end();
        }

        void SegmentStrategy::on_error(
            SegmentPosition const & pos, 
            boost::system::error_code & ec)
        {
            return media_.on_error(ec);
        }

        void SegmentStrategy::byte_range(
            SegmentPosition const & pos, 
            SegmentRange & range)
        {
            range.beg = 0;
            range.end = pos.size;
        }

        void SegmentStrategy::time_range(
            SegmentPosition const & pos, 
            SegmentRange & range)
        {
            range.beg = 0;
            range.end = pos.duration;
        }

    } // namespace data
} // namespace ppbox
