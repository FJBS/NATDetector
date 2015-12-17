// NATSimulatorView.h : interface of the CNATSimulatorView class
//


#pragma once
#include "afxcmn.h"

#include "SplitterBar.h"
#include "afxwin.h"

class CNATSimulatorView : public CFormView
{
protected: // create from serialization only
	CNATSimulatorView();
	DECLARE_DYNCREATE(CNATSimulatorView)

public:
	enum{ IDD = IDD_NATSIMULATOR_FORM };

// Attributes
public:
	CNATSimulatorDoc* GetDocument() const;

// Operations
public:

// Overrides
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CNATSimulatorView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

private:

	CSplitterBar	mSplitterBar;
	CListCtrl		mListCtrlElements;
	CListCtrl		mListCtrlCanvas;
	CListCtrl		mListCtrlMessages;

	CStatic			mStaticLink;
	CBitmap			mBitmapLink;
	CBitmap			mBitmapLinkChecked;

	CImageList		*mImageList;

	// Drag & Drop
	bool			mDragging;
	bool			mLinking;
	bool			mDragginLink;

	int				mDragIndex;
	CImageList*		mDragImage;
	CListCtrl*		mDragList;
	CWnd*			mDropWnd;

	CElement		mElementDragging;

	int				mStartDragginIndex;
	POINT			mStartDragginPoint;
	POINT			mEndDragginPoint;

	CPen			mPen;
	CPen			mGridPen;

	CArray<CUtilString>	mNetDevices;
	int					mNumDevices;

	CComboBox		mComboDevices;
	CComboBox		mComboTests;

	CStatic				mStaticAnimation;
	bool				mTerminateAnimations;
	HANDLE				mThreadAnimations;
	static void _cdecl	sThreadAnimations (void*);
	void				ThreadAnimations (void * dummy);

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMCustomdrawListCanvas(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnBegindragListElements(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnBegindragListCanvas(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnStnClickedStaticLink();
	afx_msg void OnCbnSelchangeComboDevices();
	afx_msg void OnCbnSelchangeComboTests();
	afx_msg void OnNMDblclkListCanvas(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNMRclickListCanvas(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMarkall();
	afx_msg void OnUnmarkall();
	afx_msg void OnDeleteitem();
	afx_msg void OnBnClickedButtonRunTest();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void OnCleantrafficmarks();
	afx_msg void OnNMRclickListMessages(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClean();
};

#ifndef _DEBUG  // debug version in NATSimulatorView.cpp
inline CNATSimulatorDoc* CNATSimulatorView::GetDocument() const
   { return reinterpret_cast<CNATSimulatorDoc*>(m_pDocument); }
#endif

