/******************************************************************************
*
* Copyright (c) 2011 PPLive Inc.  All rights reserved
* 
* RidListCache.h
* 
* Description: 保存近一次较好的rid list得到的peer的信息
* 
* --------------------
* 2011-12-7, kelvinchen create
* --------------------
******************************************************************************/

#ifndef RID_LIST_CACHE_H_CK_20111207
#define RID_LIST_CACHE_H_CK_20111207

#include <boost/shared_ptr.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include "Common.h"
#include <framework/timer/Timer.h>
#include <framework/timer/TickCounter.h>
#include <framework/string/Uuid.h>

using namespace framework::string;
//using namespace boost;

//boost::uint64_t now_tick = framework::timer::TickCounter::tick_count();
namespace udptrackerag
{
    struct ListResult
    {
        RID resource_id;

        //本条记录被设置的时间
        boost::uint64_t set_tick;

        //上次list的到的count，表示后台tracker的平均资源数，不是表示tracker的个数
        int count;

        //上次查询到的peer，查不到的时候备用的
        vector<protocol::CandidatePeerInfo> peers;

    };

    struct HashFunc
    {
        size_t operator()(const Uuid& s) const { return s.hash_value(); }
    };

    //按唯一的rid进行hash，按非唯一的设置时间进行排序
    typedef boost::multi_index::multi_index_container<
        ListResult,
        boost::multi_index::indexed_by<		
        boost::multi_index::hashed_unique<		
        BOOST_MULTI_INDEX_MEMBER(ListResult, RID, resource_id),HashFunc	
        >,
        boost::multi_index::ordered_non_unique<		
        BOOST_MULTI_INDEX_MEMBER(ListResult,boost::uint64_t,set_tick)		
        >
        >
    > ListResultContainer;

    typedef ListResultContainer::nth_index<0>::type ListResultRidHashIndex;
    typedef ListResultContainer::nth_index<1>::type ListResultSetTickIndex;

    class RidListCache
    { 
        RidListCache();
    public:
        static RidListCache * Instance();      

         //设置
        void SetListResult(const RID& resource_id,int count);
        void SetListResult(const RID& resource_id,int count,const vector<protocol::CandidatePeerInfo>& peer_infos);
        //查询,如果没有对应信息，返回0，有则返回对应信息
        int GetListResult(const RID& resource_id);

        //查询,如果没有对应信息，返回0，有则返回对应信息
        int GetListResult(const RID& resource_id,vector<protocol::CandidatePeerInfo>& peer_infos);

        //清空超时，建议10s调用一次
        void RemoveTimeout(unsigned timeout_ms);
      
    private:
        static RidListCache* s_instance;
        ListResultContainer list_result_;
        ListResultRidHashIndex& rid_hash_index_;
        ListResultSetTickIndex& set_tick_index_;
    };
}

#endif