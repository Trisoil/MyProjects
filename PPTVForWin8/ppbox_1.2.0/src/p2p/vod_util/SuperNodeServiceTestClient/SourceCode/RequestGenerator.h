//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef SUPER_NODE_TEST_REQUEST_GENERATOR_H
#define SUPER_NODE_TEST_REQUEST_GENERATOR_H

namespace super_node_test
{
    class Request;
    class RequestsQueue;
    class ResourceSelector;

    class RequestGenerator
    {
    public:
        RequestGenerator(boost::shared_ptr<boost::asio::io_service> io_service, boost::shared_ptr<RequestsQueue> request_queue,
            boost::shared_ptr<ResourceSelector> resource_selector);

        void Start();
        void Stop();

    private:
        void GenerateRequests();
        void GenerateGetSubpiecesRequest();
        boost::shared_ptr<Request> GenerateGetSubpiecesRequestForPeer(int peer_id, unsigned short& subpiece_id);
        void GenerateCloseSessionRequest();
        void GenerateNewPeers();
        void DeleteOldPeers();

    private:
        boost::shared_ptr<boost::asio::io_service> io_service_;
        boost::shared_ptr<RequestsQueue> request_queue_;
        boost::shared_ptr<ResourceSelector> resource_selector_;
        boost::shared_ptr<boost::asio::deadline_timer> timer_;
        int begin_peer_id_;
        int last_peer_id_;
        std::map<int, std::pair<boost::shared_ptr<std::string>, size_t> > peer_request_resources_;
        std::map<int, boost::uint16_t> peer_request_subpiece_ids_;
        RID simple_rid_;
        int total_peers_;
        int peer_last_time_in_second_;
        int new_peers_every_second_;
        int peer_send_requests_in_second_;
        int subpieces_in_one_request_;
        double segment_degrade_rate_;
    };
}

#endif //SUPER_NODE_TEST_REQUEST_GENERATOR_H