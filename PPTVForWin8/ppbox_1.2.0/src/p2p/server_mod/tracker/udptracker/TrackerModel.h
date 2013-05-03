#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <vector>

#include "TrackerStucts.h"
#include "IResourcePeerCountHandler.h"
#include "LocationCache.h"

#include "struct/Base.h"
#include "struct/Structs.h"

#include <boost/format.hpp>

namespace udptracker
{
    class TrackerModel
        : public boost::noncopyable
        , public boost::enable_shared_from_this<TrackerModel>
    {
    public:

        typedef boost::shared_ptr<TrackerModel> p;

        static p Create(const IResourcePeerCountHandler& handler);

    public:

        void Start();

        void Stop();

        void Clear();

        bool IsRunning() const { return is_running_; }

    public:

        void ListPeers(RID resource_id, boost::uint16_t max_peers_count, std::vector<protocol::CandidatePeerInfo>& peerInfos , 
            const boost::asio::ip::udp::endpoint& end_point,boost::uint8_t requirer_nat_type,unsigned reqest_ip=0);
        void ListTcpPeers(RID resource_id, boost::uint16_t max_peers_count, std::vector<protocol::CandidatePeerInfo>& peerInfos , 
            const boost::asio::ip::udp::endpoint& end_point,boost::uint8_t requirer_nat_type,unsigned reqest_ip=0);

        std::vector<protocol::CandidatePeerInfo> ListPeers(RID resource_id, boost::uint16_t max_peers_count);

        bool CommitPeerInfo(const Guid& peer_id, const vector<RID>& resource_ids, const PeerInfo& peer_info);

        // 返回ResourceCount
        boost::uint16_t ReportPeerInfo(const Guid& peer_id, const vector<protocol::REPORT_RESOURCE_STRUCT>& reports, const PeerInfo& peer_info, boost::uint16_t client_resource_count);

        // 当Peer进行List的时候,将这个Peer加入
        bool FastUpdate(const Guid& peer_id, const RID& resource_id);

        // 返回ResourceCount
        boost::uint16_t UpdatePeerKeepAliveTime(const Guid& peer_id);

        void UpdatePeerInfo(const Guid& peer_id, const PeerInfo& peer_info);

        void UpdatePeerStunInfo(const Guid& peer_id, const PeerInfo& peer_info);

        void UpdataPeerDetectInfo(const Guid& peer_id, const PeerInfo& peer_info);

        bool PeerExists(const Guid& peer_id);

        bool IsAlive(const Guid& peer_id);

        void RemovePeer(const Guid& peer_id);

        void RemoveUnAlivePeers();

        bool IsTooFrequent(const Guid& peer_id);

        unsigned GetResourcePeerCount(const Guid& resource_id);

        void ListPeerResources(const Guid& peer_id,vector<RID>& resources);

        std::string GetTrackerStatistic();

        unsigned GetResourcesCount() const { return container_->ResourcesCount(); }

        void DumpResources(const string& log_name);

        void DumpPeers(const string& log_name);

        // load resources from config
        void DumpSampleResources(const std::string& log_name, const std::set<RID>& sample_resources);
        void ClearSampleResourcesListCount(const std::set<RID>& sample_resources);

        void ClearResourcesListCount();

        void CheckResources(const string& log_name, unsigned min_peer_count);

    public:

        unsigned TotalResourcesCount() const;

        unsigned TotalPeersCount() const;

        // ConsoleLog:
        void ConsoleLogListRID();

        void ConsoleLogListPeerGuid();

        void ConsoleLogPeerGuid(Guid &peer_guid);

        void ConsoleLogRID(RID &rid);

        void ConsoleLogShowEmptyPeer();

    public:

        //////////////////////////////////////////////////////////////////////////
        // Config

        static unsigned GetKeepAliveIntervalInSeconds();

        static unsigned GetKeepAliveExceededInSeconds();

        static unsigned GetMaxListPeerCount();

        static unsigned GetMinKeepAliveIntervalInMilliseconds();

    private:

    private:

        TrackerModel(const IResourcePeerCountHandler& handler);

    private:

        bool is_running_;

    public:
        //SparseMatrix::p container_;
        LocationCache::p container_;

    private:
        //定时删除过期节点时，每次最多删除的peer数
        size_t max_remove_dead_peer_per_times_;
        //定时删除过期节点时，每次最多check的peer数
        size_t max_travel_peer_per_times_;
        size_t max_remove_dead_peer_time_elapsed_;

        const IResourcePeerCountHandler& handler;

        static size_t keep_alive_interval_in_seconds_;
        static size_t keep_alive_exceeded_in_seconds_;
        static size_t max_list_peer_count_;
        static size_t min_keep_alive_interval_in_milliseconds_;

    private:    
        //added by youngky 2012/02/22
        TrackerStatistic::p tracker_statistic_;

    public:

        static const boost::uint16_t MAX_PEERS_COUNT = 50;

        static const size_t KEEP_ALIVE_INTERVAL_IN_SECONDS = 60;

        static const size_t KEEP_ALIVE_EXCEEDED_IN_SECONDS = 3 * KEEP_ALIVE_INTERVAL_IN_SECONDS;

        static const size_t KEEP_ALIVE_EXCEEDED_IN_MILLISECONDS = KEEP_ALIVE_EXCEEDED_IN_SECONDS * 1000;

        static const size_t KEEP_ALIVE_INTERVAL_IN_MILLISECONDS = KEEP_ALIVE_INTERVAL_IN_SECONDS * 1000; // 120s

        static const size_t MIN_KLP_IN_MILLISECONDS = 500;

        static const size_t DEFAULT_MAX_REMOVE_DEAD_PEER_PER_TIMES = 200;

        static const size_t DEFAULT_MAX_TRAVEL_PEER_PER_TIMES = 2000;

        static const size_t DEFAULT_MAX_REMOVE_DEAD_PEER_TIME_ELAPSED = 200;


        static std::map<int , int> priority_bandwith;

        //根据peer上拥有的rid个数，计算优先级分值。算法为： min(rid_count_prority_limit/(ridcount), rid_count_prority_max)
        static int rid_count_prority_limit;
        static int rid_count_prority_max;

        //外网连内网的额外加分和内网连外网的额外加分
        static int public_2_inner_priority;
        static int inner_2_public_priority;
        // 
        // 0: 不同isp,同省,不同市
        // 1: 不同isp,同省,同市
        // 2: 不同isp,不同省
        // 3: 相同isp,同省,不同市
        // 4: 相同isp,同省,同市
        // 5: 相同isp,不同省
        // 
        static int priority_isp_[6];

        void update_priority();
    };
}
