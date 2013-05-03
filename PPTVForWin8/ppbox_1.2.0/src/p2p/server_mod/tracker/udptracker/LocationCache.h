/******************************************************************************
*
* Copyright (c) 2011 PPLive Inc.  All rights reserved
* 
* LocationCache.h
* 
* Description: 存储rid和peer_id的对应关系，同一个rid下的peer_id， 按照地域为key来存放
*				除了以地域位key存放外，还需要保持一个无序的队列，让peer随机返回。
*             
* 
* --------------------
* 2011-09-26, kelvinchen create
* --------------------
******************************************************************************/
#ifndef _LOCATIONCACHE_H_CK_20110926_
#define _LOCATIONCACHE_H_CK_20110926_

//#include "boost/asio/detail/hash_map.hpp"

#include "PeerResource.h"

#if defined(_MSC_VER) 
# include <hash_map> 
using stdext::hash_map; 
#else 
# include <ext/hash_map> 
using __gnu_cxx::hash_map; 
#endif 


extern log4cplus::Logger g_logger;

namespace udptracker
{

    class LocationCache
        :public boost::noncopyable
    {
    private:
        //所有rid对应的peer资源
        hash_map<boost::uint32_t,PeerResource> all_res_;
        //所有在线peer
        PeerInfoExContainer peers_;
        //所有res
        ResInfoContainer resources_;

        //是否需要使用广告文件优化
        bool adv_optimize_;

        //下两个参数参看PeerRes的add peer函数
        int max_randomlist_size_;
        int max_same_location_size_;

        //统计report的时候，上报ip变化和location变化的次数，这个对于查询peerid的重复率有好处。
        //其中，change_ip_num表示ip发生变化，但是location不变的情况。
        int change_ip_num_;
        int change_location_num_;

        //记录list的时候，各种返回peer个数的结果的数量，也是统计时用的。
        map<int,int> list_ret_num_;

        //记录listtcp的时候，各种返回peer个数的结果的数量，也是统计时用的。
        map<int,int> listtcp_ret_num_;


        //list的时候，返回locationpeer的比例，默认为70%，即这个数字是70
        size_t list_location_percent_;

        //占全peer比例达到这个数量的rid，被认为是广告，默认为5%
        size_t adv_peer_percent_;

        //peer返回时，带宽的衰减比例
        size_t dec_bandwidth_percent_;

        //小带宽的判断依据
        size_t bandwidth_limit_speed_;

        PeerContainerPeeridHashedIndex&   peer_id_hash_index_;
        PeerContainerMagicNumHashedIndex& peer_magicnum_hash_index_;
        ResContainerRidHashedIndex&	     res_id_hash_index_;
        ResContainerMagicNumHashedIndex&  res_macignum_hash_index_;



    public:
        typedef boost::shared_ptr<LocationCache> p;
        static LocationCache::p Create(bool adv_optimize=true,size_t max_randomlist_size = 500,size_t max_same_location_size=30,
            size_t res_hash_map_size=200961,size_t list_location_percent=70,size_t adv_peer_percent = 5,size_t dec_bandwidth_percent = 50,size_t bandwidth_limit_speed = 50) 
        { 
            return p(new LocationCache(adv_optimize,max_randomlist_size,max_same_location_size,res_hash_map_size,list_location_percent,adv_peer_percent,dec_bandwidth_percent,bandwidth_limit_speed)); 
        }

        bool LoadFile(const string& loadfile);

        void DumpAllPeersDetailInfo(std::ofstream & f);

        LocationCache(bool adv_optimize,size_t max_randomlist_size,size_t max_same_location_size,size_t res_hash_map_size,size_t list_location_percent,size_t adv_peer_percent,
            size_t dec_bandwidth_percent,size_t bandwidth_limit_speed);
        ~LocationCache();

    public:
        //这几个有返回值的clear函数，在清零的同时返回原数据
        int ClearChangeIpNum();
        int ClearChangeLocationNum();
        map<int,int> ClearListRetNum();
        map<int,int> ClearListTcpRetNum();

        void Clear();
        void Insert(const Guid& peer_id, const RID& resource_id);
        void RemovePeer(const Guid& peer_id);   		
        //void DelPeerResource(const Guid& peer_id, const RID& resource_id);

        size_t ResourcesCount() const { return resources_.size(); }
        size_t PeersCount() const { return peers_.size(); }

        size_t PeerResourcesCount(const Guid& peer_id) const;
        size_t ResourcePeersCount(const RID& resource_id) const;

        bool HasPeer(const Guid& peer_id) const;
        bool HasResource(const Guid& resource_id) const;
        bool HasResource(const Guid& peer_id, const RID& resource_id) const; // whether peer contains resource

        framework::timer::TickCounter::count_value_type GetPeerKPLTime(const Guid& peer_id) const;
        void UpdatePeerKPLTime(const Guid& peer_id);


        //如果不存在就创建peerinfoex，如果存在就更新。返回值是peer_magic_num
        boost::uint32_t UpdatePeerInfo(const Guid& peer_id, const PeerInfo& peer_info);


        //如果不存在这个rid，那么就添加到resources_里，并且返回对应的magicid，否则返回0
        boost::uint32_t AddResInfo(const RID& rid);

    private:		
        //返回0表示没有对应项
        //代码里所有boost::uint32_t peer_id, boost::uint32_t resource_id 为参数的find操作，都应该找到相应的数据，所以找不到时都会有assert(false)
        //这样，在调用了Rid2MagicNum和 Guid2MagicNum 之后，必须判断返回值是否为0
        boost::uint32_t Rid2MagicNum(const RID& rid) const;
        boost::uint32_t Guid2MagicNum(const Guid& gid) const;


        RID	MagicNum2Rid(boost::uint32_t magicnum) const;
        Guid MagicNum2Guid(boost::uint32_t magicnum) const;
        //最后一个参数表示是否需要修改 peerinfo_ex里对应的resource vector。对于节点的leave删除，这个变量就应该设置为false，吧对于resource_vetor的删除放到调用这个函数的地方做
        void DelPeerResource(boost::uint32_t peer_id, boost::uint32_t resource_id,bool change_peer_res_vec=true);
        uint32_t Peerid2Location(uint32_t peer_id);
        const PeerInfoEx& GetPeerInfoEx(uint32_t peer_id) const;
        PeerInfoEx& GetPeerInfoEx(uint32_t peer_id);
        void InsertPeerRes(uint32_t peer_id,uint32_t magic_rid);

        void ChangePeerLocation(uint32_t peer_id,uint32_t old_location,uint32_t new_location);
        void RemovePeer(std::set<uint32_t> remove_peers);

    public:
        // process_delete表示是否需要删除存在内存里，但是这次update里没有的resource
        // 要求这个peer_id 必须是有记录的peerid，如果是新来的资源，应该先调用UpdatePeerInfo 之后才能调用本函数,原来这里是使用模板函数的，但是觉得丑陋了，就改成了两个函数
        void UpdatePeerResources(const Guid& peer_id, const std::set<RID>& resources,bool process_delete = true);
        void UpdatePeerResources(const Guid& peer_id, const vector<RID>& resources,bool process_delete = true);

        //根据keep_alive_time来判断是否在线
        //keep_alive_time 的单位是毫秒
        void ListResourceAlivePeers(vector<protocol::CandidatePeerInfo>& peer_infos,const Guid& resource_id, size_t max_alive_peer_count,int keep_alive_time,
            const boost::asio::ip::udp::endpoint& end_point,boost::uint8_t requirer_nat_type);

        //根据keep_alive_time来判断是否在线
        //keep_alive_time 的单位是毫秒
        void ListResourceAliveTcpPeers(vector<protocol::CandidatePeerInfo>& peer_infos,const Guid& resource_id, size_t max_alive_peer_count,int keep_alive_time,
            const boost::asio::ip::udp::endpoint& end_point,boost::uint8_t requirer_nat_type);

        template <typename _ResourceIter>
        void ListPeerResources(const Guid& peer_id, _ResourceIter resource_begin) const
        {
            LOG4CPLUS_DEBUG(g_logger,"ListPeerResources,peerid:"<<peer_id<<" peer count:"<<peers_.size()<<" rescource count:"<<all_res_.size());
            PeerContainerPeeridHashedIndex::iterator it = peer_id_hash_index_.find(peer_id);
            if(it == peer_id_hash_index_.end())
            {
                LOG4CPLUS_DEBUG(g_logger,"ListPeerResources,peerid not found");
                return;
            }

            for (size_t i=0;i<it->resources.size();++i)
            {
                LOG4CPLUS_TRACE(g_logger,"ListPeerResources,add resource:"<<it->resources[i]<<" "<<MagicNum2Rid(it->resources[i]));
                *resource_begin++ = MagicNum2Rid(it->resources[i]);				
            }
        };

        //这个函数应该定时被调用，返回值表示此次移除的peer的数量，如果等于max_remove_unlive_peer_per_time，就说明可能还有需要移除的peer，建议减小调用时间或者加大max_remove_unlive_peer_per_time
        //max_remove_unlive_peer_per_time 一次调用这个函数，最多移除的peer数量;
        //keep_alive_time，peer没有交互的情况下，最长的存活时间，如果超过这个时间没有report就应该移除 keep_alive_time 的单位是毫秒
        //max_travel_peer_per_time 表示遍历的最大peer数量
        //这个函数基于hash删除元素之后，不会乱序。需要多测试		
        size_t RemoveNonAlivePeers(size_t keep_alive_time,size_t max_remove_unlive_peer_per_time,size_t max_travel_peer_per_time);

    };
};

#endif//_LOCATIONCACHE_H_CK_20110926_