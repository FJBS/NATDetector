// NATSimulator.h : main header file for the NATSimulator application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CNATSimulatorApp:
// See NATSimulator.cpp for the implementation of this class
//

class CNATSimulatorApp : public CWinApp
{
public:
	CNATSimulatorApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CNATSimulatorApp theApp;