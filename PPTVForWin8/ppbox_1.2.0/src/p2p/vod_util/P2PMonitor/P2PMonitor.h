// P2PMonitor.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CP2PMonitorApp:
// �йش����ʵ�֣������ P2PMonitor.cpp
//

class CP2PMonitorApp : public CWinApp
{
public:
	CP2PMonitorApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CP2PMonitorApp theApp;