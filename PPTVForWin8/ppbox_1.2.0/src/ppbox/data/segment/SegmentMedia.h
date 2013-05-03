// SegmentMedia.h

#ifndef _PPBOX_DATA_SEGMENT_MEDIA_H_
#define _PPBOX_DATA_SEGMENT_MEDIA_H_

#include "ppbox/data/base/MediaBase.h"
#include "ppbox/data/segment/SegmentInfo.h"

namespace ppbox 
{
    namespace data 
    {

        class SegmentMedia
            : public MediaBase
        {
        public:
            typedef boost::function<
                void (boost::system::error_code const &) > 
                response_type;

        public:
            SegmentMedia(
                boost::asio::io_service & io_svc,
                framework::string::Url const & url);

            ~SegmentMedia();

        public:
            virtual bool get_basic_info(
                MediaBasicInfo & info,
                boost::system::error_code & ec) const;

        public:
            virtual size_t segment_count() const = 0;

            virtual std::string const segment_protocol() const
            {
                return get_protocol();
            }

            virtual bool segment_url(
                size_t segment, 
                framework::string::Url & url,
                boost::system::error_code & ec) const = 0;

            virtual bool segment_info(
                size_t segment, 
                SegmentInfo & info,
                boost::system::error_code & ec) const = 0;
        };

    } // namespace data
} // namespace ppbox

#endif//_PPBOX_DATA_SEGMENT_MEDIA_H_
