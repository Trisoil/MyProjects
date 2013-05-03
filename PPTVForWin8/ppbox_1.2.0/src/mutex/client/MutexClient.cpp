// MutexClient.cpp

#include "mutex/client/Common.h"
#include "mutex/client/MutexClient.h"
#include "mutex/client/HttpHandle.h"
#include "mutex/client/UdpHandle.h"
#include "mutex/protocol/protocol.h"
using namespace mutex::protocol;
using namespace mutex::client::error;

#include <framework/string/Md5.h>
#include <framework/logger/Section.h>
using namespace framework::string;
using namespace framework::network;

#include <util/protocol/http/HttpClient.h>
#include <util/protocol/http/HttpError.h>
#include <util/archive/ArchiveBuffer.h>
using namespace util::protocol;
using namespace util::protocol::http_error;
using namespace util::archive;

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
using namespace boost::system;
using namespace boost::asio;
using namespace boost::asio::error;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("mutex.client.MutexClient", framework::logger::Debug);

namespace mutex
{
    namespace client
    {

        MutexClient::MutexClient(
            boost::asio::io_service & io_svc)
            : io_svc_(io_svc)
            , keep_alive_timer_(new clock_timer(io_svc))
            , protocol_handle_(NULL)
            , is_first_receive_(false)
            , try_left_(0)
            , is_retry_first_index_(false)
            , not_alive_time_(0)
            , protocol_type_(ProtocolType::UDP)
            , interval_(3)
            , first_mutex_server_index_(0)
            , curr_mutex_server_index_(0)
        {
        }

        MutexClient::~MutexClient()
        {
            if (keep_alive_timer_) {
                delete keep_alive_timer_;
                keep_alive_timer_ = NULL;
            }
            if (protocol_handle_) {
                // delete protocolhandle_;
                protocol_handle_.reset();
            }
        }

        error_code MutexClient::start(void)
        {
            last_error_ = error_code();
            error_code ec;

            //interval_ = 3;
            not_alive_time_ = 0;
            is_retry_first_index_ = false;
            // initial protocol type
            if (is_first_receive_) {
                if (ProtocolType::UDP == protocol_type_) {
                    try_left_ = 6;
                } else if (ProtocolType::HTTP == protocol_type_) {
                    try_left_ = 3;
                }
            } else {
                protocol_type_ = ProtocolType::UDP;
                try_left_ = 3;
            }
            select_mutex_server(ec)
                || timer_start(ec);
            if (!ec) {
                // set connect endpoint
                MutexServerInfo mutex = mutex_servers_.at(curr_mutex_server_index_);
                NetName current_addr;
                current_addr.host(mutex.mutex_server_ip);

                if (protocol_type_ == ProtocolType::HTTP) {
                    protocol_handle_.reset(new HttpHandle(io_svc_));
                    current_addr.port(mutex.http_port);
                } else if (protocol_type_ == ProtocolType::UDP) {
                    protocol_handle_.reset(new UdpHandle(io_svc_));
                    current_addr.port(mutex.udp_port);
                }

                LOG_INFO("[start] first select mutex server: " << current_addr.to_string());

                protocol_handle_->connect(current_addr, ec);
                if (ec) {
                    LOG_ERROR("[start] ec: " << ec.message());
                    state_call_back_(ec);
                    return ec;
                }

                protocol_handle_->set_packet_callback(
                    boost::bind(&MutexClient::receive_callback, this, _1, _2));
            } else {
                LOG_ERROR("[start] ec: " << ec.message());

                state_call_back_(ec);
            }
            return ec;
        }

        error_code MutexClient::stop()
        {
            error_code ec;
            this->leave();
            this->cancel();
            return ec;
        }

        error_code MutexClient::cancel()
        {
            error_code ec;
            keep_alive_timer_->cancel(ec);
            if (protocol_handle_) {
                protocol_handle_->stop();
                protocol_handle_.reset();
            }
            return ec;
        }

        error_code MutexClient::timer_start(
            error_code & ec)
        {
            not_alive_time_ += interval_;
            keep_alive_timer_->expires_from_now(Duration::seconds(interval_));
            keep_alive_timer_->async_wait(boost::bind(&MutexClient::on_timer, this, _1));
            return ec;
        }

        void MutexClient::on_timer(
            error_code const & ec)
        {
            LOG_SECTION();

            if (ec || last_error_) {
                return;
            }

            //10分钟完全没有响应
            if (not_alive_time_ >= 10 * 60) { // 10 minutes
                last_error_ = time_out;
            }
            if (!last_error_)
                send_keep_alive(last_error_);
            if (last_error_) {
                state_call_back_(last_error_);
                LOG_TRACE("[on_timer] error_last_: " << last_error_.message());
                cancel();
                return;
            }
            not_alive_time_ += interval_;
            keep_alive_timer_->expires_from_now(Duration::seconds(interval_));
            keep_alive_timer_->async_wait(boost::bind(&MutexClient::on_timer, this, _1));
        }

        error_code MutexClient::send_keep_alive(
            error_code & ec)
        {
            if (try_left_ == 0) {
                protocol_handle_->stop();
                if (!is_first_receive_) {
                    if (protocol_type_ == ProtocolType::UDP) {
                        // change protocol
                        protocol_type_ = ProtocolType::HTTP;
                    } else if (protocol_type_ == ProtocolType::HTTP) {
                        // change server
                        if (!change_mutex_server(ec)) {
                            protocol_type_ = ProtocolType::UDP;
                        }
                    }
                    try_left_ = 3;
                } else {
                    // change server
                    if (!change_mutex_server(ec)) {
                        if (ProtocolType::UDP == protocol_type_) {
                            try_left_ = 6;
                        } else if (ProtocolType::HTTP == protocol_type_) {
                            try_left_ = 3;
                        }
                    }
                }
                if (!ec) {
                    // set connect endpoint
                    MutexServerInfo mutex = mutex_servers_.at(curr_mutex_server_index_);
                    NetName current_addr;
                    current_addr.host(mutex.mutex_server_ip);

                    if (protocol_type_ == ProtocolType::HTTP) {
                        protocol_handle_.reset(new HttpHandle(io_svc_));
                        current_addr.port(mutex.http_port);
                    } else if (protocol_type_ == ProtocolType::UDP) {
                        protocol_handle_.reset(new UdpHandle(io_svc_));
                        current_addr.port(mutex.udp_port);
                    }

                    LOG_INFO("[send_keep_alive] change server: " << current_addr.host() << ":" << current_addr.port());

                    protocol_handle_->connect(current_addr, ec);
                    protocol_handle_->set_packet_callback(
                        boost::bind(&MutexClient::receive_callback, this, _1, _2));
                }
            }

            if (!ec) {
                KeepaliveRequestPackage keepalivepacket;
                memcpy(keepalivepacket.session_id, session_id_.c_str(), session_id_.size());
                keepalivepacket.time = server_time_;
                memcpy(keepalivepacket.enc, mutex_encrypt_.c_str(), mutex_encrypt_.size());
                keepalivepacket.client_id = client_id_;

                {
                    MutexServerInfo mutex = mutex_servers_.at(curr_mutex_server_index_);
                    NetName current_addr;
                    current_addr.host(mutex.mutex_server_ip);
                    if (protocol_type_ == ProtocolType::HTTP) {
                        current_addr.port(mutex.http_port);
                    } else if (protocol_type_ == ProtocolType::UDP) {
                        current_addr.port(mutex.udp_port);
                    }

                    LOG_TRACE("[send_keep_alive] current server: " << current_addr.host() << ":" << current_addr.port());
                    LOG_TRACE("[send_keep_alive] async_send_packet:" 
                        << " protocol=" << (protocol_type_ == ProtocolType::UDP ? "UDP" : "HTTP") 
                        << " try_left=" << try_left_);
                }
                protocol_handle_->async_send_packet(keepalivepacket);
                try_left_--;
            }
            return ec;
        }

        void MutexClient::receive_callback(
            mutex::protocol::Head const & head, 
            std::streambuf & packet)
        {
            LOG_SECTION();

            IUdpArchive ia(packet);

            LOG_TRACE("[receive_callback] :" 
                << " action=" << (int)head.action);

            if (head.action == KeepaliveResponePackage::action) {
                is_first_receive_ = true;

                // refine bug : try the first mutex server then end
                first_mutex_server_index_ = curr_mutex_server_index_;
                is_retry_first_index_ = false;

                if (ProtocolType::UDP == protocol_type_) {
                    try_left_ = 6;
                } else if (ProtocolType::HTTP == protocol_type_) {
                    try_left_ = 3;
                }
                not_alive_time_ = 0;

                KeepaliveResponePackage keepalive_respone;
                ia >> keepalive_respone;
                if (keepalive_respone.passstate == 0) {
                    if (keepalive_respone.interval < MININTERVAL) {
                        interval_ = MININTERVAL;
                    } else if (keepalive_respone.interval > MAXINTERVAL) {
                        interval_ = MAXINTERVAL;
                    } else {
                        interval_ = keepalive_respone.interval;
                    }
                    state_call_back_(last_error_);
                } else {
                    last_error_ = (error::errors)(keepalive_respone.passstate + response_error_delta);
                    LOG_WARN("[receive_callback] keepalive failed:" 
                        << " ec=" << last_error_.message());
                    state_call_back_(last_error_);
                    this->cancel();
                }
            } else if (head.action == KickoutPackage::action) {
                KickoutPackage packet;
                ia >> packet;

                if (packet.client_id.compare(client_id_) == 0 
                    && !memcmp(session_id_.c_str(), packet.session_id, session_id_.size())) {
                    last_error_ = kickout;
                    LOG_WARN("[receive_callback] kickout failed:" 
                        << " ec=" << last_error_.message());
                    state_call_back_(last_error_);
                    this->cancel();
                }
            } else {
                LOG_WARN("[receive_callback] receive other action:" 
                    << head.action);
            }
        }

        error_code MutexClient::change_mutex_server(
            error_code & ec)
        {
            // refine bug : try the first mutex server then end
            // cycle
            if (is_retry_first_index_) {
                ec = use_all_mutex_server;
                return ec;
            }

            if (curr_mutex_server_index_ >= (mutex_servers_.size() - 1)) {
                curr_mutex_server_index_ = 0;
            } else {
                curr_mutex_server_index_++;
            }

            if (curr_mutex_server_index_ == first_mutex_server_index_) {
                if (!is_first_receive_) {
                    ec = use_all_mutex_server;
                } else {
                    is_retry_first_index_ = true;
                }
            }
            return ec;
        }

        error_code MutexClient::select_mutex_server(
            error_code & ec)
        {
            std::string client_id = client_id_;
            std::string client_id_md5;
            if (!client_id.empty()) {
                Md5 md5;
                md5.update((unsigned char *)&client_id[0], client_id.size());
                md5.final();
                client_id_md5 = md5.to_string();
            }
            boost::uint8_t clientid[CLIENTLENGTH];
            memcpy(clientid, client_id_md5.data(), CLIENTLENGTH);
            boost::uint32_t value = 0;

            boost::uint32_t length = sizeof(boost::uint32_t);
            for (int i = 0; i < CLIENTLENGTH; i = i + length) {
                boost::uint32_t temp_value = 0;
                memcpy(&temp_value, clientid + i, length);
                value += temp_value;
            }

            if (mutex_servers_.size()) {
                first_mutex_server_index_ = value % mutex_servers_.size();
                curr_mutex_server_index_ = first_mutex_server_index_;
            } else {
                ec = no_mutex_server;
            }
            return ec;
        }

        void MutexClient::leave()
        {
            LeavePackage leavepacket;
            memcpy(leavepacket.session_id, session_id_.c_str(), session_id_.size());
            leavepacket.client_id = client_id_;
            if (protocol_handle_) {
                LOG_WARN("[leave] send leave package");
                protocol_handle_->async_send_packet(leavepacket);
            }
        }

        std::string uuidunparse(
            std::string const & uuids)
        {
            std::string res;
            res.resize(16);
            char zero = '0';
            if (uuids.size() == 32) {
                int j = 0;
                for (int i = 0; i < 32; i = i + 2) {
                    int n;
                    if (uuids[i] >= 'a' && uuids[i] <= 'f') {
                        n = uuids[i] - 'a' + 10;
                    } else {
                        n = uuids[i] - zero;
                    }
                    n = n << 4;
                    int p;
                    if (uuids[i + 1] >= 'a' && uuids[i + 1] <= 'f') {
                        p = uuids[i + 1] - 'a' + 10;
                    } else {
                        p = uuids[i + 1] - zero;
                    }
                    res[j] = (char)(n | p);
                    j++;
                }
            }
            return res;
        }

        void MutexClient::set_mutex_info(
            std::string const & client_id,
            std::string const & session_id,
            boost::uint64_t server_time,
            std::vector< ::mutex::client::MutexServerInfo> const & mutex_servers,
            std::string const & mutex_encrypt,
            state_call_back_type const & state_func)
        {
            mutex_servers_ = mutex_servers;
            client_id_ = client_id;
            session_id_ = uuidunparse(session_id);
            server_time_ = server_time;
            mutex_encrypt_ = uuidunparse(mutex_encrypt);
            state_call_back_ = state_func;

            LOG_DEBUG("server_time: " << server_time);
            LOG_DEBUG("client_id: " << client_id);
            LOG_DEBUG("mutex_encrypt: " << mutex_encrypt);
            LOG_DEBUG("session_id: " << session_id);

            for (size_t i = 0; i < mutex_servers_.size(); ++i) {
                LOG_INFO("mutex server ip " << i+1 << " : " << mutex_servers_[i].mutex_server_ip);
            }
        }

    } // namespace client
} // namespace mutex
