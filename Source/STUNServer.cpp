/*
 *  STUNSever.cpp
 *  NATDetector
 *
 *  Created by System Administrator on 22/01/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "STUNDefs.h"

CSTUNServer::CSTUNServer(CUtilString ConfigFile, CUtilTrace* Trace)
{
	mCriticalSection = new CUtilCriticalSection();

	mConfigFile = ConfigFile;

	mAdviseSndFunction = NULL;
	mAdviseRcvFunction = NULL;
	mIndex = -1;
	mObject = NULL;

	this->mTrace = Trace;
#ifndef WIN32
	this->mThread = NULL;
#else
	this->mThread.p = NULL;
#endif
}

CSTUNServer::CSTUNServer(CUtilString ConfigFile, CUtilTrace* Trace, int index, AdviseSndFunction adviseSndF, AdviseRcvFunction adviseRcvF, void * object)
{
	mCriticalSection = new CUtilCriticalSection();

	mConfigFile = ConfigFile;

	mAdviseSndFunction = adviseSndF;
	mAdviseRcvFunction = adviseRcvF;
	mIndex = index;
	mObject = object;

	this->mTrace = Trace;
#ifndef WIN32
	this->mThread = NULL;
#else
	this->mThread.p = NULL;
#endif
}

CSTUNServer::~CSTUNServer()
{
	Stop();
	delete mCriticalSection;
}

void* CSTUNServer::staticThreadProc(void* arg)
{
	CSTUNServer * This = (CSTUNServer*)arg;
	return This->realThreadProc(NULL);
}

void* CSTUNServer::realThreadProc(void* arg)
{	
	bool ret = true;
	
	CUtilString	serverAddrA(inet_ntoa(mServerAddressA));
	CUtilString	serverAddrB(inet_ntoa(mServerAddressB));
								
	CArray<CUtilString> serverAddresses;
	serverAddresses.Add(serverAddrA);
	serverAddresses.Add(serverAddrB);

	CCaptureApi			*CaptureReceiver;
	CaptureStruct		packet;
	CUtilByteArray		msg;

	CSTUNAttribute		changeRequest(mTrace);
	CSTUNAttribute		responseAddress(mTrace);
	
	CUtilString			packetRcvFromIP;
	dword				packetRcvFromPort;
	CUtilString			packetRcvToIP;
	dword				packetRcvToPort;

	CSTUNAddress		STUNMappedAddress;
	CSTUNAddress		STUNSourceAddress;
	CSTUNAddress		STUNChangedAddress;
		
	CUtilString			sourceAddrPacket;
	dword				sourcePortPacket;
	
	CaptureReceiver = new CCaptureApi(mTrace);
	ret = CaptureReceiver->Start(mNATPublicIPs, serverAddresses, mDevice);

	if ( ret )
	{
		CRawSocket rawSocketSender(mTrace);

		if ( rawSocketSender.CreateSender() )
		{
			while ( !mTerminate )
			{
				mCriticalSection->Enter();
				if ( CaptureReceiver->GetMsg(packet, 300) )
				{
					packetRcvFromIP = CUtilString(inet_ntoa(packet.Ip_src));
					packetRcvFromPort = packet.Sport;
					packetRcvToIP = CUtilString(inet_ntoa(packet.Ip_dst));
					packetRcvToPort = packet.Dport;
					
					STUNSourceAddress = CSTUNAddress();
					STUNChangedAddress = CSTUNAddress();
					try
					{
						if ( packet.Dport == mServerAPort || packet.Dport == mServerBPort )
						{
							CSTUNRequest & STUNRequest = (CSTUNRequest&)CSTUNMsg::decode(mTrace, packet.Payload, 0);
							STUNRequest.Trace();
							
							if ( mAdviseRcvFunction != NULL )
								(*mAdviseRcvFunction)(mIndex, (const char *)packetRcvFromIP, packetRcvFromPort, packetRcvToIP, packetRcvToPort, packet.Payload, mObject);

							
							STUNRequest.getAttribute(CSTUNAttribute::CHANGE_REQUEST, changeRequest);
							
							if ( changeRequest.getChangePortFlag() && changeRequest.getChangeIpFlag() )
							{
								if ( packetRcvToIP == serverAddrA && packetRcvToPort == mServerAPort )
								{
									packet.Ip_src.s_addr =  mServerAddressB.s_addr;
									packet.Sport = mServerBPort;	
								}
								else if ( packetRcvToIP == serverAddrA && packetRcvToPort == mServerBPort )
								{
									packet.Ip_src.s_addr =  mServerAddressB.s_addr;
									packet.Sport = mServerAPort;	
								}
								else if ( packetRcvToIP == serverAddrB && packetRcvToPort == mServerAPort )
								{
									packet.Ip_src.s_addr =  mServerAddressA.s_addr;
									packet.Sport = mServerBPort;	
								}
								else if ( packetRcvToIP == serverAddrB && packetRcvToPort == mServerBPort )
								{
									packet.Ip_src.s_addr =  mServerAddressA.s_addr;
									packet.Sport = mServerAPort;	
								}
							} 
							else if ( changeRequest.getChangeIpFlag() )
							{
								if ( packetRcvToIP == serverAddrA )
								{
									packet.Ip_src.s_addr =  mServerAddressB.s_addr;
									packet.Sport = packetRcvToPort;
								}
								else
								{
									packet.Ip_src.s_addr =  mServerAddressA.s_addr;
									packet.Sport = packetRcvToPort;
								}
							}
							else if ( changeRequest.getChangePortFlag() )
							{
								if ( packetRcvToIP == serverAddrA )
									packet.Ip_src.s_addr =  mServerAddressA.s_addr;
								else
									packet.Ip_src.s_addr =  mServerAddressB.s_addr;

								if ( packetRcvToPort == mServerAPort )
									packet.Sport = mServerBPort;
								else
									packet.Sport = mServerAPort;
							}
							else
							{
								if ( packetRcvToIP == serverAddrA )
									packet.Ip_src.s_addr =  mServerAddressA.s_addr;
								else
									packet.Ip_src.s_addr =  mServerAddressB.s_addr;

								if ( packetRcvToPort == mServerAPort )
									packet.Sport = mServerAPort;
								else
									packet.Sport = mServerBPort;
							}

							STUNMappedAddress = CSTUNAddress(packetRcvFromIP, packetRcvFromPort);

							sourceAddrPacket = inet_ntoa(packet.Ip_src);
							sourcePortPacket = packet.Sport;
							STUNSourceAddress = CSTUNAddress(sourceAddrPacket, sourcePortPacket);

							if ( sourceAddrPacket == serverAddrA && sourcePortPacket == mServerAPort )
								STUNChangedAddress = CSTUNAddress(serverAddrB, mServerBPort);

							else if ( sourceAddrPacket == serverAddrA && sourcePortPacket == mServerBPort )
								STUNChangedAddress = CSTUNAddress(serverAddrB, mServerAPort);

							else if ( sourceAddrPacket == serverAddrB && sourcePortPacket == mServerAPort )
								STUNChangedAddress = CSTUNAddress(serverAddrA, mServerBPort);

							else if ( sourceAddrPacket == serverAddrB && sourcePortPacket == mServerBPort )
								STUNChangedAddress = CSTUNAddress(serverAddrA, mServerAPort);
							
							CSTUNTransactionID TransactionID = STUNRequest.getTransactionID();
							CSTUNAddress STUNXORMappedAddress = CSTUNAttribute::applyXor(STUNMappedAddress, TransactionID);
							CSTUNResponse & STUNResponse = CSTUNMsgFactory::createBindingResponse(mTrace, STUNMappedAddress, STUNSourceAddress, STUNChangedAddress, STUNXORMappedAddress );							
							STUNResponse.setTransactionID(TransactionID);

							STUNResponse.encode(msg);
							STUNResponse.Trace();
							
							if ( STUNRequest.getAttribute(CSTUNAttribute::RESPONSE_ADDRESS, responseAddress) )
							{
								CUtilString destAddress = responseAddress.getAddress().ToString();
								rawSocketSender.Send(msg, (const char*)sourceAddrPacket, sourcePortPacket, (const char *)destAddress, responseAddress.getPort());
								if ( mAdviseSndFunction != NULL )
									(*mAdviseSndFunction)(mIndex, (const char*)sourceAddrPacket, sourcePortPacket, (const char *)destAddress, responseAddress.getPort(), msg, mObject);
							}
							else
							{
								rawSocketSender.Send(msg, (const char*)sourceAddrPacket, sourcePortPacket, (const char *)packetRcvFromIP, packetRcvFromPort);
								if ( mAdviseSndFunction != NULL )
									(*mAdviseSndFunction)(mIndex, (const char*)sourceAddrPacket, sourcePortPacket, (const char *)packetRcvFromIP, packetRcvFromPort, msg, mObject);
							}

							delete & STUNResponse;
							delete & STUNRequest;
						}
					}
					catch (...)
					{
						mTrace->Trace("**ERROR**", __FUNCTION__, "Failed proccessing STUN message received.");
					}
				}
				mCriticalSection->Leave();
			}
			
			rawSocketSender.TerminateSender();
		}
	}

	CaptureReceiver->Stop();
	delete CaptureReceiver;

	return 0;
}

bool CSTUNServer::Start()
{
	bool ret = true;
		
#ifndef WIN32
	if ( mThread != NULL )
#else
	if ( mThread.p != NULL )
#endif
	{
		mTrace->Trace("**DEBUG**", __FUNCTION__, "Server yet started.");
		ret = false;
	}
	else
	{		
		mTrace->Trace("**DEBUG**", __FUNCTION__, "Starting STUN server.");		
		
		readSTUNServerConfig();
		
		mTerminate = false;
		if ( pthread_create(&mThread, 0, staticThreadProc, this) != 0 )
		{
			mTrace->Trace("**ERROR**", __FUNCTION__, "Failed pthread_create for stun server.");
			ret = false;
		}
	}
		
	return ret;
}

void CSTUNServer::Stop()
{
	mTerminate = true;

#ifndef WIN32
	if ( mThread != NULL )
#else
	if ( mThread.p != NULL )
#endif
	{
		mTrace->Trace("**DEBUG**", __FUNCTION__, "Stoping first STUN server.");		

		pthread_join(mThread, 0);
		
#ifndef WIN32
		mThread = NULL;
#else
		mThread.p = NULL;
#endif
	}
	
	mNATPublicIPs.RemoveAll(); 
}

void CSTUNServer::readSTUNServerConfig()
{
	CUtilConfigFile Config(mConfigFile, mTrace);	

	mNATPublicIPs = Config.GetStringList("NATSim", "NATPublicIP"); 

	mServerAddressA.s_addr = inet_addr((const char *)Config.GetString("STUNServer", "ServerAIP", "69.0.209.22"));
	mServerAddressB.s_addr = inet_addr((const char *)Config.GetString("STUNServer", "ServerBIP", "69.0.208.27"));

	mServerAPort = (word)Config.GetLong("STUNServer", "ServerPort1", 3478);
	mServerBPort = (word)Config.GetLong("STUNServer", "ServerPort2", 3478);

	mDevice = Config.GetString("STUNServer", "ServerDev", "en0");
}

