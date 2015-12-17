// StunClientConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "StunClientConfigDlg.h"
#include "NATSimulatorDoc.h"

// CStunClientConfigDlg dialog

IMPLEMENT_DYNAMIC(CStunClientConfigDlg, CDialog)
CStunClientConfigDlg::CStunClientConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStunClientConfigDlg::IDD, pParent)
{
	mConfiguration = NULL;
}

CStunClientConfigDlg::~CStunClientConfigDlg()
{
}

void CStunClientConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IP_SERVERA, mIP);
	DDX_Control(pDX, IDC_EDIT_PORT_SERVERA, mPort);
}


BEGIN_MESSAGE_MAP(CStunClientConfigDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CStunClientConfigDlg message handlers

BOOL CStunClientConfigDlg::OnInitDialog()
{
	char buffer[10];

	CDialog::OnInitDialog();

	if ( mConfiguration != NULL )
	{
		if ( mConfiguration->IPsA.GetCount() > 0 )
			mIP.SetWindowText(mConfiguration->IPsA[0]);	

		if ( mConfiguration->PortsA.GetCount() > 0 )
			mPort.SetWindowText(ltoa(mConfiguration->PortsA[0], buffer, 10));
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CStunClientConfigDlg::OnBnClickedOk()
{
	CString aux;

	if ( mConfiguration != NULL )
	{
		mConfiguration->IPsA.RemoveAll();
		mConfiguration->PortsA.RemoveAll();

		mIP.GetWindowText(aux);
		mConfiguration->IPsA.Add(aux);

		mPort.GetWindowText(aux);
		mConfiguration->PortsA.Add(atol(aux));
	}

	OnOK();
}
