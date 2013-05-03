// P2PMonitorDlg.cpp : 实现文件
//

#include "stdafx.h"

#include "P2PMonitor.h"
#include "P2PMonitorDlg.h"

#include "DataStruct.h"
#include "Common.h"
#include "resource.h"

#include "comdef.h"
#include <windows.h>
#include <tlhelp32.h>
#include <string.h>

#include "ResizeControl.h"
#include "Decompress.h"
#include "statistic/StatisticsData.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//class StatisticsData;

CResizeControl m_Resize;


// CP2PMonitorDlg 对话框

CP2PMonitorDlg::CP2PMonitorDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CP2PMonitorDlg::IDD, pParent)
    , m_strProcess(_T(""))
    , created_p2p_dlg(FALSE)
    , created_up_dlg(FALSE)
    , created_live_p2p_dlg(FALSE)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    //DECLARE_EASYSIZE
}

void CP2PMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TAB, m_tabctrl);
    DDX_Control(pDX, IDC_PROCESS_SELECT, m_process_select);
#if !_PUBLIC_VERSION_
    DDX_Control(pDX, IDC_REMOTE_IP_LIST, m_remote_ip);
#endif
    
}

BEGIN_MESSAGE_MAP(CP2PMonitorDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_CLOSE()
    //}}AFX_MSG_MAP
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CP2PMonitorDlg::OnTcnSelchangeTab)
    ON_MESSAGE(WM_TIMER, OnTimer)
    ON_BN_CLICKED(IDC_EXPORT, &CP2PMonitorDlg::OnBnClickedExport)
    ON_BN_CLICKED(IDC_LIST_UPDATE, &CP2PMonitorDlg::OnBnClickedListUpdate)
    ON_BN_CLICKED(IDC_SAVING, &CP2PMonitorDlg::OnBnClickedSaving)
    ON_BN_CLICKED(IDC_RUN_PPAP, &CP2PMonitorDlg::OnBnClickedRunPpap)
    ON_BN_CLICKED(IDC_IS_TOP, &CP2PMonitorDlg::OnBnClickedIsTop)
    ON_BN_CLICKED(IDC_RUN_PPLIVE, &CP2PMonitorDlg::OnBnClickedRunPplive)
    ON_BN_CLICKED(IDC_CLEAN, &CP2PMonitorDlg::OnBnClickedClean)
    ON_BN_CLICKED(IDC_KILL_ALL, &CP2PMonitorDlg::OnBnClickedKillAll)
#if !_PUBLIC_VERSION_
    ON_BN_CLICKED(IDC_CONNECT_REMOTE, &CP2PMonitorDlg::OnBnClickedConnectRemote)
#endif
    ON_BN_CLICKED(IDC_RADIO_PC, &CP2PMonitorDlg::OnBnClickedRadioPc)
    ON_BN_CLICKED(IDC_RADIO_PPBOX, &CP2PMonitorDlg::OnBnClickedRadioPpbox)
    ON_BN_CLICKED(IDC_BUTTON_IMPORT, &CP2PMonitorDlg::OnBnClickedButtonImport)
    ON_BN_CLICKED(IDC_RADIO_ONLINE, &CP2PMonitorDlg::OnBnClickedRadioOnline)
    ON_BN_CLICKED(IDC_RADIO_IMPORT, &CP2PMonitorDlg::OnBnClickedRadioImport)
    //    ON_EN_CHANGE(IDC_EDIT1, &CP2PMonitorDlg::OnEnChangeEdit1)
    //ON_EN_CHANGE(IDC_EDIT1, &CP2PMonitorDlg::OnEnChangeEdit1)
    ON_BN_CLICKED(IDC_BUTTON_START, &CP2PMonitorDlg::OnBnClickedButtonStart)
    ON_BN_CLICKED(IDC_BUTTON_PAUSE, &CP2PMonitorDlg::OnBnClickedButtonPause)
    ON_BN_CLICKED(IDC_BUTTON_STOP, &CP2PMonitorDlg::OnBnClickedButtonStop)
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_WM_SIZE()
//    ON_WM_MOUSEHWHEEL()
//ON_WM_GETMINMAXINFO()
ON_BN_CLICKED(IDC_BUTTON_CHART, &CP2PMonitorDlg::OnBnClickedButtonChart)
ON_BN_CLICKED(IDC_BUTTON_NEXT_SECOND, &CP2PMonitorDlg::OnBnClickedButtonNextSecond)
END_MESSAGE_MAP()


class CommandlineSettings
{
private:
    bool auto_save_;

private:

    void AddArgument(const std::wstring& argument)
    {
        std::vector<std::wstring> name_and_value;
        boost::algorithm::split(name_and_value, argument, boost::algorithm::is_any_of(L"="));
        if (name_and_value.size() == 2)
        {
            try
            {
                AddNameValuePair(name_and_value[0], name_and_value[1]);
            }
            catch (boost::system::system_error & error)
            {
            }
        }
    }

    void AddNameValuePair(const std::wstring& name, const std::wstring& value)
    {
        std::wstring uppercase_name(name);
        boost::algorithm::to_upper(uppercase_name);
        if (uppercase_name == std::wstring(L"AUTOSAVE"))
        {
            auto_save_ = boost::lexical_cast<bool>(value);
        }
    }

public:
    void Parse()
    {
        int nArgs = 0;   
        LPWSTR *szArgList = CommandLineToArgvW(GetCommandLineW(), &nArgs);   
        if( NULL != szArgList)   
        {   
            for(int i = 1; i < nArgs; ++i)
            {
                AddArgument(std::wstring(szArgList[i]));
            }
        }

        LocalFree(szArgList);  
    }

    CommandlineSettings()
    {
        auto_save_ = false;
    }

    bool AutoSave() const 
    { 
        return auto_save_; 
    }
};



// CP2PMonitorDlg 消息处理程序

BOOL CP2PMonitorDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);            // 设置大图标
    SetIcon(m_hIcon, FALSE);        // 设置小图标

    // TODO: 在此添加额外的初始化代码
    m_tabctrl.InsertItem(0, "P2P下载", 0);
#if !_PUBLIC_VERSION_
    m_tabctrl.InsertItem(1, "上传", 0);
#endif
    m_tabctrl.InsertItem(2, "Live P2P", 0);
    // m_tabctrl.InsertItem(3, "Log Monitor", 0);

    m_imagelist.Create(16, 16, ILC_COLOR32|ILC_MASK, 4, 0);
    m_imagelist.Add(AfxGetApp()->LoadIcon(IDR_MAINFRAME));

    m_tabctrl.SetImageList(&m_imagelist);

    m_p2p_dlg = new CP2PDlg();
    m_p2p_dlg->Create(IDD_P2P_DIALOG, GetDlgItem(IDC_TAB));
    m_p2p_dlg->InitListControl();

    m_live_p2p_dlg = new LiveP2PDlg();
    m_live_p2p_dlg->Create(IDD_LIVE_P2P_DIALOG, GetDlgItem(IDC_TAB));
    m_live_p2p_dlg->InitListControl();

#if !_PUBLIC_VERSION_
    m_upload_dlg = new CUploadDlg();
    m_upload_dlg->Create(IDD_UPLOAD_DIALOG, GetDlgItem(IDC_TAB));
#endif

    //m_log_dlg = new LogMonitor();
    //m_log_dlg->Create(IDD_DIALOG_LOG, GetDlgItem(IDC_TAB));

    SetDlgItemText(IDC_STATIC_VERSION, FILE_VERSION_STR);
    
    ((CButton*)GetDlgItem(IDC_LIST_UPDATE))->SetCheck(TRUE);
    ((CButton*)GetDlgItem(IDC_RADIO_PC))->SetCheck(TRUE);

    DoTab(0);

    // Load Dll
    
    hDll = LoadLibrary("ppbox_status.dll");
    if (hDll)
    {
        ppbox_start = (PPBoxStart)GetProcAddress(hDll, "PPBOX_StatusStart");
        ppbox_stop = (PPBoxStop)GetProcAddress(hDll, "PPBOX_StatusStop");
        ppbox_getinfo = (PPBoxGetInfo)GetProcAddress(hDll, "PPBOX_StatusGetInfo");
    }
    else
    {
        ppbox_start = NULL;
        ppbox_stop = NULL;
        ppbox_getinfo = NULL;
    }

    SetTimer(1, 1000, 0);

    m_last_time = time(0);
#if _USER_VERSION_
    if (!IsPass())
    {
        exit(0);
    }
#endif

    SetFlag();

    LoadConfig();

    m_p2p_dlg->m_is_online = true;
    m_live_p2p_dlg->m_is_online = true;
    GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE); 
    GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(FALSE); 
    GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE); 
    GetDlgItem(IDC_BUTTON_NEXT_SECOND)->EnableWindow(FALSE); 
    ((CButton*)GetDlgItem(IDC_RADIO_ONLINE))->SetCheck(TRUE);
    
    GetDlgItem(IDC_EDIT_FILE)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_IMPORT)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_CHART)->EnableWindow(FALSE);
    GetWindowRect(m_rcOriginalRect);

    AfxOleInit();//关于Excel的初始化
#if _PUBLIC_VERSION_

    GetDlgItem(IDC_RADIO_PPBOX)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_REMOTE_IP_LIST)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_CONNECT_REMOTE)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_RADIO_ONLINE)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_RADIO_IMPORT)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_EDIT_FILE)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_BUTTON_IMPORT)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_BUTTON_CHART)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC2)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC3)->ShowWindow(SW_HIDE);
#endif   

    m_Resize.SetOwner(this);

    GetClientRect(&m_rect);
    m_ori_hight = m_rect.Height();
    m_ori_width = m_rect.Width();
    AddLabelInfo();
    m_p2p_dlg->AddLabelInfo();
    m_upload_dlg->AddLabelInfo();
    m_live_p2p_dlg->AddLabelInfo();

    CommandlineSettings settings;
    settings.Parse();

    if (settings.AutoSave())
    {
        ((CButton*)GetDlgItem(IDC_SAVING))->SetCheck(TRUE);
        OnBnClickedSaving();
    }

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CP2PMonitorDlg::OnPaint()
{
    //ShowWindow(SW_HIDE);
    if (IsIconic())
    {
        CPaintDC dc(this); // 用于绘制的设备上下文

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // 使图标在工作矩形中居中
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // 绘制图标
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CP2PMonitorDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CP2PMonitorDlg::DoTab(int sel)
{
    BOOL bTab[10] = {FALSE};
    
    bTab[sel] = TRUE;

    SetDlgState(m_p2p_dlg, bTab[0]);
#if !_PUBLIC_VERSION_
    SetDlgState(m_upload_dlg, bTab[1]);
#endif
    SetDlgState(m_live_p2p_dlg, bTab[2]);
    //SetDlgState(m_log_dlg, bTab[3]);
}

LRESULT CP2PMonitorDlg::OnGoto(WPARAM wParam, LPARAM lParam)
{
    CString m_process_info;

    if (m_process_select.GetCount() > 0)
    {
        m_process_select.GetLBText(m_process_select.GetCurSel(), m_process_info);

        CString m_strProcessID = m_process_info.Mid(11, m_process_info.GetLength() - 11);

        m_p2p_dlg->SetProcessID(m_strProcessID);
        m_p2p_dlg->OnShareMemoryTimer();
#if !_PUBLIC_VERSION_
        m_upload_dlg->SetProcessID(m_strProcessID);
        m_upload_dlg->OnShareMemoryTimer();
#endif
        m_live_p2p_dlg->SetProcessID(m_strProcessID);
        m_live_p2p_dlg->OnShareMemoryTimer();
    }

    return 0;
}

void CP2PMonitorDlg::SetDlgState(CWnd* pWnd, bool bShow)
{
    pWnd->EnableWindow(bShow);
    if (bShow)
    {
        pWnd->ShowWindow(SW_SHOW);
        pWnd->CenterWindow();
    }
    else
    {
        pWnd->ShowWindow(SW_HIDE);
    }
}

void CP2PMonitorDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    int sel = m_tabctrl.GetCurSel();
    
    if (sel >= 0)
        DoTab(sel);

    *pResult = 0;
}

void CP2PMonitorDlg::OnBnClickedExport()
{
    // TODO: 在此添加控件通知处理程序代码
    m_p2p_dlg->Export();
}

void CP2PMonitorDlg::OnClose()
{
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
void CP2PMonitorDlg::OnBnClickedListUpdate()
{
    // TODO: 在此添加控件通知处理程序代码
    if (((CButton*)GetDlgItem(IDC_LIST_UPDATE))->GetCheck() == BST_CHECKED)
    {
        m_p2p_dlg->SetListReportState(TRUE);
        m_live_p2p_dlg->SetListReportState(TRUE);
        #if !_PUBLIC_VERSION_
        m_upload_dlg->SetListReportState(TRUE);
#endif
    }
    else
    {
        m_p2p_dlg->SetListReportState(FALSE);
        m_live_p2p_dlg->SetListReportState(FALSE);
        #if !_PUBLIC_VERSION_
        m_upload_dlg->SetListReportState(FALSE);
#endif
    }
}

void CP2PMonitorDlg::OnBnClickedFillZero()
{
    // TODO: 在此添加控件通知处理程序代码
    m_p2p_dlg->MonitorFillZero();
    m_live_p2p_dlg->MonitorFillZero();
}

void CP2PMonitorDlg::OnBnClickedSaving()
{
    // TODO: 在此添加控件通知处理程序代码
    if (((CButton*)GetDlgItem(IDC_SAVING))->GetCheck() == BST_CHECKED)
    {
        m_p2p_dlg->SetSaving(TRUE);
        m_p2p_dlg->SetStart();
        m_live_p2p_dlg->SetSaving(TRUE);
        m_live_p2p_dlg->SetStart();
    }
    else
    {
        m_p2p_dlg->SetSaving(FALSE);
        m_live_p2p_dlg->SetSaving(FALSE);

        if (!m_p2p_dlg->FilePathName.Find("live"))
        {
            CreateChart(m_p2p_dlg->FilePathName);
        }
    }
}
void CP2PMonitorDlg::OnBnClickedRunPpap()
{
    char current_dir[512];
    GetCurrentDirectory(512, current_dir);

    // TODO: 在此添加控件通知处理程序代码

    boost::filesystem::path program_files_path("C:\\Program Files (x86)");
    if (boost::filesystem::exists(program_files_path))
    {
        SetCurrentDirectory("C:\\Program Files (x86)\\Common Files\\PPLiveNetwork");
    }
    else
    {
        SetCurrentDirectory("C:\\Program Files\\Common Files\\PPLiveNetwork");
    }

    STARTUPINFO si = {0};
    si.cb = sizeof(STARTUPINFO);

    PROCESS_INFORMATION pi;

    CreateProcess(NULL, "PPAP.exe /embedding", NULL, NULL, false, 0, NULL, NULL, &si, &pi);

    SetCurrentDirectory(current_dir);
}

void CP2PMonitorDlg::OnBnClickedIsTop()
{
    // TODO: 在此添加控件通知处理程序代码
    if (((CButton*)GetDlgItem(IDC_IS_TOP))->GetCheck() == BST_CHECKED)
    {
        ::SetWindowPos(this->GetSafeHwnd(), CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
    }
    else
    {
        ::SetWindowPos(this->GetSafeHwnd(), CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
    }
}

void CP2PMonitorDlg::OnBnClickedRunPplive()
{
    // TODO: 在此添加控件通知处理程序代码
    HKEY hkey;
    DWORD dwSize = 512;
    char string[512];
    LPCTSTR   Rgspath="Path";

    LONG lRet;

    lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\PPLive.exe\\", NULL, KEY_READ, &hkey);
    if (lRet != ERROR_SUCCESS)
    {
        return;
    }

    lRet = RegQueryValueEx(hkey, Rgspath, NULL, REG_NONE, (LPBYTE)string, &dwSize);

    if(lRet != ERROR_SUCCESS)
    {
        return;
    }

    char current_dir[512];
    GetCurrentDirectory(512, current_dir);

    SetCurrentDirectory(string);
    STARTUPINFO si = {0};
    si.cb = sizeof(STARTUPINFO);

    PROCESS_INFORMATION pi;

    CreateProcess(NULL, "PPlive.exe", NULL, NULL, false, 0, NULL, NULL, &si, &pi);

    SetCurrentDirectory(current_dir);
}

bool CP2PMonitorDlg::IsPass()
{
#if _USER_VERSION_
    HKEY hkey;
    DWORD dwSize = 512;
    char string[512];
    LPCTSTR   Rgspath="Reserved";

    LONG lRet;

    lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\PPLive.exe\\", NULL, KEY_ALL_ACCESS, &hkey);
    if (lRet != ERROR_SUCCESS)
    {
        return false;
    }

    lRet = RegQueryValueEx(hkey, Rgspath, NULL, REG_NONE, (LPBYTE)string, &dwSize);

    if(lRet != ERROR_SUCCESS)
    {
        return true;
    }

    int i = atoi(string);

    if (i < 5)
    {
        return true;
    }

    return false;
#endif    
    return true;

}

void CP2PMonitorDlg::SetFlag()
{
#if _USER_VERSION_
    HKEY hkey;
    DWORD dwSize = 512;
    char string[512] = "1";
    LPCTSTR   Rgspath="Reserved";

    LONG lRet;
    int i;


    lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\PPLive.exe\\", NULL, KEY_ALL_ACCESS, &hkey);
    if (lRet != ERROR_SUCCESS)
    {
        return;
    }

    lRet = RegQueryValueEx(hkey, Rgspath, NULL, REG_NONE, (LPBYTE)string, &dwSize);
    if (lRet != ERROR_SUCCESS)
    {
        // 没读到
    }
    else
    {
        i = atoi(string);
        i++;
        itoa(i, string, 10);
    }

    dwSize = 1;
    lRet = RegSetValueEx(hkey, Rgspath, NULL, REG_SZ, (LPBYTE)string, dwSize);

    if(lRet != ERROR_SUCCESS)
    {
        return;
    }
#endif
}

void CP2PMonitorDlg::OnBnClickedClean()
{
    // TODO: 在此添加控件通知处理程序代码
    // 清空资源文件
    SetCurrentDirectory("C:\\Documents and Settings\\All Users\\Application Data\\PPLive\\Core\\");

    FILE *fp;
    char buf[MAX_PATH];
    if ((fp = fopen("Config.ini", "r")) != NULL)
    {
        while(fgets(buf, MAX_PATH, fp) != NULL)
        {
            if (strstr(buf, "cachepath"))
            {
                string s(buf);
                int pos = s.find('=');
                s = s.substr(pos+1);

                char path[MAX_PATH] = "";
                strcpy(path, s.c_str());
                path[strlen(path)-1] = 0;

                CFileFind filefinder;
                char szFileFind[MAX_PATH];
                sprintf(szFileFind, "%s\\*.*", path);

                BOOL is_find = filefinder.FindFile(szFileFind);
                
                while(is_find)
                {
                    is_find = filefinder.FindNextFile();

                    if (!filefinder.IsDots())
                    {
                        char szFileName[MAX_PATH];
                        strcpy(szFileName, filefinder.GetFileName().GetBuffer(MAX_PATH));

                        if (!filefinder.IsDirectory())
                        {
                            // 文件
                            char szTemFile[MAX_PATH]; 
                            sprintf(szTemFile, "%s\\%s", path, szFileName);
                            DeleteFile(szTemFile); 
                        }
                    }
                }
            }
        }
    }
}

void CP2PMonitorDlg::OnBnClickedKillAll()
{
    // TODO: 在此添加控件通知处理程序代码
    // 遍历进程列表
    HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hProcess == INVALID_HANDLE_VALUE)
    {
        AfxMessageBox("CreateToolhelp32Snapshot error!");
    }

    PROCESSENTRY32 process_info;
    process_info.dwSize = sizeof(process_info);
    bool is_process_over = Process32First(hProcess, &process_info);

    bool is_setcursel = false;

    while (is_process_over)
    {
        if (stricmp(process_info.szExeFile, "PPAP.exe") == 0)
        {
            // PPAP.exe
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, process_info.th32ProcessID);
            TerminateProcess(hProcess, 0);
        }

        if (stricmp(process_info.szExeFile, "PPLive.exe") == 0)
        {
            // PPAP.exe
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, process_info.th32ProcessID);
            TerminateProcess(hProcess, 0);
        }

        if (stricmp(process_info.szExeFile, "PPLiveVA.exe") == 0)
        {
            // PPAP.exe
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, process_info.th32ProcessID);
            TerminateProcess(hProcess, 0);
        }



        is_process_over = Process32Next(hProcess, &process_info);
    }
    m_p2p_dlg->m_ridlist.ResetContent();
    m_p2p_dlg->MonitorFillZero();
    m_live_p2p_dlg->MonitorFillZero();
    CloseHandle(hProcess);

}

// 连接
void CP2PMonitorDlg::OnBnClickedConnectRemote()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData(TRUE);
    
    if (ppbox_stop)
    {
        ppbox_stop();
    }

    if (ppbox_start)
    {
        
        int cur_sel = m_remote_ip.GetCurSel();
        if (cur_sel != -1)
        {
            char str_remote_ip[MAX_PATH];
            m_remote_ip.GetLBText(cur_sel, str_remote_ip);
            ppbox_start(str_remote_ip);
        }
        else
        {
            CString remote_ip;

            GetDlgItemText(IDC_REMOTE_IP_LIST, remote_ip);

            ppbox_start((char*)(LPCSTR)remote_ip);
        }
    }
}

bool CP2PMonitorDlg::ReadShareMemory(CString name, int size, LPVOID p)
{
#if !_PUBLIC_VERSION_
    if (((CButton*)GetDlgItem(IDC_RADIO_PPBOX))->GetCheck())
    {
        // PPBox
        for (int i=0; i<m_ppbox_peer_info.number; i++)
        {
            if (!name.CompareNoCase((m_ppbox_peer_info.block+i)->name))
            {
                memcpy(p, (m_ppbox_peer_info.block+i)->ptr, (m_ppbox_peer_info.block+i)->size);
                return true;
            }
        }
    }
    else if (((CButton*)GetDlgItem(IDC_RADIO_PC))->GetCheck())
#endif
    {
        // PC
        HANDLE hFile = OpenFileMapping(FILE_MAP_READ|FILE_MAP_WRITE,
            FALSE,
            name);

        if (hFile)
        {
            LPVOID lpView = MapViewOfFile(hFile, FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, 0);
            if (lpView)
            {
                memcpy(p, lpView, size);
                UnmapViewOfFile(lpView);
                CloseHandle(hFile);
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            int i = GetLastError();
            return false;
        }
    }
    return false;
}

LRESULT CP2PMonitorDlg::OnTimer(WPARAM wParam, LPARAM lParam)
{
    m_cur_time = time(0);
    if (m_cur_time - m_last_time > 2)
    {
#if _USER_VERSION_
        exit(0);
#endif
    }

    m_last_time = m_cur_time;

    // 保存上次选择的项
    CString last_select_process;
    if (m_process_select.GetCount() > 0)
    {
        m_process_select.GetLBText(m_process_select.GetCurSel(), last_select_process);
    }

    // 清空下拉框
    

    bool is_setcursel = false;
    bool data_ok = true;
#if !_PUBLIC_VERSION_
    if (((CButton*)GetDlgItem(IDC_RADIO_PPBOX))->GetCheck())
    {
        // PPBox
        if (ppbox_getinfo && ppbox_getinfo(&m_ppbox_peer_info))
        {
            m_process_select.ResetContent();

            m_strProcessName.Format("%s", "PPAP.exe");
            m_strProcessID.Format("%s", m_ppbox_peer_info.peer_pid);
            m_strProcess = m_strProcessName + " - " + m_strProcessID;
            m_process_select.AddString(m_strProcess);
        }
        else
        {
            data_ok = false;
        }
    }
    else
#endif
    {
        // PC
        // 遍历进程列表
        m_process_select.ResetContent();

        HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (hProcess == INVALID_HANDLE_VALUE)
        {
            AfxMessageBox("CreateToolhelp32Snapshot error!");
        }

        PROCESSENTRY32 process_info;
        process_info.dwSize = sizeof(process_info);
        bool is_process_over = Process32First(hProcess, &process_info);

        while (is_process_over)
        {
            m_strProcessName.Format("%s", process_info.szExeFile);
            m_strProcessID.Format("%d", process_info.th32ProcessID);
            m_strProcess = m_strProcessName + " - " + m_strProcessID;

            STASTISTIC_INFO si;

            if (ReadShareMemory("PPVIDEO_" + m_strProcessID, sizeof(si), &si)&&("PPAP.exe"==m_strProcessName))
            {
                m_process_select.AddString(m_strProcess);

                // 恢复上次选择
                if (m_strProcess.CompareNoCase(last_select_process) == 0)
                {
                    m_process_select.SetCurSel(m_process_select.GetCount() - 1);
                    is_setcursel = true;
                }
            }

            is_process_over = Process32Next(hProcess, &process_info);
        }

        CloseHandle(hProcess);
    }

    if (!is_setcursel && m_process_select.GetCount() > 0)
    {
        m_process_select.SetCurSel(0);
    }

    CString m_process_info;

    if (m_process_select.GetCount() > 0 && data_ok)
    {
        m_process_select.GetLBText(m_process_select.GetCurSel(), m_process_info);

        int pos = m_process_info.ReverseFind('-');

        CString m_strProcessID = m_process_info.Mid(pos+2, m_process_info.GetLength() - pos - 2);

        m_p2p_dlg->SetProcessID(m_strProcessID);
        m_p2p_dlg->OnShareMemoryTimer();
#if !_PUBLIC_VERSION_
        m_upload_dlg->SetProcessID(m_strProcessID);
        m_upload_dlg->OnShareMemoryTimer();
#endif
        m_live_p2p_dlg->SetProcessID(m_strProcessID);
        m_live_p2p_dlg->OnShareMemoryTimer();
    }
    else if(!(m_p2p_dlg->m_is_online))
    {
        m_p2p_dlg->OnShareMemoryTimer();
        m_live_p2p_dlg->OnShareMemoryTimer();
    }
    if (!(m_p2p_dlg->m_is_online))
    {
        if (!m_p2p_dlg->m_is_pause && m_p2p_dlg->import_file_type == 1 && m_live_p2p_dlg->m_import_file_type == 1)
        {
            // 设计的不好的地方，有待改进
            ++m_p2p_dlg->m_second;
            ++m_live_p2p_dlg->m_second;
            if (m_p2p_dlg->m_second >= m_p2p_dlg->m_statistics_datas.size())
            {
                //m_second = m_statistics_datas.size() - 1;
                m_p2p_dlg->m_is_pause = true;
                m_p2p_dlg->m_second = 0;
                m_p2p_dlg->MonitorFillZero();
                m_live_p2p_dlg->m_is_pause = true;
                m_live_p2p_dlg->m_second = 0;
                m_live_p2p_dlg->MonitorFillZero();
                GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
                GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE); 
                GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(FALSE); 
            }
        }
    }

    return 0;
}

void CP2PMonitorDlg::LoadConfig()
{
    FILE *fp;
    if (fp = fopen("ppbox_ip_config.txt", "r"))
    {
        char ip[MAX_PATH];
        while(fgets(ip, MAX_PATH, fp))
        {
            m_remote_ip.AddString(ip);
        }
        fclose(fp);
    }
}
void CP2PMonitorDlg::OnBnClickedRadioPc()
{
    // TODO: 在此添加控件通知处理程序代码
    ((CButton*)GetDlgItem(IDC_RADIO_PC))->SetCheck(TRUE);
    ((CButton*)GetDlgItem(IDC_RADIO_PPBOX))->SetCheck(FALSE);
}

void CP2PMonitorDlg::OnBnClickedRadioPpbox()
{
    // TODO: 在此添加控件通知处理程序代码
    ((CButton*)GetDlgItem(IDC_RADIO_PPBOX))->SetCheck(TRUE);
    ((CButton*)GetDlgItem(IDC_RADIO_PC))->SetCheck(FALSE);
}

void CP2PMonitorDlg::OnBnClickedButtonImport()
{
    // TODO: 在此添加控件通知处理程序代码
    
    CFileDialog dlg(TRUE);///TRUE为OPEN对话框，FALSE为SAVE AS对话框
    if(dlg.DoModal()==IDOK)
    {
        GetDlgItem(IDC_EDIT_FILE)->SetWindowText(dlg.GetPathName());
        GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_NEXT_SECOND)->EnableWindow(TRUE);
        if (dlg.GetPathName().Find(".csv") != -1)
        {
            m_p2p_dlg->FilePathName=dlg.GetPathName();
            m_p2p_dlg->import_file_type = 0;
            m_live_p2p_dlg->FilePathName = dlg.GetPathName();
            m_live_p2p_dlg->m_import_file_type = 0;
            
            CStdioFile f;
            if (f.Open(dlg.GetPathName(), CFile::modeRead))
            {
                CString line;
                while (f.ReadString(line))
                {
                    m_p2p_dlg->m_import_file.push_back(line);
                    m_live_p2p_dlg->m_import_file.push_back(line);
                }
                f.Close();
                m_p2p_dlg->m_line_number = 0;
                m_live_p2p_dlg->m_line_number = 0;
            }
        }
        else if (dlg.GetPathName().Find(".gz") != -1)
        {
            m_filename = dlg.GetPathName();
            m_p2p_dlg->import_file_type = 1;
            m_live_p2p_dlg->m_import_file_type = 1;
            std::vector<boost::shared_ptr<statistic::StatisticsData> > statistics_datas;
            Decompressor compressor(1000*1024);
            if (compressor.Decompress(dlg.GetPathName().GetBuffer()))
            {
                std::cout<<"decompression completed successfully."<<std::endl;

                int raw_data_size = compressor.GetRawDataSize();
                const char* raw_data = compressor.GetRawData();

                if (Deserialize(raw_data, raw_data_size, statistics_datas))
                {
//                     std::cout<<"deserialization completed successfully."<<std::endl;
//                     XmlCreater xml_creater;
//                     xml_creater.CreateXmlFile(statistics_datas);
//                     CsvCreater csv_creater;
//                     csv_creater.CreateCsvFile(statistics_datas);
                    m_p2p_dlg->m_statistics_datas = statistics_datas;
                    m_live_p2p_dlg->m_statistics_datas = statistics_datas;
                }
                else
                {
                    std::cout<<"deserialization failed."<<std::endl;
                }
            }
        }
    }

}


void CP2PMonitorDlg::OnBnClickedRadioOnline()
{
    // TODO: 在此添加控件通知处理程序代码
    m_p2p_dlg->m_is_online = true;
    m_live_p2p_dlg->m_is_online = true;
    if (((CButton*)GetDlgItem(IDC_RADIO_ONLINE))->GetCheck())
    {
        //IDC_EDIT_FILE IDC_BUTTON_IMPORT IDC_BUTTON_CHART
        GetDlgItem(IDC_EDIT_FILE)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_IMPORT)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_CHART)->EnableWindow(FALSE);

        OnBnClickedButtonStop();
        GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(FALSE);
    }
}

void CP2PMonitorDlg::OnBnClickedRadioImport()
{
    // TODO: 在此添加控件通知处理程序代码
    m_p2p_dlg->m_is_online = false;
    m_live_p2p_dlg->m_is_online = false;
    if (((CButton*)GetDlgItem(IDC_RADIO_IMPORT))->GetCheck())
    {
        //IDC_EDIT_FILE IDC_BUTTON_IMPORT IDC_BUTTON_CHART
        GetDlgItem(IDC_EDIT_FILE)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_IMPORT)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_CHART)->EnableWindow(TRUE);
    }
}

void CP2PMonitorDlg::OnBnClickedButtonStart()
{
    // TODO: 在此添加控件通知处理程序代码
    m_p2p_dlg->m_is_pause = false;
    m_live_p2p_dlg->m_is_pause = false;
    GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE); 
    GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE); 
    GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(TRUE); 

}

void CP2PMonitorDlg::OnBnClickedButtonPause()
{
    // TODO: 在此添加控件通知处理程序代码
    m_p2p_dlg->m_is_pause = true;
    m_live_p2p_dlg->m_is_pause = true;
    GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
    GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE); 
    GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(FALSE); 
}

void CP2PMonitorDlg::OnBnClickedButtonStop()
{
    // TODO: 在此添加控件通知处理程序代码
    m_p2p_dlg->m_is_pause = true;
    m_p2p_dlg->m_line_number = 0;
    m_live_p2p_dlg->m_is_pause = true;
    m_live_p2p_dlg->m_line_number = 0;
    m_p2p_dlg->m_second = 0;
    m_live_p2p_dlg->m_second = 0;
    m_p2p_dlg->MonitorFillZero();
    m_live_p2p_dlg->MonitorFillZero();
    GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
    GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE); 
    GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(FALSE); 
}

void CP2PMonitorDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: Add your message handler code here and/or call default
    SCROLLINFO scrollinfo;
    GetScrollInfo(SB_HORZ,&scrollinfo,SIF_ALL);
    
    switch   (nSBCode)
    {
    case   SB_LEFT:
        ScrollWindow((scrollinfo.nPos-scrollinfo.nMin)*10,0);
        scrollinfo.nPos = scrollinfo.nMin;
        SetScrollInfo(SB_HORZ,&scrollinfo,SIF_ALL);
        break;
    case   SB_RIGHT:
        ScrollWindow((scrollinfo.nPos-scrollinfo.nMax)*10,0);
        scrollinfo.nPos = scrollinfo.nMax;
        SetScrollInfo(SB_HORZ,&scrollinfo,SIF_ALL);
        break;
    case   SB_LINELEFT:
        scrollinfo.nPos -= 1;
        if   (scrollinfo.nPos <scrollinfo.nMin)
        {
            scrollinfo.nPos = scrollinfo.nMin;
            break;
        }
        SetScrollInfo(SB_HORZ,&scrollinfo,SIF_ALL);
        ScrollWindow(10,0);
        break;
    case   SB_LINERIGHT:
        scrollinfo.nPos += 1;
        if   (scrollinfo.nPos> scrollinfo.nMax)
        {
            scrollinfo.nPos = scrollinfo.nMax;
            break;
        }
        SetScrollInfo(SB_HORZ,&scrollinfo,SIF_ALL);
        ScrollWindow(-10,0);
        break;
    case   SB_PAGELEFT:
        scrollinfo.nPos -= 5;
        if   (scrollinfo.nPos <scrollinfo.nMin)
        {
            scrollinfo.nPos = scrollinfo.nMin;
            break;
        }
        SetScrollInfo(SB_HORZ,&scrollinfo,SIF_ALL);
        ScrollWindow(10*5,0);
        break;
    case   SB_PAGERIGHT:
        scrollinfo.nPos += 5;
        if   (scrollinfo.nPos> scrollinfo.nMax)
        {
            scrollinfo.nPos = scrollinfo.nMax;
            break;
        }
        SetScrollInfo(SB_HORZ,&scrollinfo,SIF_ALL);
        ScrollWindow(-10*5,0);
        break;
    case   SB_THUMBPOSITION:
        break;
    case   SB_THUMBTRACK:
        ScrollWindow((scrollinfo.nPos-nPos)*10,0);
        scrollinfo.nPos = nPos;
        SetScrollInfo(SB_HORZ,&scrollinfo,SIF_ALL);
        break;
    case   SB_ENDSCROLL:
        break;
    }
    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CP2PMonitorDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: Add your message handler code here and/or call default
    SCROLLINFO scrollinfo;
    GetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);

    int nMaxPos = m_rcOriginalRect.Height() - m_nCurHeight;

    switch   (nSBCode)
    {
    case   SB_BOTTOM:
        ScrollWindow(0,(scrollinfo.nPos-scrollinfo.nMax)*10);
        scrollinfo.nPos = scrollinfo.nMax;
        SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);
        break;
    case   SB_TOP:
        ScrollWindow(0,(scrollinfo.nPos-scrollinfo.nMin)*10);
        scrollinfo.nPos = scrollinfo.nMin;
        SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);
        break;
    case   SB_LINEUP:
        scrollinfo.nPos -= 1;
        if   (scrollinfo.nPos <scrollinfo.nMin)
        {
            scrollinfo.nPos = scrollinfo.nMin;
            break;
        }
        SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);
        ScrollWindow(0,10);
        break;
    case   SB_LINEDOWN:
        scrollinfo.nPos += 1;
        if   (scrollinfo.nPos> scrollinfo.nMax)
        {
            scrollinfo.nPos = scrollinfo.nMax;
            break;
        }
        SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);
        ScrollWindow(0,-10);
        break;
    case   SB_PAGEUP:
        scrollinfo.nPos -= 5;
        if   (scrollinfo.nPos <scrollinfo.nMin)
        {
            scrollinfo.nPos = scrollinfo.nMin;
            break;
        }
        SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);
        ScrollWindow(0,10*5);
        break;
    case   SB_PAGEDOWN:
        scrollinfo.nPos += 5;
        if   (scrollinfo.nPos> scrollinfo.nMax)
        {
            scrollinfo.nPos = scrollinfo.nMax;
            break;
        }
        SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);
        ScrollWindow(0,-10*5);
        break;
    case   SB_ENDSCROLL:
        // MessageBox( "SB_ENDSCROLL ");
        break;
    case   SB_THUMBPOSITION:
        // ScrollWindow(0,(scrollinfo.nPos-nPos)*10);
        // scrollinfo.nPos = nPos;
        // SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);
        break;
    case   SB_THUMBTRACK:
        ScrollWindow(0,(scrollinfo.nPos-nPos)*10);
        scrollinfo.nPos = nPos;
        SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);
        break;
    }

    CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CP2PMonitorDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here

    m_nCurHeight = cy;
    m_nCurWidth = cx;
    //m_Resize.OnSize(cx,cy);
    
    ChangeLabelSize(cx, cy);

    if(created_p2p_dlg)
    {
        m_p2p_dlg->ChangeLabelSize(cx, cy, m_ori_width, m_ori_hight);
        m_p2p_dlg->OnSize(SIZE_RESTORED, 
            m_p2p_dlg->m_p2p_ori_w * cx / m_ori_width,
            m_p2p_dlg->m_p2p_ori_h * cy / m_ori_hight);
    }
    else
    {
        created_p2p_dlg = TRUE;
    }

    if(created_up_dlg)
    {
        m_upload_dlg->ChangeLabelSize(cx, cy, m_ori_width, m_ori_hight);
        m_upload_dlg->OnSize(SIZE_RESTORED, 
            m_upload_dlg->m_up_ori_w * cx / m_ori_width,
            m_upload_dlg->m_up_ori_h * cy / m_ori_hight);
    }
    else
    {
        created_up_dlg = TRUE;
    }

    if(created_live_p2p_dlg)
    {
        m_live_p2p_dlg->ChangeLabelSize(cx, cy, m_ori_width, m_ori_hight);
        m_live_p2p_dlg->OnSize(SIZE_RESTORED, 
            m_live_p2p_dlg->m_live_p2p_ori_w * cx / m_ori_width,
            m_live_p2p_dlg->m_live_p2p_ori_h * cy / m_ori_hight);
    }
    else
    {
        created_live_p2p_dlg = TRUE;
    }

    GetClientRect(&m_rect);
    
}

void CP2PMonitorDlg::CreateChart(CString filename)
{
    CApplication app;
    CWorkbooks books;
    CWorkbook book;
    CWorksheets sheets;
    CWorksheet sheet;
    CRange range;
    CFont0 font;
    CRange cols;
    int i = 3;
    CString str1, str2;
    COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
    CChart chart;
    CChartObjects chartObjects;
    CChartObject chartObject;
    CSeriesCollection seriesCollection;
    CSeries series;
    
    if( !app.CreateDispatch(_T("Excel.Application")))
    {
        MessageBox("无法创建Excel应用！");
        //return;
    }
    books=app.get_Workbooks();

    book.AttachDispatch(books.Open( filename,vtMissing,vtMissing,vtMissing,
        vtMissing,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing,vtMissing,
        vtMissing,vtMissing,vtMissing,vtMissing));

    sheets=book.get_Sheets();
    sheet=sheets.get_Item(COleVariant((short)1));

    LPDISPATCH lpDisp;
    lpDisp = sheet.ChartObjects(covOptional);

    ASSERT(lpDisp);
    chartObjects.AttachDispatch(lpDisp);

    chartObject = chartObjects.Add(10, 40, 900, 250);
    chart.AttachDispatch(chartObject.get_Chart());
    

    
    lpDisp = sheet.get_Range(COleVariant("L1"), COleVariant("P:P"));
    // The range containing the data to be charted.
    ASSERT(lpDisp);
    range.AttachDispatch(lpDisp);

    VARIANT var; // ChartWizard needs a Variant for the Source range.
    var.vt = VT_DISPATCH; // .vt is the usable member of the tagVARIANT
    // Struct. Its value is a union of options.
    var.pdispVal = lpDisp; // Assign IDispatch pointer
    // of the Source range to var.

    chart.ChartWizard(var,                    // Source.
        COleVariant((short)4),  // Gallery: 3d Column.
        covOptional,             // Format, use default.
        COleVariant((short)2),   // PlotBy: xlRows.
        COleVariant((short)0),   // CategoryLabels.
        COleVariant((short)1),   // SeriesLabels.
        COleVariant((short)TRUE),  // HasLegend.
        COleVariant("PeerMonitor"),  // Title.
        COleVariant(""),    // CategoryTitle.
        COleVariant(""),  // ValueTitles.
        covOptional              // ExtraTitle.
        );
    chart.put_ChartType(4);
    
    series = chart.SeriesCollection(COleVariant((short)5));
    
    series.put_AxisGroup(2);

    app.put_Visible(TRUE);
    app.put_UserControl(TRUE);

}
void CP2PMonitorDlg::OnBnClickedButtonChart()
{
    // TODO: Add your control notification handler code here

    if (m_p2p_dlg->m_is_online)
    {
        return;
    }

    if (m_p2p_dlg->import_file_type == 1)
    {
        CreateCsvFromGZ();
        CreateChart(m_csv_file_name);
    }
    else
    {
        CreateChart(m_csv_file_name);
    }
}

void CP2PMonitorDlg::ChangeLabelSize(int cx, int cy)
{
    CWnd *pWnd;
    CRect rect;

    for (int i = 0; i < m_label_info.size(); ++i)
    {
        pWnd = GetDlgItem(m_label_info[i].labelID);
        if(pWnd)
        {
            pWnd->GetWindowRect(&rect);
            ScreenToClient(&rect);
            rect.left = m_label_info[i].left_old * cx / m_ori_width;
            rect.right = (m_label_info[i].right_old - m_label_info[i].left_old) * cx / m_ori_width + rect.left;
            rect.top = m_label_info[i].top_old * cy / m_ori_hight;
            rect.bottom = (m_label_info[i].bottom_old - m_label_info[i].top_old) * cy / m_ori_hight + rect.top;
            pWnd->MoveWindow(rect);
        }
    }

}

void CP2PMonitorDlg::AddLabelInfo()
{
    m_label_id.push_back(IDC_BUTTON_CHART);
    m_label_id.push_back(IDC_BUTTON_IMPORT);
    m_label_id.push_back(IDC_BUTTON_PAUSE);
    m_label_id.push_back(IDC_BUTTON_START);
    m_label_id.push_back(IDC_BUTTON_STOP);
    m_label_id.push_back(IDC_CLEAN);
    m_label_id.push_back(IDC_CONNECT_REMOTE);
    m_label_id.push_back(IDC_EDIT_FILE);
    m_label_id.push_back(IDC_EXPORT);
    m_label_id.push_back(IDC_IS_TOP);
    m_label_id.push_back(IDC_KILL_ALL);
    m_label_id.push_back(IDC_LIST_UPDATE);
    m_label_id.push_back(IDC_PROCESS_SELECT);
    m_label_id.push_back(IDC_RADIO_IMPORT);
    m_label_id.push_back(IDC_RADIO_ONLINE);
    m_label_id.push_back(IDC_RADIO_PC);
    m_label_id.push_back(IDC_RADIO_PPBOX);
    m_label_id.push_back(IDC_REMOTE_IP_LIST);
    m_label_id.push_back(IDC_RUN_PPAP);
    m_label_id.push_back(IDC_RUN_PPLIVE);
    m_label_id.push_back(IDC_SAVING);
    m_label_id.push_back(IDC_STATIC_VERSION);
    m_label_id.push_back(IDC_STATIC3);
    m_label_id.push_back(IDC_TAB);
    m_label_id.push_back(IDC_STATIC9);
    m_label_id.push_back(IDC_STATIC2);
    m_label_id.push_back(IDC_STATIC11);
    m_label_id.push_back(IDC_STATIC4);
    m_label_id.push_back(IDC_STATIC5);
    m_label_id.push_back(IDC_STATIC6);
    m_label_id.push_back(IDC_STATIC7);
    m_label_id.push_back(IDC_BUTTON_NEXT_SECOND);
    
    
    LabelInfo li;
    CWnd *pWnd;
    CRect rect;

    for (int i = 0; i < m_label_id.size(); ++i)
    {
        li.labelID = m_label_id[i];

        pWnd = GetDlgItem(m_label_id[i]);
        pWnd->GetWindowRect(&rect);
        ScreenToClient(&rect);
        li.left_old = rect.left;
        li.right_old = rect.right;
        li.top_old = rect.top;
        li.bottom_old = rect.bottom;

        m_label_info.push_back(li);
    }    
}

bool CP2PMonitorDlg::Deserialize(const char* raw_data, int raw_data_size,
                                 std::vector<boost::shared_ptr<statistic::StatisticsData> >& statistics_datas)
{
    util::archive::ArchiveBuffer<> archive_buffer(const_cast<char*>(raw_data), raw_data_size, raw_data_size);
    util::archive::LittleEndianBinaryIArchive<> input(archive_buffer);

    statistic::PeerStatisticsInfo peer_statistics_info;
    input >> peer_statistics_info;

    int offset = raw_data_size - archive_buffer.size();

    for(size_t i = 0; i < peer_statistics_info.collected_statistics_size_in_seconds_; ++i)
    {
        boost::shared_ptr<statistic::StatisticsData> statistics_data(new statistic::StatisticsData());
        int incremental_bytes = statistics_data->Deserialize(
            peer_statistics_info.version_, 
            const_cast<boost::uint8_t*>(reinterpret_cast<const boost::uint8_t*>(raw_data + offset)), 
            raw_data_size - offset);

        if (incremental_bytes < 0)
        {
            return false;
        }

        statistics_datas.push_back(statistics_data);
        offset += incremental_bytes;
    }

    return offset == raw_data_size;
}

void CP2PMonitorDlg::OnBnClickedButtonNextSecond()
{
    // TODO: Add your control notification handler code here
    
    if (m_p2p_dlg->import_file_type == 1)
    {
        if (m_p2p_dlg->m_second < m_p2p_dlg->m_statistics_datas.size() - 1)
        {
            m_p2p_dlg->ShowStatisticLog();
            m_p2p_dlg->m_second++;
        }
        else
        {
            m_p2p_dlg->MonitorFillZero();
            m_p2p_dlg->m_second = 0;
        }

        if (m_live_p2p_dlg->m_second < m_live_p2p_dlg->m_statistics_datas.size() - 1)
        {
            m_live_p2p_dlg->ShowStatisticLog();
            m_live_p2p_dlg->m_second++;
        }
        else
        {
            m_live_p2p_dlg->MonitorFillZero();
            m_live_p2p_dlg->m_second = 0;
        }
    }
    else
    {
        if (m_p2p_dlg->m_line_number < m_p2p_dlg->m_import_file.size() - 1)
        {
            ++m_p2p_dlg->m_line_number;
        }
        else
        {
            m_p2p_dlg->MonitorFillZero();
            m_p2p_dlg->m_line_number = 0;
        }

        if (m_live_p2p_dlg->m_line_number < m_live_p2p_dlg->m_import_file.size() - 1)
        {
            ++m_live_p2p_dlg->m_line_number;
        }
        else
        {
            m_live_p2p_dlg->MonitorFillZero();
            m_live_p2p_dlg->m_line_number = 0;
        }
    }
}

void CP2PMonitorDlg::CreateCsvFromGZ()
{
    m_csv_file_name = m_filename;
    m_csv_file_name.Delete(m_filename.GetLength() - 2, 2);
    m_csv_file_name.Append("csv");
    m_p2p_dlg->CreateCsvFromGZ(m_csv_file_name);
}
