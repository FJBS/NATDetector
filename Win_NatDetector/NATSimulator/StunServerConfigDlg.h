#pragma once

#include "afxwin.h"
#include "afxcmn.h"

// CStunServerConfigDlg dialog

class CElement;

class CStunServerConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CStunServerConfigDlg)

public:
	CStunServerConfigDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStunServerConfigDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_STUN_SERVER_CONFIGURATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedOk();

private:
	CIPAddressCtrl	mIPA;
	CEdit			mPortA;
	CIPAddressCtrl	mIPB;
	CEdit			mPortB;

public:
	CElement		*mConfiguration;
};
