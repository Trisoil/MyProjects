// AliveClient.cpp

#include "ppbox/certify/Common.h"
#include "ppbox/certify/AliveClient.h"
#include "ppbox/certify/AuthStatus.h"
#include "ppbox/certify/KeepaliveCommand.h"
using namespace ppbox::certify::error;

#include <util/protocol/http/HttpClient.h>
#include <util/protocol/http/HttpSocket.hpp>
using namespace util::protocol;

#include <framework/string/Format.h>
#include <framework/logger/StreamRecord.h>
#include <framework/logger/Section.h>
using namespace framework::network;
using namespace framework::string;

#include <boost/asio/read_until.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
using namespace boost::system;
using namespace boost::asio;
using namespace boost::asio::error;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.certify.AliveClient", framework::logger::Debug);

Time g_time = Time::now();

namespace ppbox
{
    namespace certify
    {

        static inline std::string addr_host(
            NetName const & addr)
        {
            return addr.host() + ":" + addr.svc();
        }

        AliveClient::AliveClient(
            boost::asio::io_service & io_svc)
            : http_(new HttpClient(io_svc))
            , timer_(new clock_timer(io_svc))
            , last_time_(Time::now())
            , addr_("(v4)127.0.0.1:9002")
        {
        }

        AliveClient::~AliveClient()
        {
            error_code ec;
            if (timer_) {
                delete timer_;
                timer_ = NULL;
            }
            if (http_) {
                http_->close(ec);
                delete http_;
                http_ = NULL;
            }
        }

        void AliveClient::set_port(boost::uint16_t port)
        {
            addr_.port(port);
        }

        void AliveClient::async_keepalive(
            KeepaliveRequest const & keepalive_request, 
            certify_response_type const & resp)
        {
            certify_response_ = resp;
            HttpRequest http_request;

            http_request.head().method = HttpRequestHead::get;
            http_request.head().path = "/keepalive";
            http_request.head()["Accept"] = "{*/*}";
            http_request.head().host.reset(addr_host(addr_));

            OHttpArchive oa(http_request.data());
            oa << keepalive_request;

            last_time_ = Time::now();

            http_->async_open(http_request,
                boost::bind(&AliveClient::handle_keepalive, this, _1));

            timer_->expires_from_now(Duration::seconds(TIMEOUT));
            timer_->async_wait(boost::bind(&AliveClient::on_keepalive_timer, this, _1));
        }

        void AliveClient::handle_keepalive(
            error_code const & ec)
        {
            LOG_SECTION();

            Time now_time = Time::now();

            LOG_TRACE("[handle_keepalive] ec = " << ec.message() 
                << " last_time = " << (last_time_ - g_time).total_milliseconds()
                << ", now_time = " << (now_time - g_time).total_milliseconds());

            KeepaliveResponse response;
            if (last_time_ + Duration::seconds(TIMEOUT) < now_time) {
                LOG_WARN("[handle_keepalive] time out, " 
                    << " last_time = " << (last_time_ - g_time).total_milliseconds()
                    << ", now_time = " << (now_time - g_time).total_milliseconds());
                response.error_value = boost::asio::error::timed_out;
                response.status = AuthStatus::time_out;
            }

            if (!response.error_value) {
                response.error_value = ec;

                if (response.error_value) {
                    LOG_WARN("[handle_keepalive] response error: " << response.error_value.message());
                    response.error_value = error::alive_dead;
                    response.status = AuthStatus::time_out;
                }
            }

            if (!response.error_value) {
                if (http_->response().data().size()) {
                    parse_keepalive_response(response);
                } else {
                    response.status = AuthStatus::authing;
                }
            }

            if (response.error_value) {
                error_code ec1;
                http_->close(ec1);
                timer_->cancel(ec1);

                certify_response_type resp;
                resp.swap(certify_response_);
                resp(response);
            } else {
                last_time_ = Time::now();
                boost::asio::async_read_until(*http_, http_->response().data(),
                    "\r\n\r\n", boost::bind(&AliveClient::handle_keepalive, this, _1));
                certify_response_(response);
            }
        }

        void AliveClient::on_keepalive_timer(
            error_code const & ec)
        {
            LOG_SECTION();

            Time now_time = Time::now();
            if (ec || last_time_ + Duration::seconds(TIMEOUT) < now_time) {
                if (last_time_ + Duration::seconds(TIMEOUT) < now_time) {
                    LOG_WARN("[on_keepalive_timer] time out, " 
                        << " last_time = " << (last_time_ - g_time).total_milliseconds()
                        << ", now_time = " << (now_time - g_time).total_milliseconds());
                    error_code ec1;
                    http_->cancel(ec1);
                }
                return;
            }

            timer_->expires_from_now(Duration::seconds(TIMEOUT));
            timer_->async_wait(boost::bind(&AliveClient::on_keepalive_timer, this, _1));
        }

        void AliveClient::parse_keepalive_response(
            KeepaliveResponse & response)
        {
            IHttpArchive ia(http_->response().data());
            std::string tail_str;
            ia >> response;
            ia >> tail_str;
            http_->response().clear_data();
            if (!ia) {
                LOG_WARN("[parse_keepalive_response] !ia");
                response.error_value = response_error;
                response.status = AuthStatus::failed;
            }
        }

        void AliveClient::cancel()
        {
            error_code ec;
            http_->cancel(ec);

            timer_->cancel(ec);
        }

    } // namespace certify
} // namespace ppbox
