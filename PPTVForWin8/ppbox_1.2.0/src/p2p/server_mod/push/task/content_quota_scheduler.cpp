/* ======================================================================
 *	content_quota_scheduler.cpp
 *	Copyright (c) 2011 Synacast. All rights reserved.
 *
 *	push server新热片管理
 */


#include "stdafx.h"
#include "push_server.h"
#include "push_struct.h"
#include "content_quota_scheduler.h"
#include "content_matcher.h"
#include "key_generator.h"
#include "tinyxml/tinyxml.h"
#include "URLEncode.h"
#include "type_compatible.h"
#include <push_util/util.h>

#include <boost/date_time.hpp>
#include <boost/date_time/date.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "boost/date_time/posix_time/posix_time.hpp" 
#include <stdio.h>

extern log4cplus::Logger g_logger;
extern log4cplus::Logger g_statlog;

PUSH_SERVER_NAMESPACE_BEGIN

FRAMEWORK_LOGGER_DECLARE_MODULE("scheduler");

ContentQuotaScheduler::ContentQuotaScheduler() : max_daily_rid_num(0), max_minute_rid_num(0),
                            pause_time_start(0), pause_time_end(0),
                            daily_count(0), minute_count(0),
                            last_minute(0), last_day(0),
                            max_active_rid_num(0),
                            active_minute_count(0), active_daily_total(0),
                            vip_minute_total(0), vip_daily_total(0)
{

}

void ContentQuotaScheduler::ReloadConfig(ContentManagerCfg *m_inconfig, bool is_started)
{
    max_daily_rid_num = m_inconfig->CONTENT_RIDS_DAILY_NUM;
    max_minute_rid_num = m_inconfig->CONTENT_RIDS_MINUTE_NUM;
    pause_time_start = m_inconfig->CONTENT_PAUSE_START_TIME;
    pause_time_end = m_inconfig->CONTENT_PAUSE_END_TIME;
    if (true == is_started) {
        LOG4CPLUS_INFO(g_logger, "Scheduler: config reloaded, daily_cap "<< max_daily_rid_num
            << " minute_cap " << max_minute_rid_num << " pause_start " << pause_time_start << " end " << pause_time_end);

    }else{
        CONSOLE_LOG("Scheduler: config loaded, daily_cap "<< max_daily_rid_num
            << " minute_cap " << max_minute_rid_num << " pause_start " << pause_time_start << " end " << pause_time_end);
    }
}

void ContentQuotaScheduler::Reset(ContentManagerCfg *m_inconfig)
{
    ReloadConfig(m_inconfig);
    daily_count = max_daily_rid_num;
    minute_count = max_minute_rid_num;
    unsigned int time_now = ContentManager::get_epoch_time();
    last_minute = time_now;
    last_day = time_now;
    active_daily_total = 0;
    active_minute_count = 0;
    vip_minute_total = 0;
    vip_daily_total = 0;
    LOG4CPLUS_INFO(g_logger, "Scheduler: reseted.");
}

void ContentQuotaScheduler::PerMinuteTimerProcess()
{
    //we will try to process time.
    uint32_t p_time = ContentManager::get_epoch_time();
    LOG4CPLUS_DEBUG(g_logger, "ENTER:PerMinuteTimerProcess--minute_count:" << minute_count
        << " max_minute_rid_num:" << max_minute_rid_num
        << " active_minute_count:" << max_active_rid_num
        << " p_time:" << p_time << " last_minute:" << last_minute
        << " daily_count:" << daily_count
        << " max_daily_rid_num:" << max_daily_rid_num
        << " active_daily_total:" << active_daily_total
        << " vip_minute_total :" << vip_minute_total
        << " vip_daily_total :" << vip_daily_total
        << " last_day:" << last_day);

    if ( (p_time - last_minute) > 50 ) {
        LOG4CPLUS_INFO(g_statlog, "mp = "<< (max_minute_rid_num - minute_count) << " ma = "<< (max_active_rid_num - active_minute_count) << " mv = " << vip_minute_total);
        minute_count = max_minute_rid_num;
        active_minute_count = max_active_rid_num;
        vip_minute_total = 0;
        last_minute = p_time;
    }
    if ((p_time - last_day) > (3600 * 24-100)) {
        LOG4CPLUS_INFO(g_statlog, "dp = "<< (max_daily_rid_num - daily_count) << " da = "<< active_daily_total << " dv = " << vip_daily_total);
        daily_count = max_daily_rid_num;
        active_daily_total = 0;
        vip_daily_total = 0;
        last_day = p_time;
    }
    LOG4CPLUS_DEBUG(g_logger, "EXIT:PerMinuteTimerProcess--minute_count:" << minute_count
        << " max_minute_rid_num:" << max_minute_rid_num
        << " active_minute_count:" << max_active_rid_num
        << " p_time:" << p_time << " last_minute:" << last_minute
        << " daily_count:" << daily_count
        << " max_daily_rid_num:" << max_daily_rid_num
        << " active_daily_total:" << active_daily_total
        << " vip_minute_total :" << vip_minute_total
        << " vip_daily_total :" << vip_daily_total
        << " last_day:" << last_day);
}

void ContentQuotaScheduler::QuotaRestore(const int target)
{
    LOG4CPLUS_DEBUG(g_logger, "ENTER: QuotaRestore--daily_count:" << daily_count
        << " minute_count:" << minute_count << " target:" << target);
    if (target > 0) {
        daily_count += target;
        minute_count += target;
    }
    LOG4CPLUS_DEBUG(g_logger, "EXIT: QuotaRestore--daily_count:" << daily_count
        << " minute_count:" << minute_count << " target:" << target);

}

uint32_t ContentQuotaScheduler::GetDailyNum()
{
    LOG4CPLUS_DEBUG(g_logger, "GetDailyNum--daily_count:" << daily_count);
    return daily_count;
}

uint32_t ContentQuotaScheduler::GetDailyTimePoint()
{
    LOG4CPLUS_DEBUG(g_logger, "GetDailyTimePoint--last_day:" << last_day);
    return last_day;
}

void ContentQuotaScheduler::SetLastPoint(uint32_t p_dailytime, uint32_t p_daily_count)
{
    if (p_daily_count > max_daily_rid_num) p_daily_count = max_daily_rid_num;
    if (p_daily_count < 0) p_daily_count = max_daily_rid_num;
    daily_count = p_daily_count;
    last_day = p_dailytime;
    LOG4CPLUS_INFO(g_logger, "Restore Data Point, timestamp " << last_day << " cap " << daily_count << " Local time is " << ContentManager::get_epoch_time());
    //    LOG4CPLUS_INFO(g_logger, "Local time is " << ContentManager::get_epoch_time());
}

bool ContentQuotaScheduler::HasQuota(const int target)
{
    LOG4CPLUS_DEBUG(g_logger, "HasQuota--daily_count: " << daily_count << " minite_count: " << minute_count << " target: " << target);
    return ((daily_count >= target) && (minute_count >= target));
}

bool ContentQuotaScheduler::HasActiveQuota(const int target)
{
    LOG4CPLUS_DEBUG(g_logger, "HasActiveQuota--active_minute_count: " << active_minute_count << " target: " << target);
    return (active_minute_count >= target);
}

int ContentQuotaScheduler::QuerySchedulerForActiveQuota(const int target, int quota_type)
{
    LOG4CPLUS_DEBUG(g_logger, "ENTER: QuerySchedulerForActiveQuota--active_minute_count: " << active_minute_count << " target: " << target);
    if (active_minute_count >= target) {
        active_minute_count -= target;
        active_daily_total += target;
        if (quota_type == 1) {
            vip_daily_total += target;
            vip_minute_total += target;
        }
        LOG4CPLUS_DEBUG(g_logger, "EXIT1: QuerySchedulerForActiveQuota--active_minute_count: " << active_minute_count << " target: " << target);
        return target;
    }
    LOG4CPLUS_DEBUG(g_logger, "EXIT2: QuerySchedulerForActiveQuota--active_minute_count: " << active_minute_count << " target: " << target);
    return 0;
}

void ContentQuotaScheduler::SetActiveMaxCap(const uint32_t p_maxcap)
{
    LOG4CPLUS_DEBUG(g_logger, "ENTER: SetActiveMaxCap--max_active_rid_num: " << max_active_rid_num << " p_maxcap:" << p_maxcap 
        << "active_minute_count: " << active_minute_count );
    max_active_rid_num = p_maxcap;
    active_minute_count = max_active_rid_num;
    LOG4CPLUS_DEBUG(g_logger, "EXIT: SetActiveMaxCap--max_active_rid_num: " << max_active_rid_num << " p_maxcap:" << p_maxcap 
        << "active_minute_count: " << active_minute_count );
}

int ContentQuotaScheduler::QuerySchedulerForQuota(const int target)
{
    //we first check whether we are in pause time frame.
    LOG4CPLUS_DEBUG(g_logger, "ENTER: QuerySchedulerForQuota--daily_count: " << daily_count 
        << "minute_count: " << minute_count << " target: " << target);
    if ( (daily_count >= target) && (minute_count >= target) ) {
        daily_count -= target;
        minute_count -= target;
        LOG4CPLUS_DEBUG(g_logger, "EXIT1: QuerySchedulerForQuota--daily_count: " << daily_count 
            << "minute_count: " << minute_count << " target: " << target);
        return target;
    }
    LOG4CPLUS_DEBUG(g_logger, "EXIT2: QuerySchedulerForQuota--daily_count: " << daily_count 
        << "minute_count: " << minute_count << " target: " << target);
    return 0;
}

bool ContentQuotaScheduler::IsBusyTime()
{
    //we first check whether we are in pause time frame.
    uint32_t time_now = ContentManager::get_epoch_time();
    time_now = time_now % (24*3600);

    //there are two cases
    // 1) pause_time_start < pause_time_end, this is the normal case.
    // 2) pause_time_start > pause_time_end, this means we have wrapp around, 0-time_end, time_end-24*3600
    if ( pause_time_start < pause_time_end) {
        if ( (time_now > pause_time_start) && (time_now < pause_time_end) ) {
            LOG4CPLUS_DEBUG(g_logger, "IN Pause Period. Request ignored.");
            return true;
        }
    }else if ( pause_time_start > pause_time_end) {
        if ( (time_now < pause_time_end) || (time_now > pause_time_start) ) {
            LOG4CPLUS_DEBUG(g_logger, "IN 'Wrap' Pause Period. Request ignored.");
            return true;
        }
    }
    return false;
}

PUSH_SERVER_NAMESPACE_END