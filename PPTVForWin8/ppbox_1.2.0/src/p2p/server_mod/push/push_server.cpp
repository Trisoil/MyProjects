/* ======================================================================
 *	push_server.cpp
 *	Copyright (c) 2009 Synacast. All rights reserved.
 *
 *	push server主线程网络模块
 * ======================================================================
 *	  Time         Changer     ChangeLog
 *	~~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 *	2009-06-04     cnhbdu      创建
 */

#include "stdafx.h"

#include "push_server.h"

#include <cstdio>
#include <cstdlib>

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/date_time.hpp>
#include <boost/asio/placeholders.hpp>

extern log4cplus::Logger g_logger;
extern log4cplus::Logger g_statlog;
extern log4cplus::Logger g_statistics;

PUSH_SERVER_NAMESPACE_BEGIN

using protocol::QueryPushTaskPacketV2;
using protocol::QueryPushTaskPacketV3;

FRAMEWORK_LOGGER_DECLARE_MODULE("server");
static boost::shared_ptr<framework::timer::AsioTimerManager> timer_manager;

AsioThread::p main_thread()
{
    static AsioThread::p main_thread = AsioThread::create();
    return main_thread;
}

AsioThread::p read_thread()
{
    static AsioThread::p read_thread = AsioThread::create();
    return read_thread;
}

AsioThread::p working_task_read_write_thread()
{
    static AsioThread::p working_task_read_write_thread = AsioThread::create();
    return working_task_read_write_thread;
}

framework::timer::TimerQueue & global_second_timer()
{
    if (!timer_manager)
    {
        timer_manager.reset(new framework::timer::AsioTimerManager(main_thread()->Ios(), boost::posix_time::seconds(1)));
        timer_manager->start();
    }

    return *timer_manager;
}

void reset_global_second_timer()
{
    timer_manager.reset();
}


PushServer::PushServer() : m_statistics_timer_(global_second_timer(), 1000, boost::bind(&PushServer::OnTimerElapsed, this, &m_statistics_timer_))
{
	// 初始化配置项

	m_is_running = false;
    m_content_manager = ContentManager::p(new ContentManager);
}

void PushServer::Start(const string& config_file)
{
	if (m_is_running) return;

	// 初始化定时器
	CONSOLE_LOG("Init Timer...");
//	PS_LOG("Init Timer...");

	// 加载配置文件，并设定配置值
	m_cfg_filename = config_file;
	LoadConfig();

    std::string log_conf = m_content_cfg.CONTENT_PUSH_LOG_CONFNAME;
    log4cplus::PropertyConfigurator::doConfigure(log_conf);

    // 设定UDP端口，开始收包
    m_udp_server.reset(new protocol::UdpServer(main_thread()->Ios(), shared_from_this()));
    if ( false == m_udp_server->Listen(m_config.UDP_PORT) )
    {
        CONSOLE_LOG("StartUP failed:: cannot bind to port.");
        LOG4CPLUS_ERROR(g_logger, "StartUP failed:: cannot bind to port.");
        exit(0);
    }

    m_udp_client.reset(new boost::asio::ip::udp::socket(main_thread()->Ios()));
    m_udp_client->open(boost::asio::ip::udp::v4());

    m_content_manager->Start();

    m_udp_server->Recv(m_config.UDP_RECV_LEN);
    CONSOLE_LOG( "Startup Successful, listen " << m_config.UDP_PORT << " UDP port." );
    LOG4CPLUS_INFO(g_logger, "Startup Successful, listen " << m_config.UDP_PORT << " udp port." );

    register_push_packetv2(*m_udp_server);
    register_push_packetv3(*m_udp_server);
    m_is_running = true;

    m_statistics_timer_->start();
}

void PushServer::Stop()
{
	if (false == m_is_running) return;

	if (m_udp_server)
	{
		m_udp_server->Close();
		m_udp_server.reset();
	}

    if (m_udp_client) {
        m_udp_client->close();
        m_udp_client.reset();
    }

    m_content_manager->Stop();

	m_is_running = false;
}

void PushServer::OnUdpRecv(protocol::Packet const & packet)
{
	if (false == m_is_running) return;

    //检查是否是请求报文，如果不是则不进行任何处理
    if (false == static_cast<protocol::ServerPacket const &>(packet).IsRequest) return;

	m_recv_stat.Add();

	// 检查包头部正确，如果读取包头部失败，不处理
	switch (packet.PacketAction)
	{
    case protocol::ReportPushTaskCompletedPacket::Action:
        OnReportPushTaskComplete((protocol::ReportPushTaskCompletedPacket const &)packet);
        break;
    case protocol::QueryPushTaskPacketV3::Action:
        ++m_push_statistics_.total_recv_;
        ++m_push_statistics_.request_per_sec_;
        OnQueryPushTaskV3((protocol::QueryPushTaskPacketV3 const&)packet);
        break;
    case protocol::QueryPushTaskPacketV2::Action:
        ++m_push_statistics_.total_recv_;
        ++m_push_statistics_.request_per_sec_;
        OnQueryPushTaskV2((protocol::QueryPushTaskPacketV2 const&)packet);
        break;
	default:
		return;
	}
}

template <typename PacketType>
void PushServer::DoSendPacket(PacketType const & packet, boost::uint16_t dest_protocol_version)
{
	if(false == m_is_running) return;

	m_udp_server->send_packet(packet, dest_protocol_version);
}

void PushServer::LoadConfig()
{
	CONSOLE_LOG("Loading Config File...");
//	PS_LOG("Loading Config File...");
	UpdateConfig();

    m_content_manager->UpdateConfig(m_content_cfg);

	CONSOLE_LOG("Done Loading Config File.");
//	PS_LOG("Done Loading Config File.");
}

void PushServer::UpdateConfig()
{
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");

    desc.add_options()
        ("push_server.UDP_PORT", po::value<uint16_t>(&m_config.UDP_PORT))
        ("push_server.UDP_RECV_LEN", po::value<uint32_t>(&m_config.UDP_RECV_LEN))
        ("push_server.PUSH_WAIT_TIME", po::value<uint32_t>(&m_config.PUSH_WAIT_TIME))
        ("push_server.HOT_STATISTIC_SERVER", po::value<string>(&m_config.HOT_STATISTIC_SERVER))
        ("push_server.HOT_STATISTIC_SERVER_PORT", po::value<uint16_t>(&m_config.HOT_STATISTIC_SERVER_PORT));
    desc.add_options()
        ("reader.MAX_IDLE_SPEED_KB", po::value<uint16_t>(&m_reader_cfg.MAX_IDLE_SPEED_KB))
        ("reader.MAX_NORMAL_SPEED_KB", po::value<uint16_t>(&m_reader_cfg.MAX_NORMAL_SPEED_KB))
        ("reader.MIN_SPEED_KB", po::value<uint16_t>(&m_reader_cfg.MIN_SPEED_KB))
        ("reader.PUSH_INTERVAL_SEC", po::value<uint16_t>(&m_reader_cfg.PUSH_INTERVAL_SEC))
        ("reader.IDLE_SPEED_BW_RATIO", po::value<float>(&m_reader_cfg.IDLE_SPEED_BW_RATIO))
        ("reader.NORMAL_SPEED_BW_RATIO", po::value<float>(&m_reader_cfg.NORMAL_SPEED_BW_RATIO))
        ("reader.UPDATE_TASK_TIME", po::value<uint32_t>(&m_reader_cfg.UPDATE_TASK_TIME))
        ("reader.IS_GEN_KEY_EVERYTIME", po::value<bool>(&m_reader_cfg.IS_GEN_KEY_EVERYTIME))
        ("reader.GEN_KEY_TIMER", po::value<uint32_t>(&m_reader_cfg.GEN_KEY_TIMER));
    desc.add_options()
        ("content_manager.CONTENT_PUSH_DISABLED", po::value<uint16_t>(&m_content_cfg.CONTENT_PUSH_DISABLED))
        ("content_manager.CONTENT_PUSHAHEAD_NUM", po::value<uint16_t>(&m_content_cfg.CONTENT_PUSHAHEAD_NUM))
        ("content_manager.CONTENT_SESSION_SAVE_FILE", po::value<string>(&m_content_cfg.CONTENT_SESSION_SAVE_FILE))
        ("content_manager.CONTENT_HISTORY_DAYS", po::value<uint16_t>(&m_content_cfg.CONTENT_HISTORY_DAYS))
        ("content_manager.CONTENT_HOT_RETIRE_TIME", po::value<uint32_t>(&m_content_cfg.CONTENT_HOT_RETIRE_TIME))
        ("content_manager.CONTENT_HOT_EP_RETIRE_TIME", po::value<uint32_t>(&m_content_cfg.CONTENT_HOT_EP_RETIRE_TIME))
        ("content_manager.CONTENT_NEWHOT_XML_FILENAME", po::value<string>(&m_content_cfg.CONTENT_NEWHOT_XML_FILENAME))
        ("content_manager.CONTENT_NEWUPLOAD_XML_FILENAME", po::value<string>(&m_content_cfg.CONTENT_NEWUPLOAD_XML_FILENAME))
        ("content_manager.CONTENT_NEWVIP_RID_XML_FILENAME", po::value<string>(&m_content_cfg.CONTENT_NEWVIP_RID_XML_FILENAME))
        ("content_manager.CONTENT_NEWRID_MAKEUP_XML_FILENAME", po::value<string>(&m_content_cfg.CONTENT_NEWRID_MAKEUP_XML_FILENAME))
        ("content_manager.CONTENT_HOTSAVE_FILENAME", po::value<string>(&m_content_cfg.CONTENT_HOTSAVE_FILENAME))
        ("content_manager.CONTENT_VIP_RID_FILENAME", po::value<string>(&m_content_cfg.CONTENT_VIP_RID_FILENAME))
        ("content_manager.CONTENT_RIDS_DAILY_NUM", po::value<uint32_t>(&m_content_cfg.CONTENT_RIDS_DAILY_NUM))
        ("content_manager.CONTENT_RIDS_MINUTE_NUM", po::value<uint32_t>(&m_content_cfg.CONTENT_RIDS_MINUTE_NUM))
        ("content_manager.CONTENT_NEWVIP_RID_TARGET", po::value<uint32_t>(&m_content_cfg.CONTENT_NEWVIP_RID_TARGET))
        ("content_manager.CONTENT_PUSH_BASE_URL", po::value<string>(&m_content_cfg.CONTENT_PUSH_BASE_URL))
        ("content_manager.CONTENT_PUSH_BASE_APPENDIX", po::value<string>(&m_content_cfg.CONTENT_PUSH_BASE_APPENDIX))
        ("content_manager.CONTENT_PUSH_LOG_CONFNAME", po::value<string>(&m_content_cfg.CONTENT_PUSH_LOG_CONFNAME))
        ("content_manager.CONTENT_PUSH_THRESH_RNUM", po::value<uint32_t>(&m_content_cfg.CONTENT_PUSH_THRESH_RNUM))
        ("content_manager.CONTENT_PUSH_THRESH_DURATION", po::value<uint32_t>(&m_content_cfg.CONTENT_PUSH_THRESH_DURATION))
        ("content_manager.CONTENT_PAUSE_START_TIME", po::value<uint32_t>(&m_content_cfg.CONTENT_PAUSE_START_TIME))
        ("content_manager.CONTENT_PAUSE_END_TIME", po::value<uint32_t>(&m_content_cfg.CONTENT_PAUSE_END_TIME))
        ("content_manager.CONTENT_PAUSE_DISABLED", po::value<uint32_t>(&m_content_cfg.CONTENT_PAUSE_DISABLED))
        ("content_manager.CONTENT_HOTSAVE_RELOAD_TIME", po::value<uint32_t>(&m_content_cfg.CONTENT_HOTSAVE_RELOAD_TIME))
        ("content_manager.CONTENT_ACTPUSH_ENABLED", po::value<uint32_t>(&m_content_cfg.CONTENT_ACTPUSH_ENABLED))
        ("content_manager.CONTENT_VIPPUSH_ENABLED", po::value<uint32_t>(&m_content_cfg.CONTENT_VIPPUSH_ENABLED))
        ("content_manager.CONTENT_VIPPUSH_RETIRE_THRESHOLD", po::value<uint32_t>(&m_content_cfg.CONTENT_VIPPUSH_RETIRE_THRESHOLD))
        ("content_manager.CONTENT_ACTPUSH_CONTROL_MINUTE", po::value<uint32_t>(&m_content_cfg.CONTENT_ACTPUSH_CONTROL_MINUTE))
        ("content_manager.CONTENT_ACTPUSH_PER_RID_LIMIT", po::value<uint32_t>(&m_content_cfg.CONTENT_ACTPUSH_PER_RID_LIMIT))
        ("content_manager.CONTENT_ACTPUSH_BIG_BANDWIDTH", po::value<uint32_t>(&m_content_cfg.CONTENT_ACTPUSH_BIG_BANDWIDTH))
        ("content_manager.CONTENT_ACTPUSH_BIG_BW_RATIO", po::value<uint32_t>(&m_content_cfg.CONTENT_ACTPUSH_BIG_BW_RATIO))
        ("content_manager.CONTENT_ACTPUSH_PER_REQ_LIMIT", po::value<uint32_t>(&m_content_cfg.CONTENT_ACTPUSH_PER_REQ_LIMIT))
        ("content_manager.CONTENT_ACTPUSH_MIN_MIN_LIMIT", po::value<uint32_t>(&m_content_cfg.CONTENT_ACTPUSH_MIN_MIN_LIMIT))
        ("content_manager.CONTENT_PUSH_BITRATE_TYPE", po::value<uint16_t>(&m_content_cfg.CONTENT_PUSH_BITRATE_TYPE))
        ("content_manager.CONTENT_PASPUSH_PER_RID_LIMIT", po::value<uint32_t>(&m_content_cfg.CONTENT_PASPUSH_PER_RID_LIMIT))
        ("content_manager.CONTENT_ACTPUSH_BIG_FREE_DISK", po::value<uint32_t>(&m_content_cfg.CONTENT_ACTPUSH_BIG_FREE_DISK))
        ("content_manager.CONTENT_ACTPUSH_BIG_DISK_RATIO", po::value<uint32_t>(&m_content_cfg.CONTENT_ACTPUSH_BIG_DISK_RATIO))
        ("content_manager.CONTENT_ACTPUSH_HIGH_ONLINE_TIME", po::value<uint32_t>(&m_content_cfg.CONTENT_ACTPUSH_HIGH_ONLINE_TIME))
        ("content_manager.CONTENT_ACTPUSH_HIGH_ONLINE_RATIO", po::value<uint32_t>(&m_content_cfg.CONTENT_ACTPUSH_HIGH_ONLINE_RATIO))
        ("content_manager.CONTENT_ACTPUSH_GOOD_NAT_STRING", po::value<string>(&m_content_cfg.CONTENT_ACTPUSH_GOOD_NAT_STRING))
        ;


    std::ifstream ifs(m_cfg_filename.c_str());
    if (ifs)
    {
        po::variables_map vm;
        po::store(po::parse_config_file(ifs, desc), vm);
        po::notify(vm);

        // TODO(herain):2011-05-13:check config
    }

    // TODO(herain):2011-05-13:can not load config???
}

void PushServer::OnReportPushTaskComplete(protocol::ReportPushTaskCompletedPacket const & packet)
{
    //m_task_manager->ReportPushTask(packet.request.peer_guid_, packet.request.rid_info_);
    //protocol::ReportPushTaskCompletedPacket response_pakt(packet.transaction_id_, 
    //    packet.request.rid_info_, packet.end_point);
    //DoSendPacket(response_pakt, packet.peer_version_);
    
}

void PushServer::OnQueryPushTaskV2(protocol::QueryPushTaskPacketV2 const & packet)
{
    //This is ugly.
    //This is temporary until new peer is largely used.
    TaskInfo task_info;
    boost::shared_ptr<QueryPushTaskPacketV2> send_pakt;
    std::vector<protocol::PushTaskItem> ptask_list;
    LOG4CPLUS_DEBUG(g_logger, "Received PushRequestV2 from " << packet.end_point);
    m_push_statistics_.AddIpTimes(packet.end_point.address().to_string());

    //report video play count to statistic server
    std::vector<std::string> vecVideoNames;
    for (size_t i = 0; i < packet.request.play_history_vec_.size(); i++) {
        vecVideoNames.push_back(packet.request.play_history_vec_[i].video_name_);
    }
    ReportVideoPlayCountToStatisticServer(vecVideoNames);

    task_info.type_flag = PUSHTYPE_OPEN_SERVICE;
    task_info.push_interval = m_reader_cfg.PUSH_INTERVAL_SEC;
    task_info.max_idle_speed_kb = m_reader_cfg.MAX_IDLE_SPEED_KB;
    task_info.max_normal_speed_kb = m_reader_cfg.MAX_NORMAL_SPEED_KB;
    task_info.min_speed_kb = m_reader_cfg.MIN_SPEED_KB;
    task_info.idle_speed_bw_ratio = m_reader_cfg.IDLE_SPEED_BW_RATIO;
    task_info.normal_speed_bw_ratio = m_reader_cfg.NORMAL_SPEED_BW_RATIO;

    PeerInfo peer_info(packet.request.peer_guid_, packet.request.used_disk_size_
        , packet.request.upload_bandwidth_kbs_, packet.request.avg_upload_speed_kbs_, 
        0, 0, 0);  //TODO wenjiewang: for compatibility.

    std::vector<protocol::PushTaskItem>::iterator piter;
    int task_count;
    int retcode = m_content_manager->GetPushRIDs(peer_info, packet.request.play_history_vec_, ptask_list);
    switch (retcode)
    {
    case PUSH_HAS_TASK:
    case PUSH_HAS_ACT_TASK:
        piter = ptask_list.begin(); 
        while ( piter != ptask_list.end() )  {
            send_pakt.reset(new QueryPushTaskPacketV2(packet.transaction_id_, task_info.TaskParam(), packet.end_point));
            task_count = CONTENT_CONST_RID_PER_PACKET;
            do {
                send_pakt->response.push_task_vec_.push_back(*piter);
                piter ++;
                task_count --;
            } while ( (task_count>0) && (piter != ptask_list.end()) );
            if (retcode == PUSH_HAS_TASK) {
                m_send_v2_stat.Add();
                m_send_rid_stat.Add(CONTENT_CONST_RID_PER_PACKET - task_count);
            }else{
                m_send_v1_stat.Add(CONTENT_CONST_RID_PER_PACKET - task_count);
            }
            DoSendPacket(*send_pakt, packet.peer_version_);
            LOG4CPLUS_DEBUG(g_logger, "Pushed packet send to " << packet.end_point << " with " << (CONTENT_CONST_RID_PER_PACKET - task_count) << " records" );

            m_push_statistics_.total_push_count_ += CONTENT_CONST_RID_PER_PACKET - task_count;
            m_push_statistics_.push_count_per_sec_ += CONTENT_CONST_RID_PER_PACKET - task_count;
        }

        if (retcode == PUSH_HAS_TASK)
        {
            ++m_push_statistics_.total_passive_push_;
            ++m_push_statistics_.passive_push_per_sec_;
        }

        if (retcode == PUSH_HAS_ACT_TASK)
        {
            ++m_push_statistics_.total_active_push_;
            ++m_push_statistics_.active_push_per_sec_;
        }
        break;

    case PUSH_NO_TASK:
        ++m_push_statistics_.total_no_task_count_;
        ++m_push_statistics_.no_task_count_per_sec_;
        send_pakt.reset(new QueryPushTaskPacketV2(packet.transaction_id_, QueryPushTaskPacketV2::NO_TASK, 
            (int32_t)m_config.PUSH_WAIT_TIME, packet.end_point));
        m_send_v2_null_stat.Add();
        DoSendPacket(*send_pakt, packet.peer_version_);
        break;
    default:
        assert(0);
    }
}

void PushServer::OnQueryPushTaskV3(protocol::QueryPushTaskPacketV3 const & packet)
{
    TaskInfo task_info;
    boost::shared_ptr<QueryPushTaskPacketV3> send_pakt;
    std::vector<protocol::PushTaskItem> ptask_list;
    LOG4CPLUS_DEBUG(g_logger, "Received PushRequest from " << packet.end_point);
    m_push_statistics_.AddIpTimes(packet.end_point.address().to_string());
    
    //report video play count to statistic server
    std::vector<std::string> vecVideoNames;
    for (size_t i = 0; i < packet.request.play_history_vec_.size(); i++) {
        vecVideoNames.push_back(packet.request.play_history_vec_[i].video_name_);
    }
    ReportVideoPlayCountToStatisticServer(vecVideoNames);

    task_info.type_flag = PUSHTYPE_OPEN_SERVICE;
    task_info.push_interval = m_reader_cfg.PUSH_INTERVAL_SEC;
    task_info.max_idle_speed_kb = m_reader_cfg.MAX_IDLE_SPEED_KB;
    task_info.max_normal_speed_kb = m_reader_cfg.MAX_NORMAL_SPEED_KB;
    task_info.min_speed_kb = m_reader_cfg.MIN_SPEED_KB;
    task_info.idle_speed_bw_ratio = m_reader_cfg.IDLE_SPEED_BW_RATIO;
    task_info.normal_speed_bw_ratio = m_reader_cfg.NORMAL_SPEED_BW_RATIO;

    PeerInfo peer_info(packet.request.peer_guid_, packet.request.used_disk_size_
        , packet.request.upload_bandwidth_kbs_, packet.request.avg_upload_speed_kbs_, 
        packet.request.total_disk_size_, packet.request.nat_type_, packet.request.online_percent_);

    std::vector<protocol::PushTaskItem>::iterator piter;
    int task_count;
    int retcode = m_content_manager->GetPushRIDs(peer_info, packet.request.play_history_vec_, ptask_list);
    switch (retcode)
    {
    case PUSH_HAS_TASK:
    case PUSH_HAS_ACT_TASK:
        piter = ptask_list.begin(); 
        while ( piter != ptask_list.end() )  {
            send_pakt.reset(new QueryPushTaskPacketV3(packet.transaction_id_, task_info.TaskParam(), packet.end_point));
            task_count = CONTENT_CONST_RID_PER_PACKET;
            do {
                send_pakt->response.push_task_vec_.push_back(*piter);
                piter ++;
                task_count --;
            } while ( (task_count>0) && (piter != ptask_list.end()) );
            if (retcode == PUSH_HAS_TASK) {
                m_send_v2_stat.Add();
                m_send_rid_stat.Add(CONTENT_CONST_RID_PER_PACKET - task_count);
            }else{
                m_send_v1_stat.Add(CONTENT_CONST_RID_PER_PACKET - task_count);
            }
            DoSendPacket(*send_pakt, packet.peer_version_);
            LOG4CPLUS_DEBUG(g_logger, "Pushed packet send to " << packet.end_point << " with " << (CONTENT_CONST_RID_PER_PACKET - task_count) << " records" );
			
			m_push_statistics_.total_push_count_ += CONTENT_CONST_RID_PER_PACKET - task_count;
			m_push_statistics_.push_count_per_sec_ += CONTENT_CONST_RID_PER_PACKET - task_count;
        }

        if (retcode == PUSH_HAS_TASK)
        {
            ++m_push_statistics_.total_passive_push_;
            ++m_push_statistics_.passive_push_per_sec_;
        }

        if (retcode == PUSH_HAS_ACT_TASK)
        {
            ++m_push_statistics_.total_active_push_;
            ++m_push_statistics_.active_push_per_sec_;
        }
        break;

    case PUSH_NO_TASK:
        ++m_push_statistics_.total_no_task_count_;
        ++m_push_statistics_.no_task_count_per_sec_;
        send_pakt.reset(new QueryPushTaskPacketV3(packet.transaction_id_, QueryPushTaskPacketV3::NO_TASK, 
            (int32_t)m_config.PUSH_WAIT_TIME, packet.end_point));
        m_send_v2_null_stat.Add();
        DoSendPacket(*send_pakt, packet.peer_version_);
        break;
    default:
        assert(0);
    }
}

void PushServer::ReportVideoPlayCountToStatisticServer(const std::vector<std::string>& video_name_vec)
{
    BOOST_ASSERT(m_udp_client);

    std::ostringstream oss;
    std::copy(video_name_vec.begin(), video_name_vec.end(), std::ostream_iterator<std::string>(oss, "\n"));
    
    boost::asio::ip::udp::endpoint dest(boost::asio::ip::address::from_string(m_config.HOT_STATISTIC_SERVER)
        , m_config.HOT_STATISTIC_SERVER_PORT);

    string *str = new string(oss.str());

    m_udp_client->async_send_to(boost::asio::buffer(*str), dest,
        boost::bind(&PushServer::HandleSend, shared_from_this(), str, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void PushServer::ShowConfig()
{
	printf("+++++++++++++++++++++++ Server Config Info+++++++++++++++++++++++\n");
    printf("UDP Port: %d\n", m_config.UDP_PORT);
}

void PushServer::ShowPaktStat()
{
	PacketStat::StatData recv_stat = m_recv_stat.GetStat();
	PacketStat::StatData err_stat = m_recv_err_stat.GetStat();
	PacketStat::StatData send_v1_stat = m_send_v1_stat.GetStat();
	PacketStat::StatData null_v1_stat = m_send_v1_null_stat.GetStat();
    PacketStat::StatData send_v2_stat = m_send_v2_stat.GetStat();
    PacketStat::StatData null_v2_stat = m_send_v2_null_stat.GetStat();
    PacketStat::StatData send_rid_stat = m_send_rid_stat.GetStat();

    printf("+++++++++++++++++++++++ Pkt Statistics +++++++++++++++++++++++\n");
	printf("%-15s%15s%15s%15s\n", "[Pkt Type]", "Last_Minute", "Last_30M", "Last_30M_Total");
	printf("------------------------------------------------------------\n");
	printf("%-15s%13.2f/s%13.2f/s%15u\n", "[RECV]", recv_stat.minute_sp,
		recv_stat.hour_sp, recv_stat.total_num);
	printf("%-15s%13.2f/s%13.2f/s%15u\n", "[ERR]", err_stat.minute_sp,
		err_stat.hour_sp, err_stat.total_num);
	printf("%-15s%13.2f/s%13.2f/s%15u\n", "[SENDTaskV1]", send_v1_stat.minute_sp,
		send_v1_stat.hour_sp, send_v1_stat.total_num);
	printf("%-15s%13.2f/s%13.2f/s%15u\n", "[SENDNoTaskV1]", null_v1_stat.minute_sp,
		null_v1_stat.hour_sp, null_v1_stat.total_num);
    printf("%-15s%13.2f/s%13.2f/s%15u\n", "[SENDTaskV2]", send_v2_stat.minute_sp,
        send_v2_stat.hour_sp, send_v2_stat.total_num);
    printf("%-15s%13.2f/s%13.2f/s%15u\n", "[SENDNoTaskV2]", null_v2_stat.minute_sp,
        null_v2_stat.hour_sp, null_v2_stat.total_num);
    printf("%-15s%13.2f/s%13.2f/s%15u\n", "[SEND_RID]", send_rid_stat.minute_sp,
        send_rid_stat.hour_sp, send_rid_stat.total_num);
}


void PushServer::SavePushStatisticsData()
{
    LOG4CPLUS_INFO(g_statistics, "total recv: " << m_push_statistics_.total_recv_
            << "  Persecond: " << m_push_statistics_.request_per_sec_
            << "   total push: " << m_push_statistics_.total_push_count_
            << "  persecond: " << m_push_statistics_.push_count_per_sec_
            << "   total no task: " << m_push_statistics_.total_no_task_count_
            << "  persecond: " << m_push_statistics_.no_task_count_per_sec_
            << "   total passive push: " << m_push_statistics_.total_passive_push_
            << "  persecond: " << m_push_statistics_.passive_push_per_sec_
            << "   totoal active push: " << m_push_statistics_.total_active_push_
            << "  persecond: " << m_push_statistics_.active_push_per_sec_
            << "  MaxIp: " << m_push_statistics_.GetMaxIp().first
            << "  count: " << m_push_statistics_.GetMaxIp().second);
}

void PushServer::OnTimerElapsed( framework::timer::Timer * timer )
{
    SavePushStatisticsData();
    m_push_statistics_.ClearStat();
}

PUSH_SERVER_NAMESPACE_END
