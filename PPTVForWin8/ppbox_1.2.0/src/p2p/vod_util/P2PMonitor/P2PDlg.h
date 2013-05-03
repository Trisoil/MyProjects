#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "Common.h"
#include "DataStruct.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "statistic/StatisticsData.h"

using namespace std;
using namespace p2p_monitor;


// CP2PDlg 对话框

class CP2PDlg : public CDialog
{
    DECLARE_DYNAMIC(CP2PDlg)

public:
    CP2PDlg(CWnd* pParent = NULL);   // 标准构造函数
    virtual ~CP2PDlg();

// 对话框数据
    enum { IDD = IDD_P2P_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    void SetProcessID(CString processid);
    void OnShareMemoryTimer();
    void InitListControl();
    BOOL OnInitDialog();
    void Export();
    void SetListReportState(BOOL is_list_update);
    void SetSaving(BOOL is_saving);
    void MonitorFillZero();
    void ControlSwitch(int type);
    void ShowReport();
    void SetStart();
    CString getCSVFilePath();
    void ShowStatisticLog();
private:
    CString m_strProcessID;
public:
    // RID列表框
    CListBox m_ridlist;
    afx_msg void OnLbnSelchangeRidList();

private:   
    // 文件长度
    int m_filelen;

    int m_total_http;
    int m_total_p2p;
    CString m_save_rate;

    // 总发送
    int m_Send;
    // 总收到
    int m_Receive;

    CListCtrl m_peerlist;
    afx_msg void OnLvnColumnclickList(NMHDR *pNMHDR, LRESULT *pResult);
    int m_http_kbps;
    int m_p2p_kbps;
    STASTISTIC_INFO m_si;
    CString m_peer_connect_count;
    int m_datarate;
    CString m_switch_state;
    CStatic m_switch_http;
    CStatic m_switch_p2p;

    // 排序列
    int m_sort_col;

    // 列表控件是否更新
    BOOL m_list_update;

    CString m_filename;

    int m_sort;

    int m_last_pc_count;

    int m_is_saving;

    IpLocater m_locater;

    P2P_CONNECTION_INFO m_last_pc[100];

    HICON m_hOn;
    HICON m_hOff;

    std::string csvFileName;
    CString csvFilePath;
    bool m_is_start;

    int full_block_peer_count;
    int peer_count;
    int ip_pool_peer_count;
    
    int m_sn_state;
    
public:
    afx_msg void OnBnClickedSwitchHp();
    afx_msg void OnBnClickedSwitchH();
    afx_msg void OnBnClickedSwitchP();
    afx_msg void OnBnClickedReset();
    void ReadFile();
    void ReadStatisticData();
    CString m_strIP;
    CString m_strLocation;

    // 冗余率
    CString m_re_rate;
    // 丢包率
    CString m_lost_rate;
    // HTTP服务器状态
    UINT m_http_server_status;

    // 被选中的Guid
    CString m_sel_guid;
    int m_NonConsistentSize;

    // 
    HICON h_on;
    HICON h_off;

    void CreateCSVFileName();

public:
    int m_rest_time;
    int m_speed_limit;
    int m_bandwidth;
    bool m_is_online;
    bool m_is_pause;

    CString FilePathName;
    std::vector<CString> m_import_file;
    int m_line_number;
    int last_line_number_;

    vector<LabelInfo> m_label_info;
    vector<WORD> m_label_id;
    CRect m_rect;
    int m_p2p_ori_h;
    int m_p2p_ori_w;
    bool m_is_created;
    int import_file_type;
    int m_second;
    std::vector<boost::shared_ptr<statistic::StatisticsData> > m_statistics_datas;
private:
    void ClearContent();
public:
    // 每秒连接 & 每秒踢连接个数
    CString m_strConnectKick;
    // 全局window size
    int m_GlobalWindowSize;
    //每秒发出的请求数
    int m_GlobalRequestSendCount;
    //内存池剩余大小
    int m_MemoryPoolLeftSize;
    // 第一片空的subpiece与最后一片已经拥有的subpiece的距离
    int m_empty_subpiece_distance;

    afx_msg void OnSize(UINT nType, int cx, int cy);
    void ChangeLabelSize(int, int, int, int);
    void AddLabelInfo();
    int m_p2p_sn_kbps;
    int m_p2p_peer_kbps;
    void Display(CString rid);
    void CreateCsvFromGZ(CString filename);
    int m_total_sn;
};