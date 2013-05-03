/******************************************************************************
*
* Copyright (c) 2011 PPLive Inc.  All rights reserved
* 
* ListRequestInfo.h
* 
* Description: List请求的相关信息          
* 
* --------------------
* 2011-12-12, kelvinchen create
* --------------------
******************************************************************************/

#include "Common.h"
#include "protocol/include/struct/Base.h"


using namespace std;
extern log4cplus::Logger g_logger;

namespace udptrackerag
{
    struct ListRequestInfo
    {
        ListRequestInfo();

        void UpdateMagicNum();        

        static boost::uint64_t GetMagicNum(const RID& resource_id,unsigned transaction_id);        

        //random_shuf表示是否随机选取。如果为true，就会在返回的各个tracker的list结果里，随机选取，否则会把tracker返回结果的前location_percent*request_peer_count排序，其余的用ComplementPeerInfos补足
        int GetPeerInfos(vector<protocol::CandidatePeerInfo>& peer_info,int location_percent,bool random_shuf) const;

        //获取从tracker查询到的peer的数量，未去重
        unsigned GetResponsePeerCount() const;      

        //查询还有多少个response没有收到
        unsigned GetLeftResponse();

        //会改变传入的参数，以减少copy
        void AddPeerInfo(const boost::asio::ip::udp::endpoint& end_point,vector<protocol::CandidatePeerInfo>& peer_infos);

        //发送查询到tracker的时间,精度是tick(毫秒)
        boost::uint64_t send_request_time;
        RID  resource_id;
        //list包回复给end_point。这个可能是tracker_proxy或者peer
        boost::asio::ip::udp::endpoint end_point;
        unsigned short request_peer_count;
        boost::uint8_t action;

        //记录从tracker查询到的结果
        map<boost::asio::ip::udp::endpoint,vector<protocol::CandidatePeerInfo> > tracker_response;
        //序列号
        unsigned transaction_id; 

        //发给几个tracker等待回包，当tracker_response的key个数等于query_tracker_num的个数，就说明tracker都回复了。
        unsigned query_tracker_num;

        //魔数，唯一标识一个查询的对象,只能通过调用setmagicnum来设置
        boost::uint64_t magic_num;  

        //client的版本，要记录下来，回复的时候给客户端。
        uint16_t peer_version;

    private:
        //补足peer,vector里已经有的数据，是高优先级的数据，这里需要补足的是随机的数据，目前采取的是从返回结果的后端查找填充。
        //调用这个函数的前提是peer_info里已经是高优先级的数据了。
        void ComplementPeerInfos(vector<protocol::CandidatePeerInfo>& peer_info) const;
    };
}