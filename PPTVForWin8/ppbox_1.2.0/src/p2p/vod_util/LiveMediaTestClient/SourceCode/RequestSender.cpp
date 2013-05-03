//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "Request.h"
#include "RequestSender.h"
#include "RequestsQueue.h"
#include "PeerSessions.h"
#include <framework/configure/Config.h>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#ifndef BOOST_WINDOWS_API
#include <time.h>
#endif

namespace live_media_test
{
    RequestSender::RequestSender(boost::shared_ptr<boost::asio::io_service> io_service, boost::shared_ptr<RequestsQueue> request_queue)
        : request_queue_(request_queue)
    {
        peer_sessions_.reset(new PeerSessions(io_service));
    }

    void RequestSender::Start()
    {
        if (!LoadServiceEndpoint())
        {
            std::cout<<"ERROR: The service endpoint setting is invalid."<<std::endl;
        }
        else
        {
            running_ = true;
            thread_.reset(new boost::thread(boost::bind(&RequestSender::SendRequests, shared_from_this())));
        }
    }

    void RequestSender::Stop()
    {
        running_ = false;
        thread_->join();
    }

    bool RequestSender::LoadServiceEndpoint()
    {
        string service_address_list;
        size_t service_port(0);
        framework::configure::Config conf("LiveMediaServiceTestClient.conf");
        conf.register_module("Service")
            << CONFIG_PARAM_NAME_RDONLY("service_address", service_address_list)
            << CONFIG_PARAM_NAME_RDONLY("service_port", service_port);

        if (service_address_list.size() == 0 || service_port == 0)
        {
            return false;
        }

        std::vector<string> server_addresses;
        boost::algorithm::split(server_addresses, service_address_list, boost::algorithm::is_any_of(","));

        if (server_addresses.size() == 0)
        {
            return false;
        }

        for(size_t i = 0; i < server_addresses.size(); ++i)
        {
            boost::system::error_code error;
            boost::asio::ip::address_v4 server_address_v4(boost::asio::ip::address_v4::from_string(server_addresses[i], error));
            if (error)
            {
                return false;
            }

            service_endpoints_.push_back(boost::asio::ip::udp::endpoint(server_address_v4, service_port));
        }

        return true;
    }

    void RequestSender::SendRequests()
    {
#ifdef BOOST_WINDOWS_API
        LARGE_INTEGER perf_frequency={0};
        QueryPerformanceFrequency(&perf_frequency); 
#endif
        while (running_)
        {
            if(request_queue_->Size() > 0)
            {
                while(!request_queue_->IsEmpty())
                {
                    boost::shared_ptr<Requests> requests = request_queue_->Pop();
                    pending_requests_.insert(pending_requests_.end(), requests->begin(), requests->end());
                }

                peer_sessions_->RemoveExpiredSessions();
            }

            if (pending_requests_.size() == 0)
            {
                continue;
            }

            std::cout<<"sending "<<pending_requests_.size()<<" requests..."<<std::endl;
            
#ifdef BOOST_WINDOWS_API
            LARGE_INTEGER start_time={0};
            QueryPerformanceCounter(&start_time);
#else
            timespec start_time;
            clock_gettime(CLOCK_REALTIME, &start_time);
#endif
            size_t count_down = RequestSender::IntervalsPerSecond;
            size_t time_to_elapse = 0;
            while(running_ && count_down > 0)
            {
                time_to_elapse += RequestSender::TimerIntervalInMiliseconds;

#ifdef BOOST_WINDOWS_API
                LARGE_INTEGER now={0};
                do 
                {
                    QueryPerformanceCounter(&now);

                } while (running_ && (now.QuadPart - start_time.QuadPart)*1000 < time_to_elapse*perf_frequency.QuadPart);
#else
                int64_t due_nano_seconds = start_time.tv_nsec + 1000000*time_to_elapse;
                int64_t due_seconds = start_time.tv_sec + due_nano_seconds/1000000000;
                due_nano_seconds %= 1000000000;

                timespec now;
                do 
                {
                    clock_gettime(CLOCK_REALTIME, &now);
                } while (running_ && 
                        (now.tv_sec < due_seconds || 
                         now.tv_sec == due_seconds && now.tv_nsec < due_nano_seconds));
#endif
                size_t requests_to_send = pending_requests_.size()/count_down;

                if (requests_to_send > 0)
                {
                    for(size_t i = 0; i < requests_to_send; ++i)
                    {
                        UdpBuffer send_buffer;
                        boost::uint32_t transaction_id = peer_sessions_->NewTransactionId(pending_requests_[i]->GetPeerId());

                        for(size_t k = 0; k < service_endpoints_.size(); ++k)
                        {
                            pending_requests_[i]->BuildPacketBuffer(send_buffer, service_endpoints_[k], transaction_id);
                            try
                            {
                                peer_sessions_->SendPacket(pending_requests_[i]->GetPeerId(), send_buffer);
                            }
                            catch (...)
                            {
                                std::cout<<"An error occurred while sending packet."<<std::endl;
                            }
                        }
                    }

                    pending_requests_.erase(pending_requests_.begin(), pending_requests_.begin() + requests_to_send);
                }
                
                --count_down;
            }
        }
    }
}