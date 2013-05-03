#pragma once
#ifdef BOOST_WINDOWS_API
#define NEED_TO_POST_MESSAGE
#endif

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
#include <framework/logger/Logger.h>
#include <framework/logger/LoggerStreamRecord.h>

#include <util/Util.h>
#include <util/serialization/Uuid.h>

//using namespace framework::configure;

enum LOG_LEVEL
{
    __DEBUG = framework::logger::Logger::kLevelDebug, 
    __INFO = framework::logger::Logger::kLevelInfor, 
    __EVENT = framework::logger::Logger::kLevelEvent, 
    __WARN = framework::logger::Logger::kLevelAlarm, 
    __ERROR = framework::logger::Logger::kLevelError
};

#define  STRINGIFY(x) #x 
#define  TOSTRING(x) STRINGIFY(x) 

#if (defined _DEBUG || defined DEBUG)
#  define LOG(level, type, msg) LOG_S(level, msg)
#  define LOGX(level, type, msg) LOG_S(level, __FILE__":"TOSTRING(__LINE__)" " << msg)
#else
#  define LOG(level, type, msg) 
#  define LOGX(level, type, message) 
#endif

#  if CONSOLE_LOG_DISABLE
#    define COUT(msg) 
#  else
#include <iostream>
#    define COUT(msg) std::cout << __FILE__":"TOSTRING(__LINE__)" " << msg << std::endl
#  endif

#   define RELEASE_LOG(message) LOG_S( 0 , message )
/*
#ifdef _DEBUG
#else
#   define RELEASE_LOG(message) do{\
std::ostringstream oss; \
oss << message; \
framework::Log::Inst().WriteLog(0, "", oss.str()); } while(false)
#endif
*/

#ifndef CONSOLE_LOG
#define  CONSOLE_LOG(msg) COUT(msg)
#endif //CONSOLE_LOG

#ifndef CONSOLE_OUTPUT
#define  CONSOLE_OUTPUT(msg) COUT(msg)
#endif //CONSOLE_OUTPUT

typedef framework::string::Uuid RID;
typedef framework::string::Uuid MD5;
typedef framework::string::Uuid Guid;

using std::string;
using std::map;
using std::vector;
using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;

//const size_t SUB_PIECE_SIZE = 1024;
//const size_t SUB_PIECE_COUNT_PER_PIECE = 128;
//const size_t PIECE_SIZE = SUB_PIECE_SIZE * SUB_PIECE_COUNT_PER_PIECE;
//const size_t BLOCK_MIN_SIZE = 2*1024*1024;
//const size_t BLOCK_MAX_COUNT = 50;

//#include "macro.h"
//#include "base/AppBuffer.h"
//#include "struct/SubPieceBuffer.h"

extern framework::timer::TimerQueue & global_second_timer();
extern framework::timer::TimerQueue & global_250ms_timer();

#ifndef ppassert
#define ppassert assert 
#endif 

struct SOCKET_ADDR
{
    boost::uint32_t IP;
    boost::uint16_t Port;

    SOCKET_ADDR(boost::uint32_t ip = 0, boost::uint16_t port = 0) 
        : IP(ip), Port(port) 
    {
    }

    bool operator ==(const SOCKET_ADDR& n) const
    {
        return 0 == memcmp(this, &n, sizeof(SOCKET_ADDR));
    }
};

boost::asio::ip::udp::endpoint IpPortToUdpEndpoint(u_long ip, u_short port);
std::ostream& operator << (std::ostream& os, const SOCKET_ADDR& socket_addr);

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


// 用来适配原来的接口
struct Config
{
    static Config& Inst()
    {
        static Config g_config;
        return g_config;
    }

    size_t GetInteger(  const std::string& section,const std::string & key , size_t default_value = 0 )
    {
        std::string value = this->GetTString(section, key , "" );

        if( !value.size() )
            return default_value;

        // 转换成 INT
        try {
            size_t v = boost::lexical_cast<size_t> (value.c_str());

            return v;
        } catch( boost::bad_lexical_cast &e) {
            return default_value;
        }
        //
        //		this->profile_.get( new_format.first , new_format.second , "" );
        return 0;
    }

    bool LoadConfig( const std::string & file_name )
    {
        return this->profile_.load( file_name ) == 0;
    }

    bool GetBoolean( const std::string& section, const std::string & key , bool default_value = false )
    {
        std::string value ;

        this->profile_.get( section , key , value);

        if(value != "true" && value != "false")
        {
            return default_value;
        }
        return value == "true";	
    }

    std::string GetTString( const std::string& section,const std::string & key , const std::string & default_value = "" )
    {
        std::string value ;
        this->profile_.get( section , key , value);

        if( !value.size() )
            return default_value;

        return value;
    }

    framework::configure::Profile profile_;
};

std::string GetDateString();

//#include "network/UdpServerHandlePacket.h"
#include "protocol/UdpServer.h"

#include <util/archive/LittleEndianBinaryIArchive.h>
#include <util/archive/LittleEndianBinaryOArchive.h>

#include <iostream>

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>

