/* ======================================================================
 *    Bootstrap.h
 *    Copyright (c) 2009 Synacast. All rights reserved.
 *
 *    Bootstrap网络模块，负责响应请求。单例
 * ======================================================================
 *      Time        Changer     ChangeLog
 *    ~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 *    2009-4-27     cnhbdu      创建
 */

#ifndef __BOOTSTRAP_H__
#define __BOOTSTRAP_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "IPLib.h"
#include "ServerPacker.h"
//#include "IOUtil.h"
#include "io/FileControl.h"
#include "BConfig.h"
#include "PacketStat.h"

#include "framework/timer/Timer.h"
#include "framework/timer/AsioTimerManager.h"

#include <deque>

#include "ServerInfo.h"
#include "protocol/UdpServer.h"
#include "Tracker.h"
#include "SuperNode.h"

namespace util
{
    namespace protocol
    {
        class HttpClient;
    }
}

class IpLocation;

BOOTSTRAP_NAMESPACE_BEGIN

namespace fmk = framework;
using std::map;

/**************************************************
*    配置项缺省值
**************************************************/

// UDP端口
const boost::uint16_t DEFAULT_UDP_PORT = 6400;
// UDP收包队列
const boost::uint32_t DEFAULT_UDP_RECV_LEN = 2000;
// 是否同步MIS文件
const bool DEFAULT_IF_SYN_MIS = true;
// 定时读取MIS文件的时间间隔
const boost::uint32_t DEFAULT_MIS_SYNTIME = 60;
// 与MIS同步文件路径
const std::string DEFAULT_MIS_SYNDIR = "./mis_dir";
// 本地拷贝文件路径
const std::string DEFAULT_CONF_DIR = "./config_dir";

class MisSync;

/**************************************************
*    Bootstrap
**************************************************/

class Bootstrap
    : public boost::noncopyable
    , public boost::enable_shared_from_this<Bootstrap>
    , public protocol::IUdpServerListener
{
public:
    typedef boost::shared_ptr<Bootstrap> p;
    static Bootstrap::p Inst() { return m_inst; }

    typedef protocol::QueryServerListPacket QListPakt;

    typedef protocol::QueryUploadPicProbabilityPacket QUpPicPakt;

    typedef protocol::QueryNeedReportPacket QNReportPakt;

    typedef boost::asio::ip::udp::endpoint IpUdpEndpoint_T;

    struct PaktCache
    {
        map<boost::uint16_t, boost::shared_ptr<QListPakt> > tracker_commit_pakt;
        map<boost::uint16_t, boost::shared_ptr<QListPakt> > tracker_list_pakt;
        map<boost::uint16_t, boost::shared_ptr<QListPakt> > cache_pakt;
        map<boost::uint16_t, boost::shared_ptr<QListPakt> > collection_pakt;
        map<boost::uint16_t, boost::shared_ptr<QListPakt> > live_tracker_commit_pakt;
        map<boost::uint16_t, boost::shared_ptr<QListPakt> > live_tracker_list_pakt;
    };

    struct OldPaktCache
    {
        boost::shared_ptr<protocol::QueryTrackerListPacket> tracker_list_packet;
        boost::shared_ptr<protocol::QueryDataCollectionServerPacket> collection_packet;
        boost::shared_ptr<protocol::QueryNotifyListPacket> notify_packet;
        boost::uint16_t live_tracker_group_count_;
        std::vector<boost::shared_ptr<protocol::TRACKER_INFO> > live_tracker_list_;
        std::vector<boost::shared_ptr<protocol::TRACKER_INFO> > live_tracker_for_udpserver_list_;  // 只有PEER_VERSION_V10版本的peer会用到
        boost::uint16_t tracker_for_listing_group_count_[protocol::QueryTrackerForListingPacket::MAX_TRACKER_FOR_LISTING_TYPE];
        std::vector<boost::shared_ptr<protocol::TRACKER_INFO> > tracker_for_listing_list_[protocol::QueryTrackerForListingPacket::MAX_TRACKER_FOR_LISTING_TYPE];
        std::vector<boost::shared_ptr<protocol::TRACKER_INFO> > live_tracker_for_listing_udpserver_;
    };

    ~Bootstrap();

    static boost::uint32_t ConfigDownload(void * param);

public:
    void Start(const string& config_file);

    void Stop();

    void InitConfig();

    void ShowStatus();

    void ShowPaktStat();

    virtual void OnTimerElapsed(framework::timer::Timer::pointer * pointer, u_int times);

    virtual void OnUdpRecv(protocol::Packet const & packet);

    template<typename type>
    void DoSendPacket(type const & packet, boost::uint16_t dest_protocol_version);

    void LoadConfig();

    void OnConfigDownload(const boost::system::error_code* ec);

    void LoadDataInfo();

    void OnMisSyncOver();

    void OnQSrvList(const QListPakt & qlist_recv_pakt);

    void OnQUpPic(const QUpPicPakt & recv_pakt);

    void OnQNeedReport(const QNReportPakt & recv_pakt);

    void GenPacketCache();

    void GenOldPacketCache();

    void SendIndexPakt(const QListPakt & qlist_recv_pakt, 
        protocol::REGION_INFO real_region, boost::uint32_t tran_id);

    void SendStunPakt(const QListPakt & qlist_recv_pakt, 
        protocol::REGION_INFO real_region, boost::uint32_t tran_id);

    void SendMapPaktCache(
        const map<boost::uint16_t, boost::shared_ptr<QListPakt> >& pakt_map,
        const IpUdpEndpoint_T& end_point,
        boost::uint16_t level, 
        protocol::REGION_INFO real_region,
        boost::uint32_t tran_id,
        protocol::SERVER_TYPE_INFO type_info,
        boost::uint16_t dest_protocol_version
        );

    IPLib& GetIPLib() { return m_ip_lib; }
    ServerPacker& GetSrvPacker() { return m_srv_pack; }

    // 旧协议的请求
    void OnQueryTrackerList(const protocol::QueryTrackerListPacket & packet);

    void OnQueryLiveTrackerList(const protocol::QueryLiveTrackerListPacket & packet);

    void OnQueryStunServerList(const protocol::QueryStunServerListPacket & packet);

    void OnQueryIndexServerList(const protocol::QueryIndexServerListPacket & packet);

    void OnQueryDataCollectionServer(const protocol::QueryDataCollectionServerPacket & packet);

    void OnQueryNotifyServer(const protocol::QueryNotifyListPacket & packet);

    void OnQuerySuperNodeList(const protocol::QuerySnListPacket & packet);

    void OnQueryVipSuperNodeList(const protocol::QueryVipSnListPacket & packet);

    void OnQueryTrackerForListingList(const protocol::QueryTrackerForListingPacket & packet);

    void RegisterAllPackets();

    static bool IsReportTracker(boost::uint8_t tracker_type);

private:
    void UpdateConfig();

    void OnLoadConfigWhenStart();

    protocol::REGION_INFO GenRealRegion(const IpUdpEndpoint_T& end_point, 
        const QListPakt & qlist_recv_pakt);

    std::vector<protocol::SERVER_LIST> ConToPrtclSL(
        const bootstrap::SrvList& srv_list
        );

    map<boost::uint16_t, boost::shared_ptr<QListPakt> > GenMapPaktCache(const ServerMap_T& sev_map);

    void SyncMisFolder();

    void LoadIPServer();

    // 随机选择不超过100个的stun server
    boost::shared_ptr<protocol::QueryStunServerListPacket> RandomSelectStunLists(u_int trans_id,
        vector<protocol::STUN_SERVER_INFO> stun_servers);

    // 随机选择不超过100个的tracker server
    boost::shared_ptr<protocol::QueryTrackerListPacket> RandomSelectTrackerLists(u_int trans_id,
        u_int group_count, vector<protocol::TRACKER_INFO> tracker_servers);

    // 随机选择不超过100个的live tracker server，这不超过100个tracker中包括所有的只有UdpServer向其Report的tracker
    void RandomSelectLiveTrackerLists(boost::uint32_t region_code, std::vector<protocol::TRACKER_INFO> & tracker_infos,
        bool return_udpserver_tracker);

    // 根据tracker_type随机选择不超过100个的tracker
    // 如果是直播tracker的请求，则这不超过100个的tracker中包含所有的只有UdpServer向其Report的tracker
    void RandomSelectTrackerForListingList(boost::uint32_t region_code, std::vector<protocol::TRACKER_INFO> & tracker_infos, boost::uint8_t tracker_type);

    void DownloadIPDistributionDat();
    void HandleDownloadIPDistributionDat(boost::shared_ptr<util::protocol::HttpClient> http_client, const boost::system::error_code& err);
    void GenerateSuperNodeList();

    void GenerateAllTrackerList();
    void GenerateStunList();
    void GenerateTrackerList(boost::uint8_t tracker_type);
    void AddTracker(const protocol::TRACKER_INFO & tracker_info, std::map<boost::uint8_t, std::vector<protocol::TRACKER_INFO> > & trackers);

    boost::uint32_t GetServerCountBasedOnIsp(boost::uint32_t total_server_count, boost::uint32_t percentage, boost::uint8_t mod)const;
    boost::uint32_t GetRandomServerCount(boost::uint32_t total_server_count, boost::uint32_t based_on_isp_percentage, boost::uint8_t mod) const;

    void OnLogTimerElapsed();

    void GetTrackerList(boost::uint8_t tracker_type, boost::uint32_t rigion_code, boost::uint32_t ip,
        boost::uint32_t servers_based_on_isp_count_in_one_mod,
        boost::uint32_t random_servers_count_in_one_mod,
        std::vector<protocol::TRACKER_INFO> & select_trackers);

    void GetTrackerListForPacket(boost::uint8_t tracker_type, boost::uint32_t ip, std::vector<protocol::TRACKER_INFO> & trackers,
        boost::uint32_t rigion_code);

public:

    // 程序中会依赖到这些项的顺序，在增删改的时候需要注意
    enum TrackerType
    {
        VodReportTracker,
        VodListTracker,
        LiveReportTracker,
        LiveListTracker,
        LiveUdpServerReportTracker,
        LiveListUdpServerTracker,
        TrackerMaxType,
    };

private:
    /* ---------- 配置信息 ---------- */
    string m_config;                    // 配置文件名

    boost::uint16_t m_UDP_PORT;            // UDP端口
    boost::uint32_t m_UDP_RECV_LEN;        // UDP收包队列
    bool m_IF_SYN_MIS;                    // 是否同步MIS文件
    boost::uint32_t m_MIS_SYNTIME;        // 定时读取MIS文件的时间间隔
    std::string m_MIS_SYNDIR;            // 与MIS同步文件路径
    std::string m_CONF_DIR;                // 本地拷贝文件路径

    std::string m_BS_CONF_URL;

    boost::uint16_t m_MIN_PROTOCOL_VERSION;  // 最低支持版本号

    /* ---------- 报文统计 ---------- */
    PacketStat m_recv_stat;
    PacketStat m_send_stat;
    std::map<boost::uint32_t, PacketStat> m_ac_stat;

private:
    Bootstrap();

    boost::shared_ptr<protocol::UdpServer> m_udp_server;
    fmk::timer::PeriodicTimer::pointer m_mis_timer;

    IPLib m_ip_lib;
    ServerPacker m_srv_pack;
    MisSync *m_mis_sync;

    map<boost::uint32_t, PaktCache> m_pakt_cache;
    boost::shared_ptr<QListPakt> m_index_pakt_cache;
    boost::shared_ptr<QListPakt> m_stun_pakt_cache;

    map<boost::uint32_t, OldPaktCache> m_old_pakt_cache;
    boost::shared_ptr<protocol::QueryStunServerListPacket> m_old_stun_list_packet;
    boost::shared_ptr<protocol::QueryIndexServerListPacket> m_old_index_list_packet;

    static Bootstrap::p m_inst;
    bool m_is_running;
    bool m_is_staring;
    bool m_is_downloading_config;

    //只是为了生成一个end point,构造cache包的时候用，在发包之前会赋值为正确的end point
    IpUdpEndpoint_T my_end_point;// = IpUdpEndpoint_T(boost::asio::ip::udp::v4(), 8080);

    framework::timer::PeriodicTimer::pointer ip_distribution_timer_;
    boost::uint32_t ip_distribution_interval_;
    std::string ip_distribution_url_;

    SuperNodes common_super_node_;
    SuperNodes vip_super_node;

    ServerInfo<protocol::STUN_SERVER_INFO> stun_servers_;

    std::map<boost::uint8_t, Tracker> tracker_servers_;

    boost::uint32_t stun_based_on_isp_percentage_;
    boost::uint32_t stun_total_count_;

    boost::uint32_t stun_request_count_in_period_;

    enum PacketType
    {
        Notify,
        Stun,
        Index,
        ConfigString,
        SuperNode,
        VipSuperNode,
        PacketMaxType,
    };

    boost::uint32_t statistic[PacketMaxType];

    boost::shared_ptr<boost::asio::deadline_timer> log_timer_;

    static const std::string IpDistributionFilename_;

public:
    framework::timer::AsioTimerManager * asio_timer_manager_;
};

BOOTSTRAP_NAMESPACE_END

#endif // __BOOTSTRAP_H__
