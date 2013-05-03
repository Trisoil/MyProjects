// MmspSession.cpp

#include "ppbox/mmspd/Common.h"
#include "ppbox/mmspd/MmspDispatcher.h"

#include <util/protocol/mmsp/MmspError.h>
#include <util/protocol/mmsp/MmspMacToViewerMessage.h>
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

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.mmspd.MmspDispatcher", framework::logger::Debug)

namespace ppbox
{
    namespace mmspd
    {
        MmspDispatcher::MmspDispatcher(
            ppbox::dispatch::DispatcherBase & dispatcher)
            : ppbox::dispatch::CustomDispatcher(dispatcher)
            , sink_(NULL)
        {
        }

        MmspDispatcher::~MmspDispatcher()
        {
        }

        void MmspDispatcher::async_open(
            framework::string::Url & url, 
            MmspDataReportOpenFile & rsp, 
            ppbox::dispatch::response_t  const & resp)
        {
            url.param(ppbox::dispatch::param_format, "mms");
            url.param("mux.Asf.packet_length", "1024");
            CustomDispatcher::async_open(url, 
                boost::bind(&MmspDispatcher::handle_open, this, boost::ref(rsp), resp, _1));
        }

        bool MmspDispatcher::setup(
            boost::asio::ip::tcp::socket & mmsp_sock, 
            std::string const & transport, 
            boost::system::error_code & ec)
        {
            util::stream::Sink * sink = new util::stream::TcpSocket(mmsp_sock);
            sink_ = new ppbox::dispatch::WrapSink(*sink);
            return CustomDispatcher::setup(-1, *sink_, ec);
        }

        bool MmspDispatcher::write_header(
            boost::asio::ip::tcp::socket & mmsp_sock, 
            boost::system::error_code & ec)
        {
            boost::asio::write(mmsp_sock, 
                boost::asio::buffer(media_info_.format_data), 
                boost::asio::transfer_all(), 
                ec);
            return !ec;
        }

        void MmspDispatcher::async_play(
            util::protocol::MmspDataStartPlaying & req, 
            ppbox::dispatch::response_t const & seek_resp, 
            ppbox::dispatch::response_t const & resp)
        {
            ppbox::dispatch::SeekRange range;
            return CustomDispatcher::async_play(range, seek_resp, resp);
        }
        
        bool MmspDispatcher::teardown(
            boost::system::error_code & ec)
        {
            delete &sink_->sink();
            delete sink_;
            sink_ = NULL;
            ec.clear();
            return true;
        }

        void MmspDispatcher::handle_open(
            MmspDataReportOpenFile & rsp, 
            ppbox::dispatch::response_t const & resp, 
            boost::system::error_code ec)
        {
            if (!ec) {
                CustomDispatcher::get_media_info(media_info_, ec);
            }

            if(ec) {
                resp(ec);
                return;
            }

            rsp.fileAttributes = 0x01000000; // FILE_ATTRIBUTE_MMS_CANSEEK
            rsp.fileBlocks = media_info_.duration / 1000;
            rsp.filePacketSize = 1024;
            rsp.fileHeaderSize = media_info_.format_data.size() - 8;

            resp(ec);
        }

    } // namespace mmspd
} // namespace ppbox
