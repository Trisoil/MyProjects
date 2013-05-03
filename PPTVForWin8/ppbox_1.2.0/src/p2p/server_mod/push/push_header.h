/* ======================================================================
 *	push_header.h
 *	Copyright (c) 2009 Synacast. All rights reserved.
 *
 *	push server工程相关头文件
 * ======================================================================
 *	  Time         Changer     ChangeLog
 *	~~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 *	2009-06-02     cnhbdu      创建
 */

#ifndef __PUSH_HEADER_H__
#define __PUSH_HEADER_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

/* ---------- 关闭MSVC无用的警告 ---------- */
#ifdef WIN32
#pragma warning(disable:4819)
#pragma warning(disable:4996)
#pragma warning(disable:4355)
#define _CRT_SECURE_NO_WARNINGS
#include <Winbase.h>
#endif // WIN32

#define _PS_LOG(message) do{ \
	std::ostringstream oss; \
	oss << message; \
	LOG_S(framework::logger::Logger::kLevelEvent, oss.str()); \
	} while(false)

//#define PS_LOG(message) _PS_LOG("["__FUNCTION__<<" ("<<__LINE__<<")] "<<message)
#define PS_LOG(message)

#ifdef _DEBUG
#define PS_DEBUG_LOG(message) PS_LOG(message)
#else
#define PS_DEBUG_LOG(message)
#endif // _DEBUG

#define CONSOLE_LOG(message) std::cout << message << std::endl;

#define PUSH_SERVER_NAMESPACE_BEGIN namespace push_server {
#define PUSH_SERVER_NAMESPACE_END }

#endif // __PUSH_HEADER_H__