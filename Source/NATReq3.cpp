/*
 *  NATReq1.cpp
 *  NATDetector
 *
 *  Created by System Administrator on 30/12/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "NATReqDefs.h"

CNATReq3::CNATReq3(CUtilTrace * Trace)
{
	mTrace = Trace;

#ifndef WIN32
	mThreadClientA = NULL;
	mThreadClientB = NULL;
#else
	mThreadClientA.p = NULL;
	mThreadClientB.p = NULL;
#endif

	mAdviseSndFunction = NULL;
	mAdviseRcvFunction = NULL;
	mIndexA = -1;
	mIndexB = -1;
	mObject = NULL;

	mFromClientA = "";
	mFromClientB = "";
	mToServer = "";
}

CNATReq3::CNATReq3(CUtilTrace * Trace, int indexA, int indexB, AdviseSndFunction adviseSndF, AdviseRcvFunction adviseRcvF, void * object )
{
	mTrace = Trace;

#ifndef WIN32
	mThreadClientA = NULL;
	mThreadClientB = NULL;
#else
	mThreadClientA.p = NULL;
	mThreadClientB.p = NULL;
#endif

	mAdviseSndFunction = adviseSndF;
	mAdviseRcvFunction = adviseRcvF;
	mIndexA = indexA;
	mIndexB = indexB;
	mObject = object;

	mFromClientA = "";
	mFromClientB = "";
	mToServer = "";
}

CNATReq3::~CNATReq3()
{
}

bool CNATReq3::Test(CUtilString & fromClientA, CUtilString & fromClientB, CUtilString & toServer, int numMessages, CUtilString & report)
{
	bool ret = true;
	
	bool PortPreservation = true;
	bool PortOverloading = true;

	report = "";
	mTrace->Trace("**DEBUG**", __FUNCTION__ , "Starting port overloading test...");

	mFromClientA = fromClientA;
	mFromClientB = fromClientB;
	mToServer = toServer;

	CUtilConfigFile ConfigClient("STUNClients.config", mTrace);	
	word sourcePort = (word)ConfigClient.GetLong("STUNClient", (CUtilString)"Client" + mFromClientA + "Port", 10000);

#ifndef WIN32
	if ( mThreadClientA != NULL || mThreadClientB != NULL )
#else
	if ( mThreadClientA.p != NULL || mThreadClientB.p != NULL  )
#endif
		return false;

	while ( numMessages-- > 0 && PortPreservation && PortOverloading )
	{
		mReceivedInA = false;
		mMappedAddressA = new CSTUNAttribute(mTrace);

		if ( pthread_create(&mThreadClientA, 0, staticThreadClientAProc, this) != 0 )
		{
			mTrace->Trace("**ERROR**", __FUNCTION__, "Failed pthread_create for client A.");
			report += "Failed pthread_create for client A.";
			ret = false;
		}

		pthread_join(mThreadClientA, 0);
		
#ifndef WIN32
	mThreadClientA = NULL;
#else
	mThreadClientA.p = NULL;
#endif

		mReceivedInB = false;
		mMappedAddressB = new CSTUNAttribute(mTrace);

		if ( pthread_create(&mThreadClientB, 0, staticThreadClientBProc, this) != 0 )
		{
			mTrace->Trace("**ERROR**", __FUNCTION__, "Failed pthread_create for client B.");
			report += "Failed pthread_create for client B.";
			ret = false;
		}

		pthread_join(mThreadClientB, 0);
		
#ifndef WIN32
	mThreadClientB = NULL;
#else
	mThreadClientB.p = NULL;
#endif

		if ( ret )
		{
			if ( mReceivedInA && mReceivedInB )
			{
				CUtilByteArray & aux1 = mMappedAddressA->getAddressBytes();
				CUtilByteArray & aux2 = mMappedAddressB->getAddressBytes();

				if ( mMappedAddressA->getPort() == sourcePort )
				{
					if ( mMappedAddressA->getPort() == mMappedAddressB->getPort() )
					{
						PortOverloading = true;
					}
					else
					{
						PortOverloading = false;
					}
				}
				else
				{
					PortPreservation = false;
					PortOverloading = false;
				}
			}
			else if ( mReceivedInA )
			{
				PortOverloading = false;

				CUtilByteArray & aux1 = mMappedAddressA->getAddressBytes();

				if ( mMappedAddressA->getPort() == sourcePort )
					PortPreservation = true;
				else
					PortPreservation = false;
			}
			else
			{
				PortOverloading = false;
				PortPreservation = false;

			}
		}

		delete mMappedAddressA;
		delete mMappedAddressB;
	}

	mFromClientA = "";
	mFromClientB = "";
	mToServer = "";

	if ( PortPreservation )
	{	
		mTrace->Trace("**ERROR**", __FUNCTION__, "NAT with Port Preservation.");
		report += "NAT with Port Preservation.";
	}
	else
	{
		mTrace->Trace("**ERROR**", __FUNCTION__, "NAT without Port Preservation.");
		report += "NAT without Port Preservation.";
	}

	if ( PortOverloading )
	{
		mTrace->Trace("**ERROR**", __FUNCTION__, " NAT with Port Overloading.");
		report += "NAT with Port Overloading.";
	}
	else
	{
		mTrace->Trace("**ERROR**", __FUNCTION__, " NAT without Port Overloading.");
		report += "NAT without Port Overloading.";
	}

	mTrace->Trace("**DEBUG**", __FUNCTION__ , "Terminating port overloading test...\n");

	return ret;
}

void* CNATReq3::staticThreadClientAProc(void* arg)
{
	CNATReq3 * This = (CNATReq3*)arg;
	return This->realThreadClientAProc(NULL);
}

void* CNATReq3::realThreadClientAProc(void* arg)
{	
	CUtilConfigFile ConfigClient("STUNClients.config", mTrace);	
	CUtilConfigFile ConfigServer((CUtilString)"StunServer" + mToServer + ".config", mTrace);	

	CUtilString		sourceAddress = ConfigClient.GetString("STUNClient", (CUtilString)"Client" + mFromClientA + "IP", "192.168.0.14");
	word			sourcePort = (word)ConfigClient.GetLong("STUNClient", (CUtilString)"Client" + mFromClientA + "Port", 10000);
	CUtilString		device = ConfigClient.GetString("STUNClient", (CUtilString)"Client" + mFromClientA + "Dev", "en0");

	CUtilString		addressSTUNServer = ConfigServer.GetString("STUNServer", "ServerAIP", "69.0.208.27");
	word			portSTUNServer = (word)ConfigServer.GetLong("STUNServer", "ServerPort1", 3478); 
    
	CSTUNClientTransaction STUNTransaction(mTrace);

	if ( STUNTransaction.Create(sourceAddress, sourcePort, device, ConfigServer) )
	{
		CUtilByteArray	msgToSend;
		CUtilByteArray	* msgReceived;
		
		CSTUNRequest &	STUNRequest = CSTUNMsgFactory::createBindingRequest(mTrace, false, false);

		CSTUNAttribute	mappedAddress(mTrace);

		STUNRequest.encode(msgToSend);
		STUNRequest.Trace();
		
		if ( mAdviseSndFunction!= NULL )
			(*mAdviseSndFunction)(mIndexA, (const char *)sourceAddress, sourcePort, (const char *)addressSTUNServer, portSTUNServer, msgToSend, mObject);

		if ( STUNTransaction.SendAndReceive(msgToSend, &msgReceived, (const char *)sourceAddress, sourcePort, (const char *)addressSTUNServer, portSTUNServer) )
		{
			if ( mAdviseRcvFunction != NULL )
				(*mAdviseRcvFunction)(mIndexA, (const char *)addressSTUNServer, portSTUNServer, (const char *)sourceAddress, sourcePort, msgReceived, mObject);

			CSTUNResponse & STUNResponse = (CSTUNResponse&)CSTUNMsg::decode(mTrace, msgReceived, 0);
			STUNResponse.Trace();

			if ( STUNResponse.getAttribute(CSTUNAttribute::MAPPED_ADDRESS, mappedAddress) )
			{
				mReceivedInA = true;
				*mMappedAddressA = mappedAddress;
			}

			delete msgReceived;
			delete & STUNResponse;
		}

		delete & STUNRequest;

		STUNTransaction.Terminate();
	}
	
	return 0;
}

void* CNATReq3::staticThreadClientBProc(void* arg)
{
	CNATReq3 * This = (CNATReq3*)arg;
	return This->realThreadClientBProc(NULL);
}


void* CNATReq3::realThreadClientBProc(void* arg)
{	
	CUtilConfigFile ConfigClient("STUNClients.config", mTrace);	
	CUtilConfigFile ConfigServer((CUtilString)"StunServer" + mToServer + ".config", mTrace);	

	CUtilString		sourceAddress = ConfigClient.GetString("STUNClient", (CUtilString)"Client" + mFromClientB + "IP", "192.168.0.14");
	word			sourcePort = (word)ConfigClient.GetLong("STUNClient", (CUtilString)"Client" + mFromClientB + "Port", 10000);
	CUtilString		device = ConfigClient.GetString("STUNClient", (CUtilString)"Client" + mFromClientA + "Dev", "en0");

	CUtilString		addressSTUNServer = ConfigServer.GetString("STUNServer", "ServerAIP", "69.0.208.27");
	word			portSTUNServer = (word)ConfigServer.GetLong("STUNServer", "ServerPort1", 3478); 

	CSTUNClientTransaction STUNTransaction(mTrace);
	
	if ( STUNTransaction.Create(sourceAddress , sourcePort, device, ConfigServer) )
	{
		CUtilByteArray	msgToSend;
		CUtilByteArray	* msgReceived;
		
		CSTUNRequest &	STUNRequest = CSTUNMsgFactory::createBindingRequest(mTrace, false, false);

		CSTUNAttribute	mappedAddress(mTrace);

		STUNRequest.encode(msgToSend);
		STUNRequest.Trace();
		
		if ( mAdviseSndFunction!= NULL )
			(*mAdviseSndFunction)(mIndexB, (const char *)sourceAddress, sourcePort, (const char *)addressSTUNServer, portSTUNServer, msgToSend, mObject);

		if ( STUNTransaction.SendAndReceive(msgToSend, &msgReceived, (const char *)sourceAddress, sourcePort, (const char *)addressSTUNServer, portSTUNServer) )
		{
			if ( mAdviseRcvFunction != NULL )
				(*mAdviseRcvFunction)(mIndexB, (const char *)addressSTUNServer, portSTUNServer, (const char *)sourceAddress, sourcePort, msgReceived, mObject);

			CSTUNResponse & STUNResponse = (CSTUNResponse&)CSTUNMsg::decode(mTrace, msgReceived, 0);
			STUNResponse.Trace();

			if ( STUNResponse.getAttribute(CSTUNAttribute::MAPPED_ADDRESS, mappedAddress) )
			{
				mReceivedInB = true;
				*mMappedAddressB = mappedAddress;
			}

			delete msgReceived;
			delete & STUNResponse;
		}

		delete & STUNRequest;

		STUNTransaction.Terminate();
	}
	
	return 0;
}

