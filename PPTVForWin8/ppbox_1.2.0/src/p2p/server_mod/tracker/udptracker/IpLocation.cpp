#include "IpLocation.h"
#include <sstream>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>

using namespace std;
extern log4cplus::Logger g_logger;

IpLocation* IpLocation::s_instance = NULL;
unsigned long long IpLocation::s_ip_found_ = 0;
unsigned long long IpLocation::s_ip_not_found_ = 0;

IpLocation* IpLocation::Instance(const string& filename)
{		
    if( NULL == s_instance )
    {
        LOG4CPLUS_INFO(g_logger,"iplocation load file "<<filename);
        s_instance = new IpLocation(filename);
    }
    return s_instance;
}

unsigned long long IpLocation::ClearIpFound()
{
    unsigned long long ret = s_ip_found_;
    s_ip_found_ = 0;
    return ret;
}

unsigned long long IpLocation::ClearIpNotFound()
{
    unsigned long long ret = s_ip_not_found_;
    s_ip_not_found_ = 0;
    return ret;
}


IpLocation::IpLocation(const string& filename)
{
    ifstream loadfile_stream;
    loadfile_stream.open(filename.c_str());
    if (loadfile_stream.fail())
    {
        LOG4CPLUS_ERROR(g_logger,"iplocation load file error "<<filename);
        return;
    }

    string line;
    while (getline(loadfile_stream,line)) 
    {
        stringstream ss(line);
        unsigned ipupper,iplower,location;
        ss>>iplower>>ipupper>>location;
        //iplocation_[iplower] = make_pair(ipupper,location);
        iplocation_[ipupper] = make_pair(iplower,location);		
    }
    LOG4CPLUS_INFO(g_logger,"iplocation success "<<iplocation_.size()<<" items");
}

unsigned int IpLocation::GetLocation(unsigned int ip)
{
    std::map<unsigned,std::pair<unsigned,unsigned> >::iterator it = iplocation_.lower_bound(ip);
    if(it == iplocation_.end() ||  it->second.first > ip)
    {
        //不在合法的范围内
        ++s_ip_not_found_;
        LOG4CPLUS_TRACE(g_logger,"meet unknown ip"<<ip<<" ipfound:"<<s_ip_found_<<"ip_not_found:"<<s_ip_not_found_);		
        return 0;
    }
    ++s_ip_found_;
    return it->second.second;
}

unsigned int IpLocation::GetRandomRecordIp()
{
    static unsigned rand_num = rand();
    srand(time(NULL));
    rand_num += rand() * rand();

    std::map<unsigned,std::pair<unsigned,unsigned> >::iterator it= iplocation_.lower_bound(rand_num);
    while(it == iplocation_.end())
    {
        rand_num += rand();
        it= iplocation_.lower_bound(rand_num);
    }

    return it->second.first + rand() % (it->first - it->second.first +1);

}