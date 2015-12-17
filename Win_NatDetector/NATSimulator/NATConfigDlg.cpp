// NATConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "NATConfigDlg.h"
#include "NATSimulatorDoc.h"


// CNATConfigDlg dialog

IMPLEMENT_DYNAMIC(CNATConfigDlg, CDialog)
CNATConfigDlg::CNATConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNATConfigDlg::IDD, pParent)
{
}

CNATConfigDlg::~CNATConfigDlg()
{
}

void CNATConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_PUBLIC, mIPPublic);
	DDX_Control(pDX, IDC_LIST_PUBLIC_IPS, mListCtrlPublicIps);
	DDX_Control(pDX, IDC_IPADDRESS_PRIVATE, mIPAddrPrivate);
	DDX_Control(pDX, IDC_LIST_PRIVATE_IPS, mListCtrlPrivateIps);
	DDX_Control(pDX, IDC_COMBO_MAPPING, mComboMapping);
	DDX_Control(pDX, IDC_COMBO_FILTERING, mComboFiltering);
	DDX_Control(pDX, IDC_EDIT_TIMEOUT, mEditTimeout);
	DDX_Control(pDX, IDC_CHECK_INBOUND_REFRESH, mCheckInboundRefresh);
	DDX_Control(pDX, IDC_CHECK_PORT_PRESERVATION, mCheckPortPreservation);
	DDX_Control(pDX, IDC_CHECK_PORT_OVERLOADING, mCheckPortOverloading);
	DDX_Control(pDX, IDC_COMBO_IP_ASSIGN_MODE, mComboAssignIPsMode);
}


BEGIN_MESSAGE_MAP(CNATConfigDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_ADD_PUBLIC_IP, OnBnClickedButtonAddPublicIp)
	ON_BN_CLICKED(IDC_BUTTON_DEL_PUBLIC_IP, OnBnClickedButtonDelPublicIp)
	ON_BN_CLICKED(IDC_BUTTON_ADD_PRIVATE_IP, OnBnClickedButtonAddPrivateIp)
	ON_BN_CLICKED(IDC_BUTTON_DEL_PRIVATE_IP, OnBnClickedButtonDelPrivateIp)
END_MESSAGE_MAP()


// CNATConfigDlg message handlers

void CNATConfigDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString	aux;

	if ( mListCtrlPublicIps.GetItemCount() == 0 || mListCtrlPrivateIps.GetItemCount() == 0 )
	{
		AfxMessageBox("Enter Public and Private IP Address for the NAT");
		return;
	}

	if ( mNATConfiguration != NULL )
	{
		mNATConfiguration->Mapping = (CNATSim::eMappingBehaviour)mComboMapping.GetCurSel();
		mNATConfiguration->Filtering = (CNATSim::eFilteringBehaviour)mComboFiltering.GetCurSel();

		mEditTimeout.GetWindowText(aux);
		mNATConfiguration->Timeout = atol((const char *)aux);

		mNATConfiguration->InboundRefresh = mCheckInboundRefresh.GetCheck()==BST_CHECKED?true:false;
		mNATConfiguration->PortOverloading = mCheckPortOverloading.GetCheck()==BST_CHECKED?true:false;
		mNATConfiguration->PortPreservation = mCheckPortPreservation.GetCheck()==BST_CHECKED?true:false;
		mNATConfiguration->AssignIPsMode = (CNATSim::eIPAssignMode)mComboAssignIPsMode.GetCurSel();

		mNATConfiguration->IPsA.RemoveAll();
		mNATConfiguration->PortsA.RemoveAll();
		mNATConfiguration->IPsB.RemoveAll();
		mNATConfiguration->PortsB.RemoveAll();

		for ( int i = 0; i < mListCtrlPublicIps.GetItemCount(); i++ )
			mNATConfiguration->IPsA.Add(mListCtrlPublicIps.GetItemText(i,0));

		for ( int i = 0; i < mListCtrlPrivateIps.GetItemCount(); i++ )
			mNATConfiguration->IPsB.Add(mListCtrlPrivateIps.GetItemText(i,0));
	}

	OnOK();
}

BOOL CNATConfigDlg::OnInitDialog()
{
	char buffer[10];
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	mListCtrlPublicIps.InsertColumn(0, "IP Address", LVCFMT_CENTER, 150, -1);
	mListCtrlPublicIps.SetExtendedStyle(mListCtrlPublicIps.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	mListCtrlPrivateIps.InsertColumn(0, "IP Address", LVCFMT_CENTER, 150, -1);
	mListCtrlPrivateIps.SetExtendedStyle(mListCtrlPrivateIps.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	mComboMapping.InsertString(0, "Not Nated");
	mComboMapping.InsertString(1, "End Point Independent");
	mComboMapping.InsertString(2, "Address Dependent");
	mComboMapping.InsertString(3, "Address and Port Dependent");

	mComboFiltering.InsertString(0, "Address Independent");
	mComboFiltering.InsertString(1, "Address Dependent");
	mComboFiltering.InsertString(2, "Address and Port Dependent");

	mEditTimeout.SetWindowText("300");

	mComboAssignIPsMode.InsertString(0, "Paired");
	mComboAssignIPsMode.InsertString(1, "Random");

	if ( mNATConfiguration != NULL )
	{
		mComboMapping.SetCurSel(mNATConfiguration->Mapping);
		mComboFiltering.SetCurSel(mNATConfiguration->Filtering);
		mEditTimeout.SetWindowText(ltoa(mNATConfiguration->Timeout, buffer, 10));
		mCheckInboundRefresh.SetCheck(mNATConfiguration->InboundRefresh?BST_CHECKED:BST_UNCHECKED);
		mCheckPortOverloading.SetCheck(mNATConfiguration->PortOverloading?BST_CHECKED:BST_UNCHECKED);
		mCheckPortPreservation.SetCheck(mNATConfiguration->PortPreservation?BST_CHECKED:BST_UNCHECKED);
		mComboAssignIPsMode.SetCurSel(mNATConfiguration->AssignIPsMode);

		for ( int i = 0; i < mNATConfiguration->IPsA.GetCount(); i++ )
			mListCtrlPublicIps.InsertItem(i, mNATConfiguration->IPsA[i]);	

		for ( int i = 0; i < mNATConfiguration->IPsB.GetCount(); i++ )
			mListCtrlPrivateIps.InsertItem(i, mNATConfiguration->IPsB[i]);	

	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CNATConfigDlg::OnBnClickedButtonAddPublicIp()
{
	CString IP;
	mIPPublic.GetWindowText(IP);

	if ( IP != "0.0.0.0" )
	{
		mListCtrlPublicIps.InsertItem(0, IP);
	}
}

void CNATConfigDlg::OnBnClickedButtonDelPublicIp()
{
	int nItem;
	POSITION position = mListCtrlPublicIps.GetFirstSelectedItemPosition();

	if ( position )
	{
		nItem = mListCtrlPublicIps.GetNextSelectedItem(position);

		mListCtrlPublicIps.DeleteItem(nItem);
	}
}

void CNATConfigDlg::OnBnClickedButtonAddPrivateIp()
{
	CString IP;
	mIPAddrPrivate.GetWindowText(IP);

	if ( IP != "0.0.0.0" )
	{
		mListCtrlPrivateIps.InsertItem(0, IP);
	}
}

void CNATConfigDlg::OnBnClickedButtonDelPrivateIp()
{
	int nItem;
	POSITION position = mListCtrlPrivateIps.GetFirstSelectedItemPosition();

	if ( position )
	{
		nItem = mListCtrlPrivateIps.GetNextSelectedItem(position);

		mListCtrlPrivateIps.DeleteItem(nItem);
	}
}
