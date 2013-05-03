#ifndef __PPLIVE_PUB_API_H_CK_20110908__
#define __PPLIVE_PUB_API_H_CK_20110908__

#include "Memconfig.h"
#include <boost/thread/detail/singleton.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <map>
#include <string>
#include <boost/thread/thread.hpp>

#ifndef BOOST_WINDOWS_API
#include <sys/types.h>
#include <unistd.h>
#endif

namespace ns_pplive
{
    typedef struct PubApiTag
    {
        inline static std::string ConfigItem(const std::string& seg_name, const std::string& item_name)
        {
            static std::map<std::string, std::map<std::string, std::string> > _config_map;
            if( _config_map[seg_name][item_name] == "" )
                _config_map[seg_name][item_name] = (boost::detail::thread::singleton<CPPliveCgiConfigFile>::instance())(seg_name.c_str(), item_name.c_str());

            return _config_map[seg_name][item_name];
        };

        inline static unsigned int GenRandom()
        {
            struct timeval _cur_t;
            unsigned int atime=0;
            //just get a random time seed
#ifndef BOOST_WINDOWS_API
            gettimeofday(&_cur_t,NULL);
            atime= (((unsigned long)_cur_t.tv_sec & 0xFFFF)+(unsigned long)_cur_t.tv_usec)^(unsigned long)_cur_t.tv_usec;
            atime += getpid();
#else
            atime = GetExactTimeNow()*10000;
            atime += GetCurrentThreadId();
#endif
            srand(atime);
            return rand();
        };

        inline static double GetExactTimeNow()
        {
            double time_now = (boost::uint64_t)time(NULL);
            boost::posix_time::ptime t(boost::posix_time::microsec_clock::local_time());		
            boost::posix_time::time_duration td = t.time_of_day();
            time_now += td.total_microseconds() % 1000000 * 0.000001;
            return time_now;

        };

    } PubApi;
};

#endif