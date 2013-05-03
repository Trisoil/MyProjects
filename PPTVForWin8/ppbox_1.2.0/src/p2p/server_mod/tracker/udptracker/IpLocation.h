/******************************************************************************
*
* Copyright (c) 2011 PPLive Inc.  All rights reserved
* 
* IpLocation.h
* 
* Description: 将ip转换成4字节的location
*             
* 
* --------------------
* 2011-10-13, kelvinchen create
* --------------------
******************************************************************************/

#ifndef IPLOCATION_H_20111013_CK
#define IPLOCATION_H_20111013_CK
#include <string>
#include <map>
//#include <pair>


class IpLocation
{
public:
    static IpLocation * Instance(const std::string& filename = "ipdistribution.dat");
    static IpLocation* s_instance;
    static unsigned long long ClearIpFound();
    static unsigned long long ClearIpNotFound();
    static unsigned long long s_ip_found_;
    static unsigned long long s_ip_not_found_;
    IpLocation(const std::string& filename);
    //如果ip没有记录，那么就返回0
    unsigned int GetLocation(unsigned int ip);
    //在记录的ip里，随机选取一个
    unsigned int GetRandomRecordIp();
private:
    //map的key为upperip，pair的first为lowerip，pair的second为location
    std::map<unsigned,std::pair<unsigned,unsigned> > iplocation_;
};


#endif//IPLOCATION_H_20111013_CK