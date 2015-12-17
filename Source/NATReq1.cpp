/*
 *  NATReq1.cpp
 *  NATDetector
 *
 *  Created by System Administrator on 30/12/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "NATReqDefs.h"

CNATReq1::CNATReq1(CUtilTrace * Trace)
{
	mTrace = Trace;
	mAdviseSndFunction = NULL;
	mAdviseRcvFunction = NULL;
	mIndex = -1;
	mObject = NULL;
}

CNATReq1::CNATReq1(CUtilTrace * Trace, int index, AdviseSndFunction adviseSndF, AdviseRcvFunction adviseRcvF, void * object )
{
	mTrace = Trace;
	mAdviseSndFunction = adviseSndF;
	mAdviseRcvFunction = adviseRcvF;
	mIndex = index;
	mObject = object;
}

CNATReq1::~CNATReq1()
{
}

bool CNATReq1::Test(CUtilString & fromClient, CUtilString & toServer, CUtilString & report)
{
	bool ret = true;
	
	CUtilConfigFile ConfigClient("STUNClients.config", mTrace);	
	CUtilConfigFile ConfigServer((CUtilString)"StunServer" + toServer + ".config", mTrace);	

	CUtilString		sourceAddress = ConfigClient.GetString("STUNClient", (CUtilString)"Client" + fromClient + "IP", "192.168.0.14");
	word			sourcePort = (word)ConfigClient.GetLong("STUNClient", (CUtilString)"Client" + fromClient + "Port", 10000);
	CUtilString		device = ConfigClient.GetString("STUNClient", (CUtilString)"Client" + fromClient + "Dev", "en0");

	CUtilString		addressSTUNServer = ConfigServer.GetString("STUNServer", "ServerAIP", "69.0.208.27");
	word			portSTUNServer = (word)ConfigServer.GetLong("STUNServer", "ServerPort1", 3478); 

	mTrace->Trace("**DEBUG**", __FUNCTION__ , "Starting mapping behavior test...");

	CSTUNClientTransaction STUNTransaction(mTrace);
	
	if ( STUNTransaction.Create(sourceAddress, sourcePort, device, ConfigServer) )
	{
		CSTUNTransactionID	TransactionID;
		CSTUNAddress		STUNAddress;
		
		// Mapping Behavior
		bool			nextTest = true;
		CUtilByteArray	msgToSend;
		CUtilByteArray	* msgReceived;
		
		// Test I
		CSTUNRequest & STUNRequestTestI = CSTUNMsgFactory::createBindingRequest(mTrace, false, false);

		CSTUNAttribute	changedAddressTestI(mTrace);
		CSTUNAttribute	xorMappedAddressTestI(mTrace);
		CSTUNAddress	xorMappedAddressCalculatedTestI;
		CSTUNAttribute	mappedAddressTestI(mTrace);
		
		CSTUNTransactionID transactionIDTestI;

		STUNRequestTestI.encode(msgToSend);
		STUNRequestTestI.Trace();
		
		if ( mAdviseSndFunction!= NULL )
			(*mAdviseSndFunction)(mIndex, (const char *)sourceAddress, sourcePort, (const char *)addressSTUNServer, portSTUNServer, msgToSend, mObject);

		if ( STUNTransaction.SendAndReceive(msgToSend, &msgReceived, (const char *)sourceAddress, sourcePort, (const char *)addressSTUNServer, portSTUNServer) )
		{
			if ( mAdviseRcvFunction != NULL )
				(*mAdviseRcvFunction)(mIndex, (const char *)addressSTUNServer, portSTUNServer, (const char *)sourceAddress, sourcePort, msgReceived, mObject);

			CSTUNResponse & STUNResponseTestI = (CSTUNResponse&)CSTUNMsg::decode(mTrace, msgReceived, 0);
			STUNResponseTestI.Trace();

			transactionIDTestI = STUNResponseTestI.getTransactionID();

			if ( STUNResponseTestI.getAttribute(CSTUNAttribute::CHANGED_ADDRESS, changedAddressTestI) )
			{
				if ( STUNResponseTestI.getAttribute(CSTUNAttribute::XOR_MAPPED_ADDRESS, xorMappedAddressTestI) )
				{
					if ( STUNResponseTestI.getAttribute(CSTUNAttribute::MAPPED_ADDRESS, mappedAddressTestI) )
					{
						if ( mappedAddressTestI.getAddress().ToString() == sourceAddress && mappedAddressTestI.getPort() == sourcePort )
						{
							mTrace->Trace("**INFO **", __FUNCTION__, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>   NOT NATed - Endpoint independent\n");
							report += "NOT NATed - Endpoint independent.";
							nextTest = false;
						}
					}
				}
				else
				{
					nextTest = false;
				}						
			}
			else
			{
				nextTest = false;
			}
			
			delete msgReceived;

			delete & STUNResponseTestI;
		}
		else
			nextTest = false;
		
		// Test II
		CSTUNRequest &	STUNRequestTestII = CSTUNMsgFactory::createBindingRequest(mTrace, false, false);

		CSTUNAttribute	mappedAddressTestII(mTrace);
		CSTUNAttribute	xorMappedAddressTestII(mTrace);
		CSTUNAttribute	xorMappedAddressCalculatedTestII(mTrace);

		CSTUNTransactionID transactionIDTestII;

		if ( nextTest )
		{
			nextTest = false;
			STUNRequestTestII.encode(msgToSend);
			STUNRequestTestII.Trace();

			if ( mAdviseSndFunction!= NULL )
				(*mAdviseSndFunction)(mIndex, (const char *)sourceAddress, sourcePort, (const char*)changedAddressTestI.getAddress().ToString(), portSTUNServer, msgToSend, mObject);

			if ( STUNTransaction.SendAndReceive(msgToSend, &msgReceived, (const char *)sourceAddress, sourcePort, (const char*)changedAddressTestI.getAddress().ToString(), portSTUNServer) )
			{
				if ( mAdviseRcvFunction != NULL )
					(*mAdviseRcvFunction)(mIndex, (const char*)changedAddressTestI.getAddress().ToString(), portSTUNServer, (const char *)sourceAddress, sourcePort, msgReceived, mObject);

				CSTUNResponse & STUNResponseTestII = (CSTUNResponse&)CSTUNMsg::decode(mTrace, msgReceived, 0);
				STUNResponseTestII.Trace();

				transactionIDTestII = STUNResponseTestII.getTransactionID();

				if ( STUNResponseTestII.getAttribute(CSTUNAttribute::XOR_MAPPED_ADDRESS, xorMappedAddressTestII) )
				{
					if ( STUNResponseTestII.getAttribute(CSTUNAttribute::MAPPED_ADDRESS, mappedAddressTestII) )
					{
						if ( CSTUNAttribute::applyXor(xorMappedAddressTestII.getAddress(), STUNResponseTestII.getTransactionID()).equals(CSTUNAttribute::applyXor(xorMappedAddressTestI.getAddress(), transactionIDTestI)) )
						{
							mTrace->Trace("**INFO **", __FUNCTION__, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>   Endpoint independent mapping");
							report += "Endpoint independent mapping.";
							nextTest = false;
						}
						else
							nextTest = true;
					}						
				}
				else
				{
					nextTest = false;
				}	
				
				delete msgReceived;

				delete & STUNResponseTestII;
			}
			else
				nextTest = false;
		}
		
		// Test III
		CSTUNRequest &	STUNRequestTestIII = CSTUNMsgFactory::createBindingRequest(mTrace, false, false);
		CSTUNResponse	STUNResponseTestIII(mTrace);

		CSTUNAttribute	mappedAddressTestIII(mTrace);
		CSTUNAttribute	xorMappedAddressTestIII(mTrace);
		CSTUNAttribute	xorMappedAddressCalculatedTestIII(mTrace);

		if ( nextTest )
		{
			nextTest = false;
			STUNRequestTestIII.encode(msgToSend);
			STUNRequestTestIII.Trace();

			if ( mAdviseSndFunction!= NULL )
				(*mAdviseSndFunction)(mIndex, (const char *)sourceAddress, sourcePort, (const char*)changedAddressTestI.getAddress().ToString(), changedAddressTestI.getPort(), msgToSend, mObject);

			if ( STUNTransaction.SendAndReceive(msgToSend, &msgReceived, (const char *)sourceAddress, sourcePort, (const char*)changedAddressTestI.getAddress().ToString(), changedAddressTestI.getPort()) )
			{
				if ( mAdviseRcvFunction != NULL )
					(*mAdviseRcvFunction)(mIndex, (const char*)changedAddressTestI.getAddress().ToString(), changedAddressTestI.getPort(), (const char *)sourceAddress, sourcePort, msgReceived, mObject);

				CSTUNResponse & STUNResponseTestIII = (CSTUNResponse&)CSTUNMsg::decode(mTrace, msgReceived, 0);
				STUNResponseTestIII.Trace();

				if ( STUNResponseTestIII.getAttribute(CSTUNAttribute::XOR_MAPPED_ADDRESS, xorMappedAddressTestIII) )
				{
					if ( STUNResponseTestIII.getAttribute(CSTUNAttribute::MAPPED_ADDRESS, mappedAddressTestIII) )
					{
						CSTUNAddress addrAux1 = CSTUNAttribute::applyXor(xorMappedAddressTestIII.getAddress(), STUNResponseTestIII.getTransactionID());
						CSTUNAddress addrAux2 = CSTUNAttribute::applyXor(xorMappedAddressTestII.getAddress(), transactionIDTestII);

						if ( addrAux1 == addrAux2 )
						{
							mTrace->Trace("**INFO **", __FUNCTION__, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>   Address dependent mapping");
							report += "Address dependent mapping.";
							nextTest = false;
						}
						else
						{
							mTrace->Trace("**INFO **", __FUNCTION__, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>   Address and Port dependent mapping");
							report += "Address and Port dependent mapping.";
						}	
					}
				}
				
				delete msgReceived;

				delete & STUNResponseTestIII;
			}					
		}			
		
		delete &STUNRequestTestI;
		delete &STUNRequestTestII;
		delete &STUNRequestTestIII;

		STUNTransaction.Terminate();
	}

	mTrace->Trace("**INFO **", __FUNCTION__, "Terminating mapping behavior test...\n");	

	return ret;
}
