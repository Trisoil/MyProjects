/* ======================================================================
 *    Bootstrap.cpp
 *    Copyright (c) 2009 Synacast. All rights reserved.
 *
 *    Bootstrap网络模块，负责响应请求
 * ======================================================================
 *      Time        Changer     ChangeLog
 *     ~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 *    2009-4-27     cnhbdu      创建
 */

#include "Common.h"
#include "Bootstrap.h"
#include "MainThread.h"
#include "MisSync.h"
#include "IpLocation.h"


#ifdef WIN32
#include <WinInet.h>
#else
#endif

#include <boost/filesystem.hpp>

#include "Tracker.h"


BOOTSTRAP_NAMESPACE_BEGIN
FRAMEWORK_LOGGER_DECLARE_MODULE("Bootstrap");
Bootstrap::p Bootstrap::m_inst(new Bootstrap());

const std::string Bootstrap::IpDistributionFilename_ = "ipdistribution.dat";

Bootstrap::Bootstrap() : stun_request_count_in_period_(0)
{
    InitConfig();

    m_is_running = false;
    m_is_staring = false;
    m_is_downloading_config = false;

    my_end_point = IpUdpEndpoint_T(boost::asio::ip::udp::v4(), 8080);

    memset(statistic, 0, sizeof(statistic));

    tracker_servers_.insert(std::make_pair(VodReportTracker, Tracker()));
    tracker_servers_.insert(std::make_pair(VodListTracker, Tracker()));
    tracker_servers_.insert(std::make_pair(LiveReportTracker, Tracker()));
    tracker_servers_.insert(std::make_pair(LiveListTracker, Tracker()));
    tracker_servers_.insert(std::make_pair(LiveUdpServerReportTracker, Tracker()));
    tracker_servers_.insert(std::make_pair(LiveListUdpServerTracker, Tracker()));
}

Bootstrap::~Bootstrap()
{
    if (m_mis_sync)
    {
        delete m_mis_sync;
        m_mis_sync = 0;
    }
}

void Bootstrap::InitConfig()
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::BootStrap(), "Bootstrap::InitConfig()");
    // 初始化配置项
    m_UDP_PORT = DEFAULT_UDP_PORT;
    m_UDP_RECV_LEN = DEFAULT_UDP_RECV_LEN;
    m_IF_SYN_MIS = DEFAULT_IF_SYN_MIS;
    m_MIS_SYNTIME = DEFAULT_MIS_SYNTIME;
    m_MIS_SYNDIR = DEFAULT_MIS_SYNDIR;
    m_CONF_DIR = DEFAULT_CONF_DIR;
    m_BS_CONF_URL = "";
    m_MIN_PROTOCOL_VERSION = 0;
    common_super_node_.SetReturnCountBasedOnIsp(5);
    common_super_node_.SetReturnCountInTotal(10);
    common_super_node_.SetReturnProbability(0);

    vip_super_node.SetReturnCountBasedOnIsp(4);
    vip_super_node.SetReturnCountInTotal(8);
    vip_super_node.SetReturnProbability(0);

    tracker_servers_[VodReportTracker].SetBasedOnIspPercentage(0);
    tracker_servers_[VodReportTracker].SetServerCountInOneResponse(100);
    tracker_servers_[VodListTracker].SetBasedOnIspPercentage(0);
    tracker_servers_[VodListTracker].SetServerCountInOneResponse(100);
    tracker_servers_[LiveReportTracker].SetBasedOnIspPercentage(50);
    tracker_servers_[LiveReportTracker].SetServerCountInOneResponse(100);
    tracker_servers_[LiveListTracker].SetBasedOnIspPercentage(50);
    tracker_servers_[LiveListTracker].SetServerCountInOneResponse(100);

    stun_based_on_isp_percentage_ = 50;
    stun_total_count_ = 100;
}

void Bootstrap::Start(const string& config_file)
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::BootStrap(), "Bootstrap::Start()");
    if (m_is_running || m_is_staring) return;

    asio_timer_manager_ = new framework::timer::AsioTimerManager( ::MainThread::IOS() , boost::posix_time::seconds(1) );
    asio_timer_manager_->start();

    m_is_staring = true;

    m_mis_sync = new MisSync(MainThread::IOS());
    if (m_mis_sync) m_mis_sync->Start();

    std::cout << "初始化定时器..." << std::endl;
    LOG4CPLUS_INFO(Loggers::BootStrap(), "初始化定时器...");

    m_mis_timer = new fmk::timer::PeriodicTimer( *this->asio_timer_manager_, 1000, 
        boost::bind(&Bootstrap::OnTimerElapsed, this , &this->m_mis_timer, 0 ) );

    ip_distribution_timer_ = new framework::timer::PeriodicTimer(*this->asio_timer_manager_, 24 * 60 * 60 * 1000,
        boost::bind(&Bootstrap::OnTimerElapsed, this, &this->ip_distribution_timer_, 0));

    log_timer_.reset(new boost::asio::deadline_timer(::MainThread::IOS()));
    log_timer_->expires_from_now(boost::posix_time::seconds(60));
    log_timer_->async_wait(boost::bind(&Bootstrap::OnLogTimerElapsed, shared_from_this()));

    // 加载配置文件
    m_config = config_file;

    IpLocation::SetIpLocation(IpDistributionFilename_);

    LoadConfig();

    if (m_IF_SYN_MIS)
    {
        DownloadIPDistributionDat();
    }
}

void Bootstrap::OnLoadConfigWhenStart()
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::BootStrap(), "Bootstrap::OnLoadConfigWhenStart()");
    // 设定UDP端口，开始收包
    m_udp_server.reset(new protocol::UdpServer(::MainThread::IOS() , shared_from_this()));
    while( false == m_udp_server->Listen(m_UDP_PORT) )
    {
        ++m_UDP_PORT;
        if( m_UDP_PORT >= 65534 )
        {
            std::cout << "启动失败![原因]:监听端口失败" << std::endl;
            LOG4CPLUS_INFO(Loggers::BootStrap(), "启动失败![原因]:监听端口失败");
            return;
        }
    }
    RegisterAllPackets();
    m_udp_server->Recv(m_UDP_RECV_LEN);
    std::cout << "启动完毕,开始收包!监听在" << m_UDP_PORT << "端口" << std::endl;
    LOG4CPLUS_INFO(Loggers::BootStrap(), "启动完毕,开始收包!监听在" << m_UDP_PORT << "端口");
    m_is_running = true;
}

void Bootstrap::Stop()
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::BootStrap(), "Bootstrap::Stop()");
    if (false == m_is_running) return;

    if (m_mis_sync) m_mis_sync->Stop();

    if (m_udp_server)
    {
        m_udp_server->Close();
        m_udp_server.reset();
    }
    if (m_mis_timer)
    {
        m_mis_timer->stop();
        //m_mis_timer.reset();
    }
    if (ip_distribution_timer_)
    {
        ip_distribution_timer_->stop();
    }

    m_is_running = false;
}

void Bootstrap::OnTimerElapsed( framework::timer::Timer::pointer * pointer, u_int times)
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::BootStrap(), "Bootstrap::OnTimerElapsed()");
    if (false == m_is_running) return;

    if (*pointer == m_mis_timer)
    {
        LoadConfig();
    }

    if (*pointer == ip_distribution_timer_ && m_IF_SYN_MIS)
    {
        DownloadIPDistributionDat();
    }
}

void Bootstrap::OnUdpRecv(protocol::Packet const & packet)
{
    if (false == m_is_running) return;

    m_recv_stat.Add();

    switch (packet.PacketAction)
    {
    case QListPakt::Action:
        {
            QListPakt const & list_recv_pakt = (QListPakt const &)packet;
            if(list_recv_pakt.IsRequest)
            {
                OnQSrvList(list_recv_pakt);
            }
            
        }
        break;
    case QUpPicPakt::Action:
        {
            QUpPicPakt const & pb_recv_pakt = (QUpPicPakt const &)packet;
            if(pb_recv_pakt.IsRequest)
            {
                OnQUpPic(pb_recv_pakt);
            }
            
        }
        break;
    case QNReportPakt::Action:
        {
            QNReportPakt const & report_recv_pakt = (QNReportPakt const &)packet;
            if(report_recv_pakt.IsRequest)
            {
                OnQNeedReport(report_recv_pakt);
            }
            
        }
        break;
    // 兼容以前协议
    case protocol::QueryTrackerListPacket::Action:
        {
            protocol::QueryTrackerListPacket const & pakt = (protocol::QueryTrackerListPacket const &)packet;
            if(pakt.IsRequest)
                OnQueryTrackerList(pakt);
        }
        break;
    case protocol::QueryStunServerListPacket::Action:
        {
            protocol::QueryStunServerListPacket const & pakt = (protocol::QueryStunServerListPacket const &)packet;
            if(pakt.IsRequest)
                OnQueryStunServerList(pakt);
        }
        break;
    case protocol::QueryIndexServerListPacket::Action:
        {
            protocol::QueryIndexServerListPacket const & pakt = (protocol::QueryIndexServerListPacket const &)packet;
            if(pakt.IsRequest)
                OnQueryIndexServerList(pakt);
        }
        break;
    case protocol::QueryDataCollectionServerPacket::Action:
        {
            protocol::QueryDataCollectionServerPacket const & pakt = (protocol::QueryDataCollectionServerPacket const &)packet;
            if(pakt.IsRequest)
                OnQueryDataCollectionServer(pakt);
        }
        break;
    case protocol::QueryNotifyListPacket::Action:
        {
            protocol::QueryNotifyListPacket const & pakt = (protocol::QueryNotifyListPacket const &)packet;
            if(pakt.IsRequest)
                OnQueryNotifyServer(pakt);
        }
        break;
    case protocol::QueryConfigStringPacket::Action:
        {
            if(((protocol::QueryConfigStringPacket const &)packet).IsRequest)
            {
                ++statistic[ConfigString];
                protocol::QueryConfigStringPacket config_string_packet =
                    protocol::QueryConfigStringPacket(packet.transaction_id_, 0,
                    m_srv_pack.GetConfigString(), packet.end_point);
                DoSendPacket(config_string_packet, ((protocol::QueryConfigStringPacket const &)packet).peer_version_);
            }
        }
        break;
    case protocol::QueryLiveTrackerListPacket::Action:
        {
            if (((protocol::QueryConfigStringPacket const &)packet).IsRequest)
            {
                OnQueryLiveTrackerList((protocol::QueryLiveTrackerListPacket const &)packet);
            }
        }
        break;
    case protocol::QuerySnListPacket::Action:
        {
            if (((protocol::QuerySnListPacket const &)packet).IsRequest)
            {
                OnQuerySuperNodeList((protocol::QuerySnListPacket const &)packet);
            }
        }
        break;
    case protocol::QueryVipSnListPacket::Action:
        {
            if (((protocol::QueryVipSnListPacket const &)packet).IsRequest)
            {
                OnQueryVipSuperNodeList((protocol::QueryVipSnListPacket const &)packet);
            }
        }
        break;
    case protocol::QueryTrackerForListingPacket::Action:
        {
            if (((protocol::QueryTrackerForListingPacket const &)packet).IsRequest)
            {
                OnQueryTrackerForListingList((protocol::QueryTrackerForListingPacket const &)packet);
            }
        }
    default:
        {
            int unknown_action;
            unknown_action = packet.PacketAction;
        }
        return;
    }
}

template<typename type>
void Bootstrap::DoSendPacket(type const & packet, boost::uint16_t dest_protocol_version)
{
    if(false == m_is_running) return;

    m_send_stat.Add();

    m_udp_server->send_packet(packet, dest_protocol_version);
}

void Bootstrap::LoadConfig()
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::BootStrap(), "Bootstrap::LoadConfig()");
    if (false == m_BS_CONF_URL.empty() && !m_is_downloading_config)
    {
        m_is_downloading_config = true;
        boost::thread(boost::bind(&Bootstrap::ConfigDownload, this));
    }
    else
    {
        m_is_downloading_config = false;
        boost::system::error_code* our_ec = new boost::system::error_code();
        OnConfigDownload(our_ec);
    }
}

void Bootstrap::UpdateConfig()
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::BootStrap(), "Bootstrap::UpdateConfig()");

    BPairConfig config;
    config.OpenConf(m_config);

    if (false == config.IsConfOpen())
    {
        return;
    }

    InitConfig();

    string iplib_url, index_url, stun_url, isp_url, config_url, sn_url, vip_sn_url;
    boost::uint32_t bak_retain_time;

    std::string key_s, val_s;
    while (config.ReadNextPair(key_s, val_s))
    {
        LOG4CPLUS_INFO(Loggers::BootStrap(), "ReadPair key: " << key_s << ", val: " << val_s);
        boost::algorithm::to_lower(key_s);
        boost::algorithm::to_lower(val_s);

        std::string::size_type b_pos = val_s.find('\"');
        std::string::size_type e_pos = val_s.rfind('\"');

        double val = 0.0;
        if ( 0 == b_pos && (val_s.size() -1 == e_pos) )
        {
            val_s = val_s.substr(b_pos + 1, e_pos - b_pos -1);
        }
        else
        {
            boost::uint8_t err;
            val = bootstrap_lexical_cast<double>(val_s, err);
            if (err) continue;
        }

        if ("udp_port" == key_s)            // UDP端口
        {
            m_UDP_PORT = static_cast<boost::uint16_t>(val);
        }
        else if ("udp_recv_len" == key_s)    // UDP收包队列
        {
            m_UDP_RECV_LEN = static_cast<boost::uint32_t>(val);
        }
        else if ("if_syn_mis" == key_s)        // 是否同步MIS文件
        {
            if ("yes" == val_s || "y" == val_s)
            {
                m_IF_SYN_MIS = true;
            }
            else if ("no" == val_s || "n" == val_s)
            {
                m_IF_SYN_MIS = false;
            }
            else
            {
                m_IF_SYN_MIS = DEFAULT_IF_SYN_MIS;
            }
        }
        else if ("mis_syntime" == key_s)    // 定时读取MIS文件的时间间隔
        {
            m_MIS_SYNTIME = static_cast<boost::uint32_t>(val);
        }
        else if ("mis_syndir" == key_s)        // 与MIS同步文件路径
        {
            m_MIS_SYNDIR = val_s;
        }
        else if ("conf_dir" == key_s)        // 本地拷贝文件路径
        {
            m_CONF_DIR = val_s;
        }
        else if ("ip_lib_url" == key_s)
        {
            iplib_url = val_s;
        }
        else if ("index_url" == key_s)
        {
            index_url = val_s;
        }
        else if ("stun_url" == key_s)
        {
            stun_url = val_s;
        }
        else if ("isp_server_url" == key_s)
        {
            isp_url = val_s;
        }
        else if ("bak_retain_time" == key_s)
        {
            bak_retain_time = static_cast<boost::uint32_t>(val);
        }
        else if ("bs_conf_url" == key_s)
        {
            m_BS_CONF_URL = val_s;
        }
        else if ("minimal_protocol_version" == key_s)
        {
            m_MIN_PROTOCOL_VERSION = static_cast<boost::uint16_t>(val);
        }
        else if ("config_string_url" == key_s)
        {
            config_url = val_s;
        }
        else if ("sn_server_url" == key_s)
        {
            sn_url = val_s;
        }
        else if ("vip_sn_server_url" == key_s)
        {
            vip_sn_url = val_s;
        }
        else if ("return_sn_probability" == key_s)
        {
            common_super_node_.SetReturnProbability(static_cast<boost::uint32_t>(val));
        }
        else if ("return_vip_sn_probability" == key_s)
        {
            vip_super_node.SetReturnProbability(static_cast<boost::uint32_t>(val));
        }
        else if ("return_sn_count_based_on_isp" == key_s)
        {
            common_super_node_.SetReturnCountBasedOnIsp(static_cast<boost::uint32_t>(val));
        }
        else if ("return_vip_sn_count_based_on_isp" == key_s)
        {
            vip_super_node.SetReturnCountBasedOnIsp(static_cast<boost::uint32_t>(val));
        }
        else if ("return_sn_count_in_total" == key_s)
        {
            common_super_node_.SetReturnCountInTotal(static_cast<boost::uint32_t>(val));
        }
        else if ("return_vip_sn_count_in_total" == key_s)
        {
            vip_super_node.SetReturnCountInTotal(static_cast<boost::uint32_t>(val));
        }
        else if ("ip_distribution_syntime" == key_s)
        {
            ip_distribution_interval_ = static_cast<boost::uint32_t>(val);
        }
        else if ("ip_distribution_url" == key_s)
        {
            ip_distribution_url_ = val_s;
        }
        else if ("vod_report_tracker_based_on_isp_percentage" == key_s)
        {
            tracker_servers_[VodReportTracker].SetBasedOnIspPercentage(static_cast<boost::uint32_t>(val));
        }
        else if ("vod_report_tracker_total_count" == key_s)
        {
            tracker_servers_[VodReportTracker].SetServerCountInOneResponse(static_cast<boost::uint32_t>(val));
        }
        else if ("vod_list_tracker_based_on_isp_percentage" == key_s)
        {
            tracker_servers_[VodListTracker].SetBasedOnIspPercentage(static_cast<boost::uint32_t>(val));
        }
        else if ("vod_list_tracker_total_count" == key_s)
        {
            tracker_servers_[VodListTracker].SetServerCountInOneResponse(static_cast<boost::uint32_t>(val));
        }
        else if ("live_report_tracker_based_on_isp_percentage" == key_s)
        {
            tracker_servers_[LiveReportTracker].SetBasedOnIspPercentage(static_cast<boost::uint32_t>(val));
        }
        else if ("live_report_tracker_total_count" == key_s)
        {
            tracker_servers_[LiveReportTracker].SetServerCountInOneResponse(static_cast<boost::uint32_t>(val));
        }
        else if ("live_list_tracker_based_on_isp_percentage" == key_s)
        {
            tracker_servers_[LiveListTracker].SetBasedOnIspPercentage(static_cast<boost::uint32_t>(val));
        }
        else if ("live_list_tracker_total_count" == key_s)
        {
            tracker_servers_[LiveListTracker].SetServerCountInOneResponse(static_cast<boost::uint32_t>(val));
        }
        else if ("stun_based_on_isp_percentage" == key_s)
        {
            stun_based_on_isp_percentage_ = static_cast<boost::uint32_t>(val);
        }
        else if ("stun_total_count" == key_s)
        {
            stun_total_count_ = static_cast<boost::uint32_t>(val);
        }
    }

    config.CloseConf();

    if (m_MIS_SYNTIME == 0)
    {
        m_MIS_SYNTIME = 60;
    }

    if (m_mis_sync)
    {
        m_mis_sync->UpdateConfig(iplib_url, index_url, stun_url,
            isp_url, config_url, m_MIS_SYNDIR, m_CONF_DIR, bak_retain_time, sn_url, vip_sn_url);
    }

    // m_udp_server->set_minimal_protocol_verion(m_MIN_PROTOCOL_VERSION);

}

void Bootstrap::LoadDataInfo()
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::BootStrap(), "Bootstrap::LoadDataInfo()");

    LOG4CPLUS_INFO(Loggers::BootStrap(), "开始导入数据信息...");

    if (m_IF_SYN_MIS && m_mis_sync)
    {
        m_mis_sync->Sync(boost::bind(&Bootstrap::OnMisSyncOver, shared_from_this()));
    }
    else
    {
        OnMisSyncOver();
    }

}

void Bootstrap::OnMisSyncOver()
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::BootStrap(), "Bootstrap::OnMisSyncOver()");

    LoadIPServer();

    GenPacketCache();
    GenOldPacketCache();

    LOG4CPLUS_INFO(Loggers::BootStrap(), "导入数据信息完毕");

    if (m_is_staring)
    {
        OnLoadConfigWhenStart();
        m_is_staring = false;
    }

    if (m_udp_server)
    {
        m_udp_server->set_minimal_protocol_verion(m_MIN_PROTOCOL_VERSION);
    }
}

void Bootstrap::SyncMisFolder()
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::BootStrap(), "Bootstrap::SyncMisFolder()");

    if (false == my_file_io::AccessDir(m_MIS_SYNDIR) 
        || false == m_IF_SYN_MIS)
    {
        return;
    }

    LOG4CPLUS_INFO(Loggers::BootStrap(), "同步MIS文件夹...");

    if (my_file_io::AccessDir(m_CONF_DIR))
    {
        my_file_io::DelDir(m_CONF_DIR);
    }
    my_file_io::MakeDir(m_CONF_DIR);
    my_file_io::CopyDir(m_MIS_SYNDIR, m_CONF_DIR, false);
}

void Bootstrap::LoadIPServer()
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::BootStrap(), "Bootstrap::LoadIPServer()");
    list<std::string> file_list, dir_list;
    //file_io::GetDirFileList(m_CONF_DIR, file_list, dir_list);
    my_file_io::GetDirFileList(m_CONF_DIR, file_list/*, dir_list*/);
    list<string> im_srv_list;
    
    for (list<std::string>::iterator it = file_list.begin();
        it != file_list.end(); ++it)
    {
        if (boost::algorithm::iends_with(*it, ".tmp"))
        {
            continue;
        }

        if (boost::algorithm::contains(*it, "bs_iplib"))
        {
            m_ip_lib.TryImportIpAddrs(*it);
        }
        else
        {
            im_srv_list.push_back(*it);
        }
    }

    LOG4CPLUS_INFO(Loggers::BootStrap(), "导入服务器分组信息...");

    m_srv_pack.SetPublicAc(m_ip_lib.PublicAc());
    m_srv_pack.ImportSevPack(im_srv_list);

    m_ac_stat.clear();
    std::set<boost::uint32_t> ac_list = m_ip_lib.GetAcList();
    for (std::set<boost::uint32_t>::iterator i = ac_list.begin();
        i != ac_list.end(); ++i)
    {
        boost::uint32_t ac = *i;

        m_ac_stat.insert(std::make_pair(ac, PacketStat()));
    }
    if (m_ac_stat.find(m_ip_lib.PublicAc()) == m_ac_stat.end())
    {
        m_ac_stat.insert(std::make_pair(m_ip_lib.PublicAc(), PacketStat()));
    }
}

void Bootstrap::OnQUpPic(const QUpPicPakt & recv_pakt)
{
    boost::system::error_code ec;
    std::string ip_s = recv_pakt.end_point.address().to_string(ec);
    if (!!ec) return;

    boost::uint32_t area_code = m_ip_lib.FindAreaCode(ip_s);
    boost::uint32_t tran_id = recv_pakt.transaction_id_;    //GetTransactionID();
    
    m_ac_stat[area_code].Add();

    const AcServer& ac_srv = m_srv_pack.GetAcServer(area_code);

//     QUpPicPakt::p send_pakt = QUpPicPakt::CreatePacket(
//         tran_id, static_cast<float>(ac_srv.config.up_pic_pb)
//         );
    QUpPicPakt send_pakt(tran_id, static_cast<float>(ac_srv.config.up_pic_pb), recv_pakt.end_point);
    DoSendPacket(send_pakt, recv_pakt.peer_version_);
}

void Bootstrap::OnQNeedReport(const QNReportPakt & recv_pakt)
{
    boost::system::error_code ec;
    std::string ip_s = recv_pakt.end_point.address().to_string(ec);
    if (!!ec) return;

    boost::uint32_t area_code = m_ip_lib.FindAreaCode(ip_s);
    boost::uint32_t tran_id = recv_pakt.transaction_id_;    //GetTransactionID();

    m_ac_stat[area_code].Add();

    const AcServer& ac_srv = m_srv_pack.GetAcServer(area_code);

    boost::uint8_t need_report = 0;
    if (boost::uint32_t((rand()<<16) + rand())%10000 
        < boost::uint32_t(ac_srv.config.collect_pb * 10000))
    {
        need_report = 1;
    }
//     QNReportPakt::p send_pakt = QNReportPakt::CreatePacket(
//         tran_id, need_report, ac_srv.config.dac_report_minute
//         );
    QNReportPakt send_pakt(tran_id, need_report, ac_srv.config.dac_report_minute, recv_pakt.end_point);
    DoSendPacket(send_pakt, recv_pakt.peer_version_);
}

void Bootstrap::GenPacketCache()
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::BootStrap(), "Bootstrap::GenPacketCache()");

    LOG4CPLUS_INFO(Loggers::BootStrap(), "生成Cache报文...");

    m_index_pakt_cache.reset();
    m_pakt_cache.clear();

    protocol::REGION_INFO region;

    LOG4CPLUS_INFO(Loggers::BootStrap(), "\t Index Server");
    const SrvList& index_srv = m_srv_pack.GetIndexSrv();
    boost::shared_ptr<QListPakt> index_send_pakt(new QListPakt(
        protocol::Packet::NewTransactionID(), region, ConToPrtclSL(index_srv), my_end_point));
    m_index_pakt_cache = index_send_pakt;

    const ServerPacker::AcSevMap_T& ac_sev_map = m_srv_pack.GetSevMap();

    for (ServerPacker::AcSevMap_T::const_iterator it = ac_sev_map.begin();
        it != ac_sev_map.end(); ++it)
    {
        LOG4CPLUS_INFO(Loggers::BootStrap(), "\tServerList, AreaCode: " << it->first);
        PaktCache pakt_cache;
        const AcServer& ac_srv = it->second;

        pakt_cache.tracker_commit_pakt = GenMapPaktCache(ac_srv.tracker_commit);
        pakt_cache.tracker_list_pakt = GenMapPaktCache(ac_srv.tracker_list);
        pakt_cache.cache_pakt = GenMapPaktCache(ac_srv.cache);
        pakt_cache.collection_pakt = GenMapPaktCache(ac_srv.collection);
        pakt_cache.live_tracker_commit_pakt = GenMapPaktCache(ac_srv.live_tracker_commit);
        pakt_cache.live_tracker_list_pakt = GenMapPaktCache(ac_srv.live_tracker_list);

        m_pakt_cache.insert(std::make_pair(it->first, pakt_cache));
    }
}

void Bootstrap::GenOldPacketCache()
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::BootStrap(), "Bootstrap::GenOldPacketCache()");

    LOG4CPLUS_INFO(Loggers::BootStrap(), "生成OldCache报文...");
    m_old_pakt_cache.clear();

    protocol::REGION_INFO region;

    LOG4CPLUS_INFO(Loggers::BootStrap(), "\t Index Server");
    const SrvList& index_srv = m_srv_pack.GetIndexSrv();
    vector<protocol::INDEX_SERVER_INFO> index_servers;
    for (vector<SrvInfo>::const_iterator it = index_srv.sevs.begin();
        it != index_srv.sevs.end(); ++it)
    {
        protocol::INDEX_SERVER_INFO isi(it->ip, it->port);
        isi.Type = it->prtcl;
        index_servers.push_back(isi);
    }

    boost::shared_ptr<protocol::QueryIndexServerListPacket> index_packet(
        new protocol::QueryIndexServerListPacket(
            protocol::Packet::NewTransactionID(),
            index_srv.appds, 
            index_servers,
            my_end_point
        ));

    m_old_index_list_packet = index_packet;

    if (!m_old_index_list_packet)
    {
        LOG4CPLUS_WARN(Loggers::BootStrap(), "\t Index Server生成错误");
    }

    const ServerPacker::AcSevMap_T& ac_sev_map = m_srv_pack.GetSevMap();

    for (ServerPacker::AcSevMap_T::const_iterator it = ac_sev_map.begin();
        it != ac_sev_map.end(); ++it)
    {
        LOG4CPLUS_INFO(Loggers::BootStrap(), "\tServerList, AreaCode: " << it->first);
        OldPaktCache old_packet_cache;
        const AcServer& ac_srv = it->second;

        // generate data collection packet cache buffer
        protocol::DATACOLLECTION_SERVER_INFO dsi;
        ServerMap_T::const_iterator it_collect = ac_srv.collection.find(0);
        if (it_collect != ac_srv.collection.end() && false == it_collect->second.sevs.empty())
        {
            const SrvList& collect_list = it_collect->second;
            dsi.Length = sizeof(protocol::DATACOLLECTION_SERVER_INFO);
            dsi.IP = collect_list.sevs[0].ip;
            dsi.Port = collect_list.sevs[0].port;
            dsi.Type = collect_list.sevs[0].prtcl;
        }
        old_packet_cache.collection_packet
            = boost::shared_ptr<protocol::QueryDataCollectionServerPacket>(new protocol::QueryDataCollectionServerPacket(
            protocol::Packet::NewTransactionID(), dsi, my_end_point
            ));

        if (!old_packet_cache.collection_packet)
        {
            LOG4CPLUS_WARN(Loggers::BootStrap(), "!!! Collection 报文生成错误!!!, 地域码: " << it->first);
        }

        // notify server packet cache buffer
        vector<protocol::NOTIFY_SERVER_INFO> notify_servers;
        ServerMap_T::const_iterator it_notify = ac_srv.notify.find(0);
        if (it_notify != ac_srv.notify.end() && false == it_notify->second.sevs.empty())
        {
            const SrvList& notify_list = it_notify->second;
            for (vector<SrvInfo>::const_iterator it = notify_list.sevs.begin();
                it != notify_list.sevs.end(); ++it)
            {
                protocol::NOTIFY_SERVER_INFO nsi;
                nsi.IP = it->ip;
                nsi.Port = it->port;
                notify_servers.push_back(nsi);
            }
        }
        old_packet_cache.notify_packet
            = boost::shared_ptr<protocol::QueryNotifyListPacket>(new protocol::QueryNotifyListPacket(
            0, 0, notify_servers, my_end_point));

        if (!old_packet_cache.notify_packet)
        {
            LOG4CPLUS_WARN(Loggers::BootStrap(), "!!! Notify Server 报文生成错误!!!, 地域码: " << it->first);
        }

        m_old_pakt_cache.insert(std::make_pair(it->first, old_packet_cache));

    }
    GenerateAllTrackerList();
    GenerateSuperNodeList();
    GenerateStunList();
}

protocol::REGION_INFO Bootstrap::GenRealRegion(const IpUdpEndpoint_T& end_point,
                                    const QListPakt & qlist_recv_pakt)
{
    protocol::REGION_INFO region;
    region.RigionCode = m_ip_lib.PublicAc();

    protocol::REGION_INFO peer_region;
    if (qlist_recv_pakt.transaction_id_ != 0) 
        peer_region = qlist_recv_pakt.request.region_info_;    //GetRegionInfo();

    boost::uint32_t peer_ip = static_cast<boost::uint32_t>(
        end_point.address().to_v4().to_ulong()
        );

    if (peer_region.Version == m_ip_lib.Version()
        && peer_region.PublicIP == peer_ip)
    {
        region.RigionCode = peer_region.RigionCode;
    }
    else    // 到IP地址库查询地域码
    {
        region.RigionCode = m_ip_lib.FindAreaCode(peer_ip);
    }
    region.PublicIP = peer_ip;
    region.Version = m_ip_lib.Version();

    return region;
}

std::vector<protocol::SERVER_LIST> Bootstrap::ConToPrtclSL(
    const SrvList& srv_list
    )
{
    std::vector<protocol::SERVER_LIST> ret;

    protocol::SERVER_LIST psl;
    psl.PaddingInfo = srv_list.appds;
    if (srv_list.sevs.empty())
    {
        psl.ServerTypeInfo.ConnectType = protocol::CONNECT_UDP;
    }
    else
    {
        psl.ServerTypeInfo.ConnectType = 
            static_cast<protocol::CONNECT_TYPE>(srv_list.sevs[0].prtcl);
    }
    psl.ServerTypeInfo.Level = srv_list.level;
    psl.ServerTypeInfo.ServerType = 
        static_cast<protocol::SERVER_TYPE>(srv_list.type);
    for (std::vector<SrvInfo>::const_iterator vit = srv_list.sevs.begin();
        vit != srv_list.sevs.end(); ++vit)
    {
        protocol::SERVER_DATA srv_data;
        srv_data.ConnectType = static_cast<protocol::CONNECT_TYPE>(vit->prtcl);
        srv_data.IP = vit->ip;
        srv_data.ModID = vit->mod;
        srv_data.Port = vit->port;
        psl.ServerArray.push_back(srv_data);
    }

    ret.push_back(psl);

    return ret;
}

map<boost::uint16_t, boost::shared_ptr<Bootstrap::QListPakt> > 
Bootstrap::GenMapPaktCache(const ServerMap_T& sev_map)
{
    map<boost::uint16_t, boost::shared_ptr<QListPakt> > pakt_cache_map;

    for (ServerMap_T::const_iterator it = sev_map.begin();
        it != sev_map.end(); ++it)
    {
        protocol::REGION_INFO region;
        boost::shared_ptr<QListPakt> send_pakt(new QListPakt(
            protocol::Packet::NewTransactionID(), region, ConToPrtclSL(it->second), my_end_point));
        
        pakt_cache_map.insert(make_pair(it->first, send_pakt));
    }

    return pakt_cache_map;
}

void Bootstrap::OnQSrvList(const QListPakt & qlist_recv_pakt)
{
    protocol::REGION_INFO real_region = 
        GenRealRegion(qlist_recv_pakt.end_point, qlist_recv_pakt);
    boost::uint32_t tran_id = qlist_recv_pakt.transaction_id_;//GetTransactionID();
    vector<protocol::SERVER_TYPE_INFO> qvec = qlist_recv_pakt.request.server_type_list_;// GetServerTypeList();

    m_ac_stat[real_region.RigionCode].Add();

    for (vector<protocol::SERVER_TYPE_INFO>::iterator it = qvec.begin();
        it != qvec.end(); ++it)
    {
        boost::uint16_t srv_level = it->Level;
        protocol::SERVER_TYPE_INFO type_info = *it;

        switch (it->ServerType)
        {
        case protocol::SERVER_INDEX:
            {
                SendIndexPakt(qlist_recv_pakt, real_region, tran_id);
            }
            break;
        case protocol::SERVER_STUN:
            {
                SendStunPakt(qlist_recv_pakt, real_region, tran_id);
            }
            break;
        case protocol::SERVER_TRACKER_COMMIT:
            {
                map<boost::uint32_t, PaktCache>::iterator it_cache
                    = m_pakt_cache.find(real_region.RigionCode);
                if (it_cache != m_pakt_cache.end())
                {
                    SendMapPaktCache(it_cache->second.tracker_commit_pakt,
                        qlist_recv_pakt.end_point, srv_level, real_region, tran_id, type_info, qlist_recv_pakt.peer_version_);
                }
            }
            break;
        case protocol::SERVER_TRACKER_LIST:
            {
                map<boost::uint32_t, PaktCache>::iterator it_cache
                    = m_pakt_cache.find(real_region.RigionCode);
                if (it_cache != m_pakt_cache.end())
                {
                    SendMapPaktCache(it_cache->second.tracker_list_pakt,
                        qlist_recv_pakt.end_point, srv_level, real_region, tran_id, type_info, qlist_recv_pakt.peer_version_);
                }
            }
            break;
        case protocol::SERVER_CACHE:
            {
                map<boost::uint32_t, PaktCache>::iterator it_cache
                    = m_pakt_cache.find(real_region.RigionCode);
                if (it_cache != m_pakt_cache.end())
                {
                    SendMapPaktCache(it_cache->second.cache_pakt,
                        qlist_recv_pakt.end_point, srv_level, real_region, tran_id, type_info, qlist_recv_pakt.peer_version_);
                }
            }
            break;
        case protocol::SERVER_DATA_COLLECTION:
            {
                map<boost::uint32_t, PaktCache>::iterator it_cache
                    = m_pakt_cache.find(real_region.RigionCode);
                if (it_cache != m_pakt_cache.end())
                {
                    SendMapPaktCache(it_cache->second.collection_pakt,
                        qlist_recv_pakt.end_point, srv_level, real_region, tran_id, type_info, qlist_recv_pakt.peer_version_);
                }
            }
            break;

        case protocol::SERVER_LIVE_TRACKER_COMMIT:
            {
                map<boost::uint32_t, PaktCache>::iterator it_cache
                    = m_pakt_cache.find(real_region.RigionCode);
                if (it_cache != m_pakt_cache.end())
                {
                    SendMapPaktCache(it_cache->second.live_tracker_commit_pakt,
                        qlist_recv_pakt.end_point, srv_level, real_region, tran_id, type_info, qlist_recv_pakt.peer_version_);
                }
            }
            break;

        case protocol::SERVER_LIVE_TRACKER_LIST:
            {
                map<boost::uint32_t, PaktCache>::iterator it_cache
                    = m_pakt_cache.find(real_region.RigionCode);
                if (it_cache != m_pakt_cache.end())
                {
                    SendMapPaktCache(it_cache->second.live_tracker_list_pakt,
                        qlist_recv_pakt.end_point, srv_level, real_region, tran_id, type_info, qlist_recv_pakt.peer_version_);
                }
            }
            break;

        default:
            {

            }
            break;
        }
    }
}

void Bootstrap::SendIndexPakt(const QListPakt & qlist_recv_pakt, 
                   protocol::REGION_INFO real_region, boost::uint32_t tran_id)
{
    if (m_index_pakt_cache)
    {
//         Buffer tmp_buf = m_index_pakt_cache->GetBuffer().Clone();
//         QListPakt::p send_pakt = QListPakt::ParseFromBinary(tmp_buf);
        QListPakt send_pakt(tran_id, real_region, m_index_pakt_cache->response.server_list_, qlist_recv_pakt.end_point);

        DoSendPacket(send_pakt, qlist_recv_pakt.peer_version_);

    }
}

void Bootstrap::SendStunPakt(const QListPakt & qlist_recv_pakt, 
                  protocol::REGION_INFO real_region, boost::uint32_t tran_id)
{
    if (m_stun_pakt_cache)
    {
//         Buffer tmp_buf = m_stun_pakt_cache->GetBuffer().Clone();
//         QListPakt::p send_pakt = QListPakt::ParseFromBinary(tmp_buf);
//         QListPakt send_pakt = *m_stun_pakt_cache;
//         send_pakt.transaction_id_ = tran_id;  //SetTransactionID(tran_id);
//         send_pakt.response.region_info_ = real_region;   //SetRegionInfo(real_region);
        m_stun_pakt_cache->transaction_id_ = tran_id;
        m_stun_pakt_cache->response.region_info_ = real_region; 
        m_stun_pakt_cache->end_point = qlist_recv_pakt.end_point;
        DoSendPacket(*m_stun_pakt_cache, qlist_recv_pakt.peer_version_);
    }
}

void Bootstrap::SendMapPaktCache(
            const map<boost::uint16_t, boost::shared_ptr<QListPakt> >& pakt_map,
            const IpUdpEndpoint_T& end_point,
            boost::uint16_t level, 
            protocol::REGION_INFO real_region,
            boost::uint32_t tran_id,
            protocol::SERVER_TYPE_INFO type_info,
            boost::uint16_t dest_protocol_version
            )
{
    map<boost::uint16_t, boost::shared_ptr<QListPakt> >::const_iterator it = pakt_map.find(level);
    if (it != pakt_map.end())
    {
//         Buffer tmp_buf = (it->second)->GetBuffer().Clone();
//         QListPakt::p send_pakt = QListPakt::ParseFromBinary(tmp_buf);
//         QListPakt send_pakt = *(it->second);
//         send_pakt.transaction_id_ = tran_id;   //SetTransactionID(tran_id);
//         send_pakt.response.region_info_ = real_region;   //SetRegionInfo(real_region);
        (it->second)->transaction_id_ = tran_id;
        (it->second)->response.region_info_ = real_region;
        (it->second)->end_point = end_point;
        DoSendPacket(*(it->second), dest_protocol_version);
    }
    else    // 没有找到对应的Level则返回空包
    {
        std::vector<protocol::SERVER_LIST> empty_list;
        protocol::SERVER_LIST tmp_sl;
        tmp_sl.ServerTypeInfo = type_info;
        empty_list.push_back(tmp_sl);
        
//         QListPakt::p send_pakt = QListPakt::CreatePacket(
//             tran_id, real_region, empty_list
//             );
        QListPakt send_pakt(tran_id, real_region, empty_list, end_point);
        DoSendPacket(send_pakt, dest_protocol_version);
    }
}

void Bootstrap::OnQueryTrackerList(const protocol::QueryTrackerListPacket & packet)
{
    if(false == m_is_running)
    {
        return;
    }

    QListPakt pakt;
    pakt.transaction_id_ = 0;
    protocol::REGION_INFO real_region = GenRealRegion(packet.end_point, pakt);
    m_ac_stat[real_region.RigionCode].Add();

    LOG4CPLUS_INFO(Loggers::BootStrap(), "reveive vod report server list request, ip = " << packet.end_point.address().to_v4().to_string() <<
        ", area code = " << real_region.RigionCode);

    Tracker & trackers = tracker_servers_[VodReportTracker];
    trackers.Visit(real_region.RigionCode);

    protocol::QueryTrackerListPacket response_packet;
    response_packet.transaction_id_ = packet.transaction_id_;
    response_packet.error_code_ = 0;
    response_packet.IsRequest = 0;
    response_packet.response.tracker_group_count_ = trackers.GetModNumber(real_region.RigionCode);
    response_packet.end_point = packet.end_point;

    GetTrackerListForPacket(VodReportTracker, packet.end_point.address().to_v4().to_ulong(), response_packet.response.tracker_info_, real_region.RigionCode);

    DoSendPacket(response_packet, packet.peer_version_);

    LOG4CPLUS_INFO(Loggers::BootStrap(), "send " << response_packet.response.tracker_info_.size() << " vod report tracker servers");

    if (response_packet.response.tracker_info_.empty())
    {
        LOG4CPLUS_WARN(Loggers::BootStrap(), "Response 0 vod report tracker server to " << packet.end_point.address().to_v4().to_string()
            << ", from area code " << real_region.RigionCode);
    }
}

void Bootstrap::OnQueryLiveTrackerList(const protocol::QueryLiveTrackerListPacket & packet)
{
    if(false == m_is_running)
    {
        return;
    }

    QListPakt pakt;
    pakt.transaction_id_ = 0;
    protocol::REGION_INFO real_region = GenRealRegion(packet.end_point, pakt);
    m_ac_stat[real_region.RigionCode].Add();

    LOG4CPLUS_INFO(Loggers::BootStrap(), "reveive live report server list request, ip = " << packet.end_point.address().to_v4().to_string() << 
        ", area code = " << real_region.RigionCode);

    Tracker & trackers = tracker_servers_[LiveReportTracker];
    trackers.Visit(real_region.RigionCode);

    tracker_servers_[LiveUdpServerReportTracker].Visit(real_region.RigionCode);

    protocol::QueryLiveTrackerListPacket response_packet;
    response_packet.transaction_id_ = packet.transaction_id_;
    response_packet.error_code_ = 0;
    response_packet.IsRequest = 0;
    response_packet.response.tracker_group_count_ = trackers.GetModNumber(real_region.RigionCode);
    response_packet.end_point = packet.end_point;

    GetTrackerListForPacket(LiveReportTracker, packet.end_point.address().to_v4().to_ulong(), response_packet.response.tracker_info_, real_region.RigionCode);

    DoSendPacket(response_packet, packet.peer_version_);

    LOG4CPLUS_INFO(Loggers::BootStrap(), "send " << response_packet.response.tracker_info_.size() << " live report tracker servers");

    if (response_packet.response.tracker_info_.empty())
    {
        LOG4CPLUS_WARN(Loggers::BootStrap(), "Response 0 live report server to " << packet.end_point.address().to_v4().to_string()
            << ", from area code " << real_region.RigionCode);
    }
}

void Bootstrap::OnQueryStunServerList(const protocol::QueryStunServerListPacket & packet)
{
    if(false == m_is_running) return;

    ++statistic[Stun];

    QListPakt pakt;
    pakt.transaction_id_ = 0;
    protocol::REGION_INFO real_region = GenRealRegion(packet.end_point, pakt);
    m_ac_stat[real_region.RigionCode].Add();

    if (++stun_request_count_in_period_ > 100)
    {
        stun_servers_.ChangeServersSequence();
        stun_request_count_in_period_ = 0;
    }

    LOG4CPLUS_INFO(Loggers::BootStrap(), "reveive stun server list request, ip = " << packet.end_point.address().to_v4().to_string() << 
        ", area code = " << real_region.RigionCode);

    protocol::QueryStunServerListPacket response_packet;
    response_packet.transaction_id_ = packet.transaction_id_;
    response_packet.IsRequest = 0;
    response_packet.error_code_ = 0;
    response_packet.end_point = packet.end_point;

    boost::uint32_t server_based_on_isp_count = static_cast<boost::uint32_t>((stun_total_count_ + 0.0) * stun_based_on_isp_percentage_ / 100);
    stun_servers_.GetServerList(packet.end_point.address().to_v4().to_ulong(), response_packet.response.stun_infos_, server_based_on_isp_count, stun_total_count_ - server_based_on_isp_count);

    LOG4CPLUS_INFO(Loggers::BootStrap(), "send " << response_packet.response.stun_infos_.size() << " stun tracker servers");

    if (response_packet.response.stun_infos_.empty())
    {
        LOG4CPLUS_WARN(Loggers::BootStrap(), "Response 0 stun server to " << packet.end_point.address().to_v4().to_string()
            << ", from area code " << real_region.RigionCode);
    }

    DoSendPacket(response_packet, packet.peer_version_);
}

void Bootstrap::OnQueryIndexServerList(const protocol::QueryIndexServerListPacket & packet)
{
    if(false == m_is_running) return;

    ++statistic[Index];

    QListPakt pakt;
    pakt.transaction_id_ = 0;
    protocol::REGION_INFO real_region = GenRealRegion(packet.end_point, pakt);
    m_ac_stat[real_region.RigionCode].Add();

    LOG4CPLUS_INFO(Loggers::BootStrap(), "receive index server list request, ip = " << packet.end_point.address().to_v4().to_string() <<
        ", area code = " << real_region.RigionCode);

    protocol::QueryIndexServerListPacket response_packet;
    if (m_old_index_list_packet)
    {
        std::vector<boost::uint8_t> temp_mod_index_map_(protocol::INDEX_LIST_MOD_COUNT);

        for(boost::uint32_t i = 0; i < protocol::INDEX_LIST_MOD_COUNT; ++i)
        {
            temp_mod_index_map_[i] = m_old_index_list_packet->response.mod_index_map_[i];
        }
        response_packet = protocol::QueryIndexServerListPacket(
            packet.transaction_id_,
            //m_old_index_list_packet->response.mod_index_map_,
            temp_mod_index_map_,
            m_old_index_list_packet->response.index_servers_, //GetIndexServers()
            packet.end_point
            );
    }
    else
    {
        response_packet = protocol::QueryIndexServerListPacket(
            packet.transaction_id_,
            1,
            packet.end_point
            );
    }
    LOG4CPLUS_INFO(Loggers::BootStrap(), "send " << response_packet.response.index_servers_.size() << " index servers");

    if (response_packet.response.index_servers_.empty())
    {
        LOG4CPLUS_WARN(Loggers::BootStrap(), "Response 0 index server to " << packet.end_point.address().to_v4().to_string()
            << ", from area code " << real_region.RigionCode);
    }

    DoSendPacket(response_packet, packet.peer_version_);
}

void Bootstrap::OnQueryDataCollectionServer(const protocol::QueryDataCollectionServerPacket & packet)
{
    if(false == m_is_running) return;
    QListPakt pakt;
    pakt.transaction_id_ = 0;
    protocol::REGION_INFO real_region = GenRealRegion(packet.end_point, pakt);
    m_ac_stat[real_region.RigionCode].Add();

    LOG4CPLUS_INFO(Loggers::BootStrap(), "reveive data collection server list request, ip = " << packet.end_point.address().to_v4().to_string() <<
        ", area code = " << real_region.RigionCode);

    map<boost::uint32_t, OldPaktCache>::iterator it_cache 
        = m_old_pakt_cache.find(real_region.RigionCode);

    if (it_cache != m_old_pakt_cache.end())
    {
        boost::shared_ptr<protocol::QueryDataCollectionServerPacket> cache_packet;
        cache_packet = it_cache->second.collection_packet;

        protocol::QueryDataCollectionServerPacket response_packet(
            packet.transaction_id_,
            cache_packet->response.server_info_,   //GetDataCollectionServerInfo()
            packet.end_point
            );

        LOG4CPLUS_INFO(Loggers::BootStrap(), "send data collection server, ip = " << response_packet.response.server_info_.IP);

        DoSendPacket(response_packet, packet.peer_version_);
    }
}

void Bootstrap::OnQueryNotifyServer(const protocol::QueryNotifyListPacket & packet)
{
    if (false == m_is_running) return;

    ++statistic[Notify];

    QListPakt pakt;
    pakt.transaction_id_ = 0;
    protocol::REGION_INFO real_region = GenRealRegion(packet.end_point, pakt);
    m_ac_stat[real_region.RigionCode].Add();

    LOG4CPLUS_INFO(Loggers::BootStrap(), "reveive notify server list request, ip = " << packet.end_point.address().to_v4().to_string() <<
        ", area code = " << real_region.RigionCode);

    map<boost::uint32_t, OldPaktCache>::iterator it_cache 
        = m_old_pakt_cache.find(real_region.RigionCode);

    if (it_cache == m_old_pakt_cache.end())
    {
        it_cache = m_old_pakt_cache.find(m_ip_lib.PublicAc());
    }

    if (it_cache != m_old_pakt_cache.end())
    {
        boost::shared_ptr<protocol::QueryNotifyListPacket> cache_packet;
        cache_packet = it_cache->second.notify_packet;

        protocol::QueryNotifyListPacket response_packet(
            packet.transaction_id_,
            0,
            cache_packet->response.notify_server_info_,
            packet.end_point
            );

        LOG4CPLUS_INFO(Loggers::BootStrap(), "send " << response_packet.response.notify_server_info_.size() << " notify servers");

        if (response_packet.response.notify_server_info_.empty())
        {
            LOG4CPLUS_WARN(Loggers::BootStrap(), "Response 0 notify server to " << packet.end_point.address().to_v4().to_string()
                << ", from area code " << real_region.RigionCode);
        }

        DoSendPacket(response_packet, packet.peer_version_);
    }
    else
    {
        std::vector<protocol::NOTIFY_SERVER_INFO> empty_notify_server;
        protocol::QueryNotifyListPacket response_packet(
            packet.transaction_id_,
            0,
            empty_notify_server,
            packet.end_point
            );

        DoSendPacket(response_packet, packet.peer_version_);
    }
}


boost::uint32_t Bootstrap::ConfigDownload(void * param)
{
    Bootstrap *p_bootstrap = (Bootstrap *)param;

    if (NULL == p_bootstrap)
    {
        return 1;
    }

    if (p_bootstrap->m_BS_CONF_URL.empty())
    {
        return 2;
    }

    util::protocol::HttpClient *http_client = new util::protocol::HttpClient(MainThread::IOS());
    boost::system::error_code *ec = new boost::system::error_code();

    http_client->fetch_get(
        p_bootstrap->m_BS_CONF_URL, 
        *ec);

    if(!(*ec))
    {
        boost::asio::streambuf & data = http_client->response().data();
        boost::asio::const_buffer respond_data = data.data();
        const char * p_d = boost::asio::buffer_cast<const char *>( respond_data );
        int size = boost::asio::buffer_size( respond_data );

        FILE * fp = fopen((p_bootstrap->m_config+".tmp").c_str(), "wb");
        fwrite( p_d , size , 1 , fp );
        fclose( fp );
    }
    delete http_client;

    MainThread::Post(boost::bind(&Bootstrap::OnConfigDownload, p_bootstrap, ec));

    return 0;
}

void Bootstrap::OnConfigDownload(const boost::system::error_code* ec)
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::BootStrap(), "Bootstrap::OnConfigDownload()");
    m_is_downloading_config = false;
    if (!(*ec))
    {
        if (false == m_BS_CONF_URL.empty())
        {
            LOG4CPLUS_INFO(Loggers::BootStrap(), "BS Config下载成功，Url: " << m_BS_CONF_URL);
            my_file_io::FMove((m_config+".tmp"), m_config, true);
        }

        LOG4CPLUS_INFO(Loggers::BootStrap(), "开始加载配置文件...");

        UpdateConfig();
        
        if (m_MIS_SYNTIME)
        {
            m_mis_timer->interval(m_MIS_SYNTIME);

            m_mis_timer->start();
        }

        LOG4CPLUS_INFO(Loggers::BootStrap(), "配置文件加载完毕");

        if (ip_distribution_interval_)
        {
            ip_distribution_timer_->interval(ip_distribution_interval_);
            ip_distribution_timer_->start();
        }

        LoadDataInfo();
    }
    else
    {
        LOG4CPLUS_WARN(Loggers::BootStrap(), "配置文件下载失败,不加载, Url: " << m_BS_CONF_URL);
    }
    delete ec;
}

void Bootstrap::ShowStatus()
{
    printf(
        "+++++++++++++++++++++++ 服务器运行状态 +++++++++++++++++++++++\n"
        );

    printf(
        "--------------\n"
        "[当前配置信息]\n"
        "--------------\n"
        );
    printf("%-25s%u\n", "UDP端口: ", m_UDP_PORT);
    printf("%-25s%u\n", "自动加载数据时间间隔: ", m_MIS_SYNTIME);
    printf("%-25s%u\n", "是否允许与MIS同步: ", m_IF_SYN_MIS);
    printf("%-25s%s\n", "MIS同步文件路径: ", m_MIS_SYNDIR.c_str());
    printf("%-25s%s\n", "本地拷贝文件路径: ", m_CONF_DIR.c_str());
}

void Bootstrap::ShowPaktStat()
{
    PacketStat::StatData recv_stat = m_recv_stat.GetStat();
    PacketStat::StatData send_stat = m_send_stat.GetStat();
    printf("+++++++++++++++++++++++ 总收发报文统计 +++++++++++++++++++++++\n");
    printf("%-20s%20s%20s\n", "[包类型]", "最近一分钟", "最近半小时");
    printf("------------------------------------------------------------\n");
    printf("%-20s%18.2f/s%18.2f/s\n", "[接收]", recv_stat.minute_sp,
        recv_stat.hour_sp);
    printf("%-20s%18.2f/s%18.2f/s\n", "[发送]", send_stat.minute_sp,
        send_stat.hour_sp);
    printf("+++++++++++++++++++++++ ISP报文统计 +++++++++++++++++++++++\n");
    printf("%-20s%20s%20s\n", "[AreaCode]", "最近一分钟", "最近半小时");
    printf("------------------------------------------------------------\n");
    for (std::map<boost::uint32_t, PacketStat>::iterator it = m_ac_stat.begin();
        it != m_ac_stat.end(); ++it)
    {
        printf("%-20u%18.2f/s%18.2f/s\n", it->first, it->second.GetStat().minute_sp,
            it->second.GetStat().hour_sp); 
    }
}

void Bootstrap::RegisterAllPackets()
{
    if (false == m_is_staring)
        return ;
    protocol::register_bootstrap_packet(*m_udp_server);
    protocol::register_index_packet(*m_udp_server);
}

void Bootstrap::OnQuerySuperNodeList(const protocol::QuerySnListPacket & packet)
{
    LOG4CPLUS_INFO(Loggers::BootStrap(), "receive super node list request, ip = " << packet.end_point.address().to_v4().to_string());

    ++statistic[SuperNode];

    protocol::QuerySnListPacket response_packet;
    response_packet.IsRequest = 0;
    response_packet.transaction_id_ = packet.transaction_id_;
    response_packet.error_code_ = 0;
    response_packet.end_point = packet.end_point;

    boost::uint32_t ip = packet.end_point.address().to_v4().to_ulong();

    common_super_node_.GetServerList(ip, response_packet.response.super_node_infos_);

    DoSendPacket(response_packet, packet.peer_version_);

    LOG4CPLUS_INFO(Loggers::BootStrap(), "send " << response_packet.response.super_node_infos_.size() << " super node servers.");

    if (response_packet.response.super_node_infos_.empty())
    {
        LOG4CPLUS_WARN(Loggers::BootStrap(), "Response 0 super node server to " << packet.end_point.address().to_v4().to_string());
    }
}

void Bootstrap::OnQueryVipSuperNodeList(const protocol::QueryVipSnListPacket & packet)
{
    LOG4CPLUS_INFO(Loggers::BootStrap(), "receive super node list request, ip = " << packet.end_point.address().to_v4().to_string());

    ++statistic[VipSuperNode];

    protocol::QueryVipSnListPacket response_packet;
    response_packet.IsRequest = 0;
    response_packet.transaction_id_ = packet.transaction_id_;
    response_packet.error_code_ = 0;
    response_packet.end_point = packet.end_point;

    boost::uint32_t ip = packet.end_point.address().to_v4().to_ulong();

    vip_super_node.GetServerList(ip, response_packet.response.super_node_infos_);

    DoSendPacket(response_packet, packet.peer_version_);

    LOG4CPLUS_INFO(Loggers::BootStrap(), "send " << response_packet.response.super_node_infos_.size() << " vip super node servers.");

    if (response_packet.response.super_node_infos_.empty())
    {
        LOG4CPLUS_WARN(Loggers::BootStrap(), "Response 0 vip super node tracker server to " << packet.end_point.address().to_v4().to_string());
    }
}

void Bootstrap::OnQueryTrackerForListingList(const protocol::QueryTrackerForListingPacket & packet)
{
    if(false == m_is_running)
    {
        return;
    }

    QListPakt pakt;
    pakt.transaction_id_ = 0;
    protocol::REGION_INFO real_region = GenRealRegion(packet.end_point, pakt);
    m_ac_stat[real_region.RigionCode].Add();

    protocol::QueryTrackerForListingPacket response_packet;
    response_packet.transaction_id_ = packet.transaction_id_;
    response_packet.IsRequest = 0;
    response_packet.error_code_ = 0;
    response_packet.end_point = packet.end_point;
    response_packet.tracker_type_ = packet.tracker_type_;

    if (packet.tracker_type_ == protocol::QueryTrackerForListingPacket::VOD_TRACKER_FOR_LISTING)
    {
        LOG4CPLUS_INFO(Loggers::BootStrap(), "receive vod list tracker request, ip = " << packet.end_point.address().to_v4().to_string() <<
            " area code = " << real_region.RigionCode);

        Tracker & trackers = tracker_servers_[VodListTracker];
        trackers.Visit(real_region.RigionCode);

        response_packet.response.tracker_group_count_ = trackers.GetModNumber(real_region.RigionCode);

        GetTrackerListForPacket(VodListTracker, packet.end_point.address().to_v4().to_ulong(), response_packet.response.tracker_info_, real_region.RigionCode);
    }
    else
    {
        LOG4CPLUS_INFO(Loggers::BootStrap(), "receive live list tracker request, ip = " << packet.end_point.address().to_v4().to_string() <<
            " area code = " << real_region.RigionCode);

        Tracker & trackers = tracker_servers_[LiveListTracker];
        trackers.Visit(real_region.RigionCode);

        tracker_servers_[LiveListUdpServerTracker].Visit(real_region.RigionCode);

        response_packet.response.tracker_group_count_ = trackers.GetModNumber(real_region.RigionCode);

        GetTrackerListForPacket(LiveListTracker, packet.end_point.address().to_v4().to_ulong(), response_packet.response.tracker_info_, real_region.RigionCode);
    }

    DoSendPacket(response_packet, packet.peer_version_);
    LOG4CPLUS_INFO(Loggers::BootStrap(), "send " << response_packet.response.tracker_info_.size() << " trackers");

    if (response_packet.response.tracker_info_.empty())
    {
        LOG4CPLUS_WARN(Loggers::BootStrap(), "Response 0 list tracker server to " << packet.end_point.address().to_v4().to_string()
            << ", from area code " << real_region.RigionCode);
    }
}

void Bootstrap::DownloadIPDistributionDat()
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::BootStrap(), "Bootstrap::DownloadIPDistributionDat()");
    boost::shared_ptr<util::protocol::HttpClient> http_client = boost::shared_ptr<util::protocol::HttpClient>(new util::protocol::HttpClient(MainThread::IOS()));
    http_client->async_fetch(ip_distribution_url_, boost::bind(&Bootstrap::HandleDownloadIPDistributionDat, shared_from_this(), http_client, _1));
}

void Bootstrap::HandleDownloadIPDistributionDat(boost::shared_ptr<util::protocol::HttpClient> http_client, const boost::system::error_code& err)
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::BootStrap(), "Bootstrap::HandleDownloadIPDistributionDat()");
    if (!err)
    {
        boost::asio::streambuf & data = http_client->response().data();
        boost::asio::const_buffer respond_data = data.data();

        const char * config_content = boost::asio::buffer_cast<const char *>(respond_data);
        int size = boost::asio::buffer_size(respond_data);

        std::ofstream ip_distribution_file((IpDistributionFilename_ + ".tmp").c_str(),
            std::ios_base::out|std::ios_base::binary);

        if (ip_distribution_file)
        {
            ip_distribution_file.write(config_content, size);
            ip_distribution_file.close();
        }

        my_file_io::FMove(IpDistributionFilename_ + ".tmp", IpDistributionFilename_, true);

        IpLocation::SetIpLocation(IpDistributionFilename_);

        GenerateSuperNodeList();
        GenerateAllTrackerList();
        GenerateStunList();
    }
}

void Bootstrap::GenerateSuperNodeList()
{
    LOG4CPLUS_TRACE_METHOD(Loggers::BootStrap(), "Bootstrap::GenerateSuperNodeList()");

    common_super_node_.SetServers(m_srv_pack.GetSuperNodeServers());
    vip_super_node.SetServers(m_srv_pack.GetVipSuperNodeServers());
}

void Bootstrap::GenerateAllTrackerList()
{
    GenerateTrackerList(VodReportTracker);
    GenerateTrackerList(VodListTracker);
    GenerateTrackerList(LiveReportTracker);
    GenerateTrackerList(LiveListTracker);
}

void Bootstrap::GenerateStunList()
{
    LOG4CPLUS_TRACE_METHOD(Loggers::BootStrap(), "Bootstrap::GenerateStunList()");

    const SrvList& stun_srv = m_srv_pack.GetStunSrv();
    vector<protocol::STUN_SERVER_INFO> stun_servers;
    for (vector<SrvInfo>::const_iterator it = stun_srv.sevs.begin();
        it != stun_srv.sevs.end(); ++it)
    {
        protocol::STUN_SERVER_INFO ssi;
        ssi.IP = it->ip;
        ssi.Port = it->port;
        ssi.Type = it->prtcl;
        ssi.Length = sizeof(protocol::STUN_SERVER_INFO);
        stun_servers.push_back(ssi);
    }

    stun_servers_.SetServers(stun_servers);
}

boost::uint32_t Bootstrap::GetServerCountBasedOnIsp(boost::uint32_t total_server_count, boost::uint32_t percentage, boost::uint8_t mod)const
{
    if (mod == 0)
    {
        return 0;
    }

    return static_cast<boost::uint32_t>((total_server_count + 0.0 ) * percentage / 100 / mod);
}

boost::uint32_t Bootstrap::GetRandomServerCount(boost::uint32_t total_server_count, boost::uint32_t based_on_isp_percentage, boost::uint8_t mod) const
{
    assert(based_on_isp_percentage <= 100);

    if (mod == 0)
    {
        return 0;
    }

    return static_cast<boost::uint32_t>((total_server_count + 0.0) * (100 - based_on_isp_percentage) / 100 / mod);
}

void Bootstrap::OnLogTimerElapsed()
{
    std::ofstream status_file("BootstrapStatus.txt");

    if (!status_file)
    {
        LOG4CPLUS_ERROR(Loggers::BootStrap(), "No status file!");
        log_timer_->expires_from_now(boost::posix_time::seconds(60));
        log_timer_->async_wait(boost::bind(&Bootstrap::OnLogTimerElapsed, shared_from_this()));
        return;
    }

    for (boost::uint8_t i = VodReportTracker; i <= LiveListTracker; ++i)
    {
        if (tracker_servers_.find(i) == tracker_servers_.end())
        {
            status_file << 0 << std::endl;
        }
        else
        {
            status_file << tracker_servers_[i].GetStatistic() << std::endl;
            tracker_servers_[i].ClearStatistic();
        }
    }

    for (boost::uint8_t i = 0; i < PacketMaxType; ++i)
    {
        status_file << statistic[i] << std::endl;
    }

    status_file.close();

    memset(statistic, 0, sizeof(statistic));

    log_timer_->expires_from_now(boost::posix_time::seconds(60));
    log_timer_->async_wait(boost::bind(&Bootstrap::OnLogTimerElapsed, shared_from_this()));
}

void Bootstrap::GenerateTrackerList(boost::uint8_t tracker_type)
{
    assert(tracker_servers_.find(tracker_type) != tracker_servers_.end());

    tracker_servers_[tracker_type].Clear();
    const ServerPacker::AcSevMap_T& ac_sev_map = m_srv_pack.GetSevMap();

    for (ServerPacker::AcSevMap_T::const_iterator it = ac_sev_map.begin();
        it != ac_sev_map.end(); ++it)
    {
        const AcServer& ac_srv = it->second;

        if (false == ac_srv.GetTrackerServerMap(tracker_type).empty())
        {
            ServerMap_T::const_iterator it_srv = ac_srv.GetTrackerServerMap(tracker_type).find(0);
            if (it_srv != ac_srv.GetTrackerServerMap(tracker_type).end())
            {
                std::map<boost::uint8_t, std::vector<protocol::TRACKER_INFO> > isp_trackers;
                std::map<boost::uint8_t, std::vector<protocol::TRACKER_INFO> > isp_trackers_for_udpserver;

                const SrvList& srv_list = it_srv->second;
                for (vector<SrvInfo>::const_iterator it = srv_list.sevs.begin();
                    it != srv_list.sevs.end(); ++it)
                {
                    protocol::TRACKER_INFO tracker_info = it->GenerateTracker(tracker_type);

                    if (tracker_info.IsTrackerForLiveUdpServer())
                    {
                        AddTracker(tracker_info, isp_trackers_for_udpserver);
                    }
                    else
                    {
                        AddTracker(tracker_info, isp_trackers);
                    }
                }

                Tracker::TrackerInSameIsp tracker;
                for (std::map<boost::uint8_t, std::vector<protocol::TRACKER_INFO> >::const_iterator iter = isp_trackers.begin();
                    iter != isp_trackers.end(); ++iter)
                {
                    tracker[iter->first].SetServers(iter->second);
                }

                tracker_servers_[tracker_type].Insert(it->first, tracker);

                Tracker::TrackerInSameIsp udpserver_tracker;
                for (std::map<boost::uint8_t, std::vector<protocol::TRACKER_INFO> >::const_iterator iter = isp_trackers_for_udpserver.begin();
                    iter != isp_trackers_for_udpserver.end(); ++iter)
                {
                    udpserver_tracker[iter->first].SetServers(iter->second);
                }

                if (tracker_type == LiveReportTracker)
                {
                    tracker_servers_[LiveUdpServerReportTracker].Insert(it->first, udpserver_tracker);
                }
                else if (tracker_type == LiveListTracker)
                {
                    tracker_servers_[LiveListUdpServerTracker].Insert(it->first, udpserver_tracker);
                }
            }
        }
    }
}

bool Bootstrap::IsReportTracker(boost::uint8_t tracker_type)
{
    switch(tracker_type)
    {
    case VodReportTracker:
    case LiveReportTracker:
    case LiveUdpServerReportTracker:
        return true;

    default:
        return false;
    }
}

void Bootstrap::AddTracker(const protocol::TRACKER_INFO & tracker_info, std::map<boost::uint8_t, std::vector<protocol::TRACKER_INFO> > & trackers)
{
    if (trackers.find(tracker_info.ModNo) == trackers.end())
    {
        std::vector<protocol::TRACKER_INFO> trackers_in_the_same_isp;
        trackers_in_the_same_isp.push_back(tracker_info);
        trackers.insert(std::make_pair(tracker_info.ModNo, trackers_in_the_same_isp));
    }
    else
    {
        trackers[tracker_info.ModNo].push_back(tracker_info);
    }
}

void Bootstrap::GetTrackerListForPacket(boost::uint8_t tracker_type, boost::uint32_t ip, std::vector<protocol::TRACKER_INFO> & selected_trackers,
                               boost::uint32_t rigion_code)
{
    Tracker & trackers = tracker_servers_[tracker_type];

    if (!trackers.DoesTrackerExist(rigion_code) || trackers.GetModNumber(rigion_code) == 0)
    {
        LOG4CPLUS_WARN(Loggers::BootStrap(), "The tracker list for area code " << rigion_code << " is empty.");
        rigion_code = m_ip_lib.PublicAc();
    }

    if (tracker_type == LiveReportTracker || tracker_type == LiveListTracker)
    {
        boost::uint32_t udpserver_server_count = tracker_servers_[tracker_type + 2].GetTotalServerCount(rigion_code);

        assert(trackers.GetModNumber(rigion_code) == tracker_servers_[tracker_type + 2].GetModNumber(rigion_code));

        GetTrackerList(tracker_type + 2, rigion_code, ip, udpserver_server_count, 0, selected_trackers);
    }

    boost::uint32_t selected_tracker_count = selected_trackers.size();

    boost::uint32_t server_count_in_one_response = trackers.GetServerCountInOneResponse();
    boost::uint32_t based_on_isp_percentage = trackers.GetBasedOnIspPercentage();

    if (server_count_in_one_response > selected_tracker_count)
    {
        boost::uint32_t servers_based_on_isp_count_in_one_mod = GetServerCountBasedOnIsp(server_count_in_one_response - selected_tracker_count,
            based_on_isp_percentage, trackers.GetModNumber(rigion_code));

        boost::uint32_t random_servers_count_in_one_mod = GetRandomServerCount(server_count_in_one_response - selected_tracker_count,
            based_on_isp_percentage, trackers.GetModNumber(rigion_code));

        GetTrackerList(tracker_type, rigion_code, ip, servers_based_on_isp_count_in_one_mod,
            random_servers_count_in_one_mod, selected_trackers);
    }
    else
    {
        LOG4CPLUS_WARN(Loggers::BootStrap(), "Too many udpserver trackers, udpserver server count = " << selected_tracker_count
            << " live tracker total count = " << server_count_in_one_response);
    }
}

void Bootstrap::GetTrackerList(boost::uint8_t tracker_type, boost::uint32_t rigion_code, boost::uint32_t ip,
                               boost::uint32_t servers_based_on_isp_count_in_one_mod,
                               boost::uint32_t random_servers_count_in_one_mod,
                               std::vector<protocol::TRACKER_INFO> & select_trackers)
{
    assert(tracker_servers_.find(tracker_type) != tracker_servers_.end());

    Tracker::TrackerInSameIsp & tracker = tracker_servers_[tracker_type].GetTrackers(rigion_code);

    for (Tracker::TrackerInSameIsp::iterator iter = tracker.begin(); iter != tracker.end(); ++iter)
    {
        std::vector<protocol::TRACKER_INFO> tmp;
        iter->second.GetServerList(ip, tmp, servers_based_on_isp_count_in_one_mod, random_servers_count_in_one_mod);
        for (std::vector<protocol::TRACKER_INFO>::iterator inner_iter = tmp.begin();
            inner_iter != tmp.end(); ++inner_iter)
        {
            select_trackers.push_back(*inner_iter);
        }
    }
}


BOOTSTRAP_NAMESPACE_END
