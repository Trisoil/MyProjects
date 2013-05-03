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
#undef min
#include <algorithm>

namespace super_node_test
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
        framework::configure::Config conf("SuperNodeServiceTestClient.conf");
        conf.register_module("Request")
            << CONFIG_PARAM_NAME_RDONLY("total_peers", total_peers_)
            << CONFIG_PARAM_NAME_RDONLY("peer_last_time_in_second", peer_last_time_in_second_)
            << CONFIG_PARAM_NAME_RDONLY("peer_send_requests_in_second", peer_send_requests_in_second_)
            << CONFIG_PARAM_NAME_RDONLY("subpieces_in_one_request", subpieces_in_one_request_)
            << CONFIG_PARAM_NAME_RDONLY("segment_degrade_rate", segment_degrade_rate_);

        if (segment_degrade_rate_ <= 1.05f || segment_degrade_rate_ >= 2.0f)
        {
            segment_degrade_rate_ = 1.2f;
            std::cout<<"Invalid segment_degrade_rate (should be within range (1.0, 2.0)). Falling back to use default value " << segment_degrade_rate_ <<std::endl;
        }

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

    boost::shared_ptr<Request> RequestGenerator::GenerateGetSubpiecesRequestForPeer(int peer_id, unsigned short& subpiece_id)
    {
        std::vector<SubPieceInfo> subpiece_infos;

        const static unsigned int max_block_id = 3;

        int block_id = rand()% max_block_id;

        std::map<int, std::pair<boost::shared_ptr<std::string>, size_t> >::iterator iter = peer_request_resources_.find(peer_id);

        if (iter != peer_request_resources_.end())
        {
            int block_num = std::min(iter->second.second >> 21, max_block_id);

            block_id = block_num ?  rand() % block_num : 0;
        }

        for (boost::uint8_t i = 0; i < subpieces_in_one_request_; ++i)
        {
            SubPieceInfo subpiece_info(block_id, subpiece_id++);
            subpiece_infos.push_back(subpiece_info);
        }
        return boost::shared_ptr<Request>(new GetSubpiecesRequest(peer_id, simple_rid_, *(peer_request_resources_[peer_id]).first,
            subpiece_infos));
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
                    boost::shared_ptr<Request> request = GenerateGetSubpiecesRequestForPeer(peer_id, peer_request_subpiece_ids_[peer_id]);
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
                    boost::shared_ptr<Request> request = GenerateGetSubpiecesRequestForPeer(peer_id, peer_request_subpiece_ids_[peer_id]);
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

    size_t GetSegmentIndex(size_t resource_size, double degrade_rate)
    {
        assert(degrade_rate > 1.0f && degrade_rate < 2.0f);

        const size_t BytesPerSegment = 20*1024*1024;
        //服务器的分段不是准确地根据20MB来做的，所以下面计算有点保守(0.6)，以免计算出来的segment num超过实际segment数而导致500错误。
        size_t segments_num = (resource_size + 0.6*BytesPerSegment - 1)/BytesPerSegment;

        if (segments_num <= 1)
        {
            return 0;
        }

        //(a^n - 1)/(a-1)
        double sum = (pow(degrade_rate, (int)segments_num) - 1)/(degrade_rate - 1);

        double rand_value = (rand()%10000)/10000.0;
        double adjusted_rand_value = rand_value*sum;
        for(size_t segment_index = 0; segment_index < segments_num; ++segment_index)
        {
            double current_segment_threshold = pow(degrade_rate, (int)(segments_num - segment_index - 1));
            if (adjusted_rand_value <= current_segment_threshold)
            {
                return segment_index;
            }

            adjusted_rand_value -= current_segment_threshold;
        }

        assert(false);
        return 0;
    }

    void RequestGenerator::GenerateNewPeers()
    {
        for (int peer_id = last_peer_id_ + 1; peer_id <= last_peer_id_ + new_peers_every_second_; ++peer_id)
        {
            Resource selected_resource = resource_selector_->SelectResource();
            std::string resource_name = selected_resource.GetResourceName();

            std::ostringstream resource_segment;
            size_t segment_index = GetSegmentIndex(selected_resource.GetResourceSize(), segment_degrade_rate_);
            assert(resource_name.rfind(".mp4") == resource_name.size() - 4);
            resource_segment<<resource_name.substr(0, resource_name.length() - 4)<<'['<<segment_index<<"].mp4";

            peer_request_resources_.insert( std::make_pair(peer_id,
                    std::make_pair(boost::shared_ptr<std::string>(new std::string(resource_segment.str())),
                        std::min(selected_resource.GetResourceSize() - segment_index * (20 << 20), 20U << 20)
                    )
                )
            );
            peer_request_subpiece_ids_.insert(std::make_pair(peer_id, 0));
        }
        last_peer_id_ += new_peers_every_second_;
    }

    void RequestGenerator::DeleteOldPeers()
    {
        for (int peer_id = begin_peer_id_; peer_id < begin_peer_id_ + new_peers_every_second_; ++peer_id)
        {
            peer_request_resources_.erase(peer_id);
            peer_request_subpiece_ids_.erase(peer_id);
        }
        begin_peer_id_ += new_peers_every_second_;
    }
}
