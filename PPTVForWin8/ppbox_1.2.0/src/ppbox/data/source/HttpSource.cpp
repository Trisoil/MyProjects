#include "ppbox/data/Common.h"
#include "ppbox/data/source/HttpSource.h"

#include <util/protocol/http/HttpError.h>
#include <util/protocol/http/HttpSocket.hpp>

#include <boost/asio/read.hpp>
#include <boost/asio/buffer.hpp>

#include <framework/string/Url.h>
#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/logger/StringRecord.h>

namespace ppbox
{
    namespace data
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.data.HttpSource", framework::logger::Debug);

        HttpSource::HttpSource(
            boost::asio::io_service & io_svc)
            : SourceBase(io_svc)
            , http_(io_svc)
        {
        }

        HttpSource::~HttpSource() {}

        boost::system::error_code HttpSource::open(
            framework::string::Url const & url,
            boost::uint64_t beg, 
            boost::uint64_t end, 
            boost::system::error_code & ec)
        {
            flag_ = true;

            util::protocol::HttpRequest request;
            util::protocol::HttpRequestHead & head = request.head();
            head.path = url.path_all();
            head["Accept"] = "{*.*}";
            head.host = url.host_svc();
            head.connection = util::protocol::http_field::Connection::keep_alive;
            if (beg != 0 || end != (boost::uint64_t)-1) {
                head.range.reset(util::protocol::http_field::Range((boost::int64_t)beg, (boost::int64_t)end));
            } else {
                head.range.reset();
            }
            std::ostringstream oss;
            head.get_content(oss);
            LOG_STR(framework::logger::Trace, ("http_request_head", oss.str()));

            http_.open(request, ec);

            return ec;
        }

        void HttpSource::async_open(
            framework::string::Url const & url,
            boost::uint64_t beg, 
            boost::uint64_t end, 
            response_type const & resp)
        {
            flag_ = true;

            boost::system::error_code ec;
            util::protocol::HttpRequest request;
            util::protocol::HttpRequestHead & head = request.head();
            head.path = url.path_all();
            head["Accept"] = "{*.*}";
            head.host = url.host_svc();
            head.connection = util::protocol::http_field::Connection::keep_alive;
            if (beg != 0 || end != (boost::uint64_t)-1) {
                head.range.reset(util::protocol::http_field::Range((boost::int64_t)beg, (boost::int64_t)end));
            } else {
                head.range.reset();
            }
            std::ostringstream oss;
            head.get_content(oss);
            LOG_STR(framework::logger::Trace, ("http_request_head", oss.str()));

            http_.async_open(request, resp);
        }

        bool HttpSource::is_open(
            boost::system::error_code & ec)
        {
            bool result = http_.is_open(ec);
            if(flag_ && result){
                util::protocol::HttpResponseHead head = http_.response().head();
                std::ostringstream oss;
                head.get_content(oss);
                LOG_STR(framework::logger::Trace, ("http_response_head", oss.str()));
                flag_ = false;
            }
            return result;
        }

        boost::system::error_code HttpSource::cancel(
            boost::system::error_code & ec)
        {
            return http_.cancel_forever(ec);
        }

        boost::system::error_code HttpSource::close(
            boost::system::error_code & ec)
        {
            return http_.close(ec);
        }

        std::size_t HttpSource::private_read_some(
            util::stream::StreamMutableBuffers const & buffers, 
            boost::system::error_code & ec)
        {
            assert(http_.is_open(ec));
            return http_.read_some(buffers, ec);
        }

        void HttpSource::private_async_read_some(
            util::stream::StreamMutableBuffers const & buffers, 
            util::stream::StreamHandler const & handler)
        {
            boost::system::error_code ec;
            (void)ec;
            assert(http_.is_open(ec));
            http_.async_read_some(buffers, handler);
        }

        boost::uint64_t HttpSource::total(
            boost::system::error_code & ec)
        {
            boost::uint64_t n = 0;
            if (http_.is_open(ec)) {
                if (http_.response_head().content_range.is_initialized()) {
                    n = http_.response_head().content_range.get().total();
                } else if (http_.response_head().content_length.is_initialized()) {
                    n = http_.response_head().content_length.get();
                } else{
                    ec = framework::system::logic_error::no_data;
                }
            }

            return n;
        }

        boost::system::error_code HttpSource::set_non_block(
            bool non_block, 
            boost::system::error_code & ec)
        {
            return http_.set_non_block(non_block, ec);
        }

        boost::system::error_code HttpSource::set_time_out(
            boost::uint32_t time_out, 
            boost::system::error_code & ec)
        {
            return http_.set_time_out(time_out, ec);
        }

        bool HttpSource::continuable(
            boost::system::error_code const & ec)
        {
            return ec == boost::asio::error::would_block;
        }

        bool HttpSource::recoverable(
            boost::system::error_code const & ec)
        {
            return util::protocol::HttpClient::recoverable(ec);
        }

    } // namespace data
} // namespace ppbox
