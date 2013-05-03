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
#include <boost/shared_ptr.hpp>

class IpLocation
{
public:
    //如果ip没有记录，那么就返回0
    boost::uint32_t GetLocation(boost::uint32_t ip);

    static boost::shared_ptr<IpLocation> Inst();
    static void SetIpLocation(const std::string & filename);

private:
    IpLocation(const std::string& filename);
    IpLocation(const IpLocation & iplocation);

private:
    //map的key为upperip，pair的first为lowerip，pair的second为location
    std::map<boost::uint32_t, std::pair<boost::uint32_t, boost::uint32_t> > iplocation_;

    static boost::shared_ptr<IpLocation> inst_;
};


#endif//IPLOCATION_H_20111013_CK