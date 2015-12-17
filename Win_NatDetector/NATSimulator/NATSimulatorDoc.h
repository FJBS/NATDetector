// NATSimulatorDoc.h : interface of the CNATSimulatorDoc class
//


#pragma once

class CMainFrame;

class CUpdateViewInformation : public CObject
{
public:
	int		Item;
	CString	String1;
	CString	String2;
	CString	String3;
	CString	String4;
	CString	String5;
	CString	String6;
	CString	String7;
	CString	String8;
	CString	String9;
	CString	String10;
	CString	String11;
};

class CElement : public CObject
{
public:

	CElement();
	DECLARE_SERIAL(CElement);

	virtual ~CElement();

public:

	CString							Name;
	int								Index;

	eElementType					Type;

	CStringArray					IPsA;
	CDWordArray						PortsA;

	CStringArray					IPsB;
	CDWordArray						PortsB;


	CNATSim::eMappingBehaviour		Mapping;
	CNATSim::eFilteringBehaviour	Filtering;
	dword							Timeout;
	bool							InboundRefresh;
	bool							PortPreservation;
	bool							PortOverloading;
	CNATSim::eIPAssignMode			AssignIPsMode;
	CStringArray					Devices;

	CPtrArray						LinkedTo;
	CDWordArray						LinkedInDevice;

	bool							Visited;

	CArray<CUtilString>				*Traffic;

	CElement & operator = (const CElement & other);

	CPoint							Position;
};

class CNATSimulatorDoc : public CDocument
{
protected: // create from serialization only
	CNATSimulatorDoc();
	DECLARE_DYNCREATE(CNATSimulatorDoc)

// Attributes
public:

// Operations
public:

	void GetNextElementToInsert(eElementType type, CElement & element);
	int InsertElement(CElement & element);
	void MoveElement(int index, CPoint point);
	bool ValidateLink(int IdxStart, int IdxEnd);

	void SetDevice(CString Device);
	void SetTest(CString Test);

	void StartTest();;
	bool GetTesting();

	void InitIPAdrress();
	bool CreateConfigFiles();

	bool ReadSTUNServer(CArchive & ar);
	bool ReadSTUNClient(CArchive & ar);
	bool ReadNAT(CArchive & ar);
	bool ReadLinks(CArchive & ar);

	void WriteDocument(CArchive & ar);

	CElement *GetItemData(int index);
	void DeleteItem(int index);

	int GetElementsCount();

	bool IsReachable(int Idx, int IdxFrom);
	void IsReachableAux(int Idx, int IdxFrom, bool & reacheable);

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void OnCloseDocument();
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CNATSimulatorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	CMainFrame	*mMainFrame;

	CPtrArray	mArrayElements;

	int		mNextPublic;
	int		mNextNATPublic;
	int		mNextNATPrivate;
	int		mNextPrivate;
	int		mNextPrivateIPNatToNat;

	int		mNextNATIndex;
	int		mNextClientIndex;
	int		mNextServerIndex ;

	CString	mDevice;
	CString mTest;

	// Test
	bool				mTesting;
	HANDLE				mThreadTest;
	static void _cdecl	sThreadTest (void*);
	void				ThreadTest (void * dummy);

	static void __stdcall AdviseSnd(int index, const char * FromIP, dword FromPort, const char * ToIP, dword ToPort, CUtilByteArray message, void *object);
	static void __stdcall AdviseRcv(int index, const char * FromIP, dword FromPort, const char * ToIP, dword ToPort, CUtilByteArray message, void *object);

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


