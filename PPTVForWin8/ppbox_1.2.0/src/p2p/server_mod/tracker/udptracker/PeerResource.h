/******************************************************************************
*
* Copyright (c) 2011 PPLive Inc.  All rights reserved
* 
* PeerResource.h
* 
* Description: 存储一个rid下的peer_id， 按照地域为key来存放
*				除了以地域位key存放外，还需要保持一个无序的队列，让peer随机返回。
*             
* 
* --------------------
* 2011-11-1, kelvinchen create
* --------------------
******************************************************************************/

#ifndef PEER_RESOURCE_H_CK_2011_11_1
#define PEER_RESOURCE_H_CK_2011_11_1

#include <boost/shared_ptr.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/pool/pool.hpp>
#include <boost/cstdint.hpp>

#include <map>
#include <list>
#include <vector>
//#include <framework/timer/Timer.h"
#include "udptracker/TrackerStucts.h"
#include "udptracker/TrackerStatistic.h"


#include "struct/Base.h"


extern log4cplus::Logger g_logger;


namespace udptracker
{
    //定义一些统计项：
    //list的时候，查询返回了地域信息的节点
    const std::string LOCATIONLIST="LocationList";
    //广告的查询
    const std::string ADVLIST="AdvList";
    //查询时，rid下总的peer数不足50个
    const std::string NOENOUGHLIST="NoEnoughList";
    //查询的时候，找不到查询者所在的地域
    const std::string NOSRCLOACTIONLIST="NoSrcLoactionList";
    //收到leave命令的时候，进行了节点的删除操作
    const std::string LEAVEEXIST="LeaveExist";
    //收到leave命令时，并没有找到这个节点
    const std::string LEAVENOTEXIST="LeaveNotExist";
    //在list的过程中，发现的节点超时
    const std::string LISTDEAD="ListDead";

    struct PeerInfoEx
    {
        PeerInfoEx():max_list_time_size(100)
        {

        }
        //在peerInfo的基础上再加些东西
        PeerInfo peer_info;
        //peer_id
        Guid	 peer_id;
        //每个peer_id一一对应到4字节数，用于存放方便
        boost::uint32_t peer_magicnum;		
        //这个peer下面的所有资源，正常情况不会超过200个。
        std::vector<boost::uint32_t> resources;

        //上次和tracker report的时间
        framework::timer::TickCounter::count_value_type last_report_time;

        //第一次的上报时间
        framework::timer::TickCounter::count_value_type first_report_time;

#ifdef _DEBUG
        //记录最近max_list_time_size
        std::deque<framework::timer::TickCounter::count_value_type> list_time;
#endif
        const unsigned max_list_time_size;
    };
    inline std::ostream& operator << (std::ostream& os, const PeerInfoEx& info)
    {       
        framework::timer::TickCounter::count_value_type now = framework::timer::TickCounter::tick_count();
        os << "last_report_time: " << info.last_report_time
            << ", first_report_time: " << info.first_report_time<<" ,now"<<now/1000
            << ", peer_id " << info.peer_id<<", magicnum"<<info.peer_magicnum<<" resources_size:"<<info.resources.size();

#ifdef _DEBUG
        os<<" ,list_time_size:"<<info.list_time.size();

        for(unsigned i = 0;i<info.list_time.size();++i)
        {
            os<<" list_"<<i<<" "<<((now - info.list_time[info.list_time.size()-i-1])/1000);
        }
#endif
        return os;               
    }

    typedef boost::multi_index::multi_index_container<
        PeerInfoEx,
        boost::multi_index::indexed_by<		
        boost::multi_index::hashed_unique<		
        BOOST_MULTI_INDEX_MEMBER(PeerInfoEx, Guid, peer_id)		
        >,
        boost::multi_index::hashed_unique<		
        BOOST_MULTI_INDEX_MEMBER(PeerInfoEx,boost::uint32_t,peer_magicnum)		
        >
        >
    > PeerInfoExContainer;

    struct ResInfo
    {
        //将rid映射到4字节
        RID rid;
        boost::uint32_t res_magicnum;
    };
    typedef boost::multi_index::multi_index_container<
        ResInfo,
        boost::multi_index::indexed_by<
        boost::multi_index::hashed_unique<
        BOOST_MULTI_INDEX_MEMBER(ResInfo,Guid,rid)
        >,
        boost::multi_index::hashed_unique<
        BOOST_MULTI_INDEX_MEMBER(ResInfo,boost::uint32_t,res_magicnum)
        >
        >
    >ResInfoContainer;

    typedef boost::multi_index::multi_index_container<
        boost::uint32_t,
        boost::multi_index::indexed_by<
        boost::multi_index::sequenced<>,
        boost::multi_index::ordered_unique<boost::multi_index::identity<boost::uint32_t> >		
        >
    >RandomListContainer;

    typedef PeerInfoExContainer::nth_index<0>::type PeerContainerPeeridHashedIndex;
    typedef PeerInfoExContainer::nth_index<1>::type PeerContainerMagicNumHashedIndex;

    typedef ResInfoContainer::nth_index<0>::type ResContainerRidHashedIndex;
    typedef ResInfoContainer::nth_index<1>::type ResContainerMagicNumHashedIndex;

    typedef RandomListContainer::nth_index<0>::type RandomListContainerSquencedIndex;	
    typedef RandomListContainer::nth_index<1>::type RandomListContainerOrderedIndex;



    extern unsigned g_trace_ip;
    extern map<int,int> g_proitity_num;
    uint32_t Ip2Location(unsigned ip);	

    //获取并且清空资源返回的优先级统计
    map<int,int> ClearPriorityNum();

    //获取并且清空统计
    map<std::string,int> ClearStatusNum();

    struct PeerResource
    {
        //利用了map的排序性
        std::map<boost::uint32_t,vector<boost::uint32_t> > location_peers; 

        //list比queue容易删除,因为这个队列是有序的，remove想要保持有序，queue的消耗比较大
        //std::list<boost::uint32_t> random_peer;
        RandomListContainer random_peer;		
        RandomListContainerSquencedIndex& random_list_sequenced_index;
        RandomListContainerOrderedIndex& random_list_ordered_index;
        //把random_peer里的迭代器的位置存起来。方便删除
        //std::map<boost::uint32_t,std::list<boost::uint32_t>::iterator > random_iterator_map;

        //这个变量主要作用是用来判断广告文件，不建议作为其他用途，因为它的大小和random_peer的长度不一定会相同！
        int peer_count;		

        PeerResource(const RID& Rid,unsigned dec_bandwidth_percent,unsigned bandwidth_limit_speed)
            :peer_count(0),rid_(Rid),dec_bandwidth_percent_(dec_bandwidth_percent),bandwidth_limit_speed_(bandwidth_limit_speed),
            random_list_sequenced_index((random_peer.get<0>())),random_list_ordered_index((random_peer.get<1>()))

        {

        }

        //拷贝构造函数
        PeerResource(const PeerResource &temp):peer_count(temp.peer_count),rid_(temp.rid_),dec_bandwidth_percent_(temp.dec_bandwidth_percent_),
            bandwidth_limit_speed_(temp.dec_bandwidth_percent_),random_peer(temp.random_peer),
            random_list_sequenced_index((random_peer.get<0>())),random_list_ordered_index((random_peer.get<1>()))
        {			
        }

        static bool IsSupportTcp(const PeerInfo& peer_info)
        {
            return peer_info.upnp_tcp_port != 0;
        }

        //在listtcp的时候，需要的是tcp的端口，不是udp的端口
        static void PushBackPeerInfo(vector<protocol::CandidatePeerInfo>& peer_infos,const PeerInfo& peer_info,bool need_tcp);

        void RemovePeerFromRandomList(boost::uint32_t peer_id);
        void RemovePeerFromLocationMap(boost::uint32_t peer_id,boost::uint32_t location);

        //删除一个peer
        void RemovePeer(boost::uint32_t peer_id,boost::uint32_t location);

        //添加一个peer,max_randomlist_size 表示random_peer这个队列的最大长度。添加是一个push_back的操作。如果超过长度，那就需要pop一个出来
        //max_same_location_size 表示相同的地域的peer最多存多少个，如果超过，那么就随机替换一个，否则push_back
        //ad_optimize表示是否采取了小文件优化，如果优化了，那么就有可能不插入map
        //是否是广告文件的判断依据，如果peer_count大于ad_peer_count,那么就认为是广告文件
        //peer_magicnum_hash_index 的作用是用来获取peer的信息，在randomlist需要出队列的时候只出小带宽的
        //exist_check表示添加的时候是否需要先判断peer已经存在，如果需要判断且已经存在，就不添加，目前这个值应该为false
        void AddPeer(boost::uint32_t peer_id,boost::uint32_t location,int ad_peer_count,const PeerContainerMagicNumHashedIndex& peer_magicnum_hash_index,
            int max_randomlist_size = 500,int max_same_location_size=50,bool ad_optimize = false,bool exist_check=false);

        //peer_infos 表示获取到的peer，max_location_peer_count,max_random_peer_count表示期望获取的两种peer的数量，keep_alive_time表示不在线的判断的时间间隔
        //由于list操作会修改剩余带宽，所以需要传入peer的信息,另外，删除过期节点的操作不能在这里做，因为这里只有一个rid对应的节点，过期节点可能在多个rid里有，因此需要返回给调用者去删除
        //keep_alive_time 的单位是毫秒,need_tcp表示是否查询的是tcp的信息（有些peer不能接受udp，例如flash）,requirer_nat_type表示请求者的nat类型，用于设置peer优先级的
        void ListPeer(vector<protocol::CandidatePeerInfo>& peer_infos,boost::uint32_t src_ip,boost::uint8_t requirer_nat_type,
            size_t max_location_peer_count,size_t max_random_peer_count,int keep_alive_time,
            PeerContainerMagicNumHashedIndex& peer_magicnum_hash_index,std::set<boost::uint32_t>& remove_peer,bool need_tcp);

        //改变地域信息
        void ChangeLocation(uint32_t peer_id,uint32_t old_location,uint32_t new_location,int max_same_location_size=50);

    private:
        void AddRandomList(boost::uint32_t peer_id,int ad_peer_count,const PeerContainerMagicNumHashedIndex& peer_magicnum_hash_index,int max_randomlist_size);
        void AddLocationMap(boost::uint32_t peer_id,boost::uint32_t location,int ad_peer_count,int max_same_location_size,bool ad_optimize);

        //把it里指向的peer信息，添加到peer_infos里，并且最多添加到max_peer_count个,传入srcip的目的是能够设置peer_infos里头的优先级信息   
        void AddPeerInfo(vector<protocol::CandidatePeerInfo>& peer_infos,size_t max_peer_count,const vector<boost::uint32_t>& same_location_peers,
            PeerContainerMagicNumHashedIndex& peer_magicnum_hash_index,boost::uint32_t src_ip,boost::uint32_t src_location,boost::uint8_t requirer_nat_type,bool need_tcp);      

        //减少剩余带宽
        void DecBandWidth(PeerInfo& peer_info);

        //判断剩余带宽是否有限,目前的判断方法是拍脑袋想得，没有什么依据。
        bool IsBandWidthLimit(const PeerInfo& peer_info) const;

        void SetPeerPriority(PeerInfoEx& peer_info_ex,boost::uint32_t src_ip,boost::uint32_t src_location,boost::uint8_t requirer_nat_type);

        //peer被返回之后，剩余带宽的衰减比例
        const unsigned dec_bandwidth_percent_;
        //被认为是小带宽的阀值
        const unsigned bandwidth_limit_speed_;

        //这个东西本来可以不要的，留着只是为了输出日志的时候有用
        RID rid_;
    };

};

#endif PEER_RESOURCE_H_CK_2011_11_1