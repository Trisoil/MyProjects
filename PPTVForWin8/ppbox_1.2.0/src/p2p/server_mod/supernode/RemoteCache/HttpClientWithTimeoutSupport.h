#ifndef _HTTP_CLIENT_WITH_TIMEOUT_SUPPORT_H
#define _HTTP_CLIENT_WITH_TIMEOUT_SUPPORT_H

#include <util/protocol/http/HttpClient.h>
#include <framework/timer/AsioTimerManager.h>
#include <framework/timer/Timer.h>

namespace util
{
    namespace protocol
    {
        //HttpClient from util library does not support timeout in async mode 
        //bug 6549 is tracking the work item in framework.
        //this wrapper class is added to work around it.
        class HttpClientWithTimeoutSupport
            : public HttpClient,
            public boost::enable_shared_from_this<HttpClientWithTimeoutSupport>
            ,public count_object_allocate<HttpClientWithTimeoutSupport>
        {
        public:
            HttpClientWithTimeoutSupport(boost::asio::io_service & io_svc, boost::shared_ptr<framework::timer::AsioTimerManager> time_manager)
                : HttpClient(io_svc), time_manager_(time_manager)
            {
                pending_response_ = false;
            }

            void async_fetch_result(HttpRequestHead const & head, response_type const & resp)
            {
                //no, concurrent requests are not supported
                assert(!pending_response_);

                boost::system::error_code error;
                boost::uint32_t time_out = get_time_out(error);
                if (!error && time_out > 0)
                {
                    timer_.reset(new framework::timer::OnceTimer(*time_manager_, time_out, boost::bind(&HttpClientWithTimeoutSupport::HandleFetchTimeout, shared_from_this(), resp)));
                    timer_->start();
                }

                HttpClient::async_fetch(head, boost::bind(&HttpClientWithTimeoutSupport::HandleFetchResult, shared_from_this(), _1, resp));

                pending_response_ = true;
            }

        private:
            void HandleFetchTimeout(const response_type& resp)
            {
                if (pending_response_)
                {
                    boost::system::error_code cancel_error;
                    pending_response_ = false;
                    HttpClient::cancel(cancel_error);
                    if (cancel_error)
                    {
                        LOG4CPLUS_WARN(super_node::Loggers::Service(), "Failed to cancel a HTTP request that timed out. Error code:" << cancel_error);
                    }

                    resp(boost::asio::error::timed_out);
                }

                timer_.reset();
            }

            void HandleFetchResult(const boost::system::error_code& error, const response_type& resp)
            {
                if (pending_response_)
                {
                    pending_response_ = false;
                    if (timer_)
                    {
                        timer_->cancel();
                    }
                    resp(error);
                }
                //otherwise the request is already cancelled. no-op.
                timer_.reset();
            }

        private:
            boost::shared_ptr<framework::timer::OnceTimer> timer_;
            boost::shared_ptr<framework::timer::AsioTimerManager> time_manager_;
            bool pending_response_;
        };
    }
}

#endif //_HTTP_CLIENT_WITH_TIMEOUT_SUPPORT_H