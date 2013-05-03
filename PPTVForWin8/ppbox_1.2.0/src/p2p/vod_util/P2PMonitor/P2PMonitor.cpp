// P2PMonitor.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "P2PMonitor.h"
#include "P2PMonitorDlg.h"
#include "SimpleDlg.h"
#include "Detection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CP2PMonitorApp

BEGIN_MESSAGE_MAP(CP2PMonitorApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CP2PMonitorApp ����

CP2PMonitorApp::CP2PMonitorApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CP2PMonitorApp ����

CP2PMonitorApp theApp;


// CP2PMonitorApp ��ʼ��

BOOL CP2PMonitorApp::InitInstance()
{
    HANDLE hMutex=::CreateMutex(NULL,TRUE,"PeerMonitor");
    if (hMutex != NULL)
    {
    /*    if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            AfxMessageBox("�����Ѿ�����!");
            return FALSE;
        }*/
    }
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

#if _DETECTION_VERSION_
    Detection dlg;
#else
#if _SIMPLE_VERSION_
    CSimpleDlg dlg;
#else
    CP2PMonitorDlg dlg;
#endif
#endif
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();


	if (nResponse == IDOK)
	{
		// TODO: �ڴ˴����ô����ʱ�á�ȷ�������ر�
		//  �Ի���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ�á�ȡ�������ر�
		//  �Ի���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
