/******************************************************************************
*
* Copyright (c) 2011 PPLive Inc.  All rights reserved
* 
* TrackerClient.h
* 
* Description: 和tracker进行交互的命令
*             
* 
* --------------------
* 2011-08-29, kelvinchen create
* --------------------
******************************************************************************/
#ifndef __PPLIVE_TRACKER_CLIENT_H_CK_20110829__
#define __PPLIVE_TRACKER_CLIENT_H_CK_20110829__

#include "TrackerPack.h"

namespace ns_pplive{
    namespace ns_tracker{

        class TrackerClient{
        public:
            int ListPeer(std::vector<CandidatePeerInfo>& candidate_peer_info,boost::uint32_t transaction_id,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,
                boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout);           
            int ListPeerTcp(std::vector<CandidatePeerInfo>& candidate_peer_info,boost::uint32_t transaction_id,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,
                boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout);	
            int ListPeerTcpWithIp(std::vector<CandidatePeerInfo>& candidate_peer_info,boost::uint32_t transaction_id,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,
                unsigned request_ip,boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout);	
            int ListPeerWithIp(std::vector<CandidatePeerInfo>& candidate_peer_info,boost::uint32_t transaction_id,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,
                unsigned request_ip,boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout);
			int ListPeerWithIp(std::string& candidate_peer_string,boost::uint32_t transaction_id,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,
				unsigned request_ip,boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout);


            //参数太多，用起来有点麻烦，可以再封装一个简单的,不过暂时没有强烈需求推动
            int ReportPeer(boost::uint16_t& keepalive_interval,	boost::uint32_t& detected_ip,boost::uint16_t& detected_udp_port,boost::uint16_t& resource_count,
                boost::uint32_t transaction_id,const Guid& peer_guid,boost::uint16_t local_resource_count,	boost::uint16_t server_resource_count,
                boost::uint16_t udp_port,boost::uint32_t stun_peer_ip,boost::uint16_t stun_peer_udpport,boost::uint32_t stun_detected_ip,boost::uint16_t stun_detected_udp_port,const std::vector<boost::uint32_t>& real_ips,
                const std::vector<REPORT_RESOURCE_STRUCT>& resource_ids,boost::uint8_t nat_type,boost::uint8_t upload_priority,boost::uint8_t idle_time_in_mins,
                boost::int32_t upload_bandwidth_kbs,boost::int32_t upload_limit_kbs,boost::int32_t upload_speed_kbs,boost::asio::ip::udp::endpoint endpoint_,boost::asio::io_service& ios, double timeout);

            int Leave(boost::uint32_t transaction_id,const Guid& peer_guid,const boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout);

            int QueryPeerCount(boost::uint32_t& peer_count,boost::uint32_t transaction_id,const RID& resource_id,const boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout);
            int QueryPeerResources(std::vector<RID>& resources,boost::uint32_t transaction_id,const Guid& peer_id,const boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout);

            int QueryTrackerStatistic(std::string& tracker_statistic,boost::uint32_t transaction_id,const boost::asio::ip::udp::endpoint& endpoint_,boost::asio::io_service& ios, double timeout);

            int InternalCommand(boost::uint32_t transaction_id, 
                const boost::asio::ip::udp::endpoint& endpoint_, 
                boost::uint16_t cmd, boost::uint32_t arg,
                boost::asio::io_service& ios, double timeout);
        public:
            TrackerClient();
            ~TrackerClient();

        private:
            TrackerPack 			pack_;
            ostringstream			err_msg_;

        public:
            string GetErrMsg();

            void SetTraceLog(bool trace);

        };
    };
};

#endif /* __PPLIVE_TRACKER_CLIENT_H_CK_20110829__ */

