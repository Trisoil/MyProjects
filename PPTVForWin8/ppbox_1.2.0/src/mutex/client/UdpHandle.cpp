// UdpHandle.cpp

#include "mutex/client/Common.h"
#include "mutex/client/UdpHandle.h"

#include <util/protocol/pptv/CheckSum.h>
using namespace util::protocol::pptv;

#include <framework/network/NetName.h>
#include <framework/logger/Section.h>
using namespace framework::network;

#include <boost/bind.hpp>
#include <boost/ref.hpp>
using namespace boost::system;
using namespace boost::asio::ip;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("mutex.client.UdpHandle", framework::logger::Warn);

namespace mutex
{
    namespace client
    {

        UdpHandle::UdpHandle(
            boost::asio::io_service & io_svc)
            : socket_(io_svc)
        {
        }

        UdpHandle::~UdpHandle(void)
        {
            if (socket_.is_open()) {
                socket_.close();
            }
        }

        std::streambuf & UdpHandle::create_packet()
        {
            return *new PacketBuffer<char>();
        }

        void UdpHandle::delete_packet(
            std::streambuf & packet)
        {
            delete &packet;
        }

        error_code UdpHandle::connect(
            NetName const & addr, 
            error_code & ec)
        {
            socket_.open(udp::v4(), ec);
            if (ec)
                return ec;

            socket_.connect(udp::endpoint(address::from_string(addr.host()), addr.port()), ec);
            if (ec)
                return ec;

            socket_.async_receive(recv_buf_.prepare(1024), 
                boost::bind(&UdpHandle::handle_receive, ref(this), _1, _2));

            return ec;
        }

        void UdpHandle::do_send_packet(
            std::streambuf & packet)
        {
            PacketBuffer<char> & buf = (PacketBuffer<char> &)packet;

            // checksum
            boost::uint16_t const * buffer_checksum = /*(boost::uint16_t const *)*/boost::asio::buffer_cast<boost::uint16_t const *>(buf.data());
            boost::uint16_t checkvalue = check_sum(buffer_checksum + 1, buf.size() - 2);
            boost::uint16_t * buffer_checksum_1 = const_cast<boost::uint16_t *>(buffer_checksum);
            memcpy(buffer_checksum_1, &checkvalue, 2);

            LOG_TRACE("[do_send_packet] checksum: " << checkvalue);

            socket_.async_send(buf.data(),
                boost::bind(&UdpHandle::handle_send, ref(this), _1, _2, boost::ref(packet)));
        }

        void UdpHandle::do_stop()
        {
            error_code ec;
            socket_.close(ec);
        }

        void UdpHandle::handle_send(
            error_code const & ec, 
            size_t len,
            std::streambuf & packet)
        {
            LOG_SECTION()

            if (ec) {
            }
            // delete send buffer
            delete &packet;
        }

        void UdpHandle::handle_receive(
            error_code const & ec,
            size_t len)
        {
            LOG_SECTION()

            if (ec) {
                LOG_WARN("[handle_receive] ec= " << ec.message() << ",value: " << ec.value());
                return;
            }

            recv_buf_.commit(len);

            boost::uint16_t const * buffer_checksum = (boost::uint16_t const *)boost::asio::buffer_cast<boost::uint16_t const *>(recv_buf_.data());
            boost::uint16_t checkvalue = check_sum(buffer_checksum + 1, recv_buf_.size() - 2);

            IUdpArchive ia(recv_buf_);
            protocol::Head head;
            ia >> head;

            if (ia && checkvalue == *buffer_checksum) {
                //LOG_TRACE("[handle_receive] checksum: " << checkvalue);
                on_recv_packet(recv_buf_, head);
            } else {
                LOG_WARN("[handle_receive] error package" << checkvalue);
                LOG_WARN("[handle_receive] real checksum: " << *buffer_checksum);
            }

            recv_buf_.consume(len);
            socket_.async_receive(recv_buf_.prepare(1024), 
                boost::bind(&UdpHandle::handle_receive, ref(this), _1, _2));
        }

    } // namespace client
} // namespace mutex
