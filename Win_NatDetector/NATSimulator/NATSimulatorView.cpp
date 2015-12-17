	// NATSimulatorView.cpp : implementation of the CNATSimulatorView class
//

#include "stdafx.h"
#include "NATSimulator.h"

#include "NATSimulatorDoc.h"
#include "NATSimulatorView.h"
#include "MainFrm.h"
#include "NATConfigDlg.h"
#include "StunClientConfigDlg.h"
#include "StunServerConfigDlg.h"
#include "Arrow.h"
#include ".\natsimulatorview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNATSimulatorView

IMPLEMENT_DYNCREATE(CNATSimulatorView, CFormView)

BEGIN_MESSAGE_MAP(CNATSimulatorView, CFormView)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_CANVAS, OnNMCustomdrawListCanvas)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_ELEMENTS, OnLvnBegindragListElements)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_CANVAS, OnLvnBegindragListCanvas)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_STN_CLICKED(IDC_STATIC_LINK, OnStnClickedStaticLink)
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICES, OnCbnSelchangeComboDevices)
	ON_CBN_SELCHANGE(IDC_COMBO_TESTS, OnCbnSelchangeComboTests)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CANVAS, OnNMDblclkListCanvas)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_CANVAS, OnNMRclickListCanvas)
	ON_COMMAND(ID_MARKALL, OnMarkall)
	ON_COMMAND(ID_UNMARKALL, OnUnmarkall)
	ON_COMMAND(ID_DELETEITEM, OnDeleteitem)
	ON_BN_CLICKED(IDC_BUTTON_RUN_TEST, OnBnClickedButtonRunTest)
	ON_COMMAND(ID_CLEANTRAFFICMARKS, OnCleantrafficmarks)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_MESSAGES, OnNMRclickListMessages)
	ON_COMMAND(ID_CLEAN, OnClean)
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CNATSimulatorView construction/destruction

CNATSimulatorView::CNATSimulatorView()
	: CFormView(CNATSimulatorView::IDD)
{
	mImageList = new CImageList();
	mImageList->Create(48, 48, TRUE, 3, 1);
	mImageList->Add(AfxGetApp()->LoadIcon(IDI_ICON_ELEMENT_NAT));
	mImageList->Add(AfxGetApp()->LoadIcon(IDI_ICON_ELEMENT_STUN_CLIENT));
	mImageList->Add(AfxGetApp()->LoadIcon(IDI_ICON_ELEMENT_STUN_SERVER));
	mImageList->Add(AfxGetApp()->LoadIcon(IDI_ICON_ELEMENT_STUN_CLIENT_REAL));
	mImageList->Add(AfxGetApp()->LoadIcon(IDI_ICON_ELEMENT_STUN_SERVER_REAL));
	mImageList->Add(AfxGetApp()->LoadIcon(IDI_ICON_ELEMENT_NAT_VISITED));
	mImageList->Add(AfxGetApp()->LoadIcon(IDI_ICON_ELEMENT_STUN_SERVER_VISITED));
	mImageList->Add(AfxGetApp()->LoadIcon(IDI_ICON_ELEMENT_STUN_CLIENT_VISITED));

	mBitmapLink.LoadBitmap(MAKEINTRESOURCE(IDB_LINK));
	mBitmapLinkChecked.LoadBitmap(MAKEINTRESOURCE(IDB_LINK_CHECKED));

	mGridPen.CreatePen(PS_ENDCAP_FLAT|PS_COSMETIC|PS_ALTERNATE, 2, RGB(80,80,200));
	mPen.CreatePen(PS_ENDCAP_FLAT|PS_COSMETIC|PS_ALTERNATE, 2, RGB(100,100,100));

	CCaptureApi::GetDevices(&mNetDevices, &mNumDevices);

	mTerminateAnimations = false;
	mThreadAnimations = (HANDLE*) _beginthread (sThreadAnimations, 0, this);

}

CNATSimulatorView::~CNATSimulatorView()
{
	mTerminateAnimations = true;
	WaitForSingleObject(mThreadAnimations, INFINITE);

	delete mImageList;
}

void CNATSimulatorView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ELEMENTS, mListCtrlElements);
	DDX_Control(pDX, IDC_LIST_CANVAS, mListCtrlCanvas);
	DDX_Control(pDX, IDC_LIST_MESSAGES, mListCtrlMessages);
	DDX_Control(pDX, IDC_STATIC_LINK, mStaticLink);
	DDX_Control(pDX, IDC_COMBO_DEVICES, mComboDevices);
	DDX_Control(pDX, IDC_COMBO_TESTS, mComboTests);
	DDX_Control(pDX, IDC_STATIC_RUNNING_TEST, mStaticAnimation);
}

BOOL CNATSimulatorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CNATSimulatorView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	ResizeParentToFit();

	mListCtrlCanvas.SetExtendedStyle(mListCtrlCanvas.GetExtendedStyle()|LVS_EX_CHECKBOXES);

	// mListCtrlCanvas.SetListCtrlMessages(&mListCtrlMessages);

	mListCtrlCanvas.SetImageList(mImageList, LVSIL_NORMAL);
	mListCtrlElements.SetImageList(mImageList, LVSIL_NORMAL);

	POINT pt;
	CRect r;
	mListCtrlElements.InsertItem(0, "NAT", 0);
    mListCtrlElements.GetItemPosition(0, &pt);
	mListCtrlElements.GetItemRect(0, &r, LVIR_BOUNDS);
	mListCtrlElements.InsertItem(1, "STUN CLIENT", 1);
	pt.y += (r.bottom - r.top) + 30;
	mListCtrlElements.SetItemPosition(1, pt);
	mListCtrlElements.InsertItem(2, "STUN SERVER", 2);
	pt.y += (r.bottom - r.top) + 30;
	mListCtrlElements.SetItemPosition(2, pt);
	mListCtrlElements.InsertItem(3, "REAL STUN CLIENT", 3);
	pt.y += (r.bottom - r.top) + 30;
	mListCtrlElements.SetItemPosition(3, pt);
	mListCtrlElements.InsertItem(4, "REAL STUN SERVER", 4);
	pt.y += (r.bottom - r.top) + 30;
	mListCtrlElements.SetItemPosition(4, pt);

	mListCtrlElements.SetBkColor(RGB(234,234,234));
	mListCtrlElements.SetTextBkColor(RGB(234,234,234));

	mListCtrlMessages.InsertColumn(0, "", LVCFMT_LEFT, 40, -1);
	mListCtrlMessages.InsertColumn(1, "From(IP)", LVCFMT_RIGHT, 90, -1);
	mListCtrlMessages.InsertColumn(2, "From(Port)", LVCFMT_RIGHT, 50, -1);
	mListCtrlMessages.InsertColumn(3, "To(IP)", LVCFMT_RIGHT, 90, -1);
	mListCtrlMessages.InsertColumn(4, "To(Port)", LVCFMT_RIGHT, 50, -1);
	mListCtrlMessages.InsertColumn(5, "Trans. ID", LVCFMT_LEFT, 150, -1);
	mListCtrlMessages.InsertColumn(6, "Mapped Address", LVCFMT_RIGHT, 90, -1);
	mListCtrlMessages.InsertColumn(7, "Mapped Port", LVCFMT_RIGHT, 50, -1);
	mListCtrlMessages.InsertColumn(8, "Changed Address", LVCFMT_RIGHT, 90, -1);
	mListCtrlMessages.InsertColumn(9, "Changed Port", LVCFMT_RIGHT, 50, -1);
	mListCtrlMessages.InsertColumn(10, "XOR Mapped Address", LVCFMT_RIGHT, 90, -1);
	mListCtrlMessages.InsertColumn(11, "XOR Mapped Port", LVCFMT_RIGHT, 50, -1);
	mListCtrlMessages.SetExtendedStyle(mListCtrlMessages.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	CRect rect(0,0,0,0);
	mSplitterBar.Create(WS_CHILD|WS_BORDER|WS_DLGFRAME|WS_VISIBLE,rect,this,10000, TRUE);
	mSplitterBar.SetPanes(&mListCtrlMessages,&mListCtrlCanvas);

	CComboBox *comboTests = (CComboBox*) GetDlgItem(IDC_COMBO_TESTS);

	CString aux;

	aux.LoadString(IDS_MAPPING_TEST);
	comboTests->AddString(aux);

	aux.LoadString(IDS_FILTERING_TEST);
	comboTests->AddString(aux);

	aux.LoadString(IDS_BINDING_LIFETIME);
	comboTests->AddString(aux);

	aux.LoadString(IDS_ADDRESS_POOLING_TEST);
	comboTests->AddString(aux);

	aux.LoadString(IDS_PORT_OVERLOADING_TEST);
	comboTests->AddString(aux);

	CComboBox *comboDevices = (CComboBox*) GetDlgItem(IDC_COMBO_DEVICES);
	for ( int i = 0; i < mNumDevices; i++ )
		comboDevices->AddString(mNetDevices[i]);

	mDragging = false;
	mDragginLink = false;
	mLinking = false;
	mDragIndex = -1;
	mDragImage = NULL;

	CPoint pos;
	for ( int i=0; i < GetDocument()->GetElementsCount(); i++ )
	{
		CElement *element;
		element = GetDocument()->GetItemData(i);
		mListCtrlCanvas.InsertItem(LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM, i, element->Name, 0, LVIS_SELECTED, element->Type, NULL);
		mListCtrlCanvas.SetCheck(i, TRUE);
		pos = element->Position;
		mListCtrlCanvas.SetItemPosition(i, pos);
	}
}


// CNATSimulatorView diagnostics

#ifdef _DEBUG
void CNATSimulatorView::AssertValid() const
{
	CFormView::AssertValid();
}

void CNATSimulatorView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CNATSimulatorDoc* CNATSimulatorView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNATSimulatorDoc)));
	return (CNATSimulatorDoc*)m_pDocument;
}
#endif //_DEBUG


// CNATSimulatorView message handlers

void CNATSimulatorView::OnSize(UINT nType, int cx, int cy)
{
	// CFormView::OnSize(nType, cx, cy);

	RECT WndRect, CanvasWndRect, ElementsWndRect, MessagesWndRect, SppliterWndRect;

	if ( mListCtrlCanvas.m_hWnd )
	{
		GetWindowRect(&WndRect);
		ScreenToClient(&WndRect);

		mListCtrlElements.GetWindowRect(&ElementsWndRect);
		mListCtrlCanvas.GetWindowRect(&CanvasWndRect);
		mListCtrlMessages.GetWindowRect(&MessagesWndRect);

		ScreenToClient(&ElementsWndRect);
		ElementsWndRect.bottom = WndRect.bottom - 6;
		mListCtrlElements.MoveWindow(&ElementsWndRect, TRUE);

		ScreenToClient(&MessagesWndRect);
		MessagesWndRect.right = WndRect.right - 6;
		mListCtrlMessages.MoveWindow(&MessagesWndRect, TRUE);

		ScreenToClient(&CanvasWndRect);
		CanvasWndRect.right = WndRect.right - 6;
		CanvasWndRect.bottom = WndRect.bottom - 6;
		mListCtrlCanvas.MoveWindow(&CanvasWndRect, TRUE);
	}

	if ( mSplitterBar.m_hWnd )
	{
		mSplitterBar.GetWindowRect(&SppliterWndRect);
		ScreenToClient(&SppliterWndRect);
		SppliterWndRect.right = WndRect.right - 6;
		mSplitterBar.MoveWindow(&SppliterWndRect, TRUE);

		mSplitterBar.SetPanes(&mListCtrlMessages,&mListCtrlCanvas);
	}
}

//
//
void CNATSimulatorView::OnLvnBegindragListElements(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	POINT	pt;
	pt.x = pt.y = 0;;

	mDragging = TRUE;
	mDragList = &mListCtrlElements;
	mDropWnd = &mListCtrlElements;

	mDragIndex = pNMLV->iItem;
	mDragImage = mListCtrlElements.CreateDragImage(mDragIndex, &pt);

	mDragImage->BeginDrag(0, CPoint(0,0));
	mDragImage->DragEnter(GetDesktopWindow(), pNMLV->ptAction);

	GetDocument()->GetNextElementToInsert((eElementType)mDragIndex, mElementDragging);
	
	SetCapture ();

	*pResult = 0;
}

void CNATSimulatorView::OnLvnBegindragListCanvas(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	POINT pt;
	CRect r;

	mDragIndex = pNMLV->iItem;

	pt.x = 0;
	pt.y = 0;

	mDragImage = mListCtrlCanvas.CreateDragImage(mDragIndex, &pt);

	if ( mLinking )
	{
		mElementDragging.Index = pNMLV->iItem;
		mListCtrlCanvas.GetItemRect(pNMLV->iItem, &r, LVIR_BOUNDS);

		mStartDragginIndex = pNMLV->iItem;
		mStartDragginPoint = r.CenterPoint();

		mDragginLink = true;

		SetCapture ();
	}
	else
	{
		mDragImage->BeginDrag(0, CPoint(0,0));
		mDragImage->DragEnter(GetDesktopWindow(), pNMLV->ptAction);
		
		mDragging = true;	
		mDragList = &mListCtrlCanvas;
		mDropWnd = &mListCtrlCanvas;

		mElementDragging.Index = pNMLV->iItem;

		SetCapture ();
	}

	*pResult = 0;
}

void CNATSimulatorView::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( mDragginLink )
		mListCtrlCanvas.Invalidate();

	if (mDragging)
	{
		CPoint pt(point);
		ClientToScreen(&pt);

		CRect r;
		mListCtrlElements.GetItemRect(0, &r, LVIR_ICON);

		pt.x = pt.x - ((r.right - r.left)/2);
		pt.y = pt.y - ((r.bottom - r.top)/2);

		mDragImage->DragMove(pt);
		mDragImage->DragShowNolock(false);

		CWnd* pDropWnd = WindowFromPoint (pt);
		mDropWnd = pDropWnd;
		pDropWnd->ScreenToClient(&pt);

		if(pDropWnd->IsKindOf(RUNTIME_CLASS(CListCtrl)) && pDropWnd->m_hWnd == mListCtrlCanvas.m_hWnd )
		{
			SetCursor(LoadCursor(NULL, IDC_ARROW));
		}
		else
		{
			SetCursor(LoadCursor(NULL, IDC_NO));
		}
		mDragImage->DragShowNolock(true);
	}

	// DEBUG
	//CPoint org;
	//char buffer[10];
	//CString Text = "";
	//Text  = "OnMouseMove ";
	//Text += (CString)"( Window Point X = " + itoa(point.x, buffer,10) + " Y = " + itoa(point.y, buffer,10) + " ) ";
	//ClientToScreen(&point);
	////Text += (CString)"( Screen Point X = " + itoa(point.x, buffer,10) + " Y = " + itoa(point.y, buffer,10) + " ) ";
	//mListCtrlCanvas.ScreenToClient(&point);
	//Text += (CString)"( Canvas Point X = " + itoa(point.x, buffer,10) + " Y = " + itoa(point.y, buffer,10) + " ) ";
	//mListCtrlCanvas.GetOrigin(&org);
	//Text += (CString)"( Origin Point X = " + itoa(org.x, buffer,10) + " Y = " + itoa(org.y, buffer,10) + " ) ";

	//((CMainFrame*)AfxGetMainWnd())->SetStatusBarPaneText(0, Text);
	// DEBUG

	CFormView::OnMouseMove(nFlags, point);
}

void CNATSimulatorView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CPoint org;
	mListCtrlCanvas.GetOrigin(&org);

	if ( mDragginLink )
	{
		mDragginLink = false;

		ReleaseCapture ();

		mDragImage->DragLeave (GetDesktopWindow ());
		mDragImage->EndDrag ();

		UINT i, nCount = mListCtrlCanvas.GetItemCount();

		CPoint pt (point);
		ClientToScreen (&pt);
		mListCtrlCanvas.ScreenToClient(&pt);
		for (i=0;i < nCount;i++)
		{
			CRect r;
			mListCtrlCanvas.GetItemRect(i, &r, LVIR_BOUNDS);
			if (r.PtInRect(pt))
			{
				if ( GetDocument()->ValidateLink(mStartDragginIndex, i) )
				{
					UINT flag = LVIS_SELECTED | LVIS_FOCUSED;
					mListCtrlCanvas.SetItemState(i, flag, flag);
				}

				break;
			}
		}
		mListCtrlCanvas.Invalidate();
	}
	else if (mDragging)
	{
		ReleaseCapture ();

		mDragging = FALSE;

		mDragImage->DragLeave (GetDesktopWindow ());
		mDragImage->EndDrag ();
		delete mDragImage; 

		CPoint pt (point); 
		ClientToScreen (&pt);

		CWnd* pDropWnd = WindowFromPoint (pt);
		if (pDropWnd->IsKindOf (RUNTIME_CLASS (CListCtrl)))
		{
			if ( pDropWnd->m_hWnd == mListCtrlCanvas.m_hWnd )
			{
				if ( mDragList == &mListCtrlCanvas )
				{
					mListCtrlCanvas.ScreenToClient(&pt);

					CRect r;
					mListCtrlElements.GetItemRect(0, &r, LVIR_ICON);

					pt.x += org.x;
					pt.y += org.y;

					pt.x = pt.x - ((r.right - r.left)/2);
					pt.y = pt.y - ((r.bottom - r.top)/2);

					pt.x = pt.x < 0 ? 10: pt.x;
					pt.y = pt.y < 0 ? 10: pt.y;

					GetDocument()->MoveElement(mElementDragging.Index, pt);
					mListCtrlCanvas.SetItemPosition(mElementDragging.Index, pt);					
				}
				else
				{
					mListCtrlCanvas.ScreenToClient(&pt);

					mElementDragging.Position = pt;
					int index = GetDocument()->InsertElement(mElementDragging);
					mListCtrlCanvas.InsertItem(LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM, index, GetDocument()->GetItemData(index)->Name, 0, LVIS_SELECTED, GetDocument()->GetItemData(index)->Type, NULL);
					mListCtrlCanvas.SetCheck(index, TRUE);

					CRect r;
					mListCtrlElements.GetItemRect(0, &r, LVIR_BOUNDS);

					pt.x += org.x;
					pt.y += org.y;

					pt.x = pt.x - ((r.right - r.left)/2);
					pt.y = pt.y - ((r.bottom - r.top)/2);

					pt.x = pt.x < 0 ? 10: pt.x;
					pt.y = pt.y < 0 ? 10: pt.y;

					mListCtrlCanvas.SetItemPosition(index, pt);	

					// DEBUG
					//char buffer[10];
					//CString Text = "";
					//Text  = (CString)"Position ( X = " + itoa(pt.x, buffer,10) + " Y = " + itoa(pt.y, buffer,10) + " ) ";
					//mListCtrlCanvas.GetOrigin(&pt);
					//Text  += (CString)"Org( X = " + itoa(pt.x, buffer,10) + " Y = " + itoa(pt.y, buffer,10) + " ) ";
					//((CMainFrame*)AfxGetMainWnd())->SetStatusBarPaneText(0, Text);
					// DEBUG

				}
				mListCtrlCanvas.Invalidate();
			}
		}
	}

	CFormView::OnLButtonUp(nFlags, point);
}

//
//
void CNATSimulatorView::OnStnClickedStaticLink()
{
	if ( mLinking )
	{
		mStaticLink.SetBitmap(mBitmapLink);
		//mStaticLink.ModifyStyleEx(WS_EX_DLGMODALFRAME, 0);
		mStaticLink.Invalidate();
		mLinking = false;
	}
	else
	{
		mStaticLink.SetBitmap(mBitmapLinkChecked);
		//mStaticLink.ModifyStyleEx(0, WS_EX_DLGMODALFRAME);
		mStaticLink.Invalidate();
		mLinking = true;
	}
}

//
//
void CNATSimulatorView::OnCbnSelchangeComboDevices()
{
	CString strItem;

	int nIndex = mComboDevices.GetCurSel();
	if (nIndex == CB_ERR)
		return;

	mComboDevices.GetLBText(nIndex, strItem);

	GetDocument()->SetDevice(strItem);
}

void CNATSimulatorView::OnCbnSelchangeComboTests()
{
	CString strItem;

	int nIndex = mComboTests.GetCurSel();
	if (nIndex == CB_ERR)
		return ;

	mComboTests.GetLBText(nIndex, strItem);

	GetDocument()->SetTest(strItem);
}

//
//
void CNATSimulatorView::OnNMDblclkListCanvas(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint pt;
	GetCursorPos(&pt);
	mListCtrlCanvas.ScreenToClient(&pt);

	LVHITTESTINFO lvhti;

	lvhti.pt = pt;

	mListCtrlCanvas.SubItemHitTest(&lvhti);

	if ( lvhti.iItem >= 0 )
	{
		CElement * Element = GetDocument()->GetItemData(lvhti.iItem);
	
		CString aux;
		if ( Element->Type == eNAT )
		{
			CNATConfigDlg dlg;

			dlg.mNATConfiguration = Element;

			dlg.DoModal();

			Element->Name = Element->IPsB[0] + "\n" + Element->IPsA[0];
			mListCtrlCanvas.SetItemText(lvhti.iItem, 0, Element->Name);
		}
		else if ( Element->Type == eSTUNClient ||Element->Type == eSTUNClientReal )
		{
			CStunClientConfigDlg dlg;

			dlg.mConfiguration = Element;

			dlg.DoModal();		

			Element->Name = Element->IPsA[0];
			mListCtrlCanvas.SetItemText(lvhti.iItem, 0, Element->Name);
		} 
		else if ( Element->Type == eSTUNServer || Element->Type == eSTUNServerReal )
		{
			CStunServerConfigDlg dlg;

			dlg.mConfiguration = Element;

			dlg.DoModal();

			Element->Name = Element->IPsA[0] + "\n" + Element->IPsB[0];
			mListCtrlCanvas.SetItemText(lvhti.iItem, 0, Element->Name);
		}
	}

	*pResult = 0;
}


void CNATSimulatorView::OnNMRclickListCanvas(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint pt;
	GetCursorPos(&pt);
	mListCtrlCanvas.ScreenToClient(&pt);

	LVHITTESTINFO lvhti;

	lvhti.pt = pt;

	mListCtrlCanvas.SubItemHitTest(&lvhti);

	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_MENU_CANVAS));
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	if ( lvhti.iItem < 0 )
	{
		pPopup->EnableMenuItem(ID_DELETEITEM, MF_DISABLED | MF_GRAYED);
	}
	mListCtrlCanvas.ClientToScreen(&pt);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);

	*pResult = 0;
}

void CNATSimulatorView::OnNMCustomdrawListCanvas(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	*pResult = 0;

	CDC*			dc; 
	POINT			Point;

	dc = CDC::FromHandle(pNMCD->hdc);

	//dc->SelectObject(&mGridPen);

	//char buffer[10];
	//CPoint org;
	//mListCtrlCanvas.GetOrigin(&org);

	//for ( int x = -org.x; x < 2000; x+= 10 )
	//{
	//	dc->MoveTo(x, 0);
	//	dc->LineTo(x, 2000);
	//}

	//for ( int y = -org.y; y < 2000; y+= 10 )
	//{
	//	dc->MoveTo(0, y);
	//	dc->LineTo(2000, y);
	//}

	//dc->SelectObject(&mPen);
	//for ( int x = -org.x; x < 2000; x+= 100 )
	//{
	//	dc->MoveTo(x, 0);
	//	dc->LineTo(x, 10);
	//	dc->DrawText(itoa(x,buffer,10), CRect(CPoint(x, 0), CPoint(x+30,30)), DT_CENTER );
	//}

	//for ( int y = -org.y; y < 2000; y+= 100 )
	//{
	//	dc->MoveTo(0, y);
	//	dc->LineTo(10, y);
	//	dc->DrawText(itoa(y,buffer,10), CRect(CPoint(0,y), CPoint(30,y+30)), DT_CENTER );
	//}

	dc->SelectObject(&mPen);

	if ( mDragginLink )
	{
		GetCursorPos(&Point);
		mListCtrlCanvas.ScreenToClient(&Point);

		dc->MoveTo(mStartDragginPoint);
		dc->LineTo(Point);
	}

	CElement *element;
	CElement *elementAux;
	POINT p;
	CRect r;
	CRect rAux;
	for ( int i=0; i < mListCtrlCanvas.GetItemCount(); i++ )
	{
		element = GetDocument()->GetItemData(i);

		mListCtrlCanvas.GetItemPosition(i, &p);
		mListCtrlCanvas.GetItemRect(i, &r, LVIR_BOUNDS);

		for ( int j=0; j < element->LinkedTo.GetCount(); j++ )
		{
			elementAux = (CElement*) element->LinkedTo[j];
			mListCtrlCanvas.GetItemRect(elementAux->Index, &rAux, LVIR_BOUNDS);
			dc->MoveTo(r.CenterPoint());
			dc->LineTo(rAux.CenterPoint());
		}
	}

	for ( int i=0; i < mListCtrlCanvas.GetItemCount(); i++ )
	{
		element = GetDocument()->GetItemData(i);

		mListCtrlCanvas.GetItemPosition(i, &p);
		mListCtrlCanvas.GetItemRect(i, &r, LVIR_BOUNDS);

		for ( int j=0; j < element->LinkedTo.GetCount(); j++ )
		{
			elementAux = (CElement*) element->LinkedTo[j];
			mListCtrlCanvas.GetItemRect(elementAux->Index, &rAux, LVIR_BOUNDS);
			dc->MoveTo(rAux.CenterPoint());

			if ( element->Type == eNAT && elementAux->Type == eNAT )
			{
				if ( element->LinkedInDevice[j] == ePublic )
				{
					//dc->SelectObject(&mGridPen);

					ARROWSTRUCT		Arrow;
					Arrow.fTheta = 1.1;
					Arrow.nWidth = 20;
					Arrow.bFill = true;

					POINT ArrowPoint;
					if ( r.CenterPoint().x > rAux.CenterPoint().x )
						ArrowPoint.x = r.CenterPoint().x - ( abs(r.CenterPoint().x - rAux.CenterPoint().x) / 2 );
					else
						ArrowPoint.x = rAux.CenterPoint().x - ( abs(rAux.CenterPoint().x - r.CenterPoint().x) / 2 );

					if ( r.CenterPoint().y > rAux.CenterPoint().y )
						ArrowPoint.y = r.CenterPoint().y - ( abs(r.CenterPoint().y - rAux.CenterPoint().y) / 2 );
					else
						ArrowPoint.y = rAux.CenterPoint().y - ( abs(rAux.CenterPoint().y - r.CenterPoint().y) / 2 );

					ArrowTo(dc->m_hDC, ArrowPoint.x, ArrowPoint.y, &Arrow);
				}
			}
		}
	}
}

void CNATSimulatorView::OnNMRclickListMessages(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint pt;
	GetCursorPos(&pt);
	//mListCtrlMessages.ScreenToClient(&pt);

	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_MENU_MESSAGES));
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	//mListCtrlMessages.ClientToScreen(&pt);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
	
	*pResult = 0;
}
//
//
void CNATSimulatorView::OnMarkall()
{
	for ( int i=0; i < mListCtrlCanvas.GetItemCount(); i++ )
		mListCtrlCanvas.SetCheck(i, TRUE);
}

void CNATSimulatorView::OnUnmarkall()
{
	for ( int i=0; i < mListCtrlCanvas.GetItemCount(); i++ )
		mListCtrlCanvas.SetCheck(i, FALSE);
}

void CNATSimulatorView::OnDeleteitem()
{
	POSITION pos = mListCtrlCanvas.GetFirstSelectedItemPosition();
	if ( pos )
	{
		int nItem = mListCtrlCanvas.GetNextSelectedItem(pos);
		if ( nItem>= 0 )
		{
			CElement* element = GetDocument()->GetItemData(nItem);
			for ( int i=nItem+1; i < mListCtrlCanvas.GetItemCount(); i++ )
			{
				CElement * element = GetDocument()->GetItemData(i);
				element->Index = i-1;
			}
			for ( int i=0; i < mListCtrlCanvas.GetItemCount(); i++ )
			{
				CElement * elementAux = GetDocument()->GetItemData(i);
				for ( int j=0; j < elementAux->LinkedTo.GetCount(); j++ )
				{
					if ( elementAux->LinkedTo[j] == element )
					{
						elementAux->LinkedTo.RemoveAt(j);
						elementAux->LinkedInDevice.RemoveAt(j);
						j--;
					}
				}
			}
			mListCtrlCanvas.DeleteItem(nItem);
			GetDocument()->DeleteItem(nItem);
			mListCtrlCanvas.Invalidate();
		}
	}
}


void CNATSimulatorView::OnCleantrafficmarks()
{
	LVITEM lvItem;
	for ( int i=0; i < mListCtrlCanvas.GetItemCount(); i++ )
	{
		lvItem.iItem = i;
		lvItem.iSubItem = 0;
		lvItem.mask = LVIF_IMAGE ;
		mListCtrlCanvas.GetItem(&lvItem);

		if ( GetDocument()->GetItemData(i)->Type == eNAT )
			lvItem.iImage = 0;
		else if ( GetDocument()->GetItemData(i)->Type == eSTUNServer )
			lvItem.iImage = 2;
		else if ( GetDocument()->GetItemData(i)->Type == eSTUNClient )
			lvItem.iImage = 1;
		
		lvItem.mask = LVIF_IMAGE;
		mListCtrlCanvas.SetItem (&lvItem);
	}
}
void CNATSimulatorView::OnBnClickedButtonRunTest()
{
	GetDocument()->StartTest();
}
void CNATSimulatorView::OnClean()
{
	mListCtrlMessages.DeleteAllItems();
}
//
//
void _cdecl CNATSimulatorView::sThreadAnimations(void* This)
{
	((CNATSimulatorView *) This)->ThreadAnimations(This);
}
//
//
void CNATSimulatorView::ThreadAnimations (void* This)
{
	CNATSimulatorView * pThis = (CNATSimulatorView*)This;

	int i = 0;
	CBitmap Bitmap1;
	Bitmap1.LoadBitmap(MAKEINTRESOURCE(IDB_BITMAP_RUNNING1));
	CBitmap Bitmap2;
	Bitmap2.LoadBitmap(MAKEINTRESOURCE(IDB_BITMAP_RUNNING2));
	CBitmap Bitmap3;
	Bitmap3.LoadBitmap(MAKEINTRESOURCE(IDB_BITMAP_RUNNING3));
	CBitmap Bitmap4;
	Bitmap4.LoadBitmap(MAKEINTRESOURCE(IDB_BITMAP_RUNNING4));

	while (!mTerminateAnimations)
	{
		if ( pThis->mStaticAnimation.m_hWnd )
		{
			if ( pThis->GetDocument()->GetTesting() )
			{
				pThis->mStaticAnimation.ShowWindow(SW_SHOW);
				pThis->mStaticAnimation.SetBitmap(i==0?Bitmap1:(i==1?Bitmap2:(i==2?Bitmap3:(i==3?Bitmap4:Bitmap1))));
				pThis->mStaticAnimation.Invalidate();

				i++;
				if ( i> 3 )
					i = 0;
			}
			else
			{
				pThis->mStaticAnimation.ShowWindow(SW_HIDE);
			}
		}

		Sleep(100);
	}
}
//
//
void CNATSimulatorView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if ( pHint )
	{
		CUpdateViewInformation *inf = (CUpdateViewInformation*)pHint;
		if ( lHint == UPDATE_VIEW_ITEM_NAME )
		{
			mListCtrlCanvas.SetItemText(inf->Item, 0, inf->String1);
		}
		else if ( lHint == UPDATE_VIEW_MESSAGE_SEND || lHint == UPDATE_VIEW_MESSAGE_RECV )
		{
			if ( !(mListCtrlCanvas.GetCheck(inf->Item)) )
				return;

			LVITEM lvItem;
			lvItem.iItem = inf->Item;
			lvItem.iSubItem = 0;
			lvItem.mask = LVIF_IMAGE | LVIF_PARAM ;
			mListCtrlCanvas.GetItem(&lvItem);

			if ( GetDocument()->GetItemData(inf->Item)->Type == eNAT )
				lvItem.iImage = 5;
			else if ( GetDocument()->GetItemData(inf->Item)->Type == eSTUNServer )
				lvItem.iImage = 6;
			else if ( GetDocument()->GetItemData(inf->Item)->Type == eSTUNClient )
				lvItem.iImage = 7;
			
			lvItem.mask = LVIF_IMAGE;
			mListCtrlCanvas.SetItem (&lvItem);

			if ( mListCtrlMessages )
			{
				int i = mListCtrlMessages.InsertItem(mListCtrlMessages.GetItemCount(), lHint == UPDATE_VIEW_MESSAGE_SEND?"SEND":"RECV");	
				mListCtrlMessages.SetItemText(i, 1, inf->String1);
				mListCtrlMessages.SetItemText(i, 2, inf->String2);
				mListCtrlMessages.SetItemText(i, 3, inf->String3);
				mListCtrlMessages.SetItemText(i, 4, inf->String4);
				mListCtrlMessages.SetItemText(i, 5, inf->String5);
				mListCtrlMessages.SetItemText(i, 6, inf->String6);
				mListCtrlMessages.SetItemText(i, 7, inf->String7);
				mListCtrlMessages.SetItemText(i, 8, inf->String8);
				mListCtrlMessages.SetItemText(i, 9, inf->String9);
				mListCtrlMessages.SetItemText(i, 10, inf->String10);
				mListCtrlMessages.SetItemText(i, 11, inf->String11);
			}
		}
	}
}






