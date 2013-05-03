// P2PMonitorDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "P2PDlg.h"
#include "UploadDlg.h"
#include "LiveP2PDlg.h"
#include "FileVersion.h"
#include "LogMonitor.h"

//#include <boost/asio.hpp>
//#include <boost/bind.hpp>
//#include <boost/asio/io_service.hpp>
//#include <boost/timer.hpp>
#include "afxwin.h"
#include "statistic/StatisticsRequest.h"
#include "statistic/StatisticStructs.h"
#include "statistic/StatisticsData.h"

class StatisticsData;

struct PeerBlock
{
    char name[256];
    size_t size;
    const void * ptr;
};

struct PeerInfo
{
    size_t number;
    char peer_pid[20];
    PeerBlock *block;
};

// struct LabelInfo
// {
//     WORD labelID;
//     int left_old;
//     int right_old;
//     int top_old;
//     int bottom_old;
// };
typedef bool (*PPBoxStart)(char *ip);
typedef bool (*PPBoxStop)();
typedef bool (*PPBoxGetInfo)(PeerInfo *peer_info);

// CP2PMonitorDlg 对话框
class CP2PMonitorDlg : public CDialog
{
// 构造
public:
    CP2PMonitorDlg(CWnd* pParent = NULL);    // 标准构造函数

// 对话框数据
    enum { IDD = IDD_P2PMONITOR_DIALOG };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持


// 实现
protected:
    HICON m_hIcon;

    // 生成的消息映射函数
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    CTabCtrl m_tabctrl;
    afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
    void DoTab(int sel);
    void SetDlgState(CWnd* pWnd, bool bShow);
    void OnClose();
    static void ThreadRun(LPVOID param);
    LRESULT OnGoto(WPARAM wParam, LPARAM lParam);
    LRESULT OnTimer(WPARAM wParam, LPARAM lParam);

//private:
    CP2PDlg* m_p2p_dlg;
    LiveP2PDlg* m_live_p2p_dlg;
#if !_PUBLIC_VERSION_
    CUploadDlg* m_upload_dlg;
#endif
    LogMonitor* m_log_dlg;

    CImageList m_imagelist;

//public:
    // 进程信息
    CString m_strProcessName;
    CString m_strProcessID;
    CString m_strProcess;

    PeerInfo m_ppbox_peer_info;

    // 
    HINSTANCE hDll;
    
    PPBoxStart ppbox_start;
    PPBoxStop  ppbox_stop;
    PPBoxGetInfo ppbox_getinfo;

    CRect m_rect;
    vector<LabelInfo> m_label_info;
    vector<WORD> m_label_id;

    bool created_p2p_dlg;
    bool created_up_dlg;
    bool created_live_p2p_dlg;

    // 进程选择
    CComboBox m_process_select;
    afx_msg void OnBnClickedExport();
    afx_msg void OnBnClickedListUpdate();
    afx_msg void OnBnClickedFillZero();
    afx_msg void OnBnClickedSaving();
    afx_msg void OnBnClickedRunPpap();
    afx_msg void OnBnClickedIsTop();
    afx_msg void OnBnClickedRunPplive();
    afx_msg void OnBnClickedConnectRemote();
    afx_msg void OnBnClickedClean();
    afx_msg void OnBnClickedKillAll();

    time_t m_last_time;
    time_t m_cur_time;

    bool IsPass();
    void SetFlag();
    void LoadConfig();
    
public:
    bool ReadShareMemory(CString name, int size, LPVOID p);
public:
    // PPBOX远程IP列表
    CComboBox m_remote_ip;
public:
    afx_msg void OnBnClickedRadioPc();
public:
    afx_msg void OnBnClickedRadioPpbox();
public:
    afx_msg void OnBnClickedButtonImport();

    
public:
    afx_msg void OnBnClickedRadioOnline();
public:
    afx_msg void OnBnClickedRadioImport();

    
public:
//    afx_msg void OnEnChangeEdit1();
public:
//    afx_msg void OnEnChangeEdit1();
public:
    afx_msg void OnBnClickedButtonStart();
public:
    afx_msg void OnBnClickedButtonPause();
public:
    afx_msg void OnBnClickedButtonStop();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnSize(UINT nType, int cx, int cy);


    CRect    m_rcOriginalRect;
    int m_nCurHeight;
    int m_nCurWidth;

    int m_ori_width;
    int m_ori_hight;
//    afx_msg BOOL OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
//    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

    void CreateChart(CString filename);
    afx_msg void OnBnClickedButtonChart();

    void ChangeLabelSize(int, int);
    void AddLabelInfo();
private:
    bool Deserialize(const char* raw_data, int raw_data_size, std::vector<boost::shared_ptr<statistic::StatisticsData> >& statistics_datas);

public:
    afx_msg void OnBnClickedButtonNextSecond();
    void CreateCsvFromGZ();
    CString m_filename;
    CString m_csv_file_name;
};

