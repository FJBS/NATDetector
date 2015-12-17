#pragma once
#include "afxwin.h"


// CNatConnectDlg dialog

class CNatConnectDlg : public CDialog
{
	DECLARE_DYNAMIC(CNatConnectDlg)

public:
	CNatConnectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNatConnectDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_NAT_CONNECT };

public:

	CString		mNATAIPs;
	CString		mNATBIPs;
	eConnector	mConnectorNAT1;
	eConnector	mConnectorNAT2;

protected:

	void SetConnectorNAT1(eConnector connector);
	void SetConnectorNAT2(eConnector connector);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnStnClickedStaticPublicNat1();
	afx_msg void OnStnClickedStaticPublicNat2();
	afx_msg void OnStnClickedStaticPrivateNat2();
	afx_msg void OnStnClickedStaticPrivateNat1();

	DECLARE_MESSAGE_MAP()

private:
	CStatic mStaticNAT1;
	CStatic mStaticNAT2;

	CStatic mStaticPublicNAT1;
	CStatic mStaticPublicNAT2;
	CStatic mStaticPrivateNAT1;
	CStatic mStaticPrivateNAT2;

	CBitmap mBitmapRedConnector;
	CBitmap mBitmapBlackConnector;
};
