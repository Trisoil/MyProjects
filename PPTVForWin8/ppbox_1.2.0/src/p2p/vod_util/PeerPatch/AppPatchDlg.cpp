// AppPatchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AppPatch.h"
#include "AppPatchDlg.h"
#include "atlbase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "version.gen"

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAppPatchDlg dialog




CAppPatchDlg::CAppPatchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAppPatchDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAppPatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAppPatchDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_UPDATE, &CAppPatchDlg::OnBnClickedUpdate)
	ON_BN_CLICKED(IDC_RESTORE, &CAppPatchDlg::OnBnClickedRestore)
END_MESSAGE_MAP()


// CAppPatchDlg message handlers


LPVOID getPeerDllResource(int &size)
{
	// 
	HRSRC hRsrc = FindResource(NULL, MAKEINTRESOURCE(IDR_PEERDLL2), TEXT("PEERDLL"));
	if (NULL == hRsrc) {
		::AfxMessageBox( _T("程序打包有问题") );
		return 0;
	}

	DWORD dwSize = SizeofResource(NULL, hRsrc); 
	if (0 == dwSize) {
		::AfxMessageBox( _T("程序打包有问题") );
		return 0;
	}

	HGLOBAL hGlobal = LoadResource(NULL, hRsrc); 
	if (0 == dwSize) {
		::AfxMessageBox( _T("程序打包有问题") );
		return 0;
	}

	LPVOID pBuffer = LockResource(hGlobal); 
	if (0 == dwSize) {
		::AfxMessageBox( _T("程序打包有问题") );
		return 0;
	}

	size = dwSize;
	return pBuffer;
}

BOOL CAppPatchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	this->SetDlgItemText( IDC_UPDATE , _T("更新") );
	this->SetDlgItemText( IDC_RESTORE , _T("还原") );

	CString version;
	version.Format( _T("Peer更新程序,版本号: %s") , _T(PEER_VERSION) );

	this->SetWindowText( version );
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAppPatchDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAppPatchDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAppPatchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CString getPPLiveInstallPath()
{
    HKEY hkey;
    DWORD dwSize = MAX_PATH;
    TCHAR sInstallPath[MAX_PATH];
    LPCTSTR   Rgspath=_T("Path");
	CString path;

	DWORD lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\PPLive.exe"), NULL, KEY_READ, &hkey);
    if (lRet != ERROR_SUCCESS)
    {
		::RegCloseKey( hkey );
        return path;
    }

    lRet = RegQueryValueEx(hkey, Rgspath, NULL, REG_NONE, (LPBYTE)sInstallPath, &dwSize);

    if(lRet != ERROR_SUCCESS)
    {
		::RegCloseKey( hkey );
        return path;
    }

	::RegCloseKey( hkey );

	path = sInstallPath;

	return path;
}

CString getPeerDllInstallPath()
{
    DWORD dwSize = MAX_PATH;
    TCHAR sInstallPath[MAX_PATH];
    LPCTSTR   Rgspath=_T("Path");

	CString path;

	HRESULT hr = ::SHGetFolderPath( NULL , CSIDL_PROGRAM_FILES_COMMON, NULL, SHGFP_TYPE_CURRENT, sInstallPath);

	if( hr == S_OK ) {
		path = sInstallPath;
	}

	return path;
}

bool killProcess()
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

    ZeroMemory( &pi, sizeof(pi) );

	LPWSTR kill_pplive = TEXT("C:\\WINDOWS\\system32\\taskkill.exe /f /im PPLive.exe");
	kill_pplive = new wchar_t[1024];
	lstrcpy( kill_pplive , TEXT("taskkill.exe /f /im PPLive.exe") );
	::CreateProcess( NULL , kill_pplive , NULL , NULL , FALSE , 0 , NULL , NULL , &si , &pi );

	::WaitForSingleObject( pi.hProcess , INFINITE );

	::CloseHandle( pi.hProcess );
	::CloseHandle( pi.hThread );

	lstrcpy( kill_pplive , TEXT("taskkill.exe /f /im PPAP.exe") );
	::CreateProcess( NULL , kill_pplive , NULL , NULL , FALSE , 0 , NULL , NULL , &si , &pi );

	::WaitForSingleObject( pi.hProcess , INFINITE );

	::CloseHandle( pi.hProcess );
	::CloseHandle( pi.hThread );

	delete []kill_pplive;

	// 
	return true;
}

bool restartApp()
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

	CString installPath = getPPLiveInstallPath();
	if( !installPath.GetLength() ) {
		::AfxMessageBox( _T("启动pplive失败!!!") );
		return false;
	}

	// 
	installPath += _T("\\PPLive.exe");

	wchar_t *cmd_pplive = new wchar_t[1024];
	lstrcpy( cmd_pplive , installPath );

	::CreateProcess( NULL , cmd_pplive , NULL , NULL , FALSE , 0 , NULL , NULL , &si , &pi );

	::CloseHandle( pi.hProcess );
	::CloseHandle( pi.hThread );

	delete cmd_pplive;
	return true;
}

void CAppPatchDlg::OnBnClickedUpdate()
{
	if( !killProcess() ) {
		::AfxMessageBox( _T("杀死进程失败，可能是权限问题!!!") );
		return;
	}

	// TODO: Add your control notification handler code here
	CString sInstallPath = getPeerDllInstallPath();
	if( !sInstallPath.GetLength() ) {
		::AfxMessageBox( _T("没有安装PPTV") );
		return;
	}

	// 
	HRSRC hRsrc = FindResource(NULL, MAKEINTRESOURCE(IDR_PEERDLL2), TEXT("PEERDLL"));
	if (NULL == hRsrc) {
		::AfxMessageBox( _T("程序打包有问题") );
		return ;
	}

	DWORD dwSize = SizeofResource(NULL, hRsrc); 
	if (0 == dwSize) {
		::AfxMessageBox( _T("程序打包有问题") );
		return ;
	}

	HGLOBAL hGlobal = LoadResource(NULL, hRsrc); 
	if (0 == dwSize) {
		::AfxMessageBox( _T("程序打包有问题") );
		return ;
	}

	LPVOID pBuffer = LockResource(hGlobal); 
	if (0 == dwSize) {
		::AfxMessageBox( _T("程序打包有问题") );
		return ;
	}

	// 
	CString dll_path = sInstallPath + _T("\\PPLiveNetwork\\kernel\\peer.dll");
	CString dll_bak_path = dll_path + _T(".bak");
	::DeleteFile( dll_bak_path );
	if( !::MoveFile( dll_path , dll_bak_path ) ) {
		// 备份文件失败

		::AfxMessageBox( _T("创建备份文件失败") );
//		return;
	}

	// 
	FILE* fp = NULL;
	
	errno_t error = _tfopen_s(&fp, dll_path, _T("wb"));
	if( error != 0 ) {
		::AfxMessageBox( _T("创建peer.dll文件失败: ") + dll_path );
		return;
	}

	if(dwSize != fwrite( pBuffer, sizeof(char), dwSize, fp)) {
		::AfxMessageBox( _T("写入peer.dll文件失败: ") + dll_path );
		fclose(fp );
		return;
	}

	fclose(fp );

	int ret = ::AfxMessageBox( _T("更新成功: ") + dll_path + _T("\r\n\r\n 现在就启动PPTV吗?"), MB_YESNO);
	
	if (ret == IDYES)
	{
		restartApp();
	}

	return;
}

void CAppPatchDlg::OnBnClickedRestore()
{
	if( !killProcess() ) {
		::AfxMessageBox( _T("杀死进程失败，可能是权限问题!!!") );
		return;
	}

	// TODO: Add your control notification handler code here
	CString sInstallPath = getPeerDllInstallPath();
	if( !sInstallPath.GetLength() ) {
		::AfxMessageBox( _T("没有安装PPTV") );
		return;
	}
	CString dll_path = sInstallPath + _T("\\PPLiveNetwork\\kernel\\peer.dll");
	CString dll_bak_path = dll_path + _T(".bak");
	if( 0xFFFFFFFF  == ::GetFileAttributes( dll_bak_path ) ) {
		::AfxMessageBox( _T("备份文件不存在: ") + dll_bak_path );
		return;
	}

	Sleep(200);
	// 
	::DeleteFile( dll_path );
	if( !::MoveFile( dll_bak_path , dll_path ) ) {
		::AfxMessageBox( _T("恢复文件写入失败: ") + dll_path );
		return;
	}

	::AfxMessageBox( _T("还原成功: ") + dll_path );

	return;
}
