/* ======================================================================
 *    MisSync.h
 *    Copyright (c) 2009 Synacast. All rights reserved.
 *
 *    与Mis通信模块
 * ======================================================================
 *      Time        Changer     ChangeLog
 *    ~~~~~~~~~    ~~~~~~~~~    ~~~~~~~~~
 *    2010-03-08     cnhbdu      创建
 */

#ifndef __MIS_SYNC_H__
#define __MIS_SYNC_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

//#include "HttpDownloader.h"
#include "MainThread.h"
#include "BHeader.h"

#include "util/protocol/http/HttpClient.h"

#include <string>

#include <boost/function.hpp>
#include <boost/enable_shared_from_this.hpp>
//#include <deque>

using std::string;

BOOTSTRAP_NAMESPACE_BEGIN

class bs_category : public boost::system::error_category
{
public:
    bs_category(const std::string& msg) : m_msg(msg) {}

    const char* name() const
    {
        return "bootstrap.error";
    }

    std::string message(int value) const
    {
        return m_msg;
    }

    std::string m_msg;
};

struct DownloadTask
{
    std::string url;
    std::string download_path;
    bool is_iplib_;

    DownloadTask(const std::string& d_url, const std::string& d_path, bool is_iplib)
        : url(d_url), download_path(d_path), is_iplib_(is_iplib) {}

    DownloadTask() {}
};

class MisSync
    : public boost::enable_shared_from_this<MisSync>
{
public:
    MisSync(boost::asio::io_service& io_service);

    static boost::uint32_t HttpDownload(void *, DownloadTask *down_task);
    
public:

    void Start();

    void Stop();

    void UpdateConfig(
        const string& iplib_url,
        const string& index_url,
        const string& stun_url,
        const string& isp_url,
        const string& config_string_url,
        const string& mis_dir,
        const string& conf_dir,
        boost::uint32_t bak_retain_time,
        const string& sn_url,
        const string & vip_sn_url);

    void OnHttpDownloadResult(const boost::system::error_code* ec, DownloadTask *dt);

    void Sync(boost::function<void()> handler);

    void OnIpLibDownOver(const string& iplib_path);

    void SyncMisFolder();

    static string TimeStr();

private:

    void ClearMisDir();

    string GenBakFilename(const string& fname);

    bool IsFileSame(const string& lname, const string& rname);

    bool IsFileNeedDel(const string &name);

private:
    std::string m_iplib_url;
    std::string m_index_url;
    std::string m_stun_url;
    std::string m_isp_url;
    std::string m_config_string_url;
    std::string m_sn_url;
    std::string m_vip_sn_url;

    string m_mis_dir;
    string m_conf_dir;

    int m_mis_dir_length;

    boost::uint32_t m_bak_retain_time;

    boost::function<void()> m_mis_sync_handler;
//    HttpDownload m_http_download;
    std::deque<DownloadTask> m_need_down_task;
    DownloadTask m_downloading_task;

    bool m_is_running;
    //static util::protocol::HttpClient *http_client;
//    boost::thread* ios_thread;
};

BOOTSTRAP_NAMESPACE_END

#endif // __MIS_SYNC_H__
