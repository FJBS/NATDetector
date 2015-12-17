// StunServerConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "StunServerConfigDlg.h"
#include "NATSimulatorDoc.h"

// CStunServerConfigDlg dialog

IMPLEMENT_DYNAMIC(CStunServerConfigDlg, CDialog)
CStunServerConfigDlg::CStunServerConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStunServerConfigDlg::IDD, pParent)
{
	mConfiguration = NULL;
}

CStunServerConfigDlg::~CStunServerConfigDlg()
{
}

void CStunServerConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IP_SERVERA, mIPA);
	DDX_Control(pDX, IDC_EDIT_PORT_SERVERA, mPortA);
	DDX_Control(pDX, IDC_IP_SERVERB, mIPB);
	DDX_Control(pDX, IDC_EDIT_PORT_SERVERB, mPortB);
}


BEGIN_MESSAGE_MAP(CStunServerConfigDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CStunServerConfigDlg message handlers

void CStunServerConfigDlg::OnBnClickedOk()
{
	CString aux;

	if ( mConfiguration != NULL )
	{
		mConfiguration->IPsA.RemoveAll();
		mConfiguration->PortsA.RemoveAll();
		mConfiguration->IPsB.RemoveAll();
		mConfiguration->PortsB.RemoveAll();

		mIPA.GetWindowText(aux);
		mConfiguration->IPsA.Add(aux);

		mPortA.GetWindowText(aux);
		mConfiguration->PortsA.Add(atol(aux));

		mIPB.GetWindowText(aux);
		mConfiguration->IPsB.Add(aux);

		mPortB.GetWindowText(aux);
		mConfiguration->PortsB.Add(atol(aux));
	}

	OnOK();
}

BOOL CStunServerConfigDlg::OnInitDialog()
{
	char buffer[10];

	CDialog::OnInitDialog();

	if ( mConfiguration != NULL )
	{
		if ( mConfiguration->IPsA.GetCount() > 0 )
			mIPA.SetWindowText(mConfiguration->IPsA[0]);	

		if ( mConfiguration->PortsA.GetCount() > 0 )
			mPortA.SetWindowText(ltoa(mConfiguration->PortsA[0], buffer, 10));

		if ( mConfiguration->IPsB.GetCount() > 0 )
			mIPB.SetWindowText(mConfiguration->IPsB[0]);	

		if ( mConfiguration->PortsA.GetCount() > 0 )
			mPortB.SetWindowText(ltoa(mConfiguration->PortsB[0], buffer, 10));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
