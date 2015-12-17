#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CStunClientConfigDlg dialog

class CElement;

class CStunClientConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CStunClientConfigDlg)

public:
	CStunClientConfigDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStunClientConfigDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_STUN_CLIENT_CONFIGURATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

private:
	CIPAddressCtrl	mIP;
	CEdit			mPort;

public:
	CElement		*mConfiguration;
};
