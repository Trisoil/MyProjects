/* ======================================================================
 *	task_struct.h
 *	Copyright (c) 2009 Synacast. All rights reserved.
 *
 *	push server 主要数据结构和配置项
 * ======================================================================
 *	  Time         Changer     ChangeLog
 *	~~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 *	2009-06-02     cnhbdu      创建
 */

#ifndef __TASK_STRUCT_H__
#define __TASK_STRUCT_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "push_header.h"

#include <ctime>

PUSH_SERVER_NAMESPACE_BEGIN

enum PushErrCode
{
	PUSH_HAS_TASK = 0x00,
    PUSH_HAS_ACT_TASK = 0x01,
	PUSH_NO_TASK = 0x02
};

struct PeerInfo
{
    framework::Guid guid;
    uint32_t used_disk_size;
    uint32_t upload_bandwidth;
    uint32_t avg_upload_speed_kbs;
    uint32_t total_disk_size;
    uint32_t nat_type;
    uint32_t online_percent;

    PeerInfo(framework::Guid guid_, uint32_t used_disk_size_, uint32_t upload_bandwidth_, 
        uint32_t avg_upload_speed_kbs_, uint32_t total_disk_size_, uint32_t nat_type_, uint32_t online_percent_
        ): guid(guid_), used_disk_size(used_disk_size_), 
           upload_bandwidth(upload_bandwidth_), avg_upload_speed_kbs(avg_upload_speed_kbs_),
           total_disk_size(total_disk_size_), nat_type(nat_type_), online_percent(online_percent_)
    {}
};

enum PushTypeFlag
{
	PUSHTYPE_GENERIC = protocol::TASK_GENERIC,
	PUSHTYPE_OPEN_SERVICE = protocol::TASK_OPEN_SERVICE,
};

struct TaskInfo
{
	protocol::RidInfo rid_info;
	protocol::UrlInfo url_info;

	vector<string> urls;
	string refer;
	uint32_t url_index;

	PushTypeFlag type_flag;

	uint16_t push_interval;

	uint16_t max_idle_speed_kb;
	uint16_t max_normal_speed_kb;
	uint16_t min_speed_kb;

	float idle_speed_bw_ratio;
	float normal_speed_bw_ratio;

	TaskInfo() : type_flag(PUSHTYPE_GENERIC), url_index(0) {}

	protocol::PUSH_TASK_PARAM TaskParam()
	{
		protocol::PUSH_TASK_PARAM param;
		param.TaskType = type_flag;

		param.ProtectTimeIntervalInSeconds = push_interval;

		param.MaxDownloadSpeedInKBpsWhenIdle = max_idle_speed_kb;
		param.MaxDownloadSpeedInKBpsWhenNormal = max_normal_speed_kb;
		param.MinDownloadSpeedInKBps = min_speed_kb;

		param.BandwidthRatioWhenIdle = static_cast<uint8_t>(idle_speed_bw_ratio * 255);
		param.BandwidthRatioWhenNormal = static_cast<uint8_t>(normal_speed_bw_ratio * 255);

		return param;
	}
};

struct Guid_hash : std::unary_function<Guid, std::size_t>
{
	std::size_t operator()(const Guid& guid) const
	{
		return boost::hash_value(guid);
	}
};

struct Guid_equal_to : std::binary_function<Guid, Guid, bool>
{
	bool operator()(const Guid& _Left, const Guid& _Right) const
	{
		return 0 == memcmp((void*)&_Left, (void*)&_Right, sizeof(Guid));
	}
};

// --------------------------------------------------
// 配置项

const uint16_t DEFAULT_MAX_IDLE_SPEED_KB = 400;
const uint16_t DEFAULT_MAX_NORMAL_SPEED_KB = 50;
const uint16_t DEFAULT_MIN_SPEED_KB = 5;
const uint16_t DEFAULT_PUSH_INTERVAL_SEC = 1800;
const float DEFAULT_IDLE_SPEED_BW_RATIO = 0.75;
const float DEFAULT_NORMAL_SPEED_BW_RATIO = 0.15;
const uint32_t DEFAULT_UPDATE_TASK_TIME = 600;
const uint32_t DEFAULT_GEN_KEY_TIMER = 60;
const bool DEFAULT_IS_GEN_KEY_EVERYTIME = false;

struct ReaderCfg
{
    uint16_t MAX_IDLE_SPEED_KB;     // 空闲时最大的下载速度
    uint16_t MAX_NORMAL_SPEED_KB;   // 平时最大下载速度
    uint16_t MIN_SPEED_KB;          // 最小的下载速度
    uint16_t PUSH_INTERVAL_SEC;     // peer push时间间隔(秒)
    float IDLE_SPEED_BW_RATIO;      // 空闲时最大下载速度占带宽百分比
    float NORMAL_SPEED_BW_RATIO;    // 非空闲时最大下载速度占带宽百分比
    uint32_t UPDATE_TASK_TIME;      // 更新任务列表的定时间隔
    uint32_t GEN_KEY_TIMER;         // 生成url key的时间间隔
    bool IS_GEN_KEY_EVERYTIME;      // 是否每次生成key

    ReaderCfg()
    {
        MAX_IDLE_SPEED_KB = DEFAULT_MAX_IDLE_SPEED_KB;
        MAX_NORMAL_SPEED_KB = DEFAULT_MAX_NORMAL_SPEED_KB;
        MIN_SPEED_KB = DEFAULT_MIN_SPEED_KB;
        PUSH_INTERVAL_SEC = DEFAULT_PUSH_INTERVAL_SEC;
        IDLE_SPEED_BW_RATIO = DEFAULT_IDLE_SPEED_BW_RATIO;
        NORMAL_SPEED_BW_RATIO = DEFAULT_NORMAL_SPEED_BW_RATIO;
        UPDATE_TASK_TIME = DEFAULT_UPDATE_TASK_TIME;
        GEN_KEY_TIMER = DEFAULT_GEN_KEY_TIMER;
        IS_GEN_KEY_EVERYTIME = DEFAULT_IS_GEN_KEY_EVERYTIME;
    }
};

const uint16_t DEFAULT_CONTENT_PUSH_DISABLED = 0;
const uint16_t DEFAULT_CONTENT_PUSHAHEAD_NUM = 2;
const uint16_t DEFAULT_CONTENT_HISTORY_DAYS = 10;
const string DEFAULT_CONTENT_HOTSAVE_FILENAME = "pushhot.clist.config";
const string DEFAULT_CONTENT_VIP_RID_FILENAME = "pushhot.viprid.config";
const uint32_t DEFAULT_CONTENT_HOT_RETIRE_TIME = 30*24*3600;
const uint32_t DEFAULT_CONTENT_EPI_RETIRE_TIME = 3*24*3600;
const string DEFAULT_CONTENT_NEWHOT_XML_FILENAME = "newhot_content_list.xml";
const string DEFAULT_CONTENT_UPLOAD_XML_FILENAME = "newupload_content_list.xml";
const string DEFAULT_CONTENT_NEWVIP_RID_XML_FILENAME = "newvip_rid_list.xml";
const string DEFAULT_CONTENT_NEWRID_MAKEUP_XML_FILENAME = "newhot_rid_makeup.xml";
const string DEFAULT_CONTENT_PUSH_BASE_URL = "http://dl.jump.synacast.com/";
const string DEFAULT_CONTENT_PUSH_BASE_APPENDIX = "&type=OffDown&rtype=Redirect";
const uint32_t DEFAULT_CONTENT_RIDS_DAILY_NUM = 10000000;
const uint32_t DEFAULT_CONTENT_RIDS_MINUTE_NUM = DEFAULT_CONTENT_RIDS_DAILY_NUM/330;  // (11*60/2).
const uint32_t DEFAULT_CONTENT_PAUSE_START_TIME = 19*3600;
const uint32_t DEFAULT_CONTENT_PAUSE_END_TIME = 1800;
const uint32_t DEFAULT_CONTENT_PUSH_THRESH_RNUM = 2;
const uint32_t DEFAULT_CONTENT_PUSH_THRESH_DURATION = 1200;
const string DEFAULT_CONTENT_PUSH_LOG_CONFNAME = "push_logging.conf";
const uint32_t DEFAULT_CONTENT_HOTSAVE_RELOAD_TIME = 30;   //Interval to reload the hot/new content files. (in minutes).
const string DEFAULT_CONTENT_SESSION_SAVE_FILE = "push_session.conf";
const uint32_t DEFAULT_CONTENT_ACTPUSH_ENABLED = 1;
const uint32_t DEFAULT_CONTENT_VIPPUSH_ENABLED = 1;
const uint32_t DEFAULT_CONTENT_VIPPUSH_RETIRE_THRESHOLD = 2000;
const uint32_t DEFAULT_CONTENT_ACTPUSH_PER_RID_LIMIT = 500;
const uint32_t DEFAULT_CONTENT_ACTPUSH_CONTROL_MINUTE = 60;
const uint32_t DEFAULT_CONTENT_ACTPUSH_BIG_BANDWIDTH = 80;
const uint32_t DEFAULT_CONTENT_ACTPUSH_BIG_BW_RATIO = 80;
const uint32_t DEFAULT_CONTENT_ACTPUSH_PER_REQ_LIMIT = 1;
const uint32_t DEFAULT_CONTENT_NEWVIP_RID_TARGET = 200;
const uint32_t DEFAULT_CONTENT_PAUSE_DISABLED = 0;
const uint32_t DEFAULT_CONTENT_ACTPUSH_MIN_MIN_LIMIT = 60;
const uint16_t DEFAULT_CONTENT_PUSH_BITRATE_TYPE = 0;
const uint32_t DEFAULT_CONTENT_ACTPUSH_BIG_FREE_DISK = 100;
const uint32_t DEFAULT_CONTENT_ACTPUSH_BIG_DISK_RATIO = 80;
const uint32_t DEFAULT_CONTENT_PASPUSH_PER_RID_LIMIT = 350;
const uint32_t DEFAULT_CONTENT_ACTPUSH_HIGH_ONLINE_TIME = 10;
const uint32_t DEFAULT_CONTENT_ACTPUSH_HIGH_ONLINE_RATIO = 350;
const string DEFAULT_CONTENT_ACTPUSH_GOOD_NAT_STRING = "0,1,2,4";

struct ContentManagerCfg
{
    uint16_t CONTENT_PUSH_DISABLED;
    uint16_t CONTENT_PUSHAHEAD_NUM;
    uint16_t CONTENT_HISTORY_DAYS;
    string   CONTENT_HOTSAVE_FILENAME;
    string   CONTENT_VIP_RID_FILENAME;
    uint32_t CONTENT_HOT_RETIRE_TIME;
    uint32_t CONTENT_HOT_EP_RETIRE_TIME;
    string   CONTENT_NEWHOT_XML_FILENAME;
    string   CONTENT_NEWUPLOAD_XML_FILENAME;
    string   CONTENT_NEWVIP_RID_XML_FILENAME;
    string   CONTENT_NEWRID_MAKEUP_XML_FILENAME;
    uint32_t CONTENT_RIDS_DAILY_NUM;
    uint32_t CONTENT_RIDS_MINUTE_NUM;
    uint32_t CONTENT_NEWVIP_RID_TARGET;
    uint32_t CONTENT_PAUSE_START_TIME;
    uint32_t CONTENT_PAUSE_END_TIME;
    string   CONTENT_PUSH_BASE_URL;
    string   CONTENT_PUSH_BASE_APPENDIX;
    uint32_t CONTENT_PUSH_THRESH_RNUM;
    uint32_t CONTENT_PUSH_THRESH_DURATION;
    string   CONTENT_PUSH_LOG_CONFNAME;
    uint32_t CONTENT_HOTSAVE_RELOAD_TIME;
    string   CONTENT_SESSION_SAVE_FILE;
    uint32_t CONTENT_ACTPUSH_ENABLED;
    uint32_t CONTENT_VIPPUSH_ENABLED;
    uint32_t CONTENT_VIPPUSH_RETIRE_THRESHOLD;
    uint32_t CONTENT_ACTPUSH_PER_RID_LIMIT;
    uint32_t CONTENT_ACTPUSH_CONTROL_MINUTE;
    uint32_t CONTENT_ACTPUSH_BIG_BANDWIDTH;
    uint32_t CONTENT_ACTPUSH_BIG_BW_RATIO;
    uint32_t CONTENT_ACTPUSH_PER_REQ_LIMIT;
    uint32_t CONTENT_PAUSE_DISABLED;
    uint32_t CONTENT_ACTPUSH_MIN_MIN_LIMIT;
    uint16_t CONTENT_PUSH_BITRATE_TYPE;
    uint32_t CONTENT_ACTPUSH_BIG_FREE_DISK;
    uint32_t CONTENT_ACTPUSH_BIG_DISK_RATIO;
    uint32_t CONTENT_PASPUSH_PER_RID_LIMIT;
    uint32_t CONTENT_ACTPUSH_HIGH_ONLINE_TIME;
    uint32_t CONTENT_ACTPUSH_HIGH_ONLINE_RATIO;
    string   CONTENT_ACTPUSH_GOOD_NAT_STRING;

    ContentManagerCfg()
    {
        CONTENT_PUSH_DISABLED = DEFAULT_CONTENT_PUSH_DISABLED;
        CONTENT_PUSHAHEAD_NUM = DEFAULT_CONTENT_PUSHAHEAD_NUM;
        CONTENT_HISTORY_DAYS = DEFAULT_CONTENT_HISTORY_DAYS;
        CONTENT_HOTSAVE_FILENAME = DEFAULT_CONTENT_HOTSAVE_FILENAME;
        CONTENT_VIP_RID_FILENAME = DEFAULT_CONTENT_VIP_RID_FILENAME;
        CONTENT_HOT_RETIRE_TIME = DEFAULT_CONTENT_HOT_RETIRE_TIME;
        CONTENT_HOT_EP_RETIRE_TIME = DEFAULT_CONTENT_EPI_RETIRE_TIME;
        CONTENT_NEWHOT_XML_FILENAME = DEFAULT_CONTENT_NEWHOT_XML_FILENAME;
        CONTENT_NEWUPLOAD_XML_FILENAME = DEFAULT_CONTENT_UPLOAD_XML_FILENAME;
        CONTENT_NEWVIP_RID_XML_FILENAME = DEFAULT_CONTENT_NEWVIP_RID_XML_FILENAME;
        CONTENT_NEWRID_MAKEUP_XML_FILENAME = DEFAULT_CONTENT_NEWRID_MAKEUP_XML_FILENAME;
        CONTENT_RIDS_DAILY_NUM = DEFAULT_CONTENT_RIDS_DAILY_NUM;
        CONTENT_RIDS_MINUTE_NUM = DEFAULT_CONTENT_RIDS_MINUTE_NUM;
        CONTENT_NEWVIP_RID_TARGET = DEFAULT_CONTENT_NEWVIP_RID_TARGET;
        CONTENT_PAUSE_START_TIME = DEFAULT_CONTENT_PAUSE_START_TIME;
        CONTENT_PAUSE_END_TIME = DEFAULT_CONTENT_PAUSE_END_TIME;
        CONTENT_PUSH_BASE_URL = DEFAULT_CONTENT_PUSH_BASE_URL;
        CONTENT_PUSH_BASE_APPENDIX = DEFAULT_CONTENT_PUSH_BASE_APPENDIX;
        CONTENT_PUSH_THRESH_RNUM = DEFAULT_CONTENT_PUSH_THRESH_RNUM;
        CONTENT_PUSH_THRESH_DURATION = DEFAULT_CONTENT_PUSH_THRESH_DURATION;
        CONTENT_PUSH_LOG_CONFNAME = DEFAULT_CONTENT_PUSH_LOG_CONFNAME;
        CONTENT_HOTSAVE_RELOAD_TIME = DEFAULT_CONTENT_HOTSAVE_RELOAD_TIME;
        CONTENT_SESSION_SAVE_FILE = DEFAULT_CONTENT_SESSION_SAVE_FILE;
        CONTENT_ACTPUSH_ENABLED = DEFAULT_CONTENT_ACTPUSH_ENABLED;
        CONTENT_VIPPUSH_ENABLED = DEFAULT_CONTENT_VIPPUSH_ENABLED;
        CONTENT_VIPPUSH_RETIRE_THRESHOLD = DEFAULT_CONTENT_VIPPUSH_RETIRE_THRESHOLD;
        CONTENT_ACTPUSH_PER_RID_LIMIT = DEFAULT_CONTENT_ACTPUSH_PER_RID_LIMIT;
        CONTENT_ACTPUSH_CONTROL_MINUTE = DEFAULT_CONTENT_ACTPUSH_CONTROL_MINUTE;
        CONTENT_ACTPUSH_BIG_BANDWIDTH = DEFAULT_CONTENT_ACTPUSH_BIG_BANDWIDTH;
        CONTENT_ACTPUSH_BIG_BW_RATIO = DEFAULT_CONTENT_ACTPUSH_BIG_BW_RATIO;
        CONTENT_ACTPUSH_PER_REQ_LIMIT = DEFAULT_CONTENT_ACTPUSH_PER_REQ_LIMIT;
        CONTENT_PAUSE_DISABLED = DEFAULT_CONTENT_PAUSE_DISABLED;
        CONTENT_ACTPUSH_MIN_MIN_LIMIT = DEFAULT_CONTENT_ACTPUSH_MIN_MIN_LIMIT;
        CONTENT_PUSH_BITRATE_TYPE = DEFAULT_CONTENT_PUSH_BITRATE_TYPE;
        CONTENT_ACTPUSH_BIG_FREE_DISK = DEFAULT_CONTENT_ACTPUSH_BIG_FREE_DISK;
        CONTENT_ACTPUSH_BIG_DISK_RATIO = DEFAULT_CONTENT_ACTPUSH_BIG_DISK_RATIO;
        CONTENT_PASPUSH_PER_RID_LIMIT = DEFAULT_CONTENT_PASPUSH_PER_RID_LIMIT;
        CONTENT_ACTPUSH_HIGH_ONLINE_TIME = DEFAULT_CONTENT_ACTPUSH_HIGH_ONLINE_TIME;
        CONTENT_ACTPUSH_HIGH_ONLINE_RATIO = DEFAULT_CONTENT_ACTPUSH_HIGH_ONLINE_RATIO;
        CONTENT_ACTPUSH_GOOD_NAT_STRING = DEFAULT_CONTENT_ACTPUSH_GOOD_NAT_STRING;
    }
};

const uint16_t DEFAULT_UDP_PORT = 6900;
const uint32_t DEFAULT_UDP_RECV_LEN = 100;
const uint32_t DEFAULT_PUSH_WAIT_TIME = 3600;

struct PushSrvCfg
{
	uint16_t UDP_PORT;			// push server UDP端口
	uint32_t UDP_RECV_LEN;		// push server UDP收包队列
    uint32_t PUSH_WAIT_TIME;    // 没有PUSH任务时的等待请求间隔，时间为s

    std::string HOT_STATISTIC_SERVER;  //热门电影统计服务器地址
    uint16_t HOT_STATISTIC_SERVER_PORT;//热门电影统计服务器端口
};

PUSH_SERVER_NAMESPACE_END

#endif // __TASK_STRUCT_H__
