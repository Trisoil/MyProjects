//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"

#include "MisSync.h"
#include "IPLib.h"
//#include "IOUtil.h"
#include "io/FileControl.h"
#include "MD5.h"

#ifdef WIN32
#include <WinInet.h>
#else
#endif

#include <set>
#include <map>
#include <ctime>
#include <sstream>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem/path.hpp>

#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "Urlmon.lib")

BOOTSTRAP_NAMESPACE_BEGIN

using std::set;
using std::ostringstream;
FRAMEWORK_LOGGER_DECLARE_MODULE("Bootstrap");

MisSync::MisSync(boost::asio::io_service& io_service)
{
    m_is_running = false;
}

void MisSync::Start()
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::MisSync(), "MisSync::Start()");
    m_is_running = true;
}

void MisSync::Stop()
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::MisSync(), "MisSync::Stop()");
    m_is_running = false;
}

void MisSync::UpdateConfig(const string& iplib_url,
                           const string& index_url,
                           const string& stun_url,
                           const string& isp_url,
                           const string& config_string_url,
                           const string& mis_dir,
                           const string& conf_dir,
                           boost::uint32_t bak_retain_time,
                           const string& sn_url,
                           const string & vip_sn_url)
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::MisSync(), "MisSync::UpdateConfig()");
    m_iplib_url = iplib_url;
    m_index_url = index_url;
    m_stun_url = stun_url;
    m_isp_url = isp_url;
    m_config_string_url = config_string_url;
    m_mis_dir = mis_dir;
    m_sn_url = sn_url;
    m_vip_sn_url = vip_sn_url;
    m_conf_dir = conf_dir;
    m_mis_dir_length = mis_dir.length();

    m_bak_retain_time = bak_retain_time;

    if (false == my_file_io::AccessDir(m_mis_dir))
    {
        my_file_io::MakeDir(m_mis_dir);
    }
}

void MisSync::Sync(boost::function<void()> handler)
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::MisSync(), "MisSync::Sync()");
    m_mis_sync_handler = handler;

    // 先下载IP地址库
    if (false == m_iplib_url.empty()
        && false == m_index_url.empty()
        && false == m_stun_url.empty()
        && false == m_isp_url.empty()
        && false == m_config_string_url.empty()
        && false == m_sn_url.empty()
        && false == m_vip_sn_url.empty())
    {
        boost::filesystem::path file_path(m_mis_dir);
        file_path = file_path / "bs_iplib";
        DownloadTask dt(m_iplib_url, file_path.native_file_string(), true);
        m_downloading_task = dt;
        m_need_down_task.push_back(dt);
        
        DownloadTask *download_task = new DownloadTask(dt);
        boost::thread(boost::bind(&MisSync::HttpDownload, this, download_task));

    }
    else
    {
        LOG4CPLUS_ERROR(bootstrap::Loggers::MisSync(), "m_iplib_url.empty(): " << m_iplib_url.empty()
            << ", m_index_url.empty() " << m_index_url.empty()
            << ", m_stun_url.empty() " << m_stun_url.empty()
            << ", m_isp_url.empty() " << m_isp_url.empty()
            << ", m_config_string_url.empty() " << m_config_string_url.empty()
            << ", m_sn_url.empty() " << m_sn_url.empty()
            << ", m_vip_sn_url.empty() " << m_vip_sn_url.empty());
    }

}

void MisSync::OnIpLibDownOver(const string& iplib_path)
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::MisSync(), "MisSync::OnIpLibDownOver()");
    IPLib tmp_iplib;

    if (false == tmp_iplib.TryImportIpAddrs(iplib_path))
    {
        LOG4CPLUS_ERROR(bootstrap::Loggers::MisSync(), "Import ip lib failed");
        std::cout << "Import ip lib failed" << std::endl;
        boost::filesystem::path file_path(m_conf_dir);
        file_path = file_path / "bs_iplib";

        if (false == tmp_iplib.TryImportIpAddrs(file_path.string()))
        {
            LOG4CPLUS_ERROR(bootstrap::Loggers::MisSync(), "Import local ip lib failed");
            std::cout << "Import local ip lib failed, too" << std::endl;
            return;
        }

        // 如果新下载的解析不成功，而本地已有的能解析成功，则用本地已有的替换掉新下载的，免得在同步文件时本地被替换掉
        my_file_io::FMove(file_path.string(), iplib_path, false);
    }

    set<boost::uint32_t> ac_list = tmp_iplib.GetAcList();
    boost::uint32_t public_ac = tmp_iplib.PublicAc();

    boost::filesystem::path mis_dir_path(m_mis_dir);
    boost::filesystem::path ac_path = mis_dir_path / "bs_ac";

    // 公网isp配置信息的地址
    string public_url;
    ostringstream public_oss;
    public_oss << m_isp_url << "?id=" << public_ac;
    public_url = public_oss.str();
    public_oss.str("");

    public_oss << ac_path.native_file_string() << "_" << public_ac;
    DownloadTask public_dt(public_url, public_oss.str(), false);
    m_need_down_task.push_back(public_dt);

    // 各ISP配置信息的地址
    for (set<boost::uint32_t>::iterator it = ac_list.begin();
        it != ac_list.end(); ++it)
    {
        string isp_url, isp_path;

        ostringstream oss;
        oss << m_isp_url << "?id=" << *it;
        isp_url = oss.str();
        oss.str("");
        oss << ac_path.native_file_string() << "_" << *it;
        isp_path = oss.str();

        m_need_down_task.push_back(DownloadTask(isp_url, isp_path, false));
    }

    // Index Server, Stun Server和supernode server的地址
    boost::filesystem::path index_path, stun_path, config_string_path, sn_path, vip_sn_path;
    index_path = mis_dir_path / "bs_srv_index";
    stun_path = mis_dir_path / "bs_srv_stun";
    config_string_path = mis_dir_path / "bs_config_string";
    sn_path = mis_dir_path / "bs_srv_sn";
    vip_sn_path = mis_dir_path / "bs_srv_vip_sn";

    m_need_down_task.push_back(DownloadTask(m_index_url, index_path.native_file_string(), false));
    m_need_down_task.push_back(DownloadTask(m_stun_url, stun_path.native_file_string(), false));
    m_need_down_task.push_back(DownloadTask(m_config_string_url, config_string_path.native_file_string(), false));
    m_need_down_task.push_back(DownloadTask(m_sn_url, sn_path.native_file_string(), false));
    m_need_down_task.push_back(DownloadTask(m_vip_sn_url, vip_sn_path.native_file_string(), false));

    ClearMisDir();
}

boost::uint32_t MisSync::HttpDownload(void * param, DownloadTask *down_task)
{
    MisSync *p_mis_sync = (MisSync *)param;

    if (NULL == p_mis_sync)
    {
        return 1;
    }

    if (down_task->url.empty())
    {
        return 2;
    }

    boost::system::error_code our_ec;

    util::protocol::HttpClient *http_client = new util::protocol::HttpClient(MainThread::IOS());
    
    boost::system::error_code *ec = new boost::system::error_code();
    
    http_client->fetch_get(
        down_task->url, 
        *ec);

    if(!(*ec))
    {
        boost::asio::streambuf & data = http_client->response().data();
        boost::asio::const_buffer respond_data = data.data();
        const char * p_d = boost::asio::buffer_cast<const char *>( respond_data );
        int size = boost::asio::buffer_size( respond_data );

        FILE * fp = fopen((down_task->download_path + ".tmp").c_str(), "wb");
        fwrite( p_d , size , 1 , fp );
        fclose( fp );
    }
    else
    {
        LOG4CPLUS_WARN(bootstrap::Loggers::MisSync(), "download failed, url is " << down_task->url
            << ", error code is " << ec->message());
    }
    delete http_client;

    MainThread::Post(boost::bind(&MisSync::OnHttpDownloadResult, p_mis_sync, ec, down_task));

    return 0;
}

void MisSync::OnHttpDownloadResult(const boost::system::error_code* ec, DownloadTask *dt)
{
    // 防止HttpDownload多线程执行时出现错误
    if (m_need_down_task.empty())
    {
        delete ec;
        delete dt;
        //m_need_down_task为空代表已经下载完了，
        //并且在上一次执行OnHttpDownloadResult时，已经为空了，
        //已经执行了该执行的函数了，so, 直接return
        return;
    }

    m_need_down_task.pop_front();

    //下载成功
    if (!(*ec))
    {

        my_file_io::FMove((dt->download_path + ".tmp"),
            dt->download_path);

        if (dt->is_iplib_)
        {
            OnIpLibDownOver(dt->download_path);
        }

        if (false == m_need_down_task.empty())
        {
            //(*dt) = m_need_down_task.front();
            DownloadTask *down_task = new DownloadTask(m_need_down_task.front());
            boost::thread(boost::bind(&MisSync::HttpDownload, this, down_task));
        }
        else
        {
            SyncMisFolder();

            if (m_is_running)
                m_mis_sync_handler();
        }
    }
    // 下载失败
    else
    {
        LOG4CPLUS_WARN(bootstrap::Loggers::MisSync(), "download failed, url is " << dt->url
            << ", error code is " << ec->message());

        std::string file_name = m_conf_dir + (*dt).download_path.substr(m_mis_dir_length);

        //本地有该文件
        if (my_file_io::AccessFile(file_name))
        {
            //将config_dir中的该文件拷贝到mis_dir中
            my_file_io::FMove(file_name, dt->download_path, false);

            if (dt->is_iplib_)
            {
                OnIpLibDownOver(dt->download_path);
            }

            if (false == m_need_down_task.empty())
            {
                DownloadTask *down_task = new DownloadTask(m_need_down_task.front());
                boost::thread(boost::bind(&MisSync::HttpDownload, this, down_task));
            }
            else
            {
                SyncMisFolder();
                if (m_is_running)
                    m_mis_sync_handler();
            }
        }
        //本地没该文件，并且也没有下到，报错
        else
        {
            std::cout << "HttpDownlaod Failed, Url: " << dt->url << ", File: " << dt->download_path;
            LOG4CPLUS_ERROR(bootstrap::Loggers::MisSync(), "HttpDownlaod Failed, Url: " << dt->url << ", File: " << dt->download_path);
        }
    }
    delete ec;
    delete dt;
}

void MisSync::ClearMisDir()
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::MisSync(), "MisSync::ClearMisDir()");
    set<string> useful_file;
    for (std::deque<DownloadTask>::iterator it = m_need_down_task.begin();
        it != m_need_down_task.end(); ++it)
    {
        boost::filesystem::path fp(it->download_path);
        useful_file.insert(fp.native_file_string());
    }

    list<string> mis_dir_files;
    my_file_io::GetDirFileList(m_mis_dir, mis_dir_files);
    for (list<string>::iterator it = mis_dir_files.begin();
        it != mis_dir_files.end(); ++it)
    {
        boost::filesystem::path fp(*it);

        if (boost::algorithm::iends_with(fp.leaf(), ".bak"))
        {
            if (IsFileNeedDel(fp.leaf()))
            {
                LOG4CPLUS_INFO(bootstrap::Loggers::MisSync(), "备份文件过期，删除: " << fp.native_file_string());
                my_file_io::FDel(*it);
            }
        }
        else if (boost::algorithm::istarts_with(fp.leaf(), "bs_ac")
            && useful_file.find(fp.native_file_string()) == useful_file.end())
        {
            LOG4CPLUS_INFO(bootstrap::Loggers::MisSync(), "文件不再需要，备份: " << fp.native_file_string());
            const std::string& old_name = fp.native_file_string();
            const std::string& bak_name = GenBakFilename(fp.native_file_string());
            my_file_io::FMove(old_name, bak_name, true);
        }
    }
}

void MisSync::SyncMisFolder()
{
    LOG4CPLUS_TRACE_METHOD(bootstrap::Loggers::MisSync(), "MisSync::SyncMisFolder()");
    list<string> mis_dir_files, conf_dir_files;

    my_file_io::GetDirFileList(m_mis_dir, mis_dir_files);
    my_file_io::GetDirFileList(m_conf_dir, conf_dir_files);

    map<string, string> conf_name_map;
    for (list<string>::iterator it = conf_dir_files.begin();
        it != conf_dir_files.end(); ++it)
    {
        boost::filesystem::path fp(*it);
        conf_name_map.insert(make_pair(fp.leaf(), *it));
    }

    for (list<string>::iterator it = mis_dir_files.begin();
        it != mis_dir_files.end(); ++it)
    {
        boost::filesystem::path fp(*it);

        if (boost::algorithm::iends_with(fp.leaf(), ".bak"))
        {
            continue;
        }

        map<string, string>::iterator it_map = conf_name_map.find(fp.leaf());
        if (it_map != conf_name_map.end())
        {
            if (IsFileSame(*it, it_map->second))
            {
                my_file_io::FDel(*it);
            }
            else
            {
                my_file_io::FMove(it_map->second, GenBakFilename(*it));
                my_file_io::FMove(*it, it_map->second);
            }
            conf_name_map.erase(it_map);
        }
        else
        {
            boost::filesystem::path new_path(m_conf_dir);
            new_path = new_path / fp.leaf();
            my_file_io::FMove(*it, new_path.native_file_string());
        }
    }

    // clear the config dir
    for (map<string, string>::iterator it_map = conf_name_map.begin();
        it_map != conf_name_map.end(); ++it_map)
    {
        boost::filesystem::path newfp(m_mis_dir);
        newfp /= it_map->first;

        const string& old_name = it_map->second;
        const string& bak_name = GenBakFilename(newfp.native_file_string());

        my_file_io::FMove(old_name, bak_name, true);

        LOG4CPLUS_INFO(bootstrap::Loggers::MisSync(), "File: " << old_name << " not need, bakup to: " << bak_name);
    }
}
                                                                                                           
bool MisSync::IsFileSame(const string& lname, const string& rname)
{
    bool is_same = false;

    FILE *lfp, *rfp;
    lfp = fopen(lname.c_str(), "rb");
    rfp = fopen(rname.c_str(), "rb");

    if (lfp && rfp)
    {
        unsigned char *lbuf = 0, *rbuf = 0;
        size_t llen, rlen;
        llen = my_file_io::FLen(lname);
        rlen = my_file_io::FLen(rname);

        lbuf = (unsigned char *)malloc(llen);
        rbuf = (unsigned char *)malloc(rlen);

        if (lbuf && rbuf)
        {
            Guid lmd5, rmd5;

            fread(lbuf, llen, 1, lfp);
            fread(rbuf, rlen, 1, rfp);

            lmd5 = CMD5::GenerateMD5(lbuf, llen);
            rmd5 = CMD5::GenerateMD5(rbuf, rlen);

            is_same = (lmd5 == rmd5);
        }

        if (lbuf) free(lbuf);
        if (rbuf) free(rbuf);

        fclose(lfp);
        fclose(rfp);
    }

    return is_same;
}

bool MisSync::IsFileNeedDel(const string &name)
{
    using namespace boost::gregorian;

    string::size_type pos1, pos2;
    pos2 = name.rfind('.');
    pos1 = name.rfind('_');

    string time_str = name.substr(pos1+1, 8);

    date last_date(from_undelimited_string(time_str));
    date today = day_clock::local_day();
    days days_alive = today - last_date;

    if (days_alive > days(m_bak_retain_time))
    {
        return true;
    }

    return false;
}

string MisSync::TimeStr()
{
    time_t now_time = time(0);

    struct tm *ptm = localtime(&now_time);

    char time_buf[2048];
    sprintf(time_buf, "%04d%02d%02d%02d%02d%02d", 
        ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, 
        ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

    return time_buf;
}

string MisSync::GenBakFilename(const string& fname)
{
    ostringstream oss;
    oss << fname << "_" << TimeStr() << ".bak";
    return oss.str();
}

BOOTSTRAP_NAMESPACE_END
