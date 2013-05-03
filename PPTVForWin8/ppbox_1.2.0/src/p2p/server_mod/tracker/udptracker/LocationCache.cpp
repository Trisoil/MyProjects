// 这里是因为在GCC上模板的泛化不支持后置定义的查找
#ifndef WIN32
#include <stdlib.h>
#include <string.h>
#include <boost/system/error_code.hpp>
#include <boost/cstdlib.hpp>
#include <boost/cstdint.hpp>
#include "framework/string/Uuid.h"
#endif

#include "Common.h"
#include "LocationCache.h"
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
    extern unsigned g_trace_ip;
    extern map<int,int> g_proitity_num;
    extern map<std::string,int> g_status_num;

    LocationCache::LocationCache(bool adv_optimize,size_t max_randomlist_size,size_t max_same_location_size,size_t res_hash_map_size,size_t list_location_percent,
        size_t adv_peer_percent,size_t dec_bandwidth_percent,size_t bandwidth_limit_speed):
    adv_optimize_(adv_optimize),peer_id_hash_index_(peers_.get<0>()),peer_magicnum_hash_index_(peers_.get<1>()),
        res_id_hash_index_(resources_.get<0>()),res_macignum_hash_index_(resources_.get<1>())
#ifndef _MSC_VER
        ,all_res_(res_hash_map_size)
#endif


    {
        max_randomlist_size_ = max_randomlist_size;
        max_same_location_size_ = max_same_location_size;
        list_location_percent_ = list_location_percent;
        adv_peer_percent_ = adv_peer_percent;
        dec_bandwidth_percent_ = dec_bandwidth_percent;
        bandwidth_limit_speed_ = bandwidth_limit_speed;
    }

    LocationCache::~LocationCache()
    {
        Clear();
    }

    bool LocationCache::LoadFile(const std::string& loadfile)
    {
        LOG4CPLUS_INFO(g_logger, "begin to load file:"<<loadfile<<" tick"<<TickCounter::tick_count());
        //步骤:获取peerinfo，获取peer res，更新
        Guid peer_id;
        std::vector<RID> resource_ids;
        PeerInfo peer_info;

        ifstream loadfile_stream;
        loadfile_stream.open(loadfile.c_str());
        if (loadfile_stream.fail())
        {
            return false;
        }

        std::string line;
        while (getline(loadfile_stream,line)) 
        {
            //有很多行是其他信息的，不管
            if (line.find("Address:") == std::string::npos)
            {
                continue;
            }

            /*
            正常的格式
            20E82B81B1494FCE93CB7D553A8436E3|Address: 192.168.1.100:5041, Detected Address: 218.90.43.172:1047, Stun Address: 0.0.0.0:0, PeerVersion: 263, UploadPriority: 7|29\
            |A0D9A7FABC01C70E1192C8D12CD01F12|39D544EBF7236EC0414F9192EC3D7B45|B22636E94C2BEAAC0D2C86EACEF05D52|034BA4E58B9E51C96D77F189240B551C|D68F69DCCBB39AE871C4047734A9C961\
            |D89A97D1E5834DAFC1D5B1E6EF5915A5|A43897CA07F9A996ADF434D6427615A7|EFC799C3B6A4EC4D596D67AC54370DA8|14FE83BD7213C34BCD0E8B29747E2DF5|A6E7A2B40CBAE45445FB4233A1CE04AB|\
            FED7BCA34C4AFFB04D7FB6BA776690ED|F3BE4398DFD3F809D1A2C8F80CFC7350|B929059601DDB07681C77A151FA5D3F4|D1B5C38946C4A6E42DD6713E23A06F12|4D234C86542A4445190E52CEB75BF636|\
            23369B7AD8EBE754CDA96FE9D4FB7203|C0BED37356CD9A4059E651BFCAF79249|B80FF0716C5E050DC5B25AF735E2B696|0446C369EBE8DB93E11D3187F318B331|E016BB6835FF5FC655949415B48AC6B7|\
            527E0267CC59DEA7A90A6890DB11508C|1EAFDD6570948B9539F2F42283DD8767|EC3CB4565283D8E19511DF232C47269E|32E01A4FE86E4B0EB9ED0F7CFE1BC868|0F4A623B968038758975404149DD5981|\
            4A518D1C16D7ECA9FD1E92F89DC5BF3B|5C55731424466A437520A0833BE1F956|0447D111ACC5A78515BB49588BB506B4|F25A9D09069A759A29B6F1B833FD1C24|
            支持tcp的正常格式
            20E82B81B1494FCE93CB7D553A8436E3|Address: 192.168.1.100:5041-5042, Detected Address: 218.90.43.172:1047-1048, Stun Address: 0.0.0.0:0, PeerVersion: 265, UploadPriority: 7|29\
            |A0D9A7FABC01C70E1192C8D12CD01F12|39D544EBF7236EC0414F9192EC3D7B45|B22636E94C2BEAAC0D2C86EACEF05D52|034BA4E58B9E51C96D77F189240B551C|D68F69DCCBB39AE871C4047734A9C961\
            |D89A97D1E5834DAFC1D5B1E6EF5915A5|A43897CA07F9A996ADF434D6427615A7|EFC799C3B6A4EC4D596D67AC54370DA8|14FE83BD7213C34BCD0E8B29747E2DF5|A6E7A2B40CBAE45445FB4233A1CE04AB|\
            FED7BCA34C4AFFB04D7FB6BA776690ED|F3BE4398DFD3F809D1A2C8F80CFC7350|B929059601DDB07681C77A151FA5D3F4|D1B5C38946C4A6E42DD6713E23A06F12|4D234C86542A4445190E52CEB75BF636|\
            23369B7AD8EBE754CDA96FE9D4FB7203|C0BED37356CD9A4059E651BFCAF79249|B80FF0716C5E050DC5B25AF735E2B696|0446C369EBE8DB93E11D3187F318B331|E016BB6835FF5FC655949415B48AC6B7|\
            527E0267CC59DEA7A90A6890DB11508C|1EAFDD6570948B9539F2F42283DD8767|EC3CB4565283D8E19511DF232C47269E|32E01A4FE86E4B0EB9ED0F7CFE1BC868|0F4A623B968038758975404149DD5981|\
            4A518D1C16D7ECA9FD1E92F89DC5BF3B|5C55731424466A437520A0833BE1F956|0447D111ACC5A78515BB49588BB506B4|F25A9D09069A759A29B6F1B833FD1C24|
            */

            //boost::algorithm::split(rids_, sample_rid_string, boost::algorithm::is_any_of(";,@ "));
            std::vector<std::string> sector;
            //cout<<"line:"<<line<<endl;
            boost::algorithm::split(sector,line,boost::algorithm::is_any_of("|:,"),boost::algorithm::token_compress_on);
            for(unsigned i=0;i<sector.size();++i)
            {
                boost::algorithm::trim(sector[i]);
            }
            if(sector.size() <15)
            {
                continue;
            }
            //peer_id
            peer_id.from_string(sector[0]);
            //Address 不要这个，跳过sector[1]
            //192.168.1.100
            peer_info.IP = htonl(inet_addr(sector[2].c_str()));

            
            //5041
            //peer_info.UdpPort = atoi(sector[3].c_str());
            size_t tmppos = sector[3].find('-');           
            if (tmppos != string::npos)
            { 
                peer_info.UdpPort = atoi(string(sector[3].c_str(),tmppos).c_str());
                peer_info.internal_tcp_port = atoi(string(sector[3].c_str() + tmppos+1).c_str());
            }
            else
            {
                peer_info.UdpPort = atoi(sector[3].c_str());              
            }

            //Detected Address 不要这个，跳过sector[4]
            //218.90.43.172
            peer_info.DetectIP = htonl(inet_addr(sector[5].c_str()));
            //peer_info.DetectUdpPort = atoi(sector[6].c_str());

            tmppos = sector[6].find('-');
           
            if (tmppos != string::npos)
            { 
                peer_info.DetectUdpPort = atoi(string(sector[6].c_str(),tmppos).c_str());
                peer_info.upnp_tcp_port = atoi(string(sector[6].c_str() + tmppos + 1).c_str());
            }
            else
            {
                peer_info.DetectUdpPort = atoi(sector[6].c_str());
            }

            //Stun Address 不要这个，跳过sector[7]
            //0.0.0.0
            peer_info.StunIP = htonl(inet_addr(sector[8].c_str()));
            //0
            peer_info.StunUdpPort = atoi(sector[9].c_str());
            //PeerVersion 不要这个，跳过sector[10]
            //263
            peer_info.PeerVersion = atoi(sector[11].c_str());
            //UploadPriority 不要这个，跳过sector[12]
            //7
            peer_info.UploadPriority = atoi(sector[13].c_str());
            //29
            int ridnum = atoi(sector[14].c_str());
            //最后一个-1，是因为格式里最后还有一个|
            if(ridnum != sector.size() - 15 -1 || 0 == ridnum)
            {
                continue;
            }
            //vector<RID> rids;
            set<RID> rids;
            //rids.reserve(ridnum);
            for(int i=0;i<ridnum;++i)
            {
                //rids.push_back(RID(sector[15+i]));
                rids.insert(RID(sector[15+i]));
            }
            UpdatePeerInfo(peer_id, peer_info);			
            UpdatePeerResources(peer_id, rids);
        }		
        cout<<"after load,peer size:"<<peers_.size()<<" resouce size:"<<resources_.size()<<endl;
        LOG4CPLUS_INFO(g_logger, "after load,peer size:"<<peers_.size()<<" resouce size:"<<resources_.size()<<" tick"<<TickCounter::tick_count());

        return true;
    }

    uint32_t LocationCache::Rid2MagicNum(const RID& rid) const
    {
        ResContainerRidHashedIndex::iterator it = res_id_hash_index_.find(rid);
        if(it == res_id_hash_index_.end())
        {
            return 0;
        }
        return it->res_magicnum;
    }

    uint32_t LocationCache::Guid2MagicNum(const Guid& gid) const
    {
        PeerContainerPeeridHashedIndex::iterator it = peer_id_hash_index_.find(gid);
        if(it == peer_id_hash_index_.end())
        {
            return 0;
        }
        return it->peer_magicnum;
    }

    RID	LocationCache::MagicNum2Rid(uint32_t magicnum) const
    {
        ResContainerMagicNumHashedIndex::iterator it = res_macignum_hash_index_.find(magicnum);
        if(it == res_macignum_hash_index_.end())
        {
            assert(false);			
        }
        return it->rid;
    }
    Guid LocationCache::MagicNum2Guid(uint32_t magicnum) const
    {
        PeerContainerMagicNumHashedIndex::iterator it = peer_magicnum_hash_index_.find(magicnum);
        if(it == peer_magicnum_hash_index_.end())
        {
            //不应该走到这里
            assert(false);			
        }
        return it->peer_id;
    }

    int LocationCache::ClearChangeIpNum()
    {
        int ret = change_ip_num_;
        change_ip_num_ = 0;
        return ret;
    }

    int LocationCache::ClearChangeLocationNum()
    {
        int ret = change_location_num_;
        change_location_num_ = 0;
        return ret;
    }

    map<int,int> LocationCache::ClearListRetNum()
    {
        map<int,int> tmp;
        tmp.swap(list_ret_num_);
        return tmp;
    }

    map<int,int> LocationCache::ClearListTcpRetNum()
    {
        map<int,int> tmp;
        tmp.swap(listtcp_ret_num_);
        return tmp;
    }

    void LocationCache::Clear()
    {
        all_res_.clear();
        peers_.clear();
        resources_.clear();		
    };
    void LocationCache::Insert(const Guid& peer_id, const RID& resource_id)
    {
        //暂时没有看到这个函数的调用，先不实现
        assert(false);
    };
    // remove resource and node,这个函数会修改peer_ 
    void LocationCache::RemovePeer(const Guid& peer_id)
    {
        //查找peer是否存在，不存在就不管了。
        PeerContainerPeeridHashedIndex::iterator it = peer_id_hash_index_.find(peer_id);
        if (it == peer_id_hash_index_.end())
        {
            LOG4CPLUS_DEBUG(g_logger,"remove peer:"<<peer_id<<" not exist");
            ++g_status_num[LEAVENOTEXIST];
            return;
        }		
        ++g_status_num[LEAVEEXIST];

        uint32_t magic_pid = it->peer_magicnum;
        assert(magic_pid != 0);
        //找到peer的信息
        const PeerInfoEx& peer_info_ex = *it;
        //删除peer对应的resource
        for(uint32_t i =0;i<peer_info_ex.resources.size();++i)
        {			
            //最后一个参数传入了false，是因为因为这整个peer下面都不要了，peer对应的vector自然也不用关心是否清理了。
            DelPeerResource(magic_pid,peer_info_ex.resources[i],false);
        }

        //删除peer
        peer_id_hash_index_.erase(it);
        LOG4CPLUS_TRACE(g_logger,"remove peer:"<<peer_id);

    }

    uint32_t LocationCache::Peerid2Location(uint32_t peer_id)
    {
        return Ip2Location(GetPeerInfoEx(peer_id).peer_info.DetectIP);
    }

    void LocationCache::DelPeerResource(boost::uint32_t peer_id, boost::uint32_t resource_id,bool change_peer_res_vec)
    {
        assert(resource_id != 0 && peer_id != 0);
        hash_map<boost::uint32_t,PeerResource>::iterator it = all_res_.find(resource_id);
        if(it == all_res_.end())
            return;
        it->second.RemovePeer(peer_id,Peerid2Location(peer_id));

        //这个资源已经没有peer有了，清理掉吧
        if (0 == it->second.peer_count)
        {
            //这个rid，已经没有任何peer资源了
            all_res_.erase(it);
            ResContainerMagicNumHashedIndex::iterator it_res = res_macignum_hash_index_.find(resource_id);
            assert(it_res != res_macignum_hash_index_.end());
            res_macignum_hash_index_.erase(it_res);
        }

        if (change_peer_res_vec)
        {
            //再删掉peerinfo里面的资源,这里的删除，需要使用替换来优化
            vector<boost::uint32_t>& peer_res = GetPeerInfoEx(peer_id).resources;		
            vector<boost::uint32_t>::iterator it_res = find(peer_res.begin(),peer_res.end(),resource_id);
            assert( it_res != peer_res.end());
            *it_res = *peer_res.rbegin();
            peer_res.resize(peer_res.size() -1 );
        }

        return;
    }

    const PeerInfoEx& LocationCache::GetPeerInfoEx(uint32_t peer_id) const
    {
        PeerContainerMagicNumHashedIndex::iterator it = peer_magicnum_hash_index_.find(peer_id);
        if (it == peer_magicnum_hash_index_.end())
        {
            assert(false);			
        }
        return *it;
    }

    PeerInfoEx& LocationCache::GetPeerInfoEx(uint32_t peer_id)
    {
        PeerContainerMagicNumHashedIndex::iterator it = peer_magicnum_hash_index_.find(peer_id);
        if (it == peer_magicnum_hash_index_.end())
        {
            assert(false);			
        }
        //这个转换似乎有点丑陋？暂时没有更好地办法
        return const_cast<PeerInfoEx&>(*it);
    }

    //插入peer对应的资源，前提是peer存在，而且rid存在
    void LocationCache::InsertPeerRes(uint32_t peer_id,uint32_t magic_rid)
    {
        assert(peer_id != 0 && magic_rid != 0);

        vector<boost::uint32_t>& peer_res = GetPeerInfoEx(peer_id).resources;

        //这个assert比较耗时，所以测试没有问题的话，应该删掉再上线。
        //assert(find(peer_res.begin(),peer_res.end(),rid) == peer_res.end());

        peer_res.push_back(magic_rid);

        hash_map<boost::uint32_t,PeerResource>::iterator it = all_res_.find(magic_rid);
        if(it == all_res_.end())
        {
            it = all_res_.insert(make_pair(magic_rid,PeerResource(MagicNum2Rid(magic_rid),dec_bandwidth_percent_,bandwidth_limit_speed_))).first;	
        }

        int ad_peer_count = std::max<int>(peer_magicnum_hash_index_.size()*0.01*adv_peer_percent_,1000);
        LOG4CPLUS_TRACE(g_logger,"InsertPeerRes, peer id:"<<peer_id<<" rid:"<<magic_rid<<" rid:"<<MagicNum2Rid(magic_rid));

        //void AddPeer(boost::uint32_t peer_id,boost::uint32_t location,int ad_peer_count,const PeerContainerMagicNumHashedIndex& peer_magicnum_hash_index,
        //	int max_randomlist_size = 500,int max_same_location_size=50,bool ad_optimize = false,bool exist_check=false)
        return it->second.AddPeer(peer_id,Peerid2Location(peer_id), ad_peer_count,peer_magicnum_hash_index_,max_randomlist_size_,max_same_location_size_,adv_optimize_);

    }

    void LocationCache::UpdatePeerResources(const Guid& peer_id, const set<RID>& resources,bool process_delete)
    {
        //如果peer不存在，插入
        uint32_t magic_pid = Guid2MagicNum(peer_id);		
        if (0 == magic_pid)
        {
            //不应该出现没有peer信息的情况
            assert(false);
        }

        //peer存在,比较新增的peer
        PeerInfoEx& peer_info_ex = GetPeerInfoEx(magic_pid);

        //传入的参数，是一个rid的迭代器，需要转换成magicnum
        set<uint32_t> report_res;//(resource_first,resource_last);
        //for (_Iter it_r= resource_first; it_r!=resource_last; ++it_r)
        for (set<RID>::const_iterator it_r =resources.begin();it_r != resources.end();++it_r)
        {
            uint32_t magic_rid = Rid2MagicNum(*it_r);
            if(0 == magic_rid)
            {
                magic_rid = AddResInfo(*it_r);
            }
            assert(magic_rid != 0);
            report_res.insert(magic_rid);
        }

        //新增加的res
        set<uint32_t> new_peer_res;
        //要删除的res
        set<uint32_t> delete_peer_res;

        if(0 != peer_info_ex.resources.size())
        {
            set<uint32_t> former_peer_res(peer_info_ex.resources.begin(),peer_info_ex.resources.end());
            set_difference(report_res.begin(),report_res.end(),former_peer_res.begin(),former_peer_res.end(),std::inserter(new_peer_res, new_peer_res.begin()));
            if(process_delete)
            {
                set_difference(former_peer_res.begin(),former_peer_res.end(),report_res.begin(),report_res.end(),std::inserter(delete_peer_res, delete_peer_res.begin()));
            }
        }
        else
        {		
            new_peer_res.swap(report_res);
            //原先就没有res，因此也不需要delete了,所以不设置delete_peer_res
        }

        LOG4CPLUS_TRACE(g_logger,"peer is:"<<peer_id<<"insert res size:"<<new_peer_res.size()<<" remove peer size:"<<delete_peer_res.size());

        //插入资源
        for(set<uint32_t>::iterator it = new_peer_res.begin();it != new_peer_res.end();++it)
        {
            InsertPeerRes(magic_pid,*it);
        }

        for(set<uint32_t>::iterator it = delete_peer_res.begin();it != delete_peer_res.end();++it)
        {			
            DelPeerResource(magic_pid,*it);
        }
    }

    void LocationCache::UpdatePeerResources(const Guid& peer_id, const vector<RID>& resources,bool process_delete)
    {
        set<RID> res(resources.begin(),resources.end());
        UpdatePeerResources(peer_id,res,process_delete);
        return;
    }
    size_t LocationCache::PeerResourcesCount(const Guid& peer_id) const
    {
        uint32_t magic_pid = Guid2MagicNum(peer_id);
        return 0 == magic_pid ? 0 :(GetPeerInfoEx(magic_pid).resources.size());
    }

    //这个函数不是很有用，所以不需要精确的数据
    size_t LocationCache::ResourcePeersCount(const RID& resource_id) const
    {
        uint32_t magic_rid = Rid2MagicNum(resource_id);
        if (0 == magic_rid)
            return 0;
        hash_map<boost::uint32_t,PeerResource>::const_iterator it = all_res_.find(magic_rid);
        if(it == all_res_.end())
            return 0;
        return it->second.peer_count;
    };

    bool LocationCache::HasPeer(const Guid& peer_id) const
    {
        return 0 != Guid2MagicNum(peer_id);
    }

    bool LocationCache::HasResource(const RID& resource_id) const
    {
        return 0 != Rid2MagicNum(resource_id);
    };

    // whether peer contains resource
    bool LocationCache::HasResource(const Guid& peer_id, const RID& resource_id) const
    {
        //理论上没有地方会用这个函数，但是这个函数在调试和单元测试的时候很有用，所以留下了
        if ( !HasPeer(peer_id) || !HasResource(resource_id))
        {
            return false;
        }
        const PeerInfoEx& peer_info = GetPeerInfoEx(Guid2MagicNum(peer_id));
        return find(peer_info.resources.begin(),peer_info.resources.end(),Rid2MagicNum(resource_id)) != peer_info.resources.end();		
    };

    framework::timer::TickCounter::count_value_type LocationCache::GetPeerKPLTime(const Guid& peer_id) const
    {
        uint32_t magic_pid = Guid2MagicNum(peer_id);
        return 0 == magic_pid ? 0 :(GetPeerInfoEx(magic_pid).last_report_time);		
    };
    void LocationCache::UpdatePeerKPLTime(const Guid& peer_id)
    {
        uint32_t magic_pid = Guid2MagicNum(peer_id);
        if(0 == magic_pid)
        {
            return;
        }
        GetPeerInfoEx(magic_pid).last_report_time = framework::timer::TickCounter::tick_count();		
    };

    uint32_t LocationCache::UpdatePeerInfo(const Guid& peer_id, const PeerInfo& peer_info)
    {
        uint32_t magic_pid = Guid2MagicNum(peer_id);		

        //如果没有就创建一个
        if (magic_pid == 0)
        {
            magic_pid = peer_id.hash_value();
            //生成的魔数不能为0，且不能和已有的一样
            while (magic_pid == 0 || peer_magicnum_hash_index_.find(magic_pid) != peer_magicnum_hash_index_.end())
            {
                srand(time(NULL));
                magic_pid += rand();
            }
            PeerInfoEx peer_info_ex;
            peer_info_ex.peer_id = peer_id;
            peer_info_ex.last_report_time = framework::timer::TickCounter::tick_count();
            peer_info_ex.first_report_time = peer_info_ex.last_report_time;
            peer_info_ex.peer_magicnum = magic_pid;
            peer_info_ex.peer_info = peer_info;
            peer_info_ex.peer_info.peer_location = Ip2Location(peer_info.DetectIP);
            peers_.insert(peer_info_ex);

            LOG4CPLUS_TRACE(g_logger,"UpdatePeerInfo insert "<<peer_id);

        }
        else //已经存在，需要更新
        {
            PeerInfoEx& peer_info_ex = GetPeerInfoEx(magic_pid);
            unsigned new_location =  Ip2Location(peer_info.DetectIP);
            if (peer_info_ex.peer_info.peer_location != new_location)
            {
                //换到一个新的区域，需要变更locationmap，这种情况应该比较少，出现可能和peerid重复有关系。
                LOG4CPLUS_INFO(g_logger,"old info: "<<peer_info_ex.peer_info<<" new info:"<<peer_info);
                ChangePeerLocation(magic_pid,peer_info_ex.peer_info.peer_location,new_location);								
                ++change_location_num_;
            }
            else if(peer_info_ex.peer_info.DetectIP != peer_info.DetectIP)
            {
                ++change_ip_num_;
            }


            peer_info_ex.peer_info = peer_info;
            //因为DetectIP 可能变化了，所以要重新计算
            peer_info_ex.peer_info.peer_location = new_location;
            peer_info_ex.last_report_time = framework::timer::TickCounter::tick_count();
            LOG4CPLUS_TRACE(g_logger,"UpdatePeerInfo change "<<peer_id);
        }
        return magic_pid;
    };

    void LocationCache::ChangePeerLocation(uint32_t peer_id,uint32_t old_location,uint32_t new_location)
    {
        assert(peer_id != 0);

        vector<uint32_t>&  resource = GetPeerInfoEx(peer_id).resources;
        for(uint32_t i =0;i<resource.size();++i)
        {
            hash_map<boost::uint32_t,PeerResource>::iterator it = all_res_.find(resource[i]);
            assert(it != all_res_.end());
            it->second.ChangeLocation(peer_id,old_location,new_location,max_same_location_size_);			
        }
    }


    boost::uint32_t LocationCache::AddResInfo(const RID& rid)
    {
        if(res_id_hash_index_.find(rid) != res_id_hash_index_.end())
        {
            //已经存在这个rid的相关信息了
            LOG4CPLUS_ERROR(g_logger,"add resource info"<<rid<<" exist");
            return 0;
        }

        LOG4CPLUS_TRACE(g_logger,"add resource info"<<rid);

        //不存在，创建一个magic_num
        uint32_t magic_rid = rid.hash_value();
        while (magic_rid == 0 || res_macignum_hash_index_.find(magic_rid) != res_macignum_hash_index_.end())
        {
            srand(time(NULL));
            magic_rid += rand();
        }

        ResInfo res_info;
        res_info.res_magicnum = magic_rid;
        res_info.rid = rid;
        resources_.insert(res_info);
        return magic_rid;

    }

    void LocationCache::ListResourceAlivePeers(vector<protocol::CandidatePeerInfo>& peer_infos,const Guid& resource_id, size_t max_alive_peer_count,int keep_alive_time,
        const boost::asio::ip::udp::endpoint& end_point,boost::uint8_t requirer_nat_type)
    {
        //查询资源 并设置资源的优先级
        peer_infos.clear();
        uint32_t magic_rid = Rid2MagicNum(resource_id);
        if(0 == magic_rid)
        {
            //这个rid没有任何peer
            ++list_ret_num_[-1];
            return;
        }
        hash_map<boost::uint32_t,PeerResource>::iterator it = all_res_.find(magic_rid);
        assert(it != all_res_.end());

        set<uint32_t> remove_peer;
        it->second.ListPeer(peer_infos,end_point.address().to_v4().to_ulong(),requirer_nat_type,max_alive_peer_count*list_location_percent_/100,
            max_alive_peer_count*(100-list_location_percent_)/100,keep_alive_time,peer_magicnum_hash_index_,remove_peer,false);

        LOG4CPLUS_DEBUG(g_logger, "list get:"<<peer_infos.size()<<" remove:"<<remove_peer.size());

        g_status_num[LISTDEAD] += remove_peer.size();

         //移除过期节点
        RemovePeer(remove_peer);
        ++list_ret_num_[peer_infos.size()];
    }

    void LocationCache::ListResourceAliveTcpPeers(vector<protocol::CandidatePeerInfo>& peer_infos,const Guid& resource_id, size_t max_alive_peer_count,int keep_alive_time,
        const boost::asio::ip::udp::endpoint& end_point,boost::uint8_t requirer_nat_type)
    {
        //查询资源 并设置资源的优先级
        peer_infos.clear();
        uint32_t magic_rid = Rid2MagicNum(resource_id);
        if(0 == magic_rid)
        {
            //这个rid没有任何peer
            ++listtcp_ret_num_[-1];
            return;
        }
        hash_map<boost::uint32_t,PeerResource>::iterator it = all_res_.find(magic_rid);
        assert(it != all_res_.end());

        set<uint32_t> remove_peer;
        it->second.ListPeer(peer_infos,end_point.address().to_v4().to_ulong(),requirer_nat_type,max_alive_peer_count*list_location_percent_/100,
            max_alive_peer_count*(100-list_location_percent_)/100,keep_alive_time,peer_magicnum_hash_index_,remove_peer,true);

        LOG4CPLUS_DEBUG(g_logger, "list tcp get:"<<peer_infos.size()<<" remove:"<<remove_peer.size());

        g_status_num[LISTDEAD] += remove_peer.size();


       //移除过期节点
        RemovePeer(remove_peer);

        ++listtcp_ret_num_[peer_infos.size()];
    }

    void LocationCache::RemovePeer(std::set<uint32_t> remove_peers)
    {
       
        //移除过期节点
        for(set<uint32_t>::iterator it = remove_peers.begin();it != remove_peers.end();++it)
        {
            RemovePeer(MagicNum2Guid(*it));
        }
    }


    size_t LocationCache::RemoveNonAlivePeers(size_t keep_alive_time,size_t max_remove_unlive_peer_per_time,size_t max_travel_peer_per_time)
    {
        //删除的过程中，迭代器不是很可靠,可能会和实现有关，所以先把要删除的东西丢到一个vector，然后再删掉，同时记录下一个要处理的位置
        //这里用了一个static，记录下次要处理的guid
        static Guid cur_process = Guid::null();

        PeerContainerPeeridHashedIndex::iterator it = peer_id_hash_index_.find(cur_process);

        //对于cur_process是否是null的判断，主要是担心真的有为NULL的guid存在,如果find为end，说明这个节点被删掉了，再重新遍历就好了。
        if(cur_process == Guid::null() || it == peer_id_hash_index_.end())
        {
            it = peer_id_hash_index_.begin();
        }

        vector<Guid> remove_peer;
        remove_peer.reserve(max_remove_unlive_peer_per_time);
        PeerContainerPeeridHashedIndex::iterator it_end = peer_id_hash_index_.end();
        framework::timer::TickCounter::count_value_type timenow = framework::timer::TickCounter::tick_count();

        size_t travel_num = 0;
        while((it_end != it) && (remove_peer.size() < max_remove_unlive_peer_per_time) && (travel_num < max_travel_peer_per_time ))
        {
            //这里不用担心时间计算循环了。
            if(it->last_report_time + keep_alive_time < timenow)
            {
                remove_peer.push_back(it->peer_id);
            }
            //看一下有没有非法的ip
            if (it->peer_info.DetectIP > 3979530000U)
            {
                LOG4CPLUS_INFO(g_logger, "find a illegal ip"<<it->peer_info.DetectIP);
            }
            ++it;
            ++travel_num;
        }

        if(it == it_end)
        {
            cur_process = Guid::null();
        }
        else
        {
            cur_process = it->peer_id;
        }
        LOG4CPLUS_DEBUG(g_logger,"try to remove "<<remove_peer.size()<<" time used:"<<framework::timer::TickCounter::tick_count() - timenow);
        for(size_t i = 0;i<remove_peer.size();++i)
        {
            RemovePeer(remove_peer[i]);
        }

        LOG4CPLUS_INFO(g_logger, "remove num"<<remove_peer.size()<<" travel num" <<travel_num<<" cur_process:"<<cur_process<<" time used:"<<framework::timer::TickCounter::tick_count() - timenow
            <<" Iplocation found ip:"<<IpLocation::Instance()->ClearIpFound()<<" Iplocation not found ip:"<<IpLocation::Instance()->ClearIpNotFound()<<" resource size:"<<all_res_.size()
            <<" peer size:"<<peers_.size()<<" change ip num:"<<ClearChangeIpNum()<<" change location num:"<<ClearChangeLocationNum());

        //下面做的事情，只是输出一些统计信息
        int tmptotal=0;
        for(map<int,int>::iterator it = list_ret_num_.begin();it != list_ret_num_.end();++it)
        {
            LOG4CPLUS_INFO(g_logger, "list return num:"<<it->first<<" "<<it->second);
            tmptotal += it->second;
        }
        LOG4CPLUS_INFO(g_logger,"list ret num size:"<<ClearListRetNum().size()<<" total:"<<tmptotal);
        
        tmptotal=0;
        for(map<int,int>::iterator it = listtcp_ret_num_.begin();it != listtcp_ret_num_.end();++it)
        {
            LOG4CPLUS_INFO(g_logger, "list tcp return num:"<<it->first<<" "<<it->second);
            tmptotal += it->second;
        }
        LOG4CPLUS_INFO(g_logger,"list ret num size:"<<ClearListTcpRetNum().size()<<" total:"<<tmptotal);

        tmptotal=0;
        for(map<int,int>::iterator it = g_proitity_num.begin();it != g_proitity_num.end();++it)
        {
            LOG4CPLUS_INFO(g_logger, "proiority num:"<<it->first<<" "<<it->second);
            tmptotal += it->second;
        }
        LOG4CPLUS_INFO(g_logger,"proiority num size:"<<ClearPriorityNum().size()<<" total:"<<tmptotal);


        for(map<std::string,int>::iterator it = g_status_num.begin();it != g_status_num.end();++it)
        {
            LOG4CPLUS_INFO(g_logger, "status :"<<it->first<<" num:"<<it->second);
        }
        LOG4CPLUS_INFO(g_logger,"status num size:"<<ClearStatusNum().size());

        return remove_peer.size();
    }

    void LocationCache::DumpAllPeersDetailInfo(std::ofstream & f)
    {
        f << "begin dump peer detail" << "\r\n";
        PeerContainerPeeridHashedIndex::iterator peer_iter;
        for (peer_iter = peer_id_hash_index_.begin(); peer_iter != peer_id_hash_index_.end(); ++peer_iter)
        {           

            const PeerInfoEx& peerinfo =  *peer_iter;
            f << peerinfo.peer_id << "|";
            f << peerinfo.peer_info << "|";
            f << peerinfo.resources.size() << "|";
            f << peerinfo.peer_info.upload_bandwidth_kbs << "|";
            f << peerinfo.peer_info.upload_limit_kbs << "|";
            f << peerinfo.peer_info.upload_speed_kbs << "|";
            f << "location:"<< peerinfo.peer_info.peer_location << "|";
            //for( size_t i = 0; i < resources.size();i++ ) {
            //    //
            //    f << resources[i] << "|";
            //}

            f << "\r\n";
        }
    }
};
