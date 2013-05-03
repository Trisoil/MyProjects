/* ======================================================================
 *    SrvConfig.h
 *    Copyright (c) 2009 Synacast. All rights reserved.
 *
 * ======================================================================
 *      Time        Changer     ChangeLog
 *    ~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 *    2009-4-29     cnhbdu      创建
 */

#include "Common.h"
#include "SrvConfig.h"

#include <boost/algorithm/string.hpp>

BOOTSTRAP_NAMESPACE_BEGIN

bool SrvConfig::FindNextName(SevType& type, boost::uint16_t& level)
{
    std::string line;

    while (ReadNextLine(line))
    {
        if (IsNameLine(line))
        {
            ParseNameLine(line, type, level);
            return true;
        }
    }
    return false;
}

void SrvConfig::ReadNextList(SrvList& sev_list)
{
    std::string line;

    while (ReadNextLine(line))
    {
        // 到下一个服务器结束
        if (IsNameLine(line))
        {
            BackToLastPos();
            break;
        }
        SrvInfo sev_info;
        if (ParseSevLine(line, sev_info))
        {
            sev_list.sevs.push_back(sev_info);
        }
    }
}

bool SrvConfig::ParseSevLine(const std::string& line, SrvInfo& sev_info)
{
    std::istringstream iss(line);
    std::string mod_s, prtcl_s, ip_s, port_s;
    iss >> mod_s >> prtcl_s >> ip_s >> port_s;

    boost::uint8_t err_num;

    // 对于只用来List的tracker，该值的后三位(10进制)表示机房ID，其余位表示模值
    // 如果mod = 2003，则模值 = 2, 机房ID = 3
    sev_info.mod = bootstrap_lexical_cast<boost::uint32_t>(mod_s, err_num);
    if (err_num) return false;

    // 连接协议
    boost::algorithm::to_lower(prtcl_s);
    if (prtcl_s == "udp")
    {
        sev_info.prtcl = protocol::TRACKER_INFO::UDP;
    }
    else if (prtcl_s == "tcp")
    {
        sev_info.prtcl = protocol::TRACKER_INFO::TCP;
    }
    else if (prtcl_s == "udpserver")
    {
        sev_info.prtcl = protocol::TRACKER_INFO::LIVE_UDPSERVER_TRACKER;
    }
    else
    {
        return false;
    }

    // IP地址
    boost::asio::ip::address_v4 address;
    boost::system::error_code ec;
    address = boost::asio::ip::address_v4::from_string(ip_s, ec);
    if (!!ec) return false;
    sev_info.ip = static_cast<boost::uint32_t>(address.to_ulong());

    // 端口
    sev_info.port = bootstrap_lexical_cast<boost::uint16_t>(port_s, err_num);
    if (err_num) return false;

    return true;
}

void SrvConfig::ParseNameLine(const std::string& line, 
                                 SevType& type, boost::uint16_t& level)
{
    std::string tmp_line = line.substr(1, line.size()-2);
    std::string::size_type pos = tmp_line.find('.');
    std::string name;

    if (pos != std::string::npos)
    {
        name = tmp_line.substr(0, pos);

        boost::uint8_t err;
        std::string level_s = tmp_line.substr(pos + 1);
        level = bootstrap_lexical_cast<boost::uint16_t>(level_s, err);
    }
    else
    {
        name = tmp_line;
        level = 0;
    }
    boost::algorithm::to_lower(name);

    if ("tracker_commit" == name)
    {
        type = ST_TRACK_CMIT;
    }
    else if ("tracker_list" == name)
    {
        type = ST_TRACK_LIST;
    }
    else if ("live_tracker_commit" == name)
    {
        type = ST_LIVE_TRACK_CMIT;
    }
    else if ("live_tracker_list" == name)
    {
        type = ST_LIVE_TRACK_LIST;
    }
    else if ("cache" == name)
    {
        type = ST_CACHE;
    }
    else if ("collecton" == name)
    {
        type = ST_COLLECT;
    }
    else if ("stun" == name)
    {
        type = ST_STUN;
    }
    else if ("index" == name)
    {
        type = ST_INDEX;
    }
    else if ("notify" == name)
    {
        type = ST_NOTIFY;
    }
    else if ("other" == name)
    {
        type = ST_OTHER;
    }
    else if ("tracker_for_listing" == name)
    {
        type = ST_TRACKER_FOR_LISTING_LIST;
    }
    else if ("live_tracker_for_listing" == name)
    {
        type = ST_LIVE_TRACKER_FOR_LISTING_LIST;
    }
    else
    {
        type = ST_UNKNOWN;
    }
}

bool SrvConfig::IsNameLine(const std::string& line)
{
    if (line.find('[') == 0 && line.find(']') == line.size()-1)
    {
        return true;
    }
    return false;
}

void SrvConfig::ParseFileName(const std::string name,
                                 boost::uint32_t& area_code)
{
    std::string code_s = name.substr(name.rfind('_') + 1);
    boost::uint8_t err;
    area_code = bootstrap_lexical_cast<boost::uint32_t>(code_s, err);
}


BOOTSTRAP_NAMESPACE_END