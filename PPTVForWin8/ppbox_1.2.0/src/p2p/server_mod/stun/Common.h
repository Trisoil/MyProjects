#ifndef STUNSERVER_COMMON_H
#define STUNSERVER_COMMON_H

#ifdef BOOST_WINDOWS_API
#define NEED_TO_POST_MESSAGE
#endif

#ifndef _WIN32_WINNT        // 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0500    // 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif                        

//#if(_WIN32_WINNT == 0x0500)
//typedef unsigned __int64 u_int64;
//#endif //(_WIN32_WINNT == 0x0500)

#ifdef _DEBUG

#define NEED_LOG
//#define _SECURE_SCL 0

#else

//#define NEED_LOG
// #define _SECURE_SCL 0
// 
#endif
// TODO: 在此处引用程序需要的其他头文件
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
//#include <hash_map>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>

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

#include "protocol/Protocol.h"
#include "base/AppBuffer.h"

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
#  define LOGX(level, msg) LOG_S(level, __FILE__":"TOSTRING(__LINE__)" " << msg)
#else
#  define LOG(level, type, msg) LOG_S(level, msg)
#  define LOGX(level, type, message) 
#endif

#   define RELEASE_LOG(message) LOG_S( 0 , message )

#  if CONSOLE_LOG_DISABLE
#    define COUT(msg) 
#  else
#include <iostream>
#    define COUT(msg) std::cout << /*__FILE__":"TOSTRING(__LINE__)" " <<*/ msg << std::endl
#  endif

#ifndef CONSOLE_LOG
#define CONSOLE_LOG(msg) COUT(msg)
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
using namespace std;

struct Config
{
    static Config& Inst()
    {
        static Config g_config;
        return g_config;
    }

    size_t GetInteger( const std::string & key , size_t default_value = 0 )
    {
        // 先查缓存
        {
            std::map< std::string , size_t >::iterator iter = this->integer_cached_map_.find( key );
            if( iter != this->integer_cached_map_.end() ) {
                return iter->second;
            }
        }

        std::string value = this->GetTString( key , "" );

        if( !value.size() )
            return default_value;

        // 转换成 INT
        try {
            size_t v = boost::lexical_cast<size_t> (value.c_str());

            // 加入
            this->integer_cached_map_.insert( std::make_pair( key , v ) );
            return v;
        } catch( boost::bad_lexical_cast &e) {
            return default_value;
        }
        //
        //        this->profile_.get( new_format.first , new_format.second , "" );
        return 0;
    }

    bool LoadConfig( const std::string & file_name )
    {
        return this->profile_.load( file_name ) == 0;
    }

    bool GetBoolean( const std::string & key , bool default_value = false )
    {
        std::string value ;
        // 这里硬编码到 stun
        this->profile_.get( "stun" , key , value);

        return value == "true";
    }

    std::string GetTString( const std::string & key , const std::string & default_value = "" )
    {
        std::string value ;
        // 这里硬编码到 stun
        this->profile_.get( "stun" , key , value);

        if( !value.size() )
            return default_value;

        return value;
    }

    std::map< std::string , size_t > integer_cached_map_;

    framework::configure::Profile profile_;
};



#endif //STUNSERVER_COMMON_H

