/******************************************************************************
*
* Copyright (c) 2011 PPLive Inc.  All rights reserved
* 
* TrackerApi.h
* 
* Description: 和tracker进行交互的命令
*             
* 
* --------------------
* 2011-09-08, kelvinchen create
* --------------------
******************************************************************************/
#ifndef __PPLIVE_TRACKER_API_H_CK_20110908__
#define __PPLIVE_TRACKER_API_H_CK_20110908__

#include "TrackerClient.h"
#include "framework/logger/Logger.h"
#include <framework/logger/LoggerListRecord.h>
#include <framework/logger/LoggerFormatRecord.h>
#include <framework/logger/LoggerStreamRecord.h>

namespace ns_pplive{
    namespace ns_tracker{

        class TrackerApi{
        public:
            int ListPeer(std::vector<CandidatePeerInfo>& candidate_peer_info,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,
                boost::asio::io_service& ios,double timeout=0.0);	
            int ListPeerTcp(std::vector<CandidatePeerInfo>& candidate_peer_info,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,
                boost::asio::io_service& ios,double timeout=0.0);
            int ListPeerTcpWithIp(std::vector<CandidatePeerInfo>& candidate_peer_info,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,
                unsigned request_ip,boost::asio::io_service& ios,double timeout=0.0);
            //读入tracker的配置文件
            static int ReadTrackerConfig(map<int,set<boost::asio::ip::udp::endpoint> >&  tracker_group);
            TrackerApi();
            ~TrackerApi();

        private:
            //根据传入的action不同，来决定下层调用的clientapi的不同
            int ListPeerCom(std::vector<CandidatePeerInfo>& candidate_peer_info,const RID& resource_id,const Guid& peer_guid,boost::uint16_t request_peer_count,
                boost::asio::io_service& ios,boost::uint8_t action,boost::uint32_t request_ip,double timeout=0.0);
            TrackerClient 			client_;
            ostringstream			err_msg_;
            map<int,set<boost::asio::ip::udp::endpoint> >  tracker_group_;

        public:
            void SetTraceLog(bool trace);

        };
    };
};

#endif /* __PPLIVE_TRACKER_API_H_CK_20110908__ */

