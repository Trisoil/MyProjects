//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef BOOTSTRAP_COMMON_H
#define BOOTSTRAP_COMMON_H

#ifdef BOOST_WINDOWS_API
#define NEED_TO_POST_MESSAGE
#endif

#ifndef _WIN32_WINNT        // 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0500    // 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif                        

//#if(_WIN32_WINNT == 0x0500)
//typedef unsigned __int64 u_int64;
//#endif //(_WIN32_WINNT == 0x0500)


#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/bind.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/function/function_base.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <framework/Framework.h>
#include <framework/string/Uuid.h>
#include <framework/string/Format.h>
#include <framework/string/Parse.h>
#include <framework/timer/TickCounter.h>
#include <framework/timer/Timer.h>
#include <framework/timer/TimerQueue.h>
#include <framework/configure/Config.h>

#include <util/Util.h>
#include <util/serialization/Uuid.h>

#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <set>

#include "Loggers.h"

#include <log4cplus/logger.h>



//#ifdef LINUX
//#define DWORD boost::uint32_t
//#else
//#include <WinInet.h>
//#endif


#define  STRINGIFY(x) #x 
#define  TOSTRING(x) STRINGIFY(x) 

typedef framework::string::Uuid RID;
typedef framework::string::Uuid MD5;
typedef framework::string::Uuid Guid;


using std::string;
using std::map;
using std::vector;
using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using namespace std;

//#include "macro.h"
//#include "protocol/UdpServer.h"
#include "protocol/Protocol.h"
#include "base/AppBuffer.h"


extern framework::timer::TimerQueue & global_second_timer();
extern framework::timer::TimerQueue & global_250ms_timer();

#ifndef ppassert
#define ppassert assert 
#endif 


namespace std {
#ifdef UNICODE
    typedef wstring tstring;
#else
    typedef string tstring;
#endif
}


#ifndef interface
#define interface struct
#endif

// std::string w2b(const std::wstring& _src);
// std::wstring b2w(const std::string& _src);


boost::asio::ip::udp::endpoint IpPortToUdpEndpoint(u_long ip, u_short port);

/// 遍历stl的容器(non-const方式)
#define STL_FOR_EACH(containerType, container, iter)    \
    for ( containerType::iterator iter = (container).begin(); (iter) != (container).end(); ++(iter) )

/// 遍历stl的容器(const方式)
#define STL_FOR_EACH_CONST(containerType, container, iter)    \
    for ( containerType::const_iterator iter = (container).begin(); (iter) != (container).end(); ++(iter) )

/// 限制指定变量的最小值
#define LIMIT_MIN(val, minVal) do { if ((val) < (minVal)) (val) = (minVal); ppassert((val) >= (minVal)); } while (false)

/// 限制指定变量的最大值
#define LIMIT_MAX(val, maxVal) do { if ((val) > (maxVal)) (val) = (maxVal); ppassert((val) <= (maxVal)); } while (false)

/// 限制指定变量的最小值和最大值
#define LIMIT_MIN_MAX(val, minVal, maxVal) \
    do { \
    if ((val) > (maxVal)) (val) = (maxVal); \
    if ((val) < (minVal)) (val) = (minVal); \
    assert((val) >= (minVal)); \
    assert((val) <= (maxVal)); \
    } while (false)

std::string GetDateString();

#endif //BOOTSTRAP_COMMON_H

