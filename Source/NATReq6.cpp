/*
 *  NATReq3.cpp
 *  NATDetector
 *
 *  Created by System Administrator on 30/12/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "NATReqDefs.h"

CNATReq6::CNATReq6(CUtilTrace * Trace)
{
	mTrace = Trace;
	mAdviseSndFunction = NULL;
	mAdviseRcvFunction = NULL;
	mIndex = -1;
	mObject = NULL;
}

CNATReq6::CNATReq6(CUtilTrace * Trace, int index, AdviseSndFunction adviseSndF, AdviseRcvFunction adviseRcvF, void * object )
{
	mTrace = Trace;
	mAdviseSndFunction = adviseSndF;
	mAdviseRcvFunction = adviseRcvF;
	mIndex = index;
	mObject = object;
}

CNATReq6::~CNATReq6()
{
}

bool CNATReq6::Test(CUtilString & fromClient, CUtilString & toServer, unsigned int milisecs, CUtilString & report)
{
	bool ret = true;
	
	
	CUtilConfigFile ConfigClient("STUNClients.config", mTrace);	
	CUtilConfigFile ConfigServer((CUtilString)"StunServer" + toServer + ".config", mTrace);	

	CUtilString		sourceAddress = ConfigClient.GetString("STUNClient", (CUtilString)"Client" + fromClient + "IP", "192.168.0.14");
	word			sourcePort = (word)ConfigClient.GetLong("STUNClient", (CUtilString)"Client" + fromClient + "Port", 10000);
	CUtilString		device = ConfigClient.GetString("STUNClient", (CUtilString)"Client" + fromClient + "Dev", "en0");

	CUtilString		addressSTUNServer = ConfigServer.GetString("STUNServer", "ServerAIP", "69.0.208.27");
	word			portSTUNServer = (word)ConfigServer.GetLong("STUNServer", "ServerPort1", 3478); 

	report = "";
	mTrace->Trace("DEBUG**", __FUNCTION__, "Starting binding lifetime discovery test for %d miliseconds...", milisecs);

	CSTUNClientTransaction STUNTransactionP(mTrace);
	
	bool			nextTest = true;
	CUtilByteArray	msgToSend;
	CUtilByteArray	*msgReceived;
	
	// Test I
	CSTUNRequest &	STUNRequestTestI = CSTUNMsgFactory::createBindingRequest(mTrace, false, false);

	CSTUNAttribute	mappedAddressTestI(mTrace);
	
	if ( STUNTransactionP.Create(sourceAddress, sourcePort, device, ConfigServer) )
	{
		STUNRequestTestI.encode(msgToSend);
		STUNRequestTestI.Trace();

		if ( mAdviseSndFunction!= NULL )
			(*mAdviseSndFunction)(mIndex, (const char *)sourceAddress, sourcePort, (const char *)addressSTUNServer, portSTUNServer, msgToSend, mObject);

		if ( STUNTransactionP.SendAndReceive(msgToSend, &msgReceived, (const char *)sourceAddress, sourcePort, (const char *)addressSTUNServer, portSTUNServer) )
		{
			if ( mAdviseRcvFunction != NULL )
				(*mAdviseRcvFunction)(mIndex, (const char *)addressSTUNServer, portSTUNServer, (const char *)sourceAddress, sourcePort, msgReceived, mObject);

			CSTUNResponse & STUNResponseTestI = (CSTUNResponse&)CSTUNMsg::decode(mTrace, msgReceived, 0);
			STUNResponseTestI.Trace();
			
			if ( !STUNResponseTestI.getAttribute(CSTUNAttribute::MAPPED_ADDRESS, mappedAddressTestI) )
			{
				// server does not support this usage and this test cannot be run
				nextTest = false;
			}
			
			delete msgReceived;
			delete & STUNResponseTestI;
		}
		else
			nextTest = false;
		
		if ( nextTest )
		{
#ifndef WIN32
			// TODO: see if return is 0, otherwise sleep "return" seconds.
			sleep(milisecs/1000);
#else
			Sleep(milisecs);
#endif
			
			// Change sourcePort
			sourcePort += 1;
			
			// Test II
			CSTUNClientTransaction STUNTransactionS(mTrace);

			CSTUNRequest & STUNRequestTestII = CSTUNMsgFactory::createBindingRequest(mTrace, false, false);
			CSTUNAttribute & STUNResponseAddressAttribute = CSTUNAttributeFactory::createResponseAddressAttribute(mTrace, mappedAddressTestI.getAddress());

			// TODO: Probe new attribute XOR-RESPONSE-TARGET
			STUNRequestTestII.addAttribute(STUNResponseAddressAttribute);
			
			if ( STUNTransactionS.Create(sourceAddress, sourcePort, device, ConfigServer) )
			{		
				if ( nextTest )
				{
					nextTest = false;
					STUNRequestTestII.encode(msgToSend);
					STUNRequestTestII.Trace();

					mSTUNTransactionP	= &STUNTransactionP;
					mSTUNTransactionS	= &STUNTransactionS;
					mMsgToSendS			= &msgToSend;

					mSourceAddress		= &sourceAddress;
					mSourcePort			= &sourcePort;

					mAddressSTUNServer	= &addressSTUNServer;
					mPortSTUNServer		= &portSTUNServer;

					mReceivedInA		= false;
					mReceivedInB		= false;

					if ( pthread_create(&mThreadClientA, 0, staticThreadClientAProc, this) != 0 )
					{
						mTrace->Trace("**ERROR**", __FUNCTION__, "Failed pthread_create for client A.");
						report += "Failed pthread_create for client A.";
						ret = false;
					}

					// Wait 3 sec to send the request
#ifndef WIN32
					sleep(3);
#else
					Sleep(3000);
#endif
					if ( pthread_create(&mThreadClientB, 0, staticThreadClientBProc, this) != 0 )
					{
						mTrace->Trace("**ERROR**", __FUNCTION__, "Failed pthread_create for client B.");
						report += "Failed pthread_create for client B.";
						ret = false;
					}

					pthread_join(mThreadClientA, 0);
					pthread_join(mThreadClientB, 0);

					if ( mReceivedInA )
					{
						mTrace->Trace("**DEBUG**", __FUNCTION__, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>   Timeout has expired.");
						report += "Timeout has expired.";
					}
					else if ( mReceivedInB )
					{
						mTrace->Trace("**DEBUG**", __FUNCTION__, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>   Timeout has NOT expired.");
						report += "Timeout has not expired.";					}
					else
					{
						mTrace->Trace("**DEBUG**", __FUNCTION__, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>   Timeout has expired.");
						report += "Timeout has expired.";					}
				}
				
				STUNTransactionS.Terminate();
			}
			
			delete & STUNResponseAddressAttribute;
			delete & STUNRequestTestII;

			STUNTransactionP.Terminate();
		}
	}
	
	delete & STUNRequestTestI;

	mTrace->Trace("**DEBUG**", __FUNCTION__, "Terminating binding lifetime discovery test...\n");	

	return ret;
}


void* CNATReq6::staticThreadClientAProc(void* arg)
{
	CNATReq6 * This = (CNATReq6*)arg;
	return This->realThreadClientAProc(NULL);
}

void* CNATReq6::realThreadClientAProc(void* arg)
{	
	CUtilByteArray	*msgReceived;

	if ( mAdviseSndFunction!= NULL )
		(*mAdviseSndFunction)(mIndex, (const char *)(const char *)(*mSourceAddress), *mSourcePort, (const char *)(*mAddressSTUNServer), *mPortSTUNServer, mMsgToSendS, mObject);

	if ( mSTUNTransactionS->SendAndReceive(*mMsgToSendS, &msgReceived, (const char *)(*mSourceAddress), *mSourcePort, (const char*)(*mAddressSTUNServer), *mPortSTUNServer) )
	{
		if ( mAdviseRcvFunction != NULL )
			(*mAdviseRcvFunction)(mIndex, (const char*)(*mAddressSTUNServer), *mPortSTUNServer, (const char *)(*mSourceAddress), *mSourcePort, msgReceived, mObject);

		CSTUNResponse & STUNResponse = (CSTUNResponse&)CSTUNMsg::decode(mTrace, msgReceived, 0);
		STUNResponse.Trace();
		
		delete msgReceived;	
		delete & STUNResponse;

		mReceivedInA = true;
	}
	
	return 0;
}

void* CNATReq6::staticThreadClientBProc(void* arg)
{
	CNATReq6 * This = (CNATReq6*)arg;
	return This->realThreadClientBProc(NULL);
}


void* CNATReq6::realThreadClientBProc(void* arg)
{	
	CUtilByteArray	*msgReceived;

	if ( mSTUNTransactionP->Receive(&msgReceived) )
	{
		if ( mAdviseRcvFunction != NULL )
			(*mAdviseRcvFunction)(mIndex, (const char*)(*mAddressSTUNServer), *mPortSTUNServer, (const char *)(*mSourceAddress), (*mSourcePort)-1, msgReceived, mObject);

		CSTUNResponse & STUNResponse = (CSTUNResponse&)CSTUNMsg::decode(mTrace, msgReceived, 0);
		STUNResponse.Trace();

		delete msgReceived;	
		delete & STUNResponse;

		mReceivedInB = true;
	}
	
	return 0;
}

