/******************************************************************************
*
* Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
* 
* Util.h
* 
* Description: 服务器间公用的实用函数
*             
* 
* --------------------
* 2011-03-37, Create by youngkyyang
* --------------------
********************************************************************************/

#ifndef __PPLIVE_UTILITY_UTIL_H__
#define __PPLIVE_UTILITY_UTIL_H__

namespace ns_pplive
{
	namespace utility
	{
        //获取当前时间，微妙级
		double GetExactTimeNow();
		void BindCPU(int cpu_id);
	}
}
#endif //__PPLIVE_UTILITY_UTIL_H__