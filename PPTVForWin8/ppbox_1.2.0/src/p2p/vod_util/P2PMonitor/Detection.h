#pragma once

#include "boost/asio.hpp"
#include "P2PMonitorDlg.h"
#include "SimpleDlg.h"
#include "afxcmn.h"
#include <set>

// Detection dialog
struct ConcernData
{
    CString m_filename;  // 文件名
    int m_filelen;  // 文件长度
    int m_datarate;  // 码流率
    CString m_strIP;  // CDN IP
    int m_Send;  // 总发送
    int m_Receive;  // 总收到
    CString m_lost_rate;  // 丢包率
    CString m_re_rate;  // 冗余率
    int m_total_http;  //http总下载
    int m_total_p2p;  //p2p总下载
    int m_rest_time;  //剩余时间
    int m_http_kbps;  //http速度
    int m_p2p_kbps;  //p2p速度
    CString m_switch_state;  //状态机
    CString m_peer_connect_count;  //连接数
    int m_full_resource_peer;  // 满资源数的peer
    int m_connect_peer;  // 连接上的peer
    int m_queryed_peer;  // 查询到的peer

    ConcernData()
    {
        m_filename = "";
        m_filelen = 0;
        m_datarate = 0;
        m_strIP = "";
        m_Send = 0;
        m_Receive = 0;
        m_lost_rate = "";
        m_re_rate = "";
        m_total_http = 0;
        m_total_p2p = 0;
        m_rest_time = 0;
        m_http_kbps = 0;
        m_p2p_kbps = 0;
        m_switch_state = "";
        m_peer_connect_count = "";
        m_full_resource_peer = 0;
        m_connect_peer = 0;
        m_queryed_peer = 0;
    }
};

struct TrackerInfo
{
    string ip;
    boost::uint16_t port;

    TrackerInfo()
    {
        ip = "";
        port = 0;
    }
};
class Detection : public CDialog
{
    DECLARE_DYNAMIC(Detection)

public:
    Detection(CWnd* pParent = NULL);   // standard constructor
    virtual ~Detection();

    // Dialog Data
    enum { IDD = IDD_DETECTION_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonDetStart();
    void CheckIP138();
    void CheckIP();
    void TestTinydrag();
    void Ping();
    void GetLocalIP();
    void Test9KAPPPAC();
    void Test9K();
    void Test8888();
    void WorkFunc();
    void DrawProcessControl();
    void Analyze();
    void ReadFile(const CString& line);
    void TestBs();
    void TestTracker();
    void HandleReceiveFrom(int packet_type, int tracker_index, boost::asio::deadline_timer& timer,
        const boost::system::error_code& error,
        size_t bytes_recvd);
    void TimeOutHandler(int packet_type, const boost::system::error_code& error,
        boost::asio::ip::udp::socket& socket, int tracker_index);
    void PingSomeIP(int switch_type, std::set<CString> test_ip);
    void IniTrachkerList();
    void WriteTrackerResult();
private:
    CP2PMonitorDlg* m_p2p_monitor_dlg;
    CSimpleDlg* m_simple_dlg;
    SHELLEXECUTEINFO ShExecInfo;
    CProgressCtrl m_progress_control;
    int m_progress_pos_;
    std::vector<CString> m_import_file;
    std::vector<ConcernData> m_datas;
    std::vector<TrackerInfo> m_tracker_infos;
    boost::uint8_t data_[2000];
    boost::uint8_t tracker_response_data[30][2000];
    bool has_tracker_list;
    bool has_peer_list;
    std::set<CString> m_3000_cdn_ips;
    std::set<CString> m_2300_cdn_ips;
    std::set<CString> m_2200_cdn_ips;

    boost::uint32_t m_switch_3000_seconds;
    boost::uint32_t m_switch_2300_seconds;
    boost::uint32_t m_switch_3200_seconds;
    boost::uint32_t m_switch_2200_seconds;

    boost::uint32_t m_switch_3000_http_total_download;
    boost::uint32_t m_switch_2300_http_total_download;
    boost::uint32_t m_switch_3200_p2p_total_download;
    boost::uint32_t m_switch_2200_http_total_download;
    boost::uint32_t m_switch_2200_p2p_total_download;

    boost::uint32_t m_3200_total_connected_peers;
    boost::uint32_t m_2200_total_connected_peers;
    boost::uint32_t m_2300_total_connected_peers;

    boost::uint32_t m_3200_max_p2p_speed;
    boost::uint32_t m_2200_max_p2p_speed;

    std::string result_ip138;
    std::string result_checkip;
    std::string result_tinydrag;
    std::string result_ping;
    std::string result_9kapppac;
    std::vector<std::string> result_trackers;


};
