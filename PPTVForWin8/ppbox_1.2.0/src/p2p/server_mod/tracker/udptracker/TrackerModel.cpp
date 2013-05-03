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
#include "udptracker/TrackerModel.h"
#include "udptracker/TrackerLogger.h"
#include "udptracker/IpLocation.h"
#include <boost/tokenizer.hpp>

extern log4cplus::Logger g_logger;
extern unsigned g_trace_ip;

using namespace framework::timer;
using namespace std;



namespace udptracker
{
    size_t TrackerModel::keep_alive_interval_in_seconds_;
    size_t TrackerModel::keep_alive_exceeded_in_seconds_;
    size_t TrackerModel::max_list_peer_count_;
    int TrackerModel::rid_count_prority_limit;
    int TrackerModel::rid_count_prority_max;
    int TrackerModel::inner_2_public_priority;
    int TrackerModel::public_2_inner_priority;

    //判断上报频率是否过于频繁
    size_t TrackerModel::min_keep_alive_interval_in_milliseconds_;

    std::map<int , int> TrackerModel::priority_bandwith;
    int TrackerModel::priority_isp_[6];

    TrackerModel::p TrackerModel::Create(const IResourcePeerCountHandler& handler)
    {
        return TrackerModel::p(new TrackerModel(handler));
    }

    TrackerModel::TrackerModel(const IResourcePeerCountHandler& handler)
        : handler(handler) , is_running_(false)
    {
        bool ad_optimize = Config::Inst().GetBoolean("tracker.ad_optimize",true);
        size_t max_randomlist_size = Config::Inst().GetInteger("tracker.max_randomlist_size",500);
        if (max_randomlist_size > 200000 || max_randomlist_size <50)
        {
            max_randomlist_size = 500;
        }
        size_t max_same_location_size = Config::Inst().GetInteger("tracker.max_same_location_size",30);

        if (max_same_location_size > 2000 || max_same_location_size<5)
        {
            max_same_location_size = 100;
        }


        size_t res_hash_map_size = Config::Inst().GetInteger("tracker.res_hash_map_size",301057);
        if (res_hash_map_size > 1000*10000 && res_hash_map_size<1000)
        {
            res_hash_map_size = 301057;
        }
        int list_location_percent = Config::Inst().GetInteger("tracker.list_location_percent",70);		
        if(list_location_percent<0 || list_location_percent > 100)
        {
            list_location_percent = 0;
        }
        int adv_peer_percent = Config::Inst().GetInteger("tracker.adv_peer_percent",5);		
        if(adv_peer_percent <0 || adv_peer_percent > 20)
        {
            adv_peer_percent = 5;
        }

        int dec_bandwidth_percent = Config::Inst().GetInteger("tracker.dec_bandwidth_percent",30);		
        if(dec_bandwidth_percent <0 || dec_bandwidth_percent > 100)
        {
            dec_bandwidth_percent = 30;
        }

        int bandwidth_limit_speed = Config::Inst().GetInteger("tracker.bandwidth_limit_speed",50);		
        if(bandwidth_limit_speed <0 || bandwidth_limit_speed > 500)
        {
            bandwidth_limit_speed = 50;
        }

        //(bool ad_optimize=true,size_t max_randomlist_size = 500,size_t max_same_location_size=30)
        container_ = LocationCache::Create(ad_optimize,max_randomlist_size,max_same_location_size,res_hash_map_size,list_location_percent,adv_peer_percent,dec_bandwidth_percent,bandwidth_limit_speed);

        rid_count_prority_limit = Config::Inst().GetInteger("tracker.rid_count_prority_limit",200);		
        if(rid_count_prority_limit <0 || rid_count_prority_limit > 1000)
        {
            bandwidth_limit_speed = 200;
        }

        rid_count_prority_max = Config::Inst().GetInteger("tracker.rid_count_prority_max",50);		
        if(rid_count_prority_max <0 || rid_count_prority_max > 100)
        {
            rid_count_prority_max = 50;
        }

        inner_2_public_priority = Config::Inst().GetInteger("tracker.inner_2_public_priority",0);
        if(inner_2_public_priority <0 || inner_2_public_priority>50)
        {
            inner_2_public_priority = 0;
        }

        public_2_inner_priority = Config::Inst().GetInteger("tracker.public_2_inner_priority",0);
        if(public_2_inner_priority <0 || public_2_inner_priority>50)
        {
            public_2_inner_priority = 0;
        }

        string trace_ip = Config::Inst().GetTString("tracker.trace_ip","122.225.35.103");
        boost::asio::ip::address_v4 av4;
        av4 = boost::asio::ip::address_v4::from_string(trace_ip);
        if(0 != av4.to_ulong() )
        {
            g_trace_ip = av4.to_ulong();
        }

        LOG4CPLUS_INFO(g_logger, "ad_optimize:"<<int(ad_optimize)<<" max_randomlist_size:"<<max_randomlist_size<<" max_same_location_size:"<<max_same_location_size
            <<" res_hash_map_size:"<<res_hash_map_size<<" trace ip:"<<g_trace_ip<<" adv peer percent:"<<adv_peer_percent
            <<" dec_bandwidth_percent:"<<dec_bandwidth_percent<<" bandwidth_limit_speed:"<<bandwidth_limit_speed<<" rid_count_prority_limit:"
            <<rid_count_prority_limit<<" rid_count_prority_max:"<<rid_count_prority_max);
    }

    void TrackerModel::Start()
    {
        LOG4CPLUS_INFO(g_logger, "Starting Tracker Model.");

        if (is_running_ )
        {
            LOG4CPLUS_WARN(g_logger,"TrackerModel::Start Tracker Model is running. Return.");
            return ;
        }

        Clear();

        string IpLocationFile = Config::Inst().GetTString("tracker.iplocation", "ip_distribution.dat");

        IpLocation::Instance(IpLocationFile);

        max_remove_dead_peer_per_times_ = Config::Inst().GetInteger("tracker.max_remove_unlive_peer_per_times", DEFAULT_MAX_REMOVE_DEAD_PEER_PER_TIMES);
        max_travel_peer_per_times_ = Config::Inst().GetInteger("tracker.max_travel_peer_per_times", DEFAULT_MAX_TRAVEL_PEER_PER_TIMES);
        max_remove_dead_peer_time_elapsed_ = Config::Inst().GetInteger("tracker.max_remove_unlive_peer_time_elapsed", DEFAULT_MAX_REMOVE_DEAD_PEER_TIME_ELAPSED);

        container_->Clear();
        string tracker_loadfile = Config::Inst().GetTString("tracker.loadfile", "tracker_dump.log");
        if(container_->LoadFile(tracker_loadfile))
        {
            CONSOLE_OUTPUT("================ load file success  ================");
        }
        else
        {
            CONSOLE_OUTPUT("================ load file fail,file:"<<  tracker_loadfile <<"================");
        }

        //BeginCheckResources();


        keep_alive_interval_in_seconds_ = 
            Config::Inst().GetInteger("tracker.keep_alive_interval_in_seconds", KEEP_ALIVE_INTERVAL_IN_SECONDS);
        keep_alive_exceeded_in_seconds_ = 
            Config::Inst().GetInteger("tracker.keep_alive_exceeded_in_seconds", KEEP_ALIVE_EXCEEDED_IN_SECONDS);

        //轮询的remove不能短于list的remove
        if( max_remove_dead_peer_time_elapsed_ < keep_alive_exceeded_in_seconds_)
        {
            max_remove_dead_peer_time_elapsed_ = keep_alive_exceeded_in_seconds_;
        }

        max_list_peer_count_ = 
            Config::Inst().GetInteger("tracker.max_list_peer_count", MAX_PEERS_COUNT);
        min_keep_alive_interval_in_milliseconds_ = 
            Config::Inst().GetInteger("tracker.min_keep_alive_interval_in_milliseconds", MIN_KLP_IN_MILLISECONDS);

        this->update_priority();

        is_running_ = true;

        // Statistic
        size_t res_hash_map_size = Config::Inst().GetInteger("tracker.res_hash_map_size",301057);
        if (res_hash_map_size > 1000*10000 && res_hash_map_size<1000)
        {
            res_hash_map_size = 301057;
        }
        tracker_statistic_ = TrackerStatistic::Inst();
        tracker_statistic_->Start(res_hash_map_size);

        LOG4CPLUS_INFO(g_logger, "Tracker Model is started successfully.");
    }

    void TrackerModel::Stop()
    {
        LOG4CPLUS_DEBUG(g_logger, "Stopping Tracker Model.");

        if (!is_running_)
        {
            LOG4CPLUS_WARN(g_logger,"TrackerModel::Stop Tracker Model is not running. Return.");
            return ;
        }

        Clear();

        is_running_ = false;

        tracker_statistic_->Stop();

        LOG4CPLUS_INFO(g_logger, "Tracker Model is stopped successfully.");
    }

    void TrackerModel::Clear()
    {
        LOG4CPLUS_DEBUG(g_logger, "Clearing TrackerModel");
        container_->Clear();
    }

    void TrackerModel::update_priority()
    {
        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
        boost::char_separator<char> sep("|");

        // 分析带宽配置
        {
            std::string bandwidth = Config::Inst().GetTString("priority.bandwidth", "");
            tokenizer tokens(bandwidth, sep);
            for (tokenizer::iterator tok_iter = tokens.begin();tok_iter != tokens.end(); ++tok_iter)
            {
                std::string s = *tok_iter;
                size_t pos = s.find( "," );
                if( pos != s.npos )
                {
                    // 设置位置
                    s[pos] = '\0';
                    size_t k = boost::lexical_cast<size_t> (s.c_str());
                    size_t v = boost::lexical_cast<size_t> (s.c_str() + pos + 1);

                    priority_bandwith[k] = v;
                }
            }
        }

        // 分析ISP配置
        {
            std::string isp = Config::Inst().GetTString("priority.isp", "");
            tokenizer tokens(isp, sep);
            int i = 0;
            for (tokenizer::iterator tok_iter = tokens.begin();tok_iter != tokens.end(); ++tok_iter)
            {
                priority_isp_[i++] = boost::lexical_cast<size_t> ( *tok_iter );
                if( i == 6 )
                    break;
            }
        }
    }

    void TrackerModel::ListPeers(RID resource_id, boost::uint16_t max_peers_count, std::vector<protocol::CandidatePeerInfo>& peerInfos, 
        const boost::asio::ip::udp::endpoint& end_point,boost::uint8_t requirer_nat_type,unsigned request_ip)	 
    {
        LOG4CPLUS_DEBUG(g_logger,"TrackerModel::ListPeers:  RID = " << resource_id << ", max_peer_count = " << max_peers_count);
        if (!is_running_)
        {
            LOG4CPLUS_WARN(g_logger,"TrackerModel::ListPeers: Tracker Model is not running. Return.");
            return ;
        }

        if (max_peers_count > GetMaxListPeerCount())
        {
            LOG4CPLUS_WARN(g_logger, "Peers Count should not be larger than " << GetMaxListPeerCount());
            max_peers_count = GetMaxListPeerCount();
        }

        peerInfos.resize(0);

        boost::asio::ip::udp::endpoint request_end_point = end_point;
        if (request_ip != 0)
        {
             request_end_point.address(boost::asio::ip::address_v4(request_ip));
        }

        container_->ListResourceAlivePeers(peerInfos,resource_id,max_peers_count,GetKeepAliveExceededInSeconds() * 1000,request_end_point,requirer_nat_type);

        LOG4CPLUS_DEBUG(g_logger, "Found " << peerInfos.size() << " Peers associated with resource " << resource_id);
    }

    void TrackerModel::ListTcpPeers(RID resource_id, boost::uint16_t max_peers_count,std::vector<protocol::CandidatePeerInfo>& peerInfos,
        const boost::asio::ip::udp::endpoint& end_point,boost::uint8_t requirer_nat_type,unsigned request_ip)
    {
        LOG4CPLUS_DEBUG(g_logger,"TrackerModel::ListTcpPeers:  RID = " << resource_id << ", max_peer_count = " << max_peers_count);

        if (!is_running_)
        {
            LOG4CPLUS_WARN(g_logger,"TrackerModel::ListTcpPeers: Tracker Model is not running. Return.");
            return ;
        }

        if (max_peers_count > GetMaxListPeerCount())
        {
            LOG4CPLUS_WARN(g_logger, "Peers Count should not be larger than " << GetMaxListPeerCount());
            max_peers_count = GetMaxListPeerCount();
        }

        peerInfos.resize(0);

        boost::asio::ip::udp::endpoint request_end_point = end_point;
        if (request_ip != 0)
        {
            request_end_point.address(boost::asio::ip::address_v4(request_ip));
        }

        container_->ListResourceAliveTcpPeers(peerInfos,resource_id,max_peers_count,GetKeepAliveExceededInSeconds() * 1000,request_end_point,requirer_nat_type);

        LOG4CPLUS_DEBUG(g_logger, "Found " << peerInfos.size() << " Peers associated with resource " << resource_id);
    }

    bool TrackerModel::FastUpdate(const Guid& peer_id, const RID& resource_id)
    {
        LOG4CPLUS_DEBUG(g_logger, "TrackerModel::FastUpdate: PID = " << peer_id << ", RID = " << resource_id);

        if (IsAlive(peer_id)) // 确保这个Peer是对当前Tracker进行Commit/KeepAlive的
        {
            LOG4CPLUS_DEBUG(g_logger, "Insert ID: PID = " << peer_id << "; RID = " << resource_id);

            if (container_->HasResource(peer_id, resource_id) == false)
            {
                container_->Insert(peer_id, resource_id);
            }

            return true;
        }
        else
        {
            LOG4CPLUS_DEBUG(g_logger,"Peer is not alive or Does not exist: " << peer_id);
            RemovePeer(peer_id);
            LOG4CPLUS_INFO(g_logger,"Resource Count: " << TotalResourcesCount() << " Peer Count: " << TotalPeersCount());
            return false; 
        }
    }

    bool TrackerModel::CommitPeerInfo(const Guid& peer_id, const vector<RID>& resource_ids, const PeerInfo& peer_info)
    {

        container_->UpdatePeerInfo(peer_id,peer_info);		
        container_->UpdatePeerResources(peer_id, resource_ids);

        LOG4CPLUS_DEBUG(g_logger, "Resource Count: " << TotalResourcesCount() << " Peer Count: " << TotalPeersCount());

        return false;
    }

    // return resource count
    boost::uint16_t TrackerModel::ReportPeerInfo(const Guid& peer_id, const vector<protocol::REPORT_RESOURCE_STRUCT>& reports, const PeerInfo& peer_info, boost::uint16_t client_resoure_count)
    {
        LOG4CPLUS_DEBUG(g_logger,"TrackerModel::ReportPeerInfo:  " << peer_id << ", IP: " << SOCKET_ADDR(peer_info.IP));

        tracker_statistic_->AddPeerversionTimes(peer_info.PeerVersion);
        tracker_statistic_->AddNatTypeTimes(peer_info.PeerNatType);
        
        // has peer		
        container_->UpdatePeerInfo(peer_id,peer_info);

        //if (container_->HasPeer(peer_id) == false || reports.size() > 0)
        if(reports.size() > 0)
        {
            // resources
            std::set<Guid> resources;
            container_->ListPeerResources(peer_id, std::inserter(resources, resources.begin()));

            //做一个上报资源数量的统计
            int report_add = reports.size();
            int report_del = 0;

            // merge
            STL_FOR_EACH_CONST(vector<protocol::REPORT_RESOURCE_STRUCT>, reports, iter)
            {
                if (iter->Type == 1) // add
                {
                    resources.insert(iter->ResourceID);
                }
                else if (iter->Type == 0) // del
                {
                    resources.erase(iter->ResourceID);
                    ++report_del;
                    --report_add;
                }
                else
                {
                    // ignore!
                    LOG4CPLUS_WARN(g_logger,"TrackerModel::ReportPeerInfo: Invalid Report Type: " << (boost::uint32_t)(iter->Type));
                }
            }

            tracker_statistic_->AddReportNumTimes( report_del*1000 + report_add );

            // update, 保证resources有序
            container_->UpdatePeerResources(peer_id, resources);
        }
        else
        {
            tracker_statistic_->AddReportNumTimes(0);
        }


        // resource count
        return container_->PeerResourcesCount(peer_id);

    }

    boost::uint16_t TrackerModel::UpdatePeerKeepAliveTime(const Guid& peer_id)
    {
        LOG4CPLUS_DEBUG(g_logger,"TrackerModel::UpdatePeerKeepAliveTime:  " << peer_id);
        container_->UpdatePeerKPLTime(peer_id);
        return container_->PeerResourcesCount(peer_id);
    }

    void TrackerModel::UpdatePeerInfo(const Guid& peer_id, const PeerInfo& peer_info)
    {
        container_->UpdatePeerInfo(peer_id, peer_info);
    }

    void TrackerModel::UpdatePeerStunInfo(const Guid& peer_id, const PeerInfo& peer_info)
    {
        container_->UpdatePeerInfo(peer_id, peer_info);
    }

    void TrackerModel::UpdataPeerDetectInfo(const Guid& peer_id, const PeerInfo& peer_info)
    {
        container_->UpdatePeerInfo(peer_id, peer_info);
    }

    bool TrackerModel::PeerExists(const Guid& peer_id)
    {
        return container_->HasPeer(peer_id);
    }

    bool TrackerModel::IsAlive(const Guid& peer_id)
    {
        LOG4CPLUS_DEBUG(g_logger, "TrackerModel::IsAlive: peer_id = " << peer_id);
        framework::timer::TickCounter::count_value_type curr = framework::timer::TickCounter::tick_count();
        if (container_->HasPeer(peer_id))
        {
            framework::timer::TickCounter::count_value_type last_kpl_time = container_->GetPeerKPLTime(peer_id);
            size_t interval = (curr - last_kpl_time);
            LOG4CPLUS_DEBUG(g_logger,"Current Tick: " << curr << ", Last Tick: " << last_kpl_time);
            LOG4CPLUS_DEBUG(g_logger,"NonActive Interval: " << interval << "(ms), Time Limit: " << GetKeepAliveExceededInSeconds() << "(s)");
            return  interval <= GetKeepAliveExceededInSeconds() * 1000;
        }
        LOG4CPLUS_WARN(g_logger,"No Such Peer, Return false.");
        return false;
    }

    void TrackerModel::RemovePeer(const Guid& peer_id)
    {
        LOG4CPLUS_DEBUG(g_logger,"TrackerModel::RemovePeer " << peer_id);
        container_->RemovePeer(peer_id);
    }

    void TrackerModel::RemoveUnAlivePeers()
    {
        LOG4CPLUS_DEBUG(g_logger,"TrackerModel::RemoveUnAlivePeers");
        container_->RemoveNonAlivePeers(max_remove_dead_peer_time_elapsed_*1000,max_remove_dead_peer_per_times_,max_travel_peer_per_times_);
    }

    bool TrackerModel::IsTooFrequent(const Guid& peer_id)
    {
        LOG4CPLUS_TRACE(g_logger, "TrackerModel::IsTooFrequent: peer_id = " << peer_id);
        framework::timer::TickCounter::count_value_type curr = framework::timer::TickCounter::tick_count();
        if (container_->HasPeer(peer_id))
        {
            framework::timer::TickCounter::count_value_type last = container_->GetPeerKPLTime(peer_id);
            size_t interval = (curr - last);
            LOG4CPLUS_TRACE(g_logger,"Current Tick: " << curr << ", Last Tick: " << last);
            LOG4CPLUS_TRACE(g_logger,"NonActive Interval: " << interval << "(ms), Min Time Limit: " << GetMinKeepAliveIntervalInMilliseconds() << "(ms)");
            return  interval <= GetMinKeepAliveIntervalInMilliseconds();
        }
        LOG4CPLUS_DEBUG(g_logger,"No Such Peer, Return false.may be first report");
        return false;
    }

    unsigned TrackerModel::TotalResourcesCount() const
    {
        return container_->ResourcesCount();
    }

    unsigned TrackerModel::TotalPeersCount() const
    {
        return container_->PeersCount();
    }

    unsigned TrackerModel::GetResourcePeerCount(const Guid& resource_id)
    {
        return container_->ResourcePeersCount(resource_id);
    }

    void TrackerModel::ListPeerResources(const Guid& peer_id,vector<RID>& resources)
    {
        container_->ListPeerResources(peer_id,std::back_inserter(resources));
    }

    std::string TrackerModel::GetTrackerStatistic()
    {
        std::stringstream ss;
        ss<<"resource count:"<< container_->ResourcesCount()<<endl;
        ss<<"peer count:"<<container_->PeersCount()<<endl;
        ss<<" cmd--times:"<<endl;

		ss << tracker_statistic_->GetTrackerStatisticForQuery();
       
        ss<<"change ip num:"<<container_->ClearChangeIpNum()<<endl;
        ss<<"change location num:"<<container_->ClearChangeLocationNum()<<endl;
        ss<<" Iplocation found ip:"<<IpLocation::Instance()->ClearIpFound()<<" Iplocation not found ip:"<<IpLocation::Instance()->ClearIpNotFound()<<endl;
        ss<<"list return num:"<<endl;
        map<int,int> list_ret_num = container_->ClearListRetNum();
        int tmptotal = 0;
        for (map<int,int>::iterator it = list_ret_num.begin();it != list_ret_num.end();++it)
        {
            ss<<it->first<<"-"<<it->second<<endl;
            tmptotal += it->second;
        }
        ss<<" total:"<<tmptotal<<endl;
        ss<<"priority num"<<endl;
        map<int,int> proitory_num = udptracker::ClearPriorityNum();
        tmptotal = 0;
        for (map<int,int>::iterator it = proitory_num.begin();it != proitory_num.end();++it)
        {
            ss<<it->first<<"-"<<it->second<<endl;
            tmptotal += it->second;
        }
        ss<<" total:"<<tmptotal<<endl;
        return ss.str();
    }

    void TrackerModel::ConsoleLogPeerGuid(Guid &peer_guid) 
    {
        if (container_->HasPeer(peer_guid))
        {
            protocol::CandidatePeerInfo peer_info;	
            framework::timer::TickCounter::count_value_type last = container_->GetPeerKPLTime(peer_guid);
            std::list<Guid> resources;
            container_->ListPeerResources(peer_guid, std::back_inserter(resources));

            CONSOLE_OUTPUT("IP: "<<boost::asio::ip::address_v4(peer_info.IP)<<"\r\nDetect IP: "<<boost::asio::ip::address_v4(peer_info.DetectIP)<<"\r\nDetect UdpPort: "<<peer_info.DetectUdpPort<<"\r\nUdpPort: "<<peer_info.UdpPort<<"\r\nStunIP: "<<boost::asio::ip::address_v4(peer_info.StunIP)<<"\r\nStunUdpPort: "<<peer_info.StunUdpPort);
            CONSOLE_OUTPUT("LastKPLTime: "<< last);
            CONSOLE_OUTPUT("RID Count: " << resources.size());
            STL_FOR_EACH_CONST(std::list<Guid>, resources, iter)
            {
                CONSOLE_OUTPUT("RID " << *iter << " " << container_->ResourcePeersCount(*iter));
            }
        }
        else
        {
            CONSOLE_OUTPUT("peer no found " << peer_guid);
        }
    }

    void TrackerModel::ConsoleLogRID(RID &rid)
    {
        //把这一个函数注释了，因为现在list操作的trace_ip日志里，已经能够查询到需求了，不要为一个不怎么使用的应用，添加函数
        CONSOLE_OUTPUT("not support this founction any more,try to see trace ip list in log ");
        return;

    }

    void TrackerModel::ConsoleLogListRID()
    {
        CONSOLE_OUTPUT("Not support this command any more,peer guid can be seen in trace_ip list log");
        return;
    }

    void TrackerModel::ConsoleLogListPeerGuid()
    {
        CONSOLE_OUTPUT("Not support this command any more,peer guid can be seen in trace_ip list log");
        return;
    }

    void TrackerModel::ConsoleLogShowEmptyPeer()
    {
        //size_t e_count = container_->EmptyPeersCount();
        //不统计这个东西了，真需要统计，就dump一份数据来统计。
        size_t e_count = 0;
        size_t t_count = container_->PeersCount();
        double f = (e_count == 0 ? 0 : (0.0 + e_count) / t_count);
        CONSOLE_OUTPUT("Total: " << t_count << ", Empty: " << e_count << ", Percent: " << size_t(f * 10000 + 0.5) / 100.0 << "%");
    }

    void TrackerModel::DumpResources(const string& log_name)
    {

    }

    void TrackerModel::DumpSampleResources(const string& log_name, const std::set<RID>& sample_resources)
    {

    }

    void TrackerModel::ClearSampleResourcesListCount(const std::set<RID>& sample_resources)
    {
    }

    void TrackerModel::DumpPeers(const string& log_name)
    {

    }

    void TrackerModel::CheckResources(const string& log_name, unsigned min_peer_count)
    {

    }

    void TrackerModel::ClearResourcesListCount()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    // Config

    unsigned TrackerModel::GetKeepAliveIntervalInSeconds()
    {
        return keep_alive_interval_in_seconds_;
    }

    unsigned TrackerModel::GetKeepAliveExceededInSeconds()
    {
        return keep_alive_exceeded_in_seconds_;
    }

    unsigned TrackerModel::GetMaxListPeerCount()
    {
        return max_list_peer_count_;
    }

    unsigned TrackerModel::GetMinKeepAliveIntervalInMilliseconds()
    {
        return min_keep_alive_interval_in_milliseconds_;
    }
}
