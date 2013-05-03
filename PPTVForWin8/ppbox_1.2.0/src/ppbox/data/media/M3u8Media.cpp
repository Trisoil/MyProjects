
// M3u8Media.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/media/M3u8Media.h"
#include "ppbox/data/media/M3u8MediaImpl.h"

namespace ppbox
{
    namespace data
    {

        M3u8Media::M3u8Media(
            boost::asio::io_service & io_svc,
            framework::string::Url const & url)
            : SegmentMedia(io_svc, url)
            , impl_(new M3u8MediaImpl(io_svc, url))
        {
        }

        M3u8Media::~M3u8Media()
        {
        }

        void M3u8Media::async_open(
            response_type const & resp)
        {
            impl_->async_open(resp);
        }

        void M3u8Media::cancel(
            boost::system::error_code & ec)
        {
            impl_->cancel(ec);
        }

        void M3u8Media::close(
            boost::system::error_code & ec)
        {
            impl_->close(ec);
            impl_ = NULL;
        }

        bool M3u8Media::get_basic_info(
            MediaBasicInfo & info,
            boost::system::error_code & ec) const
        {
            return impl_->get_basic_info(info, ec);
        }

        bool M3u8Media::get_info(
            MediaInfo & info,
            boost::system::error_code & ec) const
        {
            return impl_->get_info(info, ec);
        }

        void M3u8Media::on_error(
            boost::system::error_code & ec) const
        {
            impl_->on_error(ec);
        }

        size_t M3u8Media::segment_count() const
        {
            return impl_->segment_count();
        }

        std::string const M3u8Media::segment_protocol() const
        {
            return impl_->segment_protocol();
        }

        bool M3u8Media::segment_url(
            size_t segment, 
            framework::string::Url & url,
            boost::system::error_code & ec) const
        {
            return impl_->segment_url(segment, url, ec);
        }

        bool M3u8Media::segment_info(
            size_t segment, 
            SegmentInfo & info,
                boost::system::error_code & ec) const
        {
            return impl_->segment_info(segment, info, ec);
        }

    } // data
} // ppbox
