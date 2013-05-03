#pragma once

/**
* @file
* @brief TrackerManager ��İ����ļ�
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

    //UPNP�����ɹ�
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
    * @brief ���� RID �� ģֵ �� TrackerGroup��������
    *    [Key]   ģ������
    *    [Value] TrackerGroup ����ָ��
    */
    typedef std::map<int, TrackerGroup::p> ModIndexer;
    ModIndexer mod_indexer;

    /**
    * @brief ���� Tracker ��endpoint ��TrackerGroup ��������
    *    [Key]   Tracker��enpoint
    *    [Value] TrackerGroup ����ָ��
    */
    std::map<boost::asio::ip::udp::endpoint, TrackerGroup::p> endpoint_indexer;

    // nightsuns: �ĳ�һ�� group �´�һ�� tracker �ķ�ʽ,�������е� tracker ����ע��Ĳ���
    std::map<boost::asio::ip::udp::endpoint, TrackerGroup::p> cached_groups_;

    /**
    * @brief Group����Ҳ�� RID ģ�ĳ���
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
