/******************************************************************************
*
* Copyright (c) 2011 PPLive Inc.  All rights reserved
* 
* TrackerRequestHandler.h
* 
* Description: 处理到tracker发送的请求，调用者无需关心trakcer的分布和hash情况。          
* 
* --------------------
* 2011-11-21, kelvinchen create
* --------------------
******************************************************************************/

#ifndef TRACKER_REQUEST_HANDLER_H_CK_20111121
#define TRACKER_REQUEST_HANDLER_H_CK_20111121
#include "Common.h"
#include "protocol/Protocol.h"
#include <util/archive/LittleEndianBinaryIArchive.h>
#include <util/archive/LittleEndianBinaryOArchive.h>
#include "protocol/CheckSum.h"
#include <util/buffers/SubBuffers.h>
#include <set>

using namespace std;
extern log4cplus::Logger g_logger;

namespace udptrackerag
{
    class TrackerRequestHandler
    {
    public:
        static TrackerRequestHandler* Instance();        
        TrackerRequestHandler();

        void InitConfigValue();

        //第一次使用的时候，需要setudpserver，才能正常发送数据
        void SetUdpServer(boost::shared_ptr<protocol::UdpServer> );

        //向tracker发送查询请求。需要查询的是tcp还是udp，可以从packet里的action里看出来，另外，packet里包含一些其他的基本查询信息
        //返回值为正数，表示查询的tracker的个数。负数表示失败。0表示没有任何对应的tracker的信息
        int TrackerListRequest(const RID& resource_id,const Guid& peer_id,unsigned short request_peer_count,const protocol::ServerPacket &packet,unsigned request_ip);

        //在set里随机选择require_num个，如果不够就全部返回
        static void SetRandomSelect(set<boost::asio::ip::udp::endpoint>& selectset,int require_num,unsigned seed=0);


        map<boost::asio::ip::udp::endpoint,int> ClearListEndRequest();


    private:

        //ListPackType有两种可能，一种是listwithip，一种是listtcpwithip
        template <typename ListPackType>
        int TrackerListRequest(ListPackType& list_request)
        {
            LOG4CPLUS_TRACE(g_logger,"TrackerListRequest");
            set<boost::asio::ip::udp::endpoint> trackers;
            GetListTrackers(trackers,list_request.request.resource_id_);
            for (set<boost::asio::ip::udp::endpoint>::iterator it = trackers.begin();it != trackers.end();++it)
            {
                ++list_endpoint_request_[*it];
                SendRequest(*it,list_request);
            }
            return trackers.size();
        }

        void GetListTrackers(set<boost::asio::ip::udp::endpoint>& trackers,const RID& resource_id);

        template <typename ListPackType>
        void SendRequest(const boost::asio::ip::udp::endpoint& dst_endpoint,ListPackType& list_request)
        {
            LOG4CPLUS_TRACE(g_logger,"SendRequest");
            list_request.end_point = dst_endpoint;
            list_request.peer_version_ = protocol::PEER_VERSION;               
            udp_server_->send_packet( list_request , list_request.peer_version_);
        }   

    private:
        static TrackerRequestHandler* s_instance;
        //protocol::UdpServer::pointer udp_server_;
        boost::shared_ptr<protocol::UdpServer> udp_server_;
        map<int,set<boost::asio::ip::udp::endpoint> >  tracker_group_;

        //统计每个tracker发送的次数
        map<boost::asio::ip::udp::endpoint,int> list_endpoint_request_;

        //每次最多请求多少个tracker
        int max_req_num_;
	   
		//每次最少请求多少个tracker
		int min_req_num_;

        //每次list最大请求的<Peer>个数
        int max_list_num_;
    };
}


#endif //TRACKER_REQUEST_HANDLER_H_CK_20111121