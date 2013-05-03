/* ======================================================================
 *    IPLibImpl.cpp
 *    Copyright (c) 2009 Synacast. All rights reserved.
 *
 *    管理IP地址库，并向Bootstrap提供“IP-->地域码”查询
 * ======================================================================
 *    Time        Changer     ChangeLog
 *    ~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 *    2009-4-24     cnhbdu      创建
 */

#include "Common.h"
#include "IPLibImpl.h"

#include <sstream>
#include <boost/algorithm/string.hpp>

BOOTSTRAP_NAMESPACE_BEGIN
FRAMEWORK_LOGGER_DECLARE_MODULE("Bootstrap");
IPLibImpl::IPLibImpl() : m_version(0), m_public_ac(0)
{

}

IPLibImpl::~IPLibImpl()
{

}

bool IPLibImpl::TryImportIpAddrs(const std::string& import_file)
{
    std::ifstream config_file;
    config_file.open(import_file.c_str());
    if (false == config_file.is_open())
    {
        return false;
    }

    boost::uint32_t read_version;
    boost::uint32_t read_public_ac;

    bool b_ret = ReadNextPair(config_file, "IPLibVersion", read_version);
    if (false == b_ret) 
        return false;

    b_ret = ReadNextPair(config_file, "PublicID", read_public_ac);
    if (false == b_ret) 
        return false;

    //BS_LOG(">> IPLibVersion: "<<read_version<<" PublicAc: "<<m_public_ac);
    LOG4CPLUS_INFO(Loggers::IpLib(), "IPLibVersion: " << read_version << " PublicAc: " << m_public_ac);
    LOG4CPLUS_INFO(Loggers::IpLib(), "导入IP地址库...");

    m_version = read_version;
    m_public_ac = read_public_ac;

    IPUnitMap_T tmp_iplib;

    IPLibUnit unit;
    while (ReadNextUnit(config_file, unit))
    {
        tmp_iplib.insert(std::make_pair(unit.end_ipaddr, unit));
        LOG4CPLUS_INFO(Loggers::IpLib(), "\t " << unit.start_ipaddr << ' ' << unit.end_ipaddr << ' '
            << unit.area_code);
    }
    config_file.close();

    if (tmp_iplib.empty())
    {
        LOG4CPLUS_ERROR(Loggers::IpLib(), "IP lib is empty!");
        return false;
    }

    m_iplib.clear();
    m_iplib = tmp_iplib;

    return true;
}

bool IPLibImpl::ReadNextUnit(std::ifstream& config_file, IPLibUnit& unit)
{
    std::string line;
    while (ReadNextLine(config_file, line))
    {
        std::istringstream iss(line);
        std::string ip_l, ip_r, cod_s;
        iss >> ip_l >> ip_r >> cod_s;

        boost::system::error_code ec;
        boost::asio::ip::address_v4 address;
        // 起始IP地址
        address = boost::asio::ip::address_v4::from_string(ip_l, ec);
        if (!!ec) continue;
        unit.start_ipaddr = static_cast<boost::uint32_t>(address.to_ulong());

        // 结束IP地址
        address = boost::asio::ip::address_v4::from_string(ip_r, ec);
        if (!!ec) continue;
        unit.end_ipaddr = static_cast<boost::uint32_t>(address.to_ulong());

        // 地域码
        boost::uint8_t err_num;
        unit.area_code = bootstrap_lexical_cast<boost::uint32_t>(cod_s, err_num);
        if (!err_num)
        {
            return true;
        }
    }
    return false;
}

bool IPLibImpl::ReadNextPair(std::ifstream& config_file,
                const std::string& key_name, boost::uint32_t& val_ret)
{
    std::string line;
    while (ReadNextLine(config_file, line))
    {
        std::string::size_type pos = line.find('=');
        if (std::string::npos == pos)
        {
            continue;
        }
        std::string key = boost::algorithm::trim_copy(line.substr(0, pos));
        if (key_name == key)
        {
            std::string val = boost::algorithm::trim_copy(line.substr(pos+1));
            boost::uint8_t err_num;
            val_ret = bootstrap_lexical_cast<boost::uint32_t>(val, err_num);
            if (!err_num)
            {
                return true;
            }
        }
    }
    return false;
}

bool IPLibImpl::ReadNextLine(std::ifstream& config_file, std::string& line)
{
    while (true)
    {
        if (config_file.bad() || config_file.eof())
        {
            return false;
        }

        getline(config_file, line);
        line = boost::algorithm::trim_copy(line.substr(0, line.find('#')));
        if (line.empty())
        {
            continue;
        }
        return true;
    }
}

boost::uint32_t IPLibImpl::FindAreaCode(boost::uint32_t ip_addr) const
{
    IPUnitMap_T::const_iterator it = m_iplib.lower_bound(ip_addr);
    if (it != m_iplib.end() && ip_addr >= it->second.start_ipaddr)
    {
        return it->second.area_code;
    }
    return PublicAc();
}

boost::uint32_t IPLibImpl::FindAreaCode(const std::string& ip_addr) const
{
    boost::asio::ip::address_v4 address;
    boost::system::error_code ec;
    address = boost::asio::ip::address_v4::from_string(ip_addr, ec);
    if (!ec)
    {
        return FindAreaCode(static_cast<boost::uint32_t>(address.to_ulong()));
    }
    return PublicAc();
}

std::set<boost::uint32_t> IPLibImpl::GetAcList() const
{
    std::set<boost::uint32_t> ret;
    for (IPUnitMap_T::const_iterator cit = m_iplib.begin();
        cit != m_iplib.end(); ++cit)
    {
        ret.insert(cit->second.area_code);
    }
    return ret;
}

BOOTSTRAP_NAMESPACE_END