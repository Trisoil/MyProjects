// HttpClient.cpp

#include "tools/util_test/Common.h"

#include <util/protocol/http/HttpClient.h>
#include <util/protocol/http/HttpRequest.h>
#include <util/protocol/http/HttpResponse.h>
#include <util/stream/AsyncIStream.h>
using namespace util::protocol;

#include <framework/logger/FormatRecord.h>
#include <framework/network/NetName.h>
using namespace framework::configure;
using namespace framework::logger;
using namespace framework::network;

#include <boost/asio/read.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

using namespace boost::system;
using namespace boost::asio;
using namespace boost::asio::error;
using namespace boost::asio::ip;

streambuf buff;
NetName addr("www.baidu.com",80);
HttpRequestHead head;
util::protocol::HttpClient *http_client = NULL;

FRAMEWORK_LOGGER_DECLARE_MODULE("TestHttpClient");

FILE * fp;
void on_fetch(
    boost::system::error_code const & ec)
{
    http_client->response_head().get_content(std::cout);

    fp = fopen("a.gz", "wb");
    fwrite((void *)boost::asio::detail::buffer_cast_helper(http_client->response().data().data()), http_client->response().data().size(), 1, fp);
    fclose(fp);
    //std::cout << std::hex << &http_client->response().data() << std::endl;
    delete http_client;
}

boost::asio::streambuf rdbuffer;

void on_read(
             boost::system::error_code const & ec,
             size_t bytes_transferred)
{
    if (!ec) {
        fp = fopen("a.ungz", "wb");
        fwrite((void *)boost::asio::detail::buffer_cast_helper(rdbuffer.data()), bytes_transferred, 1, fp);
        fclose(fp);
    }
    delete http_client;
}

void on_open(
              boost::system::error_code const & ec)
{
    if (!ec) {
        util::stream::async_filtering_istream & afi = http_client->get_response_stream();
        afi.async_read_some(rdbuffer.prepare(4541010), boost::bind(on_read, _1, _2));
    }
}

void test_http_client_async(io_service &io_svc)
{
    http_client = new util::protocol::HttpClient(io_svc);
    //std::ostream *os = new std::ostream(&buff);
    //http_client->async_open(
    //    "http://jump.g1d.net/%b0%d7%c9%ab%bb%d1%d1%d4(%b5%da001%bc%af).mp4", 
    //    bind(util::binder::ptr_fun(on_open), _1));
    //error_code ec;
    //http_client->bind_host("jump.g1d.net", ec);
    //head.method = HttpRequestHead::get;
    //head.path = "/%b0%d7%c9%ab%bb%d1%d1%d4(%b5%da001%bc%af).mp4";
    ////head["Accept"] = "{*/*,*/*.xml}";
    //head["Accept-Encoding"] = "{gzip}";
    //head.host = "jump.g1d.net";
    //head.connection = http_field::Connection::keep_alive;
    //std::string host = head["Host"];
    //std::string accept = head["Accept"];
    //head.get_content(std::cout);
    //http_client->async_fetch(head, boost::bind(on_fetch, _1));
    //http://192.168.13.22:80/%BA%A3%D1%F3%B9%FA%D3%EF%B0%E6%28%C0%B6%B9%E2%29.mp4?key=653f2f1cfd475039d0c1607b74ed3b21&type=android&w=1&z=1
    error_code ec;
    http_client->bind_host("192.168.13.22", ec);
    head.method = HttpRequestHead::get;
    head.path = "/%BA%A3%D1%F3%B9%FA%D3%EF%B0%E6%28%C0%B6%B9%E2%29.mp4?key=45e34b05bfc65c2632eb481ec66dea25&type=android&w=1&z=1";
    //head["Accept"] = "{*/*,*/*.xml}";
    head.range.reset(util::protocol::http_field::Range(0, 2541010));
    head["Accept-Encoding"] = "{gzip}";
    head.host = "jump.g1d.net";
    head.connection = http_field::Connection::keep_alive;
    std::string host = head["Host"];
    std::string accept = head["Accept"];
    head.get_content(std::cout);
    //http_client->async_fetch(head, boost::bind(on_fetch, _1));
    http_client->async_open(head, boost::bind(on_open, _1));
}

void test_http_client_non_block(io_service &io_svc)
{
    http_client = new util::protocol::HttpClient(io_svc);
    //std::ostream *os = new std::ostream(&buff);
    //http_client->async_open(
    //    "http://jump.g1d.net/%b0%d7%c9%ab%bb%d1%d1%d4(%b5%da001%bc%af).mp4", 
    //    bind(util::binder::ptr_fun(on_open), _1));
    error_code ec;
    http_client->bind_host("jump.g1d.net", ec);
    head.method = HttpRequestHead::get;
    head.path = "/%b0%d7%c9%ab%bb%d1%d1%d4(%b5%da001%bc%af).mp4";
    head["Accept"] = "{*/*,*/*.xml}";
    head.host = "jump.g1d.net";
    std::string host = head["Host"];
    std::string accept = head["Accept"];
    head.get_content(std::cout);
    //http_client->async_open(head, bind(util::binder::ptr_fun(on_open), _1));
    http_client->set_non_block(true, ec);
    http_client->open(head, ec);
    while (ec == would_block) {
        std::cout << "block\n";
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
        http_client->is_open(ec);
    }
    http_client->response_head().get_content(std::cout);
    streambuf buf;
    boost::asio::read(*http_client, buf, boost::asio::transfer_all(), ec);
    while (ec == would_block) {
        std::cout << "block\n";
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
        boost::asio::read(*http_client, buf, boost::asio::transfer_all(), ec);
    }
    delete http_client;
    std::cout << &buf << std::endl;
}

void test_http_client(Config & conf)
{
    {
        LOG_SECTION()
        LOG_DEBUG(("[test_http_client_async] start"));
        io_service io_svc;
        test_http_client_async(io_svc);
        io_svc.run();
        LOG_DEBUG(("[test_http_client_async] end"));
    }

    //{
    //    LOG_SECTION()
    //    LOG_DEBUG(("[test_http_client_non_block] start"));
    //    io_service io_svc;
    //    test_http_client_non_block(io_svc);
    //    LOG_DEBUG(("[test_http_client_non_block] end"));
    //}
}

static TestRegister tr("http_client", test_http_client);
