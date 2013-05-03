// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

// 如果必须将位于下面指定平台之前的平台作为目标，请修改下列定义。
// 有关不同平台对应值的最新信息，请参考 MSDN。
#ifndef WINVER                // 允许使用特定于 Windows XP 或更高版本的功能。
#define WINVER 0x0500        // 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINNT        // 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0500    // 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif                        

#ifndef _WIN32_WINDOWS        // 允许使用特定于 Windows 98 或更高版本的功能。
#define _WIN32_WINDOWS 0x0410 // 将此值更改为适当的值，以指定将 Windows Me 或更高版本作为目标。
#endif

#ifndef _WIN32_IE            // 允许使用特定于 IE 6.0 或更高版本的功能。
#define _WIN32_IE 0x0600    // 将此值更改为相应的值，以适用于 IE 的其他版本。
#endif

#define WIN32_LEAN_AND_MEAN        // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#ifdef BOOST_WINDOWS_API
#include <windows.h>
#endif

#ifndef NEED_LOG
#define NEED_LOG
#endif

// TODO: 在此处引用程序需要的其他头文件
#include <string>
#include "boost/cstdint.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/enable_shared_from_this.hpp"
#include "boost/shared_array.hpp"
#include "protocol/Protocol.h"
#include "boost/bind.hpp"

#include <iomanip>

using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::int32_t;
using std::string;

#include <framework/Framework.h>
#include <framework/string/Uuid.h>
#include <framework/string/Format.h>
#include <framework/string/FormatStl.h>
#include <framework/string/Parse.h>
#include <framework/string/ParseStl.h>
#include <framework/timer/TickCounter.h>
#include <framework/timer/Timer.h>
#include <framework/timer/TimerQueue.h>
#include <framework/configure/Config.h>
#include <framework/logger/Logger.h>
#include <framework/logger/LogMsgStream.h>



#if (defined _DEBUG || defined DEBUG)
// #  define LOG(level, type, msg) LOG_S(level, msg)
// #  define LOGX(level, type, msg) LOG_S(level, __FILE__":"TOSTRING(__LINE__)" " << msg)
#  define LOG(level, type, msg)
#  define LOGX(level, type, msg)
#else
#  define LOG(level, type, msg)
#  define LOGX(level, type, msg)
#endif

#include <iostream>

framework::timer::TimerQueue & global_second_timer();

#undef max

#include "Logger.h"
