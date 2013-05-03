/* ======================================================================
 *	PacketStat.h
 *	Copyright (c) 2009 Synacast. All rights reserved.
 *
 *	����ͳ��
 * ======================================================================
 *	  Time         Changer     ChangeLog
 *	~~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 *	2009-05-07     cnhbdu      ����
 */

#ifndef __PACKET_STAT_H__
#define __PACKET_STAT_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <list>
#include <ctime>
#include <boost/cstdint.hpp>

class PacketStat
{
public:
	struct StatData
	{
		float minute_sp;
		float hour_sp;
		boost::uint32_t total_num;
	};

public:
	PacketStat() : m_last_time(clock()) { m_data_list.push_back(0); }

	void Add(uint32_t p_cnt = 1)
	{
		clock_t now_time = clock();
		if ((now_time - m_last_time)/CLOCKS_PER_SEC >= 60)
		{
			m_data_list.push_back(p_cnt);
			m_last_time = now_time;
			if (m_data_list.size() > 30)
			{
				m_data_list.pop_front();
			}
		}
		else
		{
			m_data_list.back() += p_cnt;
		}
	}

	StatData GetStat()
	{
		StatData stat_data;
		clock_t now_time = clock();
		boost::uint32_t last_sec = (now_time - m_last_time) / CLOCKS_PER_SEC;
		stat_data.minute_sp = float(m_data_list.back()) / last_sec;
		boost::uint32_t total_sec = (m_data_list.size() - 1) * 60 + last_sec;
		boost::uint32_t total_num = 0;
		for (std::list<boost::uint32_t>::iterator it = m_data_list.begin();
			it != m_data_list.end(); ++it)
		{
			total_num += *it;
		}
		stat_data.hour_sp = float(total_num) / total_sec;
		stat_data.total_num = total_num;
		return stat_data;
	}

private:
	clock_t m_last_time;
	std::list<boost::uint32_t> m_data_list;
};

struct PushStatistics 
{
    unsigned int total_recv_;
    unsigned int request_per_sec_;
    unsigned int total_push_count_;
    unsigned int push_count_per_sec_;
    unsigned int total_no_task_count_;
    unsigned int no_task_count_per_sec_;
    unsigned int total_passive_push_;
    unsigned int passive_push_per_sec_;
    unsigned int total_active_push_;
    unsigned int active_push_per_sec_;

    PushStatistics(): total_recv_(0), request_per_sec_(0), total_push_count_(0), push_count_per_sec_(0), 
        total_no_task_count_(0), no_task_count_per_sec_(0), total_passive_push_(0), passive_push_per_sec_(0),
        total_active_push_(0), active_push_per_sec_(0)
    {
    }

    ~PushStatistics() {}

    //调用的时候自己会先初始化为0
    void AddIpTimes(const std::string &ip) { ++query_ip_times_[ip];}
    std::pair<std::string, int> GetMaxIp()
    {
        std::pair<std::string, int> max_pair;
        for (std::map<std::string, int>::iterator it = query_ip_times_.begin(); it != query_ip_times_.end(); ++it)
        {
            if (max_pair.second < it->second)
            {
                max_pair = *it;
            }
        }

        return max_pair;
    }

    void ClearStat()
    {
        query_ip_times_.clear();
        push_count_per_sec_ = 0;
        no_task_count_per_sec_ = 0;
        request_per_sec_ = 0;
        passive_push_per_sec_ = 0;
        active_push_per_sec_ = 0;
    }

private:
    //统计查询请求每个ip出现的次数，目的是计算一定时间内最大查询次数的ip
    std::map<std::string, int> query_ip_times_;
};
#endif // __PACKET_STAT_H__