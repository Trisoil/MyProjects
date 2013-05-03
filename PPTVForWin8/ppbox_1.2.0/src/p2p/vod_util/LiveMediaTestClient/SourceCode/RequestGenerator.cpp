//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "Request.h"
#include "RequestGenerator.h"
#include "RequestsQueue.h"
#include "ResourceSelector.h"
#include <cmath>
#include <framework/configure/Config.h>

namespace live_media_test
{
    RequestGenerator::RequestGenerator(boost::shared_ptr<boost::asio::io_service> io_service, boost::shared_ptr<RequestsQueue> request_queue,
        boost::shared_ptr<ResourceSelector> resource_selector)
        : io_service_(io_service), request_queue_(request_queue), resource_selector_(resource_selector)
        , begin_peer_id_(0), last_peer_id_(-1)
    {
        timer_.reset(new boost::asio::deadline_timer(*io_service_));
        simple_rid_.from_string("simple_rid");
    }

    void RequestGenerator::Start()
    {
        framework::configure::Config conf("LiveMediaServiceTestClient.conf");

        conf.register_module("Request")
            << CONFIG_PARAM_NAME_RDONLY("total_peers", total_peers_)
            << CONFIG_PARAM_NAME_RDONLY("peer_last_time_in_second", peer_last_time_in_second_)
            << CONFIG_PARAM_NAME_RDONLY("peer_send_requests_in_second", peer_send_requests_in_second_)
            << CONFIG_PARAM_NAME_RDONLY("subpieces_in_one_request", subpieces_in_one_request_)
            << CONFIG_PARAM_NAME_RDONLY("announce_interval_in_seconds", announce_interval_in_seconds_);

        assert(peer_last_time_in_second_ > 0);
        new_peers_every_second_ = total_peers_ / peer_last_time_in_second_;

        timer_->expires_from_now(boost::posix_time::seconds(1));
        timer_->async_wait(boost::bind(&RequestGenerator::GenerateRequests, this));
    }

    void RequestGenerator::Stop()
    {
        timer_->cancel();
    }

    void RequestGenerator::GenerateRequests()
    {
        if (resource_selector_->IsStarted())
        {
            GenerateNewPeers();
            GenerateConnectRequestsForNewPeers();
            GenerateAnnounceRequests();
            GenerateGetSubpiecesRequest();

            if (last_peer_id_ + 1 >= total_peers_)
            {
                GenerateCloseSessionRequest();
                DeleteOldPeers();
            }
        }

        timer_->expires_from_now(boost::posix_time::seconds(1));
        timer_->async_wait(boost::bind(&RequestGenerator::GenerateRequests, this));
    }

    void RequestGenerator::GenerateConnectRequestsForNewPeers()
    {
        if (last_peer_id_ >= new_peers_every_second_)
        {
            boost::shared_ptr<std::vector<boost::shared_ptr<Request> > > connect_requests_from_new_peers(new std::vector<boost::shared_ptr<Request> >);
            for(int peer_id = last_peer_id_ + 1 - new_peers_every_second_;
                peer_id <= last_peer_id_;
                ++peer_id)
            {
                boost::shared_ptr<Request> request(new ConnectRequest(peer_id, peer_request_resources_[peer_id].peer_guid, peer_request_resources_[peer_id].channel_id));
                connect_requests_from_new_peers->push_back(request);
            }

            request_queue_->Push(connect_requests_from_new_peers);
        }
    }

    void RequestGenerator::GenerateAnnounceRequests()
    {
        if (announce_interval_in_seconds_ <= 0)
        {
            return;
        }

        boost::shared_ptr<std::vector<boost::shared_ptr<Request> > > announce_requests(new std::vector<boost::shared_ptr<Request> >);

        for (int peer_id = begin_peer_id_; peer_id <= last_peer_id_ - new_peers_every_second_; ++peer_id)
        {
            if (rand()%announce_interval_in_seconds_ != 0)
            {
                continue;
            }

            PeerDownloadInfo& peer_download_info = peer_request_resources_[peer_id];
            boost::shared_ptr<Request> request(new AnnouceRequest(peer_id, peer_download_info.channel_id, peer_download_info.block_id));
            announce_requests->push_back(request);
        }

        request_queue_->Push(announce_requests);
    }

    boost::shared_ptr<Request> RequestGenerator::GenerateGetSubpiecesRequestForPeer(int peer_id, PeerDownloadInfo& peer_download_info)
    {
        std::vector<LiveSubPieceInfo> subpiece_infos;

        int block_id = peer_download_info.block_id;
        size_t subpiece_id = peer_download_info.subpiece_id++;
        for (boost::uint8_t i = 0; i < subpieces_in_one_request_; ++i)
        {
            LiveSubPieceInfo subpiece_info(block_id, subpiece_id++);
            subpiece_infos.push_back(subpiece_info);
        }

        return boost::shared_ptr<Request>(new GetSubpiecesRequest(peer_id, peer_request_resources_[peer_id].channel_id, subpiece_infos));
    }

    void RequestGenerator::GenerateGetSubpiecesRequest()
    {
        boost::shared_ptr<std::vector<boost::shared_ptr<Request> > > requests(new std::vector<boost::shared_ptr<Request> >);

        if (last_peer_id_ >= new_peers_every_second_)
        {
            for (boost::uint8_t i = 0; i < peer_send_requests_in_second_; ++i)
            {
                for (int peer_id = begin_peer_id_; peer_id <= last_peer_id_ - new_peers_every_second_; ++peer_id)
                {
                    boost::shared_ptr<Request> request = GenerateGetSubpiecesRequestForPeer(peer_id, peer_request_resources_[peer_id]);
                    requests->push_back(request);
                }
            }
        }

        if (last_peer_id_ >= new_peers_every_second_-1)
        {
            std::vector<boost::shared_ptr<Request> > requests_from_new_peers;
            for(int peer_id = last_peer_id_ + 1 - new_peers_every_second_;
                peer_id <= last_peer_id_;
                ++peer_id)
            {
                for (boost::uint8_t i = 0; i < peer_send_requests_in_second_; ++i)
                {
                    boost::shared_ptr<Request> request = GenerateGetSubpiecesRequestForPeer(peer_id, peer_request_resources_[peer_id]);
                    requests_from_new_peers.push_back(request);
                }
            }

            if (requests_from_new_peers.size() > 0)
            {
                const size_t BatchCount = 20;
                size_t batch_size = (requests_from_new_peers.size() + BatchCount - 1)/BatchCount;
                size_t old_requests_batch_size = (requests->size() + BatchCount - 1)/BatchCount;

                for(size_t batch_index = 0; batch_index < BatchCount; ++batch_index)
                {
                    std::vector<boost::shared_ptr<Request> >::iterator batch_begin = requests_from_new_peers.begin();
                    std::vector<boost::shared_ptr<Request> >::iterator batch_end = 
                        requests_from_new_peers.size() <= batch_size ? 
                        requests_from_new_peers.end() :
                        batch_begin + batch_size;

                    std::vector<boost::shared_ptr<Request> >::iterator insert_pos = requests->begin()+batch_index*(batch_size + old_requests_batch_size);
                    requests->insert(insert_pos, batch_begin, batch_end);
                    
                    std::vector<boost::shared_ptr<Request> >::iterator shuffle_begin = requests->begin()+batch_index*(batch_size + old_requests_batch_size);
                    std::vector<boost::shared_ptr<Request> >::iterator shuffle_end = 
                        (requests->end() - shuffle_begin) > (batch_size + old_requests_batch_size) ?
                        (shuffle_begin + batch_size + old_requests_batch_size) :
                        requests->end();
                    std::random_shuffle(shuffle_begin, shuffle_end);

                    requests_from_new_peers.erase(batch_begin, batch_end);
                    if (requests_from_new_peers.size() == 0)
                    {
                        break;
                    }
                }
            }
        }

        request_queue_->Push(requests);
    }

    void RequestGenerator::GenerateCloseSessionRequest()
    {
        boost::shared_ptr<std::vector<boost::shared_ptr<Request> > > requests(new std::vector<boost::shared_ptr<Request> >);
        for (int peer_id = begin_peer_id_; peer_id < begin_peer_id_ + new_peers_every_second_; ++peer_id)
        {
            // 1%的几率不发CloseSession
            if (peer_id % 100 == 0)
            {
                continue;
            }

            boost::shared_ptr<Request> request(new CloseSessionRequest(peer_id));
            requests->push_back(request);
        }
        request_queue_->Push(requests);
    }

    void RequestGenerator::GenerateNewPeers()
    {
        for (int peer_id = last_peer_id_ + 1; peer_id <= last_peer_id_ + new_peers_every_second_; ++peer_id)
        {
            Resource selected_resource = resource_selector_->SelectResource();

            PeerDownloadInfo peer_download_info;
            size_t position = ::time(0) - RequestGenerator::RequestDelayInSeconds;
            peer_download_info.block_id = position - (position%BlockIntervalInSeconds);
            peer_download_info.subpiece_id = 0;
            peer_download_info.channel_id = RID(selected_resource.GetChannelId());
            peer_download_info.peer_guid.generate();

            peer_request_resources_.insert(std::make_pair(peer_id,peer_download_info));
        }
        last_peer_id_ += new_peers_every_second_;
    }

    void RequestGenerator::DeleteOldPeers()
    {
        for (int peer_id = begin_peer_id_; peer_id < begin_peer_id_ + new_peers_every_second_; ++peer_id)
        {
            peer_request_resources_.erase(peer_id);
        }
        begin_peer_id_ += new_peers_every_second_;
    }
}