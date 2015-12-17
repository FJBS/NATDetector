#pragma once
#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"

// CNATConfigDlg dialog

class CElement;

class CNATConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CNATConfigDlg)

public:
	CNATConfigDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNATConfigDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_NAT_CONFIGURATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonAddPublicIp();
	afx_msg void OnBnClickedButtonDelPublicIp();
	afx_msg void OnBnClickedButtonAddPrivateIp();
	afx_msg void OnBnClickedButtonDelPrivateIp();

	DECLARE_MESSAGE_MAP()

private:
	CIPAddressCtrl	mIPPublic;
	CListCtrl		mListCtrlPublicIps;
	CIPAddressCtrl	mIPAddrPrivate;
	CListCtrl		mListCtrlPrivateIps;
	CComboBox		mComboMapping;
	CComboBox		mComboFiltering;
	CEdit			mEditTimeout;
	CButton			mCheckInboundRefresh;
	CButton			mCheckPortPreservation;
	CButton			mCheckPortOverloading;
	CComboBox		mComboAssignIPsMode;

public:
	CElement		*mNATConfiguration;
};
