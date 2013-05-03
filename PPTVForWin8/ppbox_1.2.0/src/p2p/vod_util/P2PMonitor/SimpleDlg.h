#pragma once
#include "afxwin.h"


// CSimpleDlg �Ի���

class CSimpleDlg : public CDialog
{
	DECLARE_DYNAMIC(CSimpleDlg)

public:
	CSimpleDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSimpleDlg();

// �Ի�������
	enum { IDD = IDD_SIMPLE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStartButton();
public:
	afx_msg void OnBnClickedStopButton();
	void LoadConfig();

private:
	CP2PMonitorDlg* m_p2p_monitor_dlg;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSimplePc();
	afx_msg void OnBnClickedSimpleBox();
	afx_msg void OnBnClickedSimpleConnect();
	//afx_msg void OnCbnEditchangeSimpleIp();
public:
	CComboBox m_simple_ip;
    afx_msg void OnClose();
//    afx_msg void OnStnClickedStatic4();
};
