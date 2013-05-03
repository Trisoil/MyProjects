// livemedia_gui_test.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Clivemedia_gui_testApp:
// See livemedia_gui_test.cpp for the implementation of this class
//

class Clivemedia_gui_testApp : public CWinApp
{
public:
	Clivemedia_gui_testApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Clivemedia_gui_testApp theApp;