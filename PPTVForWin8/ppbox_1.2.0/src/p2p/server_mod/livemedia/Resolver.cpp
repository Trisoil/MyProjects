//#include "Common.h"

#include "pre.h"
#include "network/Resolver.h"

#include <framework/logger/Logger.h>

#include <boost/asio/placeholders.hpp>


namespace network
{

    Resolver::Resolver(boost::asio::io_service & io_svc, framework::timer::AsioTimerManager & timer_manager, string const & url, boost::uint16_t port, IResolverListener::p handler)
        : resolver_(io_svc)
        , url_(url)
        , port_(port)
        , resolver_timer_(timer_manager, 30 * 1000, boost::bind(&Resolver::OnTimerElapsed, this, &resolver_timer_))
        , resolver_timeout_(30 * 1000)
        , handler_(handler)
        , is_resolving_(false)
        , failed_times_(0)

    {
    }

    Resolver::p Resolver::create(boost::asio::io_service & io_svc, framework::timer::AsioTimerManager & timer_manager, string const & url, boost::uint16_t port, IResolverListener::p handler)
    {
        return Resolver::p(new Resolver(io_svc, timer_manager, url, port, handler));
    }

    void Resolver::Close()
    {
        resolver_.cancel();
        resolver_timer_.stop();
        if (handler_)
        {
            //handler_.reset();
        }
    }

    void Resolver::DoResolver()
    {
        if (true == is_resolving_)
            return;
        is_resolving_ = true;

        string domain = url_;
        if (domain == "")
        { // ���?��handler_���� Url ������Ϣ
            if (handler_)
            {
                handler_->OnResolverFailed(1);
            }
            Close();
        }

        boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), domain, string("80"));
        resolver_.async_resolve(query, boost::bind(&Resolver::HandleResolve, shared_from_this(),
            boost::asio::placeholders::error, boost::asio::placeholders::iterator));
        resolver_timer_.start();
    }

    void Resolver::OnTimerElapsed(framework::timer::Timer * pointer)
    {
        if (false == is_resolving_)
            return;

        if (pointer == &resolver_timer_)
        {
            HandleResolveTimeout();
            resolver_timer_.stop();
        }
        else
            assert(0);
    }

    void Resolver::HandleResolveTimeout()
    {
        if (false == is_resolving_)
            return;

        LOG4CPLUS_INFO(Loggers::Service(),  "Resolver::HandleResolveTimeout() failed times:"<<failed_times_<<url_);

        is_resolving_ = false;

        if (failed_times_ >= 0)
        {
            if (handler_)
            {
                handler_->OnResolverFailed(4);
            }
        }
        else
        {
            failed_times_++;
            DoResolver();
        }
    }

    void Resolver::HandleResolve(const boost::system::error_code& err,
        boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
    {
        if (false == is_resolving_)
            return;

        is_resolving_ = false;

        resolver_timer_.stop();

        if (!err)
        {
            endpoint_ = *endpoint_iterator;
            LOG4CPLUS_INFO(Loggers::Service(), "HandleResolveSucced "<< endpoint_<<url_);
            if (handler_)
            {
                handler_->OnResolverSucced(endpoint_.address().to_v4().to_ulong(), port_);
            }
        }
        else if (err == boost::asio::error::operation_aborted)
        {
        }
        else
        { // ���?��handler_���� �������ʧ�� ������Ϣ
            if (handler_)
            {
                LOG4CPLUS_INFO(Loggers::Service(), "HandleResolveFailed"<< url_);
                handler_->OnResolverFailed(2);
            }
            Close();
        }
    }

}
