// SinkBase.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/base/SinkBase.h"

#include <boost/bind.hpp>

namespace ppbox
{
    namespace data
    {

        SinkBase * SinkBase::create(
            boost::asio::io_service & io_svc,
            std::string const & proto)
        {
            return factory_type::create(proto, io_svc);
        }

        SinkBase::SinkBase(
            boost::asio::io_service & io_svc)
            : util::stream::Sink(io_svc)
        {
        }

        SinkBase::~SinkBase()
        {
        }

        void SinkBase::async_open(
            framework::string::Url const & url,
            response_type const & resp)
        {
            boost::system::error_code ec;
            open(url, ec);
            get_io_service().post(boost::bind(resp, ec));
        }

        boost::system::error_code SinkBase::cancel(
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::not_supported;
            return ec;
        }

        bool SinkBase::continuable(
            boost::system::error_code const & ec)
        {
            return false;
        }

        bool SinkBase::recoverable(
            boost::system::error_code const & ec)
        {
            return false;
        }

    } // namespace data
} // namespace ppbox
