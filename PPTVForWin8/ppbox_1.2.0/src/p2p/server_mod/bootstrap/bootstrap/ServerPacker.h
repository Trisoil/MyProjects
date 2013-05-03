/* ======================================================================
 *    ServerPacker.h
 *    Copyright (c) 2009 Synacast. All rights reserved.
 *
 *    管理服务器分组信息，提供"AreaCode-->服务器信息"的查询
 * ======================================================================
 *      Time        Changer     ChangeLog
 *    ~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 *    2009-4-24     cnhbdu      创建
 */

#ifndef __SERVER_PACKER_H__
#define __SERVER_PACKER_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "BHeader.h"
#include "BConfig.h"

#include <map>
#include <list>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/noncopyable.hpp>

BOOTSTRAP_NAMESPACE_BEGIN

enum SevType
{
    ST_STUN = 0x01,    // stun server
    ST_CACHE,        // cache server
    ST_INDEX,        // index server
    ST_STATISTIC,    // statistic server
    ST_COLLECT,        // data collection server
    ST_TRACK_CMIT,    // commit tracker
    ST_TRACK_LIST,    // list tracker
    ST_LIVE_TRACK_CMIT,  // live commit tracker
    ST_LIVE_TRACK_LIST,  // live list tracker
    ST_NOTIFY,      // notify server
    ST_OTHER,        // 其他配置项，不是服务器信息
    ST_TRACKER_FOR_LISTING_LIST,
    ST_LIVE_TRACKER_FOR_LISTING_LIST,
    ST_UNKNOWN,     // don't know
};

const string SevTyeStr[] = 
{
    "Null",
    "Stun Server",
    "Cache Server",
    "Index Server",
    "Statistic Server",
    "Data Collection Server",
    "Tracker Commit",
    "Tracker List",
    "Live Tracker Commit",
    "Live Tracer List",
    "Notify Server",
    "Other",
    "Tracker For Listing",
    "Live Tracker For Listing",
    "unknown"
};

struct SrvInfo
{
    boost::uint32_t mod;
    protocol::TRACKER_INFO::TrackerType prtcl;
    boost::uint32_t ip;
    boost::uint16_t port;

    SrvInfo() { memset(this, 0, sizeof(SrvInfo)); }
    SrvInfo(const SrvInfo& other)
    {
        mod = other.mod;
        prtcl = other.prtcl;
        ip = other.ip;
        port = other.port;
    }

    const SrvInfo& operator=(const SrvInfo& other)
    {
        SrvInfo tmp_si(other);
        Swap(tmp_si);
        return *this;
    }

    void Swap(SrvInfo& other)
    {
        std::swap(mod, other.mod);
        std::swap(prtcl, other.prtcl);
        std::swap(ip, other.ip);
        std::swap(port, other.port);
    }

    protocol::TRACKER_INFO GenerateTracker(boost::uint8_t tracker_type) const;
};

struct SrvList
{
    SevType type;
    boost::uint16_t level;

    std::vector<SrvInfo> sevs;
    std::vector<boost::uint8_t> appds;

    SrvList() : type(ST_OTHER), level(0) { sevs.clear(); appds.clear(); }
    SrvList(const SrvList& other)
    {
        type = other.type;
        level = other.level;
        sevs = other.sevs;
        appds = other.appds;
    }

    const SrvList& operator=(const SrvList& other)
    {
        SrvList tmp_tl(other);
        Swap(tmp_tl);
        return *this;
    }

    void Swap(SrvList& other)
    {
        std::swap(type, other.type);
        std::swap(level, other.level);
        std::swap(sevs, other.sevs);
        std::swap(appds, other.appds);
    }

    friend std::ostream& operator<<(std::ostream& out, const SrvList& srv_list)
    {
        out << "[SevType: " << SevTyeStr[srv_list.type]
            << ", Level: " << srv_list.level
            << ", ServerNum: " << srv_list.sevs.size()
            << "]";
        return out;
    }
};

// level <--> server
typedef std::map<boost::uint16_t, SrvList> ServerMap_T;

inline std::ostream& operator<<(std::ostream& output, const ServerMap_T& server_map)
{
    for (ServerMap_T::const_iterator it = server_map.begin();
        it != server_map.end(); ++it)
    {
        output << it->second << " ";
    }
    return output;
}

struct SrvConf
{
    double collect_pb;
    double up_pic_pb;
    boost::uint16_t dac_report_minute;
};

struct AcServer
{
    ServerMap_T tracker_commit;
    ServerMap_T tracker_list;
    ServerMap_T live_tracker_commit;
    ServerMap_T live_tracker_list;
    ServerMap_T cache;
    ServerMap_T collection;
    ServerMap_T notify;
    SrvConf config;
    ServerMap_T tracker_for_listing_list[protocol::QueryTrackerForListingPacket::MAX_TRACKER_FOR_LISTING_TYPE];

    const ServerMap_T & GetTrackerServerMap(boost::uint8_t tracker_type) const;
};

inline std::ostream& operator<<(std::ostream& output, const AcServer& ac_server)
{
    output << ac_server.tracker_commit
           << ac_server.tracker_list
           << ac_server.live_tracker_commit
           << ac_server.live_tracker_list
           << ac_server.cache
           << ac_server.collection
           << ac_server.notify;
    return output;
}

class ServerPacker : private boost::noncopyable
{
public:
    // area code <--> server list
    typedef map<boost::uint32_t, AcServer> AcSevMap_T;

public:
    ServerPacker();

    void SetPublicAc(boost::uint32_t ac) { m_public_ac = ac; }

    bool IsPublicAcExist() const;

    bool ImportSevPack(const std::list<std::string>& import_list);

    bool ImportSevList(const std::string& import_file);

    bool ImportSomeSrv(const std::string& import_file);

    const AcServer& GetAcServer(boost::uint32_t area_code);

    const SrvList& GetIndexSrv() const { return m_index_server; }

    const SrvList& GetStunSrv() const { return m_stun_server; }

    const AcSevMap_T& GetSevMap() const { return m_sevpack; }

    void ImportConfigString(const std::string& import_file);

    const std::string& GetConfigString() const
    {
        return m_config_string;
    }

    const std::vector<protocol::SuperNodeInfo> & GetSuperNodeServers()
    {
        return super_node_infos_;
    }

    const std::vector<protocol::SuperNodeInfo> & GetVipSuperNodeServers()
    {
        return vip_super_node_infos_;
    }

private:
    void ImportSNList(const std::string& import_file, std::vector<protocol::SuperNodeInfo> & super_node_infos);
    void ImportCommonSNList(const std::string& import_file);
    void ImportVipSNList(const std::string& import_file);

private:
    AcSevMap_T m_sevpack;
    boost::uint32_t m_public_ac;

    SrvList m_index_server;
    SrvList m_stun_server;

    std::string m_config_string;

    std::vector<protocol::SuperNodeInfo> super_node_infos_;
    std::vector<protocol::SuperNodeInfo> vip_super_node_infos_;

    static const boost::uint8_t Priority;
};

BOOTSTRAP_NAMESPACE_END

#endif // __SERVER_PACKER_H__
