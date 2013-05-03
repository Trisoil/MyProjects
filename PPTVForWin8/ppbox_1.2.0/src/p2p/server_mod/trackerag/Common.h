#pragma once
#ifndef COMMON_H_20111122
#define COMMON_H_20111122

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

#define  STRINGIFY(x) #x 
#define  TOSTRING(x) STRINGIFY(x) 


#include <iostream>
#    define COUT(msg) std::cout << __FILE__":"TOSTRING(__LINE__)" " << msg << std::endl


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

extern framework::timer::TimerQueue & global_second_timer();
extern framework::timer::TimerQueue & global_250ms_timer();

#ifndef ppassert
#define ppassert assert 
#endif 

// 用来适配原来的接口
struct Config
{
    static Config& Instance()
    {
        static Config g_config;
        return g_config;
    }

    size_t GetInteger( const std::string & key , size_t default_value = 0 )
    {
        std::string value = this->GetTString( key , "" );

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

    bool GetBoolean( const std::string & key , bool default_value = false )
    {
        std::string value ;
        // 这里硬编码到 tracker
        this->profile_.get( "trackerag" , key , value);

        if(value != "true" && value != "false")
        {
            return default_value;
        }
        return value == "true";	
    }

    std::string GetTString( const std::string & key , const std::string & default_value = "" )
    {
        std::string value ;
        // 这里硬编码到 tracker
        this->profile_.get( "trackerag" , key , value);

        if( !value.size() )
            return default_value;

        return value;
    }

    framework::configure::Profile profile_;
};

#include "protocol/UdpServer.h"

#include <util/archive/LittleEndianBinaryIArchive.h>
#include <util/archive/LittleEndianBinaryOArchive.h>

#include <iostream>

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>

#endif//COMMON_H_20111122

