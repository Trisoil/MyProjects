// SourceBase.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/base/SourceBase.h"
#include "ppbox/data/base/MediaBase.h"
#include "ppbox/data/segment/SegmentMedia.h"

#include <boost/bind.hpp>

namespace ppbox
{
    namespace data
    {

        SourceBase * SourceBase::create(
            boost::asio::io_service & io_svc,
            std::string const & proto)
        {
            return factory_type::create(proto, io_svc);
        }

        SourceBase * SourceBase::create(
            boost::asio::io_service & io_svc,
            MediaBase & media)
        {
            SourceBase * source = factory_type::create(media.get_protocol(), io_svc);
            if (source == NULL) {
                MediaBasicInfo info;
                boost::system::error_code ec;
                if (media.get_basic_info(info, ec) && (info.flags & MediaInfo::f_segment)) {
                    source = factory_type::create(((SegmentMedia &)media).segment_protocol(), io_svc);
                }
            }
            return source;
        }

        SourceBase::SourceBase(
            boost::asio::io_service & io_svc)
            : util::stream::Source(io_svc)
        {
        }

        SourceBase::~SourceBase()
        {
        }

        void SourceBase::async_open(
            framework::string::Url const & url,
            boost::uint64_t beg, 
            boost::uint64_t end, 
            response_type const & resp)
        {
            boost::system::error_code ec;
            open(url, beg, end, ec);
            get_io_service().post(boost::bind(resp, ec));
        }

        boost::system::error_code SourceBase::cancel(
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::not_supported;
            return ec;
        }

        boost::uint64_t SourceBase::total(
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::no_data;
            return 0;
        }

        boost::system::error_code SourceBase::set_non_block(
            bool non_block, 
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::not_supported;
            return ec;
        }

        boost::system::error_code SourceBase::set_time_out(
            boost::uint32_t time_out, 
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::not_supported;
            return ec;
        }

        bool SourceBase::continuable(
            boost::system::error_code const & ec)
        {
            return false;
        }

        bool SourceBase::recoverable(
            boost::system::error_code const & ec)
        {
            return false;
        }

    } // namespace data
} // namespace ppbox
