// M3u8Media.h

#ifndef _PPBOX_DATA_MEDIA_M3U8_MEDIA_H_
#define _PPBOX_DATA_MEDIA_M3U8_MEDIA_H_

#include "ppbox/data/segment/SegmentMedia.h"

#include <boost/shared_ptr.hpp>

namespace ppbox 
{
    namespace data 
    {

        class SourceBase;
        class M3u8MediaImpl;

        class M3u8Media
            : public SegmentMedia
        {
        public:
            typedef boost::function<
                void (boost::system::error_code const &) > 
                response_type;

        public:
            M3u8Media(
                boost::asio::io_service & io_svc,
                framework::string::Url const & url);

            ~M3u8Media();

        public:
            virtual void async_open(
                response_type const & resp);

            virtual void cancel(
                boost::system::error_code & ec);

            virtual void close(
                boost::system::error_code & ec);

        public:
            virtual bool get_basic_info(
                MediaBasicInfo & info,
                boost::system::error_code & ec) const;

            virtual bool get_info(
                MediaInfo & info,
                boost::system::error_code & ec) const;

        public:
            virtual void on_error(
                boost::system::error_code & ec) const;

        public:
            virtual size_t segment_count() const;

            virtual std::string const segment_protocol() const;

            virtual bool segment_url(
                size_t segment, 
                framework::string::Url & url,
                boost::system::error_code & ec) const;

            virtual bool segment_info(
                size_t segment, 
                SegmentInfo & info,
                boost::system::error_code & ec) const;

        private:
            M3u8MediaImpl * impl_;
        };

        PPBOX_REGISTER_MEDIA_BY_FORMAT("m3u8", M3u8Media);

    } // namespace data
} // namespace ppbox

#endif//_PPBOX_DATA_MEDIA_M3U8_MEDIA_H_
