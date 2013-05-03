//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_TEST_REQUEST_SENDER_H
#define SUPER_NODE_TEST_REQUEST_SENDER_H

using namespace protocol;

namespace super_node_test
{
    class Request;
    class RequestsQueue;
    class PeerSessions;

    class RequestSender
        : public boost::enable_shared_from_this<RequestSender>
    {
        typedef std::vector<boost::shared_ptr<Request> > Requests;

    public:
        RequestSender(boost::shared_ptr<boost::asio::io_service> io_service, boost::shared_ptr<RequestsQueue> request_queue);

        void Start();

        void Stop();

    private:
        void SendRequests();
        bool LoadServiceEndpoint();

    private:
        static const size_t TimerIntervalInMiliseconds = 1;
        static const size_t IntervalsPerSecond = 1000/TimerIntervalInMiliseconds;

        Requests pending_requests_;
        size_t count_down_;

        bool running_;
        boost::shared_ptr<boost::thread> thread_;
        std::vector<boost::asio::ip::udp::endpoint> service_endpoints_;
        boost::shared_ptr<boost::asio::io_service> io_service_;
        boost::shared_ptr<RequestsQueue> request_queue_;

        boost::shared_ptr<PeerSessions> peer_sessions_;
        std::vector<size_t> sent_requests_count_;
    };
}

#endif //SUPER_NODE_TEST_REQUEST_SENDER_H
