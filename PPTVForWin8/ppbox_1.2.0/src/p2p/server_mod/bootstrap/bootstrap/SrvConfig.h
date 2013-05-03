/* ======================================================================
 *    SrvConfig.h
 *    Copyright (c) 2009 Synacast. All rights reserved.
 *
 *    读取并解析服务器分组信息
 * ======================================================================
 *      Time        Changer     ChangeLog
 *    ~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 *    2009-4-29     cnhbdu      创建
 */

#ifndef __SRV_CONFIG_H__
#define __SRV_CONFIG_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "BConfig.h"
#include "ServerPacker.h"

BOOTSTRAP_NAMESPACE_BEGIN

class SrvConfig : public BPairConfig
{
public:
    bool FindNextName(SevType& type, boost::uint16_t& level);

    void ReadNextList(SrvList& sev_list);

    bool ParseSevLine(const std::string& line, SrvInfo& sev_info);

    void ParseNameLine(const std::string& line, 
        SevType& type, boost::uint16_t& level);

    bool IsNameLine(const std::string& line);

    void ParseFileName(const std::string name, boost::uint32_t& area_code);

private:
};

BOOTSTRAP_NAMESPACE_END

#endif // __SRV_CONFIG_H__