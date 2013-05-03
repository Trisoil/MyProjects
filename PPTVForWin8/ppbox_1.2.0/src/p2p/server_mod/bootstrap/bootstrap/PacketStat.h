/* ======================================================================
 *    PacketStat.h
 *    Copyright (c) 2009 Synacast. All rights reserved.
 *
 *    报文统计
 * ======================================================================
 *      Time         Changer     ChangeLog
 *    ~~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 *    2009-05-07     cnhbdu      创建
 */

#ifndef __PACKET_STAT_H__
#define __PACKET_STAT_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "BHeader.h"

#include <list>
#include <ctime>
#include <boost/cstdint.hpp>

BOOTSTRAP_NAMESPACE_BEGIN

class PacketStat
{
public:
    struct StatData
    {
        float minute_sp;
        float hour_sp;
    };

public:
    PacketStat() : m_last_time(clock()) { m_data_list.push_back(0); }

    void Add()
    {
        clock_t now_time = clock();
        if ((now_time - m_last_time)/CLOCKS_PER_SEC >= 60)
        {
            m_data_list.push_back(1);
            m_last_time = now_time;
            if (m_data_list.size() > 30)
            {
                m_data_list.pop_front();
            }
        }
        else
        {
            ++m_data_list.back();
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
        for (list<boost::uint32_t>::iterator it = m_data_list.begin(); it != m_data_list.end(); ++it)
        {
            total_num += *it;
        }
        stat_data.hour_sp = float(total_num) / total_sec;
        return stat_data;
    }

private:
    clock_t m_last_time;
    std::list<boost::uint32_t> m_data_list;
};

BOOTSTRAP_NAMESPACE_END

#endif // __PACKET_STAT_H__