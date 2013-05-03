// ListStrategy.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/strategy/ListStrategy.h"
#include "ppbox/data/base/SourceError.h"

namespace ppbox
{
    namespace data
    {

        ListStrategy::ListStrategy(
            SegmentMedia & media)
            : SegmentStrategy(media)
            , strategys_(NULL)
            , last_strategy_(NULL)
        {
        }

        ListStrategy::~ListStrategy()
        {
            clear();
        }

        bool ListStrategy::next_segment(
            SegmentPosition & pos, 
            boost::system::error_code & ec)
        {
            if (pos.item_context == NULL) {
                pos = SegmentPosition();
                strategys_->strategy->next_segment(pos, ec);
                pos.item_context = strategys_;
                return true;
            }

            ListItem * item = (ListItem *)pos.item_context;
            if (item->strategy->next_segment(pos, ec)) {
                return true;
            }

            if (ec == source_error::no_more_segment && item->next) {
                item = item->next;
                pos.item_context = NULL;
                item->strategy->next_segment(pos, ec);
                pos.item_context = item;
            }

            return !ec;
        }

        bool ListStrategy::get_url(
            SegmentPosition const & pos, 
            framework::string::Url & url, 
            boost::system::error_code & ec)
        {
            ListItem * item = (ListItem *)pos.item_context;
            return item->strategy->get_url(pos, url, ec);
        }

        void ListStrategy::on_error(
            SegmentPosition const & pos, 
            boost::system::error_code & ec)
        {
            ListItem * item = (ListItem *)pos.item_context;
            return item->strategy->on_error(pos, ec);
        }

        void ListStrategy::insert(
            SegmentStrategy & strategy)
        {
            ListItem * item = new ListItem;
            item->strategy = &strategy;
            if (last_strategy_) {
                item->next = last_strategy_->next;
                last_strategy_->next = item;
                last_strategy_ = item;
            } else {
                strategys_ = last_strategy_ = item;
            }
        }

        void ListStrategy::clear()
        {
            while (strategys_) {
                last_strategy_ = strategys_->next;
                delete strategys_;
                strategys_ = last_strategy_;
            }
        }

    } // namespace data
} // namespace ppbox
