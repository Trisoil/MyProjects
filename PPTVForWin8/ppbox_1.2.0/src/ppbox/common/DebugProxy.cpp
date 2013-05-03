// DebugProxy.cpp

#include "ppbox/common/Common.h"
#include "ppbox/common/DebugProxy.h"

#include <ppbox/common/Debuger.h>

#include <util/protocol/http/HttpRequest.h>
#include <util/protocol/http/HttpResponse.h>
#include <util/protocol/http/HttpSocket.h>
using namespace util::protocol;

#include <framework/network/NetName.h>
#include <framework/network/TcpSocket.hpp>
using namespace framework::process;

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
using namespace boost::system;

#ifdef BOOST_WINDOWS_API
#  if (defined UNDER_CE) || (defined __MINGW32__)
#    define localtime_r(x, y) *y = *localtime(x)
#  else 
#    define localtime_r(x, y) localtime_s(y, x)
#  endif
#  define snprintf _snprintf
#endif

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.common.DebugProxy", framework::logger::Debug);

namespace ppbox
{
    namespace common
    {

        DebugProxy::DebugProxy(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<DebugProxy>(daemon, "DebugProxy")
            , debuger_(util::daemon::use_module<Debuger>(daemon))
            , addr_("(tcp)(v4)0.0.0.0:9003")
            , socket_(io_svc())
            , acceptor_(io_svc())
            , work_thread_(NULL)
            , exit_(false)
        {
            config().register_module("DebugProxy")
                << CONFIG_PARAM_NAME_NOACC("addr", addr_);
        }

        DebugProxy::~DebugProxy()
        {
        }

        error_code DebugProxy::startup()
        {
            error_code ec;
            if (acceptor_.open<boost::asio::ip::tcp>(addr_.endpoint(), ec)) {
                ec.clear();
                return ec;
            }
            work_thread_ = new boost::thread(
                boost::bind(&DebugProxy::start, this));

            return ec;
        }

        void DebugProxy::shutdown()
        {
            {
                boost::mutex::scoped_lock lock(mutex_);

                error_code ec;
                exit_ = true;

                util::protocol::HttpSocket client(io_svc());
                framework::network::NetName addr("127.0.0.1", addr_.port());
                client.connect(addr, ec);

                acceptor_.close(ec);

                //boost::asio::detail::socket_ops::shutdown(acceptor_.native(), boost::asio::socket_base::shutdown_both, ec);
                socket_.cancel(ec);
            }

            if (work_thread_) {
                work_thread_->join();
                delete work_thread_;
                work_thread_ = NULL;
            }
        }

        void DebugProxy::start()
        {
            size_t len = 0;
            error_code ec;
            framework::network::NetName addr("(tcp)(v4)0.0.0.0:9003");
            while (!exit_) {
                ec.clear();
                socket_.accept(acceptor_, ec);

                {
                    boost::mutex::scoped_lock lock(mutex_);
                    if (exit_) {
                        ec = boost::asio::error::operation_aborted;
                    }
                }

                if (ec) {
                    //std::cout << "start accept close:ec" << std::endl;
                    socket_.close(ec);
                    break;
                }
                //std::cout << "start accept ok" << std::endl;

                util::protocol::HttpRequest request;
                len = socket_.read(request.head(), ec);

                if (ec) {
                    //std::cout << "read close" << std::endl;

                    socket_.close(ec);
                    break;
                }

                request.head().get_content(std::cout);
                std::string::size_type p = request.head()["User-Agent"].value().find("Firefox");
                if (p == std::string::npos) {
                    std::string s_buf = "Please user Firefox to check log!";
                    len = s_buf.length();
                    util::protocol::HttpResponse response;
                    response.head().version = 0x101;
                    response.head().set_field("Content-Type", "{text/plain}");
                    response.head().set_field("Content-Length", boost::lexical_cast<std::string>(len));
                    response.head().connection = util::protocol::http_field::Connection::close;

                    socket_.write(response.head(), ec);
                    if (ec) {
                        //std::cout << "write head close" << std::endl;

                        socket_.close(ec);
                        break;
                    }

                    boost::asio::write(
                        socket_, 
                        boost::asio::buffer(&s_buf[0], len), 
                        boost::asio::transfer_all(), 
                        ec);
                    socket_.close(ec);

                    continue;
                }

                debuger_.change_debug_mode(true);

                util::protocol::HttpResponse response;
                response.head().version = 0x101;
                response.head().set_field("Content-Type", "{text/plain}");
                //response.head().set_field("Transfer-Encoding", "{chunked}");
                response.head().connection = util::protocol::http_field::Connection::keep_alive;

                response.head().get_content(std::cout);

                socket_.write(response.head(), ec);
                if (ec) {
                    //std::cout << "write head close" << std::endl;

                    socket_.close(ec);
                    debuger_.change_debug_mode(false);
                    break;
                }

                while (!exit_) {
                    std::vector<Message> msgs;
                    std::string s_buf;

                    debuger_.get_debug_msg(msgs, 1, NULL, 5);

                    size_t size = msgs.size();
                    if (size == 0) {
                        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
                        continue;
                    }

                    for (size_t i = 0; i < size; ++i) {
                        time_t tt = msgs[i].time;
                        struct tm lt;
                        char time[40];
                        localtime_r(&tt, &lt);
                        strftime(time, sizeof(time), "%Y-%m-%d %H:%M:%S", &lt);

                        char szbuf[1024] = { 0 };
                        snprintf(szbuf, sizeof(szbuf), "[%s] [%s] %s", time, msgs[i].sender.c_str(), msgs[i].data.c_str());

                        s_buf += szbuf;
                    }

                    len = s_buf.length();

                    boost::asio::write(
                        socket_, 
                        boost::asio::buffer(&s_buf[0], len), 
                        boost::asio::transfer_all(), 
                        ec);
                    if (ec) {
                        std::cout << "write data close" << std::endl;
                        socket_.close(ec);
                        debuger_.change_debug_mode(false);
                        break;
                    }
                }
            }
        }

    } // namespace common
} // namespace ppbox
