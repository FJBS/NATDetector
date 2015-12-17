/*
 *  NATReq8.cpp
 *  NATDetector
 *
 *  Created by System Administrator on 30/12/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "NATReqDefs.h"

CNATReq8::CNATReq8(CUtilTrace * Trace)
{
	mTrace = Trace;
	mAdviseSndFunction = NULL;
	mAdviseRcvFunction = NULL;
	mIndex = -1;
	mObject = NULL;
}

CNATReq8::CNATReq8(CUtilTrace * Trace, int index, AdviseSndFunction adviseSndF, AdviseRcvFunction adviseRcvF, void * object)
{
	mTrace = Trace;
	mAdviseSndFunction = adviseSndF;
	mAdviseRcvFunction = adviseRcvF;
	mIndex = index;
	mObject = object;
}

CNATReq8::~CNATReq8()
{
}

bool CNATReq8::Test(CUtilString & fromClient, CUtilString & toServer, CUtilString & report)
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
	mTrace->Trace("**INFO **", __FUNCTION__, "Starting NAT Filtering behavior test...");

	CSTUNClientTransaction STUNTransaction(mTrace);
	
	if ( STUNTransaction.Create(sourceAddress, sourcePort, device, ConfigServer) )
	{
		// Mapping Behavior
		bool			nextTest = true;
		CUtilByteArray	msgToSend;
		CUtilByteArray  *msgReceived;		
		
		// Test I
		CSTUNRequest &	STUNRequestTestI = CSTUNMsgFactory::createBindingRequest(mTrace, false, false);
		CSTUNAttribute	changedAddressTestI(mTrace);

		if ( STUNRequestTestI.encode(msgToSend) )
		{
			STUNRequestTestI.Trace();

			if ( mAdviseSndFunction!= NULL )
				(*mAdviseSndFunction)(mIndex, (const char *)sourceAddress, sourcePort, (const char *)addressSTUNServer, portSTUNServer, msgToSend, mObject);

			if ( STUNTransaction.SendAndReceive(msgToSend, &msgReceived, (const char *)sourceAddress, sourcePort, (const char *)addressSTUNServer, portSTUNServer) )
			{
				CSTUNResponse &	STUNResponseTestI = (CSTUNResponse&)CSTUNMsg::decode(mTrace, msgReceived, 0);
				STUNResponseTestI.Trace();
				
				if ( mAdviseRcvFunction != NULL )
					(*mAdviseRcvFunction)(mIndex, (const char *)addressSTUNServer, portSTUNServer, (const char *)sourceAddress, sourcePort, msgReceived, mObject);

				if ( !(STUNResponseTestI.getAttribute(CSTUNAttribute::CHANGED_ADDRESS, changedAddressTestI)) )
				{
					mTrace->Trace("**INFO **", __FUNCTION__, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>   Server does not support this usage and this test cannot be run.");
					report += "Server does not support this usage and this test cannot be run.";
					nextTest = false;
				}
				
				delete msgReceived;
				delete & STUNResponseTestI;
			}
		}
		else 
		{
			mTrace->Trace("**ERROR**", __FUNCTION__, "Error encoding message for test I.");
			report += "Error encoding message for test I.";
		}

		delete & STUNRequestTestI;
			
		// Test II
		CSTUNRequest & STUNRequestTestII = CSTUNMsgFactory::createBindingRequest(mTrace, true, true);

		if ( nextTest )
		{
			nextTest = false;
			if ( STUNRequestTestII.encode(msgToSend) )
			{
				STUNRequestTestII.Trace();

				if ( mAdviseSndFunction!= NULL )
					(*mAdviseSndFunction)(mIndex, (const char *)sourceAddress, sourcePort, (const char *)addressSTUNServer, portSTUNServer, msgToSend, mObject);

				if ( STUNTransaction.SendAndReceive(msgToSend, &msgReceived, (const char *)sourceAddress, sourcePort, (const char*)addressSTUNServer, portSTUNServer) )
				{
					CSTUNResponse &	STUNResponseTestII = (CSTUNResponse&)CSTUNMsg::decode(mTrace, msgReceived, 0);
					STUNResponseTestII.Trace();
					
					if ( mAdviseRcvFunction != NULL )
						(*mAdviseRcvFunction)(mIndex, (const char *)changedAddressTestI.getAddress().ToString(), changedAddressTestI.getPort(), (const char *)sourceAddress, sourcePort, msgReceived, mObject);

					mTrace->Trace("**INFO **", __FUNCTION__, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>   Address Independent Filtering.");
					report += "Address Independent Filtering.";

					delete msgReceived;
					delete & STUNResponseTestII;
				}
				else
					nextTest = true;
			}
			else 
			{
				mTrace->Trace("**ERROR**", __FUNCTION__, "Error encoding message for test II.");
				report += "Error encoding message for test II.";
			}
		}
		
		delete & STUNRequestTestII;

		// Test III
		CSTUNRequest &	STUNRequestTestIII = CSTUNMsgFactory::createBindingRequest(mTrace, false, true);

		if ( nextTest )
		{
			nextTest = false;
			if ( STUNRequestTestIII.encode(msgToSend) )
			{
				STUNRequestTestIII.Trace();
				
				if ( mAdviseSndFunction!= NULL )
					(*mAdviseSndFunction)(mIndex, (const char *)sourceAddress, sourcePort, (const char *)addressSTUNServer, portSTUNServer, msgToSend, mObject);

				if ( STUNTransaction.SendAndReceive(msgToSend, &msgReceived, (const char *)sourceAddress, sourcePort, (const char *)addressSTUNServer, portSTUNServer) )
				{

					CSTUNResponse &	STUNResponseTestIII = (CSTUNResponse&)CSTUNMsg::decode(mTrace, msgReceived, 0);
					STUNResponseTestIII.Trace();

					if ( mAdviseRcvFunction != NULL )
						(*mAdviseRcvFunction)(mIndex, (const char *)addressSTUNServer, changedAddressTestI.getPort(), (const char *)sourceAddress, sourcePort, msgReceived, mObject);

					mTrace->Trace("**INFO **", __FUNCTION__, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>   Address Dependent Filtering.");
					report += "Address Dependent Filtering.";

					delete msgReceived;
					delete & STUNResponseTestIII;
				}	
				else
				{
					mTrace->Trace("**INFO **", __FUNCTION__, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>   Address and Port Dependent Filtering.");
					report += "Address and Port Dependent Filtering.";
				}	
			}			
			else 
			{
				mTrace->Trace("**ERROR**", __FUNCTION__, "Error encoding message for test III.");
				report += "Error encoding message for test III.";
			}
		}	
		
		delete & STUNRequestTestIII;

		STUNTransaction.Terminate();
	}

	mTrace->Trace("**INFO **", __FUNCTION__, "Terminating NAT Filtering behavior test...\n");	

	return ret;
}
