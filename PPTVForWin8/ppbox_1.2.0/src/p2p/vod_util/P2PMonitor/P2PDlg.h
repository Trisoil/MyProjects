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


// CP2PDlg �Ի���

class CP2PDlg : public CDialog
{
    DECLARE_DYNAMIC(CP2PDlg)

public:
    CP2PDlg(CWnd* pParent = NULL);   // ��׼���캯��
    virtual ~CP2PDlg();

// �Ի�������
    enum { IDD = IDD_P2P_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
    // RID�б��
    CListBox m_ridlist;
    afx_msg void OnLbnSelchangeRidList();

private:   
    // �ļ�����
    int m_filelen;

    int m_total_http;
    int m_total_p2p;
    CString m_save_rate;

    // �ܷ���
    int m_Send;
    // ���յ�
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

    // ������
    int m_sort_col;

    // �б�ؼ��Ƿ����
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

    // ������
    CString m_re_rate;
    // ������
    CString m_lost_rate;
    // HTTP������״̬
    UINT m_http_server_status;

    // ��ѡ�е�Guid
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
    // ÿ������ & ÿ�������Ӹ���
    CString m_strConnectKick;
    // ȫ��window size
    int m_GlobalWindowSize;
    //ÿ�뷢����������
    int m_GlobalRequestSendCount;
    //�ڴ��ʣ���С
    int m_MemoryPoolLeftSize;
    // ��һƬ�յ�subpiece�����һƬ�Ѿ�ӵ�е�subpiece�ľ���
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