/******************************************************************************
*
* Copyright (c) 2011 PPLive Inc.  All rights reserved
* 
* ListRequestCache.h
* 
* Description: 保存已经发送给tracker，等着tracker回复给tracker网关的请求。并处理回复给客户端的逻辑。          
* 
* --------------------
* 2011-11-23, kelvinchen create
* --------------------
******************************************************************************/
#ifndef LISTREQUESTCACHE_H_CK_20111123
#define LISTREQUESTCACHE_H_CK_20111123

#include "Common.h"
#include <framework/timer/Timer.h>
#include <framework/timer/TickCounter.h>
#include <boost/shared_ptr.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "protocol/include/struct/Base.h"
#include "ListRequestInfo.h"


using namespace std;
extern log4cplus::Logger g_logger;

namespace udptrackerag
{
    //按唯一的魔数进行hash，按非唯一的发送时间进行排序。
    typedef boost::multi_index::multi_index_container<
        ListRequestInfo,
        boost::multi_index::indexed_by<		
        boost::multi_index::hashed_unique<		
        BOOST_MULTI_INDEX_MEMBER(ListRequestInfo, boost::uint64_t, magic_num)		
        >,
        boost::multi_index::ordered_non_unique<		
        BOOST_MULTI_INDEX_MEMBER(ListRequestInfo,boost::uint64_t,send_request_time)		
        >
        >
    > ListRequestInfoContainer;

    typedef ListRequestInfoContainer::nth_index<0>::type ListRequestInfoMagicNumHashIndex;
    typedef ListRequestInfoContainer::nth_index<1>::type ListRequestInfoRequestTimeIndex;

    class ListRequestCache
    {        
    public:
        static ListRequestCache * Instance();
          
        unsigned GetRequestCount()
        {
            return request_infos_.size();
        }
      
        //发出去的请求期待收到回包
        void AddRequest(const RID& resource_id,const boost::asio::ip::udp::endpoint& end_point,boost::uint8_t PacketAction,uint32_t transaction_id,uint16_t peer_version,
            unsigned short request_peer_count,unsigned query_tracker_num);

        //收到一个回复了。end_point表示responser的发送端，很可能是tracker服务器。返回正数或者0，表示还差多少个request等待reponse
        int AddResponse(const RID& resource_id,uint32_t transaction_id,const boost::asio::ip::udp::endpoint& end_point,vector<protocol::CandidatePeerInfo>& peer_infos);
    
        //获取response的结果，并且删除掉相应的ListRequestInfo,返回非0表示失败
        //random_shuf表示是否随机选取，如果随机选取，location_percent传入的数值就没有意义了。
        int GetResponse(vector<protocol::CandidatePeerInfo>& peer_infos,boost::asio::ip::udp::endpoint& end_point,boost::uint8_t& action,boost::uint16_t& peer_version,
            const RID& resource_id,uint32_t transaction_id,int location_percent,bool random_shuf);


        //获取平均回复的peer的个数和查询的tracker的个数,返回非0表示查询失败（例如没有相应的记录）
        int GetResponseCount(const RID& resource_id,uint32_t transaction_id,int &avg_count,int& response_count);

        //找出过期的请求，建议调用的时间间隔控制在超时时间间隔之内，例如，100ms的keep_alive_ms的话，这个函数建议50ms调用一次  ,map 的后一项是transaction_id    
        void GetDeadlineRequest(unsigned keep_alive_ms,map<RID,uint32_t>& dead_requests);
        
        int RemoveRequestInfo(const RID& resource_id,uint32_t transaction_id);
        //规定的时间内未回复的response数
		int GetUnResponseCount(const RID& resource_id,uint32_t transaction_id);


    private:
        ListRequestCache();
        int GetResponse(vector<protocol::CandidatePeerInfo>& peer_infos,boost::asio::ip::udp::endpoint& end_point,boost::uint8_t& action,boost::uint16_t& peer_version,
            boost::uint64_t magic_num,int location_percent,bool random_shuf);

        int GetResponse(vector<protocol::CandidatePeerInfo>& peer_infos,boost::asio::ip::udp::endpoint& end_point,boost::uint8_t& action,boost::uint16_t& peer_version,
            RID& resouce_id,uint32_t& transaction_id,boost::uint64_t magic_num,int location_percent,bool random_shuf);

        int RemoveRequestInfo(boost::uint64_t magic_num);
        ListRequestInfoContainer request_infos_;
        ListRequestInfoMagicNumHashIndex& request_info_magic_index_;
        ListRequestInfoRequestTimeIndex&  request_info_time_index_;
        static ListRequestCache* s_instance;     
    };
}
#endif