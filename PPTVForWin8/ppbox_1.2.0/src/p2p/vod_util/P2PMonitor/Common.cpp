#include "stdafx.h"
#include "Common.h"

void Value2IP(const int& nValue, char* str)
{  
    //数值转化为IP  
    //没有格式检查  
    //返回值就是结果  

    sprintf(str,"%d.%d.%d.%d",
        (nValue&0xff000000)>>24,  
        (nValue&0x00ff0000)>>16,  
        (nValue&0x0000ff00)>>8,  
        (nValue&0x000000ff));  
}

boost::uint32_t IP2Value(const std::string & str)
{
    boost::system::error_code ec;
    boost::asio::ip::address_v4 address(boost::asio::ip::address_v4::from_string(str, ec));
    return address.to_ulong();
}