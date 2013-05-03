#pragma once
#include "DataStruct.h"
#include "Common.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "statistic/StatisticsData.h"

// LiveP2PDlg dialog

using namespace p2p_monitor;

class LiveP2PDlg : public CDialog
{
	DECLARE_DYNAMIC(LiveP2PDlg)

public:
	LiveP2PDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~LiveP2PDlg();

// Dialog Data
	enum { IDD = IDD_LIVE_P2P_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();

    void OnShareMemoryTimer();
    void SetProcessID(CString processid);
    void ShowReport();
    void InitListControl();
    void CreateCSVFileName();
    void SetStart();
    void SetSaving(BOOL is_saving);
    void Export();
    void ClearContent();
    void MonitorFillZero();
    void Display(CString rid);
    void ChangeLabelSize(int cx, int cy, int w, int h);
    void AddLabelInfo();
    void ShowStatisticLog();
    void SetListReportState(BOOL is_list_update);
    void ReadFile();
    void CreatePeerList(int index1, int index2);

private:
    CString m_strProcessID;
    // LIVE_DOWNLOADDRIVER_STATISTIC_INFO m_live_download_info;

    int m_live_http_speed;
    CString m_live_p2p_speed;

    STASTISTIC_INFO m_si;

    //CString m_switch_state;
    CString m_switch_state;
    IpLocater m_locater;
    P2P_CONNECTION_INFO m_last_pc[100];
    int peer_count;

    std::string csvFileName;
    bool m_is_start;
    int m_is_saving;
    CString csvFilePath;
    
    int connect_peer;
    int ippool;
    // std::vector<CString> m_import_file;

    // еепРап
    int m_sort_col;
    int m_sort;
    BOOL m_list_update;
    CString m_sel_ip;

    boost::uint32_t m_unique_requests[20];
    boost::uint32_t m_receives[20];
    boost::uint32_t m_unique_receives[20];
    boost::uint32_t m_send_subpiece_count[20];

    boost::uint8_t rate_index;
    UPLOAD_INFO m_upload_info;

    boost::uint8_t second_index_;

public:
    CString m_receive;
    CString m_re_rate;
    CString m_lost_rate;
    CString m_ip;
    CString m_save_rate;
    CString m_strLocation;
    UINT m_http_server_status;
    int m_total_http;
    int m_total_p2p;
    UINT32 m_live_p2p_subpiece_speed;
    CListCtrl m_peerlist;
    CString m_unic_request;
    CString m_unic_receive;
    CString m_peer_connect_count;
    bool m_is_online;
    bool m_is_pause;
    int m_line_number;
    int last_line_number_;
    CString FilePathName;
    CCrypt my_crypt;
    int m_datarate;
    int m_cache_size;
    CString m_cache_interval;
    int m_playing_position;
    int m_left_capacity;
    int m_rest_time;
    CListBox m_ridlist;
    int m_cache_start;
    int m_cache_end;
    afx_msg void OnLbnSelchangeRidList();
    UINT m_data_rate_level_;
    int m_live_point_;
    UINT m_jump_times_;
    UINT m_checksum_failed_times_;
    UINT m_is_2300_resttime_enouth_;
    //CString m_channel_id;
	int m_import_file_type;
	std::vector<boost::shared_ptr<statistic::StatisticsData> > m_statistics_datas;
	int m_second;
	std::vector<CString> m_import_file;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	BOOL m_is_created;
	int m_live_p2p_ori_h;
	int m_live_p2p_ori_w;
	CRect m_rect;
	vector<LabelInfo> m_label_info;
	vector<WORD> m_label_id;
    int m_udpserver_download_bytes;
    UINT m_pms_status;
    int m_unique_id;
    UINT m_mem_unique_id;
    CString m_udp_server_speed;
    UINT m_live_pause;
    UINT m_live_replay;
    afx_msg void OnLvnColumnclickList1(NMHDR *pNMHDR, LRESULT *pResult);
private:
    UINT upload_speed_;
    UINT upload_connection_count_;
    UINT upload_speed_limit_;
    CString slide_lost_rate_;
    CString slide_re_rate_;
    CString missing_existing_subpiece_count_;
    CString send_subpiece_count_;
};
