// HttpHandle.cpp

#include "mutex/client/Common.h"
#include "mutex/client/HttpHandle.h"

#include <util/protocol/http/HttpClient.h>
#include <util/protocol/http/HttpRequest.h>
#include <util/protocol/http/HttpResponse.h>
#include <util/protocol/http/HttpError.h>
#include <util/buffers/BufferCopy.h>
#include <util/protocol/pptv/CheckSum.h>
using namespace util::protocol;
using namespace util::protocol::http_error;
using namespace util::protocol::pptv;

#include <framework/network/NetName.h>
#include <framework/logger/Logger.h>
#include <framework/logger/Section.h>
using namespace framework::network;

#include <boost/bind.hpp>
using namespace boost::system;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("mutex.client.HttpHandle", framework::logger::Warn);

namespace mutex
{
    namespace client
    {

        HttpHandle::HttpHandle(
            boost::asio::io_service & io_svc)
            : http_(new HttpClient(io_svc))
            , request_(new HttpRequest())
            , is_busy_(false)
            , is_pending_(false)
            , is_received_(false)
        {
        }

        HttpHandle::~HttpHandle(void)
        {
            if (request_) {
                delete request_;
                request_ = NULL;
            }
            if (http_) {
                delete http_;
                http_ = NULL;
            }
        }

        std::streambuf & HttpHandle::create_packet()
        {
            request_->data().reset();
            return request_->data();
        }

        void HttpHandle::delete_packet(
            std::streambuf & packet)
        {
            // delete &packet;
        }

        error_code HttpHandle::connect(
            NetName const & addr, 
            error_code & ec)
        {
            http_->bind_host(addr, ec);
            if (ec)
                return ec;

            HttpRequestHead & head = request_->head();
            head.method = HttpRequestHead::post;
            head.connection = util::protocol::http_field::Connection::keep_alive;
            head.path = "/mutex";

            return ec;
        }

        void HttpHandle::do_send_packet(
            std::streambuf & packet)
        {
            error_code ec;
            if (is_busy_) {
                if (!is_received_) {
                    http_->cancel(ec);
                }

                is_pending_ = true;
                return;
            }

            boost::asio::streambuf buf;
            util::buffers::buffer_copy(
                buf.prepare(request_->data().size()), 
                request_->data().data());
            buf.commit(request_->data().size());
            // checksum
            boost::uint16_t const * buffer_checksum = boost::asio::buffer_cast<boost::uint16_t const *>(buf.data());
            boost::uint16_t checkvalue = check_sum(buffer_checksum + 1, buf.size() - 2);
            boost::uint16_t * buffer_checksum_1 = const_cast<boost::uint16_t *>(
                boost::asio::buffer_cast<boost::uint16_t const *>(
                request_->data().data()));
            memcpy(buffer_checksum_1, &checkvalue, 2);

            LOG_TRACE("[do_send_packet] checksum: " << checkvalue);

            http_->async_fetch(*request_, 
                boost::bind(&HttpHandle::handle_receive, ref(this), _1));
            request_->data().consume(request_->data().size());
            is_busy_ = true;
        }

        void HttpHandle::do_stop()
        {
            error_code ec;
            is_pending_ = false;
            if (http_) {
                http_->cancel(ec);
            }
        }

        void HttpHandle::handle_receive(
            boost::system::error_code const & ec)
        {
            LOG_SECTION();

            if (!ec) {
                boost::asio::streambuf buf;
                util::buffers::buffer_copy(
                    buf.prepare(http_->response().data().size()), 
                    http_->response().data().data());
                buf.commit(http_->response().data().size());
                // checksum
                boost::uint16_t const * buffer_checksum = boost::asio::buffer_cast<boost::uint16_t const *>(buf.data());
                boost::uint16_t checkvalue = check_sum(buffer_checksum + 1, buf.size() - 2);

                if (checkvalue == *buffer_checksum) {
                    //LOG_TRACE("[handle_receive] checksum: " << checkvalue);
                    IUdpArchive ia(http_->response().data());
                    protocol::Head head;
                    ia >> head;
                    if (ia) {
                        on_recv_packet(http_->response().data(), head);
                    }
                } else {
                    LOG_WARN("[handle_receive] error package" << checkvalue);
                    LOG_WARN("[handle_receive] real checksum: " << *buffer_checksum);
                }
                is_received_ = true;
            }

            if (is_pending_) {
                http_->async_fetch(*request_, 
                    boost::bind(&HttpHandle::handle_receive, ref(this), _1));
                is_pending_ = false;
            } else {
                is_busy_ = false;
            }
        }

    } // namespace client
} // namespace mutex
