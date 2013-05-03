#pragma once
#include "DataStruct.h"
#include "afxcmn.h"
#include "Common.h"
#include <vector>

using namespace std;
using namespace p2p_monitor;
// CUploadDlg �Ի���

class CUploadDlg : public CDialog
{
	DECLARE_DYNAMIC(CUploadDlg)

public:
	CUploadDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CUploadDlg();

    BOOL OnInitDialog();

    void OnShareMemoryTimer();
    void SetProcessID(CString processid);
    void ShowReport();
    void SetListReportState(BOOL is_list_update);

    afx_msg void OnLvnColumnclickUploadList(NMHDR *pNMHDR, LRESULT *pResult);

// �Ի�������
	enum { IDD = IDD_UPLOAD_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
    int m_speed_limit;
    CString m_strProcessID;

    UPLOAD_INFO m_upload_info;
public:
    int m_upload_count;
    CListCtrl m_upload_list;

    IpLocater m_locater;

    // ������
    int m_sort_col;

    // �б�ؼ��Ƿ����
    BOOL m_list_update;

    PEER_UPLOAD_INFO m_last_[256];

    int m_sort;

	vector<LabelInfo> m_label_info;
	vector<WORD> m_label_id;
	CRect m_rect;
	int m_up_ori_h;
	int m_up_ori_w;
	bool m_is_created;

	afx_msg void OnSize(UINT nType, int cx, int cy);

	void ChangeLabelSize(int, int, int, int);
	void AddLabelInfo();
    int m_upload_speed;
    int m_upload_speed_limit2;
};
