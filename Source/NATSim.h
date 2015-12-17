/*
 *  NATSim.h
 *  NATDetector
 *
 *  Created by System Administrator on 17/02/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

class CNATSim
{
public:
	enum eMappingBehaviour 
						{
							eMNotNated,
							eMEndPointIndependent,
							eMAddressDependent,
							eMAddressPortDependent
						};

	enum eFilteringBehaviour 
						{
							eFAddressIndependent,
							eFAddressDependent,
							eFAddressPortDependent
						};

	enum eIPAssignMode
						{
							eAPaired,
							eARandom
						};

public:
	CNATSim(CUtilString ConfigFile, CUtilTrace* Trace);
	CNATSim(CUtilString ConfigFile, CUtilTrace* Trace, int index, AdviseSndFunction adviseSndF, AdviseRcvFunction adviseRcvF, void *object );
	~CNATSim();
		
	bool Start();
	bool Stop();
	
private:

	eMappingBehaviour		mMappingBehavior;
	eFilteringBehaviour		mFilteringBehavior;
	
	unsigned int			mTimeout;

	bool					mIncommingRefreshTimeout;
	
	bool					mPortPreservation;
	bool					mPortOverloading;

	eIPAssignMode			mIPAssignMode;

	CUtilTrace *			mTrace;
	CUtilString				mConfigFile; 
		
	CNATSimTrafficTable		mOutgoingTable;
	CNATSimTrafficTable		mIncommingTable;
	
	CArray<CUtilString>		mPrivateIPs;		// Ips of host behind the NAT. Public Zone
	CArray<CUtilString>		mPublicIPs;			// Ips of host beyond the NAT. Private Zone
	
	CArray<CUtilString>		mNATPublicIPs;		// Public interfaces for the NAT
	CArray<CUtilString>		mNATPrivateIPs;		// Public interfaces for the NAT

	CUtilString				mPublicDevice;
	CUtilString				mPrivateDevice;

	pthread_t				mThreadPublic;		// Thread to treat incoming traffic
	static void*			staticThreadProcPublic(void* arg);
	void*					realThreadProcPublic(void* arg);

	pthread_t				mThreadPrivate;		// Thread to treat outgoing traffic
	static void*			staticThreadProcPrivate(void* arg);
	void*					realThreadProcPrivate(void* arg);

	CNATSimPortsArray		mAvailablePorts;	// Array of ports (one for each NAT Public IP) of 65536 bits
	
	bool					mTerminate;
	
	void					readNatConfig();
	
	void					TreatOutgoingPacket(CaptureStruct & packet, CRawSocket * rawSocketSender);
	void					TreatIncommingPacket(CaptureStruct & packet, CRawSocket * rawSocketSender);

	int						mIndex;
	AdviseSndFunction		mAdviseSndFunction;
	AdviseRcvFunction		mAdviseRcvFunction;
	void*					mObject;

};

