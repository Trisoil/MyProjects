// StatusProxy.cpp

#include "ppbox/peer_worker/Common.h"
#include "ppbox/peer_worker/StatusProxy.h"
#include "ppbox/peer_worker/SharedStatus.h"

#include <util/protocol/http/HttpProxy.h>
#include <util/serialization/stl/map.h>
#include <util/archive/LittleEndianBinaryOArchive.h>
#include <util/buffers/BuffersSize.h>
using namespace util::protocol;

#include <framework/network/TcpSocket.hpp>
#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#include <boost/asio/streambuf.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/write.hpp>
using namespace boost::system;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.peer_worker.StatusProxy", framework::logger::Warn)

namespace ppbox
{
    namespace peer_worker
    {

        class StatusSession;

        class StatusProxy::ProxyManager
            : public framework::network::ServerManager<StatusSession, ProxyManager>
            , public SharedStatus
        {
        public:
            ProxyManager(
                boost::asio::io_service & io_svc, 
                StatusProxy & module)
                : framework::network::ServerManager<StatusSession, ProxyManager>(io_svc)
                , module_(module)
            {
            }

        public:
            StatusProxy & module()
            {
                return module_;
            }

        private:
            StatusProxy & module_;
        };

        struct MyString
        {
            MyString(std::string const & t)
            : str_(t){}
            template <typename Archive>
            void serialize(Archive & ar)
            {
                ar & util::serialization::make_sized<boost::uint32_t>(str_);
            }
            std::string str_;
        };


        class StatusProxy::StatusSession
            : public util::protocol::HttpProxy
        {
        public:
            StatusSession(
                ProxyManager & mgr)
                : util::protocol::HttpProxy(mgr.io_svc())
                , mgr_(mgr)
            {
            }

        public:
            virtual bool on_receive_request_head(
                HttpRequestHead & request_head)
            {
                return false;
            }

            virtual void local_process(
                response_type const & resp)
            {
                size_t len = 0;
                error_code ec = mgr_.get_memory_blocks(blocks_);
                if (!ec) {
                    std::ostream os(&head_buf_);
                    util::archive::LittleEndianBinaryOArchive<> oa(os);
                    oa << (boost::uint32_t)blocks_.size();
                    for (std::map<std::string, SharedStatus::Block>::const_iterator iter1 = blocks_.begin();
                         iter1 != blocks_.end(); ++iter1) {
                        oa << MyString(iter1->first);
                        oa << iter1->second;
                    }
                    buffers_.push_back(head_buf_.data());
                    for (std::map<std::string, SharedStatus::Block>::const_iterator iter = blocks_.begin(); 
                        iter != blocks_.end(); ++iter) {
                            buffers_.push_back(boost::asio::const_buffer(iter->second.ptr, iter->second.size));
                    }
                    len = util::buffers::buffers_size(buffers_);
                }
                resp(ec, len);
            }

            virtual void transfer_response_data(
                response_type const & resp)
            {
                if (buffers_.empty()) {
                    resp(error_code(), std::make_pair(size_t(0), size_t(0)));
                    return;
                }
                boost::asio::async_write(get_client_data_stream(), buffers_, 
                    boost::bind(&StatusSession::handle_transfer_response_data, this, 
#if (!defined WINRT) && (!defined WIN_PHONE)
                        resp, _1, boost::bind(std::make_pair<size_t, size_t>, _2, _2)));
#else
                        resp, _1, boost::bind(std::make_pair<size_t &, size_t &>, _2, _2)));
#endif
            }

            void handle_transfer_response_data(
                response_type const & resp, 
                error_code const & ec, 
                std::pair<size_t, size_t> const & bytes_transferred)
            {
                buffers_.clear();
                head_buf_.reset();
                blocks_.clear();
                resp(ec, bytes_transferred);
            }

            virtual void on_error(
                boost::system::error_code const & ec)
            {
                LOG_DEBUG("[on_error] " << ec.message());
                if (ec == boost::asio::error::address_in_use)
                    mgr_.module().get_daemon().post_stop();
            }

        private:
            ProxyManager & mgr_;
            boost::asio::streambuf head_buf_;
            std::vector<boost::asio::const_buffer> buffers_;
            std::map<std::string, SharedStatus::Block> blocks_;
        };

        StatusProxy::StatusProxy(
            util::daemon::Daemon & daemon)
            : util::daemon::ModuleBase<StatusProxy>(daemon, "StatusProxy")
            , addr_("(v4)0.0.0.0:1802")
        {
            config().register_module("StatusProxy")
                << CONFIG_PARAM_NAME_NOACC("addr", addr_);
            mgr_ = new ProxyManager(io_svc(), *this);
        }

        StatusProxy::~StatusProxy()
        {
            delete mgr_;
        }

        error_code StatusProxy::startup()
        {
            error_code ec;
            mgr_->start(addr_, ec);
            ec.clear();
            return ec;
        }

        void StatusProxy::shutdown()
        {
            mgr_->stop();
        }

    } // namespace peer_worker
} // namespace ppbox
