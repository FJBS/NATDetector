/*
 *  NATSim.cpp
 *  NATDetector
 *
 *  Created by System Administrator on 17/02/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "NATSimDefs.h"

CNATSim::CNATSim(CUtilString ConfigFile, CUtilTrace * Trace)
{
	mConfigFile = ConfigFile;
	mTrace = Trace;

	mTerminate = false;
	
	mPublicDevice = "";

	mTimeout=60000;

#ifndef WIN32
	this->mThreadPublic = NULL;
#else
	this->mThreadPublic.p = NULL;
#endif

	mPrivateDevice = "";
#ifndef WIN32
	this->mThreadPrivate = NULL;
#else
	this->mThreadPrivate.p = NULL;
#endif

	mAdviseSndFunction = NULL;
	mAdviseRcvFunction = NULL;
	mIndex = -1;
	mObject = NULL;
}

CNATSim::CNATSim(CUtilString ConfigFile, CUtilTrace * Trace, int index, AdviseSndFunction adviseSndF, AdviseRcvFunction adviseRcvF, void * object)
{
	mConfigFile = ConfigFile;
	mTrace = Trace;

	mTerminate = false;
	
	mPublicDevice = "";

	mTimeout=60000;

#ifndef WIN32
	this->mThreadPublic = NULL;
#else
	this->mThreadPublic.p = NULL;
#endif

	mPrivateDevice = "";
#ifndef WIN32
	this->mThreadPrivate = NULL;
#else
	this->mThreadPrivate.p = NULL;
#endif

	mAdviseSndFunction = adviseSndF;
	mAdviseRcvFunction = adviseRcvF;
	mIndex = index;
	mObject = object;
}

CNATSim::~CNATSim()
{
	Stop();
}

void* CNATSim::staticThreadProcPublic(void* arg)
{
	CNATSim * This = (CNATSim*)arg;
	return This->realThreadProcPublic(NULL);
}

void* CNATSim::realThreadProcPublic(void* arg)
{	
	CCaptureApi		*Capture;
	CaptureStruct	packet;
	
	Capture = new CCaptureApi(mTrace);

	if ( Capture->Start(mPublicIPs, mNATPublicIPs, mPublicDevice) )
	{
		CRawSocket rawSocketSender(mTrace);

		if ( rawSocketSender.CreateSender() )
		{
			while ( !mTerminate )
			{				
				if ( Capture->GetMsg(packet, 300) )
				{
					TreatIncommingPacket(packet, &rawSocketSender);

					CUtilString outgoinTable = mOutgoingTable.ToString();
					CUtilString  incommingTable = mIncommingTable.ToString();

					mTrace->Trace("**INFO **", __FUNCTION__, "Outgoing Table");
					mTrace->Trace("**INFO **", __FUNCTION__, "%s", (const char *)outgoinTable);
					mTrace->Trace("**INFO **", __FUNCTION__, "Incomming Table");
					mTrace->Trace("**INFO **", __FUNCTION__, "%s", (const char *)incommingTable);
				}
	
			}
			
			rawSocketSender.TerminateSender();
		}
	}

	Capture->Stop();
	delete Capture;

	return 0;
}

void* CNATSim::staticThreadProcPrivate(void* arg)
{
	CNATSim * This = (CNATSim*)arg;
	return This->realThreadProcPrivate(NULL);
}

void* CNATSim::realThreadProcPrivate(void* arg)
{	
	CCaptureApi		*Capture;
	CaptureStruct	packet;
	
	Capture = new CCaptureApi(mTrace);

	if ( Capture->Start(mPrivateIPs, mPublicIPs, mPrivateDevice) )
	{
		CRawSocket rawSocketSender(mTrace);

		if ( rawSocketSender.CreateSender() )
		{
			while ( !mTerminate )
			{
				if ( Capture->GetMsg(packet, 300) )
				{
					TreatOutgoingPacket(packet, &rawSocketSender);

					CUtilString outgoinTable = mOutgoingTable.ToString();
					CUtilString incommingTable = mIncommingTable.ToString();

					mTrace->Trace("**INFO **", __FUNCTION__, "Outgoing Table");
					mTrace->Trace("**INFO **", __FUNCTION__, "%s", (const char *)outgoinTable);
					mTrace->Trace("**INFO **", __FUNCTION__, "Incomming Table");
					mTrace->Trace("**INFO **", __FUNCTION__, "%s", (const char *)incommingTable);
				}	
			}
			
			rawSocketSender.TerminateSender();
		}
	}

	Capture->Stop();
	delete Capture;

	return 0;
}

bool CNATSim::Start()
{
	bool	ret = true;
	
	readNatConfig();
	
#ifndef WIN32
	if ( mThreadPublic != NULL )
#else
	if ( mThreadPublic.p != NULL )
#endif
	{
		mTrace->Trace("**DEBUG**", __FUNCTION__, "Capture public traffic yet started.");
		ret = false;
	}
	else
	{
		mTrace->Trace("**INFO **", __FUNCTION__, "Starting capture of public traffic.");		
		
		mTerminate = false;
		if ( pthread_create(&mThreadPublic, 0, staticThreadProcPublic, this) != 0 )
		{
			mTrace->Trace("**ERROR**", __FUNCTION__, "Failed pthread_create for capturing public traffic.");
			ret = false;
		}
	}
	
#ifndef WIN32
	if ( mThreadPrivate != NULL )
#else
	if ( mThreadPrivate.p != NULL )
#endif
	{
		mTrace->Trace("**DEBUG**", __FUNCTION__, "Capture private traffic yet started.");
		ret = false;
	}
	else
	{
		mTrace->Trace("**INFO **", __FUNCTION__, "Starting capture of private traffic.");		
		
		mTerminate = false;
		if ( pthread_create(&mThreadPrivate, 0, staticThreadProcPrivate, this) != 0 )
		{
			mTrace->Trace("**ERROR**", __FUNCTION__, "Failed pthread_create for capturing private traffic.");
			ret = false;
		}
	}
	
	if ( ret )
	{	
		for ( dword i = 0; i < mNATPublicIPs.GetElements(); i++ )
		{
			mAvailablePorts.InsertIP(mNATPublicIPs[i]);
		}
	}
	
	return ret;
}

bool CNATSim::Stop()
{
	bool ret = true;

	mTerminate = true;

#ifndef WIN32
	if ( mThreadPublic != NULL )
#else
	if ( mThreadPublic.p != NULL )
#endif
	{
		mTrace->Trace("**DEBUG**", __FUNCTION__, "Stoping capture of public traffic.");		
		pthread_join(mThreadPublic, 0);
	}
	
#ifndef WIN32
	if ( mThreadPrivate != NULL )
#else
	if ( mThreadPrivate.p != NULL )
#endif
	{
		mTrace->Trace("**DEBUG**", __FUNCTION__, "Stoping capture of private traffic.");		
		pthread_join(mThreadPrivate, 0);
	}
			
#ifndef WIN32
	mThreadPublic = NULL;
#else
	mThreadPublic.p = NULL;
#endif
	mPublicDevice = "";

#ifndef WIN32
	mThreadPrivate = NULL;
#else
	mThreadPrivate.p = NULL;
#endif
	mPrivateDevice = "";
	
	mPrivateIPs.RemoveAll();
	mPublicIPs.RemoveAll();	
	mNATPublicIPs.RemoveAll();
	mNATPrivateIPs.RemoveAll();

	mOutgoingTable.RemoveAll();
	mIncommingTable.RemoveAll();
	
	mAvailablePorts.RemoveAll();
	
	mPublicDevice = "";
	mPrivateDevice = "";
	
	return ret;
}

void CNATSim::readNatConfig()
{
	CUtilString	aux;
	
	CUtilConfigFile Config(mConfigFile, mTrace);	

	mPrivateIPs = Config.GetStringList("NATSim", "PrivateIP");
	mPublicIPs = Config.GetStringList("NATSim", "PublicIP");
	
	mNATPublicIPs = Config.GetStringList("NATSim", "NATPublicIP");
	mPublicDevice = Config.GetString("NATSim", "PublicDev", "en0");

	mNATPrivateIPs = Config.GetStringList("NATSim", "NATPrivateIP");
	mPrivateDevice = Config.GetString("NATSim", "PrivateDev", "en0");

	aux = Config.GetString("NATSim", "MappingBehavior", "AddressPortDependent");
	if ( aux == "AddressPortDependent" )
	{
		mTrace->Trace("**INFO **", __FUNCTION__, "Mapping Behavior AddressPortDependant.");
		mMappingBehavior = eMAddressPortDependent;
	} 
	else if ( aux == "AddressDependent" )
	{
		mTrace->Trace("**INFO **", __FUNCTION__, "Mapping Behavior AddressDependent.");
		mMappingBehavior = eMAddressDependent;
	}
	else if ( aux == "EndPointIndependent" )
	{
		mTrace->Trace("**INFO **", __FUNCTION__, "Mapping Behavior EndPointIndependent.");
		mMappingBehavior = eMEndPointIndependent;
	}
	else if ( aux == "NotNated" )
	{
		mTrace->Trace("**INFO **", __FUNCTION__, "Mapping Behavior NotNated.");
		mMappingBehavior = eMNotNated;
	}
	else 	
	{
		mTrace->Trace("**INFO **", __FUNCTION__, "Invalid value for parameter MappingBehavior. Assuming AddressPortDependant.");
		mMappingBehavior = eMAddressPortDependent;
	}

	aux = Config.GetString("NATSim", "FilteringBehavior", "AddressPortDependent");
	if ( aux == "AddressPortDependent" )
	{
		mTrace->Trace("**INFO **", __FUNCTION__, "Filtering Behavior AddressPortDependant.");
		mFilteringBehavior = eFAddressPortDependent;
	} 
	else if ( aux == "AddressDependent" )
	{
		mTrace->Trace("**INFO **", __FUNCTION__, "Filtering Behavior AddressDependent.");
		mFilteringBehavior = eFAddressDependent;
	}
	else if ( aux == "AddressIndependent" )
	{
		mTrace->Trace("**INFO **", __FUNCTION__, "Filtering Behavior AddressIndependent.");
		mFilteringBehavior = eFAddressIndependent;
	}
	else 	
	{
		mTrace->Trace("**INFO **", __FUNCTION__, "FilteringBehavior. Assuming AddressPortDependant.");
		mFilteringBehavior = eFAddressPortDependent;
	}

	mTimeout = Config.GetLong("NATSim", "TimeOut", 60) * 1000;
	mTrace->Trace("**INFO **", __FUNCTION__, "Timeout %d.", mTimeout);

	mIncommingRefreshTimeout = (Config.GetLong("NATSim", "IncommingRefreshTimeOut", false))==1?true:false;
	mTrace->Trace("**INFO **", __FUNCTION__, "Incomming Refresh Timeout %s.", mIncommingRefreshTimeout?"true":"false");

	mPortPreservation = (Config.GetLong("NATSim", "PortPreservation", false))==1?true:false;
	mTrace->Trace("**INFO **", __FUNCTION__, "Port Preservation %s.", mPortPreservation?"true":"false");

	mPortOverloading = (Config.GetLong("NATSim", "PortOverloading", false))==1?true:false;
	mTrace->Trace("**INFO **", __FUNCTION__, "Port Overloading %s.", mPortOverloading?"true":"false");

	aux = Config.GetString("NATSim", "IPAssignMode", "Paired");
	if ( aux == "Paired" )
	{
		mTrace->Trace("**INFO **", __FUNCTION__, "IP Assign Mode Paired.");
		mIPAssignMode = eAPaired;
	} 
	else if ( aux == "Random" )
	{
		mTrace->Trace("**INFO **", __FUNCTION__, "IP Assign Mode Random.");
		mIPAssignMode = eARandom;
	}
	else 	
	{
		mTrace->Trace("**INFO **", __FUNCTION__, "IP Assign Mode. Assuming Random.");
		mIPAssignMode = eARandom;
	}
}

void CNATSim::TreatOutgoingPacket(CaptureStruct & packet, CRawSocket * rawSocketSender)
{
	CUtilByteArray		msg;
	CUtilString			IpSrc;
	CUtilString			IpDst;
	
	dword				Idx;
	
	CUtilString			BindingIP;
	dword				BindingPort;

	CNATSimTrafficId	outgoingPk;
	CNATSimTrafficId	incommingPk;
	CNATSimTrafficId	deletePk;
	
	bool				ret = true;

	dword				i, iIni=0;

#ifndef WIN32
	struct tms ticks;
	dword Now = times(&ticks);
#else
	dword Now = GetTickCount();
#endif

	IpSrc = (CUtilString)inet_ntoa(packet.Ip_src);
	IpDst = (CUtilString)inet_ntoa(packet.Ip_dst);

	if ( mAdviseRcvFunction != NULL )
		(*mAdviseRcvFunction)(mIndex, (const char *)IpSrc, packet.Sport, (const char *)IpDst, packet.Dport, packet.Payload, mObject);

	switch ( mMappingBehavior )
	{
		case ( eMAddressPortDependent ):
			outgoingPk = CNATSimTrafficId(IpSrc, packet.Sport, IpDst, packet.Dport);
			break;
		case ( eMAddressDependent ):
			outgoingPk = CNATSimTrafficId(IpSrc, packet.Sport, IpDst, MAX_NUM_PORT+1);
			break;
		case ( eMEndPointIndependent ):
			outgoingPk = CNATSimTrafficId(IpSrc, packet.Sport, "0.0.0.0", MAX_NUM_PORT+1);
			break;
	}
	
	if ( mOutgoingTable.FindItem(outgoingPk, &Idx) )
	{
		const TrafficDataStruct & TrafficData = mOutgoingTable.GetItem(Idx);
	
		switch ( mFilteringBehavior )
		{
			case ( eFAddressIndependent ):
				incommingPk = CNATSimTrafficId("0.0.0.0", MAX_NUM_PORT+1, TrafficData.IP, TrafficData.Port);
				break;
			case ( eFAddressDependent ):
				incommingPk = CNATSimTrafficId(IpDst, MAX_NUM_PORT+1, TrafficData.IP, TrafficData.Port);
				break;
			case ( eFAddressPortDependent ):
				incommingPk = CNATSimTrafficId(IpDst, packet.Dport, TrafficData.IP, TrafficData.Port);
				break;
		}

#ifndef WIN32
		if ( Now - TrafficData.LastTraffic >= mTimeout/10 )
#else
		if ( Now - TrafficData.LastTraffic >= mTimeout )
#endif
		{
			mTrace->Trace("**INFO **", __FUNCTION__, "Timeout for outgoing traffic %s:%d - %s:%d. Elapsed time %d. Timeout %d.", (const char*)outgoingPk.GetIpS(), outgoingPk.GetPortS(), (const char*)outgoingPk.GetIpD(), outgoingPk.GetPortD(),  Now - TrafficData.LastTraffic, mTimeout);		
			ret = false;
			mIncommingTable.Clean(TrafficData.TrafficId);
			mAvailablePorts.FreePort(TrafficData.IP, TrafficData.Port);
			mOutgoingTable.Remove(Idx);
		}
		else
		{
			mOutgoingTable.UpdateLastTrafficTime(Idx);
			mTrace->Trace("**INFO **", __FUNCTION__, "Refresh traffic time for outgoing entry %s:%d - %s:%d.", (const char*)outgoingPk.GetIpS(), outgoingPk.GetPortS(), (const char*)outgoingPk.GetIpD(), outgoingPk.GetPortD());		
		}

		if ( ret )
		{
			rawSocketSender->Send(packet.Payload, (const char *)TrafficData.IP, TrafficData.Port, inet_ntoa(packet.Ip_dst), packet.Dport);

			if ( mAdviseSndFunction!= NULL )
				(*mAdviseSndFunction)(mIndex, (const char *)TrafficData.IP, TrafficData.Port, (const char *)inet_ntoa(packet.Ip_dst ), packet.Dport, packet.Payload, mObject);

			if ( mPortOverloading )
				mIncommingTable.DeleteTraffic(incommingPk);

			if ( mIncommingTable.InsertTraffic(incommingPk, IpSrc, packet.Sport, outgoingPk) )
				mTrace->Trace("**DEBUG**", __FUNCTION__, "Inserted entry into incomming table. From %s:%d To %s:%d.", (const char*)incommingPk.GetIpS(), incommingPk.GetPortS(), (const char*)incommingPk.GetIpD(), incommingPk.GetPortD());		
			else
				mTrace->Trace("**ERROR**", __FUNCTION__, "Error inserting entry into incomming table. From %s:%d To %s:%d.", (const char*)incommingPk.GetIpS(), incommingPk.GetPortS(), (const char*)incommingPk.GetIpD(), incommingPk.GetPortD());		
		}
	}
	else
	{
		BindingPort = MAX_NUM_PORT + 1;

		iIni = 0;
		if ( mIPAssignMode == eARandom )
		{
#ifndef WIN32
			iIni = times(&ticks) % mNATPublicIPs.GetElements();
#else
			iIni = GetTickCount() % mNATPublicIPs.GetElements();
#endif
			if ( iIni == mNATPublicIPs.GetElements() )
				iIni-=1;
		}

		i = iIni;
		while ( true )
		{
			if ( mPortOverloading )
			{
				if ( mAvailablePorts.IsFree(mNATPublicIPs[i], packet.Sport) )
				{
					BindingPort = packet.Sport;
					mAvailablePorts.ReserverPort(mNATPublicIPs[i], packet.Sport);
					break;
				}
			}
			else if ( mPortPreservation )
			{
				if ( mAvailablePorts.IsFree(mNATPublicIPs[i], packet.Sport) )
				{
					BindingPort = packet.Sport;
					mAvailablePorts.ReserverPort(mNATPublicIPs[i], packet.Sport);
					break;
				}
			}
			else 
			{
				BindingPort = mAvailablePorts.ReserveFirstAvailablePort(mNATPublicIPs[i]);
				if ( BindingPort <= MAX_NUM_PORT )
					break;
			}

			i++;
			if ( i == mNATPublicIPs.GetElements() )
				i = 0;
			if ( i == iIni )
			{
				if ( mPortOverloading )
				{
					switch ( mFilteringBehavior )
					{
						case ( eFAddressIndependent ):
							deletePk = CNATSimTrafficId("0.0.0.0", MAX_NUM_PORT+1, mNATPublicIPs[i], packet.Sport);
							break;
						case ( eFAddressDependent ):
							deletePk = CNATSimTrafficId(IpDst, MAX_NUM_PORT+1, mNATPublicIPs[i], packet.Sport);
							break;
						case ( eFAddressPortDependent ):
							deletePk = CNATSimTrafficId(IpDst, packet.Dport, mNATPublicIPs[i], packet.Sport);
							break;
					}
					mIncommingTable.Clean(deletePk);
					mOutgoingTable.DeleteTraffic(deletePk);

					BindingPort = packet.Sport;
					mAvailablePorts.ReserverPort(mNATPublicIPs[i], packet.Sport);
				}
				else if ( mPortPreservation )
				{
					bool bAux = false;
					for ( int j=0; j < mNATPublicIPs.GetElements(); j++ )
					{
						if ( mAvailablePorts.IsFree(mNATPublicIPs[j], packet.Sport) )
						{
							BindingPort = packet.Sport;
							mAvailablePorts.ReserverPort(mNATPublicIPs[j], packet.Sport);
							i = j;
							bAux = true;
							break;
						}
					}
					if ( ! bAux )
					{
						for ( int j=0; j < mNATPublicIPs.GetElements(); j++ )
						{
							BindingPort = mAvailablePorts.ReserveFirstAvailablePort(mNATPublicIPs[j]);
							if ( BindingPort <= MAX_NUM_PORT )
							{
								i = j;
								break;
							}
						}
					}
				}
				break;
			}
		}

		if ( BindingPort >= 0 && BindingPort <= MAX_NUM_PORT)
		{
			BindingIP = mNATPublicIPs[i];
			
			switch ( mFilteringBehavior )
			{
				case ( eFAddressIndependent ):
					incommingPk = CNATSimTrafficId("0.0.0.0", MAX_NUM_PORT+1, BindingIP, BindingPort);
					break;
				case ( eFAddressDependent ):
					incommingPk = CNATSimTrafficId(IpDst, MAX_NUM_PORT+1, BindingIP, BindingPort);
					break;
				case ( eFAddressPortDependent ):
					incommingPk = CNATSimTrafficId(IpDst, packet.Dport, BindingIP, BindingPort);
					break;
			}
			
			if ( mOutgoingTable.InsertTraffic(outgoingPk, BindingIP, BindingPort, incommingPk ) )
				mTrace->Trace("**DEBUG**", __FUNCTION__, "Inserted entry into outgoing table. From %s:%d To %s:%d.", (const char*)outgoingPk.GetIpS(), outgoingPk.GetPortS(), (const char*)outgoingPk.GetIpD(), outgoingPk.GetPortD());		
			else
				mTrace->Trace("**ERROR**", __FUNCTION__, "Error inserting entry into outgoing table. From %s:%d To %s:%d.", (const char*)outgoingPk.GetIpS(), outgoingPk.GetPortS(), (const char*)outgoingPk.GetIpD(), outgoingPk.GetPortD());		

			if ( mPortOverloading )
				mIncommingTable.DeleteTraffic(incommingPk);

			if ( mIncommingTable.InsertTraffic(incommingPk, IpSrc, packet.Sport, outgoingPk) )
				mTrace->Trace("**DEBUG**", __FUNCTION__, "Inserted entry into incomming table. From %s:%d To %s:%d.", (const char*)incommingPk.GetIpS(), incommingPk.GetPortS(), (const char*)incommingPk.GetIpD(), incommingPk.GetPortD());		
			else
				mTrace->Trace("**ERROR**", __FUNCTION__, "Error inserting entry into incomming table. From %s:%d To %s:%d.", (const char*)incommingPk.GetIpS(), incommingPk.GetPortS(), (const char*)incommingPk.GetIpD(), incommingPk.GetPortD());		
			
			rawSocketSender->Send(packet.Payload, (const char *)BindingIP, BindingPort, inet_ntoa(packet.Ip_dst), packet.Dport);

			if ( mAdviseSndFunction!= NULL )
				(*mAdviseSndFunction)(mIndex, (const char *)BindingIP, BindingPort, (const char *)inet_ntoa(packet.Ip_dst ), packet.Dport, packet.Payload, mObject);
		}			
	}
}

void CNATSim::TreatIncommingPacket(CaptureStruct & packet, CRawSocket * rawSocketSender)
{
	CUtilByteArray		msg;
	CUtilString			IpSrc;
	CUtilString			IpDst;
	
	dword				Idx, Idx2;

	CNATSimTrafficId	outgoingPk;
	CNATSimTrafficId	incommingPk;

	bool				ret = true;

#ifndef WIN32
	struct tms ticks;
	dword Now = times(&ticks);
#else
	dword Now = GetTickCount();
#endif

	IpSrc = (CUtilString)inet_ntoa(packet.Ip_src);
	IpDst = (CUtilString)inet_ntoa(packet.Ip_dst);
	
	if ( mAdviseRcvFunction != NULL )
		(*mAdviseRcvFunction)(mIndex, (const char *)IpSrc, packet.Sport, (const char *)IpDst, packet.Dport, packet.Payload, mObject);

	switch ( mFilteringBehavior )
	{
		case ( eFAddressIndependent ):
			incommingPk = CNATSimTrafficId("0.0.0.0", MAX_NUM_PORT+1, IpDst, packet.Dport);
			break;
		case ( eFAddressDependent ):
			incommingPk = CNATSimTrafficId(IpSrc, MAX_NUM_PORT+1, IpDst, packet.Dport);
			break;
		case ( eFAddressPortDependent ):
			incommingPk = CNATSimTrafficId(IpSrc, packet.Sport, IpDst, packet.Dport);
			break;
	}

	if ( mIncommingTable.FindItem(incommingPk, &Idx) )
	{
		const TrafficDataStruct & TrafficData = mIncommingTable.GetItem(Idx);

		switch ( mMappingBehavior )
		{
			case ( eMAddressPortDependent ):
				outgoingPk = CNATSimTrafficId(TrafficData.IP, TrafficData.Port, IpSrc, packet.Sport );
				break;
			case ( eMAddressDependent ):
				outgoingPk = CNATSimTrafficId(TrafficData.IP, TrafficData.Port, IpSrc, MAX_NUM_PORT+1 );
				break;
			case ( eMEndPointIndependent ):
				outgoingPk = CNATSimTrafficId(TrafficData.IP, TrafficData.Port, "0.0.0.0", MAX_NUM_PORT+1);
				break;
		}

		if ( mOutgoingTable.FindItem(outgoingPk, &Idx2) )
		{
			const TrafficDataStruct & TrafficData2 = mOutgoingTable.GetItem(Idx2);
			
#ifndef WIN32
			if ( Now - TrafficData2.LastTraffic >= mTimeout/10 )
#else
			if ( Now - TrafficData2.LastTraffic >= mTimeout )
#endif
			{
				mTrace->Trace("**INFO **", __FUNCTION__, "Timeout for incomming traffic %s:%d - %s:%d. Elapsed time %d. Timeout %d.", inet_ntoa(packet.Ip_src), packet.Sport, (const char *)TrafficData.IP, TrafficData.Port, Now - TrafficData2.LastTraffic, mTimeout);		
				ret = false;
				mIncommingTable.Clean(TrafficData2.TrafficId);
				mAvailablePorts.FreePort(TrafficData2.IP, TrafficData2.Port);
				mOutgoingTable.Remove(Idx2);
			}
			else
			{
				if ( mIncommingRefreshTimeout )
				{
					mOutgoingTable.UpdateLastTrafficTime(Idx2);
					mTrace->Trace("**INFO **", __FUNCTION__, "Refresh traffic time for outgoing entry %s:%d - %s:%d.", (const char*)outgoingPk.GetIpS(), outgoingPk.GetPortS(), (const char*)outgoingPk.GetIpD(), outgoingPk.GetPortD());
				}
			}
		}
		else
		{
			mTrace->Trace("**INFO **", __FUNCTION__, "Outgoing entry not found %s:%d - %s:%d.", (const char *)outgoingPk.GetIpS(), outgoingPk.GetPortS(), (const char *)outgoingPk.GetIpD(), outgoingPk.GetPortD());		
		}

		if ( ret )
		{
			rawSocketSender->Send(packet.Payload, inet_ntoa(packet.Ip_src), packet.Sport, TrafficData.IP, TrafficData.Port);
		
			if ( mAdviseSndFunction!= NULL )
				(*mAdviseSndFunction)(mIndex, (const char *)inet_ntoa(packet.Ip_src), packet.Sport, (const char *)TrafficData.IP, TrafficData.Port, packet.Payload, mObject);
		}
	}
	else
	{
		mTrace->Trace("**DEBUG**", __FUNCTION__, "Captured traffic from public address not bindng. From %s:%d To %s:%d.", (const char*)IpSrc, packet.Sport, (const char*)IpDst, packet.Dport);
	}
}

