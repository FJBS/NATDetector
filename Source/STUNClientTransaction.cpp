/*
 *  STUNSender.cpp
 *  NATDetector
 *
 *  Created by System Administrator on 09/01/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "STUNDefs.h"

CSTUNClientTransaction::CSTUNClientTransaction(CUtilTrace* Trace)
{	
	mTrace = Trace;

	mRawSocketReceiver = new CRawSocket(mTrace);
	mRawSocketSender = new CRawSocket(mTrace);
	mCaptureReceiver = new CCaptureApi(mTrace);
}

CSTUNClientTransaction::~CSTUNClientTransaction()
{
	delete mRawSocketReceiver;
	delete mRawSocketSender;
	delete mCaptureReceiver;
}

bool CSTUNClientTransaction::Create(CUtilString & sourceAddress, word sourcePort, CUtilString & dev, CUtilConfigFile & Config)
{
	bool ret = false;
	
	CArray<CUtilString> sourceAddresses;
	sourceAddresses.Add(sourceAddress);

	CArray<dword> sourcePorts;
	sourcePorts.Add((dword)sourcePort);

	CArray<CUtilString> serverAddresses;
	//serverAddresses.Add(Config.GetString("STUNServer", "ServerAIP", "69.0.209.22"));
	//serverAddresses.Add(Config.GetString("STUNServer", "ServerBIP", "69.0.208.27"));

	CArray<dword> serverPorts;
	//serverPorts.Add((dword)Config.GetLong("STUNServer", "ServerPort1", 3478));
	//serverPorts.Add((dword)Config.GetLong("STUNServer", "ServerPort2", 3479));

	if ( mCaptureReceiver->Start(serverAddresses, serverPorts, sourceAddresses, sourcePorts, dev) )
	{
		//if ( mRawSocketReceiver->CreateReceiver() )
		//{
		//	if ( mRawSocketReceiver->Bind(sourceAddress, sourcePort) )
		//	{ 
				if ( mRawSocketSender->CreateSender() )
				{
					ret = true;
				}
		//	}
		//}
	}
	
	return ret;
}

bool CSTUNClientTransaction::Terminate()
{
	bool ret = true;
	
	ret &= mRawSocketSender->TerminateSender();
	//ret &= mRawSocketReceiver->TerminateReceiver();
	mCaptureReceiver->Stop();

	return ret;
}

bool CSTUNClientTransaction::SendAndReceive(CUtilByteArray & dataSend, CUtilByteArray ** dataRecv, const char* sourceAddr, word sourcePort, const char* destAddr, word destPort)
{
	// TODO
	/*		
	 * Once formulated and sent, the client sends the Binding Request.  Reliability
	 * is accomplished through request retransmissions.  The ClientTransaction
	 * retransmits the request starting with an interval of 100ms, doubling
	 * all retransmit until the interval reaches 1.6s.  Retransmissions
	 * continue with intervals of 1.6s until a response is received, or a
	 * total of 9 requests have been sent. If no response is received by 1.6
	 * seconds after the last request has been sent, the client SHOULD
	 * consider the transaction to have failed. In other words, requests
	 * would be sent at times 0ms, 100ms, 300ms, 700ms, 1500ms, 3100ms,
	 * 4700ms, 6300ms, and 7900ms. At 9500ms, the client considers the
	 * transaction to have failed if no response has been received.
		switch ( retries )
		{
		case 0:
			timeout.tv_usec = 100 * 1000;
			break;
		case 1:
			timeout.tv_usec = 300 * 1000;
			break;
		case 2:
			timeout.tv_usec = 700 * 1000;
			break;
		case 3:
			timeout.tv_sec = 1;
			timeout.tv_usec = 500 * 1000;
			break;
		case 4:
			timeout.tv_sec = 3;
			timeout.tv_usec = 100 * 1000;
			break;
		case 5:
			timeout.tv_sec = 4;
			timeout.tv_usec = 700 * 1000;
			break;
		case 6:
			timeout.tv_sec = 6;
			timeout.tv_usec = 300 * 1000;
			break;
		case 7:
			timeout.tv_sec = 7;
			timeout.tv_usec = 900 * 1000;
			break;
		case 8:
			timeout.tv_sec = 9;
			timeout.tv_usec = 500 * 1000;
			break;
		default:
			break;
		}	
	*/
	bool			ret = false;
	
	int				retries = 0;
	CaptureStruct	packet;

	*dataRecv = NULL;

	while ( !ret && retries < 3 )
	{
		ret = mRawSocketSender->Send(dataSend, sourceAddr, sourcePort, destAddr, destPort);

		if ( ret )
			ret = mCaptureReceiver->GetMsg(packet, 300);

		if ( ret )
			*dataRecv = new CUtilByteArray(packet.Payload);

		retries++;
	}

	if ( !ret )
		mTrace->Trace("**ERROR**", __FUNCTION__, "Transaction not ended.");

	return ret;
}

bool CSTUNClientTransaction::Receive(CUtilByteArray ** dataRecv)
{
	bool			ret = true;
	
	CaptureStruct	packet;
	
	*dataRecv = NULL;

	ret = mCaptureReceiver->GetMsg(packet, 300);

	if ( ret )
		*dataRecv = new CUtilByteArray(packet.Payload);

	if ( !ret )
		mTrace->Trace("**ERROR**", __FUNCTION__, "Message not received. Possible timeout receiving.");

	return ret;
}
