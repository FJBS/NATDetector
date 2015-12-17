#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CTestDlg dialog

class CTestDlg : public CDialog
{
	DECLARE_DYNAMIC(CTestDlg)

public:
	CTestDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTestDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_REQUIREMET_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

public:
	CStringArray	mSTUNClients;
	CArray<int>		mSTUNClientsIdx;
	CStringArray	mSTUNServer;
	CArray<int>		mSTUNServerIdx;
	CString			mTest;

	CArray<int>		mSelectedClients;
	CArray<int>		mSelectedServers;

	int				mTimeout;
	int				mNumMessages;

private:
	CListCtrl		mListCtrlClients;
	CListCtrl		mListCtrlServers;
	CStatic			mStaticTest;
	CStatic			mStaticNum1;
	CEdit			mEditNum1;
};
