// P2PMonitor.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CP2PMonitorApp:
// 有关此类的实现，请参阅 P2PMonitor.cpp
//

class CP2PMonitorApp : public CWinApp
{
public:
	CP2PMonitorApp();

// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CP2PMonitorApp theApp;