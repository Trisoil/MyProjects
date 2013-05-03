/* ======================================================================
 *    IPLibImpl.h
 *    Copyright (c) 2009 Synacast. All rights reserved.
 *
 *    管理IP地址库，并向Bootstrap提供“IP-->地域码”查询
 * ======================================================================
 *      Time        Changer     ChangeLog
 *    ~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 *    2009-4-24     cnhbdu      创建
 */

#ifndef __IPLIB_IMPL_H__
#define __IPLIB_IMPL_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "BHeader.h"

#include <map>
#include <string>
#include <set>
#include <fstream>
#include <iostream>
#include <boost/asio/ip/address_v4.hpp>

BOOTSTRAP_NAMESPACE_BEGIN

class IPLibImpl
{
public:
    struct IPLibUnit 
    {
        boost::uint32_t start_ipaddr;
        boost::uint32_t end_ipaddr;
        boost::uint32_t area_code;

        bool operator<(const IPLibUnit& other) 
        { return this->start_ipaddr < other.start_ipaddr; }
    };

public:
    IPLibImpl();

    ~IPLibImpl();

    boost::uint32_t Version() const { return m_version; }

    boost::uint32_t PublicAc() const { return m_public_ac; }

    bool TryImportIpAddrs(const std::string& import_file);

    boost::uint32_t FindAreaCode(boost::uint32_t ip_addr) const;

    boost::uint32_t FindAreaCode(const std::string& ip_addr) const;

    std::set<boost::uint32_t> GetAcList() const;

private:
    bool ReadNextUnit(std::ifstream& config_file, IPLibUnit& unit);

    bool ReadNextPair(std::ifstream& config_file, 
        const std::string& key_name, boost::uint32_t& val_ret);

    bool ReadNextLine(std::ifstream& config_file, std::string& line);

private:
    typedef std::map<boost::uint32_t, IPLibUnit> IPUnitMap_T;
    IPUnitMap_T m_iplib;
    boost::uint32_t m_version;
    boost::uint32_t m_public_ac;
};

inline std::ostream& operator<<(std::ostream& output, const IPLibImpl::IPLibUnit& unit)
{
    boost::asio::ip::address_v4 sip(unit.start_ipaddr);
    boost::asio::ip::address_v4 eip(unit.end_ipaddr);
    output<<"<"<<sip<<"> <"<<eip<<"> AreaCode: "<<unit.area_code;
    return output;
}

BOOTSTRAP_NAMESPACE_END

#endif // __IPLIB_IMPL_H__