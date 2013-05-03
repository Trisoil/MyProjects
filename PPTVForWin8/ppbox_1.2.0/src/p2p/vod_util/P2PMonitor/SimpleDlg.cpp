// SimpleDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "P2PMonitor.h"
#include "P2PMonitorDlg.h"
#include "P2PDlg.h"
#include "SimpleDlg.h"


// CSimpleDlg 对话框

IMPLEMENT_DYNAMIC(CSimpleDlg, CDialog)

CSimpleDlg::CSimpleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSimpleDlg::IDD, pParent)
{
	m_p2p_monitor_dlg = new CP2PMonitorDlg();
	m_p2p_monitor_dlg->Create(IDD_P2PMONITOR_DIALOG);
	m_p2p_monitor_dlg->ShowWindow(FALSE);
}

CSimpleDlg::~CSimpleDlg()
{
}

void CSimpleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SIMPLE_IP, m_simple_ip);
}


BEGIN_MESSAGE_MAP(CSimpleDlg, CDialog)
	ON_BN_CLICKED(IDC_START_BUTTON, &CSimpleDlg::OnBnClickedStartButton)
	ON_BN_CLICKED(IDC_STOP_BUTTON, &CSimpleDlg::OnBnClickedStopButton)
	ON_BN_CLICKED(IDC_SIMPLE_PC, &CSimpleDlg::OnBnClickedSimplePc)
	ON_BN_CLICKED(IDC_SIMPLE_BOX, &CSimpleDlg::OnBnClickedSimpleBox)
	ON_BN_CLICKED(IDC_SIMPLE_CONNECT, &CSimpleDlg::OnBnClickedSimpleConnect)
//	ON_CBN_EDITCHANGE(IDC_SIMPLE_IP, &CSimpleDlg::OnCbnEditchangeSimpleIp)
	//ON_CBN_SELCHANGE(IDC_SIMPLE_IP, &CSimpleDlg::OnCbnSelchangeSimpleIp)
    ON_WM_CLOSE()
//    ON_STN_CLICKED(IDC_STATIC4, &CSimpleDlg::OnStnClickedStatic4)
END_MESSAGE_MAP()


// CSimpleDlg 消息处理程序

void CSimpleDlg::OnBnClickedStartButton()
{
	// TODO: 在此添加控件通知处理程序代码
	
	m_p2p_monitor_dlg->m_p2p_dlg->SetSaving(TRUE);
	m_p2p_monitor_dlg->m_p2p_dlg->SetStart();
	
	GetDlgItem(IDC_STOP_BUTTON)->EnableWindow(TRUE); 
	GetDlgItem(IDC_START_BUTTON)->EnableWindow(FALSE); 
	((CButton*)GetDlgItem(IDC_SIMPLE_BOX))->SetCheck(TRUE);
}

void CSimpleDlg::OnBnClickedStopButton()
{
	// TODO: 在此添加控件通知处理程序代码
	m_p2p_monitor_dlg->m_p2p_dlg->SetSaving(FALSE);

	GetDlgItem(IDC_STOP_BUTTON)->EnableWindow(FALSE); 
	GetDlgItem(IDC_START_BUTTON)->EnableWindow(TRUE);
	
	CString logName = "日志文件存在: ";
	logName = logName + m_p2p_monitor_dlg->m_p2p_dlg->getCSVFilePath();
	MessageBox( logName );
}

BOOL CSimpleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	((CButton*)GetDlgItem(IDC_SIMPLE_PC))->SetCheck(TRUE);
	GetDlgItem(IDC_SIMPLE_CONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_SIMPLE_IP)->EnableWindow(FALSE);
	LoadConfig();
#if _SIMPLE_CUSTOMER_VERSION_

    GetDlgItem(IDC_STATIC1)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC2)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_SIMPLE_PC)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_SIMPLE_BOX)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC3)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_SIMPLE_IP2)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_SIMPLE_CONNECT)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC4)->ShowWindow(SW_SHOW);
#else
    GetDlgItem(IDC_STATIC4)->ShowWindow(SW_HIDE);
#endif
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSimpleDlg::OnBnClickedSimplePc()
{
	// TODO: 在此添加控件通知处理程序代码
	//((CButton*)GetDlgItem(IDC_RADIO_PC))->SetCheck(TRUE);
	//((CButton*)GetDlgItem(IDC_RADIO_PPBOX))->SetCheck(FALSE);
	m_p2p_monitor_dlg->OnBnClickedRadioPc();
	GetDlgItem(IDC_SIMPLE_CONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_SIMPLE_IP)->EnableWindow(FALSE);	
}

void CSimpleDlg::OnBnClickedSimpleBox()
{
	// TODO: 在此添加控件通知处理程序代码
	//((CButton*)GetDlgItem(IDC_RADIO_PC))->SetCheck(FALSE);
	//((CButton*)GetDlgItem(IDC_RADIO_PPBOX))->SetCheck(TRUE);
	m_p2p_monitor_dlg->OnBnClickedRadioPpbox();
	GetDlgItem(IDC_SIMPLE_CONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_SIMPLE_IP)->EnableWindow(TRUE);
	
}

void CSimpleDlg::OnBnClickedSimpleConnect()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	int cur_sel = m_simple_ip.GetCurSel();
	if (cur_sel != -1)
	{
		char str_remote_ip[MAX_PATH];
		m_simple_ip.GetLBText(cur_sel, str_remote_ip);
		m_p2p_monitor_dlg->ppbox_start(str_remote_ip);
	}
	else
	{
		CString remote_ip;

		GetDlgItemText(IDC_SIMPLE_IP, remote_ip);

		m_p2p_monitor_dlg->ppbox_start((char*)(LPCSTR)remote_ip);
	}
}


void CSimpleDlg::LoadConfig()
{
	FILE *fp;
	if (fp = fopen("ppbox_ip_config.txt", "r"))
	{
		char ip[MAX_PATH];
		while(fgets(ip, MAX_PATH, fp))
		{
			m_simple_ip.AddString(ip);
		}
		fclose(fp);
	}
}

void CSimpleDlg::OnClose()
{
    // TODO: Add your message handler code here and/or call default

    KillTimer(1);
    CDialog::OnClose();

#if _USER_VERSION_
    // 自删除
    char cmd[1024];
    GetEnvironmentVariable("ComSpec",cmd,1024);
    GetModuleFileName(GetModuleHandle(0),cmd+lstrlen(lstrcat(cmd," /c del \"")),512);
    WinExec(cmd,0);
#endif
    exit(0);
}

//void CSimpleDlg::OnStnClickedStatic4()
//{
//    // TODO: Add your control notification handler code here
//}
