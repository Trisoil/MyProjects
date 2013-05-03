// HttpServer.h

#ifndef _MUTEX_SERVER_HTTP_SERVER_H_
#define _MUTEX_SERVER_HTTP_SERVER_H_

#include "mutex/server/Common.h"
#include "mutex/protocol/protocol.h"
#include "mutex/server/MutexSession.h"

#include <util/Util.h>
#include <util/archive/ArchiveBuffer.h>
#include <util/protocol/http/HttpProxyManager.h>
#include <util/protocol/http/HttpProxy.h>
#include <util/protocol/http/HttpRequest.h>
#include <util/protocol/http/HttpResponse.h>
#include <util/stream/StreamTransfer.h>
using namespace util::protocol;
using namespace util::stream;
using namespace util::archive;

#include <framework/configure/Config.h>
#include <framework/logger/Logger.h>
#include <framework/logger/LoggerSection.h>
#include <framework/logger/LoggerFormatRecord.h>
#include <framework/network/NetName.h>
using namespace framework::network;
using namespace framework::configure;
using namespace framework::logger;

#include <boost/shared_array.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio/io_service.hpp>
using namespace boost::system;
using namespace boost::asio;
using namespace boost::asio::ip;

#include <iostream>

namespace mutexserver
{

    class HttpServer
        : public HttpProxy
    {
    public:
        HttpServer(
            boost::asio::io_service & io_svc)
            : HttpProxy(io_svc)
            , send_buf_(new char[1024])
            , contentLength_(0)
            , blocked_(false)
            , isKeepAlive_(false)
            , responseData4Get_("")
            , plainSeted_(false)
        {
        }

        virtual bool on_receive_request_head(
            HttpRequestHead & request_head);

        virtual void on_receive_response_head(
            HttpResponseHead & response_head);

        virtual void on_receive_request_data(
            boost::asio::streambuf & request_data);

        virtual void on_receive_response_data(
            boost::asio::streambuf & response_data);

        virtual void on_error(
            boost::system::error_code const & ec);

        virtual void local_process(
            local_process_response_type const & resp);

        void return_now(
            bool pass);

    private:
        boost::asio::streambuf response_buffer_;
        local_process_response_type resp_;
        boost::shared_array<char> send_buf_;
        protocol::Head responseHead_;
        protocol::KeepaliveResponePackage response_;
        size_t contentLength_;
        std::string sessionID_;
        std::string responseData4Get_;
        bool isKeepAlive_;
        bool blocked_;
        bool plainSeted_;
    };
}

#endif // _MUTEX_SERVER_HTTP_SERVER_H_
