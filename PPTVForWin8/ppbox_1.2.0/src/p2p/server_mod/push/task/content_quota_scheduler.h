/* ======================================================================
 *	content_quota_scheduler.h
 *	Copyright (c) 2009 Synacast. All rights reserved.
 *
 *	push server新热片管理
 */

#ifndef __CONTENT_QUOTA_SCHEDULER_H__
#define __CONTENT_QUOTA_SCHEDULER_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "push_header.h"
#include "push_struct.h"

#include <list>
#include <string>
#include <vector>
#include <map>

PUSH_SERVER_NAMESPACE_BEGIN

class ContentQuotaScheduler
{
public:
    ContentQuotaScheduler();
    void ReloadConfig(ContentManagerCfg *m_inconfig, bool is_started = true);
    void Reset(ContentManagerCfg *m_inconfig);
    void PerMinuteTimerProcess();
    bool IsBusyTime();
    bool HasQuota(const int target);
    void QuotaRestore(const int target);
    int QuerySchedulerForQuota(const int target);
    uint32_t GetDailyNum();
    uint32_t GetDailyTimePoint();
    void SetLastPoint(uint32_t p_dailytime, uint32_t p_daily_count);
    void SetActiveMaxCap(const uint32_t p_activemax);
    bool HasActiveQuota(const int target);
    int QuerySchedulerForActiveQuota(const int target, const int quota_type = 0);

private:
    uint32_t max_daily_rid_num;
    uint32_t max_minute_rid_num;
    uint32_t pause_time_start;
    uint32_t pause_time_end;
    uint32_t daily_count;
    uint32_t minute_count;
    uint32_t last_minute;
    uint32_t last_day;
    uint32_t max_active_rid_num;
    uint32_t active_minute_count;
    uint32_t active_daily_total;
    uint32_t vip_minute_total;
    uint32_t vip_daily_total;
};


PUSH_SERVER_NAMESPACE_END

#endif // __CONTENT_QUOTA_SCHEDULER_H__
