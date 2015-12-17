// NATSimulatorDoc.cpp : implementation of the CNATSimulatorDoc class
//

#include "stdafx.h"
#include "NATSimulator.h"

#include "NATSimulatorDoc.h"
#include "NATSimulatorView.h"
#include "MainFrm.h"
#include "TestDlg.h"
#include "NATConnectDlg.h"
#include ".\natsimulatordoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CElement, CObject, 0)

CElement::CElement()
{
	Visited = false;
}

CElement::~CElement()
{
}

CElement & CElement::operator = (const CElement & other)
{
	IPsA.RemoveAll();
	PortsA.RemoveAll();

	IPsB.RemoveAll();
	PortsB.RemoveAll();

	Devices.RemoveAll();

	Name = other.Name;
	Index = other.Index;

	Type = other.Type;

	IPsA.Copy(other.IPsA);
	PortsA.Copy(other.PortsA);

	IPsB.Copy(other.IPsB);
	PortsB.Copy(other.PortsB);

	Mapping = other.Mapping;
	Filtering = other.Filtering;
	Timeout = other.Timeout;
	InboundRefresh = other.InboundRefresh;
	PortPreservation = other.PortPreservation;
	PortOverloading = other.PortOverloading;
	AssignIPsMode = other.AssignIPsMode;
	Devices.Copy(other.Devices);

	LinkedTo.Copy(other.LinkedTo);
	LinkedInDevice.Copy(other.LinkedInDevice);

	Visited = other.Visited;

	Position = other.Position;

	return *this;
}

// CNATSimulatorDoc

IMPLEMENT_DYNCREATE(CNATSimulatorDoc, CDocument)

BEGIN_MESSAGE_MAP(CNATSimulatorDoc, CDocument)
END_MESSAGE_MAP()


// CNATSimulatorDoc construction/destruction

CNATSimulatorDoc::CNATSimulatorDoc()
{
	mMainFrame = (CMainFrame*)AfxGetMainWnd();
}

CNATSimulatorDoc::~CNATSimulatorDoc()
{
}

//
//
BOOL CNATSimulatorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	InitIPAdrress();
	
	mTesting = false;

	mDevice = "";
	mTest = "";

	return TRUE;
}

BOOL CNATSimulatorDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	InitIPAdrress();

	mTesting = false;

	mDevice = "";
	mTest = "";

	return TRUE;
}

BOOL CNATSimulatorDoc::CanCloseFrame(CFrameWnd* pFrame)
{
	if (mTesting)
		return FALSE;

	return CDocument::CanCloseFrame(pFrame);
}

//
//
void CNATSimulatorDoc::OnCloseDocument()
{
	if (mTesting)
		return ;

	CElement * element;
	for ( int i = 0; i < mArrayElements.GetCount(); i++ )
	{
		element = (CElement*) this->GetItemData(i);
		delete element;
	}

	CDocument::OnCloseDocument();
}

// CNATSimulatorDoc serialization

void CNATSimulatorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
		WriteDocument(ar);
	}
	else
	{
		CString line;

		// TODO: add loading code here
		while (ar.ReadString(line))
		{
			if ( line.Left((int)strlen("#STUNServer")) == (CString)"#STUNServer" )
			{
				ReadSTUNServer(ar);
			}
			else if ( line.Left((int)strlen("#STUNClient")) == (CString)"#STUNClient" )
			{
				ReadSTUNClient(ar);
			}
			else if ( line.Left((int)strlen("#NATSim")) == (CString)"#NATSim" )	
			{
				ReadNAT(ar);
			}
			else if ( line.Left((int)strlen("#Links")) == (CString)"#Links" )	
			{
				ReadLinks(ar);
			}
		}
	}
}


// CNATSimulatorDoc diagnostics

#ifdef _DEBUG
void CNATSimulatorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CNATSimulatorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void CNATSimulatorDoc::InitIPAdrress()
{
	mNextNATPublic = 125;
	mNextNATPrivate = 3;
	mNextPublic = 0;
	mNextPrivate = 2;
	mNextPrivateIPNatToNat = 1;

	mNextNATIndex = 1;
	mNextClientIndex = 1;
	mNextServerIndex = 1;
}
//
//
void CNATSimulatorDoc::GetNextElementToInsert(eElementType type, CElement & element)
{
	char buffer[20];

	element.Type = type;

	element.IPsA.RemoveAll();
	element.IPsB.RemoveAll();
	element.PortsA.RemoveAll();
	element.PortsB.RemoveAll();

	switch ( element.Type )
	{
	case eNAT:
		element.Name += itoa(mNextNATIndex, buffer, 10);

		element.IPsA.Add("217." + (CString)ltoa(mNextNATPublic, buffer, 10) + ".242.39");
		element.IPsA.Add("217." + (CString)ltoa(mNextNATPublic, buffer, 10) + ".242.40");

		element.IPsB.Add("192.168." + (CString)ltoa(mNextNATPrivate, buffer, 10) + ".1");

		element.Mapping = CNATSim::eMEndPointIndependent;
		element.Filtering = CNATSim::eFAddressIndependent;

		element.Timeout = 300;

		element.InboundRefresh = false;
		element.PortPreservation = true;
		element.PortOverloading = false;
		element.AssignIPsMode = CNATSim::eAPaired;

		mNextNATPrivate += 1;
		mNextNATPublic += 1;
		mNextNATIndex +=1;

		break;

	case eSTUNClient:
		element.Name += itoa(mNextClientIndex, buffer, 10);

		element.IPsA.Add("192.168.3." + (CString)ltoa(mNextPrivate, buffer, 10));
		element.PortsA.Add(10000);

		mNextPrivate += 1;
		mNextClientIndex += 1;

		break;

	case eSTUNServer:
		element.Name += itoa(mNextServerIndex, buffer, 10);

		element.IPsA.Add("169." + (CString)ltoa(mNextPublic, buffer, 10) + ".209.22");
		element.PortsA.Add(3478);
		element.IPsB.Add("169." + (CString)ltoa(mNextPublic, buffer, 10) + ".208.27");
		element.PortsB.Add(3479);

		mNextPublic+=1;
		mNextServerIndex += 1;

		break;

	case eSTUNClientReal:

		char address[100];
 		char buf[256];
		buf[0]=0;
 		if ( gethostname(buf, 256) == 0)
		{
			struct addrinfo* l_addrInfo = NULL;
 			struct addrinfo l_addrInfoHints;

			ZeroMemory(&l_addrInfoHints, sizeof(addrinfo));

 			l_addrInfoHints.ai_socktype = SOCK_STREAM;
 			l_addrInfoHints.ai_family = PF_INET;
		
		 	if ( getaddrinfo(buf,NULL, &l_addrInfoHints,&l_addrInfo) == 0 )
 			{
				strcpy(address, inet_ntoa(((struct sockaddr_in*)l_addrInfo->ai_addr)->sin_addr));
				element.Name = "localhost";		
				element.IPsA.Add(address);
				element.PortsA.Add(10000);
			}
 		}

		break;

	case eSTUNServerReal:

		// element.Name += itoa(mNextServerIndex, buffer, 10);

		element.IPsA.Add("69.0.208.27");
		element.PortsA.Add(3478);
		element.IPsB.Add("69.0.209.22");
		element.PortsB.Add(3479);

		break;
	}
}
//
//

int CNATSimulatorDoc::InsertElement(CElement & element)
{
	CPoint org;
	INT_PTR index = mArrayElements.GetCount();

	CElement *NewElement = new CElement();
	*NewElement = element;

	NewElement->Index = (int)index;
	switch ( NewElement->Type )
	{
	case eSTUNClient:
		NewElement->Name = element.IPsA[0];
		break;
	case eSTUNServer:
		NewElement->Name = element.IPsA[0] + "\n" + element.IPsB[0];
		break;
	case eNAT:
		NewElement->Name = element.IPsB[0] + "\n" + element.IPsA[0];
		break;
	case eSTUNClientReal:
		NewElement->Name = element.IPsA[0];
		break;
	case eSTUNServerReal:
		NewElement->Name = element.IPsA[0] + "\n" + element.IPsB[0];
		break;
	}

	mArrayElements.Add(NewElement);

	return (int)index;
}
//
//
void CNATSimulatorDoc::MoveElement(int index, CPoint point)
{
	ASSERT( mArrayElements.GetCount() > index );

	((CElement*)mArrayElements[index])->Position = point;
}

//
//
bool CNATSimulatorDoc::ValidateLink(int IdxStart, int IdxEnd)
{
	bool ret = true;

	char buffer[10];

	eConnector startConnector = ePublic;
	eConnector endConnector = ePublic;

	CElement *elementStart = (CElement*)GetItemData(IdxStart);
	CElement *elementEnd = (CElement*)GetItemData(IdxEnd);
	
	if ( elementStart->Index == elementEnd->Index )
	{
		ret = false;
	}
	else if ( elementStart->Type == eSTUNClient )
	{
		if ( elementEnd->Type != eNAT )
			ret = false;
		else 
		{
			if ( IsReachable(elementEnd->Index, elementStart->Index ) )
				ret = false;
		}
	}
	else if ( elementStart->Type == eSTUNServer )
	{
		if ( elementEnd->Type != eNAT )
			ret = false;
		else
		{
			if ( IsReachable(elementEnd->Index, elementStart->Index ) )
				ret = false;
		}
	}
	else if ( elementStart->Type == eNAT )
	{
		if ( elementEnd->Type == eSTUNServer )
		{
			if ( IsReachable(elementStart->Index, elementEnd->Index ) )
				ret = false;
		}
		if ( elementEnd->Type == eSTUNClient )
		{
			if ( IsReachable(elementStart->Index, elementEnd->Index ) )
				ret = false;
		}
		if ( elementEnd->Type == eNAT )
		{
			CNatConnectDlg dlg;

			dlg.mNATAIPs = elementStart->IPsA[0];
			dlg.mNATBIPs = elementEnd->IPsA[0];

			INT_PTR opc = dlg.DoModal();

			if ( opc == IDOK )
			{
				startConnector = dlg.mConnectorNAT1;
				endConnector = dlg.mConnectorNAT2;
			}
			else
				ret = false;
		}

	}

	if ( ret )
	{
		if ( elementStart->Type == eNAT && elementEnd->Type == eNAT )
		{
			elementEnd->IPsB.RemoveAll();

			for ( int i=0; i < elementStart->IPsA.GetCount(); i++ )
			{
				CString IP = elementStart->IPsA[i];
				IP = IP.Left(IP.ReverseFind('.')) + "." + itoa(mNextPrivateIPNatToNat, buffer, 10);
				mNextPrivateIPNatToNat++;
				elementEnd->IPsB.Add(IP);

				CUpdateViewInformation UpdateViewInformation;
				UpdateViewInformation.Item = elementEnd->Index;
				UpdateViewInformation.String1 = elementEnd->IPsB[0] + "\n" + elementEnd->IPsA[0];
				UpdateAllViews(NULL, UPDATE_VIEW_ITEM_NAME, (CObject*)&UpdateViewInformation);

				break;
			}
		}

		elementEnd->LinkedTo.Add(elementStart);
		elementStart->LinkedTo.Add(elementEnd);
		elementEnd->LinkedInDevice.Add(startConnector);
		elementStart->LinkedInDevice.Add(endConnector);
	}

	return ret;
}
//
//
bool CNATSimulatorDoc::IsReachable(int IdxTo, int IdxFrom)
{
	bool		ret = false;

	ASSERT( mArrayElements.GetCount() > IdxTo );
	ASSERT( mArrayElements.GetCount() > IdxFrom );

	for ( int i = 0; i < mArrayElements.GetCount(); i++ )
		((CElement*)mArrayElements[i])->Visited = false;

	IsReachableAux(IdxTo, IdxFrom, ret);

	return ret;
}

void CNATSimulatorDoc::IsReachableAux(int IdxTo, int IdxFrom, bool & reachable)
{
	CElement *	element;

	if ( mArrayElements.GetCount() > IdxFrom )
	{
		element = (CElement*) GetItemData(IdxFrom);

		if ( !element->Visited/* && element->Type != eSTUNServer*/ )
		{
			for ( int i = 0; i < element->LinkedTo.GetCount(); i++ )
			{
				CElement * elementLinked = (CElement*) element->LinkedTo[i];

				if ( elementLinked->Index == IdxTo )
				{
					if ( elementLinked->Type == eNAT && element->Type == eNAT )
					{
						if ( element->LinkedInDevice[i] == ePrivate )
						{
							reachable = true;
							break;
						}
					}
					else
					{
						reachable = true;
						break;
					}
				}				
			}
			element->Visited = true;

			if ( !reachable )
			{
				for ( int i = 0; i < element->LinkedTo.GetCount(); i++ )
				{
					CElement * elementLinked = (CElement*) element->LinkedTo[i];
					if ( elementLinked->Type == eNAT && element->Type == eNAT )
					{
						if ( element->LinkedInDevice[i] == ePrivate )
							IsReachableAux(IdxTo, elementLinked->Index, reachable);
					}
					else
						IsReachableAux(IdxTo, elementLinked->Index, reachable);

					if ( reachable )
						break;
				}
			}
		}
	}


	return;
}
// CNATSimulatorDoc commands

CElement *CNATSimulatorDoc::GetItemData(int index)
{
	ASSERT( mArrayElements.GetCount() > index );
	return (CElement *)mArrayElements[index];
}
//
//
void CNATSimulatorDoc::DeleteItem(int index)
{
	ASSERT( mArrayElements.GetCount() > index );
	mArrayElements.RemoveAt(index);
}
int CNATSimulatorDoc::GetElementsCount()
{
	return mArrayElements.GetCount();
}
//
//
bool CNATSimulatorDoc::CreateConfigFiles()
{
	bool	ret = true;

	char*	p;
	char	Filename[512];
	char	buffer[10];

	CString Aux;

	CString STUNServer = "STUNServer";
	CString NATSim = "NATSim";

	char	chr = 'A';

	GetModuleFileName(NULL, Filename, sizeof(Filename));
	p = strrchr (Filename, '\\');
	strcpy (p+1, "\0");

	HANDLE hClientsFile = CreateFile(_T(Filename + (CString)"STUNClients.config"), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hClientsFile == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("Couldn't create the clients file!"));
		return false;
	}

	CFile clientsFile(hClientsFile);
	clientsFile.Write("#STUNClient\n", lstrlen("#STUNClient\n"));

	for ( int i=0, idxServer=1, idxNAT=1; i < mArrayElements.GetCount(); i++ )
	{
		CElement* elementA =  (CElement*)GetItemData(i);

		if ( elementA->Type == eSTUNServer || elementA->Type == eSTUNServerReal )
		{
			HANDLE hFile = CreateFile(_T(Filename + STUNServer + ltoa(idxServer++, buffer, 10) + ".config"), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

			if (hFile == INVALID_HANDLE_VALUE)
				AfxMessageBox(_T("Couldn't create the file!"));
			else
			{
				CFile file(hFile);

				file.Write("#STUNServer\n", lstrlen("#STUNServer\n"));

				Aux  = "ServerDev=" + mDevice + "\n";
				Aux += "ServerAIP=" + elementA->IPsA[0] + "\n";
				Aux += "ServerBIP=" + elementA->IPsB[0] + "\n\n";
				Aux += "ServerPort1=" + (CString)ltoa(elementA->PortsA[0],buffer,10) + "\n";
				Aux += "ServerPort2=" + (CString)ltoa(elementA->PortsB[0],buffer,10) + "\n\n";

				file.Write(Aux, Aux.GetLength());

				file.Write("#NATSim\n", lstrlen("#NATSim\n"));

				int ipNumber=1;
				for ( int k = 0; k < elementA->LinkedTo.GetCount(); k++ )
				{
					CElement * elementAux = (CElement*) elementA->LinkedTo[k];
					if ( elementAux->Type == eNAT )
					{
						Aux = "";
						for ( int l=0; l < elementAux->IPsA.GetCount(); l++ )
							Aux  += (CString)"NATPublicIP" + ltoa(ipNumber++, buffer, 10) + "=" + elementAux->IPsA[l] + "\n";

						Aux +="\n";
						file.Write(Aux, Aux.GetLength());
					}
				}

				file.Close();
			}
		}
		else if ( elementA->Type == eSTUNClient || elementA->Type == eSTUNClientReal )
		{
			Aux  = (CString)"Client" + chr + "Dev=" + mDevice + "\n";
			Aux += (CString)"Client" + chr + "IP=" + elementA->IPsA[0] + "\n";
			Aux += (CString)"Client" + chr + "Port=" + (CString)ltoa(elementA->PortsA[0],buffer,10) + "\n\n";

			clientsFile.Write(Aux, Aux.GetLength());

			chr ++;
		}
		else if ( elementA->Type == eNAT )
		{
			HANDLE hFile = CreateFile(_T(Filename + NATSim + ltoa(idxNAT++, buffer, 10) + ".config"), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

			if (hFile == INVALID_HANDLE_VALUE)
				AfxMessageBox(_T("Couldn't create the NAT file!"));
			else
			{
				CFile file(hFile);

				file.Write("#NATSim\n", lstrlen("#NATSim\n"));

				if ( elementA->Mapping == CNATSim::eMAddressDependent )
				{
					Aux  = (CString)"MappingBehavior=AddressDependent\n";
				}
				else if ( elementA->Mapping == CNATSim::eMAddressPortDependent )
				{
					Aux  = (CString)"MappingBehavior=AddressPortDependent\n";
				}
				else if ( elementA->Mapping == CNATSim::eMEndPointIndependent )
				{
					Aux  = (CString)"MappingBehavior=EndPointIndependent\n";
				}
				else if ( elementA->Mapping == CNATSim::eMNotNated )
				{
					Aux  = (CString)"MappingBehavior=NotNated\n";
				}

				if ( elementA->Filtering == CNATSim::eFAddressDependent )
				{
					Aux += (CString)"FilteringBehavior=AddressDependent\n";
				}
				else if ( elementA->Filtering == CNATSim::eFAddressIndependent )
				{
					Aux += (CString)"FilteringBehavior=AddressIndependent\n";
				}
				else if ( elementA->Filtering == CNATSim::eFAddressPortDependent )
				{
					Aux += (CString)"FilteringBehavior=AddressPortDependent\n";
				}
	
				Aux += (CString)"TimeOut=" + ltoa(elementA->Timeout, buffer, 10) + "\n";
				Aux += (CString)"IncommingRefreshTimeOut=" + (CString)(elementA->InboundRefresh?"1":"0") + "\n";
				Aux += (CString)"PortPreservation=" + (CString)(elementA->PortPreservation?"1":"0") + "\n";
				Aux += (CString)"PortOverloading=" + (CString)(elementA->PortOverloading?"1":"0") + "\n";
				Aux += (CString)"IPAssignMode=" + (CString)(elementA->AssignIPsMode==CNATSim::eAPaired?"Paired":"Random") + "\n";

				if ( elementA->AssignIPsMode == CNATSim::eAPaired )
				{
					Aux += (CString)"IPAssignMode=Paired\n";
				}
				else if ( elementA->Filtering == CNATSim::eARandom )
				{
					Aux += (CString)"IPAssignMode=Random\n";
				}

				file.Write(Aux, Aux.GetLength());

				Aux = "PublicDev=" + mDevice + "\n";
				Aux += "PrivateDev=" + mDevice + "\n";
				for ( int k=0; k < elementA->IPsB.GetCount(); k++ )
					Aux  += (CString)"NATPrivateIP" + ltoa(k+1, buffer, 10) + "=" + elementA->IPsB[k] + "\n";

				for ( int k=0; k < elementA->IPsA.GetCount(); k++ )
					Aux  += (CString)"NATPublicIP" + ltoa(k+1, buffer, 10) + "=" + elementA->IPsA[k] + "\n";
		
				int pri = 1;
				int pub = 1;
				for ( int k = 0; k < elementA->LinkedTo.GetCount(); k++ )
				{
					CElement * elementAux = (CElement*) elementA->LinkedTo[k];
					if ( elementAux->Type == eSTUNClient )
					{
						for ( int l=0; l < elementAux->IPsA.GetCount(); l++ )
							Aux  += (CString)"PrivateIP" + ltoa(pri++, buffer, 10) + "=" + elementAux->IPsA[l] + "\n";
					}
					else if ( elementAux->Type == eNAT )
					{
						//if ( elementA->LinkedInDevice[k] == ePrivate )
						//{
						//	for ( int l=0; l < elementAux->IPsA.GetCount(); l++ )
						//		Aux  += (CString)"PublicIP" + ltoa(pub++, buffer, 10) + "=" + elementAux->IPsA[l] + "\n";
						//}
						if ( elementA->LinkedInDevice[k] == ePublic )
						{
							for ( int l=0; l < elementAux->IPsA.GetCount(); l++ )
								Aux  += (CString)"PrivateIP" + ltoa(pri++, buffer, 10) + "=" + elementAux->IPsA[l] + "\n";
						}
					}
				}

				for ( int k=0; k < mArrayElements.GetCount(); k++ )
				{
					CElement * elementAux =  (CElement*)GetItemData(k);

					if ( elementAux->Index != elementA->Index && elementAux->Type == eSTUNServer )
					{
						if ( IsReachable(elementAux->Index, elementA->Index) )
						{
							Aux  += (CString)"PublicIP" + ltoa(pub++, buffer, 10) + "=" + elementAux->IPsA[0] + "\n";
							Aux  += (CString)"PublicIP" + ltoa(pub++, buffer, 10) + "=" + elementAux->IPsB[0] + "\n";
						}
					}
				}

				file.Write(Aux, Aux.GetLength());

				file.Close();
			}
		}
	}

	clientsFile.Close();

	return ret;
}
//
//
void CNATSimulatorDoc::SetDevice(CString Device)
{
	mDevice = Device;
}

void CNATSimulatorDoc::SetTest(CString Test)
{
	mTest = Test;
}
//
//
//
//
bool CNATSimulatorDoc::ReadSTUNServer(CArchive & ar)
{
	bool				ret = true;
	bool				end = false;

	CElement			*element = new CElement();
	CPoint				originPoint;
	CString				line;

	element->Type = eSTUNServer;

	while ( ret && !end)
	{
		ret = ar.ReadString(line);

		if ( !ret )
			break;

		if ( line.Left((int)strlen("ServerAIP=")) == (CString)"ServerAIP=" )
			element->IPsA.Add(line.Mid(line.Find("=")+1));
		else if ( line.Left((int)strlen("ServerBIP=")) == (CString)"ServerBIP=" )
			element->IPsB.Add(line.Mid(line.Find("=")+1));
		else if ( line.Left((int)strlen("ServerPort1=")) == (CString)"ServerPort1=" )
			element->PortsA.Add(atol((const char *)line.Mid(line.Find("=")+1)));
		else if ( line.Left((int)strlen("ServerPort2=")) == (CString)"ServerPort2=" )
			element->PortsB.Add(atol((const char *)line.Mid(line.Find("=")+1)));
		else if ( line.Left((int)strlen("PositionX=")) == (CString)"PositionX=" )
			element->Position.x = atol((const char *)line.Mid(line.Find("=")+1));
		else if ( line.Left((int)strlen("PositionY=")) == (CString)"PositionY=" )
			element->Position.y =  atol((const char *)line.Mid(line.Find("=")+1));
		else if ( line.Left((int)strlen("Index=")) == (CString)"Index=" )
			element->Index =  atol((const char *)line.Mid(line.Find("=")+1));
		else if ( line.Left((int)strlen("#STUNServer")) == (CString)"#STUNServer" )
			end = true;
	}

	//if ( ret && end )
	//{
	//	CPoint pAux(positionX, positionY);
	//	ClientToScreen (&pAux);
	//	element->Type = eSTUNServer;
	//	this->InsertElement(element, pAux, true);
	//	ret = true;
	//}
	//else
	//	ret = false;

	if ( ret & end )
	{
		element->Name = element->IPsA[0] + "\n" + element->IPsB[0];
		mArrayElements.Add(element);
	}

	return ret;
}


//
//
bool CNATSimulatorDoc::ReadSTUNClient(CArchive & ar)
{		
	bool				ret = true;
	bool				end = false;

	CElement			*element = new CElement();
	CPoint				originPoint;
	CString				line;

	element->Type = eSTUNClient;

	while ( ret && !end)
	{
		ret = ar.ReadString(line);

		if ( !ret )
			break;

		if ( line.Left((int)strlen("ClientIP=")) == (CString)"ClientIP=" )
			element->IPsA.Add(line.Mid(line.Find("=")+1));
		else if ( line.Left((int)strlen("ClientPort=")) == (CString)"ClientPort=" )
			element->PortsA.Add(atol((const char *)line.Mid(line.Find("=")+1)));
		else if ( line.Left((int)strlen("PositionX=")) == (CString)"PositionX=" )
			element->Position.x = atol((const char *)line.Mid(line.Find("=")+1));
		else if ( line.Left((int)strlen("PositionY=")) == (CString)"PositionY=" )
			element->Position.y =  atol((const char *)line.Mid(line.Find("=")+1));
		else if ( line.Left((int)strlen("Index=")) == (CString)"Index=" )
			element->Index =  atol((const char *)line.Mid(line.Find("=")+1));
		else if ( line.Left((int)strlen("#STUNClient")) == (CString)"#STUNClient" )
			end = true;	
	}

	//if ( ret && end )
	//{
	//	CPoint pAux(positionX, positionY);
	//	ClientToScreen (&pAux);
	//	element->Type = eSTUNClient;
	//	this->InsertElement(element, pAux, true);
	//	ret = true;
	//}
	//else
	//	ret = false;

	if ( ret & end )
	{
		element->Name = element->IPsA[0];
		mArrayElements.Add(element);
	}

	return ret;

}
//
//
bool CNATSimulatorDoc::ReadNAT(CArchive & ar)
{
	bool				ret = true;
	bool				end = false;

	CElement			*element = new CElement();
	CPoint				originPoint;
	CString				line;
	CString				Aux;

	element->Type = eNAT;

	while ( ret && !end)
	{
		ret = ar.ReadString(line);

		if ( !ret )
			break;

		if ( line.Left((int)strlen("MappingBehavior=")) == (CString)"MappingBehavior=" )
		{
			Aux = line.Mid(line.Find("=")+1);
			if ( Aux == "AddressDependent" )
				element->Mapping = CNATSim::eMAddressDependent;
			else if ( Aux == "AddressPortDependent" )
				element->Mapping = CNATSim::eMAddressPortDependent;
			else if ( Aux == "EndPointIndependent" )
				element->Mapping = CNATSim::eMEndPointIndependent;
			else if ( Aux == "NotNated" )
				element->Mapping = CNATSim::eMNotNated;
		}
		else if ( line.Left((int)strlen("FilteringBehavior=")) == (CString)"FilteringBehavior=" )
		{
			Aux = line.Mid(line.Find("=")+1);
			if ( Aux == "AddressDependent" )
				element->Filtering = CNATSim::eFAddressDependent;
			else if ( Aux == "AddressIndependent" )
				element->Filtering = CNATSim::eFAddressIndependent;
			else if ( Aux == "AddressPortDependent" )
				element->Filtering = CNATSim::eFAddressPortDependent;
		}
		else if ( line.Left((int)strlen("TimeOut=")) == (CString)"TimeOut=" )
		{
			Aux = line.Mid(line.Find("=")+1);
			element->Timeout = atol((const char *)Aux);
		}
		else if ( line.Left((int)strlen("IncommingRefreshTimeOut=")) == (CString)"IncommingRefreshTimeOut=" )
		{
			Aux = line.Mid(line.Find("=")+1);
			element->InboundRefresh = Aux=="0"?false:true;
		}
		else if ( line.Left((int)strlen("PortPreservation=")) == (CString)"PortPreservation=" )
		{
			Aux = line.Mid(line.Find("=")+1);
			element->PortPreservation = Aux=="0"?false:true;
		}
		else if ( line.Left((int)strlen("PortOverloading=")) == (CString)"PortOverloading=" )
		{
			Aux = line.Mid(line.Find("=")+1);
			element->PortOverloading = Aux=="0"?false:true;
		}
		else if ( line.Left((int)strlen("IPAssignMode=")) == (CString)"IPAssignMode=" )
		{
			Aux = line.Mid(line.Find("=")+1);
			element->AssignIPsMode = Aux=="Paired"?CNATSim::eAPaired:CNATSim::eARandom;
		}
		else if ( line.Left((int)strlen((CString)"NATPrivateIP")) == (CString)"NATPrivateIP" )
		{
			Aux = line.Mid(line.Find("=")+1);
			element->IPsB.Add(line.Mid(line.Find("=")+1));
		}
		else if ( line.Left((int)strlen((CString)"NATPublicIP")) == (CString)"NATPublicIP" )
		{
			Aux = line.Mid(line.Find("=")+1);
			element->IPsA.Add(line.Mid(line.Find("=")+1));
		}
		else if ( line.Left((int)strlen("PositionX=")) == (CString)"PositionX=" )
		{
			element->Position.x = atol((const char *)line.Mid(line.Find("=")+1));
		}
		else if ( line.Left((int)strlen("PositionY=")) == (CString)"PositionY=" )
		{
			element->Position.y =  atol((const char *)line.Mid(line.Find("=")+1));
		}
		else if ( line.Left((int)strlen("Index=")) == (CString)"Index=" )
		{
			element->Index =  atol((const char *)line.Mid(line.Find("=")+1));
		}
		else if ( line.Left((int)strlen("#NATSim")) == (CString)"#NATSim" )
			end = true;	
	}

	//if ( ret && end )
	//{
	//	CPoint pAux(positionX, positionY);
	//	ClientToScreen (&pAux);
	//	element.Type = eNAT;
	//	this->InsertElement(element, pAux, true);
	//	ret = true;
	//}
	//else
	//	ret = false;

	if ( ret & end )
	{
		element->Name = element->IPsB[0] + "\n" + element->IPsA[0];
		mArrayElements.Add(element);
	}

	return ret;
}
//
//
bool CNATSimulatorDoc::ReadLinks(CArchive & ar)
{
	bool				ret = true;
	bool				end = false;

	CString				line;
	CString				token;

	int					pos = 0;

	int					indexFrom, indexTo, device;

	while ( ret && !end)
	{
		ret = ar.ReadString(line);

		if ( !ret )
			break;

		pos = 0;
		token = line.Tokenize("= - :", pos);
		if ( token == "" )
			ret = false;
		else
			indexFrom = atol(token);

		while ( token != "" )
		{
			token = line.Tokenize("= - :", pos);
			if ( token != "" )
				indexTo = atol(token);
			else
				break;

			if ( ret )
			{
				token = line.Tokenize("= - :", pos);
				if ( token == "" )
					ret = false;
				else
					device = atol(token);
			}

			if ( ret )
			{
				CElement * element = (CElement *)GetItemData(indexFrom);
				element->LinkedTo.Add((void*)GetItemData(indexTo));
				element->LinkedInDevice.Add(device);
			}
			else
				break;
		}
	}

	if ( ret && end )
	{
		ret = true;
	}
	else
		ret = false;

	return ret;
}

//
//
void CNATSimulatorDoc::WriteDocument(CArchive & ar)
{
	CString		Links = "", Aux;
	char		buffer[10];

	CElement	*element;
	for ( int i = 0; i < mArrayElements.GetCount(); i++ )
	{
		element = (CElement*) this->GetItemData(i);

		if ( element->Type == eSTUNServer )
		{
			ar.Write("#STUNServer\n", lstrlen("#STUNServer\n"));
			Aux  = "ServerAIP=" + element->IPsA[0] + "\n";
			Aux += "ServerBIP=" + element->IPsB[0] + "\n";
			Aux += "ServerPort1=" + (CString)ltoa(element->PortsA[0],buffer,10) + "\n";
			Aux += "ServerPort2=" + (CString)ltoa(element->PortsB[0],buffer,10) + "\n";
			Aux += (CString)"PositionX=" + ltoa(element->Position.x/*+originPoint.x*/,buffer,10) + "\n";
			Aux += (CString)"PositionY=" + ltoa(element->Position.y/*+originPoint.y*/,buffer,10) + "\n";
			Aux += (CString)"Index=" + + ltoa(element->Index,buffer,10) + "\n";
			Aux += "#STUNServer\n\n";

			ar.Write(Aux, Aux.GetLength());

			CString linkedTo = "";
			for ( int j = 0; j < element->LinkedTo.GetCount(); j++ )
			{
				CElement * elementAux = (CElement*) element->LinkedTo[j];
				linkedTo += ltoa(elementAux->Index, buffer, 10);
				linkedTo += "-";
				linkedTo += ltoa(element->LinkedInDevice[j], buffer, 10);
				linkedTo += ":";
			}
			if ( linkedTo.GetLength() > 0 )
			{
				linkedTo = (CString)ltoa(element->Index, buffer,10) + "=" + linkedTo;
				Links += linkedTo + "\n";
			}

		}
		else if ( element->Type == eSTUNClient )
		{
			ar.Write("#STUNClient\n", lstrlen("#STUNClient\n"));
			Aux  = (CString)"ClientIP=" + element->IPsA[0] + "\n";
			Aux += (CString)"ClientPort=" + (CString)ltoa(element->PortsA[0],buffer,10) + "\n";
			Aux += (CString)"PositionX=" + ltoa(element->Position.x/*+originPoint.x*/,buffer,10) + "\n";
			Aux += (CString)"PositionY=" + ltoa(element->Position.y/*+originPoint.y*/,buffer,10) + "\n";
			Aux += (CString)"Index=" + + ltoa(element->Index,buffer,10) + "\n";
			Aux += "#STUNClient\n\n";

			ar.Write(Aux, Aux.GetLength());

			CString linkedTo = "";
			for ( int j = 0; j < element->LinkedTo.GetCount(); j++ )
			{
				CElement * elementAux = (CElement*) element->LinkedTo[j];
				linkedTo += ltoa(elementAux->Index, buffer, 10);
				linkedTo += "-";
				linkedTo += ltoa(element->LinkedInDevice[j], buffer, 10);
				linkedTo += ":";
			}
			if ( linkedTo.GetLength() > 0 )
			{
				linkedTo = (CString)ltoa(element->Index, buffer,10) + "=" + linkedTo;
				Links += linkedTo + "\n";
			}
		}
		else if ( element->Type == eNAT )
		{
			ar.Write("#NATSim\n", lstrlen("#NATSim\n"));

			if ( element->Mapping == CNATSim::eMAddressDependent )
			{
				Aux  = (CString)"MappingBehavior=AddressDependent\n";
			}
			else if ( element->Mapping == CNATSim::eMAddressPortDependent )
			{
				Aux  = (CString)"MappingBehavior=AddressPortDependent\n";
			}
			else if ( element->Mapping == CNATSim::eMEndPointIndependent )
			{
				Aux  = (CString)"MappingBehavior=EndPointIndependent\n";
			}
			else if ( element->Mapping == CNATSim::eMNotNated )
			{
				Aux  = (CString)"MappingBehavior=NotNated\n";
			}

			if ( element->Filtering == CNATSim::eFAddressDependent )
			{
				Aux += (CString)"FilteringBehavior=AddressDependent\n";
			}
			else if ( element->Filtering == CNATSim::eFAddressIndependent )
			{
				Aux += (CString)"FilteringBehavior=AddressIndependent\n";
			}
			else if ( element->Filtering == CNATSim::eFAddressPortDependent )
			{
				Aux += (CString)"FilteringBehavior=AddressPortDependent\n";
			}

			Aux += (CString)"TimeOut=" + ltoa(element->Timeout, buffer, 10) + "\n";
			Aux += (CString)"IncommingRefreshTimeOut=" + (CString)(element->InboundRefresh?"1":"0") + "\n";
			Aux += (CString)"PortPreservation=" + (CString)(element->PortPreservation?"1":"0") + "\n";
			Aux += (CString)"PortOverloading=" + (CString)(element->PortOverloading?"1":"0") + "\n";
			Aux += (CString)"IPAssignMode=" + (CString)(element->AssignIPsMode==CNATSim::eAPaired?"Paired":"Random") + "\n";

			ar.Write(Aux, Aux.GetLength());
			
			for ( int k=0; k < element->IPsB.GetCount(); k++ )
				Aux  += (CString)"NATPrivateIP" + ltoa(k+1, buffer, 10) + "=" + element->IPsB[k] + "\n";

			for ( int k=0; k < element->IPsA.GetCount(); k++ )
				Aux  += (CString)"NATPublicIP" + ltoa(k+1, buffer, 10) + "=" + element->IPsA[k] + "\n";

			Aux += (CString)"PositionX=" + ltoa(element->Position.x/*+originPoint.x*/,buffer,10) + "\n";
			Aux += (CString)"PositionY=" + ltoa(element->Position.y/*+originPoint.y*/,buffer,10) + "\n";
			Aux += (CString)"Index=" + + ltoa(element->Index,buffer,10) + "\n";
			Aux += "#NATSim\n\n";

			ar.Write(Aux, Aux.GetLength());

			CString linkedTo = "";
			for ( int j = 0; j < element->LinkedTo.GetCount(); j++ )
			{
				CElement * elementAux = (CElement*) element->LinkedTo[j];
				linkedTo += ltoa(elementAux->Index, buffer, 10);
				linkedTo += "-";
				linkedTo += ltoa(element->LinkedInDevice[j], buffer, 10);
				linkedTo += ":";
			}
			if ( linkedTo.GetLength() > 0 )
			{
				linkedTo = (CString)ltoa(element->Index, buffer,10) + "=" + linkedTo;
				Links += linkedTo + "\n";
			}
		}

	}

	if ( Links.GetLength() > 0 )
	{
		Aux = "#Links\n" + Links + "#Links\n\n";
		ar.Write(Aux, Aux.GetLength());
	}

}



//
//
bool CNATSimulatorDoc::GetTesting()
{
	return mTesting;
}
void CNATSimulatorDoc::StartTest()
{
	mThreadTest = (HANDLE*) _beginthread (sThreadTest, 0, this);
}

void _cdecl CNATSimulatorDoc::sThreadTest(void* This)
{
	((CNATSimulatorDoc *) This)->ThreadTest(This);
}
void CNATSimulatorDoc::ThreadTest (void* dummy)
{
	CTestDlg			dlg;
	CElement*			element;

	CUtilTrace**		TraceSTUNServer=NULL;
	CSTUNServer**		StunServer=NULL;
	int					numServers=0;

	CUtilTrace**		TraceNATSim=NULL;
	CNATSim**			NATSim=NULL;
	int					numNATs=0;

	char				buffer[10];

	CString mappingTest;
	mappingTest.LoadString(IDS_MAPPING_TEST);

	CString filetringTest;
	filetringTest.LoadString(IDS_FILTERING_TEST);

	CString bindingLifetimeTest;
	bindingLifetimeTest.LoadString(IDS_BINDING_LIFETIME);

	CString ipAddressPoolingTest;
	ipAddressPoolingTest.LoadString(IDS_ADDRESS_POOLING_TEST);

	CString portOverloadingTest;
	portOverloadingTest.LoadString(IDS_PORT_OVERLOADING_TEST);

	CUtilString fileName;

	if ( mDevice.GetLength() > 0 && mTest.GetLength() > 0 && mArrayElements.GetCount() > 0 )
	{
		dlg.mTest = mTest;

		for ( int i=0; i < mArrayElements.GetCount(); i++ )
		{
			element = (CElement*) this->GetItemData(i);
			switch (element->Type )
			{
			case eNAT:	
				numNATs++;
				break;
			case eSTUNClient:
			case eSTUNClientReal:
				dlg.mSTUNClients.Add(element->Name);
				dlg.mSTUNClientsIdx.Add(element->Index);
				break;
			case eSTUNServer:
				numServers++;
				dlg.mSTUNServer.Add(element->Name);
				dlg.mSTUNServerIdx.Add(element->Index);
				break;
			case eSTUNServerReal:
				dlg.mSTUNServer.Add(element->Name);
				dlg.mSTUNServerIdx.Add(element->Index);
				break;
			}
		}

		INT_PTR opc = dlg.DoModal();

		if ( opc == IDOK )
		{
			if ( CreateConfigFiles() )
			{
				mTesting = true;
				mMainFrame->IncreaseTestsRunning();

				CUtilTrace	Trace("NATDetector","NATDetector");
				CUtilString	report = "";

				WSADATA wsaData;
				if (WSAStartup(0x202,&wsaData) == SOCKET_ERROR) 
				{
					Trace.Trace("**ERROR**", __FUNCTION__, "WSAStartup failed with error %d",WSAGetLastError());
					WSACleanup();
				}

				TraceSTUNServer = new CUtilTrace*[numServers];
				StunServer = new CSTUNServer*[numServers];

				TraceNATSim = new CUtilTrace*[numNATs];
				NATSim = new CNATSim*[numNATs];

				for ( int i=0, numNAT=0, numServer=0; i < this->mArrayElements.GetCount(); i++ )
				{
					element = (CElement*)this->GetItemData(i);
					switch (element->Type )
					{
					case eNAT:

						fileName = (CUtilString)"NATSim" + ltoa(numNAT+1, buffer,10);
						TraceNATSim[numNAT] = new CUtilTrace((char *)(const char *)(fileName), (char *)(const char *)fileName);

						fileName += ".config";
						NATSim[numNAT] = new CNATSim((char *)(const char *)fileName, TraceNATSim[numNAT], element->Index, &CNATSimulatorDoc::AdviseSnd, &CNATSimulatorDoc::AdviseRcv, this);
						//NATSim[numNAT] = new CNATSim((char *)(const char *)fileName, TraceNATSim[numNAT]);

						NATSim[numNAT]->Start();
						numNAT++;

						Sleep(100);

						break;

					case eSTUNServer:

						fileName = (CUtilString)"STUNServer" + ltoa(numServer+1, buffer,10);
						TraceSTUNServer[numServer] = new CUtilTrace((char *)(const char *)(fileName), (char *)(const char *)fileName);

						fileName += ".config";
						StunServer[numServer] = new CSTUNServer((char *)(const char *)fileName, TraceSTUNServer[numServer], element->Index, &CNATSimulatorDoc::AdviseSnd, &CNATSimulatorDoc::AdviseRcv, this);
						//StunServer[numServer] = new CSTUNServer((char *)(const char *)fileName, TraceSTUNServer[numServer]);

						StunServer[numServer]->Start();
						numServer++;

						Sleep(100);

						break;

					}
				}
					
				Sleep(100);

				if ( mTest == mappingTest )
				{
					CUtilString	fromClient = "A";
					int			indexClient = 0;
					CUtilString	toServer = "1";
					char chr = 'A';
					char buffer[10];

					for ( int i=0, idxServer=1; i < this->mArrayElements.GetCount(); i++)
					{
						element = (CElement*) this->GetItemData(i);
						switch (element->Type )
						{
						case eSTUNClient:
							if ( element->Index == dlg.mSelectedClients[0] )
							{
								indexClient = element->Index;
								fromClient = chr;
							}
							chr++;
							break;
						case eSTUNServer:
							if ( element->Index == dlg.mSelectedServers[0] )
								toServer = itoa(idxServer, buffer, 10);
							idxServer ++;
							break;
						}
					}

					CUtilTrace Trace("Mapping", "Mapping");
					CNATReq1 NATReq1(&Trace, indexClient, &CNATSimulatorDoc::AdviseSnd, &CNATSimulatorDoc::AdviseRcv, this);

					NATReq1.Test(fromClient, toServer, report);
					AfxMessageBox((const char *)report);

					Sleep(100);
				}
				else if ( mTest == filetringTest )
				{
					CUtilString	fromClient = "A";
					int			indexClient = 0;
					CUtilString	toServer = "1";
					char chr = 'A';
					char buffer[10];

					for ( int i=0, idxServer=1; i <  this->mArrayElements.GetCount(); i++)
					{
						element = (CElement*) this->GetItemData(i);
						switch (element->Type )
						{
						case eSTUNClient:
							if ( element->Index == dlg.mSelectedClients[0] )
							{
								indexClient = element->Index;
								fromClient = chr;
							}
							chr++;
							break;
						case eSTUNServer:
							if ( element->Index == dlg.mSelectedServers[0] )
								toServer = itoa(idxServer, buffer, 10);
							idxServer ++;
							break;
						}
					}

					CUtilTrace Trace("Filtering", "Filtering");
					CNATReq8 NATReq8(&Trace, indexClient, &CNATSimulatorDoc::AdviseSnd, &CNATSimulatorDoc::AdviseRcv, this);

					NATReq8.Test(fromClient, toServer, report);
					AfxMessageBox((const char *) report);
					Sleep(100);
				}
				else if ( mTest == ipAddressPoolingTest )
				{
					CUtilString	fromClient = "A";
					int			indexClient = 0;
					CUtilString	toServer = "1";
					char chr = 'A';
					char buffer[10];

					for ( int i=0, idxServer=1; i <  this->mArrayElements.GetCount(); i++)
					{
						element = (CElement*) this->GetItemData(i);
						switch (element->Type )
						{
						case eSTUNClient:
							if ( element->Index == dlg.mSelectedClients[0] )
							{
								indexClient = element->Index;
								fromClient = chr;
							}
							chr++;
							break;
						case eSTUNServer:
							if ( element->Index == dlg.mSelectedServers[0] )
								toServer = itoa(idxServer, buffer, 10);
							idxServer ++;
							break;
						}
					}

					CUtilTrace Trace("Filtering", "Filtering");
					CNATReq2 NATReq2(&Trace, indexClient, &CNATSimulatorDoc::AdviseSnd, &CNATSimulatorDoc::AdviseRcv, this);

					NATReq2.Test(fromClient, toServer, dlg.mNumMessages, report);
					AfxMessageBox((const char *)report);

					Sleep(100);
				}
				else if ( mTest == bindingLifetimeTest )
				{
					CUtilTrace Trace("BindingLifetime", "BindingLifetime");

					CUtilString	fromClient = "A";
					CUtilString	toServer = "1";
					int			indexClient = 0;
					char chr = 'A';
					char buffer[10];

					for ( int i=0, idxServer=1; i < this->mArrayElements.GetCount(); i++)
					{
						element = (CElement*) this->GetItemData(i);
						switch (element->Type )
						{
						case eSTUNClient:
							if ( element->Index == dlg.mSelectedClients[0] )
							{
								indexClient = element->Index;
								fromClient = chr;
							}
							chr++;
							break;
						case eSTUNServer:
							if ( element->Index == dlg.mSelectedServers[0] )
								toServer = itoa(idxServer, buffer, 10);
							idxServer ++;
							break;
						}
					}

					CNATReq6 NATReq6(&Trace, indexClient, &CNATSimulatorDoc::AdviseSnd, &CNATSimulatorDoc::AdviseRcv, this);
					NATReq6.Test(fromClient, toServer, dlg.mTimeout*1000, report);
					AfxMessageBox((const char *)report);
					Sleep(500);
				} 
				else if ( mTest == portOverloadingTest )
				{
					CUtilTrace Trace("PortOverloading", "PortOverloading");

					CUtilString	fromClientA = "A";
					CUtilString	fromClientB = "A";
					CUtilString	toServer = "1";
					int			indexClientA = 0;
					int			indexClientB = 0;
					char chr = 'A';
					char buffer[10];

					for ( int i=0, idxServer=1; i < this->mArrayElements.GetCount(); i++)
					{
						element = (CElement*) this->GetItemData(i);
						switch (element->Type )
						{
						case eSTUNClient:
							if ( element->Index == dlg.mSelectedClients[0] )
							{
								indexClientA = element->Index;
								fromClientA = chr;
							}
							else if ( element->Index == dlg.mSelectedClients[1] )
							{
								indexClientB = element->Index;
								fromClientB = chr;
							}
							chr++;
							break;
						case eSTUNServer:
							if ( element->Index == dlg.mSelectedServers[0] )
								toServer = itoa(idxServer, buffer, 10);
							idxServer ++;
							break;
						}
					}

					CNATReq3 NATReq3(&Trace, indexClientA, indexClientB, &CNATSimulatorDoc::AdviseSnd, &CNATSimulatorDoc::AdviseRcv, this);
					NATReq3.Test(fromClientA, fromClientB, toServer, dlg.mNumMessages, report);
					AfxMessageBox((const char *)report);

					Sleep(500);
				} 	

				for ( int i = 0; i < numNATs; i++ )
				{
					NATSim[i]->Stop();
					delete TraceNATSim[i];
					delete NATSim[i];
				}

				for ( int i = 0; i < numServers; i++ )
				{
					StunServer[i]->Stop();
					delete TraceSTUNServer[i];
					delete StunServer[i];
				}

				delete TraceNATSim;
				delete NATSim;

				delete TraceSTUNServer;
				delete StunServer;

				WSACleanup();

				mMainFrame->DecreaseTestsRunning();
				mTesting = false;
			}
		}
	}
}

//
//
void CNATSimulatorDoc::AdviseSnd(int index, const char * FromIP, dword FromPort, const char * ToIP, dword ToPort, CUtilByteArray message, void * object)
{
	char buffer[10];
	CUpdateViewInformation UpdateViewInformation;

	CSTUNMsg & STUNMsg = CSTUNMsg::decode(NULL, message, 0);

	CSTUNAttribute	mappedAddress(NULL);
	CSTUNAttribute	changedAddress(NULL);
	CSTUNAttribute	xorMappedAddress(NULL);

	CUtilString strMappedAddress = "";
	CUtilString strChangedAddress = "";
	CUtilString strXorMappedAddress = "";
	CUtilString strMappedPort = "";
	CUtilString strChangedPort = "";
	CUtilString strXorMappedPort = "";

	CSTUNTransactionID transID = STUNMsg.getTransactionID();

	if ( STUNMsg.getAttribute(CSTUNAttribute::MAPPED_ADDRESS, mappedAddress) )
	{
		strMappedAddress = mappedAddress.getAddress().ToString();
		strMappedPort = itoa(mappedAddress.getPort(), buffer, 10);
	}
	if ( STUNMsg.getAttribute(CSTUNAttribute::CHANGED_ADDRESS, changedAddress) )
	{
		strChangedAddress = changedAddress.getAddress().ToString();
		strChangedPort = itoa(changedAddress.getPort(), buffer, 10);
	}
	if ( STUNMsg.getAttribute(CSTUNAttribute::XOR_MAPPED_ADDRESS, xorMappedAddress) )
	{
		strXorMappedAddress = xorMappedAddress.getAddress().ToString();
		strXorMappedPort = itoa(xorMappedAddress.getPort(), buffer, 10);
	}

	UpdateViewInformation.Item = index;
	UpdateViewInformation.String1 = FromIP;
	UpdateViewInformation.String2 = itoa(FromPort,buffer,10);
	UpdateViewInformation.String3 = ToIP;
	UpdateViewInformation.String4 = itoa(ToPort,buffer,10);
	UpdateViewInformation.String5 = (const char *)(CUtilString)transID;
	UpdateViewInformation.String6 = strMappedAddress;
	UpdateViewInformation.String7 = strMappedPort;
	UpdateViewInformation.String8 = strChangedAddress;
	UpdateViewInformation.String9 = strChangedPort;
	UpdateViewInformation.String10 = strXorMappedAddress;
	UpdateViewInformation.String11 = strXorMappedPort;

	CNATSimulatorDoc * doc = ((CNATSimulatorDoc*)object);
	
	POSITION pos = doc->GetFirstViewPosition();
	while (pos != NULL)
	{
		CNATSimulatorView* pView = (CNATSimulatorView*)doc->GetNextView(pos);
		pView->OnUpdate(NULL, UPDATE_VIEW_MESSAGE_SEND, (CObject*)&UpdateViewInformation);
	}

	delete & STUNMsg;
}

void CNATSimulatorDoc::AdviseRcv(int index, const char * FromIP, dword FromPort, const char * ToIP, dword ToPort, CUtilByteArray message, void * object)
{
	char buffer[10];
	CUpdateViewInformation UpdateViewInformation;

	CSTUNMsg & STUNMsg = CSTUNMsg::decode(NULL, message, 0);

	CSTUNAttribute	mappedAddress(NULL);
	CSTUNAttribute	changedAddress(NULL);
	CSTUNAttribute	xorMappedAddress(NULL);

	CUtilString strMappedAddress = "";
	CUtilString strChangedAddress = "";
	CUtilString strXorMappedAddress = "";
	CUtilString strMappedPort = "";
	CUtilString strChangedPort = "";
	CUtilString strXorMappedPort = "";

	CSTUNTransactionID transID = STUNMsg.getTransactionID();

	if ( STUNMsg.getAttribute(CSTUNAttribute::MAPPED_ADDRESS, mappedAddress) )
	{
		strMappedAddress = mappedAddress.getAddress().ToString();
		strMappedPort = itoa(mappedAddress.getPort(), buffer, 10);
	}
	if ( STUNMsg.getAttribute(CSTUNAttribute::CHANGED_ADDRESS, changedAddress) )
	{
		strChangedAddress = changedAddress.getAddress().ToString();
		strChangedPort = itoa(changedAddress.getPort(), buffer, 10);
	}
	if ( STUNMsg.getAttribute(CSTUNAttribute::XOR_MAPPED_ADDRESS, xorMappedAddress) )
	{
		strXorMappedAddress = xorMappedAddress.getAddress().ToString();
		strXorMappedPort = itoa(xorMappedAddress.getPort(), buffer, 10);
	}

	UpdateViewInformation.Item = index;
	UpdateViewInformation.String1 = FromIP;
	UpdateViewInformation.String2 = itoa(FromPort,buffer,10);
	UpdateViewInformation.String3 = ToIP;
	UpdateViewInformation.String4 = itoa(ToPort,buffer,10);
	UpdateViewInformation.String5 = (const char *)(CUtilString)transID;
	UpdateViewInformation.String6 = strMappedAddress;
	UpdateViewInformation.String7 = strMappedPort;
	UpdateViewInformation.String8 = strChangedAddress;
	UpdateViewInformation.String9 = strChangedPort;
	UpdateViewInformation.String10 = strXorMappedAddress;
	UpdateViewInformation.String11 = strXorMappedPort;

	CNATSimulatorDoc * doc = ((CNATSimulatorDoc*)object);
	
	POSITION pos = doc->GetFirstViewPosition();
	while (pos != NULL)
	{
		CNATSimulatorView* pView = (CNATSimulatorView*)doc->GetNextView(pos);
		pView->OnUpdate(NULL, UPDATE_VIEW_MESSAGE_RECV, (CObject*)&UpdateViewInformation);
	}

	delete & STUNMsg;
}




