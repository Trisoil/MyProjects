//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef LIVE_MEDIA_TEST_REQUEST_GENERATOR_H
#define LIVE_MEDIA_TEST_REQUEST_GENERATOR_H

namespace live_media_test
{
    class Request;
    class RequestsQueue;
    class ResourceSelector;

    struct PeerDownloadInfo
    {
        RID channel_id;
        Guid peer_guid;

        size_t block_id;
        size_t subpiece_id;
    };

    class RequestGenerator
    {
    public:
        RequestGenerator(boost::shared_ptr<boost::asio::io_service> io_service, boost::shared_ptr<RequestsQueue> request_queue,
            boost::shared_ptr<ResourceSelector> resource_selector);

        void Start();
        void Stop();

    private:
        void GenerateRequests();
        void GenerateConnectRequestsForNewPeers();
        void GenerateAnnounceRequests();
        void GenerateGetSubpiecesRequest();
        boost::shared_ptr<Request> GenerateGetSubpiecesRequestForPeer(int peer_id, PeerDownloadInfo& peer_download_info);
        void GenerateCloseSessionRequest();
        void GenerateNewPeers();
        void DeleteOldPeers();

    private:
        const static size_t BlockIntervalInSeconds = 5;
        const static size_t RequestDelayInSeconds = 45;

        boost::shared_ptr<boost::asio::io_service> io_service_;
        boost::shared_ptr<RequestsQueue> request_queue_;
        boost::shared_ptr<ResourceSelector> resource_selector_;
        boost::shared_ptr<boost::asio::deadline_timer> timer_;
        int begin_peer_id_;
        int last_peer_id_;
        std::map<int, PeerDownloadInfo> peer_request_resources_;
        RID simple_rid_;
        int total_peers_;
        int peer_last_time_in_second_;
        int new_peers_every_second_;
        int peer_send_requests_in_second_;
        int subpieces_in_one_request_;
        int announce_interval_in_seconds_;
    };
}

#endif //LIVE_MEDIA_TEST_REQUEST_GENERATOR_H