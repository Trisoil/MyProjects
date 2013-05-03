/* ======================================================================
 *	push_server.h
 *	Copyright (c) 2009 Synacast. All rights reserved.
 *
 *	push server主线程网络模块
 * ======================================================================
 *	  Time         Changer     ChangeLog
 *	~~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 *	2009-06-04     cnhbdu      创建
 */

#ifndef __PUSH_SERVER_H__
#define __PUSH_SERVER_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "push_header.h"
#include "push_struct.h"
#include "packet_stat.h"
#include "task/asio_thread.h"
#include "task/content_matcher.h"
#include "task/content_quota_scheduler.h"
#include <framework/logger/Logger.h>
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>

#include <map>

PUSH_SERVER_NAMESPACE_BEGIN

namespace fmk = framework;

AsioThread::p main_thread();
AsioThread::p read_thread();
AsioThread::p working_task_read_write_thread();
framework::timer::TimerQueue & global_second_timer();
void reset_global_second_timer();

/**************************************************
*	class PushServer
**************************************************/

class PushServer
	: public boost::noncopyable
	, public boost::enable_shared_from_this<PushServer>
    , public protocol::IUdpServerListener
{
public:
	typedef boost::shared_ptr<PushServer> p;

	/***
	 *  @brief: 保证全局唯一PushServer实例的接口
	 */
	static PushServer::p Inst() 
	{
		static PushServer::p m_inst;
		static boost::mutex m_inst_mutex;

		if (!m_inst)
		{
			boost::mutex::scoped_lock inst_lock(m_inst_mutex);
			if (!m_inst)
			{
				m_inst = PushServer::p(new PushServer);
			}
		}
		return m_inst;
	}

public:
	/***
	 *  @brief: 加载配置文件，
	 *  @param <<config_file : 配置文件名
	 */
	void Start(const string& config_file);

	/***
	 *  @brief: 关闭PushServer
	 */
	void Stop();

	/***
	 *  @brief: 控制台显示服务器状态
	 */
	void ShowConfig();

	/***
	 *  @brief: 控制台显示报文收发速度
	 */
	void ShowPaktStat();

	/***
	 *  @brief: UDP收包触发函数
	 */
    virtual void OnUdpRecv(protocol::Packet const & packet);

	/***
	 *  @brief: 发包
	 */
    template <typename PacketType>
    void DoSendPacket(PacketType const & packet, boost::uint16_t dest_protocol_version);

	/***
	 *  @brief: 加载配置文件
	 *   DloadCenter的配置信息
	 */
	void LoadConfig();

	void ShowListInfo();

	void UpTaskList();

	void ExportAllInfo(const string& filename);

private:
	void UpdateConfig();

    void ReportVideoPlayCountToStatisticServer(const std::vector<std::string>& video_name_vec);

    /*void OnQueryPushTask(protocol::QueryPushTaskPacket const & packet);*/
    void OnReportPushTaskComplete(protocol::ReportPushTaskCompletedPacket const & packet);
    void OnQueryPushTaskV2(protocol::QueryPushTaskPacketV2 const & packet);
    void OnQueryPushTaskV3(protocol::QueryPushTaskPacketV3 const & packet);

    void HandleSend(string * to_del_str, const boost::system::error_code& err, std::size_t sz)
    {
        if (to_del_str != NULL) {
            delete to_del_str;
            to_del_str = NULL;
    
        }
    }

    void OnTimerElapsed(framework::timer::Timer * timer);
    void SavePushStatisticsData();

private:
	/* ---------- 配置信息 ---------- */
	string m_cfg_filename;					// 配置文件名
	PushSrvCfg m_config;
	ReaderCfg m_reader_cfg;
    ContentManagerCfg m_content_cfg;

	/* ---------- 报文统计 ---------- */
	PacketStat m_recv_stat;
	PacketStat m_recv_err_stat;
	PacketStat m_send_v1_stat;
	PacketStat m_send_v1_null_stat;
    PacketStat m_send_v2_stat;
    PacketStat m_send_v2_null_stat;
    PacketStat m_send_rid_stat;

	//TaskManager::p m_task_manager;
    ContentManager::p m_content_manager;

private:
	PushServer();

    boost::shared_ptr<protocol::UdpServer> m_udp_server;
    
    typedef boost::shared_ptr<boost::asio::ip::udp::socket> udp_socket_ptr;
    udp_socket_ptr m_udp_client;

	bool m_is_running;

    framework::timer::PeriodicTimer m_statistics_timer_;
    PushStatistics m_push_statistics_;
};

PUSH_SERVER_NAMESPACE_END

#endif // __PUSH_SERVER_H__