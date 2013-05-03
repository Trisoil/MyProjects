// RtmpSession.cpp

#include "ppbox/rtmpd/Common.h"
#include "ppbox/rtmpd/RtmpDispatcher.h"
#include "ppbox/rtmpd/RtmpSink.h"

#include <util/protocol/rtmp/RtmpError.h>
#include <util/protocol/rtmp/RtmpMessage.h>
#include <util/stream/TcpSocket.h>
using namespace util::protocol;

#include <framework/system/LogicError.h>
#include <framework/string/Base16.h>
#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
using namespace framework::string;

#include <boost/bind.hpp>
#include <boost/asio/write.hpp>
using namespace boost::system;

#include <fstream>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.rtmpd.RtmpDispatcher", framework::logger::Debug)

namespace ppbox
{
    namespace rtmpd
    {
        RtmpDispatcher::RtmpDispatcher(
            ppbox::dispatch::DispatcherBase & dispatcher)
            : ppbox::dispatch::CustomDispatcher(dispatcher)
            , sink_(NULL)
        {
        }

        RtmpDispatcher::~RtmpDispatcher()
        {
        }

        void RtmpDispatcher::async_open_play(
            RtmpSocket & socket, 
            framework::string::Url & url, 
            ppbox::dispatch::response_t const & seek_resp, 
            ppbox::dispatch::response_t  const & resp)
        {
            url.param(ppbox::dispatch::param_format, "rtm");
            CustomDispatcher::async_open(url, 
                boost::bind(&RtmpDispatcher::handle_open, this, boost::ref(socket), seek_resp, resp, _1));
        }

        bool RtmpDispatcher::teardown(
            boost::system::error_code & ec)
        {
            delete sink_;
            sink_ = NULL;
            ec.clear();
            return true;
        }

        void RtmpDispatcher::handle_open(
            RtmpSocket & socket, 
            ppbox::dispatch::response_t const & seek_resp, 
            ppbox::dispatch::response_t const & resp, 
            boost::system::error_code ec)
        {
            if (!ec) {
                sink_ = new RtmpSink(socket);
                if (CustomDispatcher::setup(-1, *sink_, ec)) {
                    ppbox::dispatch::SeekRange range;
                    CustomDispatcher::async_play(range, seek_resp, resp);
                    return;
                }
            }

            seek_resp(ec);
            resp(ec);
        }

    } // namespace rtmpd
} // namespace ppbox
