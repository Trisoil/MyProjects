#include "Common.h"
#include "PeerResource.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include "IpLocation.h"
#include "TrackerModel.h"

#include <log4cplus/logger.h>


using namespace boost;
using namespace protocol;
using namespace std;
using namespace framework::timer;

namespace udptracker
{
    //遇到这个ip来的请求，就打详细的日志
    unsigned g_trace_ip = 2061575015U;

    //记录每种优先级出现的次数
    map<int,int> g_proitity_num;

    //记录一些情况出现的次数
    map<std::string,int> g_status_num;

    map<int,int> ClearPriorityNum()
    {
        map<int,int> tmp;
        tmp.swap(g_proitity_num);
        return tmp;
    }

    map<std::string,int> ClearStatusNum()
    {
        map<std::string,int> tmp;
        tmp.swap(g_status_num);
        return tmp;
    }

    uint32_t Ip2Location(unsigned ip)
    {
        return IpLocation::Instance()->GetLocation(ip);
    }

    void  PeerResource::SetPeerPriority(PeerInfoEx& peer_info_ex,boost::uint32_t src_ip,boost::uint32_t src_location,boost::uint8_t requirer_nat_type)
    {
        PeerInfo& peer_info = peer_info_ex.peer_info;
        if(peer_info.DetectIP == src_ip)
        {
            //同一子网
            peer_info.TrackerPriority = 255;
            ++g_proitity_num[peer_info.TrackerPriority];
            return;
        }

        boost::uint8_t  priority = 0;
        //unsigned location1 = Ip2Location(peer_info.DetectIP);
        unsigned dstlocation = peer_info.peer_location;
        //unsigned location2 = Ip2Location(src_ip);
        int64_t diff = 0;
        if(0 == dstlocation|| 0 ==src_location )
        {
            //有一个是未知ip，那么就认为它们的地域差别很大,16711425是2<<24 -1
            diff = 16711425;            
        }
        else
        {
            diff = abs(int(dstlocation) - int(src_location));
        }

        if (0 == diff)
        {
            //同市
            priority += TrackerModel::priority_isp_[5];
        }
        else if(diff<256)
        {
            //同省
            priority +=  TrackerModel::priority_isp_[4];
        }
        else if(diff<65536)
        {
            //同isp
            priority += TrackerModel::priority_isp_[3];
        }
        else if(diff<16711425)
        {
            priority += TrackerModel::priority_isp_[2];
        }
        else if(diff != 16711425)
        {
            priority += TrackerModel::priority_isp_[1];
        }
        else
        {
            priority += TrackerModel::priority_isp_[0];
        }

        //做这个主要是为了防止配置不当，配置错了应该直接崩溃
        assert(!TrackerModel::priority_bandwith.empty());

        //再计算带宽
        int leftBandWidth = min<int>(peer_info.upload_bandwidth_kbs , peer_info.upload_limit_kbs) - peer_info.upload_speed_kbs;

        //leftBandWidth可能为负数
        // 在这里进行带宽的定位
        bool cacled = false;
        std::map<int , int>::iterator it = TrackerModel::priority_bandwith.begin();
        for( ++it;	it != TrackerModel::priority_bandwith.end(); ++it )
        {
            //第一个区间是最大的区间，所以for循环的初始值设置为 begin++
            if( leftBandWidth <= it->first )
            {
                // 符合				
                priority += it->second;
                break;
            }
        }
        if( it == TrackerModel::priority_bandwith.end() )
        {
            priority += TrackerModel::priority_bandwith.begin()->second;
        }

        //ck 20120626新增：根据peer上拥有的rid个数，决定优先级。priority = 带宽分 + 地域分 + rid个数分。
        //rid个数分为： min(200/(ridcount), 50) ，200和50可配置
        //这个被除数是不会为0的，所以不考虑。（不会为0，是基于本程序是同步的，异步就不一定了）
        priority += min<int>(TrackerModel::rid_count_prority_limit/(peer_info_ex.resources.size()), TrackerModel::rid_count_prority_max);

        peer_info.TrackerPriority = priority;

        ++g_proitity_num[peer_info.TrackerPriority];

        LOG4CPLUS_TRACE(g_logger, "tracker priority " << int(priority)<<" src ip:"<<src_ip<<" src location:"<<src_location<<" dstlocation:"
            <<dstlocation<<" peer res count:"<<peer_info_ex.resources.size());

        //ck20120912新增：priority = 带宽分 + 地域分 + rid个数分 + nattype 类型分
        //nattype类型分的初衷，是尽可能给外网返回内网节点，内网返回给外网节点，从而达到较高的连通率
        if(requirer_nat_type == TYPE_ERROR)
        {
            //不改变
        }
        else if(TYPE_PUBLIC == requirer_nat_type)
        {
            if(peer_info.PeerNatType != TYPE_PUBLIC)
            {
                priority += TrackerModel::public_2_inner_priority;
            }
        }
        else
        {
            if(peer_info.PeerNatType == TYPE_PUBLIC)
            {
                priority += TrackerModel::inner_2_public_priority;
            }
        }

#ifdef _DEBUG
        peer_info_ex.list_time.push_back(framework::timer::TickCounter::tick_count());
        if(peer_info_ex.list_time.size()>peer_info_ex.max_list_time_size)
        {
            peer_info_ex.list_time.pop_front();
        }
#endif
        if (src_ip == g_trace_ip)
        {
            LOG4CPLUS_INFO(g_logger, "tracker priority " << int(priority)<<" src ip:"<<src_ip<<" src location:"<<src_location<<" dstlocation:"<<dstlocation
                <<" peerversion:"<<int(peer_info_ex.peer_info.PeerVersion)<<" ,bandlimit:"<<peer_info.upload_bandwidth_kbs<<" ,upload_limit:"<<peer_info.upload_limit_kbs
                <<" ,upload speed:" <<peer_info.upload_speed_kbs<<" exinfo:"<<peer_info_ex);
        }
        
    }

    void  PeerResource::PushBackPeerInfo(vector<protocol::CandidatePeerInfo>& peer_infos,const PeerInfo& peer_info,bool need_tcp)
    {
        if( need_tcp )
        {
            PeerInfo tmp = peer_info;
            tmp.DetectUdpPort = peer_info.upnp_tcp_port;
            tmp.UdpPort = peer_info.internal_tcp_port;
            peer_infos.push_back(tmp);
        }
        else
        {
            peer_infos.push_back(peer_info);
        }
    }

    void PeerResource::RemovePeerFromRandomList(boost::uint32_t peer_id)
    {
        random_list_ordered_index.erase(peer_id);
    }

    void PeerResource::RemovePeerFromLocationMap(boost::uint32_t peer_id,boost::uint32_t location)
    {
        if (location_peers.find(location) == location_peers.end())
        {
            //如果采取了广告文件优化，可能这里为空的.空的情况也会走到这里，另外，这个location同样可能不存在。
            LOG4CPLUS_TRACE(g_logger, "location_peers.find(location) == location_peers.end(),may be empty or ad_optimize");
            return;
        }

        //为0的location不会被插入到map里面
        assert(location != 0);

        //peers出现比较多，为了减少map的查询，所以不后面不是用 location_peers[location],而使用peers
        vector<boost::uint32_t>& peers = location_peers[location];
        vector<boost::uint32_t>::iterator it = find(peers.begin(),peers.end(),peer_id);
        if (it != peers.end())
        {
            //对于peers长度为1的情况，下面的逻辑也没有问题
            *it = *peers.rbegin();
            peers.resize(peers.size() -1);
            LOG4CPLUS_DEBUG(g_logger, "location_peers["<<location<<"] new size:" << peers.size());
        }

        //做这个操作，是为了不让内存使用过大。
        if (peers.empty())
        {
            location_peers.erase(location);
        }
    }

    void PeerResource::RemovePeer(boost::uint32_t peer_id,boost::uint32_t location)
    {
        --peer_count;
        LOG4CPLUS_TRACE(g_logger, "after remove ,peer count" << peer_count);		
        assert(peer_count>=0);

        RemovePeerFromRandomList(peer_id);
        RemovePeerFromLocationMap(peer_id,location);

        return;
    }


    void PeerResource::AddRandomList(boost::uint32_t peer_id,int ad_peer_count,const PeerContainerMagicNumHashedIndex& peer_magicnum_hash_index,int max_randomlist_size)
    {
        //random_peer.push_back(peer_id);
        random_list_sequenced_index.push_back(peer_id);

        //这里的目的是添加random_peer的最后一个元素到map
        //据说 random_peer.end()--;也能达到相同的效果，但是代码看起来比较怪，所以我这里采用了反向迭代器
        //list<boost::uint32_t>::reverse_iterator r_it = random_peer.rbegin();
        //++r_it;
        //random_iterator_map[peer_id] = r_it.base();

        //random_iterator_map实际上就是 random_peer.size(),但是两者的时间复杂度却差别很大。
        //if (random_iterator_map.size()>max_randomlist_size)
        if (random_peer.size() > max_randomlist_size)
        {
            bool has_erase = false;

            //这个if表示，广告文件就不考虑谁出队列了。
            if (peer_count < ad_peer_count)
            {	
                //最多查看5个元素，超过5个就认为大剩余带宽的已经很多了，把第一个元素出队列就好了。
                int travel_num = 0;
                //for (list<uint32_t>::iterator it = random_peer.begin();it != random_peer.end() && travel_num++<5;++it)
                for (RandomListContainerSquencedIndex::iterator it = random_list_sequenced_index.begin();it!= random_list_sequenced_index.end() && travel_num++<5;++it)
                {
                    PeerContainerMagicNumHashedIndex::iterator it_info = peer_magicnum_hash_index.find(*it);
                    assert(it_info != peer_magicnum_hash_index.end());
                    //20111210去掉小带宽判断
                    //if(IsBandWidthLimit(it_info->peer_info))
                    {
                        LOG4CPLUS_TRACE(g_logger, "erase a bandwidth limit peer,upload speed:"<<it_info->peer_info.upload_speed_kbs<<":"<<
                            it_info->peer_info.upload_limit_kbs<<":"<<it_info->peer_info.upload_bandwidth_kbs);
                        random_list_sequenced_index.erase(it);
                        has_erase = true;						
                        break;
                    }
                }	
            }
            else
            {
                //广告文件，直接把第一个元素出列就好了，就是下面的 if (!has_erase)
                LOG4CPLUS_TRACE(g_logger,"peer count:"<<peer_count<<" ad_peer_count:"<<ad_peer_count);
            }
            //如果没有小剩余带宽的peer，就把第一个弹出
            if (!has_erase)
            {
                LOG4CPLUS_TRACE(g_logger,"add pop front");
                random_list_sequenced_index.pop_front();
            }
        }
    }

    void PeerResource::AddLocationMap(boost::uint32_t peer_id,boost::uint32_t location,int ad_peer_count,int max_same_location_size,bool ad_optimize)
    {
        if ( 0 == location )
        {
            //没有记录的ip，不插入了
            LOG4CPLUS_TRACE(g_logger,"AddLocationMap location == 0,peercount:"<<peer_count<<" ad_peer_count:"<<ad_peer_count<<" peer_id"<< peer_id);
            return;
        }

        if(ad_optimize &&(peer_count > ad_peer_count))
        {
            LOG4CPLUS_TRACE(g_logger,"AddLocationMap peer_count:"<<peer_count<<" ad_peer_count:"<<ad_peer_count<<" peer_id"<< peer_id);

            //map的clear能保证释放内存，如果是vector，则需要用swap来释放了。
            location_peers.clear();			
        }
        else
        {
            vector<boost::uint32_t>& peers = location_peers[location];
            if (peers.size() >= max_same_location_size)
            {
                //vector够长了，随机找一个换了,因为peerid是随机的，因此就拿peerid作为随机数，而不再新生成了。这条路径比较难走到，因为大部分情况是走到location_peers.clear();里去了
                peers[peer_id % max_same_location_size] = peer_id;
                LOG4CPLUS_INFO(g_logger,"change location map,location:"<<location<<" pid:"<<peer_id<<" max_same_location_size:"<<max_same_location_size
                    <<" location peer size:"<<peers.size()<<" Rid:"<<rid_<<" peer count:"<<peer_count<<" list size:"<<random_peer.size()<<" ad_peer_count:"
                    <<ad_peer_count<<" ad_optimize:"<<ad_optimize);
            }
            else
            {
                peers.push_back(peer_id);
            }

        }
    }

    void PeerResource::AddPeerInfo(vector<protocol::CandidatePeerInfo>& peer_infos,size_t max_peer_count,const vector<boost::uint32_t>& same_location_peers,
        PeerContainerMagicNumHashedIndex& peer_magicnum_hash_index,boost::uint32_t src_ip,boost::uint32_t src_location,boost::uint8_t requirer_nat_type,bool need_tcp)
    {
        int  need_count = min(max_peer_count - peer_infos.size(),same_location_peers.size());
        LOG4CPLUS_TRACE(g_logger,"need count:"<<need_count<<" max_peer_count:"<<max_peer_count<<" peer_infos.size:"<<peer_infos.size()<<" same_location_peers.size:"<<same_location_peers.size());
        if (src_ip == g_trace_ip)
        {
            LOG4CPLUS_INFO(g_logger,"need count:"<<need_count<<" max_peer_count:"<<max_peer_count<<" peer_infos.size:"<<peer_infos.size()<<" same_location_peers.size:"<<same_location_peers.size());
        }

        //for(vector<boost::uint32_t>::const_iterator it = same_location_peers.begin();it != same_location_peers.end();++it)
        unsigned rand_begin = rand();
        for(unsigned i = rand_begin;i<rand_begin + same_location_peers.size(); ++i)
        {
            PeerContainerMagicNumHashedIndex::iterator it_info = peer_magicnum_hash_index.find(same_location_peers[i %same_location_peers.size()]);
            if(it_info == peer_magicnum_hash_index.end())
            {
                LOG4CPLUS_ERROR(g_logger,"it_info == peer_magicnum_hash_index.end(),index:"<<(i %same_location_peers.size()));
                continue;
            }
            if (need_tcp && !IsSupportTcp(it_info->peer_info))
            {
                continue;
            }
            SetPeerPriority(const_cast<PeerInfoEx&>(*it_info),src_ip,src_location,requirer_nat_type);
            PushBackPeerInfo(peer_infos,it_info->peer_info,need_tcp);
            if((--need_count) <= 0)
            {
                break;
            }
        }
    }



    void PeerResource::AddPeer(boost::uint32_t peer_id,boost::uint32_t location,int ad_peer_count,const PeerContainerMagicNumHashedIndex& peer_magicnum_hash_index,int max_randomlist_size,int max_same_location_size,bool ad_optimize,bool exist_check)
    {
        if (exist_check)
        {
            //先把这个检查禁止了。因为目前用不上
            assert(false);
        }
        ++peer_count;		

        AddRandomList(peer_id,ad_peer_count,peer_magicnum_hash_index,max_randomlist_size);

        AddLocationMap(peer_id,location,ad_peer_count,max_same_location_size,ad_optimize);		

    }

    //这个函数虽然看起来比较长，但是大部分是日志。所以不考虑分成小函数
    void PeerResource::ListPeer(vector<CandidatePeerInfo>& peer_infos,boost::uint32_t src_ip,boost::uint8_t requirer_nat_type,
        size_t max_location_peer_count,size_t max_random_peer_count,int keep_alive_time,
        PeerContainerMagicNumHashedIndex& peer_magicnum_hash_index,set<boost::uint32_t>& remove_peer,bool need_tcp)
    {
        boost::uint32_t src_location = Ip2Location(src_ip);
        if (0 == src_location)
        {
            LOG4CPLUS_DEBUG(g_logger,"unknown ip:"<<src_ip);
        }

        framework::timer::TickCounter::count_value_type timenow = framework::timer::TickCounter::tick_count();
        peer_infos.clear();
        remove_peer.clear();
        int rand_need_count = 0;

        //loaction peer empty的情况是广告文件优化后，无需按地域排序
        //location = 0的情况是不知道这个ip是哪里的
        if ((random_peer.size() <= max_location_peer_count + max_random_peer_count) || location_peers.empty() || 0 == src_location)
        {
            rand_need_count = std::min<int>(max_location_peer_count+max_random_peer_count,random_peer.size());			
            peer_infos.reserve(rand_need_count);
            LOG4CPLUS_TRACE(g_logger,"src ip:"<<src_ip<<" peer_count:"<<peer_count<<" max_location_peer_count:"<<max_location_peer_count<<" max_random_peer_count:"<<max_random_peer_count
                <<"location_peers.size:"<<location_peers.size()<<" src location:"<<src_location<<" rand_need_count:"<<rand_need_count<<" random peer size:"<<random_peer.size());

            if(random_peer.size() <= max_location_peer_count + max_random_peer_count)
            {
                ++g_status_num[NOENOUGHLIST];
            }
            else if( location_peers.empty() )
            {
                ++g_status_num[ADVLIST];
            }
            else 
            {
                ++g_status_num[NOSRCLOACTIONLIST];
            }
        }
        else
        {	
            ++g_status_num[LOCATIONLIST];

            //有足够的资源，需要同时从map和list里取,只有randomlist里考虑remove，map里不考虑		
            peer_infos.reserve(max_random_peer_count+max_location_peer_count);

            LOG4CPLUS_TRACE(g_logger,"src ip:"<<src_ip<<" peer_count:"<<peer_count<<" max_location_peer_count:"<<max_location_peer_count<<" max_random_peer_count:"<<max_random_peer_count
                <<"location_peers.size:"<<location_peers.size()<<" src location:"<<src_location<<" rand_need_count:"<<rand_need_count<<" random peer size:"<<random_peer.size());

            //it_upper指向比src_location大的第一个，it_lower指向小等于src_location的第一个，++it_upper指向会逐渐增大，++it_lower指向会逐渐减小。
            std::map<boost::uint32_t,vector<boost::uint32_t> >::iterator it_upper =  location_peers.upper_bound(src_location);
            std::map<boost::uint32_t,vector<boost::uint32_t> >::reverse_iterator it_lower(it_upper);

            uint32_t get_num = 0;
            while(get_num < max_location_peer_count)
            {
                if(it_upper != location_peers.end() && it_lower != location_peers.rend())	
                {
                    if(abs( (int)src_location - (int)it_upper->first) <  abs( (int)src_location - (int)it_lower->first))
                    {
                        AddPeerInfo(peer_infos,max_location_peer_count,it_upper->second,peer_magicnum_hash_index,src_ip,src_location,requirer_nat_type,need_tcp);						
                        ++it_upper;
                    }
                    else
                    {
                        AddPeerInfo(peer_infos,max_location_peer_count,it_lower->second,peer_magicnum_hash_index,src_ip,src_location,requirer_nat_type,need_tcp);
                        ++it_lower;
                    }
                }
                else if(it_upper!= location_peers.end())
                {	
                    AddPeerInfo(peer_infos,max_location_peer_count,it_upper->second,peer_magicnum_hash_index,src_ip,src_location,requirer_nat_type,need_tcp);
                    ++it_upper;
                }
                else if(it_lower != location_peers.rend())
                {
                    AddPeerInfo(peer_infos,max_location_peer_count,it_lower->second,peer_magicnum_hash_index,src_ip,src_location,requirer_nat_type,need_tcp);
                    ++it_lower;
                }
                else
                {
                    //有可能走到这里，例如，location_peers 里只有一个元素
                    break;
                }	
                get_num = peer_infos.size();
            }	

            //至此，map里的取完了，再取list里的,这里rand_need_count取int而不用size_t是因为后面rand_need_count需要自减操作
            rand_need_count = max_random_peer_count + max_location_peer_count - peer_infos.size();	
            LOG4CPLUS_TRACE(g_logger,"src ip:"<<src_ip<<" peer_count:"<<peer_count<<" max_location_peer_count:"<<max_location_peer_count<<" max_random_peer_count:"<<max_random_peer_count
                <<"location_peers.size:"<<location_peers.size()<<" random list size:"<<random_peer.size()
                <<" src location:"<<src_location<<" rand_need_count:"<<rand_need_count<<" peer_infos.size():"<<peer_infos.size()<<"need tcp:"<<need_tcp);
        }

        if (src_ip == g_trace_ip)
        {
            LOG4CPLUS_INFO(g_logger,"src ip:"<<src_ip<<" src location:"<<src_location<<"location_peers.size:"<<location_peers.size()<<" random list size:"<<random_peer.size()
                <<" rand_need_count:"<<rand_need_count<<" peer_infos.size():"<<peer_infos.size()<<"need tcp"<<need_tcp);
            for (unsigned i=0;i<peer_infos.size();++i)
            {
                LOG4CPLUS_INFO(g_logger,"peer info:"<<peer_infos[i]);
            }
        }

        LOG4CPLUS_TRACE(g_logger,"rand_need_count:"<<rand_need_count);

        //for (list<uint32_t>::iterator it = random_peer.begin();it != random_peer.end() && (rand_need_count>0);++it)
        int random_get_count = 0;
        for (RandomListContainerSquencedIndex::iterator it = random_list_sequenced_index.begin();it != random_list_sequenced_index.end() && (rand_need_count>random_get_count) ;++it)
        {
            PeerContainerMagicNumHashedIndex::iterator it_info = peer_magicnum_hash_index.find(*it);
            assert(it_info != peer_magicnum_hash_index.end());
            //这里不用担心时间计算溢出了。
            if (it_info->last_report_time + keep_alive_time < timenow)
            {
                //过期的节点
                remove_peer.insert(*it);
            }
            else if((need_tcp && IsSupportTcp(it_info->peer_info) ) || !need_tcp)
            {
                //--rand_need_count;
                ++random_get_count;
                SetPeerPriority(const_cast<PeerInfoEx&>(*it_info),src_ip,src_location,requirer_nat_type);
                DecBandWidth(const_cast<PeerInfo&>(it_info->peer_info));
                PushBackPeerInfo(peer_infos,it_info->peer_info,need_tcp);
            }
            else
            {
                //走到这里，是因为需要的是支持tcp的peer，但是遍历到的peer不支持tcp
                LOG4CPLUS_TRACE(g_logger,"need tcp peer:"<<need_tcp<<",and not support,peer upnp_tcp_port"<<it_info->peer_info.upnp_tcp_port);
                if (src_ip == g_trace_ip)
                {
                    LOG4CPLUS_INFO(g_logger,"need tcp peer:"<<need_tcp<<",and not support,peer upnp_tcp_port"<<it_info->peer_info.upnp_tcp_port);
                }
            }
        }


        if (random_peer.size() > max_location_peer_count + max_random_peer_count)
        {		
            //ck 2011.12.4修改移动的方法，使得返回均匀
            RandomListContainerSquencedIndex::iterator it = random_list_sequenced_index.begin();

            advance(it,random_get_count); 
            //把前面的random_get_count个元素移到最后去。
            random_list_sequenced_index.splice(random_list_sequenced_index.end(),random_list_sequenced_index,random_list_sequenced_index.begin(),it);          
        }

        //这个代码是调试的时候用的
        if (src_ip == g_trace_ip)
        {
            LOG4CPLUS_INFO(g_logger,"src ip:"<<src_ip<<" src location:"<<src_location<<"location_peers.size:"<<location_peers.size()<<" random list size:"<<random_peer.size()
                <<" rand_need_count:"<<rand_need_count<<" peer_infos.size():"<<peer_infos.size()<<"remove size:"<<remove_peer.size()<<" peer count:"<<peer_count<<" Rid:"<<rid_);
            for (unsigned i=0;i<peer_infos.size();++i)
            {
                LOG4CPLUS_INFO(g_logger,"peer info:"<<peer_infos[i]);
            }
        }
    }

    void PeerResource::ChangeLocation(uint32_t peer_id,uint32_t old_location,uint32_t new_location,int max_same_location_size)
    {
        //存在一定的可能，原来是不是广告文件，但是后来是了。这样locationmap会有一定的不必要的元素，但是无所谓。下次add的时候就会被清理掉的。
        RemovePeerFromLocationMap(peer_id,old_location);		
        AddLocationMap(peer_id,new_location,0,max_same_location_size,false);
    }

    void PeerResource::DecBandWidth(PeerInfo& peer_info)
    {
        peer_info.upload_speed_kbs = peer_info.upload_speed_kbs * 0.01 * dec_bandwidth_percent_
            + std::min<int>(peer_info.upload_limit_kbs, peer_info.upload_bandwidth_kbs) * 0.01 * (100-dec_bandwidth_percent_);
    }

    bool PeerResource::IsBandWidthLimit(const PeerInfo& peer_info) const
    {
        //有些版本的peer会上报负数，所以这里的计算，需要用int来。
        return peer_info.upload_speed_kbs + (int)bandwidth_limit_speed_ > std::min<int>(peer_info.upload_limit_kbs , peer_info.upload_bandwidth_kbs);
    }

};