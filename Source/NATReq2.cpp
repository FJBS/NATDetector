/*
 *  NATReq2.cpp
 *  NATDetector
 *
 *  Created by System Administrator on 30/12/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "NATReqDefs.h"

CNATReq2::CNATReq2(CUtilTrace * Trace)
{
	mTrace = Trace;
	mAdviseSndFunction = NULL;
	mAdviseRcvFunction = NULL;
	mIndex = -1;
	mObject = NULL;
}

CNATReq2::CNATReq2(CUtilTrace * Trace, int index, AdviseSndFunction adviseSndF, AdviseRcvFunction adviseRcvF, void * object )
{
	mTrace = Trace;
	mAdviseSndFunction = adviseSndF;
	mAdviseRcvFunction = adviseRcvF;
	mIndex = index;
	mObject = object;
}

CNATReq2::~CNATReq2()
{
}

bool CNATReq2::Test(CUtilString & fromClient, CUtilString & toServer, word numMessages, CUtilString & report)
{
	bool ret = true;
	word count = 0;

	bool randomIPAddress = false;

	CUtilConfigFile ConfigClient("STUNClients.config", mTrace);	
	CUtilConfigFile ConfigServer((CUtilString)"StunServer" + toServer + ".config", mTrace);	

	CUtilString		sourceAddress = ConfigClient.GetString("STUNClient", (CUtilString)"Client" + fromClient + "IP", "192.168.0.14");
	word			sourcePort = (word)ConfigClient.GetLong("STUNClient", (CUtilString)"Client" + fromClient + "Port", 10000);
	CUtilString		device = ConfigClient.GetString("STUNClient", (CUtilString)"Client" + fromClient + "Dev", "en0");

	CUtilString		addressSTUNServer = ConfigServer.GetString("STUNServer", "ServerAIP", "69.0.208.27");
	word			portSTUNServer = (word)ConfigServer.GetLong("STUNServer", "ServerPort1", 3478); 

	mTrace->Trace("**DEBUG**", __FUNCTION__ , "Starting IP address polling behavior test...");

	CSTUNClientTransaction STUNTransaction(mTrace);

	CSTUNAttribute		MappedAddress(mTrace);
	CSTUNAttribute		MappedAddressAnt(mTrace);

	while ( count < numMessages )
	{
		if ( STUNTransaction.Create(sourceAddress, sourcePort+count, device, ConfigServer) )
		{
			CUtilByteArray	msgToSend;
			CUtilByteArray	* msgReceived;

			CSTUNRequest &	STUNRequest = CSTUNMsgFactory::createBindingRequest(mTrace, false, false);

			STUNRequest.encode(msgToSend);
			STUNRequest.Trace();
			
			if ( mAdviseSndFunction!= NULL )
				(*mAdviseSndFunction)(mIndex, (const char *)sourceAddress, sourcePort+count, (const char *)addressSTUNServer, portSTUNServer, msgToSend, mObject);

			if ( STUNTransaction.SendAndReceive(msgToSend, &msgReceived, (const char *)sourceAddress, sourcePort+count, (const char *)addressSTUNServer, portSTUNServer) )
			{
				if ( mAdviseRcvFunction != NULL )
					(*mAdviseRcvFunction)(mIndex, (const char *)addressSTUNServer, portSTUNServer, (const char *)sourceAddress, sourcePort+count, msgReceived, mObject);

				CSTUNResponse & STUNResponse = (CSTUNResponse&)CSTUNMsg::decode(mTrace, msgReceived, 0);
				STUNResponse.Trace();

				if ( count == 0 )
				{
					STUNResponse.getAttribute(CSTUNAttribute::MAPPED_ADDRESS, MappedAddressAnt);
				}
				else
				{
					STUNResponse.getAttribute(CSTUNAttribute::MAPPED_ADDRESS, MappedAddress);

					CUtilByteArray & aux1 = MappedAddressAnt.getAddress().getAddressBytes();
					CUtilByteArray & aux2 = MappedAddress.getAddress().getAddressBytes();
					if ( aux1 != aux2 )
					{
						randomIPAddress = true;
						break;
					}
					delete & aux1;
					delete & aux2;
				}

				delete msgReceived;
				delete & STUNResponse;
			}

			STUNTransaction.Terminate();
		
			delete & STUNRequest;
		}
		count++;
	}

	if ( randomIPAddress )
	{
		mTrace->Trace("**INFO **", __FUNCTION__, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>   Random IP address polling");
		report += "Random IP address polling.";
	}
	else
	{
		mTrace->Trace("**INFO **", __FUNCTION__, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>   Paired IP address polling");
		report += "Paired IP address polling.";
	}

	mTrace->Trace("**INFO **", __FUNCTION__, "Terminating IP address polling behavior test...\n");	

	return ret;
}
