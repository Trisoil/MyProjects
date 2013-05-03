#ifndef FRAMEWORK_NETWORK_RESOLVER_H
#define FRAMEWORK_NETWORK_RESOLVER_H

#include <boost/asio/ip/tcp.hpp>

#include <string>
#include <framework/timer/AsioTimerManager.h>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <framework/timer/Timer.h>

using namespace boost;
using namespace std;

namespace network
{
    struct IResolverListener
    {
        typedef boost::shared_ptr<IResolverListener> p;

        virtual void OnResolverSucced(uint32_t ip, boost::uint16_t port) = 0;
        virtual void OnResolverFailed(uint32_t error_code) = 0;

        virtual ~IResolverListener()
        {}
    };

    class Resolver
        : public boost::noncopyable
        , public boost::enable_shared_from_this<Resolver>
#ifdef DUMP_OBJECT
        , public count_object_allocate<Resolver>
#endif
    {
    public:
        typedef boost::shared_ptr<Resolver> p;
        static p create(boost::asio::io_service & io_svc, framework::timer::AsioTimerManager & timer_manager , string const & url, boost::uint16_t port, IResolverListener::p handler);
    public:
        Resolver(boost::asio::io_service & io_svc, framework::timer::AsioTimerManager & timer_manager , string const & url, boost::uint16_t port, IResolverListener::p handler);
		
    public:
        void Close();
        void DoResolver();
    public:
        void SetResolverTimeout(uint32_t resolver_timeout)
        {
            resolver_timeout_ = resolver_timeout;
        }
    private:
        void HandleResolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator ep_itr);
        void OnTimerElapsed(framework::timer::Timer * pointer);
        void HandleResolveTimeout();
    private:
        boost::asio::ip::tcp::endpoint endpoint_;
        boost::asio::ip::tcp::resolver resolver_;
        string url_;
        boost::uint16_t  port_;
        framework::timer::OnceTimer resolver_timer_;
        uint32_t  resolver_timeout_;
        IResolverListener::p handler_;
        // ״̬
        bool is_resolving_;
        uint32_t  failed_times_;
    };
}

#endif//FRAMEWORK_NETWORK_RESOLVER_H
