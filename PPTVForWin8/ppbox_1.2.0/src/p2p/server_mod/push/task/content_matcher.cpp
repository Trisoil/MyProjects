/* ======================================================================
 *	content_matcher.cpp
 *	Copyright (c) 2011 Synacast. All rights reserved.
 *
 *	push server新热片管理
 */

#include "stdafx.h"
#include "push_server.h"
#include "push_struct.h"
#include "content_matcher.h"
#include "content_quota_scheduler.h"
#include "key_generator.h"
#include "tinyxml/tinyxml.h"
#include "URLEncode.h"
#include "type_compatible.h"
#include <push_util/util.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <boost/date_time.hpp>
#include <boost/date_time/date.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>
#include "boost/date_time/posix_time/posix_time.hpp" 
#include <map>
#include <stdio.h>

extern log4cplus::Logger g_logger;
extern log4cplus::Logger g_statlog;
extern log4cplus::Logger g_vippush;

PUSH_SERVER_NAMESPACE_BEGIN

FRAMEWORK_LOGGER_DECLARE_MODULE("matcher");

ContentManager::ContentManager() 
#ifndef TEST_CONTENT_MANAGER
   : m_readnewhot_timer(global_second_timer(), 1000, boost::bind(&ContentManager::OnTimerElapsed, this, &m_readnewhot_timer))
#endif
{
#ifndef TEST_CONTENT_MANAGER
    m_readnewhot_timer.stop();
#endif
    m_started = false;
}

void ContentManager::Start()
{
    if (m_started == true) return;

    unsigned char ch1[] = {0xB5, 0xDA, 0x00};  // 第 in GBK encoding.
    unsigned char ch2[] = {0xBC, 0xAF, 0x00};  // 集 in GBK encoding.
    unsigned char new_ch1[] = {0xE7, 0xAC, 0xAC, 0x00};  // 第 in utf-8 encoding.
    unsigned char new_ch2[] = {0xE9, 0x9B, 0x86, 0x00};  // 集 in utf-8 encoding.
    std::stringstream s1, s2;
    s1 << ch1;
    s2 << ch2;
    std::string str1, str2;
    s1 >> str1;
    s2 >> str2;
    reg_exp1 = "(.+)\\(" + str1 + "(\\d+)" + str2 + "\\).*\\[(\\d+)\\]\\..+";
    reg_exp2 = "(.+)-(\\d{8})-.*\\[(\\d+)\\]\\..+";

    //build the new reg_expression
    std::stringstream new_s1, new_s2;
    std::string nstr1, nstr2;
    new_s1 << new_ch1;
    new_s2 << new_ch2;
    new_s1 >> nstr1;
    new_s2 >> nstr2;
    newreg_exp1 = "(.+)\\(" + nstr1 + "(\\d+)" + nstr2 + "\\).*\\[(\\d+)\\]\\.mp4\\.ft(\\d*)";
    newreg_exp2 = "(.+)-(\\d{8})-.*\\[(\\d+)\\]\\.mp4\\.ft(\\d*)";
 
    //load the saved hot content file 
    m_local_data_file = m_config.CONTENT_HOTSAVE_FILENAME;
    m_local_viprid_file = m_config.CONTENT_VIP_RID_FILENAME;
    m_new_hot_file = m_config.CONTENT_NEWHOT_XML_FILENAME;
    m_new_upload_file = m_config.CONTENT_NEWUPLOAD_XML_FILENAME;
    m_new_viprid_file = m_config.CONTENT_NEWVIP_RID_XML_FILENAME;
    m_new_ridmakeup_file = m_config.CONTENT_NEWRID_MAKEUP_XML_FILENAME;
    UpdateGoodNatSet(m_config.CONTENT_ACTPUSH_GOOD_NAT_STRING);
    m_next_internal_id = 1;
    load_local_data_file();
    if (m_config.CONTENT_VIPPUSH_ENABLED) {
        Load_New_VIP_RIDs(LOAD_SAVE_FILE);  // we will load the saved file.
    }
    internal_timer_count = 0;
    m_scheduler.Reset(&m_config);
    load_scheduler_session();
    uint32_t now_second = m_scheduler.GetDailyTimePoint();
    if ((now_second > m_last_second) && (now_second - m_last_second < (24*3600)) ) {
        m_scheduler.SetLastPoint(m_last_second, m_last_quota);
    }
    m_per_minute_key = KeyGenerator::GenKey();
    LoadNewContentsAndEpisodes();
    //it is important to call VIPRID reload after Content/Episode reload.
    ReLoadNewVIPRIDs();
    ResetPushListAndCalculateLimit();

#ifndef TEST_CONTENT_MANAGER
    m_readnewhot_timer.interval(60*1000);
    m_readnewhot_timer.start();
#endif
    m_started = true;
    LOG4CPLUS_INFO(g_statlog, "Started");
    if (m_config.CONTENT_PUSH_DISABLED != 0) {
        CONSOLE_LOG("WARNING: Push is DISABLED. Check your config file!");
        LOG4CPLUS_WARN(g_statlog, "Push is Disabled");
    }
    if (m_config.CONTENT_PAUSE_DISABLED != 0) {
        CONSOLE_LOG("WARNING: Push is enabled during PEAK hour. Check your config file!");
        LOG4CPLUS_WARN(g_statlog, "Peak Control Disabled");
    }
}

void ContentManager::Stop()
{
    if (m_started == false) return;

    save_local_data_file();
    if (m_config.CONTENT_VIPPUSH_ENABLED) {
        save_local_viprid_file();
    }
    m_content_map.clear();
    m_viprid_data.clear();
    m_started = false;
    LOG4CPLUS_INFO(g_statlog, "Stopped");
}

int64_t ContentManager::RebuildActivePushList()
{
    actpush_content_iter = m_content_map.end();
    actpush_episode_index = 0;
    actpush_rid_index = 0;
    actpush_episode_bitrate_type = 0;
    actpush_low_bw_done = false;
    actpush_low_online_done = false;
    actpush_high_disk_done = false;

    int ep_count = 0;
    int rid_count = 0;
    int64_t rid_cap_total = 0;
    int64_t rid_cap_tmp = 0;
    LOG4CPLUS_DEBUG(g_logger, "ENTER: RebuildActivePushList--ep_count:" << ep_count << " rid_count:" << rid_count
        << " rid_cap_total:" << rid_cap_total << " rid_cap_tmp:" << rid_cap_tmp 
        << "m_content_map.size:" << m_content_map.size());
    for (ContentIterator citer = m_content_map.begin(); citer != m_content_map.end(); citer ++)
    {
        ep_count += citer->second.episodes.size();
        //we now iterate through the content list.
        for (EpisodesIterator eiter = citer->second.episodes.begin(); eiter != citer->second.episodes.end(); eiter ++)
        {
            if ((actpush_content_iter == m_content_map.end()) && (false == IsEpisodeActivePushFull(*eiter))) {
                actpush_content_iter = citer;
                actpush_episode_index = eiter->eindex;
                actpush_episode_bitrate_type = eiter->bitrate_type;
                actpush_rid_index = 0;
            }
            rid_cap_tmp = (int64_t)(eiter->rid_array.size()*m_config.CONTENT_ACTPUSH_PER_RID_LIMIT) - (int64_t)eiter->total_active_pushed;
            LOG4CPLUS_DEBUG(g_logger, "rid.size:" << eiter->rid_array.size()
                << " CONTENT_ACTPUSH_PER_RID_LIMIT:" << m_config.CONTENT_ACTPUSH_PER_RID_LIMIT
                << " eiter->total_active_pushed:" << eiter->total_active_pushed
                << " rid_cap_tmp:" << rid_cap_tmp);
            if (rid_cap_tmp > 0) rid_cap_total += rid_cap_tmp;
            rid_count += eiter->rid_array.size();
        }
    }
    LOG4CPLUS_DEBUG(g_logger, "ep_count:" << ep_count << " rid_count:" << rid_count
        << " rid_cap_total:" << rid_cap_total << " m_content_map.size:" << m_content_map.size());
    LOG4CPLUS_INFO(g_logger, "ep_count:" << ep_count << " rid_count:" << rid_count
        << " rid_cap_total:" << rid_cap_total << " m_content_map.size:" << m_content_map.size());

    return rid_cap_total;
}

int64_t ContentManager::RebuildVIPRIDList()
{
    int64_t sum = 0;
    vippush_rid_iter = m_viprid_data.begin();
    vippush_low_bw_done = false;

    for (VIPRIDIterator viter = m_viprid_data.begin(); viter != m_viprid_data.end(); viter ++) {
        int count_diff = viter->second.rid_full.rid_push_target - viter->second.rid_full.rid_actpush_count;
        if (count_diff > 0) {
            sum += count_diff;
        }
    }
    LOG4CPLUS_INFO(g_logger, "VIPRIDList has " << sum << " push tasks in total. RID count " << m_viprid_data.size());
    return sum;
}

bool ContentManager::IsEpisodeActivePushFull(const Episode &p_episode)
{
    LOG4CPLUS_DEBUG(g_logger, "IsEpisodeActivePushFull--total_active_pushed:" << p_episode.total_active_pushed
        << " p_episode.rid_array.size():" << p_episode.rid_array.size()
        << " m_config.CONTENT_ACTPUSH_PER_RID_LIMIT:" << m_config.CONTENT_ACTPUSH_PER_RID_LIMIT);
    if (p_episode.total_active_pushed >= p_episode.rid_array.size()*m_config.CONTENT_ACTPUSH_PER_RID_LIMIT) 
    {
        return true;
    }
    return false;
}

void ContentManager::OnTimerElapsed(framework::timer::Timer * timer)
{
    if (m_started == false) return;

#ifndef TEST_CONTENT_MANAGER
    if (timer == &m_readnewhot_timer) {
        //now we need to check whether there are hot content, and new matched episode.
        internal_timer_count ++;

        m_scheduler.PerMinuteTimerProcess();
        m_per_minute_key = KeyGenerator::GenKey();
        save_scheduler_session();
        if ( internal_timer_count >= m_config.CONTENT_HOTSAVE_RELOAD_TIME) {
            internal_timer_count = 0;
            LoadNewContentsAndEpisodes();
            //it is important to call VIPRID reload after Content/Episode reload.
            ReLoadNewVIPRIDs();
            ResetPushListAndCalculateLimit();
        }
    }
#endif
}

void ContentManager::LoadNewContentsAndEpisodes()
{
    //before we load new content, we will always make sure VIP contents are valid.
    AddContentByName("VIP", "VIP", 0);

    Load_New_Hot_Content();
    Load_New_Upload_Episodes();

    //load the adjust files.
    Load_New_RID_Makeup();
    ProcessHotRIDMakeup();

    RetireContents();
    save_local_data_file();
}

void ContentManager::ResetPushListAndCalculateLimit()
{
    int64_t total_rid_sum = 0;
    if (m_config.CONTENT_ACTPUSH_ENABLED == 1) {
        total_rid_sum += RebuildActivePushList();
    }
    if (m_config.CONTENT_VIPPUSH_ENABLED == 1) {
        total_rid_sum += RebuildVIPRIDList();
    }

    LOG4CPLUS_DEBUG(g_logger, "ReCalculateLimit--CONTENT_ACTPUSH_CONTROL_MINUTE:"
        << m_config.CONTENT_ACTPUSH_CONTROL_MINUTE << " CONTENT_RIDS_MINUTE_NUM:" <<m_config.CONTENT_RIDS_MINUTE_NUM
        << " CONTENT_ACTPUSH_MIN_MIN_LIMIT:" << m_config.CONTENT_ACTPUSH_MIN_MIN_LIMIT);
    uint32_t avg = 0;
    if (total_rid_sum != 0) {
        avg = total_rid_sum / m_config.CONTENT_ACTPUSH_CONTROL_MINUTE;
        LOG4CPLUS_DEBUG(g_logger, "ReCalculateLimit1 -- avg" << avg);
        if (avg > m_config.CONTENT_RIDS_MINUTE_NUM) {
            LOG4CPLUS_DEBUG(g_logger, "Push Limit " << avg << " too big, reverted.");
            avg = m_config.CONTENT_RIDS_MINUTE_NUM;
            LOG4CPLUS_DEBUG(g_logger, "ReCalculateLimit2-- avg" << avg);
        }else if (avg < m_config.CONTENT_ACTPUSH_MIN_MIN_LIMIT) {
            //avg cannot be too small. 
            LOG4CPLUS_DEBUG(g_logger, "Push Limit " << avg << " too small, set to DEFAULT " << m_config.CONTENT_ACTPUSH_MIN_MIN_LIMIT);
            avg = m_config.CONTENT_ACTPUSH_MIN_MIN_LIMIT;
            LOG4CPLUS_DEBUG(g_logger, "ReCalculateLimit3-- avg" << avg);
        }
    }
    LOG4CPLUS_INFO(g_statlog, "ALimit: " << avg);
    m_scheduler.SetActiveMaxCap(avg);
}

void ContentManager::ReLoadNewVIPRIDs()
{
    if (m_config.CONTENT_VIPPUSH_ENABLED == 1) {
        Load_New_VIP_RIDs(LOAD_NEW_FILE);
        RetireVIPRIDs(true);
        save_local_viprid_file();
    }
}

void ContentManager::Load_New_Hot_Content()
{
    using namespace tinyxml;

    boost::filesystem::path path(boost::filesystem::current_path().string());
    path /= m_new_hot_file; 
    LOG4CPLUS_INFO(g_logger, "Load New Hot Content: " << path.string());
    try
    {
        if (boost::filesystem::exists(path))
        {
            TiXmlDocument data_file(path.string());
            if (data_file.LoadFile())
            {
                TiXmlElement *contentlist = data_file.FirstChild("contentlist")->ToElement();
                if (NULL == contentlist) {
                    //invalid or empty XML file, we will ignore it.
                    LOG4CPLUS_WARN(g_logger, "cannot obtain contentlist handle. Invalid or empty XML file.");
                    CONSOLE_LOG("cannot obtain contentlist handle. Invalid or empty XML file.");
                    BackupFile(m_new_hot_file, true);
                    return;
                }
                TiXmlNode *content_node = contentlist->FirstChild("content");
                if (NULL == content_node) {
                    //invalid or empty XML file, we will ignore it.
                    LOG4CPLUS_WARN(g_logger, "cannot obtain content_node handle. Invalid or empty XML file.");
                    CONSOLE_LOG("cannot obtain content_node handle. Invalid or empty XML file.");
                    BackupFile(m_new_hot_file, true);
                    return;
                }
                for (TiXmlElement *content_element = content_node->ToElement(); content_element; content_element = content_element->NextSiblingElement())
                {
                    std::string showname = content_element->FirstChild("name")->ToElement()->GetText();
                    TiXmlElement *index_element = content_element->FirstChild("index")->ToElement();
                    std::string content_type = "TV";
                    TiXmlNode *content_type_node = content_element->FirstChild("type");
                    if (content_type_node) {
                        content_type = content_type_node->ToElement()->GetText();
                    }
                    if (index_element) {
                        int eindex = atoi(index_element->GetText());
                        if ((eindex > 0) || ((content_type == "Movie" || content_type == "VIP")&&(eindex ==0))) {
                            LOG4CPLUS_DEBUG(g_logger, "Added Content " << showname );
                            AddContentByName(showname, content_type, eindex);
                        }else{
                            //for movies, we are ignoring them for now.
                        }
                    }else{
                        //for movies, we are going to ignore them for now.
                    }
                }
                data_file.SaveFile();
            }else{
                LOG4CPLUS_ERROR(g_logger, "ERROR: loading new_hot content file failed, format error.");
                CONSOLE_LOG("ERROR: loading new_hot content file failed, format error.");
            }
        }else{ 
            LOG4CPLUS_INFO(g_logger, "File " << path << " not found, probably not generated yet.");
            //CONSOLE_LOG("File " << path << " not found, probably not generated yet.");
            return;
        }
    }
    catch (boost::filesystem::basic_filesystem_error<boost::filesystem::path> &e)
    {
        LOG4CPLUS_ERROR(g_logger, "Exception caught when loading new_hot content file from DAC script.");
        CONSOLE_LOG("Exception caught when loading new_hot content file from DAC script.");
        return;
    }

    BackupFile(m_new_hot_file, true);

}

void ContentManager::Load_New_Upload_Episodes()
{
    using namespace tinyxml;

    boost::filesystem::path path(boost::filesystem::current_path().string());
    path /= m_new_upload_file; 
    LOG4CPLUS_DEBUG(g_logger, "Load New Upload Content: " << path.string());
    try
    {
        if (boost::filesystem::exists(path))
        {
            TiXmlDocument data_file(path.string());
            if (data_file.LoadFile())
            {
                TiXmlElement *eplist = data_file.FirstChild("episodes")->ToElement();
                if (eplist == NULL) {
                    LOG4CPLUS_WARN(g_logger, "cannot obtain EpisodeList handle, invalid or empty XML file");
                    CONSOLE_LOG("cannot obtain EpisodeList handle, invalid or empty XML file");
                    BackupFile(m_new_upload_file, true);
                    return;
                }
                TiXmlNode *ep_node = eplist->FirstChild("episode");
                if (ep_node == NULL) {
                    LOG4CPLUS_WARN(g_logger, "cannot obtain Episode handle, invalid or empty XML file");
                    CONSOLE_LOG("cannot obtain Episode handle, invalid or empty XML file");
                    BackupFile(m_new_upload_file, true);
                    return;
                }
                for (TiXmlElement *ep_element = ep_node->ToElement(); ep_element; ep_element = ep_element->NextSiblingElement())
                {
                    std::string showname = ep_element->FirstChild("show_name")->ToElement()->GetText();
                    TiXmlNode *ep_type_node = ep_element->FirstChild("show_type");
                    std::string showtype = "TV";
                    if (ep_type_node) {
                        showtype = ep_type_node->ToElement()->GetText();
                    }
                    //find the content, if not found, ignore.
                    ContentIterator citer;
                    if (false == SearchContentByName(showname, showtype, citer)) continue;

                    LOG4CPLUS_DEBUG(g_logger, "For content " << showname );
                    TiXmlElement *index_element = ep_element->FirstChild("show_index")->ToElement();
                    if (index_element) {
                        Episode ep;
                        ep.filename = ep_element->FirstChild("episode_name")->ToElement()->GetText();
                        ep.encode_name = ep_element->FirstChild("encode_name")->ToElement()->GetText();
                        ep.content_id = atoi(ep_element->FirstChild("episode_id")->ToElement()->GetText());
                        ep.eindex = 0;
                        TiXmlNode *ep_bitrate = ep_element->FirstChild("episode_bitrate");
                        if (ep_bitrate) {
                            ep.bitrate = atoi(ep_bitrate->ToElement()->GetText());
                        }else{
                            ep.bitrate = 0;
                        }
                        TiXmlNode *ep_bitrate_type = ep_element->FirstChild("bitrate_type");
                        if (ep_bitrate_type) {
                            ep.bitrate_type = atoi(ep_bitrate_type->ToElement()->GetText());
                        }else{
                            ep.bitrate_type = 0;
                            //this is ugly but deal with compatibility problems.
                            if (ep.bitrate > 600) ep.bitrate_type = 1;
                            if (ep.bitrate > 900) ep.bitrate_type = 2;
                            if (ep.bitrate > 1800 ) ep.bitrate_type = 3;
                            if (ep.bitrate < 200 ) ep.bitrate_type = 6;
                            if (ep.bitrate < 100 ) ep.bitrate_type = 7;
                        }

                        int32_t ep_index = atoi(index_element->GetText());
                        ep.timestamp = get_epoch_time();
                        if ((ep_index > 0) && showtype == "TV") {
                            ep.eindex = ep_index;
                        }else if (showtype == "Movie" || showtype == "VIP") {
                            //for movie and VIP, we use the EPG's content ID as episode ID.
                            ep.eindex = ep.content_id;
                        }
                        if (ep.eindex > 0) {
                            //here we are trying to load the RIDs
                            TiXmlElement *r_elements = ep_element->FirstChild("RIDs")->ToElement();
                            if (r_elements) {
                                TiXmlNode *r_node = r_elements->FirstChild("RID");
                                if (r_node) {
                                    for (TiXmlElement *r_element = r_node->ToElement(); r_element; r_element = r_element->NextSiblingElement() )
                                    {
                                        std::string rid(r_element->GetText());
                                        ep.rid_array.push_back(rid);
                                    }
                                }
                            }
                            //here we insert the episode into content.
                            LOG4CPLUS_DEBUG(g_logger, "  Added Episode " << ep.filename );
                            AddEpisode(citer->second, ep);
                        }
                    }
                }
                data_file.SaveFile();
            }else{
                LOG4CPLUS_ERROR(g_logger, "ERROR: loading new_upload content file failed, format error.");
                CONSOLE_LOG("ERROR: loading new_upload content file failed, format error.");
            }
        }else{ 
            LOG4CPLUS_WARN(g_logger, "File " << path << " not found, probably not generated yet.");
            //CONSOLE_LOG("File " << path << " not found, probably not generated yet." << std::endl);
            return;
        }
    }
    catch (boost::filesystem::basic_filesystem_error<boost::filesystem::path> &e)
    {
        LOG4CPLUS_WARN(g_logger, "Exception caught when loading new_upload content file from EPG script.");
        CONSOLE_LOG("Exception caught when loading new_upload content file from EPG script.");
        return;
    }
    BackupFile(m_new_upload_file, true);
}

void ContentManager::Load_New_VIP_RIDs(LoadMode lmode)
{
    using namespace tinyxml;

    boost::filesystem::path path(boost::filesystem::current_path().string());
    std::string m_this_file;
    if (lmode == LOAD_NEW_FILE) {
        m_this_file = m_new_viprid_file; 
    }else{
        m_this_file = m_local_viprid_file;
    }
    path /= m_this_file; 
    LOG4CPLUS_DEBUG(g_logger, "Load New VIP RIDs: " << path.string());
    try
    {
        if (boost::filesystem::exists(path))
        {
            TiXmlDocument data_file(path.string());
            if (data_file.LoadFile())
            {
                TiXmlElement *ridlist = data_file.FirstChild("rid_list")->ToElement();
                if (ridlist == NULL) {
                    LOG4CPLUS_WARN(g_logger, "cannot obtain RID_LIST handle, invalid or empty XML file");
                    CONSOLE_LOG("cannot obtain RID_LIST handle, invalid or empty XML file");
                    if (lmode == LOAD_NEW_FILE) {
                        BackupFile(m_this_file, true);
                    }
                    return;
                }
                TiXmlNode *rid_node = ridlist->FirstChild("RID");
                if (rid_node == NULL) {
                    LOG4CPLUS_WARN(g_logger, "cannot obtain RID handle, invalid or empty XML file");
                    CONSOLE_LOG("cannot obtain RID handle, invalid or empty XML file");
                    if (lmode == LOAD_NEW_FILE) { 
                        BackupFile(m_this_file, true);
                    }
                    return;
                }
                for (TiXmlElement *rid_element = rid_node->ToElement(); rid_element; rid_element = rid_element->NextSiblingElement())
                {
                    std::string ridname = rid_element->FirstChild("id")->ToElement()->GetText();
                    std::string ridfull = rid_element->FirstChild("fullrid")->ToElement()->GetText();
                    int rid_priority = atoi(rid_element->FirstChild("pri")->ToElement()->GetText());
                    uint32_t rid_content_id = atoi(rid_element->FirstChild("cid")->ToElement()->GetText());
                    std::string rid_encode_name = rid_element->FirstChild("ename")->ToElement()->GetText();
                    uint32_t rid_seg_index = atoi(rid_element->FirstChild("segno")->ToElement()->GetText());
                    uint32_t rid_push_target = atoi(rid_element->FirstChild("target")->ToElement()->GetText());
                    LOG4CPLUS_DEBUG(g_logger, " RID " << ridname << " target " << rid_push_target << " " << rid_content_id << " seg" << rid_seg_index);
                    RID_Internal_Struct internal_rid;
                    internal_rid.rid_string = ridfull;
                    internal_rid.rid_priority = rid_priority;
                    internal_rid.rid_push_target = rid_push_target;
                    RID_Tuple one_rid(rid_content_id, rid_seg_index, rid_encode_name, internal_rid);
                    if (lmode == LOAD_NEW_FILE) {
                        InsertVIPRID(ridname, one_rid);
                    }else{
                        InsertVIPRID(ridname, one_rid, 1);
                    }
                }
                data_file.SaveFile();
            }else{
                LOG4CPLUS_ERROR(g_logger, "ERROR: loading VIP RID content file failed, format error.");
                CONSOLE_LOG("ERROR: loading VIP RID content file failed, format error.");
            }
        }else{ 
            LOG4CPLUS_WARN(g_logger, "File " << path << " not found, probably not generated yet.");
            //CONSOLE_LOG("File " << path << " not found, probably not generated yet." << std::endl);
            return;
        }
    }
    catch (boost::filesystem::basic_filesystem_error<boost::filesystem::path> &e)
    {
        LOG4CPLUS_WARN(g_logger, "Exception caught when loading VIP RID content file from EPG script.");
        CONSOLE_LOG("Exception caught when loading VIP RID content file from EPG script.");
        return;
    }
    if (lmode == LOAD_NEW_FILE) {
        BackupFile(m_this_file, true);
    }
}

void ContentManager::Load_New_RID_Makeup()
{
    using namespace tinyxml;

    boost::filesystem::path path(boost::filesystem::current_path().string());
    std::string m_this_file;

    m_this_file = m_new_ridmakeup_file; 
    path /= m_this_file; 
    LOG4CPLUS_DEBUG(g_logger, "Load New VIP RIDs: " << path.string());
    m_hotrid_makeup_map.clear();
    try
    {
        if (boost::filesystem::exists(path))
        {
            TiXmlDocument data_file(path.string());
            if (data_file.LoadFile())
            {
                TiXmlElement *ridlist = data_file.FirstChild("hotrid_list")->ToElement();
                if (ridlist == NULL) {
                    LOG4CPLUS_WARN(g_logger, "cannot obtain HOTRID_LIST handle, invalid or empty XML file");
                    CONSOLE_LOG("cannot obtain HOTRID_LIST handle, invalid or empty XML file");
                    BackupFile(m_this_file, true);
                    return;
                }
                TiXmlNode *rid_node = ridlist->FirstChild("RID");
                if (rid_node == NULL) {
                    LOG4CPLUS_WARN(g_logger, "cannot obtain HOTRID handle, invalid or empty XML file");
                    CONSOLE_LOG("cannot obtain HOTRID handle, invalid or empty XML file");
                    BackupFile(m_this_file, true);
                    return;
                }
                for (TiXmlElement *rid_element = rid_node->ToElement(); rid_element; rid_element = rid_element->NextSiblingElement())
                {
                    std::string ridname = rid_element->FirstChild("id")->ToElement()->GetText();
                    int rid_push_target = atoi(rid_element->FirstChild("target")->ToElement()->GetText());
                    LOG4CPLUS_DEBUG(g_logger, " RID " << ridname << " target " << rid_push_target);
                    m_hotrid_makeup_map.insert(make_pair(ridname, rid_push_target));
                }
                data_file.SaveFile();
            }else{
                LOG4CPLUS_ERROR(g_logger, "ERROR: loading HOT RID content file failed, format error.");
                CONSOLE_LOG("ERROR: loading HOT RID content file failed, format error.");
            }
        }else{ 
            LOG4CPLUS_WARN(g_logger, "File " << path << " not found, probably not generated yet.");
            //CONSOLE_LOG("File " << path << " not found, probably not generated yet." << std::endl);
            return;
        }
    }
    catch (boost::filesystem::basic_filesystem_error<boost::filesystem::path> &e)
    {
        LOG4CPLUS_WARN(g_logger, "Exception caught when loading HOT RID content file from EPG script.");
        CONSOLE_LOG("Exception caught when loading HOT RID content file from EPG script.");
        return;
    }
    BackupFile(m_this_file, true);
}

void ContentManager::ProcessHotRIDMakeup()
{
    //we iterate through the CONTENT/EP/RID list, try to see whether RID need to be further pushed or cancelled.
    EpisodesIterator eiter;
    for (ContentIterator citer = m_content_map.begin(); citer != m_content_map.end(); citer ++ ) {
        for (eiter = citer->second.episodes.begin(); eiter != citer->second.episodes.end(); eiter ++) {
            //finally we are into RID.
            std::string rid_short;
            for (int rindex = 0; rindex < eiter->rid_array.size(); rindex ++) {
                bool ridgood = GetRidShortString(eiter->rid_array[rindex].rid_string, rid_short);
                if (ridgood == false) {
                    LOG4CPLUS_DEBUG(g_logger, " RID extraction failed " << eiter->filename << ":" << rindex);
                    continue;
                }
                std::map<std::string, int>::iterator hotriditer = m_hotrid_makeup_map.find(rid_short);
                if (hotriditer != m_hotrid_makeup_map.end()) {
                    LOG4CPLUS_DEBUG(g_logger, "RID " << rid_short << " target=" << hotriditer->second);
                    int current_push_count = MIN(eiter->rid_array[rindex].rid_actpush_count, m_config.CONTENT_ACTPUSH_PER_RID_LIMIT);
                    LOG4CPLUS_DEBUG(g_logger, " current=" << current_push_count << " episode=" << eiter->total_active_pushed);
                    if (hotriditer->second == 0) {
                        //we would like to cancel this rid.
                        if (current_push_count < m_config.CONTENT_ACTPUSH_PER_RID_LIMIT) {
                            int makeup_count = m_config.CONTENT_ACTPUSH_PER_RID_LIMIT - current_push_count;
                            LOG4CPLUS_DEBUG(g_logger, "cancel " << rid_short << " " << current_push_count << "->" << m_config.CONTENT_ACTPUSH_PER_RID_LIMIT);
                            LOG4CPLUS_INFO(g_statlog, "HREM " << rid_short << " " << current_push_count);
                            const_cast<Episode&>(*eiter).rid_array[rindex].rid_actpush_count = m_config.CONTENT_ACTPUSH_PER_RID_LIMIT;
                            const_cast<Episode&>(*eiter).total_active_pushed += makeup_count;
                            LOG4CPLUS_DEBUG(g_logger, " updated=" << eiter->rid_array[rindex].rid_actpush_count << " episode=" << eiter->total_active_pushed);
                        }else{
                            LOG4CPLUS_DEBUG(g_logger, "RID " << rid_short << " already fully pushed.");
                        }
                    }else{
                        //now we need to add stuff.
                        if (current_push_count + hotriditer->second > m_config.CONTENT_ACTPUSH_PER_RID_LIMIT) {
                           LOG4CPLUS_INFO(g_statlog, "HADD " << rid_short << " " << current_push_count << "+" << hotriditer->second);
                            int new_count = m_config.CONTENT_ACTPUSH_PER_RID_LIMIT - hotriditer->second;
                            if (new_count < 0) {
                                new_count = 0;
                            }
                            int makeup_count = current_push_count - new_count;
                            const_cast<Episode&>(*eiter).rid_array[rindex].rid_actpush_count = new_count;
                            if (const_cast<Episode&>(*eiter).total_active_pushed >= makeup_count) {
                                const_cast<Episode&>(*eiter).total_active_pushed -= makeup_count;
                            }else{
                                const_cast<Episode&>(*eiter).total_active_pushed = 0;
                            }
                            LOG4CPLUS_DEBUG(g_logger, " updated=" << eiter->rid_array[rindex].rid_actpush_count << " episode=" << eiter->total_active_pushed);
                        }else{
                            LOG4CPLUS_DEBUG(g_logger, "RID " << rid_short << " is still under-pushed.");
                        }
                    }
                }
            }
        }
    }

}

bool ContentManager::InsertVIPRID(const std::string &ridname, const RID_Tuple &one_rid, const int inserthead)
{
    LOG4CPLUS_DEBUG(g_logger, "Insert RID " << ridname << " mode " << inserthead);
    if (inserthead == 1) {
        //we are sure there are no duplicate, insert directly.
        m_viprid_data.insert(make_pair(ridname, one_rid));
        return true;
    }

    VIPRIDIterator riter = m_viprid_data.find(ridname);
    bool inserted = false;

    if (riter == m_viprid_data.end() ) {
        //content does not exist, we will insert one.
        m_viprid_data.insert(make_pair(ridname, one_rid));
        inserted = true;
    }else{
        //we only need to update the target time, priority
        LOG4CPLUS_DEBUG(g_logger, " Dup " << one_rid.rid_full.rid_push_target);
        riter->second.rid_full.rid_priority = one_rid.rid_full.rid_priority;
        //riter->second.rid_full.rid_push_target = one_rid.rid_full.rid_push_target;
    }
    return inserted;
}

bool ContentManager::SearchContentByName(const std::string &cname, const std::string &ctype, ContentIterator &citer)
{    
    //we use this function for future extension.
    if (ctype == "VIP") {
        citer = m_content_map.find(ctype+ctype);
    }else{
        citer = m_content_map.find(cname+ctype);
    }
    if (citer == m_content_map.end()) {
        return false;
    }
    return true;

}

int ContentManager::GetPushRIDs(PeerInfo &peer_info, const std::vector<protocol::PlayHistoryItem> &play_hist_vec, std::vector<protocol::PushTaskItem>& ptask_list)
{
    LOG4CPLUS_DEBUG(g_logger, "Enter: GetPushRIDs");

    if (m_config.CONTENT_PUSH_DISABLED != 0) {
        LOG4CPLUS_DEBUG(g_logger, "Exit: Push Disabled.");
        return PUSH_NO_TASK;
    }

    if ((m_config.CONTENT_PAUSE_DISABLED == 0) && (true == m_scheduler.IsBusyTime()) ) {
        LOG4CPLUS_DEBUG(g_logger, "Exit: GetPushRIDs - busy time");
        return PUSH_NO_TASK;
    }

    int ridcnt = PUSH_NO_TASK;
    if (PUSH_NO_TASK == GetPassivePushRIDs(peer_info, play_hist_vec, ptask_list) ) {
        if (m_config.CONTENT_ACTPUSH_ENABLED == 1) {
            ridcnt = GetActivePushRIDs(peer_info, ptask_list);
            if (ridcnt != PUSH_NO_TASK) {
                LOG4CPLUS_DEBUG(g_logger, "Exit: GetPushRIDs - with active push.");
                return ridcnt;
            }
        }
        //now we either have active push disabled, or active push returns no task.
        if (m_config.CONTENT_VIPPUSH_ENABLED == 1) {
            return GetVIPPushRIDs(peer_info, ptask_list);
        } else{
            LOG4CPLUS_DEBUG(g_logger, "Exit: GetPushRIDs - no task or vip push disabled.");
            return PUSH_NO_TASK;
        }
    }

    LOG4CPLUS_DEBUG(g_logger, "Exit: GetPushRIDs - with pas task");
    return PUSH_HAS_TASK;
}

int ContentManager::GetPassivePushRIDs(PeerInfo &peer_info, const std::vector<protocol::PlayHistoryItem> &play_hist_vec, std::vector<protocol::PushTaskItem>& ptask_list)
{
    LOG4CPLUS_DEBUG(g_logger, "Enter: GetPassivePushRIDs");
    if (false == m_scheduler.HasQuota(m_config.CONTENT_PUSHAHEAD_NUM)) {
        LOG4CPLUS_DEBUG(g_logger, "We are out of quota for this request.");
        LOG4CPLUS_DEBUG(g_logger, "Exit1: GetPassivePushRIDs");
        return PUSH_NO_TASK;
    }

    //we need to merge the playhistoryitem.
    //std::vector<protocol::PlayHistoryItem> play_hist_vec = pplay_hist_vec;
    
    std::map<std::string, PlayHistory_Index> hist_map;
    for (std::vector<protocol::PlayHistoryItem>::const_iterator iter = play_hist_vec.begin(); iter != play_hist_vec.end(); iter++) {

        PlayHistory_Index hist_i;
        bool re = FilterShowName(iter->video_name_, hist_i);
        if (false == re) {
            LOG4CPLUS_DEBUG(g_logger, "Pattern matching for " << iter->video_name_ << " failed. Skip");
            continue;
        }
        LOG4CPLUS_DEBUG(g_logger, "History Item: " << hist_i.show_name << " " << hist_i.dw_segment_num << " " << hist_i.played_duration << " ft=" << hist_i.bitrate_type);
        hist_i.dw_segment_num = iter->downloaded_segment_num_;
        hist_i.played_duration = iter->continuously_played_duration_;
        //now we will check against our hot_content_map, if it is not in there, we will simply discard it.
        ContentIterator c_it;
        if (false == SearchContentByName(hist_i.show_name, "TV", c_it)) {
            //from user history, we only push TV-type contents.
            continue;
        }
        std::map<std::string, PlayHistory_Index>::iterator map_it = hist_map.find(hist_i.show_name);
        if (map_it == hist_map.end()) 
        {
            hist_map.insert(make_pair(hist_i.show_name, hist_i));
        }
        else
        {
            //now we have duplicate, so we need to add things up, and replace latest episode record.
            if ( (hist_i.ep_index > map_it->second.ep_index) ||
                ( (hist_i.ep_index == map_it->second.ep_index) && (hist_i.seg_index > map_it->second.seg_index) ))
            {
                map_it->second.ep_index = hist_i.ep_index;
                map_it->second.seg_index = hist_i.seg_index;
                map_it->second.bitrate_type = hist_i.bitrate_type;
                map_it->second.dw_segment_num = hist_i.dw_segment_num;
            }
            map_it->second.played_duration += hist_i.played_duration;
        }
    }
    //We need to create the PushTaskItem.
    int total_task_num = 0;
    for (std::map<std::string, PlayHistory_Index>::iterator iter = hist_map.begin(); iter != hist_map.end(); iter ++ ) {
        std::vector<RID_Tuple> m_rids;
        ContentIterator c_iter;
        bool found = SearchContentByName(iter->first, "TV", c_iter);  //we are sure we will find it.
        //for Passive Push, we only push TV-like contents.
        if (false == found) {
            continue;
        }
        int m_pre_quota = GetNumPushRIDBasedOnHistory(iter->second);
        if (m_pre_quota <= 0) {
            LOG4CPLUS_DEBUG(g_logger, "No next RID found for " << iter->second.show_name << iter->second.ep_index);
            continue;
        }
        int m_startsegment = iter->second.seg_index;
        if ( iter->second.dw_segment_num > 0) {
            //this means the current segment is already finished for the client.
            m_startsegment ++;
        }else{
            //if the client hasn't finished the current segment, we increase the segment by 1, allowing the peer to finish the current one.
            m_pre_quota ++;
        }
        int m_count = CreatePushRIDs(c_iter->second, iter->second.ep_index, iter->second.bitrate_type, m_startsegment, m_pre_quota, m_rids);
        if (m_count > 0) {
            LOG4CPLUS_DEBUG(g_logger, "Search " << m_count << " RID for " << iter->second.show_name << iter->second.ep_index);
            int m_quota =  m_scheduler.QuerySchedulerForQuota(m_count);
            if (m_quota > 0) {
                int result = CreateTaskItemFromRID(m_rids, m_quota, ptask_list);
                total_task_num += result;
            }else{
                //we are out of quota, break the loop.
                LOG4CPLUS_DEBUG(g_logger, "We are out of RID quota, break");
                break;
            }
        }
    }

    LOG4CPLUS_DEBUG(g_logger, "Created " << total_task_num << " push task for peer " << peer_info.guid);
    if (total_task_num > 0) {
        LOG4CPLUS_DEBUG(g_logger, "Exit2: GetPassivePushRIDs");
        return PUSH_HAS_TASK;
    }

    LOG4CPLUS_DEBUG(g_logger, "Exit3: GetPassivePushRIDs");
    return PUSH_NO_TASK;
}

int ContentManager::GetActivePushRIDs(PeerInfo &peer_info, std::vector<protocol::PushTaskItem>& ptask_list)
{
    LOG4CPLUS_DEBUG(g_logger, "Enter: GetActivePushRIDs");
    LOG4CPLUS_DEBUG(g_logger, "actpush_episode_index =" << actpush_episode_index << "actpush_rid_index = " << actpush_rid_index << " actpush_bitrate_type = " << actpush_episode_bitrate_type);
    if (false == m_scheduler.HasActiveQuota(m_config.CONTENT_ACTPUSH_PER_REQ_LIMIT)) {
        LOG4CPLUS_DEBUG(g_logger, "no active quota.");
        LOG4CPLUS_DEBUG(g_logger, "Exit1: GetActivePushRIDs");
        return PUSH_NO_TASK;
    }

    if (actpush_content_iter == m_content_map.end()) {
        LOG4CPLUS_DEBUG(g_logger, "no active task.");
        LOG4CPLUS_DEBUG(g_logger, "Exit2: GetActivePushRIDs");
        return PUSH_NO_TASK;
    }

    if ((actpush_low_bw_done == true) && IsPeerBWLow(peer_info)){
        LOG4CPLUS_DEBUG(g_logger, "no active task for low bandwidth node.");
        LOG4CPLUS_DEBUG(g_logger, "Exit3a: GetActivePushRIDs");
        return PUSH_NO_TASK;
    }

    if ((actpush_high_disk_done == true) && IsPeerDiskHigh(peer_info)){
        LOG4CPLUS_DEBUG(g_logger, "no active task for disk high node.");
        LOG4CPLUS_DEBUG(g_logger, "Exit3b: GetActivePushRIDs");
        return PUSH_NO_TASK;
    }

    if ((actpush_low_online_done == true) && IsPeerOnlineLow(peer_info)){
        LOG4CPLUS_DEBUG(g_logger, "no active task for low online node.");
        LOG4CPLUS_DEBUG(g_logger, "Exit3c: GetActivePushRIDs");
        return PUSH_NO_TASK;
    }

    if (!IsPeerNATGood(peer_info) ) {
        LOG4CPLUS_DEBUG(g_logger, "no active task for bad NAT node.");
        LOG4CPLUS_DEBUG(g_logger, "Exit3d: GetActivePushRIDs");
        return PUSH_NO_TASK;
    }

    LOG4CPLUS_DEBUG(g_logger, "PInfo LowBW=" << IsPeerBWLow(peer_info) << " HighDisk=" << IsPeerDiskHigh(peer_info) << " LowOnline=" << IsPeerOnlineLow(peer_info));

    LOG4CPLUS_DEBUG(g_logger, "actpush_content_iter" << actpush_content_iter->first);
    //We need to create the PushTaskItem.
    int total_task_num = 0;
    int total_iter_content = m_content_map.size();
    while (actpush_content_iter != m_content_map.end()) {
        std::vector<RID_Tuple> m_rids;
        int m_count = CreateActivePushRIDs(actpush_content_iter->second, peer_info, actpush_episode_index, actpush_episode_bitrate_type, actpush_rid_index, m_config.CONTENT_ACTPUSH_PER_REQ_LIMIT, m_rids);
        if (m_count > 0) {
            LOG4CPLUS_DEBUG(g_logger, "Active " << m_count << " RID for " << actpush_content_iter->second.name << actpush_episode_index << " bitrate_type " << actpush_episode_bitrate_type);
            int m_quota =  m_scheduler.QuerySchedulerForActiveQuota(m_config.CONTENT_ACTPUSH_PER_REQ_LIMIT);
            if (m_quota > 0) {
                int result = CreateTaskItemFromRID(m_rids, m_quota, ptask_list);
                total_task_num += result;
                //here we don't want to go beyond to the next content to create the full allowed quota of RIDS.
                LOG4CPLUS_DEBUG(g_logger, "total_task_num now is " << total_task_num);
                break;
            }else{
                //we are out of quota, break the loop.
                LOG4CPLUS_DEBUG(g_logger, "We are out of RID quota, break");
                break;
            }
        }else{
            //we should go to next content.
            actpush_content_iter ++;
            if (actpush_content_iter == m_content_map.end()) {
                actpush_content_iter = m_content_map.begin();
            }
            actpush_episode_index = 0;
            actpush_rid_index = 0;
            actpush_episode_bitrate_type = 0;
            total_iter_content --;
            LOG4CPLUS_DEBUG(g_logger, "itercount " << total_iter_content << " actpush_content_iter:"<< actpush_content_iter->first << " actpush_episode_index =" << actpush_episode_index << " actpush_rid_index = " << actpush_rid_index << " bitrate_type = " << actpush_episode_bitrate_type);
            //we allow first visited content to be visited again (total_iter_content+1 iterations)
            // this is because it is possible we start from the second half of that content.
            if (total_iter_content < 0) break;
        }
    }
    if (total_iter_content < 0) {
        LOG4CPLUS_DEBUG(g_logger, "Active Push has no tasks. Stops.");
        if (IsPeerBWLow(peer_info) && !IsPeerDiskHigh(peer_info) && !IsPeerOnlineLow(peer_info)) {
            //this is a low bandwidth node, and we did not find match for him.
            //but we cannot simply turn LOWBW complete off, because out-of-quota for low bandwidth peers could be temporary.
            //now we allow the low bandwidth quota to go beyond the ratio of actual total pushed amount (we use the overall amount ratio as limit). 
            // so this is ok now.
            LOG4CPLUS_DEBUG(g_logger, "Active Push exausted for low bw peers.");
            actpush_low_bw_done = true;
        }
        if (!IsPeerBWLow(peer_info) && IsPeerDiskHigh(peer_info) && !IsPeerOnlineLow(peer_info)) {
            LOG4CPLUS_DEBUG(g_logger, "Active Push exausted for high disk peers.");
            actpush_high_disk_done = true;
        }
        if (!IsPeerBWLow(peer_info) && !IsPeerDiskHigh(peer_info) && IsPeerOnlineLow(peer_info)) {
            LOG4CPLUS_DEBUG(g_logger, "Active Push exausted for low online peers.");
            actpush_low_online_done = true;
        }
        if (!IsPeerBWLow(peer_info) && !IsPeerDiskHigh(peer_info) && !IsPeerOnlineLow(peer_info)){
            actpush_content_iter = m_content_map.end();
        }
    }

    LOG4CPLUS_DEBUG(g_logger, "Created " << total_task_num << " active push task for peer " << peer_info.guid);
    if (total_task_num > 0) {
        LOG4CPLUS_DEBUG(g_logger, "actpush_content_iter:"<< actpush_content_iter->first << " actpush_episode_index =" << actpush_episode_index << " actpush_rid_index = " << actpush_rid_index << " bitrate_type = " << actpush_episode_bitrate_type);
        LOG4CPLUS_DEBUG(g_logger, "Exit4: GetActivePushRIDs");
        return PUSH_HAS_ACT_TASK;
    }

    if (actpush_content_iter == m_content_map.end()) {
       LOG4CPLUS_DEBUG(g_logger, "actpush_content_iter set to end, no more active task");
    }else{
       LOG4CPLUS_DEBUG(g_logger, "actpush_content_iter:"<< actpush_content_iter->first << " actpush_episode_index =" << actpush_episode_index << " actpush_rid_index = " << actpush_rid_index << " bitrate_type = " << actpush_episode_bitrate_type);
    }
    LOG4CPLUS_DEBUG(g_logger, "Exit5: GetActivePushRIDs");
    return PUSH_NO_TASK;
}

int ContentManager::GetVIPPushRIDs(PeerInfo &peer_info, std::vector<protocol::PushTaskItem>& ptask_list)
{
    LOG4CPLUS_DEBUG(g_logger, "Enter: GetVIPPushRIDs");
    if (false == m_scheduler.HasActiveQuota(m_config.CONTENT_ACTPUSH_PER_REQ_LIMIT)) {
        LOG4CPLUS_DEBUG(g_logger, "no active quota.");
        LOG4CPLUS_DEBUG(g_logger, "Exit1: GetVIPPushRIDs");
        return PUSH_NO_TASK;
    }

    if (vippush_rid_iter == m_viprid_data.end()) {
        LOG4CPLUS_DEBUG(g_logger, "no vip task.");
        LOG4CPLUS_DEBUG(g_logger, "Exit2: GetVIPPushRIDs");
        return PUSH_NO_TASK;
    }

    if ((vippush_low_bw_done == true) && IsPeerBWLow(peer_info)){
        LOG4CPLUS_DEBUG(g_logger, "no vip task for low bandwidth node.");
        LOG4CPLUS_DEBUG(g_logger, "Exit3: GetVIPPushRIDs");
        return PUSH_NO_TASK;
    }

    LOG4CPLUS_DEBUG(g_logger, "vippush_rid_iter" << vippush_rid_iter->first);
    //We need to create the PushTaskItem.
    int total_full_rid = 0;
    int total_task_num = 0;
    int total_iter_rid = m_viprid_data.size();
    while (vippush_rid_iter != m_viprid_data.end()) {
        std::vector<RID_Tuple> m_rids;
        int m_count = CreateVIPPushRID(vippush_rid_iter->second, peer_info, m_rids, total_full_rid);
        if (m_count > 0) {
            LOG4CPLUS_DEBUG(g_logger, "VIP " << m_count << " RID " << vippush_rid_iter->first);
            int m_quota =  m_scheduler.QuerySchedulerForActiveQuota(m_count, 1);
            if (m_quota > 0) {
                int result = CreateTaskItemFromRID(m_rids, m_quota, ptask_list);
                total_task_num += result;
            }else{
                //we are out of quota, break the loop.
                LOG4CPLUS_DEBUG(g_logger, "We are out of RID quota, break");
                break;
            }
        }
        //we should go to next RID.
        vippush_rid_iter ++;
        if (vippush_rid_iter == m_viprid_data.end()) {
            vippush_rid_iter = m_viprid_data.begin();
        }
        total_iter_rid --;
        LOG4CPLUS_DEBUG(g_logger, "itercount " << total_iter_rid << " vippush_rid_iter:"<< vippush_rid_iter->first );
        //we allow first visited RID to be visited again (total_iter_content+1 iterations)
        // this is because it is possible we start from the second half of that RID.
        if (total_iter_rid < 0) break;
        if (total_task_num >= m_config.CONTENT_ACTPUSH_PER_REQ_LIMIT) {
            break;
        }

    }
    if (total_iter_rid < 0) {
        LOG4CPLUS_DEBUG(g_logger, "VIP Push has no tasks. Stops.");
        if (IsPeerBWLow(peer_info)) {
            //this is a low bandwidth node, and we did not find match for him.
            //but we cannot simply turn LOWBW complete off, because out-of-quota for low bandwidth peers could be temporary.
            //now we allow the low bandwidth quota to go beyond the ratio of actual total pushed amount (we use the overall amount ratio as limit). 
            // so this is ok now.
            vippush_low_bw_done = true;
        }else{
            LOG4CPLUS_INFO(g_logger, "VIP push exausted, retire triggered.");
            RetireVIPRIDs(false);
            vippush_rid_iter = m_viprid_data.end();
        }
    }

    //if the RID list has too many full RIDs, we will trigger a retirement.
    if ( (total_full_rid > m_config.CONTENT_VIPPUSH_RETIRE_THRESHOLD) && (total_full_rid > (m_viprid_data.size()/5)) ) 
    {
        std::string tmp_rid_str = vippush_rid_iter->first;
        LOG4CPLUS_INFO(g_logger, "VIP push scarse, retire triggered: " << total_full_rid << " rid full out of " << m_viprid_data.size());
        RetireVIPRIDs(false);
        LOG4CPLUS_INFO(g_logger, "VIP push size " << m_viprid_data.size());
        vippush_rid_iter = m_viprid_data.find(tmp_rid_str);
        if (vippush_rid_iter == m_viprid_data.end()) {
            LOG4CPLUS_INFO(g_logger, "RID " << tmp_rid_str << " cannot be found, goes to first.");
            vippush_rid_iter = m_viprid_data.begin();
        }
    }

    LOG4CPLUS_DEBUG(g_logger, "Created " << total_task_num << " VIP push task for peer " << peer_info.guid);
    if (total_task_num > 0) {
        if (vippush_rid_iter != m_viprid_data.end()) {
            LOG4CPLUS_DEBUG(g_logger, "vippush_rid_iter:"<< vippush_rid_iter->first);
        }    
        LOG4CPLUS_DEBUG(g_logger, "Exit4: GetVIPPushRIDs");
        return PUSH_HAS_ACT_TASK;
    }

    if (vippush_rid_iter == m_viprid_data.end()) {
        LOG4CPLUS_DEBUG(g_logger, "vippush_rid_iter set to end, no more VIP task");
    }else{
        LOG4CPLUS_DEBUG(g_logger, "vippush_rid_iter:"<< vippush_rid_iter->first);
    }
    LOG4CPLUS_DEBUG(g_logger, "Exit5: GetVIPPushRIDs");
    return PUSH_NO_TASK;
}


int ContentManager::CreateTaskItemFromRID(std::vector<RID_Tuple> &m_rids, const int p_quota, std::vector<protocol::PushTaskItem>& ptask_list)
{
    int m_quota = p_quota;
    for (std::vector<RID_Tuple>::iterator v_iter = m_rids.begin(); (v_iter != m_rids.end()) && (m_quota > 0); v_iter++) {
        //convert RID into PushTaskItem.
        protocol::PushTaskItem mtask;
        mtask.rid_info_ = FetchRidInfo(v_iter->rid_full.rid_string);
        if (mtask.rid_info_.file_length_ == 0) {
            //this is an invalid RID.
            LOG4CPLUS_WARN(g_logger, "  Inavlid RID " << mtask.rid_info_.rid_);
            continue;
        }
        LOG4CPLUS_DEBUG(g_logger, "  RID " << mtask.rid_info_.rid_);
        CreateURL(*v_iter, mtask.url_);
        //OMG.
        std::stringstream ss;
        ss << v_iter->content_id;
        ss >> mtask.channel_id_;
        ptask_list.push_back(mtask);
        m_quota --;
    }
    return (p_quota - m_quota);
}

bool ContentManager::IsRIDReadyForPeerActPush(const PeerInfo &pinfo, const RID_Internal_Struct &p_rid)
{
    //this rid is not fully pushed.
    LOG4CPLUS_DEBUG(g_logger, " RIDInfo " << p_rid.rid_actpush_count << "::" << p_rid.rid_actpush_low_bw_cnt << ":" << p_rid.rid_actpush_high_disk_cnt << ":" << p_rid.rid_actpush_low_online_cnt);
    
    if (p_rid.rid_actpush_count >= m_config.CONTENT_ACTPUSH_PER_RID_LIMIT) 
        return false;
    
    //this is for LOWBW peer, 
    if ( IsPeerBWLow(pinfo) &&
         (p_rid.rid_actpush_low_bw_cnt >= 
             m_config.CONTENT_ACTPUSH_PER_RID_LIMIT*(100-m_config.CONTENT_ACTPUSH_BIG_BW_RATIO)/100) 
       ) {
        return false;
    }

    //this is for HIGH DISK peer, 
    if ( IsPeerDiskHigh(pinfo) &&
        (p_rid.rid_actpush_high_disk_cnt >= 
        m_config.CONTENT_ACTPUSH_PER_RID_LIMIT*(100-m_config.CONTENT_ACTPUSH_BIG_DISK_RATIO)/100) 
        ) {
            return false;
    }

    //this is for LOW Online peer, 
    if ( IsPeerOnlineLow(pinfo) &&
        (p_rid.rid_actpush_low_online_cnt >= 
        m_config.CONTENT_ACTPUSH_PER_RID_LIMIT*(100-m_config.CONTENT_ACTPUSH_HIGH_ONLINE_RATIO)/100) 
        ) {
            return false;
    }

    //so, what we have now is the following conditions:
    // 1) actpush not reach limit.
    // 2) peer's disk space is mostly free.
    // 3) peer is high bw peer, or low bw peer we haven't reach low bw peer limit.
    return true;
}

//Core: locate a number of RIDs to be pushed. 
//Return: RNUM of RIDs found, from episode eindex, block ridindex. 
int ContentManager::CreatePushRIDs(Content &ct, const int eindex, const uint16_t bitrate_type,
                                   const int ridindex, const int rnum, std::vector<RID_Tuple> &p_rids)
{
    int rid_count = rnum;
    int index = ridindex;

    Episode ep_empty;
    ep_empty.eindex = eindex;
    ep_empty.bitrate_type = bitrate_type;
    uint32_t prev_eindex = 0;
    for (EpisodesIterator eiter = ct.episodes.lower_bound(ep_empty); eiter != ct.episodes.end(); eiter ++) {
        //we find the right index, one that is current or newer.
        //we decide whether this is a tv series.
        if (eiter->eindex != eindex) {
            //we have found the next episode, now we check whether the series are continuous.
            if (prev_eindex == 0) prev_eindex = eindex;
            if ( (eiter->eindex < 10000) && (eiter->eindex != (prev_eindex+1)) ) {
                LOG4CPLUS_DEBUG(g_logger, " Episode index is not continious, skip: " << prev_eindex << " + 1 != " << eiter->eindex);
                break;
            }
            //since this lower_bound one is newer, (eiter->eindex != eindex)
            // we need to set the RID "index" to the beginning of the new episode.
            index = 0;
        }

        //here, the eiter->eindex is either the same or continuous, 
        // so we start to check bitrate_type, 
        if (eiter->bitrate_type != bitrate_type) {
            LOG4CPLUS_DEBUG(g_logger, " bitrate type mismatch, ep->bitrate_type " << eiter->bitrate_type << " needed " << bitrate_type);
            continue;
        }

        //if the next RID has already reached its push limit, skip.
        if ( (index < eiter->rid_array.size()) && 
             (eiter->rid_array[index].rid_paspush_count >= m_config.CONTENT_PASPUSH_PER_RID_LIMIT) ) {
            LOG4CPLUS_DEBUG(g_logger, " rid limit reached: index=" << index);
            break;
        }

        while ( (rid_count > 0)&&(index < eiter->rid_array.size()) ) {
            RID_Tuple rid_tp(eiter->content_id, index, eiter->encode_name, eiter->rid_array[index]);
            p_rids.push_back(rid_tp);
            const_cast<Episode&>(*eiter).rid_array[index].rid_paspush_count ++;
            index ++; 
            rid_count --;
        }
        if (rid_count <= 0) break;
        index = 0;
        prev_eindex = eiter->eindex;
    }
    return (rnum - rid_count);

}

//Here will only search within this content, we will not go beyond to the next content.
int ContentManager::CreateActivePushRIDs(Content &ct, const PeerInfo &pinfo, uint32_t &eindex, int32_t &bitrate_type_index, uint32_t &ridindex, const int rnum, std::vector<RID_Tuple> &p_rids)
{
    LOG4CPLUS_DEBUG(g_logger, "Enter: CreateActivePushRIDs");
    LOG4CPLUS_DEBUG(g_logger, "actpush_content_iter ++:"<< actpush_content_iter->first << " actpush_episode_index =" << actpush_episode_index << " actpush_rid_index = " << actpush_rid_index);
    uint32_t rid_count = rnum;
    uint32_t index = ridindex;
    int32_t bitrate_type = bitrate_type_index;

    Episode ep_empty;
    ep_empty.eindex = eindex;
    ep_empty.bitrate_type = bitrate_type;
    EpisodesIterator eiter;
    LOG4CPLUS_DEBUG(g_logger, "eindex = " << eindex << " ridindex = " << index );
    for ( eiter = ct.episodes.lower_bound(ep_empty); eiter != ct.episodes.end(); eiter ++) {
        //if the found one is newer, we need to reset the index.
        LOG4CPLUS_DEBUG(g_logger, "eiter filename: " <<eiter->filename);
        if ( (eiter->eindex != eindex) || 
             ((eiter->eindex == eindex)&&(eiter->bitrate_type != bitrate_type))) {
            index = 0;
        }
        if (!IsEpisodeActivePushFull(*eiter)) {
            //now we check we whether can share this content.
            while ( (rid_count > 0)&&(index < eiter->rid_array.size()) ) {
                // this RID is good enough?
                if (IsRIDReadyForPeerActPush(pinfo, eiter->rid_array[index])) {
                    RID_Tuple rid_tp(eiter->content_id, index, eiter->encode_name, eiter->rid_array[index]);
                    p_rids.push_back(rid_tp);
                    IncrementRIDActPushCount(pinfo, const_cast<Episode&>(*eiter).rid_array[index]);
                    const_cast<Episode&>(*eiter).total_active_pushed ++;
                    rid_count --;
                }
                index ++;
                LOG4CPLUS_DEBUG(g_logger, "index = " << index << " bitrate_type = " << eiter->bitrate_type);
            }
        }
        if (rid_count <= 0) break;
        index = 0;
        LOG4CPLUS_DEBUG(g_logger, "index = " << index << " bitrate_type = " << bitrate_type);
    }
    if (eiter != ct.episodes.end()) {
        eindex = eiter->eindex;
        ridindex = index;
        bitrate_type_index = eiter->bitrate_type;
        LOG4CPLUS_DEBUG(g_logger, "eindex = " << eindex << " ridindex = " << index << " bitrate_type = " << bitrate_type_index);
    }else{
        eindex = 0;
        ridindex = 0;
        bitrate_type_index = 0;
        LOG4CPLUS_DEBUG(g_logger, "eindex = " << eindex << " ridindex = " << index << " bitrate_type = " << bitrate_type_index);
    }

    LOG4CPLUS_DEBUG(g_logger, "actpush_content_iter ++:"<< actpush_content_iter->first << " actpush_episode_index =" << actpush_episode_index << " actpush_rid_index = " << actpush_rid_index << " bitrate_type = " << actpush_episode_bitrate_type);
    LOG4CPLUS_DEBUG(g_logger, "Exit: CreateActivePushRIDs");
    return (rnum - rid_count);
}

int ContentManager::CreateVIPPushRID(RID_Tuple &one_rid, const PeerInfo &pinfo, std::vector<RID_Tuple> &p_rids, int &total_full_rid)
{
    int rcount = 0;
    LOG4CPLUS_DEBUG(g_logger, " check rid " << one_rid.rid_full.rid_push_target << " " << one_rid.rid_full.rid_actpush_count << " " << one_rid.rid_full.rid_actpush_low_bw_cnt << " " << one_rid.rid_full.rid_actpush_high_disk_cnt << " " << one_rid.rid_full.rid_actpush_low_online_cnt);
    if (one_rid.rid_full.rid_actpush_count < one_rid.rid_full.rid_push_target) {
        if ( IsRIDReadyForPeerActPush(pinfo, one_rid.rid_full) ) { 
            p_rids.push_back(one_rid);
            IncrementRIDActPushCount(pinfo, one_rid.rid_full);
            rcount ++;
        }
    }else{
        total_full_rid ++;
    }
    return rcount;
}

void ContentManager::IncrementRIDActPushCount(const PeerInfo &pinfo, RID_Internal_Struct &p_rid)
{
    p_rid.rid_actpush_count ++;
    if (IsPeerBWLow(pinfo)) {
        p_rid.rid_actpush_low_bw_cnt ++;
    }
    if (IsPeerOnlineLow(pinfo)) {
        p_rid.rid_actpush_low_online_cnt ++;
    }
    if (IsPeerDiskHigh(pinfo)) {
        p_rid.rid_actpush_high_disk_cnt ++;
    }
}

int ContentManager::GetNumPushRIDBasedOnHistory(const PlayHistory_Index &p_hist)
{
    //we decide how many RIDS we should push for this content.
    if ( p_hist.played_duration >= m_config.CONTENT_PUSH_THRESH_DURATION ) {
        if (p_hist.dw_segment_num >= m_config.CONTENT_PUSH_THRESH_RNUM) {
            return 0;
        }
        return m_config.CONTENT_PUSHAHEAD_NUM;
    }
    return 0;
}

void ContentManager::CreateURL(const RID_Tuple &r_tp, std::string &p_string)
{
    std::stringstream ss;

    //ss << m_config.CONTENT_PUSH_BASE_URL << r_tp.seg_index << "/" << r_tp.encode_name << "%5B" << r_tp.seg_index << "%5D.mp4";
    ss << m_config.CONTENT_PUSH_BASE_URL << r_tp.seg_index << "/" << r_tp.encode_name << ".mp4";

    if (boost::algorithm::icontains(ss.str(), "?")) {
        ss << "&key=" + m_per_minute_key << m_config.CONTENT_PUSH_BASE_APPENDIX;
    }else{
        ss << "?key=" + m_per_minute_key << m_config.CONTENT_PUSH_BASE_APPENDIX;
    }
    ss >> p_string;
}

int ContentManager::AddContentByName(const std::string &showname, const std::string &showtype, const int eindex)
{
    ContentIterator citer;

    if (false == SearchContentByName(showname, showtype, citer) ) {
        //content does not exist, we will insert one.
        Content m_content;
        m_content.internal_id = m_next_internal_id ++;
        m_content.name = showname;
        m_content.content_type = showtype;
        m_content.last_seen = get_epoch_time();
        m_content.seen_count = 1;
        m_content.last_category_id = 0;
        AddContentToMap(m_content);
    }else{
        //we only need to update the last_seen time
        citer->second.last_seen = get_epoch_time();
        citer->second.seen_count ++;
    }
    return 0;
}

void ContentManager::AddContentToMap(Content &p_content, int inserthead)
{
    std::string mapindex = p_content.name + p_content.content_type;
    if (inserthead == 1) {
        m_content_map.insert(m_content_map.begin(), std::pair<std::string, Content>(mapindex, p_content));
    }else{
        m_content_map.insert(make_pair(mapindex, p_content));
    }
}

bool ContentManager::AddEpisode(Content &ct, const Episode& ep)
{
    //Notes: for VIP or movie contents, the EPG content ID is unique, 
    // so it is not necessary to modify the logic here to handle duplicate IDs in VIP/Movies.
    // that is why we don't need to pass in the "showtype" argument.

    //For multiple bitrate content, we use show_index and bitrate_type as unique key.
    std::pair<EpisodesIterator, bool> ret = ct.episodes.insert(ep);

    bool is_insert = ret.second;

    if (is_insert) {
        std::set<Episode>::reverse_iterator rit = ct.episodes.rbegin();
        if (rit->eindex == ep.eindex) {
            //we inserted the last item.
            ct.last_category_id = 0;
            ct.last_seen = get_epoch_time();
        }
    }
    else
    {
        //如果eindex有重复就会走到这里，例如，天天向上可能会一天有2集。前一个是很小的预告片，后一个才是正片
        //这种情况下，如果都是大文件，就保留新的，如果一大一小，就保留大的（小的定义是只有一个分段）
        EpisodesIterator it = ct.episodes.find(ep);

        if( (it->rid_array.size() > 1 && ep.rid_array.size() > 1 && it->timestamp < ep.timestamp)
            ||
            (ep.rid_array.size() > 1 && it->rid_array.size()<=1)  )
        {
            ct.episodes.erase(it);
            ct.episodes.insert(ep);
            ct.last_seen = get_epoch_time();
            is_insert = true;
        }
    }

    //return true is inserted.
    return is_insert;   
}

int ContentManager::RetireContents()
{
    int rm_count = 0;
    
    time_t time_now = get_epoch_time();

    for (ContentIterator ci = m_content_map.begin(); ci != m_content_map.end(); ) {
        //check the last_seen_time
        if (ci->second.last_seen > 0) {
            if (time_now - ci->second.last_seen >= m_config.CONTENT_HOT_RETIRE_TIME) {
                //remove the content from maps.
                LOG4CPLUS_INFO(g_logger, "Retired content " << ci->second.name);
                //ci = m_content_map.erase(ci);
                m_content_map.erase(ci++);
                rm_count ++;
            }else{
                //now we goes into the episode list to remove old ones.
                //std::set<Episode>::iterator ca;
                for (EpisodesIterator ca = ci->second.episodes.begin(); ca != ci->second.episodes.end(); ) {
                    bool retire_this = false;
                    bool this_vip = false;
                    if ( (time_now - ca->timestamp) >= m_config.CONTENT_HOT_EP_RETIRE_TIME) {
                        retire_this = true;
                    }else  if (ci->first == "VIPVIP") {
                        this_vip = true;
                        std::vector<RID_Internal_Struct>::const_iterator rid_iter;
                        for (rid_iter = ca->rid_array.begin(); rid_iter != ca->rid_array.end(); rid_iter ++ ) 
                        {
                            if (rid_iter->rid_actpush_count < m_config.CONTENT_ACTPUSH_PER_RID_LIMIT) {
                                break;
                            }
                        }
                        if (rid_iter == ca->rid_array.end()) {
                            retire_this = true;
                        }
                    }
                    if (retire_this == true) {
                        LOG4CPLUS_INFO(g_logger,"Retired episode " << ca->filename);
                        if (this_vip == false) {
                            LOG4CPLUS_INFO(g_statlog, "R " << ca->filename);
                        }else{
                            LOG4CPLUS_INFO(g_statlog, "RV " << ca->filename);
                        }
                        std::stringstream counts;
                        for (std::vector<RID_Internal_Struct>::const_iterator rid_iter = ca->rid_array.begin(); rid_iter != ca->rid_array.end(); rid_iter ++ ) 
                        {
                            counts << rid_iter->rid_actpush_count << ":" << rid_iter->rid_paspush_count << ":" << rid_iter->rid_actpush_low_bw_cnt << " " << rid_iter->rid_actpush_high_disk_cnt << " " << rid_iter->rid_actpush_low_online_cnt;
                            std::string ridstr;
                            bool ridready = GetRidShortString(rid_iter->rid_string, ridstr);
                            if (ridready == TRUE) {
                                if (this_vip == false) {
                                    LOG4CPLUS_INFO(g_vippush, "R-EP " << ridstr << " " << rid_iter->rid_actpush_count << ":" << rid_iter->rid_paspush_count << ":" << rid_iter->rid_actpush_low_bw_cnt  << " " << rid_iter->rid_actpush_high_disk_cnt << " " << rid_iter->rid_actpush_low_online_cnt);
                                }else{
                                    LOG4CPLUS_INFO(g_vippush, "RV-EP " << ridstr << " " << rid_iter->rid_actpush_count << ":" << rid_iter->rid_paspush_count << ":" << rid_iter->rid_actpush_low_bw_cnt  << " " << rid_iter->rid_actpush_high_disk_cnt << " " << rid_iter->rid_actpush_low_online_cnt);
                                }
                            }
                        }
                        LOG4CPLUS_INFO(g_statlog, counts.str());
                        //ca = ci->second.episodes.erase(ca); 
                        ci->second.episodes.erase(ca++); 
                    }else{
                        ca++;
                    }
                }
                ci ++;
            }
        }else{
            ci++;
        }
    }
    return rm_count;

}

int ContentManager::RetireVIPRIDs(const bool newcontent_loaded)
{
    int rm_count = 0;
    for (VIPRIDIterator ci = m_viprid_data.begin(); ci != m_viprid_data.end(); ) {
        //check the remaining quota.
        int quota = ci->second.rid_full.rid_push_target - ci->second.rid_full.rid_actpush_count;
        if (quota <= 0) {
            LOG4CPLUS_DEBUG(g_logger, "Retired RID " << ci->first);
            LOG4CPLUS_INFO(g_vippush, "RRID " << ci->first << " " << ci->second.rid_full.rid_actpush_count);
            m_viprid_data.erase(ci++);
            rm_count ++;
        }else{
            ci++;
        }
    }
    LOG4CPLUS_INFO(g_logger, "RetireVIPRIDs " << rm_count << " removed, remaining " << m_viprid_data.size());
    LOG4CPLUS_INFO(g_statlog, "RVIP-" << rm_count << "=" << m_viprid_data.size());

    if (newcontent_loaded == false) {
        //there is no need to remove duplicate as there is no new content loaded.
        return rm_count;
    }

    //we remove duplicate push task between Active Push and VIP push.
    ContentIterator vipiter;
    int retired_rid_count = 0;
    int checked_rid_count = 0;
    bool vipfound = SearchContentByName("VIP", "VIP", vipiter);
    if ( (vipfound == true) && (vipiter != m_content_map.end()) ) {
        //now we need to get into the episodes.
        for (EpisodesIterator ca = vipiter->second.episodes.begin(); ca != vipiter->second.episodes.end(); ca++) {
            for (std::vector<RID_Internal_Struct>::const_iterator rid_iter = ca->rid_array.begin(); rid_iter != ca->rid_array.end(); rid_iter ++ ) 
            {
                std::string rid_shortstr; 
                bool found = GetRidShortString(rid_iter->rid_string, rid_shortstr);
                if (found == true) {
                    //find the rid and remove it.
                    VIPRIDIterator ri = m_viprid_data.find(rid_shortstr);
                    if (ri != m_viprid_data.end()) {
                        LOG4CPLUS_INFO(g_vippush, "RDUP " << rid_shortstr << " " << ri->second.rid_full.rid_actpush_count);
                        m_viprid_data.erase(ri);
                        retired_rid_count ++;
                    }
                }
            }
            checked_rid_count += ca->rid_array.size();
        }
        LOG4CPLUS_INFO(g_logger, "RetireVIPRIDs " << retired_rid_count << " out of " << checked_rid_count << " were retired for VIP duplication.");
        LOG4CPLUS_INFO(g_statlog, "R DUP " << checked_rid_count << "-" << retired_rid_count);
    }
    return rm_count;

}

void ContentManager::UpdateConfig(const ContentManagerCfg& m_inconfig)
{
    m_config = m_inconfig;
    m_local_data_file = m_inconfig.CONTENT_HOTSAVE_FILENAME;
    m_local_viprid_file = m_inconfig.CONTENT_VIP_RID_FILENAME;
    m_new_hot_file = m_inconfig.CONTENT_NEWHOT_XML_FILENAME;
    m_new_upload_file = m_inconfig.CONTENT_NEWUPLOAD_XML_FILENAME;
    m_new_viprid_file = m_inconfig.CONTENT_NEWVIP_RID_XML_FILENAME;
    m_new_ridmakeup_file = m_inconfig.CONTENT_NEWRID_MAKEUP_XML_FILENAME;
    m_scheduler.ReloadConfig(&m_config, m_started);
    
//    PS_LOG("ContentManager: config reloaded");
    CONSOLE_LOG("ContentManager: config reloaded");
}

void ContentManager::UpdateGoodNatSet(const std::string &nat_string)
{
    std::vector<std::string> substrings;
    m_good_nat_set.clear(); 
    boost::split(substrings, nat_string, boost::is_any_of(","));
    for (std::vector<std::string>::iterator siter = substrings.begin(); siter < substrings.end(); siter ++) {
        try
        {
            int good_nat = boost::lexical_cast<int>(*siter);
            m_good_nat_set.insert(good_nat);
            LOG4CPLUS_DEBUG(g_logger, "GOOD NAT: " << good_nat);
        }
        catch (boost::bad_lexical_cast & )
        {
            LOG4CPLUS_DEBUG(g_logger, "GOOD NAT: failed " << *siter);
        }
    }
    LOG4CPLUS_INFO(g_logger, "GOOD NAT list has " << m_good_nat_set.size() << " nats");
}

int ContentManager::load_local_data_file()
{
    //we directly read the data file.
    using namespace tinyxml;
    int content_count = 0;

    LOG4CPLUS_DEBUG(g_logger, "load_local_data_file--content_count:" << content_count);
    m_content_map.clear();
    try
    {
        boost::filesystem::path path(boost::filesystem::current_path().string());
        path /= m_local_data_file;
        LOG4CPLUS_INFO(g_logger,"Loading local data file " << path.string());
        if (boost::filesystem::exists(path))
        {
            TiXmlDocument data_file(path.string());
            if (data_file.LoadFile())
            {
                TiXmlNode *contentlist_node = data_file.FirstChild("contentlist");
                TiXmlElement *contentlist = NULL;
                TiXmlElement *content_element = NULL;
                if (contentlist_node) {
                    contentlist = contentlist_node->ToElement();
                    TiXmlNode *content_element_node = contentlist->FirstChild("content");
                    if (content_element_node) {
                        content_element = content_element_node->ToElement();
                    }
                }
                for (; content_element != NULL; content_element = content_element->NextSiblingElement())
                {
                    Content m_content;
                    m_content.internal_id = atoi(content_element->FirstChild("cid")->ToElement()->GetText());
                    m_content.name = content_element->FirstChild("name")->ToElement()->GetText();
                    TiXmlNode *content_type_node = content_element->FirstChild("type");
                    if (content_type_node) {
                        m_content.content_type = content_type_node->ToElement()->GetText();
                    }else{
                        m_content.content_type = "TV";
                    }
                    m_content.last_seen = atoi(content_element->FirstChild("last_seen")->ToElement()->GetText());
                    m_content.seen_count = atoi(content_element->FirstChild("seen_count")->ToElement()->GetText());
                    m_content.last_category_id = atoi(content_element->FirstChild("last_category_id")->ToElement()->GetText());
                    LOG4CPLUS_INFO(g_logger, " Loaded content " << m_content.name);
                    TiXmlElement *episodes_element = content_element->FirstChild("episodes")->ToElement();
                    if (episodes_element) {
                        TiXmlNode *e_node = episodes_element->FirstChild("episode");
                        if (e_node) {
                            for (TiXmlElement *e_element = e_node->ToElement(); e_element; e_element = e_element->NextSiblingElement())
                            {
                                Episode ep;
                                ep.eindex = atoi(e_element->FirstChild("eindex")->ToElement()->GetText());
                                ep.timestamp = atoi(e_element->FirstChild("timestamp")->ToElement()->GetText());
                                ep.content_id = atoi(e_element->FirstChild("content_id")->ToElement()->GetText());
                                ep.filename = e_element->FirstChild("filename")->ToElement()->GetText();
                                TiXmlNode *encode_node = e_element->FirstChild("encode_name");
                                if (encode_node) {
                                    ep.encode_name = encode_node->ToElement()->GetText();
                                }else{
                                    ep.encode_name = Util::URLEncode::EncodeURL(ep.filename);
                                }
                                TiXmlNode *ep_bitrate = e_element->FirstChild("bitrate");
                                if (ep_bitrate) {
                                    ep.bitrate = atoi(ep_bitrate->ToElement()->GetText());
                                }else{
                                    ep.bitrate = 0;
                                }
                                TiXmlNode *ep_bitrate_type = e_element->FirstChild("bitrate_type");
                                if (ep_bitrate_type) {
                                    ep.bitrate_type = atoi(ep_bitrate_type->ToElement()->GetText());
                                }else{
                                    ep.bitrate_type = 0;
                                }
                                ep.total_active_pushed = 0;
                                TiXmlElement *r_elements = e_element->FirstChild("RIDs")->ToElement();
                                if (r_elements) {
                                    TiXmlNode *r_node = r_elements->FirstChild("RID");
                                    if (r_node) {
                                        for (TiXmlElement *r_element = r_node->ToElement(); r_element; r_element = r_element->NextSiblingElement() )
                                        {
                                            RID_Internal_Struct rid_internal(r_element->GetText());
                                            //rid_internal.rid_string = r_element->GetText();
                                            //rid_internal.rid_actpush_count = 0;
                                            const char *c_attr = r_element->Attribute("act");
                                            if (c_attr) rid_internal.rid_actpush_count = atoi(c_attr);
                                            c_attr = r_element->Attribute("pas");
                                            if (c_attr) rid_internal.rid_paspush_count = atoi(c_attr);
                                            rid_internal.rid_actpush_low_bw_cnt = rid_internal.rid_actpush_count * (100 - m_config.CONTENT_ACTPUSH_BIG_BW_RATIO)/100;
                                            rid_internal.rid_actpush_high_disk_cnt = rid_internal.rid_actpush_count * (100 - m_config.CONTENT_ACTPUSH_BIG_DISK_RATIO)/100;
                                            rid_internal.rid_actpush_low_online_cnt = rid_internal.rid_actpush_count * (100 - m_config.CONTENT_ACTPUSH_HIGH_ONLINE_RATIO)/100;
                                            ep.total_active_pushed += MIN(rid_internal.rid_actpush_count, m_config.CONTENT_ACTPUSH_PER_RID_LIMIT);
                                            ep.rid_array.push_back(rid_internal);
                                        }
                                    }
                                }
                                m_content.episodes.insert(ep);
                                LOG4CPLUS_INFO(g_logger, "  Episode " << ep.filename);
                            }
                        }
                    }
                    AddContentToMap(m_content, 1);
                    content_count ++;
                    if (m_content.internal_id >= m_next_internal_id) {
                        m_next_internal_id = m_content.internal_id + 1;
                    }
                }
                data_file.SaveFile();
            }else{
                LOG4CPLUS_WARN(g_logger,"loading config file failed");
                CONSOLE_LOG("loading config file failed");
            }
        }else{ 
            LOG4CPLUS_WARN(g_logger,"File " << path << " not found");
            CONSOLE_LOG("File " << path << " not found");
        }
    }
    catch (boost::filesystem::basic_filesystem_error<boost::filesystem::path> &e)
    {
        LOG4CPLUS_WARN(g_logger,"boost::filesystem::exists() throw an exception.");
        CONSOLE_LOG("boost::filesystem::exists() throw an exception.");
        return -1;
    }

    LOG4CPLUS_INFO(g_logger,"EXIT: load_local_data_file--In total " << content_count << " loaded." << " m_content_map.size" << m_content_map.size());
    //RetireContents();
    return content_count;
}

int ContentManager::save_local_data_file()
{
    LOG4CPLUS_DEBUG(g_logger, "ENTER: save_local_data_file");
    using namespace tinyxml;

    int content_count = 0;
    //we first save to a local file DEFAULT_NAME.<date-string>
    std::string filename = m_local_data_file + "-" + get_timestr();

    LOG4CPLUS_DEBUG(g_logger, "ContentManager::save_local_data_file-- filename" << filename);
    push_util::DeleteExitFileAndCreatNewFile( filename );
    TiXmlDocument local_data_file( filename.c_str() );
    if (local_data_file.LoadFile() ) {
        TiXmlElement *contentlist = local_data_file.InsertEndChild(TiXmlElement("contentlist"))->ToElement();
        for (ContentIterator i = m_content_map.begin(); i != m_content_map.end(); i++) {
            try 
            {
                TiXmlElement *add_content = contentlist->InsertEndChild(TiXmlElement("content"))->ToElement();
                add_content->InsertEndChild(TiXmlElement("cid"))->ToElement()->InsertEndChild(TiXmlText(boost::lexical_cast<std::string>(i->second.internal_id)));
                add_content->InsertEndChild(TiXmlElement("name"))->ToElement()->InsertEndChild(TiXmlText(i->second.name));
                add_content->InsertEndChild(TiXmlElement("type"))->ToElement()->InsertEndChild(TiXmlText(i->second.content_type));
                add_content->InsertEndChild(TiXmlElement("last_seen"))->ToElement()->InsertEndChild(TiXmlText(boost::lexical_cast<std::string>(i->second.last_seen)));
                add_content->InsertEndChild(TiXmlElement("seen_count"))->ToElement()->InsertEndChild(TiXmlText(boost::lexical_cast<std::string>(i->second.seen_count)));
                add_content->InsertEndChild(TiXmlElement("last_category_id"))->ToElement()->InsertEndChild(TiXmlText(boost::lexical_cast<std::string>(i->second.last_category_id)));
                TiXmlElement *add_episodes = add_content->InsertEndChild(TiXmlElement("episodes"))->ToElement();
                EpisodesIterator ce;
                for (ce = i->second.episodes.begin(); ce != i->second.episodes.end(); ce ++) 
                {
                    TiXmlElement *add_ep = add_episodes->InsertEndChild(TiXmlElement("episode"))->ToElement(); 
                    add_ep->InsertEndChild(TiXmlElement("eindex"))->ToElement()->InsertEndChild(TiXmlText(boost::lexical_cast<std::string>(ce->eindex)));
                    add_ep->InsertEndChild(TiXmlElement("timestamp"))->ToElement()->InsertEndChild(TiXmlText(boost::lexical_cast<std::string>(ce->timestamp)));
                    add_ep->InsertEndChild(TiXmlElement("content_id"))->ToElement()->InsertEndChild(TiXmlText(boost::lexical_cast<std::string>(ce->content_id)));
                    add_ep->InsertEndChild(TiXmlElement("filename"))->ToElement()->InsertEndChild(TiXmlText(ce->filename));
                    add_ep->InsertEndChild(TiXmlElement("encode_name"))->ToElement()->InsertEndChild(TiXmlText(ce->encode_name));
                    add_ep->InsertEndChild(TiXmlElement("bitrate"))->ToElement()->InsertEndChild(TiXmlText(boost::lexical_cast<std::string>(ce->bitrate)));
                    add_ep->InsertEndChild(TiXmlElement("bitrate_type"))->ToElement()->InsertEndChild(TiXmlText(boost::lexical_cast<std::string>(ce->bitrate_type)));
                    //do we need to save RID array? or we can re-populate it
                    //live?
                    TiXmlElement *add_rids = add_ep->InsertEndChild(TiXmlElement("RIDs"))->ToElement();
                    //std::vector<RID_Internal_Struct>::iterator cr;
                    for (std::vector<RID_Internal_Struct>::const_iterator cr = ce->rid_array.begin(); cr != ce->rid_array.end(); cr ++) 
                    {
                        TiXmlElement *ridnode = add_rids->InsertEndChild(TiXmlElement("RID"))->ToElement();
                        ridnode->SetAttribute("act", cr->rid_actpush_count);
                        ridnode->SetAttribute("pas", cr->rid_paspush_count);
                        ridnode->InsertEndChild(TiXmlText(cr->rid_string.c_str()));
                    }
                }
            }
            catch (boost::bad_lexical_cast & )
            {
                LOG4CPLUS_WARN(g_logger,"lexical_cast exception for " << i->second.name);
            }
            content_count ++;
        }
    }

    LOG4CPLUS_DEBUG(g_logger, "save_local_data_file--content_count: " << content_count << " m_content_map.size:" << m_content_map.size());
    local_data_file.SaveFile();

    // then copy it to the DEFAULT_NAME.
    try
    {
        if (boost::filesystem::exists(m_local_data_file)) {
            boost::filesystem::remove(m_local_data_file);
        }
        boost::filesystem::copy_file(filename, m_local_data_file);
    }
    catch (boost::filesystem::basic_filesystem_error<boost::filesystem::path> &e)
    {
        LOG4CPLUS_WARN(g_logger,"boost::filesystem::exists() throw an eception.");
        return -1;
    }

    LOG4CPLUS_INFO(g_logger,"In total " << content_count << " saved into local file" " m_content_map.size:" << m_content_map.size());
    //TODO: (wenjie 2011-11-15) we only keep the last 10 days of date.
    LOG4CPLUS_DEBUG(g_logger, "EXIT: save_local_data_file");
    return content_count;
}

int ContentManager::save_local_viprid_file()
{
    LOG4CPLUS_DEBUG(g_logger, "ENTER: save_local_viprid_file");
    using namespace tinyxml;

    int rid_count = 0;
    //we first save to a local file DEFAULT_NAME.<date-string>
    std::string filename = m_local_viprid_file + "-" + get_timestr();

    LOG4CPLUS_DEBUG(g_logger, "save_local_viprid_file-- filename" << filename);
    push_util::DeleteExitFileAndCreatNewFile( filename );
    TiXmlDocument local_data_file( filename.c_str() );
    if (local_data_file.LoadFile() ) {
        TiXmlElement *ridlist = local_data_file.InsertEndChild(TiXmlElement("rid_list"))->ToElement();
        for (VIPRIDIterator i = m_viprid_data.begin(); i != m_viprid_data.end(); i++) {
            try 
            {
                TiXmlElement *add_rid = ridlist->InsertEndChild(TiXmlElement("RID"))->ToElement();
                add_rid->InsertEndChild(TiXmlElement("id"))->ToElement()->InsertEndChild(TiXmlText(boost::lexical_cast<std::string>(i->first)));
                add_rid->InsertEndChild(TiXmlElement("fullrid"))->ToElement()->InsertEndChild(TiXmlText(i->second.rid_full.rid_string));
                add_rid->InsertEndChild(TiXmlElement("pri"))->ToElement()->InsertEndChild(TiXmlText(boost::lexical_cast<std::string>(i->second.rid_full.rid_priority)));
                add_rid->InsertEndChild(TiXmlElement("cid"))->ToElement()->InsertEndChild(TiXmlText(boost::lexical_cast<std::string>(i->second.content_id)));
                add_rid->InsertEndChild(TiXmlElement("ename"))->ToElement()->InsertEndChild(TiXmlText(i->second.encode_name));
                add_rid->InsertEndChild(TiXmlElement("segno"))->ToElement()->InsertEndChild(TiXmlText(boost::lexical_cast<std::string>(i->second.seg_index)));
                int new_target = i->second.rid_full.rid_push_target - i->second.rid_full.rid_actpush_count;
                if (new_target < 0) {
                    new_target = 0;
                }
                add_rid->InsertEndChild(TiXmlElement("target"))->ToElement()->InsertEndChild(TiXmlText(boost::lexical_cast<std::string>(new_target)));
                rid_count ++; 
            }
            catch (boost::bad_lexical_cast & )
            {
                LOG4CPLUS_WARN(g_logger,"lexical_cast exception for " << i->first);
            }

        }
    }

    LOG4CPLUS_DEBUG(g_logger, "save_local_viprid_file--rid_count: " << rid_count << " RIDMap.size:" << m_viprid_data.size());
    local_data_file.SaveFile();

    try
    {
        if (boost::filesystem::exists(m_local_viprid_file)) {
            boost::filesystem::remove(m_local_viprid_file);
        }
        boost::filesystem::copy_file(filename, m_local_viprid_file);
        boost::filesystem::remove(filename);
    }
    catch (boost::filesystem::basic_filesystem_error<boost::filesystem::path> &e)
    {
        LOG4CPLUS_WARN(g_logger,"boost::filesystem::exists() throw an eception.");
        return -1;
    }

    LOG4CPLUS_INFO(g_logger, "In total " << rid_count << " RID saved into local file");
    LOG4CPLUS_DEBUG(g_logger, "EXIT: save_local_viprid_file");
    return rid_count;
}

bool ContentManager::BackupFile(const std::string &p_filename, bool bRemoveOrigin)
{
    //rename the file to indicate we are done with it.
    std::string m_filename = p_filename + "-" + get_timestr();
    boost::filesystem::path oldpath(boost::filesystem::current_path().string());
    boost::filesystem::path newpath(boost::filesystem::current_path().string());
    oldpath /= p_filename;
    newpath /= m_filename; 
    try
    {
        if (boost::filesystem::exists(newpath)) {
            boost::filesystem::remove(newpath);
        }
    }
    catch (boost::filesystem::basic_filesystem_error<boost::filesystem::path> &e)
    {
        LOG4CPLUS_WARN(g_logger,"boost::filesystem cannot access " << newpath.string());
        return false;
    }
    try
    {
        if (boost::filesystem::exists(oldpath)) {
            boost::filesystem::copy_file(oldpath, newpath);
        }
    }
    catch (boost::filesystem::basic_filesystem_error<boost::filesystem::path> &e)
    {
        LOG4CPLUS_WARN(g_logger,"boost::filesystem cannot access " << newpath.string());
        return false;
    }

    LOG4CPLUS_INFO(g_logger, "File " << oldpath.string() << " backup to " << newpath.string() );
    if (false == bRemoveOrigin) return true;

    //Now we try to remove the original file
    try
    {
        boost::filesystem::remove(oldpath);
    }
    catch (boost::filesystem::basic_filesystem_error<boost::filesystem::path> &e)
    {
        LOG4CPLUS_WARN(g_logger,"boost::filesystem cannot remove " << oldpath.string());
        return false;
    }
    LOG4CPLUS_INFO(g_logger, "File " << oldpath.string() << " removed");
    return true;
}

void ContentManager::load_scheduler_session()
{
    try
    {   
        boost::filesystem::path path(boost::filesystem::current_path().string());
        path /= m_config.CONTENT_SESSION_SAVE_FILE;
        if (boost::filesystem::exists(path))
        {
            std::ifstream file(path.string().c_str());
            file >> m_last_second;
            file >> m_last_quota;
        }
    }
    catch (boost::filesystem::basic_filesystem_error<boost::filesystem::path> &e)
    {
        LOG4CPLUS_WARN(g_logger, "load_scheduler_session: exists() throw an exception.");
        return;
    }
}

void ContentManager::save_scheduler_session()
{
    try
    {   
        boost::filesystem::path path(boost::filesystem::current_path().string());
        path /= m_config.CONTENT_SESSION_SAVE_FILE;

        std::ofstream file(path.string().c_str());
        file << m_scheduler.GetDailyTimePoint() << " " << m_scheduler.GetDailyNum();
    }
    catch (std::ofstream::failure e)
    {
        LOG4CPLUS_WARN(g_logger, "save_scheduler_session: throw an exception.");
        return;
    }
}

std::string ContentManager::get_timestr()
{
    boost::gregorian::date  today = boost::gregorian::day_clock::local_day();

    return to_iso_extended_string(today);
}

uint32_t ContentManager::get_epoch_time()
{
    boost::posix_time::ptime time_t_epoch(boost::gregorian::date(1970,1,1));
    boost::posix_time::ptime current_time(boost::posix_time::second_clock::local_time());

    boost::posix_time::time_duration diff = current_time - time_t_epoch;
    return diff.total_seconds();
}

bool ContentManager::FilterShowName(const std::string& raw_name, PlayHistory_Index &matched_history_item)
{
    //we first split the raw_name based on "." to see whether the name is NEW version with FT type.
    std::vector<std::string> substrings;
    boost::split(substrings, raw_name, boost::is_any_of("."));
    std::string prematch_string = raw_name;

    bool is_multi_rate = false;
    if (substrings.size() >=3 ) {
        //we will look at the last one to see whether it is 0. 
        if ( (boost::starts_with(substrings[substrings.size()-1], "ft")) ) {
            LOG4CPLUS_DEBUG(g_logger, "provided name is the new format.");
            if (substrings[substrings.size()-1] != "ft") {
                is_multi_rate = true;
            }
        }else{
            //without a "FT" ended name, this is old version. We don't provide any matches.
            //return false;
        }
    }else{
        //without a "FT" ended name, this is old version. We don't provide any matches.
        //return false;
    }

    //if multiple_rate is false, the filename is in GBK coding, we will convert it.
    if (is_multi_rate == false) {
        int ret = Util::UnicodeUTF8::API_Gbk2Utf8(raw_name.c_str(), prematch_string);
        if (ret < 0 ) {
            //conversion has failed, give up.
            LOG4CPLUS_DEBUG(g_logger, "Hist cannot be converted " << raw_name);
            return false;
        }else{
            prematch_string = prematch_string + ".ft";
            LOG4CPLUS_DEBUG(g_logger, "Hist conversion to " << prematch_string);
        }
    }

    try {
        boost::smatch what2;
        if (boost::regex_match(prematch_string, what2, boost::regex(newreg_exp2))) {
            matched_history_item.show_name.assign(what2[1].first, what2[1].second);
            matched_history_item.ep_index = boost::lexical_cast<uint32_t>(std::string(what2[2].first, what2[2].second));
            matched_history_item.seg_index = boost::lexical_cast<uint16_t>(std::string(what2[3].first, what2[3].second));
            if (is_multi_rate) {
                matched_history_item.bitrate_type = boost::lexical_cast<uint32_t>(std::string(what2[4].first, what2[4].second));
            }else{
                matched_history_item.bitrate_type = m_config.CONTENT_PUSH_BITRATE_TYPE;
            }
            return true;
        }

        boost::smatch what1;
        if (boost::regex_match(prematch_string, what1, boost::regex(newreg_exp1))) {
            matched_history_item.show_name.assign(what1[1].first, what1[1].second);
            matched_history_item.ep_index = boost::lexical_cast<uint32_t>(std::string(what1[2].first, what1[2].second));
            matched_history_item.seg_index = boost::lexical_cast<uint16_t>(std::string(what1[3].first, what1[3].second));
            if (is_multi_rate) {
                matched_history_item.bitrate_type = boost::lexical_cast<uint32_t>(std::string(what1[4].first, what1[4].second));
            }else{
                matched_history_item.bitrate_type = m_config.CONTENT_PUSH_BITRATE_TYPE;
            }
            return true;
        }            

        return false;
    }
    catch(boost::bad_lexical_cast& e) {
        return false;
    }
}

void ContentManager::DumpContents()
{
    for (ContentIterator iter = m_content_map.begin(); iter != m_content_map.end(); iter++) {
        DumpContent(iter->second);
    }
}

void ContentManager::DumpContent(const Content &c)
{
    std::cout << "content " << c.internal_id << " " << c.name << " ";
    std::cout << c.last_seen << " " << c.seen_count << std::endl;
    for (EpisodesConstIterator eiter = c.episodes.begin(); eiter != c.episodes.end(); eiter ++) 
    {
        std::cout << "  episode " << eiter->eindex << " " << eiter->timestamp << " " << eiter->content_id << " " << eiter->filename << std::endl;
        std::vector<RID_Internal_Struct>::const_iterator riter;
        std::cout << "      RIDs ";
        for (riter = eiter->rid_array.begin(); riter != eiter->rid_array.end(); riter ++) 
        {
            std::cout << riter->rid_string << " " << riter->rid_actpush_count << " "  << riter->rid_paspush_count << std::endl;
        }
        std::cout << eiter->encode_name << std::endl;
    }
}

protocol::RidInfo ContentManager::FetchRidInfo(const std::string& playurl)
{
    protocol::RidInfo rid_info;
    vector<std::string> params;

    //params = framework::util::splite(playurl, "&");
    //LOG4CPLUS_DEBUG(g_logger, "FetchRidInfo: " << playurl);
    boost::algorithm::split(params, playurl, boost::algorithm::is_any_of("&"));

    for (int i = 0; i != params.size(); ++i)
    {
        vector<std::string> pair;
        //LOG4CPLUS_DEBUG(g_logger, "FetchRidInfo: P " << params[i]);
        boost::algorithm::split(pair, params[i], boost::algorithm::is_any_of("="));
        //pair = framework::util::splite(params[i], "=");
        assert(pair.size() == 2);
        try {
            if (pair[0] == "rid")
            {
                rid_info.rid_.from_string(pair[1]);
            }
            else if (pair[0] == "filelength")
            {
                rid_info.file_length_ = boost::lexical_cast<size_t>(pair[1]);
            }
            else if (pair[0] == "blocksize")
            {
                rid_info.block_size_ = boost::lexical_cast<size_t>(pair[1]);
            }
            else if (pair[0] == "blocknum")
            {
                rid_info.block_count_ = boost::lexical_cast<size_t>(pair[1]);
            }
            else if (pair[0] == "blockmd5")
            {
                rid_info.block_md5_s_ = ParseBlockMd5s(pair[1]);
            }
        }
        catch (boost::bad_lexical_cast &) 
        {
            rid_info.file_length_ = 0;
            break;
        }
    }
    return rid_info;
}

bool ContentManager::GetRidShortString(const std::string& playurl, std::string &rid_str)
{
    vector<std::string> params;

    boost::algorithm::split(params, playurl, boost::algorithm::is_any_of("&"));

    bool found = false;
    if ( params.size() > 1 )
    {
        vector<std::string> pair;
        boost::algorithm::split(pair, params[0], boost::algorithm::is_any_of("="));
        assert(pair.size() == 2);
        if (pair[0] == "rid")
        {
            rid_str = pair[1];
            found = true;
        }
    }
    return found;
}

vector<Guid> ContentManager::ParseBlockMd5s(const std::string& block_str)
{
    vector<Guid> block_md5s;
    vector<std::string> md5_strs;
    boost::algorithm::split(md5_strs, block_str, boost::algorithm::is_any_of("@"));
    //md5_strs = framework::util::splite(block_str, "@");
    for (int i = 0; i != md5_strs.size(); ++i)
    {
        Guid md5;
        md5.from_string(md5_strs[i]);
        block_md5s.push_back(md5);
    }
    return block_md5s;
}

int ContentManager::load_test_data()
{
    //we directly read the data file.
    using namespace tinyxml;
//    int count = 0;

    m_content_map.clear();

    Content m_content;
    m_content.internal_id = 1;
    m_content.name = std::string("非诚勿扰");
    m_content.content_type = "TV";
    m_content.last_seen = get_epoch_time();
    m_content.seen_count = 5;
    m_content.last_category_id = 0;

    Episode ep3;
    ep3.eindex = 20111022;
    ep3.timestamp = get_epoch_time();
    ep3.content_id = 2343535;
    ep3.filename = std::string("非诚勿扰-20111022-测试.mp4");
    ep3.rid_array.push_back(RID_Internal_Struct("AFC1CF534BDAED5E01B9DAFD980104F0", 0, 0));
    ep3.rid_array.push_back(RID_Internal_Struct("AFC1CF534BDAED5E01B9DAFD980104F1", 0, 0));
    ep3.rid_array.push_back(RID_Internal_Struct("AFC1CF534BDAED5E01B9DAFD980104F2", 0, 0));

    m_content.episodes.insert(ep3);

    Episode ep4;
    ep4.eindex = 20111030;
    ep4.timestamp = get_epoch_time();
    ep4.content_id = 2344535;
    ep4.filename = std::string("非诚勿扰-20111030-测试.mp4");
    ep4.rid_array.push_back(RID_Internal_Struct("AFC1CF534BDAED5E01B9DAFD980104F3"));
    ep4.rid_array.push_back(RID_Internal_Struct("AFC1CF534BDAED5E01B9DAFD980104F4"));
    ep4.rid_array.push_back(RID_Internal_Struct("AFC1CF534BDAED5E01B9DAFD980104F5"));

    m_content.episodes.insert(ep4);
    
    AddContentToMap(m_content, 1);

    Content m_con;
    m_con.internal_id = 1;
    m_con.name = std::string("步步惊心");
    m_con.content_type = "TV";
    m_con.last_seen = get_epoch_time();
    m_con.seen_count = 5;
    m_con.last_category_id = 0;

    Episode ep1;
    ep1.eindex = 1;
    ep1.timestamp = get_epoch_time();
    ep1.content_id = 4234300;
    ep1.filename = std::string("步步惊心（第一集）.mp4");
    ep1.rid_array.push_back(RID_Internal_Struct("3F98FAB95B663EADEC1F5B936B141A71"));
    ep1.rid_array.push_back(RID_Internal_Struct("3F98FAB95B663EADEC1F5B936B141A72"));
    ep1.rid_array.push_back(RID_Internal_Struct("3F98FAB95B663EADEC1F5B936B141A73"));

    m_con.episodes.insert(ep1);

    Episode ep2;
    ep2.eindex = 2;
    ep2.timestamp = get_epoch_time();
    ep2.content_id = 4234356;
    ep2.filename = std::string("步步惊心（第二集）.mp4");
    ep2.rid_array.push_back(RID_Internal_Struct("3F98FAB95B663EADEC1F5B936B141A74"));
    ep2.rid_array.push_back(RID_Internal_Struct("3F98FAB95B663EADEC1F5B936B141A75"));
    ep2.rid_array.push_back(RID_Internal_Struct("3F98FAB95B663EADEC1F5B936B141A76"));

    m_con.episodes.insert(ep2);

    AddContentToMap(m_con, 1);

    return 2;
}

void ContentManager::TestContentManager()
{
    std::map<std::string, int> testcases;
    testcases[std::string("Test_Save_Data")] = 0;
    testcases[std::string("Test_Load_Data")] = 0;
    testcases[std::string("Test_InsertContent")] = 0;
    testcases[std::string("Test_NoEpisodeInfo")] = 0;
    testcases[std::string("Test_OutputRID")] = 0;
    testcases[std::string("Test_SearchContent")] = 0;
    testcases[std::string("Test_NewHot_Content")] = 0;
    testcases[std::string("Test_NewUpload_Content")] = 0;
    testcases[std::string("Test_Scheduler")] = 0;
    testcases[std::string("Test_PacketV2")] = 1;

    //we first need to load the config.
    //ContentManagerCfg m_config;
    m_local_data_file = m_config.CONTENT_HOTSAVE_FILENAME;
    m_local_viprid_file = m_config.CONTENT_VIP_RID_FILENAME;


    if (testcases[std::string("Test_Save_Data")]) {
        load_test_data();
        save_local_data_file();
        DumpContents();
    }
    if (testcases[std::string("Test_Load_Data")]) {
        Start();
        save_local_data_file();
        DumpContents();
    }
    if (testcases[std::string("Test_InsertContent")]) {
        Start();

        Content m_content;
        m_content.internal_id = 3;
        m_content.name = std::string("天天向上Fake");
        m_content.content_type = "TV";
        m_content.last_seen = get_epoch_time();
        m_content.seen_count = 5;
        m_content.last_category_id = 0;

        Episode ep3;
        ep3.eindex = 20111022;
        ep3.timestamp = get_epoch_time();
        ep3.content_id = 5544535;
        ep3.filename = std::string("天天向上-20111022-测试.mp4");
        ep3.rid_array.push_back(RID_Internal_Struct("AFC1CF534BDAED5E01B9DAFD980104A0"));
        ep3.rid_array.push_back(RID_Internal_Struct("AFC1CF534BDAED5E01B9DAFD980104A1"));
        ep3.rid_array.push_back(RID_Internal_Struct("AFC1CF534BDAED5E01B9DAFD980104A2"));
        ep3.timestamp = get_epoch_time();
        m_content.episodes.insert(ep3);

        AddContentToMap(m_content);
        //DumpContents();
        save_local_data_file();
    }
    if (testcases[std::string("Test_NoEpisodeInfo")]) {
        Start();

        Content m_content;
        m_content.internal_id = 3;
        m_content.name = std::string("天天向上");
        m_content.content_type = "TV";
        m_content.last_seen = get_epoch_time();
        m_content.seen_count = 5;
        m_content.last_category_id = 0;

        AddContentToMap(m_content);
        Stop();
        Start();
        save_local_data_file();
        DumpContents();
    }
    if (testcases[std::string("Test_OutputRID")]) {
        Start();
        Content m_content = m_content_map[std::string("VIPVIP")];
        std::vector<RID_Tuple> myRIDs;
        CreatePushRIDs(m_content, 20111022, 0, 1, 4, myRIDs);
        std::vector<RID_Tuple>::iterator it;
        std::cout << "Returned RIDs" << std::endl;
        for (it = myRIDs.begin(); it != myRIDs.end(); it++) {
            std::cout << it->rid_full.rid_string << std::endl;
        }
    }
    if (testcases[std::string("Test_SearchContent")]) {
        Start();
        ContentIterator citer;
        if (true == SearchContentByName(std::string("非诚"), "TV", citer)) {
            DumpContent(citer->second);
        }else{
            std::cout << "string not found" << std::endl;
        }
        if (true == SearchContentByName(std::string("非诚勿扰技术 "), "TV", citer)) {
            DumpContent(citer->second);
        }else{
            std::cout << "string not found" << std::endl;
        }
    }
    if (testcases[std::string("Test_NewHot_Content")]) {
        Start();
        Load_New_Hot_Content();
        DumpContents();
        save_local_data_file();
    }
    if (testcases[std::string("Test_NewUpload_Content")]) {
        Start();
        Load_New_Hot_Content();
        Load_New_Upload_Episodes();
        DumpContents();
        save_local_data_file();
    }
    if (testcases[std::string("Test_Scheduler")]) {
        Start();
        bool isbusy = m_scheduler.IsBusyTime();
        int target = m_scheduler.QuerySchedulerForQuota(2);
        std::cout << "QueryScheduler returns " << isbusy << target << std::endl;
    }
    if (testcases[std::string("Test_PacketV2")]) {
        
        m_config.CONTENT_ACTPUSH_PER_RID_LIMIT = 10;
        m_config.CONTENT_PAUSE_DISABLED = 1;
        Start();
        framework::Guid gid("thisisnonesense");
        PeerInfo peer_info(gid, 0, 100, 100, 2048, 100, 1);

        std::vector<protocol::PlayHistoryItem> play_hist_vec;
        std::vector<protocol::PushTaskItem> ptask_list;
        protocol::PlayHistoryItem hitem;
        hitem.video_name_ = "传奇故事-20111113-逃亡后的返老还童[1].mp4";
        hitem.downloaded_segment_num_ = 1;
        hitem.continuously_played_duration_ = 2000;
        play_hist_vec.push_back(hitem);

        hitem.video_name_ = "步步惊心(第20集)[3].mp4";
        hitem.downloaded_segment_num_ = 2;
        hitem.continuously_played_duration_ = 8000;
        play_hist_vec.push_back(hitem);

        hitem.video_name_ = "非诚勿扰-20111112-佐藤爱终当心动女生[16].mp4";
        hitem.downloaded_segment_num_ = 0;
        hitem.continuously_played_duration_ = 5000;
        play_hist_vec.push_back(hitem);

        hitem.video_name_ = "溯及内容[4].mp4";
        hitem.downloaded_segment_num_ = 2;
        hitem.continuously_played_duration_ = 3000;
        play_hist_vec.push_back(hitem);

        hitem.video_name_ = "步步惊心(第21集)[5].mp4";
        hitem.downloaded_segment_num_ = 5;
        hitem.continuously_played_duration_ = 2000;
        play_hist_vec.push_back(hitem);

        for (int i = 0; i < 50; i ++) {

            std::cout << "Iteration " << i << " ***********" << std::endl << std::endl;
            if (i == 41) {
                i = 41;
            }
            GetPushRIDs(peer_info, play_hist_vec, ptask_list);

            for (std::vector<protocol::PushTaskItem>::iterator it = ptask_list.begin(); it != ptask_list.end(); it ++ ) {
                std::cout << it->rid_info_.rid_ << " fl= " << it->rid_info_.file_length_ << std::endl;
                std::cout << " " << it->channel_id_ << it->url_  << std::endl;
            }
            ptask_list.clear();
        }
        save_local_data_file();

    }
    getchar();
    return;
}

PUSH_SERVER_NAMESPACE_END


