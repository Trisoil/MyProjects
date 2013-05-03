/* ======================================================================
 *	content_matcher.h
 *	Copyright (c) 2009 Synacast. All rights reserved.
 *
 *	push server新热片管理
 */

#ifndef __CONTENT_MANAGER_H__
#define __CONTENT_MANAGER_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "push_header.h"
#include "push_struct.h"
#include "content_quota_scheduler.h"

#include <list>
#include <string>
#include <vector>
#include <map>

PUSH_SERVER_NAMESPACE_BEGIN

//#define TEST_CONTENT_MANAGER

#define CONTENT_CONST_RID_PER_PACKET 2

struct RID_Internal_Struct
{
    std::string rid_string;
    uint32_t rid_actpush_count;     //push count for active pushing (not depending on client's history)
    uint32_t rid_paspush_count;     //push count based on client's watching history.
    uint32_t rid_actpush_low_bw_cnt;   //push count for peer with low bandwidth.
    uint32_t rid_actpush_low_online_cnt;
    uint32_t rid_actpush_high_disk_cnt;
    int rid_priority;               //priority for the RID, set for VIP RIDS.
    uint32_t rid_push_target;    //VIP content push target.
    RID_Internal_Struct(const std::string &ridstr, uint32_t actpush_count = 0, uint32_t paspush_count= 0, int priority = 0, uint32_t push_target= 0)
    {
        rid_string = ridstr;
        rid_actpush_count = actpush_count;
        rid_paspush_count = paspush_count;
        rid_actpush_low_bw_cnt = 0;
        rid_actpush_low_online_cnt = 0;
        rid_actpush_high_disk_cnt = 0;
        rid_priority = priority;
        rid_push_target = push_target;
    }

    RID_Internal_Struct() : rid_actpush_count(0), rid_paspush_count(0), rid_actpush_low_bw_cnt(0), rid_actpush_low_online_cnt(0), rid_actpush_high_disk_cnt(0), rid_priority(0), rid_push_target(0) {}
};

struct Episode
{
    uint32_t eindex;
    uint32_t timestamp;
    uint32_t content_id;
    uint32_t bitrate;
    uint32_t bitrate_type;
    std::string filename;
    std::string encode_name;
    uint32_t total_active_pushed;
    std::vector<RID_Internal_Struct> rid_array;

    Episode() : eindex(0),timestamp(0), content_id(0), total_active_pushed(0), bitrate(0), bitrate_type(0) {}
};

inline bool operator < (const Episode & e1, const Episode &e2 )
{
    return ( (e1.eindex < e2.eindex) || 
             ((e1.eindex == e2.eindex) && (e1.bitrate_type < e2.bitrate_type)) );
} 

struct PlayHistory_Index
{
    uint32_t ep_index;
    uint32_t seg_index;
    std::string show_name;
    uint32_t played_duration;
    uint16_t dw_segment_num;
    uint16_t bitrate_type;

    PlayHistory_Index() : ep_index(0), seg_index(0), played_duration(0), dw_segment_num(0), bitrate_type(0) {}
};

struct RID_Tuple
{
    uint32_t content_id;
    uint32_t seg_index;
    std::string encode_name;
    RID_Internal_Struct rid_full;

    RID_Tuple(uint32_t p_content_id, uint32_t p_seg_index, const std::string &p_ep_name, const RID_Internal_Struct &p_rid)
    {
        content_id = p_content_id;
        seg_index = p_seg_index;
        encode_name = p_ep_name;
        rid_full = p_rid;
    }

    RID_Tuple() : content_id(0), seg_index(0) {}
};

struct Content
{
    uint32_t internal_id;       // index for latest epside
    std::string name;
    std::string content_type;   // content type: movie, TV, keyword
    uint32_t last_seen;			// last seen timestamp (in epoch time)
    uint32_t seen_count;	    // the number of times beening hot.
    uint32_t last_category_id;  // reserved: for early-push, use last
                                //    episode's data to predict this episode.

    std::set<Episode> episodes;

    Content() : internal_id(0), last_seen(0), seen_count(0), last_category_id(0) {}
};

typedef std::set<Episode>::iterator EpisodesIterator;
typedef std::set<Episode>::const_iterator EpisodesConstIterator;
typedef std::map<std::string, RID_Tuple>::iterator VIPRIDIterator;
typedef std::map<std::string, RID_Tuple>::const_iterator VIPRIDConstIterator;
typedef std::map<std::string, Content>::iterator ContentIterator;

class ContentManager
	: public boost::enable_shared_from_this<ContentManager>
{
public:
    typedef boost::shared_ptr<ContentManager> p;

public:
    ContentManager();
    void Start();
    void Stop();
    bool SearchContentByName(const std::string &cname, const std::string &showtype, ContentIterator &citer);
    int AddContentByName(const std::string &showname, const std::string &showtype, const int eindex);

    bool FilterShowName(const std::string& raw_name, PlayHistory_Index &matched_history_item);
    static uint32_t get_epoch_time();

    void TestContentManager();
    void UpdateConfig(const ContentManagerCfg& m_inconfig);
    void OnTimerElapsed(framework::timer::Timer * timer);
    int GetPushRIDs(PeerInfo &peer_info, const std::vector<protocol::PlayHistoryItem> &play_hist_vec, std::vector<protocol::PushTaskItem> &ptask_list);

private:
    std::map<std::string, Content> m_content_map;
    std::map<std::string, RID_Tuple> m_viprid_data;
    std::map<std::string, int> m_hotrid_makeup_map;
    std::set<int> m_good_nat_set;

    std::string m_local_data_file;
    std::string m_local_viprid_file;
    std::string m_new_hot_file;
    std::string m_new_upload_file;
    std::string m_new_viprid_file;
    std::string m_new_ridmakeup_file;
    std::string m_per_minute_key;
    bool  m_started;
    int m_next_internal_id;
    int internal_timer_count;
    uint32_t m_last_second;
    uint32_t m_last_quota;
    std::string reg_exp1;
    std::string reg_exp2;
    std::string newreg_exp1;
    std::string newreg_exp2;
    ContentQuotaScheduler m_scheduler;

    ContentIterator actpush_content_iter;
    uint32_t actpush_episode_index;
    uint32_t actpush_rid_index;
    int32_t actpush_episode_bitrate_type;
    bool actpush_low_bw_done;
    bool actpush_high_disk_done;
    bool actpush_low_online_done;
    bool vippush_low_bw_done;
    VIPRIDIterator vippush_rid_iter;

    ContentManagerCfg m_config;
#ifndef TEST_CONTENT_MANAGER
    framework::timer::PeriodicTimer m_readnewhot_timer;
#endif

private:
    int load_local_data_file();
    int save_local_data_file(); 
    int save_local_viprid_file();
    int load_test_data();
    void load_scheduler_session();
    void save_scheduler_session();
    std::string get_timestr();
    struct Content convert_str_to_content();

    inline bool IsPeerBWLow(const PeerInfo &pinfo){
        return (pinfo.upload_bandwidth < m_config.CONTENT_ACTPUSH_BIG_BANDWIDTH);
    }
    inline bool IsPeerDiskHigh(const PeerInfo &pinfo){
        return ((pinfo.used_disk_size/1000000) >= m_config.CONTENT_ACTPUSH_BIG_FREE_DISK);
    }
    inline bool IsPeerOnlineLow(const PeerInfo &pinfo) {
        if (pinfo.total_disk_size == 0) {
            //this is the old version.
            return false;
        }
        return (pinfo.online_percent < m_config.CONTENT_ACTPUSH_HIGH_ONLINE_TIME);
    }
    inline bool IsPeerNATGood(const PeerInfo &pinfo) {
        return (m_good_nat_set.find(pinfo.nat_type) != m_good_nat_set.end());
    }
    void CreateURL(const RID_Tuple &r_tp, std::string &p_string);
    void DumpContents();
    void DumpContent(const Content &);
    void LoadNewContentsAndEpisodes();
    void ReLoadNewVIPRIDs();
    int64_t RebuildActivePushList();
    int64_t RebuildVIPRIDList();
    void ResetPushListAndCalculateLimit();
    int RetireContents();
    int RetireVIPRIDs(const bool newcontent_loaded);
    int CreatePushRIDs(Content &ct, const int eindex, const uint16_t bitrate_type, const int ridindex, const int rnum, std::vector<RID_Tuple> &p_rids);
    int CreateActivePushRIDs(Content &ct, const PeerInfo &pinfo, uint32_t &eindex, int32_t &bitrate_type_index, uint32_t &ridindex, const int rnum, std::vector<RID_Tuple> &p_rids);
    int CreateVIPPushRID(RID_Tuple &one_rid, const PeerInfo &pinfo, std::vector<RID_Tuple> &p_rids, int &total_full_rid);
    int GetPassivePushRIDs(PeerInfo &peer_info, const std::vector<protocol::PlayHistoryItem> &play_hist_vec, std::vector<protocol::PushTaskItem> &ptask_list); 
    int GetActivePushRIDs(PeerInfo &peer_info, std::vector<protocol::PushTaskItem> &ptask_list);
    int GetVIPPushRIDs(PeerInfo &peer_info, std::vector<protocol::PushTaskItem>& ptask_list); 
    int CreateTaskItemFromRID(std::vector<RID_Tuple> &m_rids, const int p_quota, std::vector<protocol::PushTaskItem>& ptask_list);
    bool IsRIDReadyForPeerActPush(const PeerInfo &pinfo, const RID_Internal_Struct &p_rid);
    void IncrementRIDActPushCount(const PeerInfo &pinfo, RID_Internal_Struct &p_rid);
    bool AddEpisode(Content& ct, const Episode& ep);
    void AddContentToMap(Content &ct, int inserthead = 0);
    bool InsertVIPRID(const std::string &ridname, const RID_Tuple &one_rid, const int inserthead = 0);
    void Load_New_Hot_Content();
    void Load_New_Upload_Episodes();
    typedef enum LoadMode {
        LOAD_NEW_FILE = 0,
        LOAD_SAVE_FILE = 1
    };
    void Load_New_VIP_RIDs(LoadMode lmode);
    void Load_New_RID_Makeup();
    void ProcessHotRIDMakeup();
    bool IsEpisodeActivePushFull(const Episode &p_episode);
    int  GetNumPushRIDBasedOnHistory(const PlayHistory_Index &p_hist);
    bool BackupFile(const std::string &p_filename, bool bRemoveOrigin);
    vector<Guid> ParseBlockMd5s(const std::string& block_str);
    protocol::RidInfo FetchRidInfo(const std::string& playurl);
    bool GetRidShortString(const std::string& playurl, std::string &rid_str);
    void UpdateGoodNatSet(const std::string &nat_string);
};

PUSH_SERVER_NAMESPACE_END

#endif // __CONTENT_MANAGER_H__
