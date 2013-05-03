/******************************************************************************
*
* Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
* 
* Util.cpp
* 
* Description: 服务器间公用的实用函数
*             
* 
* --------------------
* 2011-03-37, Create by youngkyyang
* --------------------
********************************************************************************/
#include "util.h"

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace ns_pplive
{
	namespace utility
	{
		double GetExactTimeNow()
		{
			double time_now = (boost::uint64_t)time(NULL);
			boost::posix_time::ptime t(boost::posix_time::microsec_clock::local_time());		
			boost::posix_time::time_duration td = t.time_of_day();
			time_now += td.total_microseconds() % 1000000 * 0.000001;
			return time_now;
		}

#ifdef _MSC_VER
		void BindCPU( int cpu_id )
		{
			return ;
		}
#else
		#include <stdlib.h>
		//#include <stdio.h>
		#include <sys/types.h>
		#include <sys/sysinfo.h>
		#include <unistd.h>
		#include <sys/syscall.h>
		#define gettid() syscall(__NR_gettid) 

		#define __USE_GNU
		#include <sched.h>
		#include <ctype.h>
		#include <string.h>

		void BindCPU(int cpu_id)
		{
			int num = sysconf(_SC_NPROCESSORS_CONF);
			if (cpu_id < 0)
			{
				return ;
			}

			cpu_id = cpu_id % num;

			cpu_set_t mask;
			CPU_ZERO(&mask);
			CPU_SET(cpu_id, &mask);
			if (sched_setaffinity(0, sizeof(mask), &mask) == -1)
			{
				return ;
			}
		}
#endif //_MSC_VER

	}
}