/* ======================================================================
 *    IPLib.h
 *    Copyright (c) 2009 Synacast. All rights reserved.
 *
 *    管理IP地址库，并向Bootstrap提供“IP-->地域码”查询
 * ======================================================================
 *      Time        Changer     ChangeLog
 *    ~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 *    2009-4-23     cnhbdu      创建
 *  2009-4-24     cnhbdu      将具体实现交由IPLibImpl
 */

#ifndef __IPLIB_H__
#define __IPLIB_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "BHeader.h"
#include "IPLibImpl.h"

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

BOOTSTRAP_NAMESPACE_BEGIN

class IPLib : private boost::noncopyable
{
public:
    IPLib() : m_impl(new IPLibImpl) {}

    ~IPLib() {}

    boost::uint32_t Version() const { return m_impl->Version(); }

    boost::uint32_t PublicAc() const { return m_impl->PublicAc(); }

    bool TryImportIpAddrs(const std::string& import_file)
    {
        return m_impl->TryImportIpAddrs(import_file);
    }

    boost::uint32_t FindAreaCode(boost::uint32_t ip_addr) const
    {
        return m_impl->FindAreaCode(ip_addr);
    }

    boost::uint32_t FindAreaCode(const std::string& ip_addr) const
    {
        return m_impl->FindAreaCode(ip_addr);
    }

    std::set<boost::uint32_t> GetAcList() const
    {
        return m_impl->GetAcList();
    }

private:
    boost::shared_ptr<IPLibImpl> m_impl;
};

BOOTSTRAP_NAMESPACE_END

#endif // __IPLIB_H__