// NATDetector.cpp : Defines the entry point for the console application.
//

#pragma comment(lib,"ws2_32.lib")  //winsock 2.2 library
#pragma comment(lib, "..\\Win_pthread\\lib\\pthreadVC2.lib")
#pragma comment(lib, "..\\Win_pcap\\lib\\wpcap.lib")
#pragma comment(lib, "..\\Win_lib\\Util.lib")
#pragma comment(lib, "..\\Win_lib\\RawSocket.lib")
#pragma comment(lib, "..\\Win_lib\\NATReq.lib")
#pragma comment(lib, "..\\Win_lib\\NATSim.lib")
#pragma comment(lib, "..\\Win_lib\\Stun.lib")

#include "..\\..\\Source\\UtilDefs.h"
#include "..\\..\\Source\\RawSocketDefs.h"
#include "..\\..\\Source\\CaptureDefs.h"
#include "..\\..\\Source\\STUNDefs.h"
#include "..\\..\\Source\\NATSimDefs.h"
#include "..\\..\\Source\\NATReqDefs.h"

int main(int argc, char* argv[])
{
	CUtilTrace	Trace("NATDetector","NATDetector");
	CUtilTrace	TraceSTUNServer("STUNServer","STUNServer");
	CUtilTrace	TraceNATSim1("NATSim1","NATSim1");
	CUtilTrace	TraceNATSim2("NATSim2","NATSim2");
	CUtilTrace	TraceNATSim3("NATSim3","NATSim3");
	
	CUtilString	report = "";

#ifdef WIN32

	WSADATA wsaData;
	if (WSAStartup(0x202,&wsaData) == SOCKET_ERROR) 
	{
		Trace.Trace("**ERROR**", __FUNCTION__, "WSAStartup failed with error %d",WSAGetLastError());
		WSACleanup();
	}

#endif

	//char		errbuf[PCAP_ERRBUF_SIZE];

	//pcap_if_t	*alldevs, *d;
	//u_int		inum;

	//int			i = 0;

	//printf("\nNo adapter selected: printing the device list:\n");
	//// The user didn't provide a packet source: Retrieve the local device list 
	//if (pcap_findalldevs(&alldevs, errbuf) == -1)
	//{
	//	fprintf(stderr,"Error in pcap_findalldevs_ex: %s\n", errbuf);
	//	return 0;
	//}
	//
	//// Print the list 
	//for(d=alldevs; d; d=d->next)
	//{
	//	printf("%d. %s\n    ", ++i, d->name);
	//	
	//	if (d->description)
	//		printf(" (%s)\n", d->description);
	//	else
	//		printf(" (No description available)\n");
	//}
	//
	//if (i==0)
	//{
	//	fprintf(stderr,"No interfaces found! Exiting.\n");
	//	return 0;
	//}
	//
	//printf("Enter the interface number (1-%d):",i);
	//scanf("%d", &inum);
	//
	//if (inum < 1 || inum > i)
	//{
	//	printf("\nInterface number out of range.\n");
	//	
	//	// Free the device list
	//	pcap_freealldevs(alldevs);
	//	return 0;
	//}
	//
	//// Jump to the selected adapter 
	//for (d=alldevs, i=0; i< inum-1 ;d=d->next, i++);

	/*
	CSTUNServer	StunServer("StunServer1.config", &TraceSTUNServer);
	StunServer.Start();

	CNATSim NATSim1("NATSim1.config", &TraceNATSim1);
	NATSim1.Start();

	//CNATSim NATSim2("NATSim2.config", &TraceNATSim2);
	//NATSim2.Start();

	//CNATSim NATSim3("NATSim3.config", &TraceNATSim3);
	//NATSim3.Start();

	//Sleep(10000);

	CNATReq1 NATReq1(&Trace);
	NATReq1.Test((CUtilString)"A", (CUtilString)"1", report);

	//CNATReq2 NATReq2(&Trace);
	//NATReq2.Test(report);

	//CNATReq3 NATReq3(&Trace);
	//NATReq3.Test(report);

	//CNATReq6 NATReq6(&Trace);
	//NATReq6.Test(29000, report);

	//CNATReq8 NATReq8(&Trace);
	//NATReq8.Test(report);

	//NATSim3.Stop();
	//NATSim2.Stop();
	NATSim1.Stop();
	StunServer.Stop();
	*/

	//if ( GetTickCount() > 0 )
	//{
	//	CSTUNRequest & req = CSTUNMsgFactory::createBindingRequest(&Trace, false, false);

	//	CUtilByteArray msg;
	//	req.encode(msg);

	//	CSTUNAddress		STUNMappedAddress;
	//	CSTUNAddress		STUNSourceAddress;
	//	CSTUNAddress		STUNChangedAddress;
	//	CSTUNAddress		STUNXORMappedAddress;
	//	CUtilString			mappedAddr = "192.168.10.1", serverAddrA = "199.1991.9.1", serverAddrB = "199.1991.9.1";	

	//	STUNMappedAddress = CSTUNAddress(mappedAddr, 10001);
	//	STUNSourceAddress = CSTUNAddress(serverAddrA, 3478);
	//	STUNChangedAddress = CSTUNAddress(serverAddrB, 3479);

	//	CSTUNResponse resp(&Trace);
	//	resp = CSTUNMsgFactory::createBindingResponse(&Trace, STUNMappedAddress, STUNSourceAddress, STUNChangedAddress, STUNXORMappedAddress );

	//	resp.encode(msg);
	//	resp.Trace();

	//	CSTUNResponse	resp2(&Trace);
	//	CSTUNAttribute	changedAddress(&Trace);
	//	CSTUNAttribute	xorMappedAddress(&Trace);
	//	CSTUNAttribute	mappedAddress(&Trace);
	//	CUtilString		sourceAddress = "192.168.10.1";

	//	resp2 = (CSTUNResponse&)CSTUNMsg::decode(&Trace, msg, 0);
	//	resp2.Trace();

	//	if ( resp2.getAttribute(CSTUNAttribute::CHANGED_ADDRESS, changedAddress) )
	//	{
	//		if ( resp2.getAttribute(CSTUNAttribute::XOR_MAPPED_ADDRESS, xorMappedAddress) )
	//		{
	//			if ( resp2.getAttribute(CSTUNAttribute::MAPPED_ADDRESS, mappedAddress) )
	//			{
	//				if ( mappedAddress.getAddress().ToString() == sourceAddress && mappedAddress.getPort() == 10001 )
	//				{
	//					Trace.Trace("**INFO **", __FUNCTION__, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>   NOT NATed - Endpoint independent\n");
	//				}
	//			}
	//		}
	//	}
	//}

	CUtilString	fromClient = "A";
	CUtilString	toServer = "1";
	CNATReq1 NATReq1(&Trace);
	NATReq1.Test(fromClient, toServer, report);


#ifdef WIN32

	WSACleanup();

#endif
	
	return 0;
}

