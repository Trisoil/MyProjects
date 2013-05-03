// ProtocolHandle.h

#ifndef _MUTEX_CLIENT_PROTOCOL_HANDLE_H_
#define _MUTEX_CLIENT_PROTOCOL_HANDLE_H_

#include "mutex/protocol/protocol.h"
using namespace mutex::protocol;

#include <util/RefenceFromThis.h>

#include <boost/function.hpp>

namespace framework
{
    namespace network
    {
        class NetName;
    }
}

namespace mutex
{
    namespace client
    {

        typedef boost::function<void (
            mutex::protocol::Head const &, 
            std::streambuf &)> recv_call_back_type;

        class ProtocolHandle
            : public util::RefenceFromThis<ProtocolHandle>
        {
        public:
            ProtocolHandle(void);

            virtual ~ProtocolHandle(void);

        public:
            template <typename PacketType>
            bool async_send_packet(
                PacketType const packet)
            {
                std::streambuf & send_buffer = create_packet();
                if (!&send_buffer) {
                    return false;
                }

                OUdpArchive oa(send_buffer);

                Head head;
                head.checksum = 0;
                head.reverse = 0;
                ++sequece_;
                ++sequece_;
                head.sequece = sequece_;
                head.version = 1;
                head.action = PacketType::action;

                oa << head;
                oa << packet;

                if (oa) {
                    send_packet_buffer(send_buffer);
                }
                return oa;
            }

            void set_packet_callback(
                recv_call_back_type const & recv_call_back)
            {
                recv_call_back_ = recv_call_back;
            }

            void stop(void)
            {
                is_running_ = false;
                do_stop();
            }

        public:
            virtual boost::system::error_code connect(
                framework::network::NetName const & addr,
                boost::system::error_code & ec) = 0;

        protected:
            virtual std::streambuf & create_packet() = 0;

            virtual void delete_packet(
                std::streambuf & packet) = 0;

            virtual void do_send_packet(
                std::streambuf & packet) = 0;

            virtual void do_stop() = 0;

        protected:
            void on_recv_packet(
                std::streambuf & packet,
                protocol::Head const & head)
            {
                if (is_running_) {
                    if ((head.sequece == sequece_ || (head.sequece & 1) == 0))
                        recv_call_back_(head, packet);
                }
            }

        private:
            void send_packet_buffer(
                std::streambuf & packet)
            {
                //assert(is_running_);
                do_send_packet(packet);
            }

        private:
            bool is_running_;
            recv_call_back_type recv_call_back_;
            boost::uint16_t sequece_;
        };

    } // namespace client
} // namespace mutex

#endif // _MUTEX_CLIENT_PROTOCOL_HANDLE_H_
