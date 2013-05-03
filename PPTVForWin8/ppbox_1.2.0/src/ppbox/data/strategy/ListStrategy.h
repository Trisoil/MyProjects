// ListStrategy.h

#ifndef _PPBOX_DATA_STRATEGY_LIST_STRATEGY_H_
#define _PPBOX_DATA_STRATEGY_LIST_STRATEGY_H_

#include <ppbox/data/segment/SegmentStrategy.h>

namespace ppbox
{
    namespace data
    {

        class ListStrategy
            : public SegmentStrategy
        {
        public:
            ListStrategy(
                SegmentMedia & media);

            virtual ~ListStrategy();

        public:
            virtual bool next_segment(
                SegmentPosition & pos, 
                boost::system::error_code & ec);

            virtual bool get_url(
                SegmentPosition const & pos, 
                framework::string::Url & url, 
                boost::system::error_code & ec);

            virtual void on_error(
                SegmentPosition const & pos, 
                boost::system::error_code & ec);

        public:
            void insert(
                SegmentStrategy & strategy);

            void clear();

        private:
            struct ListItem
            {
                ListItem()
                    : strategy(NULL)
                    , next(NULL)
                {
                }

                SegmentStrategy * strategy;
                ListItem * next;
            };

        private:
            ListItem * strategys_;
            ListItem * last_strategy_;
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_STRATEGY_LIST_STRATEGY_H_
