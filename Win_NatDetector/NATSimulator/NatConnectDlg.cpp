// NatConnectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "NatConnectDlg.h"

// CNatConnectDlg dialog

IMPLEMENT_DYNAMIC(CNatConnectDlg, CDialog)
CNatConnectDlg::CNatConnectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNatConnectDlg::IDD, pParent)
{
	mBitmapRedConnector.LoadBitmap(MAKEINTRESOURCE(IDB_RED_CONNECTOR));
	mBitmapBlackConnector.LoadBitmap(MAKEINTRESOURCE(IDB_BLACK_CONNECTOR));
	mConnectorNAT1 = ePublic;
	mConnectorNAT2 = ePrivate;
}

CNatConnectDlg::~CNatConnectDlg()
{
}

void CNatConnectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_NAT1, mStaticNAT1);
	DDX_Control(pDX, IDC_STATIC_NAT2, mStaticNAT2);
	DDX_Control(pDX, IDC_STATIC_PUBLIC_NAT_1, mStaticPublicNAT1);
	DDX_Control(pDX, IDC_STATIC_PUBLIC_NAT_2, mStaticPublicNAT2);
	DDX_Control(pDX, IDC_STATIC_PRIVATE_NAT_1, mStaticPrivateNAT1);
	DDX_Control(pDX, IDC_STATIC_PRIVATE_NAT_2, mStaticPrivateNAT2);
}


BEGIN_MESSAGE_MAP(CNatConnectDlg, CDialog)
	ON_STN_CLICKED(IDC_STATIC_PUBLIC_NAT_1, OnStnClickedStaticPublicNat1)
	ON_STN_CLICKED(IDC_STATIC_PUBLIC_NAT_2, OnStnClickedStaticPublicNat2)
	ON_STN_CLICKED(IDC_STATIC_PRIVATE_NAT_2, OnStnClickedStaticPrivateNat2)
	ON_STN_CLICKED(IDC_STATIC_PRIVATE_NAT_1, OnStnClickedStaticPrivateNat1)
END_MESSAGE_MAP()


// CNatConnectDlg message handlers

BOOL CNatConnectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	mStaticNAT1.SetWindowText(mNATAIPs);
	mStaticNAT2.SetWindowText(mNATBIPs);

	SetConnectorNAT1(ePublic);
	mConnectorNAT1 = ePublic;

	SetConnectorNAT2(ePrivate);
	mConnectorNAT2 = ePrivate;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CNatConnectDlg::OnStnClickedStaticPublicNat1()
{
	SetConnectorNAT1(ePublic);
	mConnectorNAT1 = ePublic;
}

void CNatConnectDlg::OnStnClickedStaticPublicNat2()
{
	SetConnectorNAT2(ePublic);
	mConnectorNAT2 = ePublic;
}

void CNatConnectDlg::OnStnClickedStaticPrivateNat2()
{
	SetConnectorNAT2(ePrivate);
	mConnectorNAT2 = ePrivate;
}

void CNatConnectDlg::OnStnClickedStaticPrivateNat1()
{
	SetConnectorNAT1(ePrivate);
	mConnectorNAT1 = ePrivate;
}

void CNatConnectDlg::SetConnectorNAT1(eConnector connector)
{
	if ( connector == ePublic )
	{
		mStaticPublicNAT1.SetBitmap(mBitmapRedConnector);
		mStaticPrivateNAT1.SetBitmap(mBitmapBlackConnector);
		mStaticPublicNAT1.Invalidate();
		mStaticPrivateNAT1.Invalidate();
	}
	else
	{
		mStaticPrivateNAT1.SetBitmap(mBitmapRedConnector);
		mStaticPublicNAT1.SetBitmap(mBitmapBlackConnector);
		mStaticPrivateNAT1.Invalidate();
		mStaticPublicNAT1.Invalidate();
	}
}

void CNatConnectDlg::SetConnectorNAT2(eConnector connector)
{
	if ( connector == ePublic )
	{
		mStaticPublicNAT2.SetBitmap(mBitmapRedConnector);
		mStaticPrivateNAT2.SetBitmap(mBitmapBlackConnector);
		mStaticPublicNAT2.Invalidate();
		mStaticPrivateNAT2.Invalidate();
	}
	else
	{
		mStaticPrivateNAT2.SetBitmap(mBitmapRedConnector);
		mStaticPublicNAT2.SetBitmap(mBitmapBlackConnector);
		mStaticPrivateNAT2.Invalidate();
		mStaticPublicNAT2.Invalidate();
	}
}
