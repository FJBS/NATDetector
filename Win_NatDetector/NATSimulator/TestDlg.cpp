// TestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TestDlg.h"

// CTestDlg dialog

IMPLEMENT_DYNAMIC(CTestDlg, CDialog)
CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestDlg::IDD, pParent)
{
}

CTestDlg::~CTestDlg()
{
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CLIENTS, mListCtrlClients);
	DDX_Control(pDX, IDC_LIST_SERVERS, mListCtrlServers);
	DDX_Control(pDX, IDC_STATIC_TITLE, mStaticTest);
	DDX_Control(pDX, IDC_STATIC_NUM1, mStaticNum1);
	DDX_Control(pDX, IDC_EDIT_NUM1, mEditNum1);
}


BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CTestDlg message handlers

BOOL CTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	mListCtrlClients.InsertColumn(0, "Client IP", LVCFMT_CENTER, 200, -1);
	mListCtrlClients.SetExtendedStyle(mListCtrlClients.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);

	mListCtrlServers.InsertColumn(0, "Server A IP", LVCFMT_CENTER, 100, -1);
	mListCtrlServers.InsertColumn(1, "Server B IP", LVCFMT_CENTER, 100, -1);
	mListCtrlServers.SetExtendedStyle(mListCtrlServers.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);
	
	for ( int i=0; i < mSTUNClients.GetCount(); i++ )
	{
		mListCtrlClients.InsertItem(i, mSTUNClients[i]);
	}

	for ( int i=0; i < mSTUNServer.GetCount(); i++ )
	{
		mListCtrlServers.InsertItem(i, mSTUNServer[i].Left(mSTUNServer[i].GetLength()-mSTUNServer[i].Find('\n')-1));
		mListCtrlServers.SetItemText(i, 1, mSTUNServer[i].Right(mSTUNServer[i].GetLength()-mSTUNServer[i].Find('\n')-1));
	}

	mStaticTest.SetWindowText(mTest);

	CString aux;
	aux.LoadString(IDS_BINDING_LIFETIME);
	if ( aux == mTest )
	{
		mStaticNum1.SetWindowText("Time (seconds):");
		mStaticNum1.ShowWindow(SW_SHOW);
		mEditNum1.ShowWindow(SW_SHOW);
		mEditNum1.SetWindowText("10");
	}
	
	aux.LoadString(IDS_ADDRESS_POOLING_TEST);
	if ( aux == mTest )
	{
		mStaticNum1.SetWindowText("Messages:");
		mStaticNum1.ShowWindow(SW_SHOW);
		mEditNum1.ShowWindow(SW_SHOW);
		mEditNum1.SetWindowText("5");
	}

	aux.LoadString(IDS_PORT_OVERLOADING_TEST);
	if ( aux == mTest )
	{
		mStaticNum1.SetWindowText("Messages:");
		mStaticNum1.ShowWindow(SW_SHOW);
		mEditNum1.ShowWindow(SW_SHOW);
		mEditNum1.SetWindowText("5");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTestDlg::OnBnClickedOk()
{
	mSelectedClients.RemoveAll();
	mSelectedServers.RemoveAll();

	CString sAux = "";

	for ( int i = 0; i < mListCtrlClients.GetItemCount(); i++ )
	{
		if ( mListCtrlClients.GetCheck(i) )
			mSelectedClients.Add(mSTUNClientsIdx[i]);
	}
	for ( int i = 0; i < mListCtrlServers.GetItemCount(); i++ )
	{
		if ( mListCtrlServers.GetCheck(i) )
			mSelectedServers.Add(mSTUNServerIdx[i]);
	}

	CString mappingTest;
	mappingTest.LoadString(IDS_MAPPING_TEST);
	CString filteringTest;
	filteringTest.LoadString(IDS_FILTERING_TEST);
	CString bindingTest;
	bindingTest.LoadString(IDS_BINDING_LIFETIME);
	CString ipAddressPoolingTest;
	ipAddressPoolingTest.LoadString(IDS_ADDRESS_POOLING_TEST);
	CString portOverloadingTest;
	portOverloadingTest.LoadString(IDS_PORT_OVERLOADING_TEST);

	if ( mappingTest == mTest )
	{
		if ( mSelectedServers.GetElements() != 1 || mSelectedClients.GetElements() != 1 )
		{
			sAux.LoadString(IDS_TEST_DLG_VALIDATION_1);
			AfxMessageBox(sAux);
		}
		else
		{
			OnOK();
		}
	}
	else if ( filteringTest == mTest )
	{
		if ( mSelectedServers.GetElements() != 1 || mSelectedClients.GetElements() != 1 )
		{
			sAux.LoadString(IDS_TEST_DLG_VALIDATION_1);
			AfxMessageBox(sAux);
		}
		else
		{
			OnOK();
		}
	}
	else if ( bindingTest == mTest )
	{
		mEditNum1.GetWindowText(sAux);

		if ( mSelectedServers.GetElements() != 1 || mSelectedClients.GetElements() != 1 )
		{
			sAux.LoadString(IDS_TEST_DLG_VALIDATION_1);
			AfxMessageBox(sAux);
		}
		else if ( sAux.GetLength() == 0 )
		{
			sAux.LoadString(IDS_TEST_DLG_VALIDATION_2);
			AfxMessageBox(sAux);
		}
		else
		{
			mTimeout = atoi((const char *)sAux);

			OnOK();
		}
	}	
	else if ( ipAddressPoolingTest == mTest )
	{
		mEditNum1.GetWindowText(sAux);

		if ( mSelectedServers.GetElements() != 1 || mSelectedClients.GetElements() != 1 )
		{
			sAux.LoadString(IDS_TEST_DLG_VALIDATION_1);
			AfxMessageBox(sAux);
		}
		else if ( sAux.GetLength() == 0 )
		{
			sAux.LoadString(IDS_TEST_DLG_VALIDATION_2);
			AfxMessageBox(sAux);
		}
		else
		{
			mNumMessages = atoi((const char *)sAux);

			OnOK();
		}
	}
	else if ( portOverloadingTest == mTest )
	{
		mEditNum1.GetWindowText(sAux);

		if ( mSelectedServers.GetElements() != 1 || mSelectedClients.GetElements() != 2 )
		{
			sAux.LoadString(IDS_TEST_DLG_VALIDATION_3);
			AfxMessageBox(sAux);
		}
		else if ( sAux.GetLength() == 0 )
		{
			sAux.LoadString(IDS_TEST_DLG_VALIDATION_2);
			AfxMessageBox(sAux);
		}
		else
		{
			mNumMessages = atoi((const char *)sAux);

			OnOK();
		}
	}

}
