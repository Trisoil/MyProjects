/* ======================================================================
 *    ServerPacker.cpp
 *    Copyright (c) 2009 Synacast. All rights reserved.
 *
 *    管理服务器分组信息，提供"AreaCode-->服务器信息"的查询
 * ======================================================================
 *      Time        Changer     ChangeLog
 *    ~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 *    2009-4-24     cnhbdu      创建
 */

#include "Common.h"
#include "ServerPacker.h"
#include "SrvConfig.h"
#include "Bootstrap.h"

#include <sstream>
#include <boost/algorithm/string.hpp>

BOOTSTRAP_NAMESPACE_BEGIN
FRAMEWORK_LOGGER_DECLARE_MODULE("Bootstrap");

const boost::uint8_t ServerPacker::Priority = 30;

ServerPacker::ServerPacker() : m_public_ac(0)
{

}

bool ServerPacker::IsPublicAcExist() const
{
    if (m_sevpack.find(m_public_ac) != m_sevpack.end())
    {
        return true;
    }
    return false;
}

const AcServer& ServerPacker::GetAcServer(boost::uint32_t area_code)
{
    AcSevMap_T::const_iterator it = m_sevpack.find(area_code);
    if (it != m_sevpack.end())
    {
        return it->second;
    }
    return m_sevpack[m_public_ac];
}

bool ServerPacker::ImportSevPack(const std::list<std::string>& import_list)
{

    m_sevpack.clear();

    for (std::list<std::string>::const_iterator it = import_list.begin();
        it != import_list.end(); ++it)
    {
        if (boost::algorithm::contains(*it, "bs_ac"))
        {
            ImportSevList(*it);
        }
        else if (boost::algorithm::ends_with(*it, "bs_srv_sn"))
        {
            ImportCommonSNList(*it);
        }
        else if (boost::algorithm::ends_with(*it, "bs_srv_vip_sn"))
        {
            ImportVipSNList(*it);
        }
        else if (boost::algorithm::contains(*it, "bs_srv"))
        {
            ImportSomeSrv(*it);
        }
        else if (boost::algorithm::ends_with(*it, "bs_config_string"))
        {
            ImportConfigString(*it);
        }
    }
    return true;
}

bool ServerPacker::ImportSomeSrv(const std::string& import_file)
{

    SrvConfig srv_conf;
    srv_conf.OpenConf(import_file);
    if (false == srv_conf.IsConfOpen())
    {
        return false;
    }

    SevType type;
    boost::uint16_t level;

    while (srv_conf.FindNextName(type, level))
    {
        if (ST_OTHER == type)
        {
            m_index_server.appds.clear();

            std::string key_s, val_s;
            while (srv_conf.ReadNextPair(key_s, val_s))
            {
                boost::uint8_t err;
                boost::uint8_t /*mod_l,*/ mod_r;
//                 mod_l = static_cast<boost::uint8_t>(
//                     bootstrap_lexical_cast<boost::uint16_t>(key_s, err)
//                     );
                mod_r = static_cast<boost::uint8_t>(
                    bootstrap_lexical_cast<boost::uint16_t>(val_s, err)
                    );
                m_index_server.appds.push_back(mod_r);
            }
        }
        else
        {
            SrvList sev_list;
            srv_conf.ReadNextList(sev_list);
            switch (type)
            {
            case ST_INDEX:
                m_index_server.level = level;
                m_index_server.type = type;
                m_index_server.sevs = sev_list.sevs;
                break;
            case ST_STUN:
                m_stun_server.level = level;
                m_stun_server.type = type;
                m_stun_server.sevs = sev_list.sevs;
                break;
            }
        }
    }

    srv_conf.CloseConf();

    LOG4CPLUS_INFO(bootstrap::Loggers::ServerPacker(), "导入 " << import_file);

    return true;
}

bool ServerPacker::ImportSevList(const std::string& import_file)
{
    SrvConfig srv_conf;
    srv_conf.OpenConf(import_file);
    if (false == srv_conf.IsConfOpen())
    {
        return false;
    }

    AcServer ac_server;
    boost::uint32_t area_code;
    srv_conf.ParseFileName(import_file, area_code);

    SevType type;
    boost::uint16_t level;
    while (srv_conf.FindNextName(type, level))
    {
        if (ST_OTHER == type)
        {
            std::string key_s, val_s;
            while (srv_conf.ReadNextPair(key_s, val_s))
            {
                boost::uint8_t err;
                if ("collect_pb" == key_s)
                {
                    ac_server.config.collect_pb = 
                        bootstrap_lexical_cast<double>(val_s, err);
                }
                else if ("up_pic_pb" == key_s)
                {
                    ac_server.config.up_pic_pb = 
                        bootstrap_lexical_cast<double>(val_s, err);
                }
                else if ("dac_report_minute" == key_s)
                {
                    ac_server.config.dac_report_minute =
                        bootstrap_lexical_cast<boost::uint16_t>(val_s, err);
                }
            }
        }
        else
        {
            SrvList sev_list;
            sev_list.type = type;
            sev_list.level = level;
            srv_conf.ReadNextList(sev_list);

            if (!sev_list.sevs.empty())
            {
                switch (sev_list.type)
                {
                case ST_TRACK_CMIT:
                    ac_server.tracker_commit[sev_list.level] = sev_list;
                    break;
                case ST_TRACK_LIST:
                    ac_server.tracker_list[sev_list.level] = sev_list;
                    break;
                case ST_LIVE_TRACK_CMIT:
                    ac_server.live_tracker_commit[sev_list.level] = sev_list;
                    break;
                case ST_LIVE_TRACK_LIST:
                    ac_server.live_tracker_list[sev_list.level] = sev_list;
                    break;
                case ST_CACHE:
                    ac_server.cache[sev_list.level] = sev_list;
                    break;
                case ST_COLLECT:
                    ac_server.collection[sev_list.level] = sev_list;
                    break;
                case ST_NOTIFY:
                    ac_server.notify[sev_list.level] = sev_list;
                    break;
                case ST_TRACKER_FOR_LISTING_LIST:
                    ac_server.tracker_for_listing_list[protocol::QueryTrackerForListingPacket::VOD_TRACKER_FOR_LISTING][sev_list.level] = sev_list;
                    break;
                case ST_LIVE_TRACKER_FOR_LISTING_LIST:
                    ac_server.tracker_for_listing_list[protocol::QueryTrackerForListingPacket::LIVE_TRACKER_FOR_LISTING][sev_list.level] = sev_list;
                    break;
                }
            }
        }
    }

    m_sevpack[area_code] = ac_server;

    srv_conf.CloseConf();

    LOG4CPLUS_INFO(bootstrap::Loggers::ServerPacker(), "导入" << import_file << ", Server: " << ac_server);

    return true;
}

void ServerPacker::ImportConfigString(const std::string& import_file)
{
    FILE *fp = fopen(import_file.c_str(), "rb");

    if (fp == NULL)
    {
        LOG4CPLUS_ERROR(bootstrap::Loggers::MisSync(), "缺少bs_config_string文件");
        assert(false);
    }
    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    // 协议中是用uint16类型来存储长度的，所以在这我们认为length应该小于uint16能表达的最大值
    assert(length < 65536);
    fseek(fp, 0, SEEK_SET);

    m_config_string.resize(length);
    fread((void *)m_config_string.c_str(), sizeof(char), length, fp);

    fclose(fp);
}

void ServerPacker::ImportCommonSNList(const std::string& import_file)
{
    ImportSNList(import_file, super_node_infos_);
}

void ServerPacker::ImportVipSNList(const std::string& import_file)
{
    ImportSNList(import_file, vip_super_node_infos_);
}

void ServerPacker::ImportSNList(const std::string& import_file, std::vector<protocol::SuperNodeInfo> & super_node_infos)
{
    std::ifstream sn_list(import_file.c_str());
    if (sn_list)
    {
        super_node_infos.clear();

        while (!sn_list.eof())
        {
            std::string line_text;

            std::getline(sn_list, line_text);
            std::istringstream iss(line_text, std::istringstream::in);

            std::string ip_str;
            boost::uint16_t port;

            iss >> ip_str >> port;

            if (ip_str.length() > 0)
            {
                boost::system::error_code ec;
                boost::asio::ip::address_v4 address = boost::asio::ip::address_v4::from_string(ip_str, ec);
                if (!ec)
                {
                    boost::uint32_t ip = address.to_ulong();
                    super_node_infos.push_back(protocol::SuperNodeInfo(ip, port, Priority));
                }
                else
                {
                    LOG4CPLUS_ERROR(bootstrap::Loggers::ServerPacker(), "SuperNode List 配置文件错误!!!");
                }
            }
        }

        sn_list.close();
    }
}

const ServerMap_T & AcServer::GetTrackerServerMap(boost::uint8_t tracker_type) const
{
    switch(tracker_type)
    {
    case Bootstrap::VodReportTracker:
        return tracker_list;

    case Bootstrap::VodListTracker:
        return tracker_for_listing_list[protocol::QueryTrackerForListingPacket::VOD_TRACKER_FOR_LISTING];

    case Bootstrap::LiveReportTracker:
        return live_tracker_list;

    case Bootstrap::LiveListTracker:
        return tracker_for_listing_list[protocol::QueryTrackerForListingPacket::LIVE_TRACKER_FOR_LISTING];

    default:
        assert(false);
        LOG4CPLUS_WARN(bootstrap::Loggers::ServerPacker(), "Error tracker type!!!");
        ServerMap_T empty_map;
        return empty_map;
    }
}

protocol::TRACKER_INFO SrvInfo::GenerateTracker(boost::uint8_t tracker_type) const
{
    protocol::TRACKER_INFO tracker;

    tracker.Reserve = 0;
    tracker.IP = ip;
    tracker.Port = port;
    tracker.Type = prtcl;

    if (Bootstrap::IsReportTracker(tracker_type))
    {
        tracker.StationNo = 0;
        tracker.ModNo = mod;
    }
    else
    {
        tracker.StationNo = mod % 1000;
        tracker.ModNo = mod / 1000;
    }

    return tracker;
}

BOOTSTRAP_NAMESPACE_END