//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef LIVE_MEDIA_TEST_PEER_SESSIONS_H
#define LIVE_MEDIA_TEST_PEER_SESSIONS_H

#include <framework/timer/TickCounter.h>

using namespace protocol;

namespace live_media_test
{
    class PeerSessions
    {
        class ClientSession
        {
        public:
            ClientSession(boost::shared_ptr<boost::asio::io_service> io_service)
                : socket_(*io_service), transaction_id_(0)
            {
                socket_.open(boost::asio::ip::udp::v4());
            }

            void Send(const UdpBuffer& send_buffer)
            {
                socket_.send_to(send_buffer.data(), send_buffer.end_point());
                time_counter_.reset();
            }

            bool Expires() const
            {
                return time_counter_.elapse() > 3*1000;
            }

            boost::uint32_t NewTransactionId()
            {
                return ++transaction_id_;
            }

        private:
            framework::timer::TimeCounter time_counter_;
            boost::asio::ip::udp::socket socket_;
            boost::uint32_t transaction_id_;
        };

    public:
        PeerSessions(boost::shared_ptr<boost::asio::io_service> io_service);
        void RemoveExpiredSessions();
        void SendPacket(int peer_id, const UdpBuffer& send_buffer);
        boost::uint32_t NewTransactionId(int peer_id);

    private:
        std::map<int, boost::shared_ptr<ClientSession> > active_sessions_;
        boost::shared_ptr<boost::asio::io_service> io_service_;
    };
}

#endif //LIVE_MEDIA_TEST_PEER_SESSIONS_H