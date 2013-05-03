#pragma once

/**
* @file
* @brief TrackerManager 类的包含文件
*/

#include "tracker/TrackerGroup.h"
#include "tracker/TrackerClient.h"

#include <framework/system/BytesOrder.h>
inline uint32_t GuidMod(const Guid& guid, uint32_t mod)
{
    boost::uint64_t buf[2];
    memcpy(&buf, &guid.data(), sizeof(guid.data()));
    buf[1] = framework::system::BytesOrder::little_endian_to_host_longlong(buf[1]);
    return static_cast<uint32_t> (buf[1] % mod);
}

class TrackerManager 
    : public boost::noncopyable
    , public boost::enable_shared_from_this<TrackerManager>
#ifdef DUMP_OBJECT
    , public count_object_allocate<TrackerManager>
#endif
{
public:
    typedef boost::shared_ptr<TrackerManager> p;

    void Start(const string& config_path, bool need_report = true);

    void Stop();

    void LoadTrackerList();

    void SaveTrackerList();

    void SetTrackerList(uint32_t group_count, std::vector<protocol::TRACKER_INFO> tracker_s);

    void DoList(RID rid);

    void DoQueriedPeerCount(const RID& rid);

    boost::uint32_t GetTrackersNum(const RID& rid);

    void OnUdpRecv(protocol::ServerPacket const &packet);

    void PPLeave();

    void OnDiskFileChange(const Guid& rid);

    //UPNP开启成功
    void OpenUPNPSucced(uint32_t ip, boost::uint16_t tcp_port, boost::uint16_t udp_port);

    void GetUPNPIPPort(uint32_t &ip, boost::uint16_t &tcp_port, boost::uint16_t &udp_port);

    bool NeedReport() const;

    void list_trackers();

    void UpdateTrackerRidList( const std::set<RID> & rids );

private:
    uint32_t upnp_ip_;
    boost::uint16_t upnp_tcp_port_;
    boost::uint16_t upnp_udp_port_;

protected:

    void OnListResponsePacket(protocol::ListPacket const & packet);

    void OnReportResponsePacket( protocol::ReportPacket const & packet);

    void OnQueryResponsePacket(protocol::QueryPeerCountPacket const & packet);
private:

    void StartAllGroups();

    void StopAllGroups();

    void ClearAllGroups();

private:
    struct TrackerInfoSorterByMod
    {
        bool operator() (const protocol::TRACKER_INFO& a, const protocol::TRACKER_INFO& b)
        {
            return a.ModNo < b.ModNo;
        }
    };
private:
    /**
    * @brief 根据 RID 的 模值 对 TrackerGroup进行索引
    *    [Key]   模的余数
    *    [Value] TrackerGroup 智能指针
    */
    typedef std::map<int, TrackerGroup::p> ModIndexer;
    ModIndexer mod_indexer;

    /**
    * @brief 根据 Tracker 的endpoint 对TrackerGroup 进行索引
    *    [Key]   Tracker的enpoint
    *    [Value] TrackerGroup 智能指针
    */
    std::map<boost::asio::ip::udp::endpoint, TrackerGroup::p> endpoint_indexer;

    // nightsuns: 改成一个 group 下带一个 tracker 的方式,来向所有的 tracker 进行注册的操作
    std::map<boost::asio::ip::udp::endpoint, TrackerGroup::p> cached_groups_;

    /**
    * @brief Group数，也是 RID 模的除数
    */
    boost::uint32_t group_count_;

    volatile bool is_running_;

    string tracker_list_save_path_;

    bool need_report_;

private:
    static TrackerManager::p inst_;
public:
    static framework::timer::AsioTimerManager * asio_timer_manager_;

    static TrackerManager::p Inst() { return inst_; }

private:
    TrackerManager()
        : upnp_ip_(0)
        , upnp_tcp_port_(0)
        , upnp_udp_port_(0)
        , group_count_(0)
        , is_running_(false)
        , need_report_(true)
    {
        TrackerClient::peer_guid_.generate();
    }
};
