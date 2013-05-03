// SingleMedia.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/base/SingleMedia.h"

#include <boost/bind.hpp>

namespace ppbox 
{
    namespace data 
    {

        SingleMedia::SingleMedia(
            boost::asio::io_service & io_svc,
            framework::string::Url const & url)
            : MediaBase(io_svc, url)
        {
        }

        SingleMedia::~SingleMedia()
        {
        }

        void SingleMedia::async_open(
            response_type const & resp)
        {
            boost::system::error_code ec;
            get_io_service().post(boost::bind(resp, ec));
        }

        void SingleMedia::cancel(
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::not_supported;
        }

        void SingleMedia::close(
            boost::system::error_code & ec)
        {
            ec.clear();
        }

        bool SingleMedia::get_basic_info(
            MediaBasicInfo & info, 
            boost::system::error_code & ec) const
        {
            info.format = url_.param("format");
            if (info.format.empty()) {
                std::string::size_type pos = url_.path().rfind(".");
                if (pos != std::string::npos) {
                    info.format = url_.path().substr(pos + 1);
                }
            }
            ec.clear();
            return true;
        }

        bool SingleMedia::get_info(
            MediaInfo & info,
            boost::system::error_code & ec) const
        {
            get_basic_info(info, ec);
            return true;
        }

    } // namespace data
} // namespace ppbox
